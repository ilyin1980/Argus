/**
 * @file BranchDialog.h
 * @brief Choosing which git branches an index should cover.
 */
#pragma once

#include <QDialog>
#include <QStringList>

class QCheckBox;
class QLabel;
class QListWidget;

/**
 * @brief Lists the branches of a repository and remembers which are indexed.
 *
 * The checked-out branch is deliberately absent: it is the working tree, which
 * every index already holds, and offering it would store each file twice under
 * two names for the same bytes.
 */
class BranchDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Build the dialog for one repository.
     * @param topLevel   Work tree root.
     * @param currentRef Checked-out branch, excluded from the list.
     * @param selected   Branches already in the index.
     * @param parent     Parent widget.
     */
    BranchDialog(const QString &topLevel,
                 const QString &currentRef,
                 const QStringList &selected,
                 QWidget *parent = nullptr);

    /** @return Branches the user ticked, in list order. */
    QStringList selectedBranches() const;

private:
    /** @brief Fill the list, keeping ticks that survive the new listing. */
    void reload();

    QString      m_topLevel;
    QString      m_currentRef;
    QStringList  m_selected;
    QListWidget *m_list    = nullptr;
    QCheckBox   *m_remotes = nullptr;
    QLabel      *m_summary = nullptr;
};
