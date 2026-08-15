/**
 * @file Theme.h
 * @brief Selectable looks, kept in one place and switchable at run time.
 *
 * Every theme is the same set of eight roles filled with different colours, and
 * one parametric stylesheet renders all of them. That is what makes adding a
 * look a matter of naming colours rather than writing another stylesheet, and
 * what keeps the custom painters — which read these roles at paint time —
 * correct under every theme without knowing which one is active.
 */
#pragma once

#include <QColor>
#include <QList>
#include <QString>

class QApplication;

namespace theme {

/** @brief The available looks. */
enum class Id {
    System,   ///< Whatever the desktop provides; no stylesheet at all.
    SciFi,    ///< Dark instrument panel, single teal accent.
    Material, ///< Light, flat, indigo accent.
    Blossom,  ///< Light, warm, restrained dusty rose.
};

/** @brief The eight roles every theme fills. */
struct Palette {
    QColor background; ///< Window and page background.
    QColor surface;    ///< Panels, list backgrounds, input fields.
    QColor outline;    ///< Hairlines and idle borders.
    QColor accent;     ///< Focus, selection, progress, located objects.
    QColor accentDim;  ///< Accent at low intensity, for fills and idle glows.
    QColor text;       ///< Ordinary text.
    QColor mutedText;  ///< Subtitles, hints, disabled captions.
    QColor warning;    ///< Reserved, so the accent never has to shout.
    bool   dark = true; ///< Lets painters pick contrasting overlays.
};

/** @return Every selectable theme, in menu order. */
QList<Id> all();

/**
 * @brief Human-readable name for a theme.
 * @param id Theme to name.
 * @return Name suitable for a menu entry.
 */
QString name(Id id);

/**
 * @brief Stable string used to persist a choice.
 * @param id Theme to encode.
 * @return Lowercase identifier.
 */
QString key(Id id);

/**
 * @brief Decode a persisted identifier.
 * @param key Value previously produced by @ref key.
 * @return The matching theme, or Id::SciFi when the key is unknown.
 */
Id fromKey(const QString &key);

/** @return The theme currently applied. */
Id current();

/** @return Colours of the theme currently applied. */
const Palette &palette();

/**
 * @brief Apply a theme to a running application.
 * @param app The application instance.
 * @param id  Theme to apply.
 * @note Safe to call while windows are open: every widget is repainted, and the
 *       custom painters read @ref palette on each paint rather than caching it.
 */
void apply(QApplication &app, Id id);

/// @name Shorthand accessors used by the custom painters.
/// @{
QColor background();
QColor surface();
QColor outline();
QColor accent();
QColor accentDim();
QColor text();
QColor mutedText();
QColor warning();
/// @}

} // namespace theme
