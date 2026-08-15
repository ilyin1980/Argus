/**
 * @file OnnxProvider.h
 * @brief Execution-provider selection, in one place for every session we build.
 *
 * Private to the inference sources: including it requires the ONNX Runtime
 * headers, so it is only ever pulled in under @c ARGUS_WITH_INFERENCE.
 *
 * Three call sites used to carry their own copy of this logic, which is how the
 * DirectML-specific session options ended up correct in some of them and merely
 * plausible in others.
 */
#pragma once

#ifdef ARGUS_WITH_INFERENCE

#include <QString>

#include <onnxruntime_cxx_api.h>

namespace argus::onnx {

/**
 * @brief Attach the best execution provider this platform and build can offer.
 *
 * Tries the platform's GPU provider first and silently falls back to the CPU,
 * because a machine without the right driver is a normal situation rather than
 * a failure: DirectML on Windows, CoreML on macOS, CUDA on Linux.
 *
 * @param options   Session options to configure; modified in place.
 * @param preferGpu When false, go straight to the CPU provider.
 * @param note      Optional out-parameter receiving the reason an accelerated
 *                  provider was refused. Without it a machine that *has* the
 *                  provider but cannot use it looks identical to one that never
 *                  had it, which is a miserable thing to debug.
 * @return Name of the provider actually attached: "DirectML", "CoreML",
 *         "CUDA" or "CPU".
 *
 * @note DirectML rejects the memory-pattern optimiser and parallel execution;
 *       those options are applied here so no caller has to remember them.
 * @warning The macOS and Linux branches are written but **not yet verified on
 *          hardware** — there has been no machine to run them on. Treat the
 *          first run on either platform as a test, not as a regression.
 */
QString configureProvider(Ort::SessionOptions &options, bool preferGpu, QString *note = nullptr);

} // namespace argus::onnx

#endif // ARGUS_WITH_INFERENCE
