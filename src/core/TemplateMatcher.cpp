#include "core/TemplateMatcher.h"

#ifdef IMAGEWORKER_WITH_INFERENCE

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace iw {

namespace {

/**
 * @brief Wrap a QImage as an OpenCV matrix without copying more than necessary.
 * @param image Source image.
 * @param bgr   Out-parameter receiving a 3-channel colour matrix.
 * @param alpha Out-parameter receiving the alpha channel, or a fully opaque one.
 * @return @c false when the image could not be converted.
 * @note Goes through RGBA8888 so the byte order is known; QImage scanlines are
 *       padded, hence passing bytesPerLine as the matrix step.
 */
bool toMats(const QImage &image, cv::Mat &bgr, cv::Mat &alpha)
{
    const QImage rgba = image.convertToFormat(QImage::Format_RGBA8888);
    if (rgba.isNull())
        return false;

    const cv::Mat wrapped(rgba.height(), rgba.width(), CV_8UC4,
                          const_cast<uchar *>(rgba.bits()),
                          static_cast<size_t>(rgba.bytesPerLine()));

    std::vector<cv::Mat> channels;
    cv::split(wrapped, channels); // R, G, B, A
    if (channels.size() != 4)
        return false;

    std::vector<cv::Mat> colour{ channels[2], channels[1], channels[0] };
    cv::merge(colour, bgr);
    alpha = channels[3].clone();
    bgr = bgr.clone(); // detach from the QImage buffer before it goes away
    return true;
}

/** @brief Standard deviation of the colour channels over the opaque pixels. */
double maskedStdDev(const cv::Mat &bgr, const cv::Mat &mask)
{
    cv::Scalar mean;
    cv::Scalar stddev;
    cv::meanStdDev(bgr, mean, stddev, mask);
    // Average across channels: a template that varies in any one of them has
    // something to correlate against.
    return (stddev[0] + stddev[1] + stddev[2]) / 3.0;
}

} // namespace

TemplateHit matchByTemplate(const QImage &query,
                            const QImage &asset,
                            const TemplateOptions &options)
{
    TemplateHit hit;

    if (query.isNull() || asset.isNull()) {
        hit.rejection = QStringLiteral("empty image");
        return hit;
    }

    // Work at a reduced resolution: correlation cost scales with area, and the
    // extra pixels buy nothing for locating an object.
    double queryScale = 1.0;
    QImage workQuery = query;
    const int longest = std::max(query.width(), query.height());
    if (options.queryMaxSide > 0 && longest > options.queryMaxSide) {
        workQuery = query.scaled(options.queryMaxSide, options.queryMaxSide,
                                 Qt::KeepAspectRatio, Qt::SmoothTransformation);
        queryScale = double(query.width()) / std::max(1, workQuery.width());
    }

    cv::Mat queryBgr, queryAlpha;
    cv::Mat assetBgr, assetAlpha;
    if (!toMats(workQuery, queryBgr, queryAlpha) || !toMats(asset, assetBgr, assetAlpha)) {
        hit.rejection = QStringLiteral("colour conversion failed");
        return hit;
    }

    // A template with no variation correlates with everything; refuse it.
    cv::Mat opaque;
    cv::threshold(assetAlpha, opaque, 16, 255, cv::THRESH_BINARY);
    if (cv::countNonZero(opaque) < 16) {
        hit.rejection = QStringLiteral("asset is fully transparent");
        return hit;
    }
    const double variation = maskedStdDev(assetBgr, opaque);
    if (variation < options.minTemplateStdDev) {
        // Distinguish the two ways an asset can be featureless, because they
        // mean different things. A shape carried entirely by alpha — a shadow,
        // a glow — is not merely hard to correlate: once composited, every
        // pixel of it depends on whatever was underneath, so its appearance in
        // a screenshot is not a property of the asset at all.
        cv::Scalar alphaMean;
        cv::Scalar alphaDev;
        cv::meanStdDev(assetAlpha, alphaMean, alphaDev);
        hit.rejection =
            alphaDev[0] > 8.0
                ? QStringLiteral("asset has no colour of its own (deviation %1); it is a "
                                 "shape in alpha only, and blends with whatever is behind it")
                      .arg(variation, 0, 'f', 1)
                : QStringLiteral("asset is uniform (deviation %1); nothing to correlate")
                      .arg(variation, 0, 'f', 1);
        return hit;
    }

    // Build the grid outwards from 1.0 rather than upwards from the minimum.
    // A geometric series between the bounds need not contain 1.0 at all, and
    // native size is by far the most likely one — an asset matched against
    // itself was scoring 0.93 at scale 0.91 purely because 1.00 was not tried.
    std::vector<double> scales;
    {
        const int steps = std::max(1, options.scaleSteps);
        const double ratio = std::pow(options.maxScale / options.minScale,
                                      1.0 / std::max(1, steps - 1));
        scales.push_back(1.0);
        for (double s = 1.0 / ratio; s >= options.minScale * 0.999; s /= ratio)
            scales.push_back(s);
        for (double s = 1.0 * ratio; s <= options.maxScale * 1.001; s *= ratio)
            scales.push_back(s);
        std::sort(scales.begin(), scales.end());
    }

    double bestScore = -1.0;
    double bestScale = 1.0;
    double bestZ     = -1.0;
    bool   anySharp  = false;
    cv::Point bestLoc;
    cv::Size bestSize;

    for (double assetScale : scales) {
        // The scale is relative to the asset's native size, so it has to be
        // divided by the query's own downscale to land in working pixels.
        const double workScale = assetScale / queryScale;

        cv::Size size(int(std::lround(assetBgr.cols * workScale)),
                      int(std::lround(assetBgr.rows * workScale)));
        if (size.width < options.minTemplateSide || size.height < options.minTemplateSide)
            continue;
        if (size.width > queryBgr.cols || size.height > queryBgr.rows)
            continue;

        const double area = double(size.width) * size.height;
        if (area < options.minTemplateArea * double(queryBgr.cols) * queryBgr.rows)
            continue;

        cv::Mat tmpl;
        cv::Mat mask;
        cv::resize(assetBgr, tmpl, size, 0, 0, cv::INTER_AREA);
        cv::resize(opaque, mask, size, 0, 0, cv::INTER_AREA);

        cv::Mat result;
        try {
            // TM_CCORR_NORMED is the method OpenCV supports a mask with, which
            // is what lets a sprite be compared on its own shape only.
            cv::matchTemplate(queryBgr, tmpl, result, cv::TM_CCORR_NORMED, mask);
        } catch (const cv::Exception &) {
            continue;
        }

        // A masked correlation can produce non-finite values where the mask
        // covers a constant region; those are not matches.
        cv::patchNaNs(result, 0.0f);

        double maxValue = 0.0;
        cv::Point maxLoc;
        cv::minMaxLoc(result, nullptr, &maxValue, nullptr, &maxLoc);

        // How much the peak stands out from the rest of the surface. A genuine
        // placement is a spike; a template that merely resembles smooth
        // background produces an equally high but featureless plateau.
        //
        // The measure needs somewhere for a rival peak to appear. When the
        // template nearly fills the query the map collapses towards a single
        // value, its deviation goes to zero, and the score is meaningless —
        // not bad, absent. Treat those scales as unconstrained instead, or a
        // template matched against itself loses to a shrunken copy of itself.
        constexpr int kMapNeededForZ = 64;
        const bool zApplies = result.total() >= kMapNeededForZ;

        double z = std::numeric_limits<double>::infinity();
        if (zApplies) {
            cv::Scalar mapMean;
            cv::Scalar mapDev;
            cv::meanStdDev(result, mapMean, mapDev);
            z = mapDev[0] > 1e-6 ? (maxValue - mapMean[0]) / mapDev[0] : 0.0;
        }

        // Among scales that survive the sharpness test, the highest correlation
        // wins. Ranking by sharpness alone would prefer a distinctive poor
        // match over an unmistakable good one.
        const bool sharpEnough = (z >= options.minPeakZScore);
        const bool better = sharpEnough ? (!anySharp || maxValue > bestScore)
                                        : (!anySharp && maxValue > bestScore);
        if (better) {
            anySharp  = anySharp || sharpEnough;
            bestZ     = z;
            bestScore = maxValue;
            bestScale = assetScale;
            bestLoc   = maxLoc;
            bestSize  = size;
        }
    }

    if (bestScore < 0.0) {
        hit.rejection = QStringLiteral("asset does not fit the image at any tried scale");
        return hit;
    }

    hit.score = bestScore;
    hit.scale = bestScale;
    hit.peakZ = bestZ;
    hit.box   = QRectF(bestLoc.x * queryScale, bestLoc.y * queryScale,
                       bestSize.width * queryScale, bestSize.height * queryScale);

    if (bestScore < options.minScore) {
        hit.rejection = QStringLiteral("best correlation %1 is below %2")
                            .arg(bestScore, 0, 'f', 3)
                            .arg(options.minScore, 0, 'f', 2);
        return hit;
    }
    if (bestZ < options.minPeakZScore) {
        hit.rejection = QStringLiteral("correlation %1 is high but flat (peak only %2 sigma "
                                       "above the map); it matches the background as well "
                                       "as the object")
                            .arg(bestScore, 0, 'f', 3)
                            .arg(bestZ, 0, 'f', 1);
        return hit;
    }

    hit.ok = true;
    return hit;
}

} // namespace iw

#else // IMAGEWORKER_WITH_INFERENCE

namespace iw {

TemplateHit matchByTemplate(const QImage &, const QImage &, const TemplateOptions &)
{
    TemplateHit hit;
    hit.rejection = QStringLiteral(
        "built without OpenCV; reconfigure with -DIMAGEWORKER_WITH_INFERENCE=ON");
    return hit;
}

} // namespace iw

#endif
