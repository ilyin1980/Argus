#include "core/FeatureExtractor.h"

#include <QFileInfo>

#include <algorithm>
#include <numeric>
#include <vector>

#ifdef ARGUS_WITH_INFERENCE
#include "core/OnnxProvider.h"
#endif

namespace argus {

#ifdef ARGUS_WITH_INFERENCE

namespace {

/**
 * @brief DISK's UNet halves the resolution four times.
 * @note Feeding a size that is not a multiple of 16 makes the session fail at
 *       run time rather than at load time, so pad instead of trusting callers.
 */
constexpr int kSizeMultiple = 16;

int roundUpTo(int value, int multiple)
{
    return ((value + multiple - 1) / multiple) * multiple;
}

#if defined(_WIN32)
std::wstring nativePath(const QString &path) { return path.toStdWString(); }
#else
std::string nativePath(const QString &path) { return path.toStdString(); }
#endif

} // namespace

struct FeatureExtractor::Impl {
    Ort::Env                 env{ ORT_LOGGING_LEVEL_ERROR, "argus-features" };
    std::unique_ptr<Ort::Session> session;
    Ort::AllocatorWithDefaultOptions allocator;

    std::vector<std::string> inputNames;
    std::vector<std::string> outputNames;
    QString provider = QStringLiteral("CPU");
    QString modelId;
};

FeatureExtractor::FeatureExtractor()
    : d(std::make_unique<Impl>())
{
}

FeatureExtractor::~FeatureExtractor() = default;

QString FeatureExtractor::provider() const { return d->provider; }
QString FeatureExtractor::modelId() const  { return d->modelId; }

std::unique_ptr<FeatureExtractor> FeatureExtractor::create(const QString &modelPath,
                                                           bool preferDirectML,
                                                           QString *error)
{
    if (!QFileInfo::exists(modelPath)) {
        if (error)
            *error = QStringLiteral("no such model: %1").arg(modelPath);
        return nullptr;
    }

    std::unique_ptr<FeatureExtractor> self(new FeatureExtractor);

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

        if (self->d->inputNames.size() != 1) {
            if (error)
                *error = QStringLiteral("expected a single input, model declares %1")
                             .arg(self->d->inputNames.size());
            return nullptr;
        }

        self->d->modelId = QFileInfo(modelPath).completeBaseName();
    } catch (const std::exception &e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return nullptr;
    }

    return self;
}

FeatureSet FeatureExtractor::extract(const QImage &image,
                                     const ExtractorOptions &options,
                                     QString *error)
{
    FeatureSet out;
    if (image.isNull()) {
        if (error)
            *error = QStringLiteral("empty image");
        return out;
    }

    out.imageWidth  = image.width();
    out.imageHeight = image.height();

    // ---- preprocess ---------------------------------------------------------
    // Downscale so the long side fits, then pad to the multiple the net needs.
    QImage work = image;
    double scale = 1.0;
    const int longSide = std::max(image.width(), image.height());
    if (options.maxSide > 0 && longSide > options.maxSide) {
        work = image.scaled(options.maxSide, options.maxSide,
                            Qt::KeepAspectRatio, Qt::SmoothTransformation);
        scale = static_cast<double>(image.width()) / std::max(1, work.width());
    }

    const QImage rgb = work.convertToFormat(QImage::Format_RGB888);
    if (rgb.isNull()) {
        if (error)
            *error = QStringLiteral("cannot convert to RGB888");
        return out;
    }

    const int validW = rgb.width();
    const int validH = rgb.height();
    const int padW = std::max(kSizeMultiple, roundUpTo(validW, kSizeMultiple));
    const int padH = std::max(kSizeMultiple, roundUpTo(validH, kSizeMultiple));

    // CHW, values in [0,1]; the padding stays zero and any keypoint that lands
    // in it is discarded below.
    std::vector<float> tensor(static_cast<size_t>(3) * padH * padW, 0.0f);
    const size_t plane = static_cast<size_t>(padH) * padW;
    for (int y = 0; y < validH; ++y) {
        const uchar *line = rgb.constScanLine(y);
        for (int x = 0; x < validW; ++x) {
            const size_t at = static_cast<size_t>(y) * padW + x;
            tensor[at]             = line[x * 3 + 0] / 255.0f;
            tensor[plane + at]     = line[x * 3 + 1] / 255.0f;
            tensor[2 * plane + at] = line[x * 3 + 2] / 255.0f;
        }
    }

    // ---- run ----------------------------------------------------------------
    std::vector<Ort::Value> results;
    try {
        const auto memory = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
        const std::array<int64_t, 4> shape{ 1, 3, padH, padW };
        Ort::Value input = Ort::Value::CreateTensor<float>(
            memory, tensor.data(), tensor.size(), shape.data(), shape.size());

        std::vector<const char *> inNames{ d->inputNames.front().c_str() };
        std::vector<const char *> outNames;
        outNames.reserve(d->outputNames.size());
        for (const std::string &n : d->outputNames)
            outNames.push_back(n.c_str());

        results = d->session->Run(Ort::RunOptions{ nullptr },
                                  inNames.data(), &input, 1,
                                  outNames.data(), outNames.size());
    } catch (const std::exception &e) {
        if (error)
            *error = QString::fromUtf8(e.what());
        return out;
    }

    // ---- read outputs by declared name --------------------------------------
    const Ort::Value *kptsValue = nullptr;
    const Ort::Value *scoreValue = nullptr;
    const Ort::Value *descValue = nullptr;
    for (size_t i = 0; i < d->outputNames.size() && i < results.size(); ++i) {
        const std::string &name = d->outputNames[i];
        if (name.find("keypoint") != std::string::npos)   kptsValue  = &results[i];
        else if (name.find("score") != std::string::npos) scoreValue = &results[i];
        else if (name.find("desc") != std::string::npos)  descValue  = &results[i];
    }
    if (!kptsValue || !descValue) {
        if (error)
            *error = QStringLiteral("model outputs do not look like keypoints/descriptors");
        return out;
    }

    const auto kptShape  = kptsValue->GetTensorTypeAndShapeInfo().GetShape();
    const auto descShape = descValue->GetTensorTypeAndShapeInfo().GetShape();
    if (kptShape.size() < 2 || descShape.empty()) {
        if (error)
            *error = QStringLiteral("unexpected output rank");
        return out;
    }

    const int64_t detected = kptShape[kptShape.size() - 2];
    const int64_t dim      = descShape.back();
    if (detected <= 0 || dim <= 0)
        return out; // nothing found; not an error

    const int64_t *kpts = kptsValue->GetTensorData<int64_t>();
    const float   *desc = descValue->GetTensorData<float>();
    const float   *conf = scoreValue ? scoreValue->GetTensorData<float>() : nullptr;

    // ---- filter, rank, map back to original coordinates ---------------------
    struct Candidate { int index; float score; };
    std::vector<Candidate> keep;
    keep.reserve(static_cast<size_t>(detected));

    for (int64_t i = 0; i < detected; ++i) {
        const int px = static_cast<int>(kpts[i * 2 + 0]);
        const int py = static_cast<int>(kpts[i * 2 + 1]);
        if (px < 0 || py < 0 || px >= validW || py >= validH)
            continue; // landed in the padding

        const float score = conf ? conf[i] : 1.0f;
        if (score < options.minScore)
            continue;

        if (options.useAlphaMask && image.hasAlphaChannel()) {
            const int ox = std::min(image.width()  - 1, static_cast<int>(px * scale));
            const int oy = std::min(image.height() - 1, static_cast<int>(py * scale));
            if (qAlpha(image.pixel(ox, oy)) <= options.alphaThreshold)
                continue; // a point on transparent background describes nothing
        }

        keep.push_back({ static_cast<int>(i), score });
    }

    std::sort(keep.begin(), keep.end(), [](const Candidate &a, const Candidate &b) {
        if (a.score != b.score)
            return a.score > b.score;
        return a.index < b.index;
    });
    if (options.maxKeypoints > 0 && keep.size() > static_cast<size_t>(options.maxKeypoints))
        keep.resize(static_cast<size_t>(options.maxKeypoints));

    out.dim = static_cast<int>(dim);
    out.keypoints.reserve(static_cast<int>(keep.size()) * 2);
    out.scores.reserve(static_cast<int>(keep.size()));
    out.descriptors.reserve(static_cast<int>(keep.size() * dim));

    for (const Candidate &c : keep) {
        out.keypoints.append(static_cast<float>(kpts[c.index * 2 + 0] * scale));
        out.keypoints.append(static_cast<float>(kpts[c.index * 2 + 1] * scale));
        out.scores.append(c.score);
        const float *row = desc + static_cast<size_t>(c.index) * dim;
        for (int64_t k = 0; k < dim; ++k)
            out.descriptors.append(row[k]);
    }

    return out;
}

#else // ARGUS_WITH_INFERENCE

struct FeatureExtractor::Impl {};

FeatureExtractor::FeatureExtractor() : d(std::make_unique<Impl>()) {}
FeatureExtractor::~FeatureExtractor() = default;
QString FeatureExtractor::provider() const { return QStringLiteral("none"); }
QString FeatureExtractor::modelId() const  { return {}; }

std::unique_ptr<FeatureExtractor> FeatureExtractor::create(const QString &modelPath,
                                                           bool preferDirectML,
                                                           QString *error)
{
    Q_UNUSED(modelPath)
    Q_UNUSED(preferDirectML)
    if (error) {
        *error = QStringLiteral(
            "built without inference support; reconfigure with -DARGUS_WITH_INFERENCE=ON");
    }
    return nullptr;
}

FeatureSet FeatureExtractor::extract(const QImage &image,
                                     const ExtractorOptions &options,
                                     QString *error)
{
    Q_UNUSED(image)
    Q_UNUSED(options)
    if (error)
        *error = QStringLiteral("built without inference support");
    return {};
}

#endif // ARGUS_WITH_INFERENCE

} // namespace argus
