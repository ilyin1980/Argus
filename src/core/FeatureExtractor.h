/**
 * @file FeatureExtractor.h
 * @brief Neural local-feature extraction (DISK) behind a Qt-shaped interface.
 *
 * Local features are what makes "find this object inside that screenshot"
 * possible: a global fingerprint or embedding describes the whole frame, and on
 * a screenshot the frame is mostly clutter. Keypoints are local, so the ones
 * lying on the object still match even when everything around them differs.
 */
#pragma once

#include <QImage>
#include <QList>
#include <QString>

#include <memory>

namespace argus {

/** @brief Keypoints and descriptors for one image. */
struct FeatureSet {
    int           imageWidth  = 0; ///< Size the coordinates refer to (the original image).
    int           imageHeight = 0;
    int           dim         = 0; ///< Descriptor length, 128 for DISK.
    QList<float>  keypoints;       ///< Two floats per point: x, y in original image pixels.
    QList<float>  scores;          ///< Detector confidence, one per point.
    QList<float>  descriptors;     ///< count() * dim floats, row-major.

    int  count() const { return static_cast<int>(scores.size()); }
    bool isEmpty() const { return scores.isEmpty(); }
};

/** @brief Knobs for one extraction. */
struct ExtractorOptions {
    int   maxKeypoints   = 512;   ///< Keep the highest-scoring points, drop the rest.
    int   maxSide        = 1024;  ///< Downscale the long side before inference.
    float minScore       = 0.0f;  ///< Discard points below this detector score.
    bool  useAlphaMask   = true;  ///< Drop points landing on transparent pixels.
    int   alphaThreshold = 16;    ///< Alpha at or below this counts as transparent.
};

/**
 * @brief Runs a DISK ONNX model to produce local features.
 *
 * @note One instance owns one inference session and is **not** thread-safe;
 *       serialise calls to @ref extract, or keep one instance per thread. On a
 *       single GPU serialising is the right answer anyway.
 * @note Constructing the session on the DirectML provider takes a few seconds
 *       while shaders compile, so create the extractor once and reuse it.
 */
class FeatureExtractor
{
public:
    ~FeatureExtractor();

    FeatureExtractor(const FeatureExtractor &)            = delete;
    FeatureExtractor &operator=(const FeatureExtractor &) = delete;

    /**
     * @brief Load a model and prepare an inference session.
     * @param modelPath      Path to the DISK .onnx file.
     * @param preferDirectML Try the GPU provider before falling back to the CPU.
     * @param error          Optional out-parameter receiving a failure description.
     * @return A ready extractor, or @c nullptr on failure.
     */
    static std::unique_ptr<FeatureExtractor> create(const QString &modelPath,
                                                    bool preferDirectML = true,
                                                    QString *error = nullptr);

    /**
     * @brief Extract features from one image.
     * @param image   Source image; an alpha channel is honoured when
     *                ExtractorOptions::useAlphaMask is set.
     * @param options Limits and filters.
     * @param error   Optional out-parameter receiving a failure description.
     * @return Features in the coordinate space of @p image; empty on failure.
     */
    FeatureSet extract(const QImage &image,
                       const ExtractorOptions &options,
                       QString *error = nullptr);

    /** @return Execution provider the session actually uses. */
    QString provider() const;

    /** @return Identifier stored alongside descriptors so stale ones are detected. */
    QString modelId() const;

private:
    FeatureExtractor();

    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace argus
