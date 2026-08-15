/**
 * @file Localization.h
 * @brief Language selection: which catalogues exist, and how to switch at runtime.
 */
#pragma once

#include <QString>
#include <QList>

class QCoreApplication;

namespace i18n {

/** @brief One entry of the Language menu. */
struct Language {
    QString code;   ///< Qt locale name: "uk", "pt_BR"; empty means follow the system.
    QString endonym; ///< The language's name in itself — the only name its speakers scan for.
};

/**
 * @brief Every language this build can switch to.
 * @return System default first, then the shipped catalogues in endonym order.
 * @note A catalogue that failed to compile is simply absent from the list, so
 *       the menu never offers a language that would silently do nothing.
 */
QList<Language> available();

/**
 * @brief The language the user last chose.
 * @return Locale name, or an empty string when following the system.
 */
QString savedCode();

/**
 * @brief The catalogue actually in force.
 * @return Locale name of the loaded catalogue, or an empty string for English.
 * @note Differs from savedCode() in two ways that matter: it resolves "follow
 *       the system" to a real language, and it names what was actually found
 *       rather than what was asked for. Anything picking a translated file off
 *       disk — the manual, for one — has to ask this and not the setting.
 */
QString activeCode();

/**
 * @brief Load a language and make every open window redraw its text in it.
 * @param app  The running application.
 * @param code Locale name, or an empty string to follow the system locale.
 * @return @c true when the catalogue loaded; @c false leaves the source
 *         language in place, which is English.
 * @note Persists the choice, so the next start comes up in the same language.
 */
bool install(QCoreApplication &app, const QString &code);

} // namespace i18n
