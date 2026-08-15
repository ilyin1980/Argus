/**
 * @file Embedder.h
 * @brief Seam for the P2 neural backend; no implementation yet.
 *
 * Perceptual hashing answers "is this the same picture". Embeddings answer
 * "is this a similar picture" and "does this picture match this sentence".
 * The plan is a SigLIP image/text encoder exported to ONNX driving one global
 * approximate-nearest-neighbour index, with DINOv2 kept as a reranker over
 * near-duplicate candidates only.
 *
 * The interface lives here from day one so that neither the CLI nor the GUI
 * has to change shape when the backend lands.
 */
#pragma once

#include <QImage>
#include <QList>
#include <QString>
#include <QVector>

#include <memory>

namespace argus {

/** @brief Produces fixed-length float vectors from images, and later from text. */
class IEmbedder
{
public:
    virtual ~IEmbedder() = default;

    /** @return Stable identifier of the model, used to key cached vectors. */
    virtual QString modelId() const = 0;

    /** @return Length of every vector this model produces. */
    virtual int dimensions() const = 0;

    /**
     * @brief Embed a batch of images.
     * @param images Preprocessed images, one per result.
     * @return L2-normalised vectors, in the same order; empty on failure.
     */
    virtual QList<QVector<float>> embedImages(const QList<QImage> &images) = 0;

    /**
     * @brief Embed a natural-language query.
     * @param text Query string.
     * @return L2-normalised vector, or empty if the model has no text tower.
     */
    virtual QVector<float> embedText(const QString &text) = 0;
};

/**
 * @brief Construct the configured embedding backend.
 * @param modelPath Path to the ONNX model directory or file.
 * @param error     Optional out-parameter receiving a failure description.
 * @return An embedder, or @c nullptr when this build has no inference support.
 * @note Returns @c nullptr unless the project was configured with
 *       @c ARGUS_WITH_INFERENCE.
 */
std::unique_ptr<IEmbedder> createEmbedder(const QString &modelPath, QString *error = nullptr);

/** @return @c true when this build can run neural models at all. */
bool inferenceAvailable();

} // namespace argus
