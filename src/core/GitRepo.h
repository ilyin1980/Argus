/**
 * @file GitRepo.h
 * @brief Reading images out of a git repository without checking anything out.
 *
 * An asset folder inside a repository holds one version of each file: whatever
 * is checked out. The interesting question — "which branch has this artwork" —
 * cannot be answered from the working tree at all.
 *
 * Everything here goes through the `git` command line rather than a library.
 * That buys the user's own configuration for free: credentials, LFS smudge
 * filters, `core.autocrlf`, alternates, worktrees. A linked library would have
 * to reimplement each of those or quietly get them wrong.
 */
#pragma once

#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>

#include <memory>

class QProcess;

namespace argus::git {

/** @brief Where a repository lives, and where the indexed folder sits inside it. */
struct RepoInfo {
    bool    isRepo = false; ///< False when the folder is not inside a work tree.
    QString topLevel;       ///< Absolute path of the work tree root, '/'-separated.
    QString prefix;         ///< Indexed folder relative to @ref topLevel; empty at the root.
    QString currentRef;     ///< Checked-out branch; empty when the head is detached.
    QString gitDir;         ///< Absolute path of the repository directory.
    QString lfsDir;         ///< Local Git LFS object store, whether or not it exists.
    QString error;          ///< Why inspection failed, when it did.
};

/**
 * @brief Find out whether a directory is inside a git work tree.
 * @param directory Any directory, existing or not.
 * @return Populated info; check RepoInfo::isRepo before using the rest.
 * @note Never throws and never blocks for long: one short `git rev-parse`.
 *       A machine with no git installed reports @c isRepo false, which is the
 *       same answer a plain folder gives and needs no special handling.
 */
RepoInfo inspect(const QString &directory);

/**
 * @brief List the branches of a repository.
 * @param topLevel       Work tree root, from RepoInfo::topLevel.
 * @param includeRemotes Also list remote-tracking branches.
 * @param error          Optional failure description.
 * @return Local branches first, then remote-tracking ones, each in sorted order.
 */
QStringList branches(const QString &topLevel, bool includeRemotes, QString *error = nullptr);

/**
 * @brief Resolve a ref to the commit it points at.
 * @param topLevel Work tree root.
 * @param ref      Branch name, tag, or any revision git understands.
 * @param error    Optional failure description.
 * @return Full commit id, or an empty string when the ref does not resolve.
 * @note This is the cheap change check: a branch whose tip is unchanged since
 *       the last run cannot contain a changed file, so the whole ref is skipped
 *       without listing a single tree.
 */
QString resolve(const QString &topLevel, const QString &ref, QString *error = nullptr);

/** @brief One file as it exists in some commit. */
struct BlobEntry {
    QString rel;      ///< Path relative to the indexed folder, '/'-separated.
    QString oid;      ///< Blob object id; identical content shares one id.
    qint64  size = 0; ///< Size in bytes.
};

/**
 * @brief List the image blobs a ref holds under one directory.
 * @param topLevel   Work tree root.
 * @param ref        Branch, tag or commit.
 * @param prefix     Directory inside the repository; empty means the whole tree.
 * @param extensions Lowercase extensions without dots; empty means png/jpg.
 * @param error      Optional failure description.
 * @return Entries in git's own order, paths made relative to @p prefix.
 */
QList<BlobEntry> listBlobs(const QString &topLevel,
                           const QString &ref,
                           const QString &prefix,
                           const QStringList &extensions,
                           QString *error = nullptr);

/**
 * @brief Streams blob contents from one long-lived `git cat-file --batch`.
 *
 * Starting a process per file costs more than reading the file: on Windows a
 * process launch is milliseconds, and an asset library has thousands of blobs.
 * One process answers every request over a pipe instead.
 *
 * @note Not thread-safe by design — it owns a single pipe with a strict
 *       request/response order. Read serially, decode in parallel.
 */
class BlobReader
{
public:
    /**
     * @brief Start the batch process.
     * @param topLevel Work tree root.
     */
    explicit BlobReader(const QString &topLevel);
    ~BlobReader();

    BlobReader(const BlobReader &) = delete;
    BlobReader &operator=(const BlobReader &) = delete;

    /** @return @c true when the process started and is ready for requests. */
    bool isReady() const;

    /** @return Why the reader could not start, when it could not. */
    QString error() const;

    /**
     * @return How many blobs turned out to be Git LFS pointers whose object is
     *         not in the local store.
     *
     * Counted rather than reported per file: a repository that has never run
     * `git lfs fetch --all` produces one of these for every image, and the
     * useful message is the total plus what to run, not thousands of lines.
     */
    int missingLfsObjects() const;

    /**
     * @brief Fetch the bytes of one blob.
     * @param oid   Blob object id.
     * @param error Optional failure description.
     * @return File contents, or an empty array on failure.
     */
    QByteArray read(const QString &oid, QString *error = nullptr);

private:
    /**
     * @brief Turn a Git LFS pointer into the file it stands for.
     * @param pointer Contents of the pointer blob.
     * @param error   Optional failure description.
     * @return The real bytes, or empty when the object is not stored locally.
     *
     * Resolved from the repository's own LFS store only. Fetching would mean
     * pulling gigabytes over the network as a side effect of indexing a folder,
     * which is not a thing a tool should decide on its own.
     */
    QByteArray resolveLfs(const QByteArray &pointer, QString *error);

    std::unique_ptr<QProcess> m_process;
    QString                   m_error;
    QString                   m_lfsDir;
    int                       m_missingLfs = 0;
};

} // namespace argus::git
