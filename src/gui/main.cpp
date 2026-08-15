/**
 * @file main.cpp
 * @brief GUI entry point.
 *
 * The window is a view over the same core library the CLI drives; it holds no
 * search logic of its own. An optional directory argument preselects a root so
 * the GUI can be launched straight at a folder.
 */

#include <QApplication>
#include <QIcon>
#include <QCommandLineParser>
#include <QSettings>

#include "Localization.h"
#include "MainWindow.h"
#include "Theme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // Compiled in, not loaded from disk: an icon that can go missing is an
    // icon that will, in exactly the packaged build nobody tests.
    app.setWindowIcon(QIcon(QStringLiteral(":/branding/argus-icon-256.png")));
    QCoreApplication::setApplicationName(QStringLiteral("Argus"));
    QCoreApplication::setOrganizationName(QStringLiteral("Argus"));
    QCoreApplication::setApplicationVersion(QStringLiteral(ARGUS_VERSION));

    // Before the parser, so even --help comes out in the user's language, and
    // before any window exists, so nothing is ever seen in the wrong one.
    i18n::install(app, i18n::savedCode());

    // Applied before the first window exists, so nothing is ever seen unstyled.
    theme::apply(app, theme::fromKey(
                          QSettings().value(QStringLiteral("theme"),
                                            theme::key(theme::Id::SciFi)).toString()));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Find duplicate and similar images."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("dir"),
                                 QStringLiteral("Directory to open on startup."),
                                 QStringLiteral("[dir]"));
    parser.addOption({ QStringLiteral("index"),
                       QStringLiteral("Directory holding the index, when it is not "
                                      "inside the image folder."),
                       QStringLiteral("dir") });
    parser.process(app);

    MainWindow window;
    const QStringList positional = parser.positionalArguments();
    if (!positional.isEmpty())
        window.setRoot(positional.first());
    if (parser.isSet(QStringLiteral("index")))
        window.setStorageDir(parser.value(QStringLiteral("index")));

    window.resize(1280, 820);
    window.show();
    return app.exec();
}
