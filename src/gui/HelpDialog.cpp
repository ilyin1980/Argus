#include "HelpDialog.h"

#include "Localization.h"
#include "Theme.h"

#include <QEvent>
#include <QFile>
#include <QFileInfo>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

#include <algorithm>

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Argus — Help"));
    // A manual you cannot keep open beside the window you are learning is half
    // a manual, so this is a plain window rather than a modal dialog.
    setWindowFlag(Qt::Window, true);
    resize(820, 700);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabs = new QTabWidget(this);
    layout->addWidget(m_tabs);

    loadDocuments();
}

QString HelpDialog::documentPath(const QString &baseName)
{
    const QString code = i18n::activeCode();
    if (!code.isEmpty()) {
        const QString exact =
            QStringLiteral(":/help/%1.%2.md").arg(baseName, code);
        if (QFileInfo::exists(exact))
            return exact;

        // pt_BR falls back to a pt manual before falling back to English.
        const QString language = code.section(QLatin1Char('_'), 0, 0);
        if (language != code) {
            const QString broader =
                QStringLiteral(":/help/%1.%2.md").arg(baseName, language);
            if (QFileInfo::exists(broader))
                return broader;
        }
    }
    return QStringLiteral(":/help/%1.md").arg(baseName);
}

void HelpDialog::loadDocuments()
{
    m_tabs->clear();
    addDocument(tr("Using Argus"), QStringLiteral("user-guide"));
    addDocument(tr("Command line"), QStringLiteral("cli-reference"));
}

void HelpDialog::addDocument(const QString &title, const QString &baseName)
{
    auto *browser = new QTextBrowser(m_tabs);
    browser->setOpenExternalLinks(true);
    browser->setFrameShape(QFrame::NoFrame);
    browser->document()->setDocumentMargin(18);

    const QString resourcePath = documentPath(baseName);
    QFile file(resourcePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        browser->setMarkdown(QString::fromUtf8(file.readAll()));
    } else {
        // Should never happen with compiled-in resources, but a blank tab would
        // leave the reader guessing about what went wrong.
        browser->setPlainText(tr("Could not load %1.").arg(resourcePath));
    }

    m_tabs->addTab(browser, title);
}

void HelpDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        const int current = m_tabs->currentIndex();
        setWindowTitle(tr("Argus — Help"));
        loadDocuments();
        m_tabs->setCurrentIndex(std::max(0, current));
    }
    QDialog::changeEvent(event);
}

void HelpDialog::showPage(Page page)
{
    m_tabs->setCurrentIndex(page == Page::CommandLine ? 1 : 0);
    show();
    raise();
    activateWindow();
}
