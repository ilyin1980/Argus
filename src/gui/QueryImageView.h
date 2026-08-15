/**
 * @file QueryImageView.h
 * @brief Query image display with a drag-to-select region of interest.
 */
#pragma once

#include <QImage>
#include <QPoint>
#include <QPolygonF>
#include <QRect>
#include <QString>
#include <QWidget>

/**
 * @brief Shows the reference image and lets the user box the object to search for.
 *
 * The main scenario is a game screenshot: the object of interest occupies a small
 * part of the frame and everything around it is clutter. Letting the user drag a
 * rectangle around it removes that clutter before any matching happens, which
 * costs one widget and improves precision more than any threshold tuning can.
 */
class QVariantAnimation;

class QueryImageView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal matchPulse READ matchPulse WRITE setMatchPulse)

public:
    explicit QueryImageView(QWidget *parent = nullptr);

    /**
     * @brief Replace the displayed image and drop any existing selection.
     * @param image New reference image; may be null to clear the view.
     */
    void setImage(const QImage &image);

    /** @return The full reference image, null when nothing is loaded. */
    QImage image() const { return m_image; }

    /** @return @c true while a region of interest is set. */
    bool hasSelection() const { return m_selection.isValid(); }

    /** @return Top-left of the region of interest, or the origin when unset. */
    QPointF selectionOrigin() const
    {
        return m_selection.isValid() ? QPointF(m_selection.topLeft()) : QPointF(0, 0);
    }

    /** @brief Forget the region of interest, reverting to the whole image. */
    void clearSelection();

    /**
     * @brief Draw an outline over the image showing where a match was located.
     * @param outline Corners in image coordinates; empty clears the overlay.
     * @param label   Short caption drawn beside the outline.
     * @note Coordinates are relative to the image actually searched, so when a
     *       region of interest is set they are offset by its origin here.
     */
    void setMatchOutline(const QPolygonF &outline, const QString &label = {});

    /** @return Current phase of the found-object pulse, 0..1. */
    qreal matchPulse() const { return m_matchPulse; }

    /**
     * @brief Drive the pulse that draws the eye to the located object.
     * @param value Phase in 0..1.
     */
    void setMatchPulse(qreal value);

    /**
     * @brief The image that should actually be searched for.
     * @return The selected region, or the whole image when nothing is selected.
     */
    QImage effectiveImage() const;

    QSize sizeHint() const override;

signals:
    /** @brief Emitted whenever the image or the region of interest changes. */
    void selectionChanged();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    /** @brief Rectangle inside the widget the scaled image is painted into. */
    QRect paintedRect() const;

    /** @brief Convert a widget point to image coordinates, clamped to the image. */
    QPoint toImagePoint(const QPoint &widgetPoint) const;

    QImage    m_image;
    QRect     m_selection;   ///< In image coordinates; invalid when unset.
    QPoint    m_dragOrigin;  ///< In image coordinates.
    bool      m_dragging = false;
    QPolygonF m_matchOutline; ///< In image coordinates; empty when there is none.
    QString   m_matchLabel;
    qreal     m_matchPulse = 0.0;
    QVariantAnimation *m_pulseAnimation = nullptr;
};
