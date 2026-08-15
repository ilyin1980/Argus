#include "ScanProgressBar.h"

#include "Theme.h"

#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QVariantAnimation>

#include <algorithm>
#include <cmath>

namespace {

constexpr int kSweepMs   = 1500; ///< One full pass of the highlight.
constexpr int kRadius    = 3;
constexpr qreal kBandWidth = 0.28; ///< Sweep width, as a fraction of the track.

} // namespace

ScanProgressBar::ScanProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
    setTextVisible(true);
    setMinimumHeight(20);

    m_animation = new QVariantAnimation(this);
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);
    m_animation->setDuration(kSweepMs);
    m_animation->setLoopCount(-1);
    connect(m_animation, &QVariantAnimation::valueChanged, this,
            [this](const QVariant &v) { setPhase(v.toReal()); });

    connect(this, &QProgressBar::valueChanged, this, [this] { updateAnimationState(); });
    updateAnimationState();
}

ScanProgressBar::~ScanProgressBar() = default;

void ScanProgressBar::setPhase(qreal value)
{
    m_phase = value;
    update();
}

void ScanProgressBar::showEvent(QShowEvent *event)
{
    QProgressBar::showEvent(event);
    updateAnimationState();
}

void ScanProgressBar::hideEvent(QHideEvent *event)
{
    QProgressBar::hideEvent(event);
    m_animation->stop();
}

void ScanProgressBar::updateAnimationState()
{
    // An idle bar at 0% or a finished one at 100% has nothing to say; animating
    // it would imply work that is not happening.
    const bool indeterminate = (minimum() == 0 && maximum() == 0);
    const bool working = indeterminate || (value() > minimum() && value() < maximum());

    if (isVisible() && working) {
        if (m_animation->state() != QAbstractAnimation::Running)
            m_animation->start();
    } else {
        m_animation->stop();
        m_phase = 0.0;
        update();
    }
}

void ScanProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF track = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);

    painter.setPen(QPen(theme::outline(), 1));
    painter.setBrush(theme::surface());
    painter.drawRoundedRect(track, kRadius, kRadius);

    const bool indeterminate = (minimum() == 0 && maximum() == 0);
    const QColor accent = theme::accent();

    if (indeterminate) {
        // A band that crosses and returns: unmistakably "busy, duration unknown".
        const qreal band = track.width() * kBandWidth;
        const qreal travel = track.width() + band;
        // Triangle wave, so the band eases nowhere and simply reverses.
        const qreal t = m_phase < 0.5 ? m_phase * 2.0 : (1.0 - m_phase) * 2.0;
        const qreal left = track.left() - band + travel * t;

        QLinearGradient sweep(left, 0, left + band, 0);
        sweep.setColorAt(0.0, QColor(accent.red(), accent.green(), accent.blue(), 0));
        sweep.setColorAt(0.5, QColor(accent.red(), accent.green(), accent.blue(), 150));
        sweep.setColorAt(1.0, QColor(accent.red(), accent.green(), accent.blue(), 0));

        painter.save();
        QPainterPath clip;
        clip.addRoundedRect(track, kRadius, kRadius);
        painter.setClipPath(clip);
        painter.fillRect(QRectF(left, track.top(), band, track.height()), sweep);
        painter.restore();
    } else if (maximum() > minimum()) {
        const qreal fraction =
            qreal(value() - minimum()) / qreal(maximum() - minimum());
        const qreal filledWidth = std::max(0.0, track.width() * fraction);
        if (filledWidth > 0.0) {
            const QRectF filled(track.left(), track.top(), filledWidth, track.height());

            painter.save();
            QPainterPath clip;
            clip.addRoundedRect(track, kRadius, kRadius);
            painter.setClipPath(clip);

            painter.fillRect(filled, theme::accentDim());

            // Highlight travelling along the filled part: progress that is
            // advancing looks different from progress that has stalled.
            const qreal band = std::max(24.0, filled.width() * 0.25);
            const qreal left = filled.left() - band + (filled.width() + band) * m_phase;
            QLinearGradient sweep(left, 0, left + band, 0);
            sweep.setColorAt(0.0, QColor(accent.red(), accent.green(), accent.blue(), 0));
            sweep.setColorAt(0.5, QColor(accent.red(), accent.green(), accent.blue(), 110));
            sweep.setColorAt(1.0, QColor(accent.red(), accent.green(), accent.blue(), 0));
            painter.setClipRect(filled, Qt::IntersectClip);
            painter.fillRect(QRectF(left, filled.top(), band, filled.height()), sweep);
            painter.restore();

            painter.setPen(QPen(accent, 1));
            painter.setBrush(Qt::NoBrush);
            painter.drawLine(QPointF(filled.right(), track.top() + 2),
                             QPointF(filled.right(), track.bottom() - 2));
        }
    }

    if (isTextVisible() && !text().isEmpty()) {
        painter.setPen(theme::text());
        painter.drawText(rect(), Qt::AlignCenter, text());
    }
}
