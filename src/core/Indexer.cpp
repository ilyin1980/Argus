#include "core/Indexer.h"

#include "core/ContentHash.h"
#include "core/Database.h"
#include "core/DescriptorStore.h"
#include "core/FeatureExtractor.h"
#include "core/GitRepo.h"
#include "core/ImageDecoder.h"
#include "core/Parallel.h"
#include "core/Paths.h"
#include "core/PerceptualHash.h"
#include "core/Scanner.h"

#include <QBuffer>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QImageReader>
#include <QSet>

#include <algorithm>
#include <memory>
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

void Indexer::indexBranches(Database &db,
                            const QString &root,
                            const IndexOptions &options,
                            IndexStats &stats)
{
    const git::RepoInfo repo = git::inspect(root);
    if (!repo.isRepo) {
        emit message(repo.error.isEmpty()
                         ? QStringLiteral("%1 is not inside a git repository").arg(root)
                         : QStringLiteral("git unavailable: %1").arg(repo.error));
        return;
    }

    // One process answers every blob request. Starting one per file would cost
    // more than reading the file.
    git::BlobReader reader(repo.topLevel);
    if (!reader.isReady()) {
        emit message(QStringLiteral("cannot read git objects: %1").arg(reader.error()));
        return;
    }

    DecodeRequest decodeRequest;
    decodeRequest.wantThumbnail   = options.thumbnails;
    decodeRequest.thumbSize       = options.thumbSize;
    decodeRequest.thumbQuality    = options.thumbQuality;
    decodeRequest.alphaBackground = options.alphaBackground;

    const int threads = resolveThreadCount(options.jobs);
    const int chunkSize = std::max(32, threads * 8);

    for (const QString &branch : options.branches) {
        if (m_cancel.load(std::memory_order_relaxed)) {
            stats.cancelled = true;
            return;
        }
        if (branch.isEmpty())
            continue; // the working tree, already done by the main pass

        QString error;
        const QString tip = git::resolve(repo.topLevel, branch, &error);
        if (tip.isEmpty()) {
            emit message(QStringLiteral("skipping %1: %2").arg(branch, error));
            continue;
        }

        const QString tipKey = QStringLiteral("ref_tip:") + branch;
        if (!options.force && db.metaValue(tipKey) == tip) {
            ++stats.branchesSkipped;
            continue;
        }

        emit progress(0, 0, QStringLiteral("branch"));
        emit message(QStringLiteral("Reading branch %1").arg(branch));

        const QList<git::BlobEntry> entries =
            git::listBlobs(repo.topLevel, branch, repo.prefix, options.extensions, &error);
        if (entries.isEmpty() && !error.isEmpty()) {
            emit message(QStringLiteral("cannot list %1: %2").arg(branch, error));
            continue;
        }

        // Blob ids are content addresses: an unchanged id means unchanged
        // bytes, which is a stronger statement than size and mtime can make
        // about a file on disk.
        const QHash<QString, QString> known =
            options.force ? QHash<QString, QString>() : db.loadBlobs(branch);
        QSet<QString> present;
        QList<git::BlobEntry> todo;
        present.reserve(entries.size());
        todo.reserve(entries.size());

        for (const git::BlobEntry &entry : entries) {
            present.insert(entry.rel);
            const auto it = known.constFind(entry.rel);
            if (it != known.constEnd() && *it == entry.oid) {
                ++stats.skipped;
                continue;
            }
            todo.append(entry);
        }

        QList<QByteArray> payloads;
        QList<IndexRecord> batch;

        for (int start = 0; start < todo.size(); start += chunkSize) {
            if (m_cancel.load(std::memory_order_relaxed)) {
                stats.cancelled = true;
                return;
            }

            const int end = std::min<int>(start + chunkSize, static_cast<int>(todo.size()));
            const int count = end - start;

            // Read serially - the reader owns one pipe with a strict
            // request/response order - then decode in parallel, which is where
            // the time actually goes.
            payloads.resize(count);
            for (int i = 0; i < count; ++i)
                payloads[i] = reader.read(todo.at(start + i).oid);

            batch.resize(count);
            IndexRecord *records = batch.data();
            const QByteArray *bytes = payloads.constData();

            parallelFor(count, threads, &m_cancel, [&](int i) {
                const git::BlobEntry &entry = todo.at(start + i);
                IndexRecord &record = records[i];

                record = IndexRecord{};
                record.rel  = entry.rel;
                record.ref  = branch;
                record.blob = entry.oid;
                // The size of what was hashed, not what the tree says. Under
                // Git LFS the tree records the pointer's 128 bytes, and a row
                // claiming that would both mislead the reader and stop the
                // duplicate finder from pairing this with the same file on
                // disk, which it groups by content hash and size.
                record.size = bytes[i].isEmpty() ? entry.size
                                                 : static_cast<qint64>(bytes[i].size());
                // A blob has no modification time. Storing zero says so rather
                // than inventing one that later passes would compare against.
                record.mtime = 0;
                record.contentHash = dataContentHash(bytes[i]);

                const DecodedImage decoded = decodeFromData(bytes[i], decodeRequest);
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

            QString writeError;
            if (!db.beginTransaction(&writeError)) {
                emit message(QStringLiteral("branch write failed: %1").arg(writeError));
                return;
            }
            for (const IndexRecord &record : std::as_const(batch)) {
                if (record.rel.isEmpty())
                    continue;
                if (!db.upsert(record, &writeError)) {
                    emit message(QStringLiteral("write failed for %1@%2: %3")
                                     .arg(record.rel, branch, writeError));
                    continue;
                }
                if (record.status == FileStatus::Ok)
                    ++stats.indexed;
                else
                    ++stats.failed;
            }
            if (!db.commit(&writeError)) {
                emit message(QStringLiteral("branch commit failed: %1").arg(writeError));
                return;
            }

            emit progress(end, static_cast<int>(todo.size()), QStringLiteral("branch"));
        }

        const int removed = db.pruneMissingInRef(branch, present, &error);
        if (removed > 0)
            stats.pruned += removed;

        // Written last on purpose: an interrupted run leaves the old tip in
        // place, so the next run redoes this branch instead of believing a
        // half-written one is current.
        db.setMetaValue(tipKey, tip);
        ++stats.branchesIndexed;
        stats.scanned += static_cast<int>(entries.size());
    }

    if (reader.missingLfsObjects() > 0) {
        emit message(QStringLiteral(
                         "%1 files are Git LFS objects that are not in the local store; "
                         "run 'git lfs fetch --all' to index them")
                         .arg(reader.missingLfsObjects()));
    }

    // Refs the caller no longer wants are dropped, but only when the caller
    // said what the complete set is. Otherwise a plain re-index of the working
    // tree would silently throw away every indexed branch.
    if (options.syncBranches) {
        const QStringList wanted = options.branches;
        for (const QString &ref : db.refs()) {
            if (ref.isEmpty() || wanted.contains(ref))
                continue;
            const int removed = db.removeRef(ref);
            if (removed > 0) {
                emit message(QStringLiteral("dropped %1 rows of branch %2")
                                 .arg(removed)
                                 .arg(ref));
            }
            db.setMetaValue(QStringLiteral("ref_tip:") + ref, QString());
        }
    }
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

    // Rows that came out of a branch have no file to open; their bytes come
    // from the object store instead. The reader is built only when the index
    // actually holds such rows, so a plain folder never invokes git at all.
    std::unique_ptr<git::BlobReader> blobs;
    const bool needsGit = std::any_of(pending.cbegin(), pending.cend(),
                                      [](const PendingFeature &p) { return !p.ref.isEmpty(); });
    if (needsGit) {
        const git::RepoInfo repo = git::inspect(root);
        if (repo.isRepo) {
            blobs = std::make_unique<git::BlobReader>(repo.topLevel);
            if (!blobs->isReady()) {
                emit message(QStringLiteral("cannot read git objects: %1").arg(blobs->error()));
                blobs.reset();
            }
        }
    }

    QList<QImage> decoded;
    QList<QByteArray> payloads;
    for (int start = 0; start < pending.size(); start += batchSize) {
        if (m_cancel.load(std::memory_order_relaxed)) {
            stats.cancelled = true;
            break;
        }

        const int end   = std::min<int>(start + batchSize, static_cast<int>(pending.size()));
        const int count = end - start;

        // Serial, because one pipe answers in order; the decode below is what
        // costs time and that stays parallel.
        payloads.resize(count);
        for (int i = 0; i < count; ++i) {
            const PendingFeature &item = pending.at(start + i);
            payloads[i] = (item.ref.isEmpty() || !blobs) ? QByteArray()
                                                         : blobs->read(item.blob);
        }

        decoded.resize(count);
        QImage *images = decoded.data();
        const QByteArray *bytes = payloads.constData();

        parallelFor(count, threads, &m_cancel, [&](int i) {
            const PendingFeature &item = pending.at(start + i);

            QBuffer buffer;
            QImageReader reader;
            if (item.ref.isEmpty()) {
                reader.setFileName(absolutePathFor(root, item.rel));
            } else {
                if (bytes[i].isEmpty())
                    return; // unreadable blob; counted as a failure below
                buffer.setData(bytes[i]);
                buffer.open(QIODevice::ReadOnly);
                reader.setDevice(&buffer);
            }
            reader.setAutoTransform(true);
            // Read at full resolution and let the extractor do the downscale.
            //
            // Asking the reader for a reduced decode looks like the same
            // picture and is not: the plugin resamples its own way, while
            // FeatureExtractor::extract() uses QImage::scaled with a smooth
            // transform. Two different 1024-pixel images produce two different
            // sets of DISK keypoints, so descriptors written here did not
            // describe what a query later compares against.
            //
            // A whole-image query survived that - there are hundreds of
            // keypoints and enough of them agree - but a crop of a large
            // texture did not: `match`, which reads at full size, verified a
            // quarter-crop against its parent with 31 inliers at 78%, while
            // `find` over the same index returned nothing for the same pair
            // even with every image in the shortlist.
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

    // ---- 4. Git branches ----------------------------------------------------
    if ((!options.branches.isEmpty() || options.syncBranches) && !stats.cancelled)
        indexBranches(db, root, options, stats);

    // ---- 5. Neural local features -------------------------------------------
    if (options.extractFeatures && !stats.cancelled)
        extractFeaturePass(db, root, dbPath, options, stats);

    // ---- 6. Drop rows whose file is gone ------------------------------------
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
