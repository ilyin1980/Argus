#include "Theme.h"

#include <QApplication>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>
#include <QWidget>

namespace theme {

namespace {

Id      g_current = Id::SciFi;
Palette g_palette;

/// Captured before the first theme is applied, so Id::System can be restored
/// exactly rather than approximated.
QPalette g_systemPalette;
QString  g_systemStyle;
bool     g_systemCaptured = false;

Palette sciFiPalette()
{
    Palette p;
    p.background = QColor("#080b10");
    p.surface    = QColor("#111924");
    p.outline    = QColor("#1e2c3a");
    p.accent     = QColor("#35e0c8");
    p.accentDim  = QColor("#186b62");
    p.text       = QColor("#c9d8e6");
    p.mutedText  = QColor("#7a8b9c");
    p.warning    = QColor("#ffb454");
    p.dark       = true;
    return p;
}

Palette materialPalette()
{
    // Material's light surface set, with the indigo accent and the grey ramp
    // the spec uses for dividers and secondary text.
    Palette p;
    p.background = QColor("#f5f5f5");
    p.surface    = QColor("#ffffff");
    p.outline    = QColor("#dcdcdc");
    p.accent     = QColor("#3f51b5");
    p.accentDim  = QColor("#c5cae9");
    p.text       = QColor("#212121");
    p.mutedText  = QColor("#757575");
    p.warning    = QColor("#f57c00");
    p.dark       = false;
    return p;
}

Palette blossomPalette()
{
    // Warm and light, but held back: one dusty rose accent against near-neutral
    // surfaces. Saturated pink everywhere would fight the thumbnails, which are
    // the actual subject of this window.
    Palette p;
    p.background = QColor("#fbf4f7");
    p.surface    = QColor("#ffffff");
    p.outline    = QColor("#efdae3");
    p.accent     = QColor("#c96d95");
    p.accentDim  = QColor("#f3d5e2");
    p.text       = QColor("#4b3a42");
    p.mutedText  = QColor("#9a838d");
    p.warning    = QColor("#d98324");
    p.dark       = false;
    return p;
}

/** @brief Derive the role set from whatever palette the desktop gave us. */
Palette systemPalette(const QPalette &source)
{
    Palette p;
    p.background = source.color(QPalette::Window);
    p.surface    = source.color(QPalette::Base);
    p.outline    = source.color(QPalette::Mid);
    p.accent     = source.color(QPalette::Highlight);
    p.accentDim  = source.color(QPalette::Highlight).lighter(160);
    p.text       = source.color(QPalette::WindowText);
    p.mutedText  = source.color(QPalette::Disabled, QPalette::WindowText);
    p.warning    = QColor("#d9822b");
    p.dark       = source.color(QPalette::Window).lightness() < 128;
    return p;
}

Palette paletteFor(Id id)
{
    switch (id) {
    case Id::SciFi:    return sciFiPalette();
    case Id::Material: return materialPalette();
    case Id::Blossom:  return blossomPalette();
    case Id::System:   break;
    }
    return systemPalette(g_systemPalette);
}

/** @brief Turn a role set into a QPalette. */
QPalette toQPalette(const Palette &p)
{
    QPalette q;
    q.setColor(QPalette::Window,          p.background);
    q.setColor(QPalette::WindowText,      p.text);
    q.setColor(QPalette::Base,            p.surface);
    q.setColor(QPalette::AlternateBase,   p.background);
    q.setColor(QPalette::Text,            p.text);
    q.setColor(QPalette::Button,          p.surface);
    q.setColor(QPalette::ButtonText,      p.text);
    q.setColor(QPalette::BrightText,      p.warning);
    q.setColor(QPalette::Highlight,       p.accent);
    q.setColor(QPalette::HighlightedText, p.dark ? p.background : p.surface);
    q.setColor(QPalette::ToolTipBase,     p.surface);
    q.setColor(QPalette::ToolTipText,     p.text);
    q.setColor(QPalette::PlaceholderText, p.mutedText);
    q.setColor(QPalette::Link,            p.accent);
    q.setColor(QPalette::Mid,             p.outline);

    q.setColor(QPalette::Disabled, QPalette::Text,       p.mutedText);
    q.setColor(QPalette::Disabled, QPalette::ButtonText, p.mutedText);
    q.setColor(QPalette::Disabled, QPalette::WindowText, p.mutedText);

    // Selection in an unfocused view must stay legible: the item delegate reads
    // the Inactive group, and a washed-out pair there is exactly how captions
    // turn invisible.
    q.setColor(QPalette::Inactive, QPalette::Highlight,       p.accentDim);
    q.setColor(QPalette::Inactive, QPalette::HighlightedText, p.text);
    return q;
}

/** @brief One stylesheet, filled from the role set. */
QString styleSheetFor(const Palette &p)
{
    return QStringLiteral(R"(
        QWidget { background: %1; color: %5; font-size: 10pt; }
        QLabel, QCheckBox { background: transparent; }

        QLineEdit, QSpinBox, QComboBox {
            background: %2; border: 1px solid %3; border-radius: 3px;
            padding: 4px 6px;
            selection-background-color: %4; selection-color: %8;
        }
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus { border: 1px solid %4; }
        QLineEdit:read-only { color: %6; }

        QComboBox::drop-down { border: none; width: 18px; }
        QComboBox QAbstractItemView {
            background: %2; border: 1px solid %4;
            selection-background-color: %4; selection-color: %8;
        }

        QPushButton {
            background: %2; border: 1px solid %3; border-radius: 3px;
            padding: 5px 14px;
        }
        QPushButton:hover   { border: 1px solid %4; color: %4; }
        QPushButton:pressed { background: %7; }
        QPushButton:disabled{ color: %6; }

        QTabWidget::pane { border: 1px solid %3; border-radius: 3px; top: -1px; }
        QTabBar::tab {
            background: transparent; border: 1px solid transparent;
            border-bottom: 2px solid transparent; padding: 6px 16px; color: %6;
        }
        QTabBar::tab:hover    { color: %5; }
        QTabBar::tab:selected { color: %4; border-bottom: 2px solid %4; }

        QListView, QListWidget {
            background: %2; border: 1px solid %3; border-radius: 3px; outline: none;
        }
        QListWidget::item { padding: 3px 6px; }
        QListWidget::item:selected { background: %7; color: %5; }

        QCheckBox { spacing: 6px; }
        QCheckBox::indicator {
            width: 13px; height: 13px; border: 1px solid %3;
            border-radius: 2px; background: %2;
        }
        QCheckBox::indicator:checked { background: %4; border: 1px solid %4; }

        QSplitter::handle { background: %3; width: 3px; }
        QSplitter::handle:hover { background: %4; }

        QScrollBar:vertical, QScrollBar:horizontal { background: transparent; border: none; }
        QScrollBar:vertical   { width: 10px; }
        QScrollBar:horizontal { height: 10px; }
        QScrollBar::handle { background: %3; border-radius: 4px; min-height: 24px; min-width: 24px; }
        QScrollBar::handle:hover { background: %7; }
        QScrollBar::add-line, QScrollBar::sub-line { height: 0; width: 0; }
        QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }

        QMenuBar { background: %1; }
        QMenuBar::item { padding: 5px 10px; background: transparent; }
        QMenuBar::item:selected { color: %4; }

        QMenu { background: %2; border: 1px solid %3; padding: 4px; }
        QMenu::item { padding: 5px 22px; }
        QMenu::item:selected { background: %7; color: %5; }
        QMenu::separator { height: 1px; background: %3; margin: 4px 8px; }

        QStatusBar { background: %1; color: %6; border-top: 1px solid %3; }
        QStatusBar::item { border: none; }

        QToolTip { background: %2; color: %5; border: 1px solid %4; padding: 4px; }
    )")
        .arg(p.background.name(),
             p.surface.name(),
             p.outline.name(),
             p.accent.name(),
             p.text.name(),
             p.mutedText.name(),
             p.accentDim.name(),
             (p.dark ? p.background : p.surface).name());
}

} // namespace

QList<Id> all()
{
    return { Id::System, Id::SciFi, Id::Material, Id::Blossom };
}

QString name(Id id)
{
    // Translated on every call, never cached: the menu is rebuilt when the
    // language changes and must come back in the new one.
    switch (id) {
    case Id::System:   return QCoreApplication::translate("theme", "System");
    case Id::SciFi:    return QCoreApplication::translate("theme", "Sci-fi");
    case Id::Material: return QCoreApplication::translate("theme", "Material");
    case Id::Blossom:  return QCoreApplication::translate("theme", "Blossom");
    }
    return {};
}

QString key(Id id)
{
    switch (id) {
    case Id::System:   return QStringLiteral("system");
    case Id::SciFi:    return QStringLiteral("scifi");
    case Id::Material: return QStringLiteral("material");
    case Id::Blossom:  return QStringLiteral("blossom");
    }
    return {};
}

Id fromKey(const QString &value)
{
    for (Id id : all()) {
        if (key(id) == value)
            return id;
    }
    return Id::SciFi;
}

Id current() { return g_current; }

const Palette &palette()
{
    if (g_palette.background.isValid())
        return g_palette;
    static const Palette fallback = sciFiPalette();
    return fallback;
}

void apply(QApplication &app, Id id)
{
    if (!g_systemCaptured) {
        g_systemPalette = app.palette();
        g_systemStyle   = app.style() ? app.style()->name() : QString();
        g_systemCaptured = true;
    }

    g_current = id;
    g_palette = paletteFor(id);

    if (id == Id::System) {
        if (!g_systemStyle.isEmpty())
            app.setStyle(QStyleFactory::create(g_systemStyle));
        app.setStyleSheet(QString());
        app.setPalette(g_systemPalette);
    } else {
        // Fusion, because the native Windows style ignores much of a stylesheet
        // and paints its own chrome around what it does honour.
        app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
        app.setPalette(toQPalette(g_palette));
        app.setStyleSheet(styleSheetFor(g_palette));
    }

    // Custom painters read the roles at paint time, so a repaint is all they
    // need; stylesheet changes alone do not reach them.
    const auto widgets = app.allWidgets();
    for (QWidget *w : widgets)
        w->update();
}

QColor background() { return palette().background; }
QColor surface()    { return palette().surface; }
QColor outline()    { return palette().outline; }
QColor accent()     { return palette().accent; }
QColor accentDim()  { return palette().accentDim; }
QColor text()       { return palette().text; }
QColor mutedText()  { return palette().mutedText; }
QColor warning()    { return palette().warning; }

} // namespace theme
