/**
 * @file Vocabulary.h
 * @brief Visual vocabulary: descriptors quantised into "visual words".
 *
 * Matching a query against every indexed asset with a neural matcher does not
 * scale — thousands of model runs per query. A bag-of-words index solves the
 * cheap half of the problem: quantise each descriptor to its nearest cluster
 * centre, describe every image as a weighted histogram of those clusters, and
 * rank candidates by histogram similarity. Only the survivors go on to the
 * expensive geometric matcher.
 *
 * The vocabulary is trained by k-means over a sample of the indexed
 * descriptors, so it adapts to the actual asset library rather than to some
 * generic photo corpus.
 */
#pragma once

#include <QList>
#include <QString>

#include <memory>

namespace iw {

/** @brief Training parameters for the visual vocabulary. */
struct VocabularyOptions {
    int     words             = 2048;   ///< Cluster count.
    int     sampleDescriptors = 120000; ///< Cap on descriptors used for training.
    int     iterations        = 12;     ///< k-means iterations.
    double  epsilon           = 1.0;    ///< k-means convergence threshold.
    quint32 seed              = 20260815u; ///< Fixed so a rebuild is reproducible.
};

/** @brief A trained set of cluster centres plus nearest-centre lookup. */
class Vocabulary
{
public:
    Vocabulary();
    ~Vocabulary();

    Vocabulary(const Vocabulary &)            = delete;
    Vocabulary &operator=(const Vocabulary &) = delete;

    /** @return @c true once centres are loaded or trained. */
    bool isValid() const;

    /** @return Number of visual words. */
    int words() const;

    /** @return Descriptor dimensionality the vocabulary was trained on. */
    int dim() const;

    /** @return Extractor identity the vocabulary belongs to. */
    QString modelId() const;

    /**
     * @brief Train a vocabulary by k-means over a descriptor sample.
     * @param sample  Flat buffer of <tt>n * dim</tt> floats.
     * @param dim     Descriptor length.
     * @param modelId Extractor identity to record alongside the centres.
     * @param options Cluster count and iteration limits.
     * @param error   Optional out-parameter receiving a failure description.
     * @return A trained vocabulary, or @c nullptr on failure.
     * @note The word count is reduced automatically when the sample is too
     *       small to support it; a cluster per handful of descriptors would
     *       describe noise rather than structure.
     */
    static std::unique_ptr<Vocabulary> train(const QList<float> &sample,
                                             int dim,
                                             const QString &modelId,
                                             const VocabularyOptions &options,
                                             QString *error = nullptr);

    /**
     * @brief Write the vocabulary to disk.
     * @param path  Destination file.
     * @param error Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool save(const QString &path, QString *error = nullptr) const;

    /**
     * @brief Read a vocabulary from disk.
     * @param path  Source file written by @ref save.
     * @param error Optional out-parameter receiving a failure description.
     * @return The vocabulary, or @c nullptr on failure.
     */
    static std::unique_ptr<Vocabulary> load(const QString &path, QString *error = nullptr);

    /**
     * @brief Quantise descriptors to their nearest visual words.
     * @param descriptors Flat buffer of <tt>count * dim()</tt> floats.
     * @param count       Number of descriptors.
     * @return One word id per descriptor; empty when the vocabulary is invalid.
     */
    QList<quint32> assign(const QList<float> &descriptors, int count) const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace iw
