#include "core/FeatureMatcher.h"

#include <QFileInfo>

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#ifdef IMAGEWORKER_WITH_INFERENCE
#include "core/OnnxProvider.h"

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#endif

namespace iw {

#ifdef IMAGEWORKER_WITH_INFERENCE

namespace {

#if defined(_WIN32)
std::wstring nativePath(const QString &path) { return path.toStdWString(); }
#else
std::string nativePath(const QString &path) { return path.toStdString(); }
#endif

/**
 * @brief LightGlue's keypoint normalisation: centre on the image, scale by half
 *        its longest side.
 */
std::vector<float> normalizedKeypoints(const FeatureSet &features, bool enabled)
{
    std::vector<float> out;
    out.reserve(static_cast<size_t>(features.count()) * 2);

    if (!enabled) {
        for (float v : features.keypoints)
            out.push_back(v);
        return out;
    }

    const float shiftX = features.imageWidth  * 0.5f;
    const float shiftY = features.imageHeight * 0.5f;
    const float scale  = std::max(1.0f, std::max(float(features.imageWidth),
                                                 float(features.imageHeight)) * 0.5f);

    for (int i = 0; i < features.count(); ++i) {
        out.push_back((features.keypoints.at(i * 2)     - shiftX) / scale);
        out.push_back((features.keypoints.at(i * 2 + 1) - shiftY) / scale);
    }
    return out;
}

/**
 * @brief Reject projected quadrilaterals that cannot be a real placement.
 * @param outline Projected asset corners, in query pixels.
 * @param query   Features of the query, for its image size.
 * @param asset   Features of the candidate, for its image size.
 * @param options Shape limits.
 * @return Empty when the shape is plausible, otherwise the reason to reject it.
 */
QString shapeRejection(const QPolygonF &outline,
                       const FeatureSet &query,
                       const FeatureSet &asset,
                       const GeometryOptions &options)
{
    if (outline.size() != 4)
        return QStringLiteral("outline is not a quadrilateral");

    for (const QPointF &p : outline) {
        if (!std::isfinite(p.x()) || !std::isfinite(p.y()))
            return QStringLiteral("outline has non-finite corners");
    }

    // Convexity: every cross product of consecutive edges must share a sign.
    // A self-intersecting quad means the homography folded the plane over.
    int sign = 0;
    for (int i = 0; i < 4; ++i) {
        const QPointF a = outline.at(i);
        const QPointF b = outline.at((i + 1) % 4);
        const QPointF c = outline.at((i + 2) % 4);
        const double cross = (b.x() - a.x()) * (c.y() - b.y())
                             - (b.y() - a.y()) * (c.x() - b.x());
        const int current = cross > 0 ? 1 : (cross < 0 ? -1 : 0);
        if (current == 0)
            return QStringLiteral("outline is degenerate");
        if (sign == 0)
            sign = current;
        else if (current != sign)
            return QStringLiteral("outline is not convex");
    }

    // Shoelace area against the size of the image we searched.
    double area = 0.0;
    for (int i = 0; i < 4; ++i) {
        const QPointF a = outline.at(i);
        const QPointF b = outline.at((i + 1) % 4);
        area += a.x() * b.y() - b.x() * a.y();
    }
    area = std::abs(area) * 0.5;

    const double queryArea = double(query.imageWidth) * query.imageHeight;
    if (queryArea <= 0.0)
        return QStringLiteral("query has no area");

    const double fraction = area / queryArea;
    if (fraction < options.minAreaFraction)
        return QStringLiteral("projection collapsed to %1%% of the frame")
            .arg(fraction * 100.0, 0, 'g', 2);

    // An asset bigger than the query legitimately projects to more than the
    // frame: that is exactly what "this crop is a piece of that texture" looks
    // like, and a fixed cap threw those answers away after verifying them. A
    // 25% crop of a texture had 31 inliers at 78% consistency and was rejected
    // for "covering 16x the frame", which was the correct answer stated as a
    // fault.
    //
    // The ceiling therefore scales with how much bigger the asset is: at 1:1
    // pixels the asset covers assetArea/queryArea frames, and the configured
    // factor is the slack on top of that for a query captured larger than the
    // source. A small asset blowing up over a big screenshot - the degenerate
    // fit this check exists for - still hits the same tight limit as before.
    const double assetArea = double(asset.imageWidth) * asset.imageHeight;
    const double atNativeScale = (assetArea > 0.0) ? assetArea / queryArea : 1.0;
    const double ceiling = options.maxAreaFraction * std::max(1.0, atNativeScale);
    if (fraction > ceiling) {
        return QStringLiteral("projection covers %1x the frame, over the %2x a %3x%4 asset allows")
            .arg(fraction, 0, 'f', 1)
            .arg(ceiling, 0, 'f', 1)
            .arg(asset.imageWidth)
            .arg(asset.imageHeight);
    }

    // Extreme shear shows up as one pair of sides far longer than the other.
    double shortest = std::numeric_limits<double>::max();
    double longest  = 0.0;
    for (int i = 0; i < 4; ++i) {
        const QPointF a = outline.at(i);
        const QPointF b = outline.at((i + 1) % 4);
        const double len = std::hypot(b.x() - a.x(), b.y() - a.y());
        shortest = std::min(shortest, len);
        longest  = std::max(longest, len);
    }
    if (shortest <= 1.0)
        return QStringLiteral("projection has a degenerate side");
    if (longest / shortest > options.maxAspectRatio)
        return QStringLiteral("projection is sheared %1:1").arg(longest / shortest, 0, 'f', 1);

    return {};
}

} // namespace

struct FeatureMatcher::Impl {
    Ort::Env env{ ORT_LOGGING_LEVEL_ERROR, "imageworker-matcher" };
    std::unique_ptr<Ort::Session> session;
    Ort::AllocatorWithDefaultOptions allocator;

    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    QString provider = QStringLiteral("CPU");
    bool    normalize = true;
};

FeatureMatcher::FeatureMatcher() : d(std::make_unique<Impl>()) {}
FeatureMatcher::~FeatureMatcher() = default;

QString FeatureMatcher::provider() const { return d->provider; }
void FeatureMatcher::setNormalizeKeypoints(bool enabled) { d->normalize = enabled; }
bool FeatureMatcher::normalizeKeypoints() const { return d->normalize; }

std::unique_ptr<FeatureMatcher> FeatureMatcher::create(const QString &modelPath,
                                                       bool preferDirectML,
                                                       QString *error)
{
    if (!QFileInfo::exists(modelPath)) {
        if (error)
            *error = QStringLiteral("no such model: %1").arg(modelPath);
        return nullptr;
    }

    std::unique_ptr<FeatureMatcher> self(new FeatureMatcher);
    try {
        Ort::SessionOptions options;
        self->d->provider = onnx::configureProvider(options, preferDirectML);

        self->d->session = std::make_unique<Ort::Session>(
            self->d->env, nativePath(modelPath).c_str(), options);

        for (size_t i = 0; i < self->d->session->GetInputCount(); ++i) {
            const auto name = self->d->session->GetInputNameAllocated(i, self->d->allocator);
            self->d->inputNames.emplace_back(name.get());
        }
        for (size_t i = 0; i < self->d->session->GetOutputCount(); ++i) {
            const auto name = self->d->session->GetOutputNameAllocated(i, self->d->allocator);
            self->d->outputNames.emplace_back(name.get());
        }
        if (self->d->inputNames.size() != 4) {
            if (error)
                *error = QStringLiteral("expected 4 inputs (kpts0,kpts1,desc0,desc1), model has %1")
                             .arg(self->d->inputNames.size());
            return nullptr;
        }
    } catch (const std::exception &e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return nullptr;
    }
    return self;
}

QList<FeatureMatch> FeatureMatcher::match(const FeatureSet &query,
                                          const FeatureSet &asset,
                                          QString *error)
{
    QList<FeatureMatch> out;
    if (query.isEmpty() || asset.isEmpty())
        return out;
    if (query.dim != asset.dim) {
        if (error)
            *error = QStringLiteral("descriptor dimensions differ: %1 vs %2")
                         .arg(query.dim).arg(asset.dim);
        return out;
    }

    std::vector<float> kpts0 = normalizedKeypoints(query, d->normalize);
    std::vector<float> kpts1 = normalizedKeypoints(asset, d->normalize);
    std::vector<float> desc0(query.descriptors.begin(), query.descriptors.end());
    std::vector<float> desc1(asset.descriptors.begin(), asset.descriptors.end());

    const int64_t n0 = query.count();
    const int64_t n1 = asset.count();
    const int64_t dim = query.dim;

    try {
        const auto memory = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

        const std::array<int64_t, 3> kShape0{ 1, n0, 2 };
        const std::array<int64_t, 3> kShape1{ 1, n1, 2 };
        const std::array<int64_t, 3> dShape0{ 1, n0, dim };
        const std::array<int64_t, 3> dShape1{ 1, n1, dim };

        std::vector<Ort::Value> inputs;
        inputs.reserve(4);
        inputs.push_back(Ort::Value::CreateTensor<float>(memory, kpts0.data(), kpts0.size(),
                                                         kShape0.data(), kShape0.size()));
        inputs.push_back(Ort::Value::CreateTensor<float>(memory, kpts1.data(), kpts1.size(),
                                                         kShape1.data(), kShape1.size()));
        inputs.push_back(Ort::Value::CreateTensor<float>(memory, desc0.data(), desc0.size(),
                                                         dShape0.data(), dShape0.size()));
        inputs.push_back(Ort::Value::CreateTensor<float>(memory, desc1.data(), desc1.size(),
                                                         dShape1.data(), dShape1.size()));

        std::vector<const char *> inNames;
        for (const std::string &n : d->inputNames)
            inNames.push_back(n.c_str());
        std::vector<const char *> outNames;
        for (const std::string &n : d->outputNames)
            outNames.push_back(n.c_str());

        auto results = d->session->Run(Ort::RunOptions{ nullptr },
                                       inNames.data(), inputs.data(), inputs.size(),
                                       outNames.data(), outNames.size());

        const Ort::Value *matchValue = nullptr;
        const Ort::Value *scoreValue = nullptr;
        for (size_t i = 0; i < d->outputNames.size() && i < results.size(); ++i) {
            if (d->outputNames[i].find("match") != std::string::npos)  matchValue = &results[i];
            if (d->outputNames[i].find("score") != std::string::npos)  scoreValue = &results[i];
        }
        if (!matchValue)
            return out;

        const auto shape = matchValue->GetTensorTypeAndShapeInfo().GetShape();
        const int64_t pairs = shape.empty() ? 0 : shape.front();
        if (pairs <= 0)
            return out;

        const int64_t *pairData = matchValue->GetTensorData<int64_t>();
        const float   *scores   = scoreValue ? scoreValue->GetTensorData<float>() : nullptr;

        out.reserve(static_cast<int>(pairs));
        for (int64_t i = 0; i < pairs; ++i) {
            FeatureMatch m;
            m.queryIndex = static_cast<int>(pairData[i * 2]);
            m.assetIndex = static_cast<int>(pairData[i * 2 + 1]);
            m.score      = scores ? scores[i] : 1.0f;
            if (m.queryIndex < 0 || m.queryIndex >= query.count())
                continue;
            if (m.assetIndex < 0 || m.assetIndex >= asset.count())
                continue;
            out.append(m);
        }
    } catch (const std::exception &e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return {};
    }

    return out;
}

GeometryResult verifyHomography(const FeatureSet &query,
                                const FeatureSet &asset,
                                const QList<FeatureMatch> &matches,
                                const GeometryOptions &options)
{
    GeometryResult result;
    result.candidates = static_cast<int>(matches.size());

    // A homography needs four correspondences; fewer than the caller's minimum
    // is not worth fitting at all.
    if (matches.size() < std::max(4, options.minInliers))
        return result;

    std::vector<cv::Point2f> from; // asset
    std::vector<cv::Point2f> to;   // query
    from.reserve(matches.size());
    to.reserve(matches.size());

    for (const FeatureMatch &m : matches) {
        from.emplace_back(asset.keypoints.at(m.assetIndex * 2),
                          asset.keypoints.at(m.assetIndex * 2 + 1));
        to.emplace_back(query.keypoints.at(m.queryIndex * 2),
                        query.keypoints.at(m.queryIndex * 2 + 1));
    }

    try {
        std::vector<uchar> mask;
        const cv::Mat homography = cv::findHomography(
            from, to, cv::RANSAC, options.reprojectionThreshold, mask, options.maxIterations, 0.995);
        if (homography.empty())
            return result;

        result.inliers = static_cast<int>(std::count(mask.begin(), mask.end(), uchar(1)));
        result.inlierRatio = result.candidates > 0
                                 ? double(result.inliers) / result.candidates
                                 : 0.0;

        if (result.inliers < options.minInliers) {
            result.rejection = QStringLiteral("only %1 inliers").arg(result.inliers);
            return result;
        }
        if (result.inlierRatio < options.minInlierRatio) {
            result.rejection = QStringLiteral("inlier ratio %1")
                                   .arg(result.inlierRatio, 0, 'f', 2);
            return result;
        }

        // Project the asset's own rectangle to show where it landed.
        std::vector<cv::Point2f> corners{
            { 0.0f, 0.0f },
            { float(asset.imageWidth), 0.0f },
            { float(asset.imageWidth), float(asset.imageHeight) },
            { 0.0f, float(asset.imageHeight) }
        };
        std::vector<cv::Point2f> projected;
        cv::perspectiveTransform(corners, projected, homography);

        for (const cv::Point2f &p : projected)
            result.outline << QPointF(p.x, p.y);
        result.boundingBox = result.outline.boundingRect();

        if (options.checkShape) {
            result.rejection = shapeRejection(result.outline, query, asset, options);
            if (!result.rejection.isEmpty())
                return result;
        }
        result.ok = true;
    } catch (const std::exception &) {
        return result;
    }

    return result;
}

#else // IMAGEWORKER_WITH_INFERENCE

struct FeatureMatcher::Impl {};

FeatureMatcher::FeatureMatcher() : d(std::make_unique<Impl>()) {}
FeatureMatcher::~FeatureMatcher() = default;
QString FeatureMatcher::provider() const { return QStringLiteral("none"); }
void FeatureMatcher::setNormalizeKeypoints(bool) {}
bool FeatureMatcher::normalizeKeypoints() const { return false; }

std::unique_ptr<FeatureMatcher> FeatureMatcher::create(const QString &, bool, QString *error)
{
    if (error)
        *error = QStringLiteral("built without inference support");
    return nullptr;
}

QList<FeatureMatch> FeatureMatcher::match(const FeatureSet &, const FeatureSet &, QString *error)
{
    if (error)
        *error = QStringLiteral("built without inference support");
    return {};
}

GeometryResult verifyHomography(const FeatureSet &, const FeatureSet &,
                                const QList<FeatureMatch> &, const GeometryOptions &)
{
    return {};
}

#endif // IMAGEWORKER_WITH_INFERENCE

} // namespace iw
