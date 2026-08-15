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

/// Shrink to fit a square box, keeping the aspect ratio; a no-op when already
/// small enough. Enlarging would invent detail the hash would then measure.
QImage fitToBox(const QImage &src, int box)
{
    if (src.isNull() || (src.width() <= box && src.height() <= box))
        return src;

    QSize target = src.size();
    target.scale(box, box, Qt::KeepAspectRatio);
    if (target.isEmpty())
        return src;
    return src.scaled(target, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

/// Read a file, asking the plugin for a reduced decode that fits @p box. The
/// target size is computed exactly as fitToBox() computes it, so a file read
/// this way and the same file read whole and then fitted agree pixel for pixel.
QImage readFitted(const QString &absPath, const QSize &full, int box)
{
    QImageReader reader(absPath);
    reader.setAutoTransform(true);           // honour the EXIF orientation tag
    reader.setDecideFormatFromContent(true); // wrong extensions are common

    if (full.isValid() && (full.width() > box || full.height() > box)) {
        QSize scaled = full;
        scaled.scale(box, box, Qt::KeepAspectRatio);
        if (!scaled.isEmpty())
            reader.setScaledSize(scaled);
    }
    return reader.read();
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

    QSize full;
    {
        QImageReader probe(absPath);
        probe.setAutoTransform(true);
        probe.setDecideFormatFromContent(true);
        full = probe.size();
    }

    // A preview wider than the hash box needs a bigger decode; the hash does
    // not, and must not be computed from it.
    const int box = req.wantThumbnail ? std::max(req.thumbSize, kHashSourceBox) : kHashSourceBox;

    const QImage image = readFitted(absPath, full, box);
    if (image.isNull()) {
        QImageReader reader(absPath);
        out.error = reader.errorString();
        return out;
    }

    DecodedImage derived = decodeFromImage(image, req);

    // Only reachable with an unusually large preview size. Scaling twice does
    // not land where one scale does, so the hash gets its own decode rather
    // than a second-hand one, and stays a function of the file alone.
    if (derived.ok && box != kHashSourceBox) {
        const QImage exact = readFitted(absPath, full, kHashSourceBox);
        if (!exact.isNull()) {
            DecodeRequest hashOnly = req;
            hashOnly.wantThumbnail = false;
            const DecodedImage hashed = decodeFromImage(exact, hashOnly);
            if (hashed.ok) {
                derived.gray32  = hashed.gray32;
                derived.gray9x8 = hashed.gray9x8;
            }
        }
    }

    if (full.isValid()) {
        // Keep the true on-disk dimensions, not those of the reduced decode.
        derived.width  = full.width();
        derived.height = full.height();
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

    // Fit before flattening, in that order: the reduced decode in
    // decodeForIndex() also scales while the alpha channel is still there, and
    // scaling a semi-transparent edge before or after it is composited gives
    // different pixels.
    const QImage hashSource = flattenAlpha(fitToBox(source, kHashSourceBox), req.alphaBackground);

    out.gray32  = toGray(hashSource, 32, 32);
    out.gray9x8 = toGray(hashSource, 9, 8);
    if (out.gray32.isNull() || out.gray9x8.isNull()) {
        out.error = QStringLiteral("grayscale conversion failed");
        return out;
    }

    if (req.wantThumbnail) {
        out.thumbnail = encodeThumbnail(flattenAlpha(source, req.alphaBackground),
                                        req.thumbSize, req.thumbQuality);
    }

    out.ok = true;
    return out;
}

} // namespace iw
