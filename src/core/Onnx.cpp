#include "core/Onnx.h"

#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>

#ifdef ARGUS_WITH_INFERENCE
#include "core/OnnxProvider.h"

#include <opencv2/core.hpp>
#endif

namespace argus {

#ifdef ARGUS_WITH_INFERENCE

namespace {

/** @brief Human-readable name for an ONNX tensor element type. */
QString elementTypeName(ONNXTensorElementDataType type)
{
    switch (type) {
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:   return QStringLiteral("float32");
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16: return QStringLiteral("float16");
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE:  return QStringLiteral("float64");
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:    return QStringLiteral("int8");
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:   return QStringLiteral("uint8");
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:   return QStringLiteral("int32");
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:   return QStringLiteral("int64");
    case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:    return QStringLiteral("bool");
    default:                                    return QStringLiteral("type%1").arg(int(type));
    }
}

/** @brief Widen a path for the Windows-flavoured ORT session constructor. */
#if defined(_WIN32)
std::wstring nativePath(const QString &path) { return path.toStdWString(); }
#else
std::string nativePath(const QString &path) { return path.toStdString(); }
#endif

/** @brief Copy one tensor signature out of a type info handle. */
TensorSpec specOf(const QString &name, const Ort::TypeInfo &info)
{
    TensorSpec spec;
    spec.name = name;

    const auto shapeInfo = info.GetTensorTypeAndShapeInfo();
    spec.type = elementTypeName(shapeInfo.GetElementType());
    for (int64_t dim : shapeInfo.GetShape())
        spec.shape.append(static_cast<qint64>(dim));
    return spec;
}

} // namespace

RuntimeInfo probeRuntime()
{
    RuntimeInfo info;
    info.built         = true;
    info.ortVersion    = QString::fromUtf8(OrtGetApiBase()->GetVersionString());
    info.opencvVersion = QString::fromStdString(cv::getVersionString());

    for (const std::string &p : Ort::GetAvailableProviders())
        info.providers << QString::fromStdString(p);

    // Listing a provider proves nothing about the installed driver: ask for one
    // and see what actually attaches.
    try {
        // The env must exist before a provider is appended: without one, ORT
        // reports "Attempt to use DefaultLogger but none has been registered"
        // and the probe wrongly concludes there is no accelerator.
        Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "argus-probe");

        Ort::SessionOptions options;
        QString note;
        info.gpuProvider = onnx::configureProvider(options, true, &note);
        info.gpuUsable   = (info.gpuProvider != QLatin1String("CPU"));
        if (!info.gpuUsable) {
            info.gpuError = note.isEmpty()
                                ? QStringLiteral("no accelerated provider attached")
                                : note;
        }
    } catch (const std::exception &e) {
        info.gpuProvider = QStringLiteral("CPU");
        info.gpuError    = QString::fromUtf8(e.what());
    }

    return info;
}

QString preferredMatcherModel(const QString &modelsDir)
{
    const QString half = modelsDir + QStringLiteral("/disk_lightglue_fused_fp16.onnx");
    // Despite the name this export is not CPU-only — it is the full-precision
    // one, and it runs on every provider.
    const QString full = modelsDir + QStringLiteral("/disk_lightglue_fused_cpu.onnx");

    // Measured on every provider we have, because "there is a GPU" turned out
    // to be the wrong question:
    //   DirectML, RTX 3050 Ti : fp16 12.3 s vs full 15.5 s  -> fp16
    //   CUDA, GTX 1050 Ti     : fp16 13.3 s vs full  3.5 s  -> full, by 3.8x
    //   CoreML, M1 Pro        : fp16 returns ZERO matches in 4 ms, with no
    //                           error at all                -> full
    //   CPU                   : fp16 refuses to run         -> full
    // So the half-precision export is usable in exactly one place. If a new
    // card or provider appears, measure it rather than reasoning about it.
    const bool preferHalf = (probeRuntime().gpuProvider == QLatin1String("DirectML"));

    if (preferHalf && QFileInfo::exists(half))
        return half;
    if (QFileInfo::exists(full))
        return full;
    return half; // nothing else present; a failure will at least be reported
}

ModelInfo inspectModel(const QString &modelPath, bool preferDirectML)
{
    ModelInfo out;

    if (!QFileInfo::exists(modelPath)) {
        out.error = QStringLiteral("no such file: %1").arg(modelPath);
        return out;
    }

    QElapsedTimer timer;
    timer.start();

    try {
        Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "argus-inspect");
        Ort::SessionOptions options;
        out.provider = onnx::configureProvider(options, preferDirectML);

        Ort::Session session(env, nativePath(modelPath).c_str(), options);
        Ort::AllocatorWithDefaultOptions allocator;

        for (size_t i = 0; i < session.GetInputCount(); ++i) {
            const auto name = session.GetInputNameAllocated(i, allocator);
            out.inputs.append(specOf(QString::fromUtf8(name.get()), session.GetInputTypeInfo(i)));
        }
        for (size_t i = 0; i < session.GetOutputCount(); ++i) {
            const auto name = session.GetOutputNameAllocated(i, allocator);
            out.outputs.append(specOf(QString::fromUtf8(name.get()), session.GetOutputTypeInfo(i)));
        }

        out.loadMs = timer.elapsed();
        out.ok = true;
    } catch (const Ort::Exception &e) {
        out.error = QString::fromUtf8(e.what());
    } catch (const std::exception &e) {
        out.error = QString::fromUtf8(e.what());
    }

    return out;
}

#else // ARGUS_WITH_INFERENCE

RuntimeInfo probeRuntime()
{
    RuntimeInfo info;
    info.gpuProvider = QStringLiteral("none");
    info.gpuError    = QStringLiteral(
        "built without inference support; reconfigure with -DARGUS_WITH_INFERENCE=ON");
    return info;
}

ModelInfo inspectModel(const QString &modelPath, bool preferDirectML)
{
    Q_UNUSED(modelPath)
    Q_UNUSED(preferDirectML)
    ModelInfo out;
    out.error = QStringLiteral(
        "built without inference support; reconfigure with -DARGUS_WITH_INFERENCE=ON");
    return out;
}

QString preferredMatcherModel(const QString &modelsDir)
{
    return modelsDir + QStringLiteral("/disk_lightglue_fused_fp16.onnx");
}

#endif // ARGUS_WITH_INFERENCE

} // namespace argus
