#include "core/Scanner.h"

#include "core/ImageDecoder.h"
#include "core/Paths.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

namespace iw {

QList<ScannedFile> scanDirectory(const QString &root,
                                 const ScanOptions &options,
                                 const std::atomic_bool *cancel,
                                 const std::function<void(int)> &progress)
{
    QList<ScannedFile> out;

    const QString base = normalizeRoot(root);
    if (base.isEmpty() || !QFileInfo(base).isDir())
        return out;

    QSet<QString> wanted;
    const QStringList exts = options.extensions.isEmpty() ? defaultExtensions()
                                                          : options.extensions;
    for (const QString &e : exts)
        wanted.insert(e.toLower());

    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
    if (options.recursive)
        flags |= QDirIterator::Subdirectories;
    if (options.followSymlinks)
        flags |= QDirIterator::FollowSymlinks;

    QDir::Filters filters = QDir::Files | QDir::NoDotAndDotDot | QDir::Readable;

    QDirIterator it(base, filters, flags);
    int seen = 0;

    while (it.hasNext()) {
        it.next();

        if (++seen % 4096 == 0) {
            if (cancel && cancel->load(std::memory_order_relaxed))
                return out;
            if (progress)
                progress(out.size());
        }

        const QFileInfo info = it.fileInfo();

        const QString absolute = QDir::fromNativeSeparators(info.absoluteFilePath());
        if (!absolute.startsWith(base))
            continue;

        QString rel = absolute.mid(base.size());
        if (rel.startsWith(QLatin1Char('/')))
            rel.remove(0, 1);
        if (rel.isEmpty() || isStatePath(rel))
            continue;

        if (!wanted.contains(info.suffix().toLower()))
            continue;

        const qint64 size = info.size();
        if (size <= 0)
            continue;
        if (options.maxBytes > 0 && size > options.maxBytes)
            continue;

        ScannedFile f;
        f.rel   = rel;
        f.size  = size;
        f.mtime = info.lastModified().toSecsSinceEpoch();
        out.append(f);
    }

    if (progress)
        progress(out.size());
    return out;
}

} // namespace iw
