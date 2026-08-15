#include "core/ImageDecoder.h"

#include <QBuffer>
#include <QImageReader>
#include <QImageWriter>
#include <QPainter>
#include <QSet>

#include <algorithm>

namespace iw {

namespace {

/// Grayscale-and-resize in a way that does not depend on Qt smooth-scaling any
/// particular 8-bit format: convert first, scale, then force the format back.
QImage toGray(const QImage &src, int w, int h)
{
    QImage gray = src.convertToFormat(QImage::Format_Grayscale8);
    if (gray.isNull())
        return {};

    QImage scaled = gray.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    if (scaled.isNull())
        return {};
    if (scaled.format() != QImage::Format_Grayscale8)
        scaled = scaled.convertToFormat(QImage::Format_Grayscale8);
    return scaled;
}

/// Transparent pixels have no defined colour, so two visually identical sprites
/// on different backgrounds would otherwise hash differently. Flatten onto a
/// fixed neutral grey before any hashing happens.
QImage flattenAlpha(const QImage &src, int background)
{
    if (!src.hasAlphaChannel())
        return src;

    QImage out(src.size(), QImage::Format_RGB32);
    out.fill(QColor(background, background, background));
    QPainter p(&out);
    p.drawImage(0, 0, src);
    p.end();
    return out;
}

} // namespace

QStringList supportedExtensions()
{
    QStringList out;
    const auto formats = QImageReader::supportedImageFormats();
    out.reserve(formats.size() + 8);
    for (const QByteArray &f : formats)
        out << QString::fromLatin1(f).toLower();

    // Qt reports format names, not file extensions: a "jpeg" plugin still has
    // to match .jpg on disk.
    const struct { const char *format; const char *alias; } aliases[] = {
        { "jpeg", "jpg"  },
        { "jpeg", "jpe"  },
        { "jpeg", "jfif" },
        { "tiff", "tif"  },
        { "heif", "heic" },
    };
    for (const auto &a : aliases) {
        if (out.contains(QLatin1String(a.format)))
            out << QLatin1String(a.alias);
    }

    out.removeDuplicates();
    out.sort();
    return out;
}

QStringList defaultExtensions()
{
    static const QStringList wanted = { QStringLiteral("png"),
                                        QStringLiteral("jpg"),
                                        QStringLiteral("jpeg") };
    const QStringList available = supportedExtensions();

    QStringList out;
    for (const QString &e : wanted) {
        if (available.contains(e))
            out << e;
    }
    return out;
}

QByteArray encodeThumbnail(const QImage &image, int maxSide, int quality)
{
    if (image.isNull())
        return {};

    QImage scaled = image;
    if (image.width() > maxSide || image.height() > maxSide)
        scaled = image.scaled(maxSide, maxSide, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    static const QSet<QByteArray> writable = [] {
        QSet<QByteArray> s;
        for (const QByteArray &f : QImageWriter::supportedImageFormats())
            s.insert(f.toLower());
        return s;
    }();

    struct Candidate { const char *format; int quality; };
    const Candidate candidates[] = {
        { "webp", quality },
        { "jpeg", std::max(quality, 80) },
        { "png",  -1 },
    };

    for (const Candidate &c : candidates) {
        if (!writable.contains(QByteArray(c.format)))
            continue;
        QByteArray bytes;
        QBuffer buffer(&bytes);
        if (!buffer.open(QIODevice::WriteOnly))
            continue;
        QImageWriter writer(&buffer, QByteArray(c.format));
        if (c.quality >= 0)
            writer.setQuality(c.quality);
        if (writer.write(scaled) && !bytes.isEmpty())
            return bytes;
    }
    return {};
}

DecodedImage decodeForIndex(const QString &absPath, const DecodeRequest &req)
{
    DecodedImage out;

    QImageReader reader(absPath);
    reader.setAutoTransform(true);          // honour the EXIF orientation tag
    reader.setDecideFormatFromContent(true); // wrong extensions are common

    const QSize full = reader.size();
    if (full.isValid()) {
        out.width  = full.width();
        out.height = full.height();

        // Ask the plugin for a smaller decode when it can do it cheaply
        // (libjpeg scale_denom); harmless when it cannot.
        const int target = req.wantThumbnail ? std::max(req.thumbSize, 128) : 128;
        if (full.width() > target || full.height() > target) {
            QSize scaled = full;
            scaled.scale(target, target, Qt::KeepAspectRatio);
            if (!scaled.isEmpty())
                reader.setScaledSize(scaled);
        }
    }

    QImage image = reader.read();
    if (image.isNull()) {
        out.error = reader.errorString();
        return out;
    }

    DecodedImage derived = decodeFromImage(image, req);
    if (full.isValid()) {
        // Keep the true on-disk dimensions, not those of the reduced decode.
        derived.width  = out.width;
        derived.height = out.height;
    }
    return derived;
}

DecodedImage decodeFromImage(const QImage &source, const DecodeRequest &req)
{
    DecodedImage out;
    if (source.isNull()) {
        out.error = QStringLiteral("empty image");
        return out;
    }

    out.width  = source.width();
    out.height = source.height();

    const QImage image = flattenAlpha(source, req.alphaBackground);

    out.gray32  = toGray(image, 32, 32);
    out.gray9x8 = toGray(image, 9, 8);
    if (out.gray32.isNull() || out.gray9x8.isNull()) {
        out.error = QStringLiteral("grayscale conversion failed");
        return out;
    }

    if (req.wantThumbnail)
        out.thumbnail = encodeThumbnail(image, req.thumbSize, req.thumbQuality);

    out.ok = true;
    return out;
}

} // namespace iw
