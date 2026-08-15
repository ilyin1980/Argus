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

protected:
    /**
     * @brief Reload the manual in the new language.
     *
     * Unlike a label, a document cannot be retranslated in place: the whole
     * text is a different file, so the tabs are rebuilt.
     */
    void changeEvent(QEvent *event) override;

private:
    /** @brief Fill the tab widget with the documents for the current language. */
    void loadDocuments();

    /**
     * @brief Load one Markdown document from the resources into a new tab.
     * @param title    Tab caption.
     * @param baseName Document name without language or extension, e.g. "user-guide".
     */
    void addDocument(const QString &title, const QString &baseName);

    /**
     * @brief Pick the best translation of a document that this build carries.
     * @param baseName Document name without language or extension.
     * @return Resource path; falls back to the English original.
     *
     * A language whose interface is translated but whose manual is not still
     * gets a readable manual, in English, rather than an empty tab.
     */
    static QString documentPath(const QString &baseName);

    QTabWidget *m_tabs = nullptr;
};
