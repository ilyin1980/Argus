#include "Localization.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QTranslator>

namespace i18n {

namespace {

/** @brief Where the compiled catalogues live inside the binary. */
const QLatin1String kCatalogueDir(":/i18n");

/** @brief Settings key holding the chosen locale name. */
const QLatin1String kSettingsKey("language");

/**
 * @brief The languages this project translates, in the order they are offered.
 *
 * Endonyms rather than English names: someone looking for their own language
 * scans for the word they write it with, not for "Ukrainian".
 */
struct Entry { const char *code; const char *endonym; };
const Entry kEntries[] = {
    { "en",    "English"          },
    { "uk",    "Українська"       },
    { "de",    "Deutsch"          },
    { "es",    "Español"          },
    { "fr",    "Français"         },
    { "it",    "Italiano"         },
    { "nl",    "Nederlands"       },
    { "pl",    "Polski"           },
    { "pt_BR", "Português (Brasil)" },
    { "tr",    "Türkçe"           },
    { "ar",    "العربية"           },
    { "zh_CN", "简体中文"          },
    { "ja",    "日本語"            },
    { "ko",    "한국어"            },
};

/// Live for as long as the application: Qt keeps pointers to installed
/// translators and reads them on every tr() call.
QTranslator *g_app = nullptr;
QTranslator *g_qt  = nullptr;

/** @brief Resource path of a catalogue, whether or not it exists. */
QString cataloguePath(const QString &code)
{
    return kCatalogueDir + QStringLiteral("/imageworker_") + code + QStringLiteral(".qm");
}

/**
 * @brief Pick the catalogue that best serves a locale.
 * @param code Locale name such as "pt_BR" or "de_AT".
 * @return Name of a catalogue that exists, or an empty string.
 *
 * A German-speaking Austrian gets the German catalogue rather than English,
 * which is the whole point of falling back on the language part.
 */
QString resolve(const QString &code)
{
    if (code.isEmpty())
        return {};
    if (QFileInfo::exists(cataloguePath(code)))
        return code;

    const QString language = code.section(QLatin1Char('_'), 0, 0);
    if (language != code && QFileInfo::exists(cataloguePath(language)))
        return language;

    // The other direction: asked for "pt", shipped "pt_BR".
    for (const Entry &entry : kEntries) {
        const QString candidate = QLatin1String(entry.code);
        if (candidate.section(QLatin1Char('_'), 0, 0) == language
            && QFileInfo::exists(cataloguePath(candidate))) {
            return candidate;
        }
    }
    return {};
}

} // namespace

QList<Language> available()
{
    QList<Language> out;
    out.append({ QString(), QString() }); // system default; the menu names it

    for (const Entry &entry : kEntries) {
        const QString code = QLatin1String(entry.code);
        // English is the source language: it needs no catalogue and always works.
        if (code != QLatin1String("en") && !QFileInfo::exists(cataloguePath(code)))
            continue;
        out.append({ code, QString::fromUtf8(entry.endonym) });
    }
    return out;
}

QString savedCode()
{
    return QSettings().value(kSettingsKey, QString()).toString();
}

bool install(QCoreApplication &app, const QString &code)
{
    QSettings().setValue(kSettingsKey, code);

    const QString wanted = code.isEmpty() ? QLocale::system().name() : code;
    const QString resolved = resolve(wanted);

    // Removing first, and unconditionally, is what makes switching back to
    // English work: with no catalogue to load there is nothing to replace the
    // old one, and the window would keep the previous language.
    if (g_app) {
        app.removeTranslator(g_app);
        delete g_app;
        g_app = nullptr;
    }
    if (g_qt) {
        app.removeTranslator(g_qt);
        delete g_qt;
        g_qt = nullptr;
    }

    if (resolved.isEmpty() || resolved == QLatin1String("en"))
        return resolved == QLatin1String("en") || code.isEmpty();

    auto *translator = new QTranslator;
    if (!translator->load(cataloguePath(resolved))) {
        delete translator;
        return false;
    }
    app.installTranslator(translator);
    g_app = translator;

    // Qt's own dialogs — the file picker, the message boxes' buttons — carry
    // their own catalogue. Best effort: a build without it still works, it just
    // says "Cancel" in English inside an otherwise translated dialog.
    auto *qtTranslator = new QTranslator;
    const QString qtDir = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
    if (qtTranslator->load(QStringLiteral("qtbase_") + resolved, qtDir)
        || qtTranslator->load(QStringLiteral("qtbase_") + resolved.section(QLatin1Char('_'), 0, 0),
                              qtDir)) {
        app.installTranslator(qtTranslator);
        g_qt = qtTranslator;
    } else {
        delete qtTranslator;
    }
    return true;
}

} // namespace i18n
