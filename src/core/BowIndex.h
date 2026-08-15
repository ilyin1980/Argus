/**
 * @file BowIndex.h
 * @brief Inverted bag-of-words index used to shortlist candidates.
 *
 * Every indexed image becomes a sparse tf-idf histogram over the visual
 * vocabulary. Ranking a query against thousands of those histograms costs
 * microseconds, which is what makes it affordable to reserve the neural matcher
 * for the few dozen candidates that survive.
 */
#pragma once

#include <QList>
#include <QString>

#include <atomic>
#include <functional>
#include <memory>

namespace iw {

class Database;
class DescriptorStore;
class Vocabulary;

/** @brief One shortlisted candidate. */
struct BowHit {
    qint64 fileId = 0;
    float  score  = 0.0f; ///< Cosine similarity of the tf-idf histograms, 0..1.
};

/** @brief Size of a built index. */
struct BowStats {
    int    documents = 0;
    int    words     = 0;
    qint64 postings  = 0; ///< Total (word, document) pairs.
};

/** @brief Sparse tf-idf histograms plus the inverted lists over them. */
class BowIndex
{
public:
    BowIndex();
    ~BowIndex();

    BowIndex(const BowIndex &)            = delete;
    BowIndex &operator=(const BowIndex &) = delete;

    /** @return @c true once documents are loaded or built. */
    bool isValid() const;

    /** @return Counters describing the built index. */
    BowStats stats() const;

    /**
     * @brief Build histograms for every image that has descriptors.
     * @param db         Open database holding the descriptor locations.
     * @param store      Open descriptor store.
     * @param vocabulary Trained vocabulary.
     * @param model      Extractor identity to select records by.
     * @param cancel     Optional flag polled during the build.
     * @param progress   Optional callback invoked as <tt>(done, total)</tt>.
     * @param error      Optional out-parameter receiving a failure description.
     * @return The built index, or @c nullptr on failure.
     */
    static std::unique_ptr<BowIndex> build(Database &db,
                                           DescriptorStore &store,
                                           const Vocabulary &vocabulary,
                                           const QString &model,
                                           const std::atomic_bool *cancel = nullptr,
                                           const std::function<void(int, int)> &progress = {},
                                           QString *error = nullptr);

    /**
     * @brief Write the index to disk.
     * @param path  Destination file.
     * @param error Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool save(const QString &path, QString *error = nullptr) const;

    /**
     * @brief Read an index from disk.
     * @param path  Source file written by @ref save.
     * @param error Optional out-parameter receiving a failure description.
     * @return The index, or @c nullptr on failure.
     */
    static std::unique_ptr<BowIndex> load(const QString &path, QString *error = nullptr);

    /**
     * @brief Rank documents against a query's visual words.
     * @param queryWords Word ids of the query's descriptors, repeats included.
     * @param topK       Maximum candidates to return.
     * @return Candidates ordered by descending score.
     * @note Scores are cosine similarities between L2-normalised tf-idf
     *       histograms, so they are comparable across queries but say nothing
     *       about geometry — that is the matcher's job.
     */
    QList<BowHit> query(const QList<quint32> &queryWords, int topK) const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace iw
