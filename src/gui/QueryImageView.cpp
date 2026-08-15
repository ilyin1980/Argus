#include "QueryImageView.h"

#include "Theme.h"

#include <QVariantAnimation>

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>

#include <algorithm>

namespace {

/** @brief Smallest selection worth honouring, in image pixels. */
constexpr int kMinSelection = 4;

} // namespace

QueryImageView::QueryImageView(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(260, 200);
    setCursor(Qt::CrossCursor);
    setToolTip(QStringLiteral(
        "Paste a screenshot with Ctrl+V or drop a file here.\n"
        "Drag to box the object you are looking for; double-click to clear the box."));
}

QSize QueryImageView::sizeHint() const
{
    return QSize(360, 260);
}

void QueryImageView::setImage(const QImage &image)
{
    m_image = image;
    m_selection = QRect();
    m_dragging = false;
    m_matchOutline.clear();
    m_matchLabel.clear();
    update();
    emit selectionChanged();
}

void QueryImageView::setMatchOutline(const QPolygonF &outline, const QString &label)
{
    m_matchOutline = outline;
    m_matchLabel   = label;

    if (!m_pulseAnimation) {
        m_pulseAnimation = new QVariantAnimation(this);
        m_pulseAnimation->setStartValue(0.0);
        m_pulseAnimation->setEndValue(1.0);
        m_pulseAnimation->setDuration(1400);
        m_pulseAnimation->setLoopCount(-1);
        connect(m_pulseAnimation, &QVariantAnimation::valueChanged, this,
                [this](const QVariant &v) { setMatchPulse(v.toReal()); });
    }

    // The pulse exists to draw the eye to a result that may be a small box in a
    // busy screenshot. With nothing found there is nothing to point at, so it
    // stops rather than idling.
    if (m_matchOutline.size() >= 3)
        m_pulseAnimation->start();
    else
        m_pulseAnimation->stop();

    update();
}

void QueryImageView::setMatchPulse(qreal value)
{
    m_matchPulse = value;
    update();
}

void QueryImageView::clearSelection()
{
    if (!m_selection.isValid())
        return;
    m_selection = QRect();
    update();
    emit selectionChanged();
}

QImage QueryImageView::effectiveImage() const
{
    if (m_image.isNull())
        return {};
    if (!m_selection.isValid())
        return m_image;
    return m_image.copy(m_selection);
}

QRect QueryImageView::paintedRect() const
{
    if (m_image.isNull())
        return {};

    const QSize scaled = m_image.size().scaled(size(), Qt::KeepAspectRatio);
    const int x = (width()  - scaled.width())  / 2;
    const int y = (height() - scaled.height()) / 2;
    return QRect(QPoint(x, y), scaled);
}

QPoint QueryImageView::toImagePoint(const QPoint &widgetPoint) const
{
    const QRect target = paintedRect();
    if (target.isEmpty() || m_image.isNull())
        return {};

    const double sx = static_cast<double>(m_image.width())  / target.width();
    const double sy = static_cast<double>(m_image.height()) / target.height();

    const int ix = static_cast<int>((widgetPoint.x() - target.x()) * sx);
    const int iy = static_cast<int>((widgetPoint.y() - target.y()) * sy);

    return QPoint(std::clamp(ix, 0, m_image.width()  - 1),
                  std::clamp(iy, 0, m_image.height() - 1));
}

void QueryImageView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), palette().base());

    if (m_image.isNull()) {
        painter.setPen(palette().mid().color());
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
        painter.setPen(palette().text().color());
        painter.drawText(rect(), Qt::AlignCenter,
                         QStringLiteral("Ctrl+V to paste a screenshot,\nor drop an image here"));
        return;
    }

    const QRect target = paintedRect();
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawImage(target, m_image);

    const double sx = static_cast<double>(target.width())  / m_image.width();
    const double sy = static_cast<double>(target.height()) / m_image.height();

    // Where the search says the asset sits. Drawn before the selection dimming
    // so a box the user drew still reads as the foreground.
    if (m_matchOutline.size() >= 3) {
        QPolygonF widgetOutline;
        for (const QPointF &p : m_matchOutline)
            widgetOutline << QPointF(target.x() + p.x() * sx, target.y() + p.y() * sy);

        // Triangle wave: the outline breathes rather than blinking, which stays
        // readable while still being impossible to miss.
        const qreal wave = m_matchPulse < 0.5 ? m_matchPulse * 2.0
                                              : (1.0 - m_matchPulse) * 2.0;
        const QColor accent = theme::accent();

        // A soft halo underneath does the attention-getting; the crisp line on
        // top keeps the reported position exact.
        QColor halo = accent;
        halo.setAlphaF(0.10f + 0.16f * float(wave));
        painter.setPen(QPen(halo, 9.0));
        painter.setBrush(Qt::NoBrush);
        painter.drawPolygon(widgetOutline);

        QColor fill = accent;
        fill.setAlphaF(0.10f);
        painter.setBrush(fill);
        painter.setPen(QPen(accent, 1.6 + 0.9 * wave));
        painter.drawPolygon(widgetOutline);

        if (!m_matchLabel.isEmpty()) {
            const QRectF bounds = widgetOutline.boundingRect();
            const QRectF labelRect(bounds.left(), std::max(0.0, bounds.top() - 20.0),
                                   bounds.width(), 18.0);
            QColor plate = theme::background();
            plate.setAlphaF(0.78f);
            painter.setPen(Qt::NoPen);
            painter.setBrush(plate);
            painter.drawRect(labelRect);
            painter.setPen(accent);
            painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter,
                             QStringLiteral("  ") + m_matchLabel);
        }
        painter.setBrush(Qt::NoBrush);
    }

    if (!m_selection.isValid())
        return;

    // Map the selection back into widget space and dim everything outside it.
    const QRect box(target.x() + static_cast<int>(m_selection.x() * sx),
                    target.y() + static_cast<int>(m_selection.y() * sy),
                    static_cast<int>(m_selection.width()  * sx),
                    static_cast<int>(m_selection.height() * sy));

    QRegion outside(rect());
    outside = outside.subtracted(QRegion(box));
    painter.setClipRegion(outside);
    painter.fillRect(rect(), QColor(0, 0, 0, 110));
    painter.setClipping(false);

    painter.setPen(QPen(QColor(0, 170, 255), 2));
    painter.drawRect(box);

    painter.setPen(palette().text().color());
    painter.drawText(box.adjusted(0, -18, 0, 0), Qt::AlignLeft | Qt::AlignTop,
                     QStringLiteral("%1 x %2").arg(m_selection.width()).arg(m_selection.height()));
}

void QueryImageView::mousePressEvent(QMouseEvent *event)
{
    if (m_image.isNull() || event->button() != Qt::LeftButton)
        return;

    m_dragOrigin = toImagePoint(event->pos());
    m_dragging = true;
    m_selection = QRect();
    update();
}

void QueryImageView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        clearSelection();
}

void QueryImageView::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dragging)
        return;
    m_selection = QRect(m_dragOrigin, toImagePoint(event->pos())).normalized();
    update();
}

void QueryImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_dragging || event->button() != Qt::LeftButton)
        return;
    m_dragging = false;

    m_selection = QRect(m_dragOrigin, toImagePoint(event->pos())).normalized();
    if (m_selection.width() < kMinSelection || m_selection.height() < kMinSelection)
        m_selection = QRect(); // a click, not a drag

    update();
    emit selectionChanged();
}

void QueryImageView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}
