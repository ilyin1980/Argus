/**
 * @file Scanner.h
 * @brief Filesystem walk producing indexable candidates.
 */
#pragma once

#include <QSet>
#include <QStringList>

#include <atomic>
#include <functional>

#include "core/Types.h"

namespace iw {

/** @brief Filters applied while walking a tree. */
struct ScanOptions {
    QStringList extensions;             ///< Lowercase, no dots; empty means png/jpg.
    bool        recursive      = true;  ///< Descend into subdirectories.
    qint64      maxBytes       = 0;     ///< Skip files larger than this; 0 means no limit.
    bool        followSymlinks = false; ///< Follow symlinked directories.
};

/**
 * @brief Walk a directory tree collecting candidate image files.
 * @param root     Directory to walk.
 * @param options  Extension filter, recursion and size limits.
 * @param cancel   Optional flag polled during the walk; may be @c nullptr.
 * @param progress Optional callback invoked periodically with the running match count.
 * @return Candidates in iteration order; empty if @p root is not a directory.
 * @note The @c .imageworker state directory is always excluded.
 */
QList<ScannedFile> scanDirectory(const QString &root,
                                 const ScanOptions &options,
                                 const std::atomic_bool *cancel = nullptr,
                                 const std::function<void(int)> &progress = {});

} // namespace iw
