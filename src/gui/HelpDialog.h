/**
 * @file HelpDialog.h
 * @brief The built-in manual.
 *
 * The text is compiled into the binary rather than shipped beside it, so the
 * help is present in a packaged folder, on a machine with no network, and after
 * a user has moved the executable somewhere unexpected.
 */
#pragma once

#include <QDialog>

class QTabWidget;

/** @brief Non-modal window showing the user guide and the command-line reference. */
class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    /** @brief Which document to show first. */
    enum class Page {
        UserGuide,
        CommandLine,
    };

    explicit HelpDialog(QWidget *parent = nullptr);

    /**
     * @brief Bring the requested page to the front.
     * @param page Document to show.
     */
    void showPage(Page page);

private:
    /**
     * @brief Load one Markdown document from the resources into a new tab.
     * @param title        Tab caption.
     * @param resourcePath Path inside the compiled resources.
     */
    void addDocument(const QString &title, const QString &resourcePath);

    QTabWidget *m_tabs = nullptr;
};
