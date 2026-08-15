/**
 * @file ImageDecoder.h
 * @brief Single-pass decode that yields every derivative the indexer needs.
 *
 * Decoding dominates indexing cost, so a file is read exactly once and all
 * downstream inputs — both hash sources and the preview — are derived from that
 * one decode.
 */
#pragma once

#include <QByteArray>
#include <QImage>
#include <QString>
#include <QStringList>

namespace iw {

/** @brief What a caller wants out of one decode. */
struct DecodeRequest {
    bool wantThumbnail   = true; ///< Produce an encoded preview.
    int  thumbSize       = 256;  ///< Longest preview side, pixels.
    int  thumbQuality    = 75;   ///< Preview encoder quality, 0..100.
    int  alphaBackground = 128;  ///< Grey level transparent pixels are flattened onto.
};

/** @brief Everything one decode produced. */
struct DecodedImage {
    bool       ok      = false; ///< False when the file could not be decoded.
    int        width   = 0;     ///< Original pixel width, before any downscale.
    int        height  = 0;     ///< Original pixel height, before any downscale.
    QImage     gray32;          ///< 32x32 Grayscale8, input for @ref dctPerceptualHash.
    QImage     gray9x8;         ///< 9x8 Grayscale8, input for @ref differenceHash.
    QByteArray thumbnail;       ///< Encoded preview; empty when not requested or unsupported.
    QString    error;           ///< Failure description when @ref ok is false.
};

/**
 * @brief Decode one file and derive hash inputs and an optional preview.
 * @param absPath Absolute path of the image file.
 * @param req     What to produce.
 * @return Populated result; check DecodedImage::ok before use.
 * @note Honours the EXIF orientation tag, and asks the plugin for a reduced
 *       decode when the format supports it.
 * @note Safe to call from several threads at once.
 */
DecodedImage decodeForIndex(const QString &absPath, const DecodeRequest &req);

/**
 * @brief Derive the same products from an image already in memory.
 * @param image Source image; may carry an alpha channel.
 * @param req   What to produce.
 * @return Populated result; check DecodedImage::ok before use.
 * @note Used for clipboard pastes and for user-cropped regions, which never
 *       touch the filesystem.
 */
DecodedImage decodeFromImage(const QImage &image, const DecodeRequest &req);

/**
 * @brief File extensions indexed unless the caller overrides them.
 * @return Lowercase extensions without dots: png, jpg, jpeg.
 * @note Intersected with what this build can actually read, so a Qt install
 *       missing the JPEG plugin degrades honestly instead of failing per file.
 */
QStringList defaultExtensions();

/**
 * @brief Every image extension Qt can read in this build.
 * @return Lowercase extensions without dots, including common aliases.
 */
QStringList supportedExtensions();

/**
 * @brief Encode a preview image.
 * @param image   Source image.
 * @param maxSide Longest side of the output, pixels.
 * @param quality Encoder quality, 0..100.
 * @return Encoded bytes, or empty on failure.
 * @note Prefers WEBP, falls back to JPEG then PNG depending on available plugins.
 */
QByteArray encodeThumbnail(const QImage &image, int maxSide, int quality);

} // namespace iw
