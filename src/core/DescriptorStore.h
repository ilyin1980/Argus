/**
 * @file DescriptorStore.h
 * @brief Flat, memory-mapped storage for local-feature descriptors.
 *
 * Descriptors are bulky — 512 keypoints of 128 dimensions is 128 KB per image
 * in half precision — and they are always read in whole rows. Putting them in
 * SQLite blobs would churn the page cache for no benefit, so they live in plain
 * append-only files and SQLite keeps only the offsets.
 */
#pragma once

#include <QString>

#include <memory>

#include "core/FeatureExtractor.h"

namespace argus {

/** @brief Where one image's features sit inside the flat files. */
struct FeatureLocation {
    qint64 descOffset = -1; ///< Byte offset into the descriptor file.
    qint64 kptsOffset = -1; ///< Byte offset into the keypoint file.
    int    count      = 0;  ///< Number of keypoints.
    int    dim        = 0;  ///< Descriptor length.
};

/**
 * @brief Append-only store of descriptors and keypoints.
 *
 * Descriptors are written as IEEE half precision, which halves the footprint
 * and costs nothing in matching quality — the matcher works in float anyway and
 * the values are already normalised into a small range.
 *
 * @note Append-only: re-indexing a changed file leaves its old bytes stranded.
 *       They are reclaimed only by @ref clear, which the indexer calls on a
 *       forced run. Ordinary incremental runs therefore grow the files slowly;
 *       @ref totalBytes makes that growth visible rather than silent.
 * @note One instance is **not** thread-safe.
 */
class DescriptorStore
{
public:
    DescriptorStore();
    ~DescriptorStore();

    DescriptorStore(const DescriptorStore &)            = delete;
    DescriptorStore &operator=(const DescriptorStore &) = delete;

    /**
     * @brief Open (creating if needed) the store inside a directory.
     * @param directory Directory that will hold @c desc.f16 and @c kpts.f32.
     * @param error     Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool open(const QString &directory, QString *error = nullptr);

    /** @brief Flush and close both files. Safe to call twice. */
    void close();

    /** @return @c true while the store is usable. */
    bool isOpen() const;

    /**
     * @brief Append one image's features.
     * @param features Extracted features; an empty set is written as a zero-count record.
     * @param location Out-parameter receiving the offsets to record in the database.
     * @param error    Optional out-parameter receiving a failure description.
     * @return @c true on success.
     */
    bool append(const FeatureSet &features, FeatureLocation *location, QString *error = nullptr);

    /**
     * @brief Read one image's features back.
     * @param location   Offsets previously produced by @ref append.
     * @param imageWidth Width recorded alongside the features.
     * @param imageHeight Height recorded alongside the features.
     * @return The features, or an empty set when the location is out of range.
     */
    FeatureSet load(const FeatureLocation &location, int imageWidth, int imageHeight) const;

    /** @brief Discard everything, leaving an empty store. */
    bool clear(QString *error = nullptr);

    /** @return Combined size of the two files in bytes. */
    qint64 totalBytes() const;

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

} // namespace argus
