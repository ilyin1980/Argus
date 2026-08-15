/**
 * @file ContentHash.h
 * @brief Byte-exact file fingerprinting, the cheapest stage of the cascade.
 */
#pragma once

#include <QByteArray>
#include <QString>

namespace iw {

/**
 * @brief Streamed BLAKE2b-256 of a whole file.
 *
 * Zero external dependencies: Qt ships BLAKE2b in QCryptographicHash, and at
 * roughly a gigabyte per second the hash is never the bottleneck next to I/O.
 *
 * @param absPath   Absolute path of the file to hash.
 * @param bytesRead Optional out-parameter receiving the number of bytes read.
 * @return 32-byte digest, or an empty array if the file could not be read.
 */
QByteArray fileContentHash(const QString &absPath, qint64 *bytesRead = nullptr);

/**
 * @brief The same digest over bytes already in memory.
 * @param data File contents.
 * @return 32-byte digest.
 * @note Used for git blobs, which never touch the filesystem. Producing the
 *       identical digest is what lets a branch copy and a working-tree copy of
 *       the same file group together.
 */
QByteArray dataContentHash(const QByteArray &data);

/**
 * @brief Reduce a content hash to an integer SQLite can index and group by.
 * @param contentHash Digest produced by @ref fileContentHash.
 * @return First 8 bytes reinterpreted as a signed integer; 0 when unknown.
 * @note Grouping by this value yields candidates only; confirm with the full digest.
 */
qint64 shortHash(const QByteArray &contentHash);

} // namespace iw
