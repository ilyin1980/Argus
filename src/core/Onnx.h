/**
 * @file Onnx.h
 * @brief Capability probing for the neural backend.
 *
 * Everything here degrades gracefully: in a build configured without
 * @c IMAGEWORKER_WITH_INFERENCE the functions still exist and report that the
 * backend is absent, so callers never need to be compiled two ways.
 */
#pragma once

#include <QList>
#include <QString>
#include <QStringList>

namespace iw {

/** @brief What the neural backend can do on this machine, right now. */
struct RuntimeInfo {
    bool        built = false;  ///< This build has inference support compiled in.
    QString     ortVersion;     ///< ONNX Runtime version string.
    QStringList providers;      ///< Execution providers the runtime exposes.
    QString     opencvVersion;  ///< OpenCV version string.

    /// Name of the accelerated provider this platform offers: DirectML on
    /// Windows, CoreML on macOS, CUDA on Linux — or "CPU" when none attached.
    QString gpuProvider;
    bool    gpuUsable = false; ///< The accelerated provider actually attached.
    QString gpuError;          ///< Why it did not, when it did not.
};

/**
 * @brief Report the state of the neural backend.
 * @return Populated description; check RuntimeInfo::built first.
 * @note Probing DirectML genuinely creates a session on the device rather than
 *       just listing providers, because a provider being listed says nothing
 *       about whether it works on the installed driver.
 */
RuntimeInfo probeRuntime();

/** @brief One input or output of a model. */
struct TensorSpec {
    QString      name;   ///< Tensor name as the model declares it.
    QString      type;   ///< Element type, e.g. "float32".
    QList<qint64> shape; ///< Dimensions; -1 marks a dynamic axis.
};

/** @brief Result of loading a model far enough to read its signature. */
struct ModelInfo {
    bool             ok = false;
    QString          error;
    QString          provider;   ///< Execution provider the probe session used.
    qint64           loadMs = 0; ///< Time taken to create the session.
    QList<TensorSpec> inputs;
    QList<TensorSpec> outputs;
};

/**
 * @brief Load a model and read its input and output signature.
 * @param modelPath   Path to the .onnx file.
 * @param preferDirectML Try the DirectML provider before falling back to CPU.
 * @return Signature and timing, or an error description.
 * @note Reading the signature is the cheapest way to confirm that a downloaded
 *       model is the variant we expect: a fused extractor+matcher and a bare
 *       extractor differ visibly in their inputs.
 */
ModelInfo inspectModel(const QString &modelPath, bool preferDirectML = true);

/**
 * @brief The matcher model that will actually run on this machine.
 *
 * The fp16 export is the fast one, but ONNX Runtime has no CPU kernel for its
 * packed-QKV attention: on a machine with no accelerated provider it fails on
 * every pair, which looks exactly like "nothing matched". Verified on both
 * Windows and Linux. Where the CPU is all there is, the CPU export is the only
 * working choice.
 *
 * @param modelsDir Directory holding the .onnx files.
 * @return Absolute path of the matcher to load.
 */
QString preferredMatcherModel(const QString &modelsDir);

} // namespace iw
