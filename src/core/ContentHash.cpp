#include "core/ContentHash.h"

#include <QCryptographicHash>
#include <QFile>

#include <cstring>

namespace argus {

QByteArray fileContentHash(const QString &absPath, qint64 *bytesRead)
{
    QFile file(absPath);
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QCryptographicHash hash(QCryptographicHash::Blake2b_256);
    if (!hash.addData(&file))
        return {};

    if (bytesRead)
        *bytesRead = file.size();
    return hash.result();
}

QByteArray dataContentHash(const QByteArray &data)
{
    QCryptographicHash hash(QCryptographicHash::Blake2b_256);
    hash.addData(data);
    return hash.result();
}

qint64 shortHash(const QByteArray &contentHash)
{
    if (contentHash.size() < 8)
        return 0;
    quint64 v = 0;
    std::memcpy(&v, contentHash.constData(), sizeof(v));
    return static_cast<qint64>(v);
}

} // namespace argus
