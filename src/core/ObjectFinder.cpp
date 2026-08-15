#include "core/ObjectFinder.h"

#include "core/BowIndex.h"
#include "core/Database.h"
#include "core/DescriptorStore.h"
#include "core/FeatureExtractor.h"
#include "core/Onnx.h"
#include "core/Parallel.h"
#include "core/Paths.h"
#include "core/TemplateMatcher.h"

#include <QImageReader>
#include "core/Vocabulary.h"

#include <QFileInfo>
#include <QHash>

#include <algorithm>

namespace iw {

struct ObjectFinder::Impl {
    Database        database;
    DescriptorStore store;

    std::unique_ptr<Vocabulary>       vocabulary;
    std::unique_ptr<BowIndex>         bow;
    std::unique_ptr<FeatureExtractor> extractor;

    /// One session per matcher thread; grown on demand and kept for later runs.
    std::vector<std::unique_ptr<FeatureMatcher>> matchers;
    QString matcherPath;
    bool    matcherUseGpu = true;

    /**
     * @brief Ensure at least @p count matcher sessions exist.
     * @return How many are actually available, which may be fewer than asked
     *         for if a session failed to build.
     */
    int ensureMatchers(int count, QString *error)
    {
        while (static_cast<int>(matchers.size()) < count) {
            auto extra = FeatureMatcher::create(matcherPath, matcherUseGpu, error);
            if (!extra)
                break;
            matchers.push_back(std::move(extra));
        }
        return static_cast<int>(matchers.size());
    }
};

ObjectFinder::ObjectFinder()
    : d(std::make_unique<Impl>())
{
}

ObjectFinder::~ObjectFinder() = default;

int ObjectFinder::documentCount() const
{
    return d->bow ? d->bow->stats().documents : 0;
}

QString ObjectFinder::provider() const
{
    return d->extractor ? d->extractor->provider() : QStringLiteral("none");
}

std::unique_ptr<ObjectFinder> ObjectFinder::create(const QString &databasePath,
                                                   const FindOptions &options,
                                                   QString *error)
{
    std::unique_ptr<ObjectFinder> self(new ObjectFinder);

    if (!self->d->database.open(databasePath, error))
        return nullptr;

    const QString featureDir = QFileInfo(databasePath).absolutePath()
                               + QStringLiteral("/features");
    if (!self->d->store.open(featureDir, error))
        return nullptr;

    self->d->vocabulary = Vocabulary::load(featureDir + QStringLiteral("/vocab.bin"), error);
    if (!self->d->vocabulary) {
        if (error) {
            *error = QStringLiteral("%1\nhint: run 'imageworker vocab' to train one")
                         .arg(*error);
        }
        return nullptr;
    }

    self->d->bow = BowIndex::load(featureDir + QStringLiteral("/bow.bin"), error);
    if (!self->d->bow)
        return nullptr;

    const QString models = defaultModelsDir();
    const QString extractorPath = options.extractorPath.isEmpty()
                                      ? models + QStringLiteral("/disk.onnx")
                                      : options.extractorPath;
    const QString matcherPath = options.matcherPath.isEmpty()
                                    ? preferredMatcherModel(models)
                                    : options.matcherPath;

    self->d->extractor = FeatureExtractor::create(extractorPath, options.useGpu, error);
    if (!self->d->extractor)
        return nullptr;

    self->d->matcherPath   = matcherPath;
    self->d->matcherUseGpu = options.matcherUseGpu;
    if (self->d->ensureMatchers(1, error) < 1)
        return nullptr;

    return self;
}

QList<FindResult> ObjectFinder::find(const QImage &query,
                                     const FindOptions &options,
                                     const std::atomic_bool *cancel,
                                     const std::function<void(int, int)> &progress,
                                     QString *error)
{
    QList<FindResult> out;
    if (query.isNull()) {
        if (error)
            *error = QStringLiteral("empty query image");
        return out;
    }

    // ---- 1. Features of the query ------------------------------------------
    ExtractorOptions extractorOptions;
    extractorOptions.maxKeypoints = options.maxKeypoints;
    extractorOptions.maxSide      = options.maxSide;
    // A screenshot has no meaningful alpha: masking would throw away the frame.
    extractorOptions.useAlphaMask = false;

    const FeatureSet queryFeatures = d->extractor->extract(query, extractorOptions, error);
    if (queryFeatures.isEmpty()) {
        if (error && error->isEmpty())
            *error = QStringLiteral("no features found in the query image");
        return out;
    }

    // ---- 2. Shortlist -------------------------------------------------------
    const QList<quint32> words =
        d->vocabulary->assign(queryFeatures.descriptors, queryFeatures.count());
    if (words.isEmpty()) {
        if (error)
            *error = QStringLiteral("could not quantise the query descriptors");
        return out;
    }

    const QList<BowHit> shortlist = d->bow->query(words, std::max(1, options.shortlist));
    if (shortlist.isEmpty())
        return out;

    QHash<qint64, float> bowScores;
    bowScores.reserve(shortlist.size());
    for (const BowHit &hit : shortlist)
        bowScores.insert(hit.fileId, hit.score);

    // ---- 3. Verify the shortlist -------------------------------------------
    struct Verified {
        qint64         fileId;
        int            matches;
        GeometryResult geometry;
    };
    QList<Verified> verified;

    // Phase A, serial: pull every candidate's features into memory. Neither the
    // descriptor store (one QFile it seeks around) nor the database (one
    // QSqlQuery) is safe to touch from several threads, and at roughly 40 KB per
    // candidate the whole shortlist costs a few megabytes.
    struct Candidate {
        qint64     fileId = 0;
        FeatureSet features;
    };
    QList<Candidate> candidates;
    candidates.reserve(shortlist.size());

    for (const BowHit &hit : shortlist) {
        if (cancel && cancel->load(std::memory_order_relaxed))
            break;

        FeatureRecord record;
        if (!d->database.featuresFor(hit.fileId, options.model, &record))
            continue;

        FeatureLocation location;
        location.descOffset = record.descOffset;
        location.kptsOffset = record.kptsOffset;
        location.count      = record.count;
        location.dim        = record.dim;

        FeatureSet assetFeatures = d->store.load(location, record.imageWidth, record.imageHeight);
        if (assetFeatures.isEmpty())
            continue;

        candidates.append({ hit.fileId, std::move(assetFeatures) });
    }

    // Phase B, parallel: match and verify. Each worker owns a session, because
    // the DirectML provider drives one command queue per session.
    // Four sessions, measured as the point where this GPU stops gaining: over
    // 200 candidates 1/2/3/4/6 threads give 13.2/7.5/6.4/6.0/6.2 s. There is no
    // lower cut-off — an early single measurement suggested short shortlists
    // lost out to session creation, but medians over repeated runs show 40
    // candidates finishing slightly faster in parallel too (2.56 s against
    // 2.79 s), so the extra branch was fitting noise.
    constexpr int kAutoThreads = 4;

    int wanted = options.matcherThreads;
    if (wanted <= 0)
        wanted = std::min<int>(kAutoThreads, std::max(1, static_cast<int>(candidates.size())));

    QString sessionError;
    const int workers = std::min(d->ensureMatchers(wanted, &sessionError),
                                 std::max(1, static_cast<int>(candidates.size())));

    struct Outcome {
        bool           attempted = false;
        bool           failed    = false;
        int            matches   = 0;
        GeometryResult geometry;
        QString        error;
    };
    std::vector<Outcome> outcomes(static_cast<size_t>(candidates.size()));

    std::atomic<int> done{0};
    parallelForWorker(static_cast<int>(candidates.size()), workers, cancel,
                      [&](int i, int worker) {
        const Candidate &candidate = candidates.at(i);
        Outcome &outcome = outcomes[static_cast<size_t>(i)];
        outcome.attempted = true;

        QString matchError;
        const QList<FeatureMatch> matches =
            d->matchers[static_cast<size_t>(worker)]->match(queryFeatures,
                                                            candidate.features,
                                                            &matchError);
        if (!matchError.isEmpty()) {
            outcome.failed = true;
            outcome.error  = matchError;
            return;
        }
        if (matches.isEmpty())
            return;

        outcome.matches  = static_cast<int>(matches.size());
        outcome.geometry = verifyHomography(queryFeatures, candidate.features,
                                            matches, options.geometry);

        const int finished = done.fetch_add(1, std::memory_order_relaxed) + 1;
        if (progress && (finished % 8 == 0))
            progress(finished, static_cast<int>(candidates.size()));
    });

    int matcherAttempts = 0;
    int matcherFailures = 0;
    QString lastMatcherError;
    for (int i = 0; i < candidates.size(); ++i) {
        const Outcome &outcome = outcomes[static_cast<size_t>(i)];
        if (!outcome.attempted)
            continue;
        ++matcherAttempts;
        if (outcome.failed) {
            ++matcherFailures;
            lastMatcherError = outcome.error;
            continue;
        }
        if (outcome.geometry.ok)
            verified.append({ candidates.at(i).fileId, outcome.matches, outcome.geometry });
    }

    if (progress)
        progress(static_cast<int>(candidates.size()), static_cast<int>(candidates.size()));

    // A model that cannot run at all produces the same empty result as a search
    // that genuinely found nothing. Say which one happened.
    if (matcherAttempts > 0 && matcherFailures == matcherAttempts) {
        if (error) {
            *error = QStringLiteral("the matcher failed on every candidate: %1")
                         .arg(lastMatcherError);
        }
        return out;
    }

    // ---- 4. Rank ------------------------------------------------------------
    // Measured on a screenshot containing a known asset: the true source scored
    // 0.98 inlier ratio while other artwork of the same character topped out at
    // 0.89, yet several of those produced *more* raw inliers because they are
    // larger and busier images. So rank by ratio, discounted while the absolute
    // evidence is still thin.
    const double saturation = std::max(1, options.saturationInliers);
    const auto scoreOf = [saturation](const Verified &v) {
        const double evidence = std::min(1.0, double(v.geometry.inliers) / saturation);
        return v.geometry.inlierRatio * evidence;
    };

    std::sort(verified.begin(), verified.end(), [&](const Verified &a, const Verified &b) {
        const double sa = scoreOf(a);
        const double sb = scoreOf(b);
        if (sa != sb)
            return sa > sb;
        return a.geometry.inliers > b.geometry.inliers;
    });
    if (verified.size() > options.topK)
        verified.resize(options.topK);

    QList<qint64> ids;
    ids.reserve(verified.size());
    for (const Verified &v : verified)
        ids.append(v.fileId);

    QHash<qint64, FileInfoRow> rows;
    for (const FileInfoRow &row : d->database.filesByIds(ids))
        rows.insert(row.id, row);

    out.reserve(verified.size());
    for (const Verified &v : verified) {
        const auto row = rows.constFind(v.fileId);
        if (row == rows.constEnd())
            continue;

        FindResult result;
        result.file        = *row;
        result.bowScore    = bowScores.value(v.fileId, 0.0f);
        result.matches     = v.matches;
        result.inliers     = v.geometry.inliers;
        result.inlierRatio = v.geometry.inlierRatio;
        result.score       = scoreOf(v);
        result.box         = v.geometry.boundingBox;
        result.outline     = v.geometry.outline;
        out.append(result);
    }

    // ---- 5. Correlation channel, for assets features cannot describe --------
    if (options.useTemplateChannel) {
        const QList<FileInfoRow> sparse =
            d->database.filesWithFewFeatures(options.model, 8);

        if (!sparse.isEmpty()) {
            const QString root = d->database.metaValue(QStringLiteral("root"));
            std::vector<TemplateHit> hits(sparse.size());

            // Correlation is pure CPU work with no shared state, so it scales
            // with cores in a way the GPU channel cannot.
            parallelFor(static_cast<int>(sparse.size()),
                        resolveThreadCount(0), cancel, [&](int i) {
                QImageReader reader(absolutePathFor(root, sparse.at(i).rel));
                reader.setAutoTransform(true);
                const QImage asset = reader.read();
                if (asset.isNull())
                    return;
                // Scanning is a different statistical problem from checking one
                // asset. With ~780 candidates over a dozen scales, roughly ten
                // thousand correlations are tried, so a peak that would be
                // convincing on its own turns up by chance several times.
                //
                // Measured on one cluttered frame: false peaks reached 3.6-4.2
                // sigma, while an asset genuinely present reached 6.4 — but a
                // second asset also genuinely present reached only 2.4. No
                // threshold separates presence from coincidence here, so the
                // scan is tuned for precision and simply misses the weak ones.
                // Recall is the feature channel's job; this one exists for the
                // artwork that channel cannot describe at all.
                TemplateOptions scanOptions;
                scanOptions.minPeakZScore = 6.0;
                hits[static_cast<size_t>(i)] = matchByTemplate(query, asset, scanOptions);
            });

            for (int i = 0; i < sparse.size(); ++i) {
                const TemplateHit &hit = hits[static_cast<size_t>(i)];
                if (!hit.ok)
                    continue;
                FindResult result;
                result.file    = sparse.at(i);
                result.score   = hit.score;
                result.box     = hit.box;
                result.outline = QPolygonF(hit.box);
                result.channel = QStringLiteral("correlation");
                result.scale   = hit.scale;
                out.append(result);
            }

            // Correlation scores are not inlier ratios, but both are "how sure
            // are we", and a caller wants one ordered list.
            std::sort(out.begin(), out.end(),
                      [](const FindResult &a, const FindResult &b) {
                          return a.score > b.score;
                      });
            if (out.size() > options.topK)
                out.resize(options.topK);
        }
    }

    return out;
}

} // namespace iw
