#include "core/Indexer.h"

#include "core/ContentHash.h"
#include "core/Database.h"
#include "core/DescriptorStore.h"
#include "core/FeatureExtractor.h"
#include "core/ImageDecoder.h"
#include "core/Parallel.h"
#include "core/Paths.h"
#include "core/PerceptualHash.h"
#include "core/Scanner.h"

#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QImageReader>
#include <QSet>

#include <algorithm>
#include <utility>

namespace iw {

Indexer::Indexer(QObject *parent)
    : QObject(parent)
{
}

void Indexer::cancel()
{
    m_cancel.store(true, std::memory_order_relaxed);
}

void Indexer::reset()
{
    m_cancel.store(false, std::memory_order_relaxed);
}

void Indexer::extractFeaturePass(Database &db,
                                 const QString &root,
                                 const QString &dbPath,
                                 const IndexOptions &options,
                                 IndexStats &stats)
{
    QString error;
    auto extractor = FeatureExtractor::create(options.featureModelPath,
                                              options.featureUseGpu, &error);
    if (!extractor) {
        emit message(QStringLiteral("feature extraction skipped: %1").arg(error));
        return;
    }
    stats.featureProvider = extractor->provider();

    const QString featureDir = QFileInfo(dbPath).absolutePath() + QLatin1String("/features");
    DescriptorStore store;
    if (!store.open(featureDir, &error)) {
        emit message(QStringLiteral("feature store unavailable: %1").arg(error));
        return;
    }

    const QString modelId = extractor->modelId();
    if (options.force) {
        // A forced run rewrites everything, which is also the only chance to
        // reclaim the space left behind by earlier appends.
        db.clearFeatures();
        store.clear();
    }

    const QList<PendingFeature> pending = db.filesWithoutFeatures(modelId);
    if (pending.isEmpty()) {
        emit message(QStringLiteral("local features already up to date"));
        stats.featureBytes = store.totalBytes();
        return;
    }

    emit message(QStringLiteral("extracting local features from %1 images on %2")
                     .arg(pending.size())
                     .arg(extractor->provider()));

    ExtractorOptions extractorOptions;
    extractorOptions.maxKeypoints = options.featureMaxKeypoints;
    extractorOptions.maxSide      = options.featureMaxSide;

    // Decoding is parallel, inference is not: one GPU, one queue. Decoding a
    // small batch ahead keeps the device fed without holding many full-size
    // images in memory at once.
    const int threads   = resolveThreadCount(options.jobs);
    const int batchSize = std::max(4, std::min(16, threads));

    QList<QImage> decoded;
    for (int start = 0; start < pending.size(); start += batchSize) {
        if (m_cancel.load(std::memory_order_relaxed)) {
            stats.cancelled = true;
            break;
        }

        const int end   = std::min<int>(start + batchSize, static_cast<int>(pending.size()));
        const int count = end - start;

        decoded.resize(count);
        QImage *images = decoded.data();

        parallelFor(count, threads, &m_cancel, [&](int i) {
            const QString absolute = absolutePathFor(root, pending.at(start + i).rel);
            QImageReader reader(absolute);
            reader.setAutoTransform(true);
            const QSize full = reader.size();
            if (full.isValid() && options.featureMaxSide > 0
                && std::max(full.width(), full.height()) > options.featureMaxSide) {
                QSize target = full;
                target.scale(options.featureMaxSide, options.featureMaxSide, Qt::KeepAspectRatio);
                if (!target.isEmpty())
                    reader.setScaledSize(target);
            }
            images[i] = reader.read();
        });

        if (m_cancel.load(std::memory_order_relaxed)) {
            stats.cancelled = true;
            break;
        }

        QString writeError;
        if (!db.beginTransaction(&writeError)) {
            emit message(QStringLiteral("feature write failed: %1").arg(writeError));
            break;
        }

        for (int i = 0; i < count; ++i) {
            const PendingFeature &item = pending.at(start + i);
            if (images[i].isNull()) {
                ++stats.featureFailed;
                continue;
            }

            QString extractError;
            const FeatureSet features =
                extractor->extract(images[i], extractorOptions, &extractError);
            if (features.isEmpty() && !extractError.isEmpty()) {
                ++stats.featureFailed;
                continue;
            }

            FeatureLocation location;
            if (!store.append(features, &location, &extractError)) {
                emit message(QStringLiteral("descriptor write failed for %1: %2")
                                 .arg(item.rel, extractError));
                ++stats.featureFailed;
                continue;
            }

            FeatureRecord record;
            record.fileId      = item.fileId;
            record.model       = modelId;
            record.count       = location.count;
            record.dim         = location.dim;
            record.imageWidth  = features.imageWidth;
            record.imageHeight = features.imageHeight;
            record.descOffset  = location.descOffset;
            record.kptsOffset  = location.kptsOffset;

            if (!db.upsertFeatures(record, &extractError)) {
                emit message(QStringLiteral("feature index write failed for %1: %2")
                                 .arg(item.rel, extractError));
                ++stats.featureFailed;
                continue;
            }

            ++stats.featured;
            stats.keypointsTotal += features.count();
        }

        db.commit(&writeError);
        emit progress(end, static_cast<int>(pending.size()), QStringLiteral("features"));
    }

    stats.featureBytes = store.totalBytes();
}

IndexStats Indexer::run(const IndexOptions &options, QString *error)
{
    QElapsedTimer timer;
    timer.start();

    IndexStats stats;

    const QString root = normalizeRoot(options.root);
    if (root.isEmpty() || !QFileInfo(root).isDir()) {
        if (error)
            *error = QStringLiteral("not a directory: %1").arg(options.root);
        return stats;
    }

    QString dbPath = options.dbPath;
    if (dbPath.isEmpty()) {
        dbPath = defaultDatabasePath(root, error);
        if (dbPath.isEmpty())
            return stats;
    }

    Database db;
    if (!db.open(dbPath, error))
        return stats;

    db.setMetaValue(QStringLiteral("root"), root);

    // ---- 1. Walk -----------------------------------------------------------
    emit message(QStringLiteral("Scanning %1").arg(root));
    emit progress(0, 0, QStringLiteral("scan"));

    ScanOptions scanOptions;
    scanOptions.extensions = options.extensions;
    scanOptions.maxBytes   = options.maxFileBytes;

    const QList<ScannedFile> found =
        scanDirectory(root, scanOptions, &m_cancel,
                      [this](int n) { emit progress(n, 0, QStringLiteral("scan")); });

    stats.scanned = static_cast<int>(found.size());
    if (m_cancel.load(std::memory_order_relaxed)) {
        stats.cancelled = true;
        stats.elapsedMs = timer.elapsed();
        return stats;
    }
    emit message(QStringLiteral("Found %1 candidate files").arg(stats.scanned));

    // ---- 2. Diff against what we already know ------------------------------
    const auto known = db.loadSignatures();

    QList<ScannedFile> todo;
    QSet<QString> present;
    present.reserve(found.size());
    todo.reserve(found.size());

    for (const ScannedFile &file : found) {
        present.insert(file.rel);
        if (!options.force) {
            const auto it = known.constFind(file.rel);
            if (it != known.constEnd() && it->first == file.size && it->second == file.mtime) {
                ++stats.skipped;
                continue;
            }
        }
        todo.append(file);
    }

    emit message(QStringLiteral("%1 new or changed, %2 unchanged")
                     .arg(todo.size())
                     .arg(stats.skipped));

    // ---- 3. Decode and hash in parallel, write in batches -------------------
    const int threads = resolveThreadCount(options.jobs);
    const int chunkSize = std::max(64, threads * 16);

    DecodeRequest decodeRequest;
    decodeRequest.wantThumbnail   = options.thumbnails;
    decodeRequest.thumbSize       = options.thumbSize;
    decodeRequest.thumbQuality    = options.thumbQuality;
    decodeRequest.alphaBackground = options.alphaBackground;

    std::atomic<qint64> bytesRead{0};
    QList<IndexRecord> batch;

    for (int start = 0; start < todo.size(); start += chunkSize) {
        if (m_cancel.load(std::memory_order_relaxed)) {
            stats.cancelled = true;
            break;
        }

        const int end = std::min<int>(start + chunkSize, static_cast<int>(todo.size()));
        const int count = end - start;

        batch.resize(count);
        // Take the raw pointer once: workers then write into disjoint slots
        // without ever touching QList's shared-data machinery.
        IndexRecord *records = batch.data();

        parallelFor(count, threads, &m_cancel, [&](int i) {
            const ScannedFile &file = todo.at(start + i);
            IndexRecord &record = records[i];

            record = IndexRecord{}; // entries are reused across chunks
            record.rel   = file.rel;
            record.size  = file.size;
            record.mtime = file.mtime;

            const QString absolute = absolutePathFor(root, file.rel);

            qint64 read = 0;
            record.contentHash = fileContentHash(absolute, &read);
            bytesRead.fetch_add(read, std::memory_order_relaxed);

            const DecodedImage decoded = decodeForIndex(absolute, decodeRequest);
            if (!decoded.ok) {
                record.status = FileStatus::Unreadable;
                record.error  = decoded.error;
                return;
            }

            record.status = FileStatus::Ok;
            record.width  = decoded.width;
            record.height = decoded.height;
            record.phash  = dctPerceptualHash(decoded.gray32);
            record.dhash  = differenceHash(decoded.gray9x8);
            record.thumb  = decoded.thumbnail;
        });

        if (m_cancel.load(std::memory_order_relaxed)) {
            stats.cancelled = true;
            break;
        }

        QString writeError;
        if (!db.beginTransaction(&writeError)) {
            if (error)
                *error = writeError;
            break;
        }
        for (const IndexRecord &record : std::as_const(batch)) {
            if (record.rel.isEmpty())
                continue;
            if (!db.upsert(record, &writeError)) {
                emit message(QStringLiteral("write failed for %1: %2").arg(record.rel, writeError));
                continue;
            }
            if (record.status == FileStatus::Ok)
                ++stats.indexed;
            else
                ++stats.failed;
        }
        if (!db.commit(&writeError)) {
            if (error)
                *error = writeError;
            break;
        }

        emit progress(end, static_cast<int>(todo.size()), QStringLiteral("index"));
    }

    stats.bytesRead = bytesRead.load(std::memory_order_relaxed);

    // ---- 4. Neural local features -------------------------------------------
    if (options.extractFeatures && !stats.cancelled)
        extractFeaturePass(db, root, dbPath, options, stats);

    // ---- 5. Drop rows whose file is gone ------------------------------------
    if (options.prune && !stats.cancelled) {
        emit progress(0, 0, QStringLiteral("prune"));
        QString pruneError;
        const int removed = db.pruneMissing(present, &pruneError);
        if (removed < 0)
            emit message(QStringLiteral("prune failed: %1").arg(pruneError));
        else
            stats.pruned = removed;
    }

    db.setMetaValue(QStringLiteral("last_indexed"),
                    QString::number(QDateTime::currentSecsSinceEpoch()));

    stats.elapsedMs = timer.elapsed();
    return stats;
}

} // namespace iw
