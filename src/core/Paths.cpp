#include "core/Paths.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QUrl>

namespace iw {

static const char *kStateDirName = ".imageworker";

QString normalizeRoot(const QString &root)
{
    QString p = QDir::fromNativeSeparators(QDir(root).absolutePath());
    while (p.size() > 1 && p.endsWith(QLatin1Char('/')))
        p.chop(1);
    return p;
}

QString stateDirFor(const QString &root)
{
    return normalizeRoot(root) + QLatin1Char('/') + QLatin1String(kStateDirName);
}

QString defaultDatabasePath(const QString &root, QString *error)
{
    const QString dir = stateDirFor(root);
    if (!QDir().mkpath(dir)) {
        if (error)
            *error = QStringLiteral("cannot create state directory: %1").arg(QDir::toNativeSeparators(dir));
        return {};
    }
    return dir + QLatin1String("/index.db");
}

QString absolutePathFor(const QString &root, const QString &rel)
{
    return normalizeRoot(root) + QLatin1Char('/') + rel;
}

bool isStatePath(const QString &rel)
{
    return rel == QLatin1String(kStateDirName)
        || rel.startsWith(QLatin1String(kStateDirName) + QLatin1Char('/'));
}

QString defaultModelsDir()
{
    QDir dir(QCoreApplication::applicationDirPath());
    for (int level = 0; level <= 4; ++level) {
        const QString candidate = dir.absoluteFilePath(QStringLiteral("models"));
        if (QFileInfo(candidate).isDir())
            return QDir::fromNativeSeparators(candidate);
        if (!dir.cdUp())
            break;
    }
    return QStringLiteral("models");
}

bool revealInFileManager(const QString &absPath)
{
    const QFileInfo info(absPath);
    if (!info.exists())
        return false;

#if defined(Q_OS_WIN)
    return QProcess::startDetached(QStringLiteral("explorer.exe"),
                                   { QStringLiteral("/select,") + QDir::toNativeSeparators(info.absoluteFilePath()) });
#elif defined(Q_OS_MACOS)
    return QProcess::startDetached(QStringLiteral("open"),
                                   { QStringLiteral("-R"), info.absoluteFilePath() });
#else
    return QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
#endif
}

} // namespace iw
