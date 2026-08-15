/**
 * @file DuplicateFinder.h
 * @brief Grouping pass that turns stored hashes into duplicate sets.
 */
#pragma once

#include <atomic>
#include <functional>

#include "core/Types.h"

namespace argus {

class Database;

/**
 * @brief Find byte-identical and perceptually near-identical groups.
 *
 * Runs in two stages. Exact groups come from the content hash. Near groups are
 * found with multi-index hashing: the 64-bit pHash is split into four 16-bit
 * bands, and two hashes within Hamming distance @c d must agree exactly on at
 * least one band whenever <tt>d < 4</tt> — the pigeonhole principle. Buckets of
 * equal bands therefore give candidate pairs without comparing everything to
 * everything, which is what makes the pass viable on millions of rows.
 *
 * Exact duplicates are collapsed to one representative before the near pass, so
 * a folder holding a thousand copies of the same file does not explode into a
 * million candidate pairs.
 *
 * @param db       Open database to read from.
 * @param options  Distance radius and which kinds of group to report.
 * @param cancel   Optional flag polled during the pass; may be @c nullptr.
 * @param progress Optional callback invoked as <tt>(done, total, stage)</tt>.
 * @return Groups ordered by wasted space, plus honest counters for anything skipped.
 * @note Larger radii lose the pigeonhole guarantee: at @c maxDistance above 3
 *       the band buckets become a heuristic that can miss distant pairs.
 * @note A near group means "review these", not "delete these". Perceptual
 *       hashing describes layout, so a character drawn with several expressions,
 *       consecutive video frames, or the emission and specular maps of one
 *       texture all land within a small radius of each other.
 *       DuplicateOptions::requireBothHashes exists to cut exactly that class.
 * @note When both kinds are requested, an exact group whose members all appear
 *       in a near group is not reported separately, so DuplicateReport::wastedBytes
 *       never counts the same file twice.
 */
DuplicateReport findDuplicates(Database &db,
                               const DuplicateOptions &options,
                               const std::atomic_bool *cancel = nullptr,
                               const std::function<void(int, int, const QString &)> &progress = {});

} // namespace argus
