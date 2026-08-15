#include "core/PerceptualHash.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>

namespace argus {

namespace {

constexpr int kDctSize  = 32;
constexpr int kBlock    = 8;

using CosTable = std::array<std::array<double, kDctSize>, kDctSize>;

const CosTable &cosineTable()
{
    // Magic-static: built once, thread-safe, ~8 KB.
    static const CosTable table = [] {
        CosTable t{};
        for (int u = 0; u < kDctSize; ++u)
            for (int x = 0; x < kDctSize; ++x)
                t[static_cast<size_t>(u)][static_cast<size_t>(x)] =
                    std::cos((2.0 * x + 1.0) * u * std::numbers::pi / (2.0 * kDctSize));
        return t;
    }();
    return table;
}

} // namespace

quint64 dctPerceptualHash(const QImage &gray32)
{
    if (gray32.width() != kDctSize || gray32.height() != kDctSize
        || gray32.format() != QImage::Format_Grayscale8) {
        return 0;
    }

    const CosTable &cs = cosineTable();

    static thread_local std::array<double, kDctSize * kDctSize> pixels{};
    static thread_local std::array<double, kDctSize * kDctSize> rows{};

    for (int y = 0; y < kDctSize; ++y) {
        const uchar *line = gray32.constScanLine(y);
        for (int x = 0; x < kDctSize; ++x)
            pixels[static_cast<size_t>(y * kDctSize + x)] = line[x];
    }

    // Separable DCT-II: rows first, then the 8 columns we actually need.
    for (int y = 0; y < kDctSize; ++y) {
        for (int u = 0; u < kBlock; ++u) {
            double sum = 0.0;
            for (int x = 0; x < kDctSize; ++x)
                sum += pixels[static_cast<size_t>(y * kDctSize + x)] * cs[static_cast<size_t>(u)][static_cast<size_t>(x)];
            rows[static_cast<size_t>(y * kDctSize + u)] = sum;
        }
    }

    std::array<double, kBlock * kBlock> block{};
    for (int u = 0; u < kBlock; ++u) {
        for (int v = 0; v < kBlock; ++v) {
            double sum = 0.0;
            for (int y = 0; y < kDctSize; ++y)
                sum += rows[static_cast<size_t>(y * kDctSize + v)] * cs[static_cast<size_t>(u)][static_cast<size_t>(y)];
            block[static_cast<size_t>(u * kBlock + v)] = sum;
        }
    }

    // Median over the 63 AC coefficients; the DC term carries only brightness.
    std::array<double, kBlock * kBlock - 1> sorted{};
    for (int i = 1; i < kBlock * kBlock; ++i)
        sorted[static_cast<size_t>(i - 1)] = block[static_cast<size_t>(i)];

    std::nth_element(sorted.begin(), sorted.begin() + sorted.size() / 2, sorted.end());
    const double median = sorted[sorted.size() / 2];

    quint64 hash = 0;
    for (int i = 0; i < kBlock * kBlock; ++i) {
        if (i == 0)
            continue; // DC bit stays 0 so identical images agree on it
        if (block[static_cast<size_t>(i)] > median)
            hash |= (quint64(1) << i);
    }
    return hash;
}

quint64 differenceHash(const QImage &gray9x8)
{
    if (gray9x8.width() != 9 || gray9x8.height() != 8
        || gray9x8.format() != QImage::Format_Grayscale8) {
        return 0;
    }

    quint64 hash = 0;
    int bit = 0;
    for (int y = 0; y < 8; ++y) {
        const uchar *line = gray9x8.constScanLine(y);
        for (int x = 0; x < 8; ++x, ++bit) {
            if (line[x] > line[x + 1])
                hash |= (quint64(1) << bit);
        }
    }
    return hash;
}

} // namespace argus
