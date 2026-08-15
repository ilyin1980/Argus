/**
 * @file MainWindow.h
 * @brief The whole GUI: root selection, indexing, duplicates, query by example.
 */
#pragma once

#include <QMainWindow>

#include <atomic>
#include <memory>

#include "HelpDialog.h"

#include "core/ObjectFinder.h"
#include "core/QueryEngine.h"
#include "core/Types.h"

class IndexController;
class QueryImageView;
class ResultModel;

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QListView;
class QListWidget;
class QProgressBar;
class QPushButton;
class QSpinBox;
class QTabWidget;

namespace iw {
class Database;
}

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Main application window.
 *
 * Holds no search logic: every operation is a call into imageworker_core, run
 * on a worker thread so that a pass over a large index never freezes the UI.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    /**
     * @brief Select the directory to work on and open its index if one exists.
     * @param root Directory path in any platform notation.
     */
    void setRoot(const QString &root);

    /**
     * @brief Point the index at a directory outside the image folder.
     * @param dir Directory holding @c index.db, previews and descriptors.
     * @note Marks the storage as user-chosen, so it stops following the root.
     */
    void setStorageDir(const QString &dir);

protected:
    /**
     * @brief Stop text fields from swallowing Ctrl+V when an image is pending.
     *
     * QLineEdit claims Ctrl+V through QEvent::ShortcutOverride, which switches
     * off any window-level shortcut for the same key before it can run. When
     * the clipboard holds an image the line edit has nothing to paste, so the
     * override is dropped here and the window action goes ahead.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void chooseRoot();
    void chooseStorage();
    void startIndexing();
    void cancelCurrentTask();
    void onIndexFinished(const iw::IndexStats &stats, const QString &error);

    /**
     * @brief Train the vocabulary and shortlist index after descriptors appear.
     *
     * Descriptors alone do not make object search work; without this step the
     * neural method stays greyed out and the window quietly falls back to
     * whole-image similarity, which answers a different question entirely.
     */
    void buildVocabulary();

    void findDuplicates();
    void onDuplicatesReady(const iw::DuplicateReport &report);
    void onGroupSelected(int row);

    void chooseQueryImage();
    void pasteQueryImage();
    void runQuery();
    void onQueryReady(const iw::QueryResult &result);
    void onFindReady(const QList<iw::FindResult> &results);

    void openSelected(const QModelIndex &index);
    void showResultMenu(const QPoint &position);
    void updatePathBar();
    void copySelectedPaths(bool absolute);

private:
    /**
     * @brief Bind the members below to the widgets the form created.
     *
     * The layout lives in MainWindow.ui so it can be edited in Qt Designer;
     * these aliases keep the rest of this class reading as plain member access
     * rather than @c ui->something at every use.
     */
    void bindFormWidgets();

    /** @brief Populate and wire the parts of the form that are data-driven. */
    void wireForm();

    /** @brief Build View > Theme, restoring and persisting the user's choice. */
    void buildThemeMenu();

    /** @brief Build the Help menu. */
    void buildHelpMenu();

    /**
     * @brief Show the manual.
     * @param page Which document to bring to the front.
     */
    void openHelp(HelpDialog::Page page);

    /** @brief Wire a result grid to the shared delegate, path bar and actions. */
    void configureResultView(QListView *view, ResultModel *model);

    void openDatabaseForRoot();

    /** @brief Where the index would live if the user has not chosen a location. */
    QString automaticStorageDir() const;

    /** @brief Refresh the storage field after the root changed, unless it was overridden. */
    void syncStorageField();

    /** @return Database file implied by the current storage directory. */
    QString currentDatabasePath() const;

    void setBusy(bool busy, const QString &what = {});
    void setStatus(const QString &text);
    void setQueryImage(const QString &path);
    void setQueryImage(const QImage &image, const QString &label);
    QListView *activeResultView() const;

    // Top bar
    QLineEdit    *m_rootEdit     = nullptr;
    QLineEdit    *m_storageEdit  = nullptr;
    QPushButton  *m_indexButton  = nullptr;
    QPushButton  *m_cancelButton = nullptr;
    QProgressBar *m_progress     = nullptr;

    /// False once the user picks a storage directory by hand, after which the
    /// field stops following the image folder.
    bool m_storageIsAutomatic = true;

    // Duplicates tab
    QSpinBox    *m_distanceSpin = nullptr;
    QCheckBox   *m_exactCheck   = nullptr;
    QCheckBox   *m_nearCheck    = nullptr;
    QPushButton *m_findButton   = nullptr;
    QListWidget *m_groupList    = nullptr;
    QListView   *m_groupView    = nullptr;
    ResultModel *m_groupModel   = nullptr;

    // Query tab
    QLineEdit      *m_queryEdit  = nullptr;
    QueryImageView *m_queryImage = nullptr;
    QLabel         *m_queryHint  = nullptr;
    QSpinBox       *m_topSpin    = nullptr;
    QComboBox   *m_methodCombo  = nullptr;
    QSpinBox    *m_maxDistSpin  = nullptr;
    QLabel      *m_maxDistLabel = nullptr;
    QPushButton *m_searchButton = nullptr;
    QListView   *m_queryView    = nullptr;
    ResultModel *m_queryModel   = nullptr;

    /** @return @c true when the index carries descriptors and a vocabulary. */
    bool neuralSearchAvailable() const;

    /** @brief Enable or disable the neural option to match the current index. */
    void refreshMethodAvailability();

    QTabWidget *m_tabs        = nullptr;
    QLabel     *m_statusLabel = nullptr;

    // Shared path bar under the tabs
    QLineEdit   *m_pathEdit    = nullptr;
    QPushButton *m_copyButton  = nullptr;
    QLabel      *m_pathCount   = nullptr;

    std::unique_ptr<Ui::MainWindow> ui;

    HelpDialog *m_help = nullptr;

    IndexController              *m_indexController = nullptr;
    std::unique_ptr<iw::Database> m_database;

    /// Kept between searches: constructing one loads two ONNX models and spends
    /// seconds compiling shaders, which is pure waste on every search after the
    /// first. Discarded whenever the index or the models it was built for change.
    std::unique_ptr<iw::ObjectFinder> m_finder;
    QString                           m_finderKey;
    QString                       m_root;
    QString                       m_dbPath;

    QList<iw::DuplicateGroup> m_groups;
    std::atomic_bool          m_taskCancel{false};
    bool                      m_busy = false;
};
