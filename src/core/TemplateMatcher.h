/**
 * @file TemplateMatcher.h
 * @brief Masked normalised cross-correlation, the second matching channel.
 *
 * Local features need corners. Roughly a sixth of a game asset library has
 * none — shadows, glows, gradients, translucent overlays, plain frames — and
 * for those the neural pipeline cannot answer at all. Correlation does not care
 * about corners: it compares pixels directly, which is exactly right for soft
 * artwork and exactly wrong for anything rotated or perspective-warped.
 *
 * The two channels are complementary rather than competing: features handle
 * textured art at any orientation, correlation handles smooth art at
 * axis-aligned scales.
 */
#pragma once

#include <QImage>
#include <QRectF>
#include <QString>

namespace iw {

/** @brief Knobs for one correlation search. */
struct TemplateOptions {
    double minScale     = 0.35; ///< Smallest size the asset may appear at.
    double maxScale     = 1.60; ///< Largest.
    int    scaleSteps   = 13;   ///< Sizes tried, spaced geometrically around 1.0.
    double minScore     = 0.90; ///< Correlation below this is not a match.
    int    queryMaxSide = 720;  ///< Query is worked at this resolution.

    /**
     * Smallest template accepted, in working pixels.
     *
     * Small patches correlate with almost anything: a 30x20 template scores
     * 0.999 against a smooth gradient. Nothing below this can be believed,
     * whatever the score says.
     */
    int    minTemplateSide = 28;

    /** Template must also cover at least this fraction of the query area. */
    double minTemplateArea = 0.002;

    /**
     * How far the correlation peak must stand above the rest of the map, in
     * standard deviations.
     *
     * This is what separates a real placement from a trivial one. A true match
     * is a sharp isolated spike; correlating a smooth template against a smooth
     * background gives a broad plateau that is just as high but says nothing.
     *
     * @warning Calibrated on a handful of cases: an asset genuinely present in
     *          a cluttered frame measured 4.9 sigma, while three convincing
     *          false positives measured 1.7 to 2.2. The gap is real but narrow,
     *          and this number deserves re-measuring against a proper set of
     *          labelled screenshots before it is trusted far.
     */
    double minPeakZScore = 3.5;

    /**
     * Templates flatter than this (standard deviation over the opaque pixels,
     * 0..255) are refused rather than matched.
     *
     * A uniform patch correlates near-perfectly with any equally uniform area,
     * so "found" would be meaningless. An asset with no variation genuinely
     * cannot be located by appearance, and saying so beats a confident guess.
     */
    double minTemplateStdDev = 6.0;
};

/** @brief Where and how well an asset correlated inside a query image. */
struct TemplateHit {
    bool    ok    = false;  ///< A location passed both thresholds.
    double  score = 0.0;    ///< Peak normalised correlation, 0..1.
    double  scale = 1.0;    ///< Asset size relative to its native size.
    double  peakZ = 0.0;    ///< How far the peak stood above the map, in sigmas.
    QRectF  box;            ///< Placement in query-image coordinates.
    QString rejection;      ///< Why nothing was reported, when @ref ok is false.
};

/**
 * @brief Locate an asset inside an image by masked correlation.
 * @param query   Image to search in, e.g. a screenshot.
 * @param asset   Asset to look for; its alpha channel becomes the mask.
 * @param options Scale range and thresholds.
 * @return Best placement found; check TemplateHit::ok.
 *
 * @note Only translation and uniform scale are searched. Rotation, shear and
 *       perspective are the feature channel's job.
 * @note Transparent pixels are excluded from the comparison, so a sprite is
 *       matched on its own shape rather than on whatever it was drawn over.
 */
TemplateHit matchByTemplate(const QImage &query,
                            const QImage &asset,
                            const TemplateOptions &options = {});

} // namespace iw
