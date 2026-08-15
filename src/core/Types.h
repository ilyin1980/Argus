/**
 * @file Types.h
 * @brief Plain data structures shared by every ImageWorker module.
 *
 * Nothing here owns resources or touches Qt's object system, so these types can
 * cross thread boundaries freely and be copied without surprises.
 */
#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

#include <bit>
#include <cstdint>

namespace iw {

/** @brief Outcome of trying to read and decode one file. */
enum class FileStatus : int {
    Ok         = 0, ///< Decoded successfully.
    Unreadable = 1, ///< Present on disk but no image plugin could decode it.
};

/**
 * @brief One entry produced by the filesystem walk.
 * @note Nothing has been read from the file yet beyond its stat() data.
 */
struct ScannedFile {
    QString rel;         ///< Path relative to the indexed root, '/'-separated.
    qint64  size  = 0;   ///< File size in bytes.
    qint64  mtime = 0;   ///< Last modification time, seconds since epoch.
};

/** @brief Everything learned about a single file during indexing. */
struct IndexRecord {
    QString     rel;                        ///< Path relative to the indexed root.
    QString     ref;                        ///< Git branch, or empty for the working tree.
    QString     blob;                       ///< Git object id, set only when @ref ref is.
    qint64      size   = 0;                 ///< File size in bytes.
    qint64      mtime  = 0;                 ///< Last modification time, seconds since epoch.
    int         width  = 0;                 ///< Decoded pixel width.
    int         height = 0;                 ///< Decoded pixel height.
    QByteArray  contentHash;                ///< BLAKE2b-256 of the raw file bytes.
    quint64     phash  = 0;                 ///< DCT perceptual hash.
    quint64     dhash  = 0;                 ///< Gradient (difference) hash.
    QByteArray  thumb;                      ///< Encoded preview, may be empty.
    FileStatus  status = FileStatus::Ok;    ///< Decode outcome.
    QString     error;                      ///< Human-readable reason when @ref status is not Ok.
};

/**
 * @brief Slim row used for whole-database passes.
 * @note Deliberately free of QString so that millions of rows stay affordable
 *       in RAM: 40 bytes per file instead of ~150.
 */
struct CompactRow {
    qint64  id      = 0; ///< Primary key in the @c files table.
    qint64  size    = 0; ///< File size in bytes.
    qint64  chash64 = 0; ///< First 8 bytes of the content hash, 0 if unknown.
    quint64 phash   = 0; ///< DCT perceptual hash.
    quint64 dhash   = 0; ///< Gradient hash.
};

/** @brief Row enriched with the path, materialised only for results shown to a caller. */
struct FileInfoRow {
    qint64  id     = 0;  ///< Primary key in the @c files table.
    QString rel;         ///< Path relative to the indexed root.
    QString ref;         ///< Git branch this version came from; empty means the working tree.
    QString blob;        ///< Git object id, set only when @ref ref is.
    qint64  size   = 0;  ///< File size in bytes.
    qint64  mtime  = 0;  ///< Last modification time, seconds since epoch.
    int     width  = 0;  ///< Decoded pixel width.
    int     height = 0;  ///< Decoded pixel height.
    quint64 phash  = 0;  ///< DCT perceptual hash.
};

/** @brief One hit from a query-by-example search. */
struct Match {
    FileInfoRow file;         ///< The matched file.
    int         distance = 0; ///< Combined pHash+dHash Hamming distance, 0..128.
    double      score    = 0.0; ///< Normalised similarity, @c 1.0-distance/128.
};

/** @brief Whether a duplicate group is byte-identical or merely near-identical. */
enum class GroupKind {
    Exact, ///< Members share a content hash.
    Near,  ///< Members are within the perceptual-hash radius.
};

/** @brief A set of files considered duplicates of one another. */
struct DuplicateGroup {
    GroupKind          kind = GroupKind::Exact; ///< Exact or near.
    QList<FileInfoRow> files;                   ///< Members, largest file first.
    qint64             wastedBytes = 0;         ///< Total size minus the largest member.
    int                maxDistance = 0;         ///< Widest pairwise distance; 0 for exact groups.
};

/** @brief Knobs for one indexing run. */
struct IndexOptions {
    QString     root;                    ///< Directory to index.
    QString     dbPath;                  ///< Database file; empty means the default under the root.
    QStringList extensions;              ///< Lowercase, no dots; empty means png/jpg.
    int         jobs            = 0;     ///< Worker threads; 0 means hardware concurrency.
    bool        thumbnails      = true;  ///< Generate and store previews.
    int         thumbSize       = 256;   ///< Longest preview side, pixels.
    int         thumbQuality    = 75;    ///< Preview encoder quality, 0..100.
    bool        force           = false; ///< Re-read files whose size and mtime are unchanged.
    int         alphaBackground = 128;   ///< Grey level transparent pixels are flattened onto.
    qint64      maxFileBytes    = 0;     ///< Skip files larger than this; 0 means no limit.
    bool        prune           = true;  ///< Drop rows whose file has disappeared.

    /// @name Neural local features
    /// Second indexing pass; needed for finding an object inside a screenshot.
    /// @{
    /// Git branches to index besides the working tree. Empty means the working
    /// tree alone, which is what a folder outside a repository can offer.
    QStringList branches;

    /// Treat @ref branches as the complete set the index should hold, dropping
    /// any other branch already in it. Off by default so that an ordinary
    /// re-index of the working tree cannot silently throw branches away.
    bool        syncBranches = false;

    bool    extractFeatures    = false; ///< Run the extractor after hashing.
    QString featureModelPath;           ///< Path to the extractor .onnx file.
    int     featureMaxKeypoints = 512;  ///< Keypoints kept per image.
    int     featureMaxSide      = 1024; ///< Long side the extractor sees.
    bool    featureUseGpu       = true; ///< Prefer the DirectML provider.
    /// @}
};

/** @brief Counters describing what one indexing run did. */
struct IndexStats {
    int    scanned   = 0; ///< Candidate files found on disk.
    int    indexed   = 0; ///< Files read and hashed this run.
    int    skipped   = 0; ///< Unchanged since the previous run.
    int    failed    = 0; ///< Present but undecodable.
    int    pruned    = 0; ///< Rows removed because the file is gone.
    int    branchesIndexed = 0; ///< Git refs read this run.
    int    branchesSkipped = 0; ///< Refs whose tip had not moved since last time.
    qint64 bytesRead = 0; ///< Total bytes hashed.
    qint64 elapsedMs = 0; ///< Wall-clock duration.
    bool   cancelled = false; ///< Run stopped early on request.

    int    featured      = 0; ///< Images that got local features this run.
    int    featureFailed = 0; ///< Images the extractor could not handle.
    qint64 keypointsTotal = 0; ///< Keypoints stored this run.
    qint64 featureBytes  = 0; ///< Size of the descriptor store afterwards.
    QString featureProvider;  ///< Execution provider the extractor used.
};

/** @brief Knobs for duplicate detection. */
struct DuplicateOptions {
    int  maxDistance  = 4;    ///< Hamming radius on the 64-bit pHash; 0 means identical fingerprints.
    bool includeExact = true; ///< Report byte-identical groups.
    bool includeNear  = true; ///< Report perceptually similar groups.
    int  bucketLimit  = 2000; ///< Skip degenerate MIH buckets larger than this.
    /**
     * @brief Require the gradient hash to agree as well, not just the DCT hash.
     *
     * The two fingerprints fail differently, so demanding both cuts the most
     * common false-positive class — images that share a layout but not their
     * content, such as one character drawn with several facial expressions, or
     * the emission and specular maps of a single texture.
     */
    bool requireBothHashes = true;
};

/** @brief Result of a duplicate-detection pass. */
struct DuplicateReport {
    QList<DuplicateGroup> groups;             ///< Groups, largest wasted space first.
    qint64                wastedBytes    = 0; ///< Sum over all groups.
    int                   skippedBuckets = 0; ///< Oversized buckets not expanded; never silently swallowed.
    int                   comparedPairs  = 0; ///< Candidate pairs actually compared.
    bool                  cancelled      = false; ///< Pass stopped early on request.
};

/** @brief Knobs for query-by-example. */
struct QueryOptions {
    int topK            = 20;  ///< Maximum hits to return.
    int maxDistance     = 128; ///< Combined pHash+dHash distance cutoff.
    int alphaBackground = 128; ///< Must match the value used at index time.
};

/**
 * @brief Hamming distance between two 64-bit hashes.
 * @param a First hash.
 * @param b Second hash.
 * @return Number of differing bits, 0..64.
 */
inline int hamming(quint64 a, quint64 b) noexcept
{
    return std::popcount(a ^ b);
}

} // namespace iw
