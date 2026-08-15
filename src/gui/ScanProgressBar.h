/**
 * @file ScanProgressBar.h
 * @brief Progress bar that reads as an instrument sweep rather than a bar.
 *
 * Two states, both animated, because the window spends minutes in each and a
 * frozen bar is indistinguishable from a hung program:
 *  - determinate: the filled part carries a travelling highlight;
 *  - indeterminate: a single band sweeps back and forth across the track.
 */
#pragma once

#include <QProgressBar>

class QVariantAnimation;

/** @brief Custom-painted progress bar; ignores the stylesheet by design. */
class ScanProgressBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(qreal phase READ phase WRITE setPhase)

public:
    explicit ScanProgressBar(QWidget *parent = nullptr);
    ~ScanProgressBar() override;

    /** @return Animation position, 0..1, wrapping continuously. */
    qreal phase() const { return m_phase; }

    /**
     * @brief Move the sweep.
     * @param value Position in 0..1.
     */
    void setPhase(qreal value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    /** @brief Run the sweep only while it can be seen and has work to show. */
    void updateAnimationState();

    QVariantAnimation *m_animation = nullptr;
    qreal              m_phase     = 0.0;
};
