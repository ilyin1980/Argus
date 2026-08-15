/**
 * @file Paths.h
 * @brief Root normalisation and the on-disk layout of ImageWorker state.
 *
 * Every path stored in the database is relative to an indexed root, so that a
 * database produced on the Linux worker stays meaningful on a Windows or macOS
 * client that mounts the same storage under a different prefix.
 */
#pragma once

#include <QString>

namespace iw {

/**
 * @brief Directory ImageWorker keeps its state in.
 * @param root Indexed root directory.
 * @return <tt>&lt;root&gt;/.imageworker</tt>, '/'-separated.
 */
QString stateDirFor(const QString &root);

/**
 * @brief Default database location for a root, creating the state directory.
 * @param root  Indexed root directory.
 * @param error Optional out-parameter receiving a failure description.
 * @return Path to @c index.db, or an empty string if the directory could not be created.
 */
QString defaultDatabasePath(const QString &root, QString *error = nullptr);

/**
 * @brief Canonical form of a root path.
 * @param root Directory in any platform notation.
 * @return Absolute, '/'-separated path without a trailing slash.
 */
QString normalizeRoot(const QString &root);

/**
 * @brief Rebuild an absolute path from a stored relative one.
 * @param root Indexed root directory.
 * @param rel  Path relative to that root.
 * @return Absolute, '/'-separated path.
 */
QString absolutePathFor(const QString &root, const QString &rel);

/**
 * @brief Test whether a relative path lives inside the state directory.
 * @param rel Path relative to an indexed root.
 * @return @c true for paths that must never be indexed.
 */
bool isStatePath(const QString &rel);

/**
 * @brief Locate the directory holding the ONNX models.
 *
 * Searched in order: a @c models directory beside the executable, then the same
 * name in each of its parent directories up to four levels — which covers a
 * build tree, where the binary sits in @c build/<preset>/bin. Falls back to a
 * plain relative @c models so the error message a caller prints stays sensible.
 *
 * @return Directory path, '/'-separated.
 * @note Without this the tool only works when the current directory happens to
 *       be the project root, which is a poor thing to require of a GUI.
 */
QString defaultModelsDir();

/**
 * @brief Open the platform file manager with a file selected.
 * @param absPath Absolute path of the file to reveal.
 * @return @c true if the file manager was launched.
 */
bool revealInFileManager(const QString &absPath);

} // namespace iw
