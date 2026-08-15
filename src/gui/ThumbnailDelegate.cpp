#include "ThumbnailDelegate.h"

#include "ResultModel.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QVariantAnimation>

#include <algorithm>

namespace {

/// Breathing room inside a cell, in pixels.
constexpr int kPadding = 6;

/// Gap between the preview and the caption.
constexpr int kGap = 4;

/// Entrance animation: total length, per-cell delay and the cap on that delay.
constexpr int   kAppearMs    = 520;
constexpr qreal kStaggerStep = 0.018;
constexpr qreal kMaxStagger  = 0.45;

/// How far a cell slides up as it fades in.
constexpr qreal kAppearRise = 10.0;

/// Edge length of the copy glyph beside the file name.
constexpr int kCopyIcon = 14;

} // namespace

ThumbnailDelegate::ThumbnailDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    m_appear = new QVariantAnimation(this);
    m_appear->setStartValue(0.0);
    m_appear->setEndValue(1.0);
    m_appear->setDuration(kAppearMs);
    m_appear->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_appear, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        m_appearAt = v.toReal();
        // The delegate cannot repaint itself; ask the view that owns it.
        if (auto *view = qobject_cast<QAbstractItemView *>(this->parent()))
            view->viewport()->update();
    });
}

void ThumbnailDelegate::beginAppear()
{
    m_appear->stop();
    m_appearAt = 0.0;
    m_appear->start();
}

qreal ThumbnailDelegate::appearOpacity(int row) const
{
    if (m_appearAt >= 1.0)
        return 1.0;

    // Each cell starts a little after the one before it, but the total lead-in
    // is capped: with two hundred results nobody waits for the last one.
    const qreal lead = std::min(kMaxStagger, row * kStaggerStep);
    const qreal local = (m_appearAt - lead) / (1.0 - kMaxStagger);
    return std::clamp(local, 0.0, 1.0);
}

void ThumbnailDelegate::setCellSize(const QSize &cell)
{
    m_cell = cell;
}

int ThumbnailDelegate::textHeight(const QStyleOptionViewItem &option) const
{
    const QFontMetrics titleMetrics(option.font);
    QFont small = option.font;
    small.setPointSizeF(std::max(6.0, option.font.pointSizeF() - 1.0));
    const QFontMetrics subtitleMetrics(small);
    return titleMetrics.height() + subtitleMetrics.height() + kGap;
}

QRect ThumbnailDelegate::copyIconRect(const QStyleOptionViewItem &option) const
{
    const QRect cell = option.rect.adjusted(kPadding, kPadding, -kPadding, -kPadding);
    const int captionHeight = textHeight(option);
    const QFontMetrics titleMetrics(option.font);
    const int titleTop = cell.bottom() - captionHeight + kGap;
    return QRect(cell.right() - kCopyIcon,
                 titleTop + (titleMetrics.height() - kCopyIcon) / 2,
                 kCopyIcon, kCopyIcon);
}

void ThumbnailDelegate::paintCopyIcon(QPainter *painter, const QRect &rect, const QColor &colour) const
{
    // Two offset sheets: the universal "copy" shorthand, drawn rather than
    // shipped as an asset so it follows the palette in any theme.
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(colour, 1.2));
    painter->setBrush(Qt::NoBrush);

    const QRectF back(rect.left(), rect.top(), rect.width() - 4.0, rect.height() - 4.0);
    const QRectF front(rect.left() + 4.0, rect.top() + 4.0, rect.width() - 4.0, rect.height() - 4.0);
    painter->drawRoundedRect(back, 2.0, 2.0);
    painter->fillRect(front.adjusted(1, 1, -1, -1), painter->background());
    painter->drawRoundedRect(front, 2.0, 2.0);
    painter->restore();
}

bool ThumbnailDelegate::editorEvent(QEvent *event,
                                    QAbstractItemModel *model,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        const auto *mouse = static_cast<QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton
            && copyIconRect(option).contains(mouse->position().toPoint())) {
            emit copyPathRequested(index);
            return true; // consumed: the click must not also change the selection
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize ThumbnailDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    // Deliberately constant: the whole point of this delegate is that no item
    // can grow out of its grid cell.
    return m_cell;
}

void ThumbnailDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    painter->save();

    const qreal appear = appearOpacity(index.row());
    if (appear <= 0.0) {
        painter->restore();
        return;
    }
    if (appear < 1.0) {
        painter->setOpacity(appear);
        painter->translate(0.0, (1.0 - appear) * kAppearRise);
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    // The base class would draw the icon and text itself; we only want its
    // background and focus handling.
    opt.text.clear();
    opt.icon = QIcon();
    opt.features &= ~QStyleOptionViewItem::HasDecoration;

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    const QRect cell = option.rect.adjusted(kPadding, kPadding, -kPadding, -kPadding);
    const int captionHeight = textHeight(option);
    const QRect imageRect(cell.left(), cell.top(),
                          cell.width(), std::max(0, cell.height() - captionHeight - kGap));

    const QPixmap pixmap = qvariant_cast<QPixmap>(index.data(Qt::DecorationRole));
    if (!pixmap.isNull() && imageRect.height() > 0) {
        QSize target = pixmap.size();
        target.scale(imageRect.size(), Qt::KeepAspectRatio);
        const QRect drawn(imageRect.left() + (imageRect.width()  - target.width())  / 2,
                          imageRect.top()  + (imageRect.height() - target.height()) / 2,
                          target.width(), target.height());
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter->drawPixmap(drawn, pixmap);
    }

    // Pick the colour the way the style itself would. Using HighlightedText on
    // the strength of State_Selected alone paints white on white as soon as the
    // view loses focus, because the matching highlight background is drawn from
    // the Inactive group — or not at all.
    const bool selected = option.state & QStyle::State_Selected;
    QPalette::ColorGroup group = (option.state & QStyle::State_Enabled) ? QPalette::Normal
                                                                       : QPalette::Disabled;
    if (group == QPalette::Normal && !(option.state & QStyle::State_Active))
        group = QPalette::Inactive;
    const QPalette::ColorRole textRole = selected ? QPalette::HighlightedText : QPalette::Text;

    // Title: the file name, elided in the middle so both the beginning and the
    // extension stay readable.
    const QFontMetrics titleMetrics(option.font);
    // Leave room for the copy glyph so a long name never runs under it.
    const QRect titleRect(cell.left(), imageRect.bottom() + kGap,
                          cell.width() - kCopyIcon - 2, titleMetrics.height());
    painter->setFont(option.font);
    painter->setPen(option.palette.color(group, textRole));
    painter->drawText(titleRect, Qt::AlignHCenter | Qt::AlignVCenter,
                      titleMetrics.elidedText(index.data(Qt::DisplayRole).toString(),
                                              Qt::ElideMiddle, titleRect.width()));

    // The glyph is always present so its position is predictable, but stays
    // faint until the pointer is over the cell.
    QColor iconColour = option.palette.color(group, textRole);
    iconColour.setAlphaF((option.state & QStyle::State_MouseOver) ? 0.95f : 0.35f);
    paintCopyIcon(painter, copyIconRect(option), iconColour);

    // Subtitle: score, distance or dimensions, whichever the model supplies.
    const QString subtitle = index.data(ResultModel::SubtitleRole).toString();
    if (!subtitle.isEmpty()) {
        QFont small = option.font;
        small.setPointSizeF(std::max(6.0, option.font.pointSizeF() - 1.0));
        const QFontMetrics subtitleMetrics(small);
        const QRect subtitleRect(cell.left(), titleRect.bottom(),
                                 cell.width(), subtitleMetrics.height());

        QColor colour = option.palette.color(group, textRole);
        if (!selected)
            colour.setAlphaF(0.65f);
        painter->setFont(small);
        painter->setPen(colour);
        painter->drawText(subtitleRect, Qt::AlignHCenter | Qt::AlignVCenter,
                          subtitleMetrics.elidedText(subtitle, Qt::ElideRight, subtitleRect.width()));
    }

    painter->restore();
}
