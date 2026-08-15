/**
 * @file PerceptualHash.h
 * @brief Two independent 64-bit perceptual fingerprints.
 *
 * Both hashes survive rescaling and re-encoding, but they fail in different
 * ways — the DCT hash is weak on flat images, the gradient hash on smooth
 * gradients — so ImageWorker stores and compares both.
 */
#pragma once

#include <QImage>

#include "core/Types.h"

namespace iw {

/**
 * @brief DCT-based perceptual hash, the classic pHash.
 *
 * Runs a 2D DCT-II over the 32x32 input, keeps the top-left 8x8 block of low
 * frequencies, drops the DC term and thresholds the remaining 63 coefficients
 * at their median. Robust to rescaling, re-encoding, mild colour shifts and
 * watermarks.
 *
 * @param gray32 Input image; must be 32x32 and QImage::Format_Grayscale8.
 * @return 64-bit hash, or 0 if the input does not meet the requirements.
 * @note Bit 0 is always clear: the DC coefficient carries only brightness.
 */
quint64 dctPerceptualHash(const QImage &gray32);

/**
 * @brief Gradient (difference) hash: one bit per horizontal neighbour comparison.
 * @param gray9x8 Input image; must be 9x8 and QImage::Format_Grayscale8.
 * @return 64-bit hash, or 0 if the input does not meet the requirements.
 */
quint64 differenceHash(const QImage &gray9x8);

} // namespace iw
