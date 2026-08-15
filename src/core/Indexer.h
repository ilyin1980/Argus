/**
 * @file Indexer.h
 * @brief The indexing pipeline: walk, decode, hash, persist.
 */
#pragma once

#include <QObject>

#include <atomic>

#include "core/Types.h"

namespace iw {

class Database;

/**
 * @brief Builds and refreshes the index for one root directory.
 *
 * The pipeline is scan → diff against the stored signatures → decode and hash
 * the changed files in parallel → write in batched transactions → prune rows
 * whose file disappeared. Files are re-read only when their size or mtime
 * changed, which is what keeps a re-run over millions of files cheap.
 *
 * @note @ref run blocks. Move an instance to a worker thread and connect the
 *       signals queued, or call it directly from a CLI process.
 */
class Indexer : public QObject
{
    Q_OBJECT

public:
    explicit Indexer(QObject *parent = nullptr);

    /**
     * @brief Run a full indexing pass.
     * @param options Root, database path and tuning knobs.
     * @param error   Optional out-parameter receiving a fatal failure description.
     * @return Counters describing the run; check IndexStats::cancelled.
     */
    IndexStats run(const IndexOptions &options, QString *error = nullptr);

    /** @brief Request cancellation from any thread. Idempotent. */
    void cancel();

    /** @brief Clear a previous cancellation so the instance can be reused. */
    void reset();

signals:
    /**
     * @brief Emitted as work advances.
     * @param done  Items completed in the current stage.
     * @param total Items in the current stage, or 0 while still counting.
     * @param stage Short human-readable stage name.
     */
    void progress(int done, int total, const QString &stage);

    /**
     * @brief Emitted for log-worthy events.
     * @param text Human-readable message.
     */
    void message(const QString &text);

private:
    /**
     * @brief Index the requested git branches beside the working tree.
     * @param db      Open database.
     * @param root    Indexed root; its repository is discovered from it.
     * @param options Which branches, and the decode settings to reuse.
     * @param stats   Counters updated in place.
     *
     * Blobs are read out of the object store rather than checked out, so the
     * working tree is never touched and no branch has to be switched to. The
     * cheap check is the branch tip: a ref whose tip has not moved since the
     * last run cannot hold a changed file, and is skipped without listing a
     * single tree.
     */
    void indexBranches(Database &db,
                       const QString &root,
                       const IndexOptions &options,
                       IndexStats &stats);

    /**
     * @brief Second pass: neural local features for every decodable image.
     * @param db      Open database, used for the descriptor index.
     * @param root    Indexed root, for rebuilding absolute paths.
     * @param dbPath  Database location; the descriptor files sit beside it.
     * @param options Extractor settings.
     * @param stats   Counters updated in place.
     * @note Silently does nothing when the model cannot be loaded: hashing has
     *       already succeeded and should not be lost because inference failed.
     */
    void extractFeaturePass(Database &db,
                            const QString &root,
                            const QString &dbPath,
                            const IndexOptions &options,
                            IndexStats &stats);

    std::atomic_bool m_cancel{false};
};

} // namespace iw
