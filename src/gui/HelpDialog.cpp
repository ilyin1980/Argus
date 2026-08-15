#include "HelpDialog.h"

#include "Theme.h"

#include <QFile>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("ImageWorker — Help"));
    // A manual you cannot keep open beside the window you are learning is half
    // a manual, so this is a plain window rather than a modal dialog.
    setWindowFlag(Qt::Window, true);
    resize(820, 700);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabs = new QTabWidget(this);
    layout->addWidget(m_tabs);

    addDocument(tr("Using ImageWorker"), QStringLiteral(":/help/user-guide.md"));
    addDocument(tr("Command line"), QStringLiteral(":/help/cli-reference.md"));
}

void HelpDialog::addDocument(const QString &title, const QString &resourcePath)
{
    auto *browser = new QTextBrowser(m_tabs);
    browser->setOpenExternalLinks(true);
    browser->setFrameShape(QFrame::NoFrame);
    browser->document()->setDocumentMargin(18);

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

void HelpDialog::showPage(Page page)
{
    m_tabs->setCurrentIndex(page == Page::CommandLine ? 1 : 0);
    show();
    raise();
    activateWindow();
}
