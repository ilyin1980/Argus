/**
 * @file ResultModel.h
 * @brief List model backing every thumbnail grid in the window.
 */
#pragma once

#include <QAbstractListModel>
#include <QHash>
#include <QPixmap>

#include "core/Types.h"

namespace iw {
class Database;
}

/**
 * @brief Presents file rows as a thumbnail grid.
 *
 * Previews are pulled from the database on demand and cached, so scrolling a
 * long result list never blocks on more than the rows currently visible.
 */
class ResultModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /** @brief Extra item roles beyond the Qt defaults. */
    enum Roles {
        RelPathRole = Qt::UserRole + 1, ///< QString, path relative to the root.
        AbsolutePathRole,               ///< QString, full native path for copying.
        SubtitleRole,                   ///< QString, the quiet second caption line.
        ScoreRole,                      ///< double, similarity in 0..1; -1 when not applicable.
        DistanceRole,                   ///< int, Hamming distance; -1 when not applicable.
        FileIdRole,                     ///< qint64, database primary key.
    };

    explicit ResultModel(QObject *parent = nullptr);

    /**
     * @brief Point the model at a database for preview lookups.
     * @param database Open database, or @c nullptr to detach.
     * @param root     Root the rows are relative to, used for fallback loading.
     */
    void setSource(iw::Database *database, const QString &root);

    /**
     * @brief Replace the contents with plain rows.
     * @param rows Rows to show, in display order.
     */
    void setRows(const QList<iw::FileInfoRow> &rows);

    /**
     * @brief Replace the contents with scored query hits.
     * @param matches Hits to show, in display order.
     */
    void setMatches(const QList<iw::Match> &matches);

    /**
     * @brief Name the secondary number shown under each preview.
     * @param label Short label, e.g. "d" for a hash distance or "inliers".
     * @note The same field carries a Hamming distance for a hash search and an
     *       inlier count for a neural one; labelling it "d" in both cases would
     *       misreport the second.
     */
    void setMetricLabel(const QString &label);

    /** @brief Remove every row. */
    void clear();

    /**
     * @brief Look up the row behind an index.
     * @param index Model index.
     * @return The row, or a default-constructed one if @p index is invalid.
     */
    iw::FileInfoRow rowAt(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    /** @brief One displayed entry: the row plus its optional score. */
    struct Entry {
        iw::FileInfoRow row;
        double          score    = -1.0;
        int             distance = -1;
    };

    QPixmap thumbnailFor(const Entry &entry) const;

    QList<Entry>                    m_entries;
    iw::Database                   *m_database = nullptr;
    QString                         m_root;
    mutable QHash<qint64, QPixmap>  m_cache;
    QPixmap                         m_placeholder;
    QString                         m_metricLabel = QStringLiteral("d");
};
