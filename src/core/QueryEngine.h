/**
 * @file QueryEngine.h
 * @brief Query-by-example over the stored perceptual hashes.
 *
 * This is the P0 answer to "find images like this one": exact Hamming ranking
 * over two 64-bit fingerprints. It catches rescales, re-encodes and small edits
 * but knows nothing about content — two different photographs of the same
 * subject will not match. Semantic search arrives in P2 with SigLIP embeddings
 * and an approximate-nearest-neighbour index; this interface is shaped so that
 * backend can slot in behind it.
 */
#pragma once

#include <QImage>
#include <QString>

#include "core/Types.h"

namespace argus {

class Database;

/** @brief Outcome of one query-by-example run. */
struct QueryResult {
    QList<Match> matches;      ///< Hits, best score first.
    int          scannedRows = 0; ///< Rows considered.
    quint64      queryPhash  = 0; ///< Fingerprint of the query image.
    quint64      queryDhash  = 0; ///< Second fingerprint of the query image.
};

/**
 * @brief Rank indexed images by similarity to a reference file.
 * @param db        Open database to search.
 * @param imagePath Absolute path of the reference image; need not be indexed.
 * @param options   Result count and distance cutoff.
 * @param error     Optional out-parameter receiving a failure description.
 * @return Ranked hits; empty when the reference could not be decoded.
 * @note @c options.alphaBackground must match the value used at index time,
 *       otherwise transparent images will not line up.
 */
QueryResult queryByImage(Database &db,
                         const QString &imagePath,
                         const QueryOptions &options,
                         QString *error = nullptr);

/**
 * @brief Rank indexed images by similarity to an in-memory reference.
 * @param db      Open database to search.
 * @param image   Reference image, e.g. pasted from the clipboard or cropped by the user.
 * @param options Result count and distance cutoff.
 * @param error   Optional out-parameter receiving a failure description.
 * @return Ranked hits; empty when @p image is null.
 */
QueryResult queryByImage(Database &db,
                         const QImage &image,
                         const QueryOptions &options,
                         QString *error = nullptr);

/**
 * @brief Rank indexed images against fingerprints computed elsewhere.
 * @param db      Open database to search.
 * @param phash   DCT perceptual hash of the reference.
 * @param dhash   Gradient hash of the reference.
 * @param options Result count and distance cutoff.
 * @return Ranked hits.
 * @note Lets the GUI re-query from an already-indexed row without touching disk.
 */
QueryResult queryByHashes(Database &db,
                          quint64 phash,
                          quint64 dhash,
                          const QueryOptions &options);

} // namespace argus
