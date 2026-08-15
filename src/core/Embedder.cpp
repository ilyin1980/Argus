#include "core/Embedder.h"

namespace argus {

bool inferenceAvailable()
{
#ifdef ARGUS_WITH_INFERENCE
    return true;
#else
    return false;
#endif
}

std::unique_ptr<IEmbedder> createEmbedder(const QString &modelPath, QString *error)
{
#ifdef ARGUS_WITH_INFERENCE
    // P2: construct the ONNX Runtime session here, selecting the execution
    // provider in order CUDA -> DirectML -> CPU.
    Q_UNUSED(modelPath)
    if (error)
        *error = QStringLiteral("inference backend not implemented yet");
    return nullptr;
#else
    Q_UNUSED(modelPath)
    if (error) {
        *error = QStringLiteral(
            "this build has no inference support; reconfigure with -DARGUS_WITH_INFERENCE=ON");
    }
    return nullptr;
#endif
}

} // namespace argus
