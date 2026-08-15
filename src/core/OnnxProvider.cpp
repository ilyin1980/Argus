#include "core/OnnxProvider.h"

#ifdef IMAGEWORKER_WITH_INFERENCE

#if defined(_WIN32)
#include <dml_provider_factory.h>
#elif defined(__APPLE__)
#include <coreml_provider_factory.h>
#endif

#include <algorithm>
#include <string>
#include <vector>

namespace iw::onnx {

namespace {

/**
 * @brief Whether the runtime was built with a given execution provider.
 *
 * Asking first matters: appending a provider the build does not carry still
 * tries to dlopen its library and prints a page of red error text before
 * failing, on every single run.
 */
bool providerAvailable(const char *name)
{
    static const std::vector<std::string> available = Ort::GetAvailableProviders();
    return std::find(available.begin(), available.end(), std::string(name)) != available.end();
}

} // namespace

QString configureProvider(Ort::SessionOptions &options, bool preferGpu, QString *note)
{
    const auto refuse = [note](const QString &reason) {
        if (note)
            *note = reason;
        return QStringLiteral("CPU");
    };

    if (!preferGpu)
        return QStringLiteral("CPU");

#if defined(_WIN32)
    if (!providerAvailable("DmlExecutionProvider"))
        return refuse(QStringLiteral("this ONNX Runtime build has no DirectML provider"));

    // DirectML supports neither the memory-pattern optimiser nor parallel
    // execution; setting them anyway makes session creation fail outright.
    options.DisableMemPattern();
    options.SetExecutionMode(ORT_SEQUENTIAL);

    const OrtStatus *status = OrtSessionOptionsAppendExecutionProvider_DML(options, 0);
    if (status == nullptr)
        return QStringLiteral("DirectML");

    const QString reason = QString::fromUtf8(Ort::GetApi().GetErrorMessage(status));
    Ort::GetApi().ReleaseStatus(const_cast<OrtStatus *>(status));

    // Undo the DirectML-only constraints before falling back, so the CPU
    // provider is not left running with them.
    options = Ort::SessionOptions{};
    return refuse(reason);

#elif defined(__APPLE__)
    // CoreML routes what it can to the Neural Engine or the GPU and leaves the
    // rest on the CPU, so a partial match is still a win rather than an error.
    const OrtStatus *status =
        OrtSessionOptionsAppendExecutionProvider_CoreML(options, COREML_FLAG_USE_NONE);
    if (status == nullptr)
        return QStringLiteral("CoreML");

    const QString reason = QString::fromUtf8(Ort::GetApi().GetErrorMessage(status));
    Ort::GetApi().ReleaseStatus(const_cast<OrtStatus *>(status));
    options = Ort::SessionOptions{};
    return refuse(reason);

#elif defined(__linux__)
    if (!providerAvailable("CUDAExecutionProvider"))
        return refuse(QStringLiteral("this ONNX Runtime build has no CUDA provider"));

    try {
        OrtCUDAProviderOptions cuda{};
        cuda.device_id = 0;
        options.AppendExecutionProvider_CUDA(cuda);
        return QStringLiteral("CUDA");
    } catch (const Ort::Exception &e) {
        // Present in the build but unusable here — missing CUDA or cuDNN
        // libraries, a version mismatch, or no visible device. The CPU provider
        // is always there, so report why and carry on.
        options = Ort::SessionOptions{};
        return refuse(QString::fromUtf8(e.what()));
    }

#else
    return QStringLiteral("CPU");
#endif
}

} // namespace iw::onnx

#endif // IMAGEWORKER_WITH_INFERENCE
