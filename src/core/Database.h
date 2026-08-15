/**
 * @file Database.h
 * @brief SQLite store for file metadata, hashes and previews.
 *
 * The database holds metadata only. Embedding vectors will live in a flat
 * memory-mapped file alongside it (P2), because streaming millions of them
 * through SQLite pages would evict everything useful from the cache.
 *
 * @note One instance owns one connection and is **not** thread-safe. Open a
 *       separate instance per thread; WAL mode allows concurrent readers
 *       alongside a single writer.
 */
#pragma once

#include <QByteArray>
#include <QHash>
#include <QList>
#include <QPair>
#include <QSet>
#include <QSqlDatabase>
#include <QString>

#include <memory>

#include "core/Types.h"

class QSqlQuery;

namespace iw {

/** @brief Aggregate counters describing the whole index. */
struct DatabaseSummary {
    int    files      = 0; ///< Rows with FileStatus::Ok.
    int    failed     = 0; ///< Rows with FileStatus::Unreadable.
    int    thumbs     = 0; ///< Stored previews.
    qint64 totalBytes = 0; ///< Sum of file sizes.
};

/** @brief Index entry pointing at one image's descriptors in the flat store. */
struct FeatureRecord {
    qint64  fileId      = 0;
    QString model;              ///< Extractor identity; stale models are re-extracted.
    int     count       = 0;    ///< Keypoints stored.
    int     dim         = 0;    ///< Descriptor length.
    int     imageWidth  = 0;    ///< Image size the coordinates refer to.
    int     imageHeight = 0;
    qint64  descOffset  = -1;   ///< Byte offset into the descriptor file.
    qint64  kptsOffset  = -1;   ///< Byte offset into the keypoint file.
};

/** @brief One file still awaiting feature extraction. */
struct PendingFeature {
    qint64  fileId = 0;
    QString rel;
    QString ref;  ///< Empty for the working tree, otherwise the branch to read from.
    QString blob; ///< Git object id, when @ref ref is set.
};

/** @brief Owns one SQLite connection and every statement ImageWorker issues. */
class Database
{
public:
    Database();
    ~Database();

    Database(const Database &)            = delete;
    Database &operator=(const Database &) = delete;

    /**
     * @brief Open (creating if needed) a database file and apply the schema.
     * @param path  Database file path; parent directories must exist.
     * @param error Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool open(const QString &path, QString *error = nullptr);

    /** @brief Release statements and close the connection. Safe to call twice. */
    void close();

    /** @return @c true while the connection is usable. */
    bool isOpen() const;

    /** @return Path passed to @ref open, or an empty string. */
    QString path() const { return m_path; }

    /**
     * @brief Start a write transaction.
     * @param error Optional out-parameter receiving a failure description.
     * @return @c true on success.
     * @note Batching writes matters: SQLite commits one transaction per fsync.
     */
    bool beginTransaction(QString *error = nullptr);

    /**
     * @brief Commit the current transaction.
     * @param error Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool commit(QString *error = nullptr);

    /**
     * @brief Insert or update the row for one file, plus its preview.
     * @param record Fully populated index record.
     * @param error  Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool upsert(const IndexRecord &record, QString *error = nullptr);

    /**
     * @brief Load the size and mtime of every known file.
     * @return Map from relative path to <tt>(size, mtime)</tt>.
     * @note This is what makes re-indexing incremental; it is the one place
     *       where the full path set is held in memory.
     */
    QHash<QString, QPair<qint64, qint64>> loadSignatures();

    /**
     * @brief Load the blob id recorded for every file of one ref.
     * @param ref Branch name; the working tree is not addressed this way.
     * @return Map from relative path to blob object id.
     * @note This is the incremental check for a branch. Blob ids are content
     *       addresses, so an unchanged id means unchanged bytes — a stronger
     *       statement than size and mtime can make about a file on disk.
     */
    QHash<QString, QString> loadBlobs(const QString &ref);

    /**
     * @brief Every ref that has rows in this index.
     * @return Branch names; the empty string stands for the working tree.
     */
    QStringList refs();

    /**
     * @brief Delete every row belonging to one ref.
     * @param ref   Branch name.
     * @param error Optional out-parameter receiving a failure description.
     * @return Number of rows removed, or -1 on failure.
     */
    int removeRef(const QString &ref, QString *error = nullptr);

    /**
     * @brief Delete rows whose file is no longer on disk.
     * @param present Relative paths seen by the current scan.
     * @param error   Optional out-parameter receiving a failure description.
     * @return Number of rows removed, or -1 on failure.
     */
    int pruneMissing(const QSet<QString> &present, QString *error = nullptr);

    /**
     * @brief Delete rows of one ref whose path is no longer in that ref.
     * @param ref     Branch name.
     * @param present Relative paths the ref currently holds.
     * @param error   Optional out-parameter receiving a failure description.
     * @return Number of rows removed, or -1 on failure.
     */
    int pruneMissingInRef(const QString &ref,
                          const QSet<QString> &present,
                          QString *error = nullptr);

    /**
     * @brief Load every decodable row in the compact, path-free form.
     * @return Rows suitable for whole-database grouping passes.
     */
    QList<CompactRow> loadCompactRows();

    /**
     * @brief Materialise full rows, including paths, for specific ids.
     * @param ids Primary keys to fetch.
     * @return Rows in database order; missing ids are silently absent.
     */
    QList<FileInfoRow> filesByIds(const QList<qint64> &ids);

    /**
     * @brief Fetch one stored preview.
     * @param id Primary key.
     * @return Encoded image bytes, or empty if none is stored.
     */
    QByteArray thumbnail(qint64 id);

    /**
     * @brief Fetch the full content digest of one row.
     * @param id Primary key.
     * @return 32-byte digest, or empty if unknown.
     */
    QByteArray contentHash(qint64 id);

    /**
     * @brief Record where one image's descriptors live.
     * @param record Fully populated location.
     * @param error  Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool upsertFeatures(const FeatureRecord &record, QString *error = nullptr);

    /**
     * @brief Look up one image's descriptor location.
     * @param fileId Primary key in the @c files table.
     * @param model  Extractor identity to match.
     * @param out    Out-parameter receiving the record.
     * @return @c true when a record exists.
     */
    bool featuresFor(qint64 fileId, const QString &model, FeatureRecord *out);

    /**
     * @brief List decodable files that have no descriptors for this extractor yet.
     * @param model Extractor identity.
     * @return Files needing extraction, in database order.
     */
    QList<PendingFeature> filesWithoutFeatures(const QString &model);

    /**
     * @brief Every descriptor location recorded for one extractor.
     * @param model Extractor identity.
     * @return Records, in database order.
     */
    QList<FeatureRecord> allFeatures(const QString &model);

    /**
     * @brief Drop every descriptor location.
     * @return Number of rows removed, or -1 on failure.
     * @note Only clears the index; truncating the flat files is DescriptorStore's job.
     */
    int clearFeatures();

    /**
     * @brief Files whose descriptor count falls below a threshold.
     * @param model    Extractor identity.
     * @param maxCount Return files with at most this many keypoints.
     * @return Rows ordered by keypoint count, sparsest first.
     * @note These are the images local features cannot locate — flat colours,
     *       gradients, tiny icons — and therefore the ones that need a second
     *       matching channel.
     */
    QList<FileInfoRow> filesWithFewFeatures(const QString &model, int maxCount);

    /** @return Aggregate counters for the whole index. */
    DatabaseSummary summary();

    /**
     * @brief Read a value from the key/value @c meta table.
     * @param key Key to look up.
     * @return Stored value, or an empty string.
     */
    QString metaValue(const QString &key);

    /**
     * @brief Write a value into the key/value @c meta table.
     * @param key   Key to write.
     * @param value Value to store.
     * @return @c true on success.
     */
    bool setMetaValue(const QString &key, const QString &value);

private:
    bool applySchema(QString *error);

    /**
     * @brief Rebuild a version-1 @c files table so it can hold git refs.
     * @param error Optional out-parameter receiving a failure description.
     * @return @c true when the database is already current or was migrated.
     */
    bool migrateToRefs(QString *error);

    /**
     * @brief Fix tables left pointing at a table that no longer exists.
     * @param error Optional out-parameter receiving a failure description.
     * @return @c true when nothing needed fixing or the fix succeeded.
     *
     * A rename-first migration made @c thumbs and @c features reference
     * @c files_v1, which the same migration then dropped. Nothing complains
     * until something writes to those tables, so the repair runs on open.
     */
    bool repairDanglingReferences(QString *error);

    bool prepareStatements(QString *error);

    QString      m_connectionName;
    QString      m_path;
    QSqlDatabase m_db;

    std::unique_ptr<QSqlQuery> m_upsertFile;
    std::unique_ptr<QSqlQuery> m_selectId;
    std::unique_ptr<QSqlQuery> m_upsertThumb;
    std::unique_ptr<QSqlQuery> m_deleteThumb;
};

} // namespace iw
