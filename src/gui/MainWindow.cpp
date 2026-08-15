#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "IndexController.h"
#include "QueryImageView.h"
#include "ResultModel.h"
#include "Theme.h"
#include "ThumbnailDelegate.h"

#include "core/BowIndex.h"
#include "core/Database.h"
#include "core/DescriptorStore.h"
#include "core/DuplicateFinder.h"
#include "core/Paths.h"
#include "core/QueryEngine.h"
#include "core/Vocabulary.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QMenuBar>
#include <QSettings>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDesktopServices>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImageReader>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QLocale>
#include <QMenu>
#include <QMessageBox>
#include <QMetaObject>
#include <QMimeData>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTabWidget>
#include <QThread>
#include <QUrl>
#include <QVBoxLayout>

#include <algorithm>
#include <utility>

namespace {

/** @brief Icon edge length used by every result grid. */
constexpr int kIconSize = 160;

/** @brief Footprint of one grid cell, preview plus two caption lines. */
const QSize kCellSize(kIconSize + 24, kIconSize + 54);

QString humanBytes(qint64 bytes)
{
    return QLocale::system().formattedDataSize(bytes, 2, QLocale::DataSizeIecFormat);
}

/** @brief Whether the clipboard currently holds pixels rather than text. */
bool clipboardHasImage()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mime = clipboard->mimeData();
    return (mime && mime->hasImage()) || !clipboard->image().isNull();
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , m_indexController(new IndexController(this))
{
    ui->setupUi(this);
    bindFormWidgets();
    wireForm();

    setWindowTitle(QStringLiteral("ImageWorker %1").arg(QLatin1String(IMAGEWORKER_VERSION)));

    m_statusLabel = new QLabel(this);
    statusBar()->addWidget(m_statusLabel);
    setStatus(QStringLiteral("Choose a folder to begin."));

    // Window-level paste. A shortcut on the Paste button alone is dead whenever
    // the query tab is not the visible page, because Qt disables shortcuts of
    // hidden widgets — which is exactly the case while browsing duplicates.
    auto *pasteAction = new QAction(QStringLiteral("Paste screenshot"), this);
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setShortcutContext(Qt::WindowShortcut);
    connect(pasteAction, &QAction::triggered, this, [this] {
        // An image on the clipboard always means "search for this": a line edit
        // could not use it anyway, and deferring to the focused text field on
        // the strength of focus alone makes Ctrl+V silently do nothing, which
        // is worse than having no shortcut at all.
        if (!clipboardHasImage()) {
            if (auto *edit = qobject_cast<QLineEdit *>(QApplication::focusWidget())) {
                edit->paste(); // plain text into a path box keeps working
                return;
            }
        }
        pasteQueryImage();
    });
    addAction(pasteAction);
    qApp->installEventFilter(this);

    connect(m_indexController, &IndexController::finished, this, &MainWindow::onIndexFinished);
    connect(m_indexController, &IndexController::message, this, &MainWindow::setStatus);
    connect(m_indexController, &IndexController::progress, this,
            [this](int done, int total, const QString &stage) {
                if (total > 0) {
                    m_progress->setRange(0, total);
                    m_progress->setValue(done);
                } else {
                    m_progress->setRange(0, 0);
                }
                m_progress->setFormat(QStringLiteral("%1  %p%").arg(stage));
            });
}

MainWindow::~MainWindow() = default;

void MainWindow::bindFormWidgets()
{
    m_rootEdit     = ui->rootEdit;
    m_storageEdit  = ui->storageEdit;
    m_indexButton  = ui->indexButton;
    m_cancelButton = ui->cancelButton;
    m_progress     = ui->progress;

    m_distanceSpin = ui->distanceSpin;
    m_exactCheck   = ui->exactCheck;
    m_nearCheck    = ui->nearCheck;
    m_findButton   = ui->findButton;
    m_groupList    = ui->groupList;
    m_groupView    = ui->groupView;

    m_queryEdit    = ui->queryEdit;
    m_queryImage   = ui->queryImage;
    m_queryHint    = ui->queryHint;
    m_methodCombo  = ui->methodCombo;
    m_topSpin      = ui->topSpin;
    m_maxDistSpin  = ui->maxDistSpin;
    m_maxDistLabel = ui->maxDistLabel;
    m_searchButton = ui->searchButton;
    m_queryView    = ui->queryView;

    m_tabs        = ui->tabs;
    m_pathEdit    = ui->pathEdit;
    m_pathCount   = ui->pathCount;
    m_copyButton  = ui->copyButton;
}

void MainWindow::wireForm()
{
    // ---- top bar ------------------------------------------------------------
    connect(m_rootEdit, &QLineEdit::editingFinished, this, [this] {
        m_root = iw::normalizeRoot(m_rootEdit->text());
        syncStorageField();
    });
    connect(ui->browseRootButton, &QPushButton::clicked, this, &MainWindow::chooseRoot);
    connect(m_indexButton, &QPushButton::clicked, this, &MainWindow::startIndexing);
    connect(m_cancelButton, &QPushButton::clicked, this, &MainWindow::cancelCurrentTask);

    connect(m_storageEdit, &QLineEdit::textEdited, this, [this] {
        m_storageIsAutomatic = false;
    });
    connect(ui->browseStorageButton, &QPushButton::clicked, this, &MainWindow::chooseStorage);
    connect(ui->defaultStorageButton, &QPushButton::clicked, this, [this] {
        m_storageIsAutomatic = true;
        syncStorageField();
        openDatabaseForRoot();
    });

    // ---- duplicates ---------------------------------------------------------
    connect(m_findButton, &QPushButton::clicked, this, &MainWindow::findDuplicates);
    connect(m_groupList, &QListWidget::currentRowChanged, this, &MainWindow::onGroupSelected);

    m_groupModel = new ResultModel(this);
    configureResultView(m_groupView, m_groupModel);

    ui->duplicatesSplitter->setStretchFactor(0, 0);
    ui->duplicatesSplitter->setStretchFactor(1, 1);

    // ---- query --------------------------------------------------------------
    connect(m_queryImage, &QueryImageView::selectionChanged, this, [this] {
        if (!m_queryImage->hasSelection()) {
            m_queryHint->setText(QStringLiteral("Searching the whole image. "
                                                "Drag a box to search for one object."));
            return;
        }
        const QSize s = m_queryImage->effectiveImage().size();
        m_queryHint->setText(QStringLiteral("Searching the selected %1 x %2 region — "
                                            "faster and more accurate than the whole frame.")
                                 .arg(s.width())
                                 .arg(s.height()));
    });

    connect(m_queryEdit, &QLineEdit::editingFinished, this,
            [this] { setQueryImage(m_queryEdit->text()); });
    connect(ui->browseQueryButton, &QPushButton::clicked, this, &MainWindow::chooseQueryImage);
    connect(ui->pasteButton, &QPushButton::clicked, this, &MainWindow::pasteQueryImage);
    ui->pasteButton->setToolTip(QStringLiteral("Paste a screenshot from the clipboard (Ctrl+V)"));

    // Populated here rather than in the form: the entries carry per-item tooltips
    // and one of them gets disabled depending on what the open index contains.
    m_methodCombo->addItem(QStringLiteral("Locate object in the image"));
    m_methodCombo->addItem(QStringLiteral("Whole-image similarity"));
    m_methodCombo->setItemData(
        0,
        QStringLiteral("Neural local features: finds an asset that is physically present in the\n"
                       "image, even small, rescaled or surrounded by clutter, and shows where.\n"
                       "Needs an index built with descriptors and a trained vocabulary."),
        Qt::ToolTipRole);
    m_methodCombo->setItemData(
        1,
        QStringLiteral("Perceptual hash: compares the picture as a whole.\n"
                       "Good for rescaled or re-encoded copies of the same picture,\n"
                       "useless for finding a small object inside a screenshot."),
        Qt::ToolTipRole);
    connect(m_methodCombo, &QComboBox::currentIndexChanged, this, [this](int index) {
        const bool hashMode = (index == 1);
        m_maxDistSpin->setVisible(hashMode);
        m_maxDistLabel->setVisible(hashMode);
    });
    m_maxDistSpin->setVisible(false);
    m_maxDistLabel->setVisible(false);

    connect(m_searchButton, &QPushButton::clicked, this, &MainWindow::runQuery);

    m_queryModel = new ResultModel(this);
    configureResultView(m_queryView, m_queryModel);

    // ---- path bar and tabs --------------------------------------------------
    connect(m_copyButton, &QPushButton::clicked, this, [this] { copySelectedPaths(true); });
    connect(m_tabs, &QTabWidget::currentChanged, this, [this] { updatePathBar(); });

    buildThemeMenu();
    buildHelpMenu();
}

void MainWindow::buildHelpMenu()
{
    QMenu *help = menuBar()->addMenu(QStringLiteral("&Help"));

    QAction *guide = help->addAction(QStringLiteral("&User guide"));
    guide->setShortcut(QKeySequence::HelpContents); // F1
    connect(guide, &QAction::triggered, this,
            [this] { openHelp(HelpDialog::Page::UserGuide); });

    QAction *cli = help->addAction(QStringLiteral("&Command line and automation"));
    connect(cli, &QAction::triggered, this,
            [this] { openHelp(HelpDialog::Page::CommandLine); });

    help->addSeparator();
    help->addAction(QStringLiteral("&About"), this, [this] {
        QMessageBox::about(
            this, QStringLiteral("About ImageWorker"),
            QStringLiteral(
                "<h3>ImageWorker %1</h3>"
                "<p>Finds which image assets appear inside a screenshot, and where; "
                "and groups duplicates in a folder.</p>"
                "<p>Ships as two executables built from one core library: "
                "<b>imageworker-gui</b> and the headless <b>imageworker</b>.</p>"
                "<p>Press <b>F1</b> for the manual.</p>")
                .arg(QLatin1String(IMAGEWORKER_VERSION)));
    });
}

void MainWindow::openHelp(HelpDialog::Page page)
{
    // One instance, reused: the manual is meant to sit open beside the window.
    if (!m_help)
        m_help = new HelpDialog(this);
    m_help->showPage(page);
}

void MainWindow::buildThemeMenu()
{
    QMenu *view = menuBar()->addMenu(QStringLiteral("&View"));
    QMenu *themes = view->addMenu(QStringLiteral("&Theme"));

    auto *group = new QActionGroup(this);
    group->setExclusive(true);

    QSettings settings;
    const theme::Id saved =
        theme::fromKey(settings.value(QStringLiteral("theme"), theme::key(theme::current()))
                           .toString());

    for (const theme::Id id : theme::all()) {
        QAction *action = themes->addAction(theme::name(id));
        action->setCheckable(true);
        action->setChecked(id == saved);
        group->addAction(action);
        connect(action, &QAction::triggered, this, [this, id] {
            theme::apply(*qApp, id);
            QSettings().setValue(QStringLiteral("theme"), theme::key(id));
            // Painters that cache nothing still need one nudge: the grids draw
            // from a pixmap cache that is theme-independent, but the pulse and
            // the sweep read colours at paint time.
            update();
            setStatus(QStringLiteral("Theme: %1").arg(theme::name(id)));
        });
    }

    if (saved != theme::current())
        theme::apply(*qApp, saved);
}

void MainWindow::configureResultView(QListView *view, ResultModel *model)
{
    view->setModel(model);
    view->setViewMode(QListView::IconMode);
    view->setIconSize(QSize(kIconSize, kIconSize));
    // Grid step and delegate cell must agree, otherwise the view spaces items
    // by one size while the delegate paints at another.
    view->setGridSize(kCellSize);
    view->setResizeMode(QListView::Adjust);
    view->setMovement(QListView::Static);
    view->setUniformItemSizes(true);
    view->setSpacing(6);
    view->setWordWrap(false);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setContextMenuPolicy(Qt::CustomContextMenu);

    auto *delegate = new ThumbnailDelegate(view);
    delegate->setCellSize(kCellSize);
    view->setItemDelegate(delegate);
    view->setMouseTracking(true); // so the copy glyph can react to hover
    connect(model, &QAbstractItemModel::modelReset, delegate, &ThumbnailDelegate::beginAppear);
    connect(delegate, &ThumbnailDelegate::copyPathRequested, this, [this](const QModelIndex &index) {
        const QString path = index.data(ResultModel::AbsolutePathRole).toString();
        if (path.isEmpty())
            return;
        QApplication::clipboard()->setText(path);
        setStatus(QStringLiteral("Copied %1").arg(path));
    });

    connect(view, &QListView::doubleClicked, this, &MainWindow::openSelected);
    connect(view, &QListView::customContextMenuRequested, this, &MainWindow::showResultMenu);
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this] { updatePathBar(); });
}

void MainWindow::updatePathBar()
{
    QListView *view = activeResultView();
    if (!view || !view->selectionModel()) {
        m_pathEdit->clear();
        m_pathCount->clear();
        m_copyButton->setEnabled(false);
        return;
    }

    const QModelIndexList selected = view->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        m_pathEdit->clear();
        m_pathCount->clear();
        m_copyButton->setEnabled(false);
        return;
    }

    m_pathEdit->setText(selected.first().data(ResultModel::AbsolutePathRole).toString());
    m_pathEdit->setCursorPosition(0);
    m_pathCount->setText(selected.size() > 1
                             ? QStringLiteral("+%1 more").arg(selected.size() - 1)
                             : QString());
    m_copyButton->setEnabled(true);
}

void MainWindow::copySelectedPaths(bool absolute)
{
    QListView *view = activeResultView();
    if (!view || !view->selectionModel())
        return;

    QModelIndexList selected = view->selectionModel()->selectedIndexes();
    if (selected.isEmpty())
        return;
    std::sort(selected.begin(), selected.end(),
              [](const QModelIndex &a, const QModelIndex &b) { return a.row() < b.row(); });

    const int role = absolute ? ResultModel::AbsolutePathRole : ResultModel::RelPathRole;
    QStringList paths;
    paths.reserve(selected.size());
    for (const QModelIndex &index : std::as_const(selected))
        paths << index.data(role).toString();

    QApplication::clipboard()->setText(paths.join(QLatin1Char('\n')));
    setStatus(QStringLiteral("Copied %1 %2 path%3 to the clipboard.")
                  .arg(paths.size())
                  .arg(absolute ? QStringLiteral("full") : QStringLiteral("relative"))
                  .arg(paths.size() == 1 ? QString() : QStringLiteral("s")));
}

QString MainWindow::automaticStorageDir() const
{
    if (m_root.isEmpty())
        return {};
    return iw::stateDirFor(m_root);
}

void MainWindow::syncStorageField()
{
    if (!m_storageIsAutomatic)
        return;
    m_storageEdit->setText(QDir::toNativeSeparators(automaticStorageDir()));
}

QString MainWindow::currentDatabasePath() const
{
    const QString dir = QDir::fromNativeSeparators(m_storageEdit->text().trimmed());
    if (dir.isEmpty())
        return {};
    return dir + QLatin1String("/index.db");
}

void MainWindow::chooseStorage()
{
    const QString start = m_storageEdit->text().isEmpty() ? m_root : m_storageEdit->text();
    const QString dir = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Choose where to keep the index"), start);
    if (dir.isEmpty())
        return;

    m_storageIsAutomatic = false;
    m_storageEdit->setText(QDir::toNativeSeparators(dir));
    openDatabaseForRoot();
}

// ---------------------------------------------------------------------------
// Root and database
// ---------------------------------------------------------------------------

void MainWindow::setRoot(const QString &root)
{
    m_root = iw::normalizeRoot(root);
    m_rootEdit->setText(m_root);
    syncStorageField();
    openDatabaseForRoot();
}

void MainWindow::setStorageDir(const QString &dir)
{
    m_storageIsAutomatic = false;
    m_storageEdit->setText(QDir::toNativeSeparators(QDir::fromNativeSeparators(dir)));
    openDatabaseForRoot();
}

void MainWindow::chooseRoot()
{
    const QString dir = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Choose a folder"), m_rootEdit->text());
    if (!dir.isEmpty())
        setRoot(dir);
}

void MainWindow::openDatabaseForRoot()
{
    m_database.reset();
    // Drop the cached finder so the next search does not answer from the old
    // index — but never while a search is using it. Clearing the key alone is
    // enough for correctness: it forces a rebuild next time, and the running
    // worker keeps a pointer that stays valid until it finishes.
    m_finderKey.clear();
    if (!m_busy)
        m_finder.reset();
    m_groupModel->clear();
    m_queryModel->clear();
    m_groupList->clear();
    m_groups.clear();

    if (m_root.isEmpty() || !QFileInfo(m_root).isDir()) {
        setStatus(QStringLiteral("Choose a folder to begin."));
        return;
    }

    m_dbPath = currentDatabasePath();
    if (m_dbPath.isEmpty()) {
        setStatus(QStringLiteral("Choose where the index should be kept."));
        return;
    }

    if (!QFileInfo::exists(m_dbPath)) {
        setStatus(QStringLiteral("No index at %1 — press Index to build one.")
                      .arg(QDir::toNativeSeparators(m_dbPath)));
        return;
    }

    QString error;
    auto database = std::make_unique<iw::Database>();
    if (!database->open(m_dbPath, &error)) {
        setStatus(QStringLiteral("Cannot open index: %1").arg(error));
        return;
    }
    m_database = std::move(database);

    m_groupModel->setSource(m_database.get(), m_root);
    m_queryModel->setSource(m_database.get(), m_root);

    refreshMethodAvailability();

    const iw::DatabaseSummary summary = m_database->summary();
    setStatus(QStringLiteral("%1 indexed files, %2%3")
                  .arg(summary.files)
                  .arg(humanBytes(summary.totalBytes))
                  .arg(neuralSearchAvailable()
                           ? QString()
                           : QStringLiteral(" — no descriptors, object location unavailable")));
}

// ---------------------------------------------------------------------------
// Indexing
// ---------------------------------------------------------------------------

void MainWindow::startIndexing()
{
    if (m_busy)
        return;

    m_root = iw::normalizeRoot(m_rootEdit->text());
    if (m_root.isEmpty() || !QFileInfo(m_root).isDir()) {
        QMessageBox::warning(this, windowTitle(), QStringLiteral("Choose an existing folder first."));
        return;
    }

    // The worker opens its own connection; ours must not hold the file.
    m_database.reset();
    m_groupModel->setSource(nullptr, m_root);
    m_queryModel->setSource(nullptr, m_root);

    syncStorageField();
    const QString storageDir = QDir::fromNativeSeparators(m_storageEdit->text().trimmed());
    if (storageDir.isEmpty() || !QDir().mkpath(storageDir)) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Cannot create the index directory:\n%1")
                                 .arg(QDir::toNativeSeparators(storageDir)));
        return;
    }
    m_dbPath = currentDatabasePath();

    iw::IndexOptions options;
    options.root   = m_root;
    options.dbPath = m_dbPath;

    // Without this the GUI could never produce a working object search: the
    // descriptors only ever got built from the command line, so the neural
    // method stayed greyed out and the window fell back to whole-image
    // similarity, which answers a completely different question.
    options.extractFeatures  = ui->featuresCheck->isChecked();
    options.featureModelPath = iw::defaultModelsDir() + QStringLiteral("/disk.onnx");

    setBusy(true, QStringLiteral("Indexing"));
    m_indexController->start(options);
}

void MainWindow::onIndexFinished(const iw::IndexStats &stats, const QString &error)
{
    setBusy(false);
    m_progress->setRange(0, 100);
    m_progress->setValue(100);

    if (!error.isEmpty()) {
        QMessageBox::warning(this, windowTitle(), error);
        setStatus(error);
        return;
    }

    // Descriptors are only half of what object search needs; the vocabulary and
    // shortlist index are the other half.
    if (stats.featured > 0) {
        buildVocabulary();
        return;
    }

    openDatabaseForRoot();
    setStatus(QStringLiteral("Indexed %1, unchanged %2, failed %3, pruned %4 — %5 s%6")
                  .arg(stats.indexed)
                  .arg(stats.skipped)
                  .arg(stats.failed)
                  .arg(stats.pruned)
                  .arg(stats.elapsedMs / 1000.0, 0, 'f', 1)
                  .arg(stats.cancelled ? QStringLiteral(" (cancelled)") : QString()));
}

void MainWindow::buildVocabulary()
{
    const QString dbPath = m_dbPath;
    setBusy(true, QStringLiteral("Training the visual vocabulary"));
    m_progress->setRange(0, 0);

    QThread *worker = QThread::create([this, dbPath] {
        QString error;
        iw::Database db;
        if (!db.open(dbPath, &error)) {
            QMetaObject::invokeMethod(this, [this, error] {
                setBusy(false);
                setStatus(error);
            }, Qt::QueuedConnection);
            return;
        }

        const QString featureDir =
            QFileInfo(dbPath).absolutePath() + QStringLiteral("/features");
        iw::DescriptorStore store;
        if (!store.open(featureDir, &error)) {
            QMetaObject::invokeMethod(this, [this, error] {
                setBusy(false);
                setStatus(error);
            }, Qt::QueuedConnection);
            return;
        }

        const QString model = QStringLiteral("disk");
        const QList<iw::FeatureRecord> records = db.allFeatures(model);

        // Sample evenly across the whole library so the vocabulary is not
        // dominated by whichever folder happened to be indexed first.
        qint64 total = 0;
        for (const iw::FeatureRecord &r : records)
            total += r.count;
        iw::VocabularyOptions vocabOptions;
        const int stride = std::max<qint64>(
            1, total / std::max(1, vocabOptions.sampleDescriptors));

        QList<float> sample;
        int dim = 0;
        qint64 seen = 0;
        for (const iw::FeatureRecord &record : records) {
            iw::FeatureLocation location;
            location.descOffset = record.descOffset;
            location.kptsOffset = record.kptsOffset;
            location.count      = record.count;
            location.dim        = record.dim;

            const iw::FeatureSet features =
                store.load(location, record.imageWidth, record.imageHeight);
            if (features.isEmpty())
                continue;
            dim = features.dim;
            for (int i = 0; i < features.count(); ++i, ++seen) {
                if (seen % stride != 0)
                    continue;
                for (int k = 0; k < dim; ++k)
                    sample.append(features.descriptors.at(i * dim + k));
            }
        }

        if (dim <= 0 || sample.isEmpty()) {
            QMetaObject::invokeMethod(this, [this] {
                setBusy(false);
                setStatus(QStringLiteral("No descriptors to train on."));
            }, Qt::QueuedConnection);
            return;
        }

        auto vocabulary = iw::Vocabulary::train(sample, dim, model, vocabOptions, &error);
        if (!vocabulary
            || !vocabulary->save(featureDir + QStringLiteral("/vocab.bin"), &error)) {
            QMetaObject::invokeMethod(this, [this, error] {
                setBusy(false);
                setStatus(QStringLiteral("Vocabulary failed: %1").arg(error));
            }, Qt::QueuedConnection);
            return;
        }

        auto index = iw::BowIndex::build(db, store, *vocabulary, model, &m_taskCancel,
                                         {}, &error);
        if (!index || !index->save(featureDir + QStringLiteral("/bow.bin"), &error)) {
            QMetaObject::invokeMethod(this, [this, error] {
                setBusy(false);
                setStatus(QStringLiteral("Shortlist index failed: %1").arg(error));
            }, Qt::QueuedConnection);
            return;
        }

        const iw::BowStats stats = index->stats();
        QMetaObject::invokeMethod(this, [this, stats] {
            setBusy(false);
            m_progress->setRange(0, 100);
            m_progress->setValue(100);
            openDatabaseForRoot();
            setStatus(QStringLiteral("Ready: %1 images searchable by object, "
                                     "%2-word vocabulary.")
                          .arg(stats.documents)
                          .arg(stats.words));
        }, Qt::QueuedConnection);
    });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void MainWindow::cancelCurrentTask()
{
    m_taskCancel.store(true, std::memory_order_relaxed);
    m_indexController->cancel();
    setStatus(QStringLiteral("Cancelling…"));
}

// ---------------------------------------------------------------------------
// Duplicates
// ---------------------------------------------------------------------------

void MainWindow::findDuplicates()
{
    if (m_busy)
        return;
    if (!m_database) {
        QMessageBox::information(this, windowTitle(),
                                 QStringLiteral("Build an index for this folder first."));
        return;
    }

    iw::DuplicateOptions options;
    options.maxDistance  = m_distanceSpin->value();
    options.includeExact = m_exactCheck->isChecked();
    options.includeNear  = m_nearCheck->isChecked();

    const QString dbPath = m_dbPath;
    m_taskCancel.store(false, std::memory_order_relaxed);
    setBusy(true, QStringLiteral("Matching"));
    m_progress->setRange(0, 0);

    // A second read-only connection: WAL lets it run alongside the UI's own.
    QThread *worker = QThread::create([this, dbPath, options] {
        iw::Database db;
        QString error;
        if (!db.open(dbPath, &error)) {
            QMetaObject::invokeMethod(this, [this, error] {
                setBusy(false);
                setStatus(error);
            }, Qt::QueuedConnection);
            return;
        }

        const iw::DuplicateReport report =
            iw::findDuplicates(db, options, &m_taskCancel,
                               [this](int done, int total, const QString &stage) {
                                   QMetaObject::invokeMethod(this, [this, done, total, stage] {
                                       if (total > 0) {
                                           m_progress->setRange(0, total);
                                           m_progress->setValue(done);
                                       }
                                       m_progress->setFormat(QStringLiteral("%1  %p%").arg(stage));
                                   }, Qt::QueuedConnection);
                               });

        QMetaObject::invokeMethod(this, [this, report] { onDuplicatesReady(report); },
                                  Qt::QueuedConnection);
    });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void MainWindow::onDuplicatesReady(const iw::DuplicateReport &report)
{
    setBusy(false);
    m_progress->setRange(0, 100);
    m_progress->setValue(100);

    m_groups = report.groups;
    m_groupList->clear();
    m_groupModel->clear();

    int number = 0;
    for (const iw::DuplicateGroup &group : std::as_const(m_groups)) {
        ++number;
        m_groupList->addItem(QStringLiteral("#%1  %2  %3 files  %4")
                                 .arg(number)
                                 .arg(group.kind == iw::GroupKind::Exact
                                          ? QStringLiteral("exact")
                                          : QStringLiteral("near d≤%1").arg(group.maxDistance))
                                 .arg(group.files.size())
                                 .arg(humanBytes(group.wastedBytes)));
    }

    QString status = QStringLiteral("%1 groups, %2 reclaimable")
                         .arg(m_groups.size())
                         .arg(humanBytes(report.wastedBytes));
    if (report.skippedBuckets > 0) {
        status += QStringLiteral(" — %1 oversized hash buckets skipped")
                      .arg(report.skippedBuckets);
    }
    if (report.cancelled)
        status += QStringLiteral(" (cancelled)");
    setStatus(status);

    if (!m_groups.isEmpty())
        m_groupList->setCurrentRow(0);
}

void MainWindow::onGroupSelected(int row)
{
    if (row < 0 || row >= m_groups.size()) {
        m_groupModel->clear();
        return;
    }
    m_groupModel->setRows(m_groups.at(row).files);
}

// ---------------------------------------------------------------------------
// Query by example
// ---------------------------------------------------------------------------

void MainWindow::chooseQueryImage()
{
    const QString file = QFileDialog::getOpenFileName(
        this, QStringLiteral("Choose a reference image"), m_root,
        QStringLiteral("Images (*.png *.jpg *.jpeg);;All files (*)"));
    if (!file.isEmpty())
        setQueryImage(file);
}

void MainWindow::setQueryImage(const QString &path)
{
    if (path.isEmpty()) {
        setQueryImage(QImage(), QString());
        return;
    }

    QImageReader reader(path);
    reader.setAutoTransform(true);
    const QImage image = reader.read();
    if (image.isNull()) {
        setStatus(QStringLiteral("Cannot read %1: %2").arg(path, reader.errorString()));
        return;
    }
    setQueryImage(image, path);
}

void MainWindow::setQueryImage(const QImage &image, const QString &label)
{
    m_queryEdit->setText(label);
    m_queryImage->setImage(image);

    if (image.isNull())
        m_queryHint->setText(QStringLiteral("No reference image yet."));
}

void MainWindow::pasteQueryImage()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QImage image = clipboard->image();
    if (image.isNull()) {
        // Some tools put a file path on the clipboard rather than pixels.
        const QMimeData *mime = clipboard->mimeData();
        if (mime && mime->hasUrls()) {
            const QString path = mime->urls().first().toLocalFile();
            if (!path.isEmpty()) {
                setQueryImage(path);
                m_tabs->setCurrentIndex(1);
                return;
            }
        }
        setStatus(QStringLiteral("The clipboard holds no image."));
        return;
    }

    setQueryImage(image, QStringLiteral("(clipboard, %1 x %2)")
                             .arg(image.width())
                             .arg(image.height()));
    m_tabs->setCurrentIndex(1);
    setStatus(QStringLiteral("Pasted a %1 x %2 screenshot — drag a box around the object.")
                  .arg(image.width())
                  .arg(image.height()));
}

bool MainWindow::neuralSearchAvailable() const
{
    if (m_dbPath.isEmpty())
        return false;
    const QString featureDir = QFileInfo(m_dbPath).absolutePath() + QStringLiteral("/features");
    return QFileInfo::exists(featureDir + QStringLiteral("/vocab.bin"))
        && QFileInfo::exists(featureDir + QStringLiteral("/bow.bin"));
}

void MainWindow::refreshMethodAvailability()
{
    if (!m_methodCombo)
        return;

    const bool available = neuralSearchAvailable();

    // Grey out the entry rather than hide it: an option that silently vanishes
    // teaches nothing, while a disabled one with a reason teaches what to run.
    if (auto *model = qobject_cast<QStandardItemModel *>(m_methodCombo->model())) {
        if (QStandardItem *item = model->item(0)) {
            item->setEnabled(available);
            if (!available) {
                item->setData(QStringLiteral(
                                  "This index has no descriptors or vocabulary yet. Build them with:\n"
                                  "  imageworker index <dir> --db <db> --features\n"
                                  "  imageworker vocab <dir> --db <db>"),
                              Qt::ToolTipRole);
            }
        }
    }

    if (!available && m_methodCombo->currentIndex() == 0) {
        m_methodCombo->setCurrentIndex(1);
        // Falling back silently is how a search for an object inside a picture
        // ends up answered by a whole-image hash, which returns plausible-looking
        // scores for completely unrelated files.
        if (m_database) {
            setStatus(QStringLiteral(
                "This index has no object-search data — press Index with "
                "\"Object search data\" ticked. Until then only whole-image "
                "similarity is available, which cannot find an object inside a picture."));
        }
    }
    else if (available && m_methodCombo->currentIndex() == 1 && m_queryModel->rowCount() == 0)
        m_methodCombo->setCurrentIndex(0);
}

void MainWindow::runQuery()
{
    if (m_busy)
        return;
    if (!m_database) {
        QMessageBox::information(this, windowTitle(),
                                 QStringLiteral("Build an index for this folder first."));
        return;
    }
    if (m_methodCombo->currentIndex() == 0 && !neuralSearchAvailable()) {
        QMessageBox::information(
            this, windowTitle(),
            QStringLiteral(
                "This index has no local-feature descriptors yet, so an object cannot be "
                "located inside an image.\n\nBuild them first:\n\n"
                "  imageworker index \"%1\" --db \"%2\" --features\n"
                "  imageworker vocab \"%1\" --db \"%2\"")
                .arg(QDir::toNativeSeparators(m_root), QDir::toNativeSeparators(m_dbPath)));
        return;
    }

    // Whatever the user boxed, or the whole image when nothing is boxed.
    const QImage reference = m_queryImage->effectiveImage();
    if (reference.isNull()) {
        QMessageBox::warning(this, windowTitle(),
                             QStringLiteral("Paste a screenshot with Ctrl+V, drop an image, "
                                            "or pick a file first."));
        return;
    }

    const QString dbPath = m_dbPath;
    m_queryImage->setMatchOutline({});

    // ---- neural: locate the asset inside the image --------------------------
    if (m_methodCombo->currentIndex() == 0) {
        iw::FindOptions findOptions;
        findOptions.topK = m_topSpin->value();

        // Measured on a 1280x720 screenshot holding one known asset:
        //   whole frame, shortlist 200 -> 12.3 s, 123 inliers
        //   whole frame, shortlist  60 ->  3.8 s, correct answer MISSING
        //   boxed object, shortlist 30 ->  2.2 s, 249 inliers
        // A cropped query spends all of its descriptors on the object instead
        // of the background, so the bag-of-words ranking becomes trustworthy
        // and a short shortlist stops being a gamble. Over the whole frame it
        // is not, so the full 200 stays.
        findOptions.shortlist = m_queryImage->hasSelection() ? 40 : 200;
        // Left empty on purpose: ObjectFinder resolves both, and picks a matcher
        // export this machine's execution provider can actually run.

        m_taskCancel.store(false, std::memory_order_relaxed);
        setBusy(true, QStringLiteral("Locating"));
        m_progress->setRange(0, 0);

        // Identity of what a cached finder was built for. Anything in here
        // changing means the cached models and indexes no longer apply.
        const QString finderKey = dbPath + QLatin1Char('|') + findOptions.extractorPath
                                  + QLatin1Char('|') + findOptions.matcherPath;

        QThread *finderWorker = QThread::create([this, dbPath, reference, findOptions, finderKey] {
            QString error;
            if (!m_finder || m_finderKey != finderKey) {
                m_finder = iw::ObjectFinder::create(dbPath, findOptions, &error);
                m_finderKey = m_finder ? finderKey : QString();
            }
            iw::ObjectFinder *finder = m_finder.get();
            if (!finder) {
                QMetaObject::invokeMethod(this, [this, error] {
                    setBusy(false);
                    setStatus(error);
                    QMessageBox::warning(this, windowTitle(), error);
                }, Qt::QueuedConnection);
                return;
            }

            const QList<iw::FindResult> results =
                finder->find(reference, findOptions, &m_taskCancel,
                             [this](int done, int total) {
                                 QMetaObject::invokeMethod(this, [this, done, total] {
                                     m_progress->setRange(0, total);
                                     m_progress->setValue(done);
                                     m_progress->setFormat(QStringLiteral("verifying  %p%"));
                                 }, Qt::QueuedConnection);
                             },
                             &error);

            QMetaObject::invokeMethod(this, [this, results] { onFindReady(results); },
                                      Qt::QueuedConnection);
        });
        connect(finderWorker, &QThread::finished, finderWorker, &QObject::deleteLater);
        finderWorker->start();
        return;
    }

    // ---- perceptual hash: whole-image similarity ----------------------------
    iw::QueryOptions options;
    options.topK        = m_topSpin->value();
    options.maxDistance = m_maxDistSpin->value();

    setBusy(true, QStringLiteral("Searching"));
    m_progress->setRange(0, 0);

    QThread *worker = QThread::create([this, dbPath, reference, options] {
        iw::Database db;
        QString error;
        if (!db.open(dbPath, &error)) {
            QMetaObject::invokeMethod(this, [this, error] {
                setBusy(false);
                setStatus(error);
            }, Qt::QueuedConnection);
            return;
        }

        QString queryError;
        const iw::QueryResult result = iw::queryByImage(db, reference, options, &queryError);
        QMetaObject::invokeMethod(this, [this, result, queryError] {
            if (!queryError.isEmpty()) {
                setBusy(false);
                setStatus(queryError);
                return;
            }
            onQueryReady(result);
        }, Qt::QueuedConnection);
    });
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    worker->start();
}

void MainWindow::onQueryReady(const iw::QueryResult &result)
{
    setBusy(false);
    m_progress->setRange(0, 100);
    m_progress->setValue(100);

    m_queryModel->setMetricLabel(QStringLiteral("distance"));
    m_queryModel->setMatches(result.matches);

    // Say what the numbers mean. A distance past ~30 is noise however confident
    // the score column looks, and users read the top row as "the answer".
    const int best = result.matches.isEmpty() ? 128 : result.matches.first().distance;
    QString verdict;
    if (best <= 8)
        verdict = QStringLiteral("the same picture");
    else if (best <= 20)
        verdict = QStringLiteral("plausibly the same picture, re-encoded or rescaled");
    else
        verdict = QStringLiteral("nothing convincing — these are far apart, "
                                 "whatever the scores suggest");

    setStatus(QStringLiteral("%1 hits of %2 indexed files, closest distance %3: %4. "
                             "Whole-image similarity, not object location.")
                  .arg(result.matches.size())
                  .arg(result.scannedRows)
                  .arg(best)
                  .arg(verdict));
    m_tabs->setCurrentIndex(1);
}

void MainWindow::onFindReady(const QList<iw::FindResult> &results)
{
    setBusy(false);
    m_progress->setRange(0, 100);
    m_progress->setValue(100);

    QList<iw::Match> asMatches;
    asMatches.reserve(results.size());
    for (const iw::FindResult &r : results) {
        iw::Match m;
        m.file     = r.file;
        m.score    = r.score;
        m.distance = r.inliers; // the grid subtitle shows it as the evidence count
        asMatches.append(m);
    }
    m_queryModel->setMetricLabel(QStringLiteral("inliers"));
    m_queryModel->setMatches(asMatches);

    if (results.isEmpty()) {
        m_queryImage->setMatchOutline({});
        setStatus(QStringLiteral("Nothing located. Try boxing the object, or check that the "
                                 "asset is actually indexed."));
        return;
    }

    // Show where the best result sits. The outline is in the coordinates of the
    // image that was searched, which is the crop when the user boxed one.
    const iw::FindResult &best = results.first();
    QPolygonF outline = best.outline;
    if (m_queryImage->hasSelection()) {
        const QPointF origin = m_queryImage->selectionOrigin();
        for (QPointF &p : outline)
            p += origin;
    }
    m_queryImage->setMatchOutline(outline, QFileInfo(best.file.rel).fileName());

    setStatus(QStringLiteral("Located %1 asset%2; best: %3 (%4 inliers, %5 consistent)")
                  .arg(results.size())
                  .arg(results.size() == 1 ? QString() : QStringLiteral("s"))
                  .arg(QFileInfo(best.file.rel).fileName())
                  .arg(best.inliers)
                  .arg(QStringLiteral("%1%").arg(qRound(best.inlierRatio * 100.0))));
    m_tabs->setCurrentIndex(1);
}

// ---------------------------------------------------------------------------
// Result actions
// ---------------------------------------------------------------------------

QListView *MainWindow::activeResultView() const
{
    return m_tabs->currentIndex() == 0 ? m_groupView : m_queryView;
}

void MainWindow::openSelected(const QModelIndex &index)
{
    auto *model = qobject_cast<const ResultModel *>(index.model());
    if (!model)
        return;
    const iw::FileInfoRow row = model->rowAt(index);
    if (row.rel.isEmpty())
        return;
    QDesktopServices::openUrl(QUrl::fromLocalFile(iw::absolutePathFor(m_root, row.rel)));
}

void MainWindow::showResultMenu(const QPoint &position)
{
    QListView *view = activeResultView();
    if (!view)
        return;
    const QModelIndex index = view->indexAt(position);
    if (!index.isValid())
        return;

    auto *model = qobject_cast<const ResultModel *>(index.model());
    if (!model)
        return;
    const iw::FileInfoRow row = model->rowAt(index);
    const QString absolute = iw::absolutePathFor(m_root, row.rel);

    QMenu menu(this);
    menu.addAction(QStringLiteral("Open"), this, [absolute] {
        QDesktopServices::openUrl(QUrl::fromLocalFile(absolute));
    });
    menu.addAction(QStringLiteral("Reveal in file manager"), this, [absolute] {
        iw::revealInFileManager(absolute);
    });
    menu.addSeparator();
    menu.addAction(QStringLiteral("Use as reference image"), this, [this, absolute] {
        setQueryImage(absolute);
        m_tabs->setCurrentIndex(1);
    });
    menu.addSeparator();
    menu.addAction(QStringLiteral("Copy full path"), this, [this] { copySelectedPaths(true); });
    menu.addAction(QStringLiteral("Copy path relative to the root"), this,
                   [this] { copySelectedPaths(false); });
    menu.exec(view->viewport()->mapToGlobal(position));
}

// ---------------------------------------------------------------------------
// Window plumbing
// ---------------------------------------------------------------------------

void MainWindow::setBusy(bool busy, const QString &what)
{
    m_busy = busy;
    m_indexButton->setEnabled(!busy);
    m_findButton->setEnabled(!busy);
    m_searchButton->setEnabled(!busy);
    m_cancelButton->setEnabled(busy);
    if (busy && !what.isEmpty())
        setStatus(what + QStringLiteral("…"));
}

void MainWindow::setStatus(const QString &text)
{
    m_statusLabel->setText(text);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride) {
        auto *key = static_cast<QKeyEvent *>(event);
        if (key->matches(QKeySequence::Paste) && clipboardHasImage()) {
            event->ignore();
            return true; // swallow the override, leaving it unaccepted
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
        return;

    const QString path = urls.first().toLocalFile();
    if (path.isEmpty())
        return;

    if (QFileInfo(path).isDir()) {
        setRoot(path);
    } else {
        setQueryImage(path);
        m_tabs->setCurrentIndex(1);
    }
    event->acceptProposedAction();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_taskCancel.store(true, std::memory_order_relaxed);
    m_indexController->cancel();
    event->accept();
}
