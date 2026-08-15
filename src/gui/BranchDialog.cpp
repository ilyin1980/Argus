#include "BranchDialog.h"

#include "core/GitRepo.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

BranchDialog::BranchDialog(const QString &topLevel,
                           const QString &currentRef,
                           const QStringList &selected,
                           QWidget *parent)
    : QDialog(parent)
    , m_topLevel(topLevel)
    , m_currentRef(currentRef)
    , m_selected(selected)
{
    setWindowTitle(tr("Branches to index"));
    resize(460, 520);

    auto *layout = new QVBoxLayout(this);

    auto *hint = new QLabel(
        tr("Files are read straight out of the repository, so nothing is checked "
           "out and the working tree is never touched. The branch you have "
           "checked out is not listed: it is the working tree, which the index "
           "already covers."),
        this);
    hint->setWordWrap(true);
    layout->addWidget(hint);

    m_list = new QListWidget(this);
    m_list->setSelectionMode(QAbstractItemView::NoSelection);
    layout->addWidget(m_list, 1);

    m_remotes = new QCheckBox(tr("Include remote-tracking branches"), this);
    layout->addWidget(m_remotes);
    connect(m_remotes, &QCheckBox::toggled, this, [this] { reload(); });

    m_summary = new QLabel(this);
    m_summary->setWordWrap(true);
    layout->addWidget(m_summary);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    // Clearing every tick is how a user drops branches from the index, so it
    // deserves a button rather than a hunt down the list.
    QPushButton *none = buttons->addButton(tr("Select none"), QDialogButtonBox::ResetRole);
    connect(none, &QPushButton::clicked, this, [this] {
        for (int i = 0; i < m_list->count(); ++i)
            m_list->item(i)->setCheckState(Qt::Unchecked);
    });
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(buttons);

    reload();
}

void BranchDialog::reload()
{
    // Ticks the user has just made outrank the set the dialog opened with.
    if (m_list->count() > 0) {
        QStringList ticked;
        for (int i = 0; i < m_list->count(); ++i) {
            if (m_list->item(i)->checkState() == Qt::Checked)
                ticked << m_list->item(i)->text();
        }
        m_selected = ticked;
    }

    QString error;
    const QStringList all = iw::git::branches(m_topLevel, m_remotes->isChecked(), &error);

    m_list->clear();
    for (const QString &name : all) {
        if (name == m_currentRef)
            continue;
        auto *item = new QListWidgetItem(name, m_list);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(m_selected.contains(name) ? Qt::Checked : Qt::Unchecked);
    }

    if (all.isEmpty()) {
        m_summary->setText(error.isEmpty() ? tr("This repository has no branches.")
                                           : tr("Cannot list branches: %1").arg(error));
        return;
    }

    // Branches selected earlier that this listing no longer contains would
    // otherwise vanish without a word, taking their rows with them.
    QStringList missing;
    for (const QString &name : std::as_const(m_selected)) {
        if (!all.contains(name) && name != m_currentRef)
            missing << name;
    }

    QString text = tr("%n branch(es) available.", "", m_list->count());
    if (!missing.isEmpty()) {
        text += QLatin1Char(' ')
              + tr("Indexed earlier but gone from the repository: %1. Pressing OK "
                   "drops them from the index.")
                    .arg(missing.join(QStringLiteral(", ")));
    }
    m_summary->setText(text);
}

QStringList BranchDialog::selectedBranches() const
{
    QStringList out;
    for (int i = 0; i < m_list->count(); ++i) {
        if (m_list->item(i)->checkState() == Qt::Checked)
            out << m_list->item(i)->text();
    }
    return out;
}
