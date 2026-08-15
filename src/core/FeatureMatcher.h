/**
 * @file FeatureMatcher.h
 * @brief Neural descriptor matching (LightGlue) plus geometric verification.
 *
 * Matching descriptors gives correspondences; only geometry tells you whether
 * those correspondences describe one rigid object or a scatter of coincidences.
 * A homography fitted with RANSAC does both jobs at once: it counts how many
 * matches agree, and it says where in the query image the asset sits.
 */
#pragma once

#include <QList>
#include <QPolygonF>
#include <QRectF>
#include <QString>

#include <memory>

#include "core/FeatureExtractor.h"

namespace argus {

/** @brief One descriptor correspondence between two images. */
struct FeatureMatch {
    int   queryIndex = 0; ///< Keypoint index in the query feature set.
    int   assetIndex = 0; ///< Keypoint index in the candidate feature set.
    float score      = 0.0f; ///< Matcher confidence.
};

/** @brief Outcome of fitting a homography to a set of correspondences. */
struct GeometryResult {
    bool      ok        = false; ///< A homography was found and enough matches agreed.
    int       inliers   = 0;     ///< Correspondences consistent with the homography.
    int       candidates = 0;    ///< Correspondences fed to RANSAC.
    double    inlierRatio = 0.0; ///< inliers / candidates.
    QPolygonF outline;           ///< The asset's rectangle projected into the query image.
    QRectF    boundingBox;       ///< Axis-aligned bounds of @ref outline.
    QString   rejection;         ///< Why the fit was rejected, when @ref ok is false.
};

/** @brief Knobs for verification. */
struct GeometryOptions {
    double reprojectionThreshold = 4.0; ///< RANSAC inlier distance, query pixels.
    int    minInliers            = 8;   ///< Below this a match is not believable.
    double minInlierRatio        = 0.25; ///< Guards against a homography fitted to noise.
    int    maxIterations         = 4000;

    /// @name Shape sanity
    /// Inlier counts alone do not reject a degenerate fit: RANSAC will happily
    /// return a transform that folds the asset into a sliver or blows it up to
    /// thousands of pixels outside the frame. The projected quadrilateral has
    /// to look like a plausible placement too.
    /// @{
    bool   checkShape       = true;
    double minAreaFraction  = 0.0005; ///< Projected area, relative to the query image.
    double maxAreaFraction  = 4.0;
    double maxAspectRatio   = 12.0;   ///< Longest projected side over shortest.
    /// @}
};

/**
 * @brief Fit a homography mapping asset keypoints onto query keypoints.
 * @param query   Features of the screenshot or crop being searched.
 * @param asset   Features of the indexed candidate.
 * @param matches Correspondences between them.
 * @param options RANSAC thresholds.
 * @return Inlier count and the projected outline; check GeometryResult::ok.
 * @note A homography covers anisotropic scaling, so a nine-sliced UI element
 *       stretched in one axis still verifies.
 */
GeometryResult verifyHomography(const FeatureSet &query,
                                const FeatureSet &asset,
                                const QList<FeatureMatch> &matches,
                                const GeometryOptions &options = {});

/**
 * @brief Runs a LightGlue ONNX model over two feature sets.
 *
 * @note One instance owns one inference session and is **not** thread-safe.
 * @note LightGlue expects keypoints normalised against the image they came
 *       from. Whether a particular ONNX export does that internally varies, so
 *       @ref setNormalizeKeypoints exists and the shipped default is verified
 *       empirically rather than assumed.
 */
class FeatureMatcher
{
public:
    ~FeatureMatcher();

    FeatureMatcher(const FeatureMatcher &)            = delete;
    FeatureMatcher &operator=(const FeatureMatcher &) = delete;

    /**
     * @brief Load a matcher model.
     * @param modelPath      Path to the LightGlue .onnx file.
     * @param preferDirectML Try the GPU provider before falling back to the CPU.
     * @param error          Optional out-parameter receiving a failure description.
     * @return A ready matcher, or @c nullptr on failure.
     */
    static std::unique_ptr<FeatureMatcher> create(const QString &modelPath,
                                                  bool preferDirectML = true,
                                                  QString *error = nullptr);

    /**
     * @brief Match two feature sets.
     * @param query Features of the image being searched.
     * @param asset Features of the candidate.
     * @param error Optional out-parameter receiving a failure description.
     * @return Correspondences, possibly empty.
     */
    QList<FeatureMatch> match(const FeatureSet &query,
                              const FeatureSet &asset,
                              QString *error = nullptr);

    /** @brief Choose whether keypoints are normalised before being fed to the model. */
    void setNormalizeKeypoints(bool enabled);

    /** @return Whether keypoint normalisation is applied. */
    bool normalizeKeypoints() const;

    /** @return Execution provider the session uses. */
    QString provider() const;

private:
    FeatureMatcher();

    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace argus
