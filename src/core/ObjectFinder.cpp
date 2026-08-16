#include "core/ObjectFinder.h"

#include <QRectF>
#include <QSet>

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

namespace argus {

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
     * @brief Rank candidates by asking the index about windows of the query.
     * @param query             The screenshot or crop being searched.
     * @param extractorOptions  Settings the query was described with.
     * @param wanted            How many candidates the matcher will take.
     * @return Merged ranking, best first, at most @p wanted long.
     */
    QList<BowHit> probedShortlist(const QImage &query,
                                  const ExtractorOptions &extractorOptions,
                                  int wanted);

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
            *error = QStringLiteral("%1\nhint: run 'argus vocab' to train one")
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

namespace {

/**
 * @brief The windows a query is probed through, largest first.
 * @param size Query size in pixels.
 * @return The whole frame, then a 3x3 grid of half-size windows overlapping by
 *         half, so anything up to a quarter of the frame is whole inside one.
 */
QList<QRect> probeWindows(const QSize &size)
{
    QList<QRect> windows;
    windows.append(QRect(QPoint(0, 0), size));

    const int w = size.width()  / 2;
    const int h = size.height() / 2;
    if (w < 64 || h < 64)
        return windows;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            windows.append(QRect(col * w / 2, row * h / 2, w, h));
        }
    }
    return windows;
}

} // namespace

QList<BowHit> ObjectFinder::Impl::probedShortlist(const QImage &query,
                                                  const ExtractorOptions &extractorOptions,
                                                  int wanted)
{
    // A bag-of-words vector describes a whole picture. That is the right
    // question when the query *is* the thing being looked for, and the wrong
    // one when the query is a screenshot that merely contains it: a 362x362
    // asset inside a 1280x720 frame owns a seventh of the keypoints, so its
    // words are outnumbered by furniture and the true source ranked past 500 of
    // 4948. Boxing the object by hand fixed it, which is the evidence that the
    // frame, not the matcher, was the problem.
    //
    // So the frame is asked about in windows as well as whole. Each window is
    // cropped and described in its own right rather than by reusing the
    // keypoints of the full-frame pass that happen to fall inside it - that
    // shortcut costs nothing and is worth nothing, because those points were
    // detected at the frame's scale and spread across the frame's budget.
    // Measured on the reference screenshot: reusing them left the true source
    // unreachable, describing the window properly ranks it first at 100%
    // consistency. Ten extractions cost about a second; the matcher then spends
    // ten times that on whatever they turn up.
    QList<QList<BowHit>> ranked;

    for (const QRect &window : probeWindows(query.size())) {
        const QImage piece = window.size() == query.size() ? query : query.copy(window);

        QString ignored;
        const FeatureSet features = extractor->extract(piece, extractorOptions, &ignored);
        if (features.count() < 24)
            continue; // a window of flat background describes nothing

        const QList<quint32> words =
            vocabulary->assign(features.descriptors, features.count());
        if (words.isEmpty())
            continue;

        const QList<BowHit> hits = bow->query(words, wanted);
        if (!hits.isEmpty())
            ranked.append(hits);
    }

    QList<BowHit> out;
    if (ranked.isEmpty())
        return out;

    // Merged round-robin rather than concatenated: the object lives in one
    // window and would otherwise be crowded out by the nine that hold
    // background, each ranking its own furniture first with great confidence.
    QSet<qint64> taken;
    out.reserve(wanted);
    for (int depth = 0; out.size() < wanted; ++depth) {
        bool anyLeft = false;
        for (const QList<BowHit> &list : ranked) {
            if (depth >= list.size())
                continue;
            anyLeft = true;
            const BowHit &hit = list.at(depth);
            if (taken.contains(hit.recordId))
                continue;
            taken.insert(hit.recordId);
            out.append(hit);
            if (out.size() >= wanted)
                break;
        }
        if (!anyLeft)
            break;
    }
    return out;
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
    const QList<BowHit> shortlist =
        d->probedShortlist(query, extractorOptions, std::max(1, options.shortlist));
    if (shortlist.isEmpty())
        return out;

    // Keyed by record id, because that is what the shortlist ranks: its
    // documents are tiles. Turning that into a per-file score has to wait until
    // the verification below decides which tile of a file actually won.
    QHash<qint64, float> bowScores;
    bowScores.reserve(shortlist.size());
    for (const BowHit &hit : shortlist)
        bowScores.insert(hit.recordId, hit.score);

    // ---- 3. Verify the shortlist -------------------------------------------
    struct Verified {
        qint64         fileId;
        int            matches;
        GeometryResult geometry;
    };
    // Several tiles of one picture can each verify. They are the same answer
    // seen through different windows, so only the strongest is kept.
    QHash<qint64, int> bestByFile;
    QList<Verified> verified;

    // Phase A, serial: pull every candidate's features into memory. Neither the
    // descriptor store (one QFile it seeks around) nor the database (one
    // QSqlQuery) is safe to touch from several threads, and at roughly 40 KB per
    // candidate the whole shortlist costs a few megabytes.
    struct Candidate {
        qint64     fileId   = 0;
        qint64     recordId = 0; ///< The tile this came from; what bowScores is keyed by.
        FeatureSet features;
    };
    QList<Candidate> candidates;
    candidates.reserve(shortlist.size());

    for (const BowHit &hit : shortlist) {
        if (cancel && cancel->load(std::memory_order_relaxed))
            break;

        FeatureRecord record;
        if (!d->database.featureById(hit.recordId, &record))
            continue;

        FeatureLocation location;
        location.descOffset = record.descOffset;
        location.kptsOffset = record.kptsOffset;
        location.count      = record.count;
        location.dim        = record.dim;

        FeatureSet assetFeatures = d->store.load(location, record.imageWidth, record.imageHeight);
        if (assetFeatures.isEmpty())
            continue;

        candidates.append({ record.fileId, record.id, std::move(assetFeatures) });
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

    // The shortlist score of the tile that won, per file. Reporting it needs
    // this indirection: bowScores is keyed by record id and the result is keyed
    // by file id, and reading one map with the other's keys returned 0.0 for
    // every result — a column of zeroes in the GUI and in --json that looked
    // like the shortlist had contributed nothing.
    QHash<qint64, float> bowScoreByFile;

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
        if (!outcome.geometry.ok)
            continue;

        // No coordinate translation here, and none is needed: the homography
        // maps the tile onto the query, so the outline is already in query
        // pixels, which is the frame the box is drawn on. The tile's position
        // inside its picture never leaves the index.
        const Candidate &candidate = candidates.at(i);
        Verified entry{ candidate.fileId, outcome.matches, outcome.geometry };

        const auto seen = bestByFile.constFind(entry.fileId);
        if (seen != bestByFile.constEnd()) {
            Verified &kept = verified[*seen];
            if (entry.geometry.inliers > kept.geometry.inliers) {
                kept = entry;
                bowScoreByFile.insert(entry.fileId, bowScores.value(candidate.recordId, 0.0f));
            }
            continue;
        }
        bestByFile.insert(entry.fileId, static_cast<int>(verified.size()));
        bowScoreByFile.insert(entry.fileId, bowScores.value(candidate.recordId, 0.0f));
        verified.append(entry);
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
        result.bowScore    = bowScoreByFile.value(v.fileId, 0.0f);
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

} // namespace argus
