/**
 * @file ObjectFinder.h
 * @brief The main scenario: find which indexed assets appear in a screenshot.
 *
 * Three stages, cheapest first:
 *  1. extract local features from the query once;
 *  2. rank the whole library with the bag-of-words index — microseconds;
 *  3. run the neural matcher and RANSAC only on the shortlist, which turns
 *     thousands of model runs per query into a few dozen.
 *
 * The answer is deliberately a ranked list rather than a yes/no: near-identical
 * variants of one asset — a character drawn with several expressions — all pass
 * geometric verification, but the correct one ranks clearly above them.
 */
#pragma once

#include <QImage>
#include <QList>
#include <QPolygonF>
#include <QRectF>
#include <QString>

#include <atomic>
#include <functional>
#include <memory>

#include "core/FeatureMatcher.h"
#include "core/Types.h"

namespace argus {

/** @brief Knobs for one search. */
struct FindOptions {
    /// Empty means "work it out": the extractor beside the executable, and the
    /// matcher export that this machine's execution provider can actually run.
    QString extractorPath;
    QString matcherPath;
    QString model         = QStringLiteral("disk"); ///< Extractor identity in the index.

    /**
     * @brief Candidates taken from the bag-of-words ranking.
     *
     * 400 rather than 200 because tiling turned one document per picture into
     * one per tile, and because every change to the descriptors reshuffles this
     * ranking. Measured on a 4810-tile library: at 200 the reference screenshot
     * returns other artwork of the same character and not the file it was
     * actually built from; at 400 that file comes first at 98% consistency.
     * The cost is linear — about 18 seconds against 9.
     */
    int  shortlist    = 400;
    int  topK         = 10;   ///< Verified results returned.

    /**
     * @brief Inlier count above which the absolute evidence counts as complete.
     *
     * Ranking uses the inlier *ratio*, because that is what separates the asset
     * actually present in the frame from other artwork of the same character:
     * for the real source nearly every correspondence is geometrically
     * consistent, for a lookalike only the shared region is. Ratio alone would
     * let a thin 10-of-10 match outrank a solid 123-of-126, so it is discounted
     * until the inlier count reaches this value.
     */
    int  saturationInliers = 50;
    /**
     * Also search assets that local features cannot describe, by correlation.
     *
     * About a sixth of a game library has too few keypoints to match — soft
     * gradients, frames, glows. Measured over a sample of those, correlation
     * locates roughly 45%; the rest carry their shape purely in alpha and have
     * no appearance of their own once composited, so nothing can find them.
     *
     * Off by default: it costs a correlation per candidate rather than a
     * shortlist lookup, which is seconds rather than milliseconds.
     */
    bool useTemplateChannel = false;

    int  maxKeypoints = 512;  ///< Keypoints extracted from the query.
    int  maxSide      = 1024; ///< Long side the extractor sees.
    bool useGpu       = true; ///< Execution provider for the extractor.

    /// Provider for the matcher, chosen separately from the extractor.
    /// The extractor runs one big convolutional pass and belongs on the GPU;
    /// the matcher runs hundreds of tiny transformer passes, where per-call
    /// dispatch overhead can outweigh the arithmetic. Which wins is a
    /// measurement, not an assumption — see FindOptions::matcherUseGpu in the
    /// skill notes for the numbers on this machine.
    bool matcherUseGpu = true;

    /**
     * Matcher sessions run concurrently over the shortlist.
     *
     * Each thread needs its own session: the DirectML provider drives one
     * command queue per session and is not safe to call from several threads at
     * once. Sessions cost roughly half a second of shader compilation each, so
     * more is not automatically faster: over 200 candidates 1/2/3/4/6 threads
     * measure 13.2/7.5/6.4/6.0/6.2 s — the gain flattens at four and reverses
     * beyond it.
     *
     * 0 means auto, which is four capped by the number of candidates.
     */
    int matcherThreads = 0;

    GeometryOptions geometry;
};

/** @brief One asset found inside the query image. */
struct FindResult {
    FileInfoRow file;
    float       bowScore    = 0.0f; ///< Shortlist similarity, 0..1.
    int         matches     = 0;    ///< Correspondences the matcher produced.
    int         inliers     = 0;    ///< Correspondences consistent with the geometry.
    double      inlierRatio = 0.0;
    double      score       = 0.0;  ///< Ranking score; see FindOptions::saturationInliers.
    QRectF      box;                ///< Where the asset sits in the query image.
    QPolygonF   outline;            ///< The same placement, before axis alignment.

    /// Which channel found it: "features" or "correlation". They measure
    /// different things, so their scores are not directly comparable.
    QString     channel = QStringLiteral("features");
    double      scale   = 1.0;      ///< Correlation only: size relative to native.
};

/**
 * @brief Holds the models and indexes needed to answer searches.
 *
 * @note Creating one loads two ONNX sessions, which costs a few seconds of
 *       shader compilation. Create it once and reuse it across searches.
 * @note Not thread-safe; serialise calls to @ref find.
 */
class ObjectFinder
{
public:
    ~ObjectFinder();

    ObjectFinder(const ObjectFinder &)            = delete;
    ObjectFinder &operator=(const ObjectFinder &) = delete;

    /**
     * @brief Open the index and load the models.
     * @param databasePath Path to @c index.db; descriptors and the vocabulary
     *                     are expected in a @c features directory beside it.
     * @param options      Model paths and provider preference.
     * @param error        Optional out-parameter receiving a failure description.
     * @return A ready finder, or @c nullptr on failure.
     */
    static std::unique_ptr<ObjectFinder> create(const QString &databasePath,
                                                const FindOptions &options,
                                                QString *error = nullptr);

    /**
     * @brief Find indexed assets present in an image.
     * @param query    Screenshot, or the region of it the user boxed.
     * @param options  Shortlist size, result count and geometry thresholds.
     * @param cancel   Optional flag polled between candidates.
     * @param progress Optional callback invoked as <tt>(done, total)</tt>.
     * @param error    Optional out-parameter receiving a failure description.
     * @return Verified results ordered by descending inlier count.
     */
    QList<FindResult> find(const QImage &query,
                           const FindOptions &options,
                           const std::atomic_bool *cancel = nullptr,
                           const std::function<void(int, int)> &progress = {},
                           QString *error = nullptr);

    /** @return Number of images the bag-of-words index covers. */
    int documentCount() const;

    /** @return Execution provider both models ended up using. */
    QString provider() const;

private:
    ObjectFinder();

    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace argus
