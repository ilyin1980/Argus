/**
 * @file ThumbnailDelegate.h
 * @brief Fixed-size grid cell painter for the result views.
 *
 * The stock delegate derives an item's height from its text, so a long file
 * name makes one cell taller than the grid step and neighbouring cells overlap
 * it. Painting the cell ourselves with a size hint that never varies removes
 * that whole class of problem, and gives room for a second, quieter line.
 */
#pragma once

#include <QRect>
#include <QSize>
#include <QStyledItemDelegate>

class QVariantAnimation;

/** @brief Draws a preview, one elided title line and one subtitle line. */
class ThumbnailDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ThumbnailDelegate(QObject *parent = nullptr);

    /**
     * @brief Start the staggered entrance for a freshly filled grid.
     *
     * Results arrive all at once after a wait of seconds; fading them in from
     * the top left, a few milliseconds apart, shows the reading order and marks
     * the moment the answer appeared. Connected to the model's reset signal.
     */
    void beginAppear();

    /**
     * @brief Set the cell footprint.
     * @param cell Total size of one grid cell, previews included.
     * @note Must match the owning view's grid size, or the view will still
     *       space cells by its own step.
     */
    void setCellSize(const QSize &cell);

    /** @return Current cell footprint. */
    QSize cellSize() const { return m_cell; }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void  paint(QPainter *painter,
                const QStyleOptionViewItem &option,
                const QModelIndex &index) const override;
    bool  editorEvent(QEvent *event,
                      QAbstractItemModel *model,
                      const QStyleOptionViewItem &option,
                      const QModelIndex &index) override;

signals:
    /**
     * @brief The small copy glyph beside a file name was clicked.
     * @param index Item whose full path the user wants on the clipboard.
     */
    void copyPathRequested(const QModelIndex &index);

private:
    /** @brief Height reserved below the preview for the two text lines. */
    int textHeight(const QStyleOptionViewItem &option) const;

    /** @brief Hit area of the copy glyph, at the right end of the title line. */
    QRect copyIconRect(const QStyleOptionViewItem &option) const;

    /** @brief Draw the two-sheet copy glyph inside @p rect. */
    void paintCopyIcon(QPainter *painter, const QRect &rect, const QColor &colour) const;

    /**
     * @brief Entrance opacity for one cell.
     * @param row Row index, which drives the stagger.
     * @return Opacity in 0..1; 1.0 once the entrance has finished.
     */
    qreal appearOpacity(int row) const;

    QSize              m_cell{ 184, 214 };
    QVariantAnimation *m_appear   = nullptr;
    qreal              m_appearAt = 1.0; ///< 1.0 means "fully shown".
};
