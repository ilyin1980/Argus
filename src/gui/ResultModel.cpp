#include "ResultModel.h"

#include "core/Database.h"
#include "core/Paths.h"

#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QPainter>

ResultModel::ResultModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Neutral tile shown until a preview is available, so the grid never jumps.
    m_placeholder = QPixmap(160, 160);
    m_placeholder.fill(Qt::transparent);
    QPainter painter(&m_placeholder);
    painter.setPen(QPen(QColor(128, 128, 128, 90), 1));
    painter.drawRect(0, 0, 159, 159);
    painter.end();
}

void ResultModel::setSource(argus::Database *database, const QString &root)
{
    beginResetModel();
    m_database = database;
    m_root     = root;
    m_cache.clear();
    endResetModel();
}

void ResultModel::setRows(const QList<argus::FileInfoRow> &rows)
{
    beginResetModel();
    m_entries.clear();
    m_entries.reserve(rows.size());
    for (const argus::FileInfoRow &row : rows)
        m_entries.append(Entry{ row, -1.0, -1 });
    endResetModel();
}

void ResultModel::setMatches(const QList<argus::Match> &matches)
{
    beginResetModel();
    m_entries.clear();
    m_entries.reserve(matches.size());
    for (const argus::Match &match : matches)
        m_entries.append(Entry{ match.file, match.score, match.distance });
    endResetModel();
}

void ResultModel::setMetricLabel(const QString &label)
{
    if (m_metricLabel == label)
        return;
    m_metricLabel = label;
    if (!m_entries.isEmpty())
        emit dataChanged(index(0), index(static_cast<int>(m_entries.size()) - 1), { SubtitleRole });
}

void ResultModel::clear()
{
    beginResetModel();
    m_entries.clear();
    endResetModel();
}

argus::FileInfoRow ResultModel::rowAt(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() >= m_entries.size())
        return {};
    return m_entries.at(index.row()).row;
}

int ResultModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(m_entries.size());
}

QPixmap ResultModel::thumbnailFor(const Entry &entry) const
{
    const auto cached = m_cache.constFind(entry.row.id);
    if (cached != m_cache.constEnd())
        return *cached;

    QPixmap pixmap;
    if (m_database) {
        const QByteArray bytes = m_database->thumbnail(entry.row.id);
        if (!bytes.isEmpty())
            pixmap.loadFromData(bytes);
    }
    if (pixmap.isNull() && !m_root.isEmpty()) {
        // No stored preview (indexed with --no-thumbs): fall back to the file.
        const QString absolute = argus::absolutePathFor(m_root, entry.row.rel);
        QPixmap full;
        if (full.load(absolute))
            pixmap = full.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    if (pixmap.isNull())
        pixmap = m_placeholder;

    m_cache.insert(entry.row.id, pixmap);
    return pixmap;
}

QVariant ResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_entries.size())
        return {};

    const Entry &entry = m_entries.at(index.row());

    switch (role) {
    case Qt::DecorationRole:
        return thumbnailFor(entry);

    case Qt::DisplayRole:
        // Just the name: the delegate elides it and puts detail on line two.
        return QFileInfo(entry.row.rel).fileName();

    case SubtitleRole: {
        // The branch goes first when there is one: which version of the file
        // this is changes what the numbers after it even mean.
        const QString origin = entry.row.ref.isEmpty()
                                   ? QString()
                                   : entry.row.ref + QStringLiteral("  ·  ");
        if (entry.score >= 0.0) {
            return origin + QStringLiteral("%1  ·  %2 %3")
                                .arg(entry.score, 0, 'f', 3)
                                .arg(entry.distance)
                                .arg(m_metricLabel);
        }
        return origin + QStringLiteral("%1×%2  ·  %3")
                            .arg(entry.row.width)
                            .arg(entry.row.height)
                            .arg(QLocale::system().formattedDataSize(entry.row.size));
    }

    case AbsolutePathRole:
        // A row read out of a branch has no file on disk. Handing back an
        // absolute path for it would be a lie that the copy button, the path
        // bar and every script downstream would then act on; git's own
        // branch:path syntax is both true and directly usable.
        if (!entry.row.ref.isEmpty())
            return entry.row.ref + QLatin1Char(':') + entry.row.rel;
        return m_root.isEmpty()
                   ? entry.row.rel
                   : QDir::toNativeSeparators(argus::absolutePathFor(m_root, entry.row.rel));

    case Qt::ToolTipRole: {
        QString text = entry.row.ref.isEmpty()
                           ? entry.row.rel
                           : entry.row.ref + QLatin1Char(':') + entry.row.rel;
        text += QStringLiteral("\n%1 x %2, %3")
                    .arg(entry.row.width)
                    .arg(entry.row.height)
                    .arg(QLocale::system().formattedDataSize(entry.row.size));
        if (entry.distance >= 0)
            text += QLatin1Char('\n') + tr("distance %1").arg(entry.distance);
        return text;
    }

    case RelPathRole:
        return entry.row.rel;
    case ScoreRole:
        return entry.score;
    case DistanceRole:
        return entry.distance;
    case FileIdRole:
        return entry.row.id;

    default:
        return {};
    }
}
