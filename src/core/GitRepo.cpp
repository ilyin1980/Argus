#include "core/GitRepo.h"

#include "core/ImageDecoder.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>

namespace argus::git {

namespace {

/** @brief How long any one git invocation may take before it is given up on. */
constexpr int kTimeoutMs = 30000;

/**
 * @brief Run git and collect its standard output.
 * @param workingDir Directory to run in.
 * @param args       Arguments after the program name.
 * @param output     Receives stdout verbatim.
 * @param error      Optional failure description.
 * @return @c true when git ran and exited zero.
 */
bool runGit(const QString &workingDir,
            const QStringList &args,
            QByteArray *output,
            QString *error)
{
    QProcess process;
    process.setWorkingDirectory(workingDir);
    // A repository can be enormous, and git will happily ask a pager to page
    // its output. Without this the process never exits.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert(QStringLiteral("GIT_PAGER"), QStringLiteral("cat"));
    env.insert(QStringLiteral("GIT_OPTIONAL_LOCKS"), QStringLiteral("0"));
    process.setProcessEnvironment(env);

    process.start(QStringLiteral("git"), args);
    if (!process.waitForStarted(kTimeoutMs)) {
        if (error)
            *error = QStringLiteral("git is not installed or not on PATH");
        return false;
    }
    if (!process.waitForFinished(kTimeoutMs)) {
        process.kill();
        process.waitForFinished(1000);
        if (error)
            *error = QStringLiteral("git timed out: git %1").arg(args.join(QLatin1Char(' ')));
        return false;
    }

    if (output)
        *output = process.readAllStandardOutput();

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        if (error) {
            const QString text = QString::fromUtf8(process.readAllStandardError()).trimmed();
            *error = text.isEmpty()
                         ? QStringLiteral("git %1 failed").arg(args.join(QLatin1Char(' ')))
                         : text;
        }
        return false;
    }
    return true;
}

/** @brief First line of git's output, trimmed. */
QString firstLine(const QByteArray &output)
{
    return QString::fromUtf8(output).split(QLatin1Char('\n')).value(0).trimmed();
}

} // namespace

RepoInfo inspect(const QString &directory)
{
    RepoInfo info;

    const QFileInfo dir(directory);
    if (directory.isEmpty() || !dir.isDir())
        return info;

    const QString workingDir = dir.absoluteFilePath();

    QByteArray output;
    QString error;
    if (!runGit(workingDir, { QStringLiteral("rev-parse"), QStringLiteral("--show-toplevel") },
                &output, &error)) {
        // Not being in a repository is the ordinary case for most folders, and
        // reporting it as an error would make every caller filter noise.
        info.error = error;
        return info;
    }

    const QString topLevel = QDir::fromNativeSeparators(firstLine(output));
    if (topLevel.isEmpty())
        return info;

    info.isRepo   = true;
    info.topLevel = topLevel;

    const QString absolute = QDir::fromNativeSeparators(workingDir);
    if (absolute.size() > topLevel.size() && absolute.startsWith(topLevel)) {
        info.prefix = absolute.mid(topLevel.size());
        while (info.prefix.startsWith(QLatin1Char('/')))
            info.prefix.remove(0, 1);
        while (info.prefix.endsWith(QLatin1Char('/')))
            info.prefix.chop(1);
    }

    QByteArray gitDir;
    if (runGit(topLevel, { QStringLiteral("rev-parse"), QStringLiteral("--absolute-git-dir") },
               &gitDir, nullptr)) {
        info.gitDir = QDir::fromNativeSeparators(firstLine(gitDir));
    }

    // Where Git LFS keeps the real contents. `lfs.storage` may point it
    // elsewhere — a shared cache on another drive is a common setup — and a
    // relative value is relative to the git directory.
    if (!info.gitDir.isEmpty()) {
        QByteArray storage;
        QString configured;
        if (runGit(topLevel, { QStringLiteral("config"), QStringLiteral("--get"),
                               QStringLiteral("lfs.storage") }, &storage, nullptr)) {
            configured = QDir::fromNativeSeparators(firstLine(storage));
        }
        if (configured.isEmpty())
            info.lfsDir = info.gitDir + QStringLiteral("/lfs");
        else if (QDir::isAbsolutePath(configured))
            info.lfsDir = configured;
        else
            info.lfsDir = info.gitDir + QLatin1Char('/') + configured;
    }

    // Detached heads answer "HEAD", which is not a branch and must not be
    // offered as one.
    QByteArray head;
    if (runGit(topLevel, { QStringLiteral("rev-parse"), QStringLiteral("--abbrev-ref"),
                           QStringLiteral("HEAD") }, &head, nullptr)) {
        const QString name = firstLine(head);
        if (name != QLatin1String("HEAD"))
            info.currentRef = name;
    }
    return info;
}

QStringList branches(const QString &topLevel, bool includeRemotes, QString *error)
{
    QStringList refs = { QStringLiteral("refs/heads") };
    if (includeRemotes)
        refs << QStringLiteral("refs/remotes");

    QStringList args = { QStringLiteral("for-each-ref"),
                         QStringLiteral("--format=%(refname:short)"),
                         QStringLiteral("--sort=refname") };
    args += refs;

    QByteArray output;
    if (!runGit(topLevel, args, &output, error))
        return {};

    QStringList out;
    const QList<QByteArray> lines = output.split('\n');
    for (const QByteArray &line : lines) {
        const QString name = QString::fromUtf8(line).trimmed();
        if (name.isEmpty())
            continue;
        // refs/remotes carries a symbolic origin/HEAD that resolves to another
        // entry in this same list; indexing it would duplicate a whole branch.
        if (name.endsWith(QLatin1String("/HEAD")))
            continue;
        out << name;
    }
    return out;
}

QString resolve(const QString &topLevel, const QString &ref, QString *error)
{
    QByteArray output;
    if (!runGit(topLevel, { QStringLiteral("rev-parse"), QStringLiteral("--verify"),
                            QStringLiteral("%1^{commit}").arg(ref) },
                &output, error)) {
        return {};
    }
    return firstLine(output);
}

QList<BlobEntry> listBlobs(const QString &topLevel,
                           const QString &ref,
                           const QString &prefix,
                           const QStringList &extensions,
                           QString *error)
{
    QStringList wanted = extensions;
    if (wanted.isEmpty())
        wanted = defaultExtensions();

    QStringList args = { QStringLiteral("ls-tree"), QStringLiteral("-r"),
                         QStringLiteral("-z"), QStringLiteral("--long"), ref };
    if (!prefix.isEmpty())
        args << QStringLiteral("--") << prefix;

    QByteArray output;
    if (!runGit(topLevel, args, &output, error))
        return {};

    // Records are NUL-separated; each is "<mode> <type> <oid> <size>\t<path>".
    // -z also means the path is raw rather than quoted, which is the only way
    // to survive a non-ASCII file name.
    QList<BlobEntry> entries;
    const QList<QByteArray> records = output.split('\0');
    entries.reserve(records.size());

    const QString prefixSlash = prefix.isEmpty() ? QString() : prefix + QLatin1Char('/');

    for (const QByteArray &record : records) {
        const int tab = record.indexOf('\t');
        if (tab < 0)
            continue;

        const QList<QByteArray> head = record.left(tab).simplified().split(' ');
        if (head.size() < 4 || head.at(1) != "blob")
            continue;

        QString path = QString::fromUtf8(record.mid(tab + 1));
        if (path.isEmpty())
            continue;

        const QString suffix = QFileInfo(path).suffix().toLower();
        if (!wanted.contains(suffix))
            continue;

        if (!prefixSlash.isEmpty()) {
            if (!path.startsWith(prefixSlash))
                continue;
            path = path.mid(prefixSlash.size());
        }

        BlobEntry entry;
        entry.rel  = path;
        entry.oid  = QString::fromLatin1(head.at(2));
        entry.size = head.at(3).toLongLong();
        entries.append(entry);
    }
    return entries;
}

BlobReader::BlobReader(const QString &topLevel)
    : m_process(std::make_unique<QProcess>())
{
    m_lfsDir = inspect(topLevel).lfsDir;

    m_process->setWorkingDirectory(topLevel);
    m_process->setProcessChannelMode(QProcess::SeparateChannels);
    m_process->start(QStringLiteral("git"),
                     { QStringLiteral("cat-file"), QStringLiteral("--batch") });
    if (!m_process->waitForStarted(kTimeoutMs)) {
        m_error = QStringLiteral("git is not installed or not on PATH");
        m_process.reset();
    }
}

BlobReader::~BlobReader()
{
    if (m_process) {
        m_process->closeWriteChannel();
        if (!m_process->waitForFinished(2000)) {
            m_process->kill();
            m_process->waitForFinished(1000);
        }
    }
}

bool BlobReader::isReady() const
{
    return m_process && m_process->state() == QProcess::Running;
}

QString BlobReader::error() const
{
    return m_error;
}

int BlobReader::missingLfsObjects() const
{
    return m_missingLfs;
}

QByteArray BlobReader::resolveLfs(const QByteArray &pointer, QString *error)
{
    // version https://git-lfs.github.com/spec/v1
    // oid sha256:<64 hex>
    // size <bytes>
    QString oid;
    for (const QByteArray &line : pointer.split('\n')) {
        const QByteArray trimmed = line.trimmed();
        if (!trimmed.startsWith("oid sha256:"))
            continue;
        oid = QString::fromLatin1(trimmed.mid(11));
        break;
    }
    if (oid.size() != 64 || m_lfsDir.isEmpty()) {
        if (error)
            *error = QStringLiteral("unreadable Git LFS pointer");
        ++m_missingLfs;
        return {};
    }

    // Git LFS shards its store by the first two byte pairs of the digest.
    const QString path = QStringLiteral("%1/objects/%2/%3/%4")
                             .arg(m_lfsDir, oid.left(2), oid.mid(2, 2), oid);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        if (error) {
            *error = QStringLiteral("Git LFS object %1 is not in the local store")
                         .arg(oid.left(12));
        }
        ++m_missingLfs;
        return {};
    }
    return file.readAll();
}

QByteArray BlobReader::read(const QString &oid, QString *error)
{
    if (!isReady()) {
        if (error)
            *error = m_error.isEmpty() ? QStringLiteral("git cat-file is not running") : m_error;
        return {};
    }

    m_process->write(oid.toLatin1() + '\n');
    if (!m_process->waitForBytesWritten(kTimeoutMs)) {
        if (error)
            *error = QStringLiteral("git cat-file stopped accepting requests");
        return {};
    }

    // Header line: "<oid> <type> <size>", or "<oid> missing".
    QByteArray header;
    while (!header.endsWith('\n')) {
        if (m_process->bytesAvailable() == 0 && !m_process->waitForReadyRead(kTimeoutMs)) {
            if (error)
                *error = QStringLiteral("git cat-file stopped responding");
            return {};
        }
        header += m_process->readLine();
    }

    const QList<QByteArray> parts = header.trimmed().split(' ');
    if (parts.size() < 3 || parts.at(1) != "blob") {
        if (error)
            *error = QStringLiteral("not a blob: %1").arg(oid);
        return {};
    }

    bool ok = false;
    const qint64 size = parts.at(2).toLongLong(&ok);
    if (!ok || size < 0) {
        if (error)
            *error = QStringLiteral("unreadable size for %1").arg(oid);
        return {};
    }

    // The payload is followed by a newline that belongs to the protocol, not to
    // the file. Leaving it in the pipe would desynchronise every later read.
    QByteArray data;
    data.reserve(static_cast<int>(size));
    while (data.size() < size + 1) {
        if (m_process->bytesAvailable() == 0 && !m_process->waitForReadyRead(kTimeoutMs)) {
            if (error)
                *error = QStringLiteral("git cat-file truncated %1").arg(oid);
            return {};
        }
        data += m_process->read(size + 1 - data.size());
    }
    data.chop(1);

    // A repository on Git LFS stores a pointer file where the image should be,
    // and the working tree only holds the real picture because the smudge
    // filter ran at checkout. Indexing the pointer would report every asset as
    // undecodable, which is a true statement about the wrong bytes.
    static const QByteArray kPointer = "version https://git-lfs.github.com/spec/";
    if (data.size() < 1024 && data.startsWith(kPointer))
        return resolveLfs(data, error);

    return data;
}

} // namespace argus::git
