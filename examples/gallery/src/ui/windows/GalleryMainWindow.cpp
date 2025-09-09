/**
 * QtLucide Gallery Application - Consolidated Main Window Implementation
 */

#include "GalleryMainWindow.h"
#include "../widgets/grids/IconGridWidget.h"
#include "../widgets/grids/IconThumbnailGridWidget.h"
#include "../widgets/panels/IconDetailsPanel.h"
#include "../widgets/panels/CategorySidebarWidget.h"
#include "../widgets/search/SearchWidget.h"
#include "../widgets/search/IconSearchWidget.h"
#include "../widgets/search/CategoryFilterWidget.h"
#include "../../core/managers/IconMetadataManager.h"
#include "../../core/managers/ImageMetadataManager.h"
#include "../../core/managers/ContentManager.h"
#include "../../core/managers/FavoritesManager.h"
#include "../widgets/viewers/ImageViewerWidget.h"
#include "FileBrowserWidget.h"
#include "PreferencesDialog.h"
#include "IconExportDialog.h"
#include "../../core/utils/GalleryLogger.h"
#include "../../config/LayoutConfig.h"

#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QTimer>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>
#include <QShortcut>
#include <QActionGroup>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

GalleryMainWindow::GalleryMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_lucide(nullptr)
    , m_metadataManager(nullptr)
    , m_imageMetadataManager(nullptr)
    , m_contentManager(nullptr)
    , m_favoritesManager(nullptr)
    , m_centralWidget(nullptr)
    , m_stackedWidget(nullptr)
    , m_viewStack(nullptr)
    , m_mainSplitter(nullptr)
    , m_rightSplitter(nullptr)
    , m_leftSplitter(nullptr)
    , m_contentSplitter(nullptr)
    , m_searchWidget(nullptr)
    , m_iconSearchWidget(nullptr)
    , m_categoryFilter(nullptr)
    , m_categorySidebar(nullptr)
    , m_iconGrid(nullptr)
    , m_thumbnailGrid(nullptr)
    , m_detailsPanel(nullptr)
    , m_imageViewer(nullptr)
    , m_fileBrowser(nullptr)
    , m_mainToolbar(nullptr)
    , m_viewToolbar(nullptr)
    , m_progressBar(nullptr)
    , m_statusLabel(nullptr)
    , m_iconCountLabel(nullptr)
    , m_currentIconLabel(nullptr)
    , m_exportDialogEnhanced(nullptr)
    , m_settingsDialog(nullptr)
    , m_viewModeGroup(nullptr)
    , m_themeGroup(nullptr)
    , m_toggleFavoriteAction(nullptr)
    , m_fullscreenAction(nullptr)
    , m_currentViewModeEnum(SplitView)
    , m_currentTheme(SystemTheme)
    , m_thumbnailSize(64)
    , m_slideshowInterval(3000)
    , m_showTooltips(true)
    , m_autoSaveSettings(true)
    , m_settings(new QSettings(this))
    , m_statusUpdateTimer(new QTimer(this))
    , m_slideshowTimer(new QTimer(this))
{
    GALLERY_LOG_INFO(galleryInit, "Consolidated GalleryMainWindow constructor started");
    GALLERY_START_TIMER("MainWindow initialization");

    setWindowTitle("QtLucide Enhanced Icon & Image Gallery");

    // Enhanced window sizing with responsive constraints
    setMinimumSize(800, 600);  // Minimum usable size
    resize(1200, 800);         // Optimal default size

    // Set size policy for better responsive behavior
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Enable drag and drop
    setAcceptDrops(true);

    GALLERY_LOG_DEBUG(galleryInit, "Window properties set");

    // Initialize core components first
    GALLERY_LOG_INFO(galleryInit, "Initializing components");
    initializeComponents();

    // Initialize icon system
    GALLERY_LOG_INFO(galleryInit, "Initializing icon system");
    initializeIconSystem();

    // Setup UI
    GALLERY_LOG_INFO(galleryInit, "Setting up UI components");
    setupUI();
    setupMenuBar();
    setupToolBars();
    setupStatusBar();
    setupShortcuts();

    GALLERY_LOG_INFO(galleryInit, "Setting up connections");
    setupConnections();

    // Load settings and restore state
    GALLERY_LOG_INFO(galleryInit, "Loading settings");
    loadSettings();
    applyTheme();

    // Setup status update timer
    GALLERY_LOG_DEBUG(galleryInit, "Setting up status update timer");
    m_statusUpdateTimer->setSingleShot(true);
    m_statusUpdateTimer->setInterval(500);
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &GalleryMainWindow::updateStatusBar);

    // Set application icon
    GALLERY_LOG_DEBUG(galleryInit, "Setting up application icon");
    setupApplicationIcon();

    GALLERY_END_TIMER("MainWindow initialization");
    GALLERY_LOG_INFO(galleryInit, "GalleryMainWindow constructor completed");
    GALLERY_LOG_MEMORY("After MainWindow construction");
}

GalleryMainWindow::~GalleryMainWindow()
{
    saveSettings();
}

void GalleryMainWindow::initializeComponents()
{
    GALLERY_LOG_INFO(galleryInit, "Initializing core components");

    // Create core managers (only those available in current build)
    if (!m_lucide) {
        m_lucide = new lucide::QtLucide(this);
    }

    // Only create managers that are available in the current build configuration
    if (!m_metadataManager) {
        m_metadataManager = new IconMetadataManager(this);
    }

    // Note: ContentManager, ImageMetadataManager, and FavoritesManager
    // are only created if they're available in the build configuration
    // This allows the consolidated window to work in different build contexts

    GALLERY_LOG_INFO(galleryInit, "Core components initialized");
}

void GalleryMainWindow::initializeIconSystem()
{
    GALLERY_START_TIMER("Icon system initialization");

    // Create QtLucide instance
    GALLERY_LOG_DEBUG(galleryInit, "Creating QtLucide instance");
    m_lucide = new lucide::QtLucide(this);

    GALLERY_LOG_INFO(galleryInit, "Initializing QtLucide");
    if (!m_lucide->initLucide()) {
        QString errorMsg = "Failed to initialize QtLucide icon system";
        GALLERY_LOG_CRITICAL(galleryInit, errorMsg);
        QMessageBox::critical(this, "Initialization Error", errorMsg);
        return;
    }

    int iconCount = m_lucide->availableIcons().size();
    GALLERY_LOG_INFO(galleryInit, QString("QtLucide initialized successfully with %1 icons").arg(iconCount));

    // Create metadata manager
    GALLERY_LOG_DEBUG(galleryInit, "Creating IconMetadataManager");
    m_metadataManager = new IconMetadataManager(this);

    // Connect metadata manager signals
    GALLERY_LOG_DEBUG(galleryInit, "Connecting metadata manager signals");
    connect(m_metadataManager, &IconMetadataManager::metadataLoaded,
            this, &GalleryMainWindow::onIconsLoaded);
    connect(m_metadataManager, &IconMetadataManager::metadataLoadFailed,
            this, [this](const QString& error) {
                GALLERY_LOG_CRITICAL(galleryMetadata, QString("Metadata loading failed: %1").arg(error));
                QMessageBox::warning(this, "Loading Error",
                                    QString("Failed to load icon metadata: %1").arg(error));
            });
    connect(m_metadataManager, &IconMetadataManager::filteredIconsChanged,
            this, [this](const QStringList& iconNames) {
                GALLERY_LOG_DEBUG(gallerySearch, QString("Filtered icons changed: %1 icons").arg(iconNames.size()));
                onFilteredIconsChanged(iconNames.size(), m_metadataManager->getTotalIconCount());
            });

    // Start loading metadata
    GALLERY_LOG_INFO(galleryInit, "Starting metadata loading");
    GALLERY_START_TIMER("Metadata loading");
    m_metadataManager->loadMetadata();

    GALLERY_END_TIMER("Icon system initialization");
}

void GalleryMainWindow::setupUI()
{
    createActions();
    setupMenuBar();
    setupToolBars();
    setupStatusBar();
    setupCentralWidget();
}

void GalleryMainWindow::createActions()
{
    // File menu actions
    m_aboutAction = new QAction("&About QtLucide Gallery", this);
    m_aboutAction->setStatusTip("Show information about QtLucide Gallery");
    m_aboutAction->setShortcut(QKeySequence("F1"));

    m_aboutQtAction = new QAction("About &Qt", this);
    m_aboutQtAction->setStatusTip("Show information about Qt");

    m_preferencesAction = new QAction("&Preferences...", this);
    m_preferencesAction->setShortcut(QKeySequence::Preferences);
    m_preferencesAction->setStatusTip("Open application preferences");

    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Exit the application");

    // View menu actions
    m_fullscreenAction = new QAction("&Fullscreen", this);
    m_fullscreenAction->setShortcut(QKeySequence::FullScreen);
    m_fullscreenAction->setCheckable(true);
    m_fullscreenAction->setStatusTip("Toggle fullscreen mode");

    m_toggleSidebarAction = new QAction("Show &Sidebar", this);
    m_toggleSidebarAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
    m_toggleSidebarAction->setCheckable(true);
    m_toggleSidebarAction->setChecked(true);
    m_toggleSidebarAction->setStatusTip("Toggle category filter sidebar");

    m_toggleDetailsPanelAction = new QAction("Show &Details Panel", this);
    m_toggleDetailsPanelAction->setShortcut(QKeySequence("Ctrl+Shift+D"));
    m_toggleDetailsPanelAction->setCheckable(true);
    m_toggleDetailsPanelAction->setChecked(true);
    m_toggleDetailsPanelAction->setStatusTip("Toggle icon details panel");

    m_toggleStatusBarAction = new QAction("Show Status &Bar", this);
    m_toggleStatusBarAction->setCheckable(true);
    m_toggleStatusBarAction->setChecked(true);
    m_toggleStatusBarAction->setStatusTip("Toggle status bar");

    // Grid size actions
    m_gridSizeActionGroup = new QActionGroup(this);
    QAction* smallGridAction = new QAction("&Small Icons", m_gridSizeActionGroup);
    QAction* mediumGridAction = new QAction("&Medium Icons", m_gridSizeActionGroup);
    QAction* largeGridAction = new QAction("&Large Icons", m_gridSizeActionGroup);

    smallGridAction->setCheckable(true);
    mediumGridAction->setCheckable(true);
    largeGridAction->setCheckable(true);
    mediumGridAction->setChecked(true);

    smallGridAction->setShortcut(QKeySequence("Ctrl+1"));
    mediumGridAction->setShortcut(QKeySequence("Ctrl+2"));
    largeGridAction->setShortcut(QKeySequence("Ctrl+3"));

    smallGridAction->setData(32);
    mediumGridAction->setData(64);
    largeGridAction->setData(96);

    // Export/Import actions
    m_exportAction = new QAction("&Export Icons...", this);
    m_exportAction->setShortcut(QKeySequence("Ctrl+E"));
    m_exportAction->setStatusTip("Export selected icons");

    m_importFavoritesAction = new QAction("&Import Favorites...", this);
    m_importFavoritesAction->setStatusTip("Import favorites from file");

    m_exportFavoritesAction = new QAction("Export &Favorites...", this);
    m_exportFavoritesAction->setStatusTip("Export favorites to file");

    // Icon actions
    m_copyNameAction = new QAction("Copy &Name", this);
    m_copyNameAction->setShortcut(QKeySequence("Ctrl+C"));
    m_copyNameAction->setStatusTip("Copy icon name to clipboard");
    m_copyNameAction->setEnabled(false);

    m_copyCodeAction = new QAction("Copy &Code", this);
    m_copyCodeAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
    m_copyCodeAction->setStatusTip("Copy icon code to clipboard");
    m_copyCodeAction->setEnabled(false);

    m_copySvgAction = new QAction("Copy &SVG", this);
    m_copySvgAction->setShortcut(QKeySequence("Ctrl+Alt+C"));
    m_copySvgAction->setStatusTip("Copy icon SVG to clipboard");
    m_copySvgAction->setEnabled(false);

    m_toggleFavoriteAction = new QAction("Add to &Favorites", this);
    m_toggleFavoriteAction->setShortcut(QKeySequence("Ctrl+F"));
    m_toggleFavoriteAction->setStatusTip("Toggle favorite status");
    m_toggleFavoriteAction->setEnabled(false);

    m_clearFiltersAction = new QAction("&Clear All Filters", this);
    m_clearFiltersAction->setShortcut(QKeySequence("Ctrl+Shift+X"));
    m_clearFiltersAction->setStatusTip("Clear all search filters");

    // Additional navigation shortcuts
    QAction* focusSearchAction = new QAction("Focus Search", this);
    focusSearchAction->setShortcut(QKeySequence("Ctrl+K"));
    focusSearchAction->setStatusTip("Focus on search field");
    connect(focusSearchAction, &QAction::triggered, this, [this]() {
        if (m_searchWidget) {
            m_searchWidget->setFocus();
        }
    });
    addAction(focusSearchAction);

    QAction* viewModeAction = new QAction("Toggle View Mode", this);
    viewModeAction->setShortcut(QKeySequence("Ctrl+M"));
    viewModeAction->setStatusTip("Cycle through view modes");
    connect(viewModeAction, &QAction::triggered, this, &GalleryMainWindow::onChangeViewMode);
    addAction(viewModeAction);

    QAction* zoomInAction = new QAction("Zoom In", this);
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    zoomInAction->setStatusTip("Increase icon size");
    connect(zoomInAction, &QAction::triggered, this, [this]() {
        if (m_iconGrid) {
            m_iconGrid->zoomIn();
        }
    });
    addAction(zoomInAction);

    QAction* zoomOutAction = new QAction("Zoom Out", this);
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    zoomOutAction->setStatusTip("Decrease icon size");
    connect(zoomOutAction, &QAction::triggered, this, [this]() {
        if (m_iconGrid) {
            m_iconGrid->zoomOut();
        }
    });
    addAction(zoomOutAction);

    QAction* resetZoomAction = new QAction("Reset Zoom", this);
    resetZoomAction->setShortcut(QKeySequence("Ctrl+0"));
    resetZoomAction->setStatusTip("Reset icon size to default");
    connect(resetZoomAction, &QAction::triggered, this, [this]() {
        if (m_iconGrid) {
            m_iconGrid->resetZoom();
        }
    });
    addAction(resetZoomAction);

    QAction* refreshAction = new QAction("Refresh", this);
    refreshAction->setShortcut(QKeySequence::Refresh);
    refreshAction->setStatusTip("Refresh icon grid");
    connect(refreshAction, &QAction::triggered, this, [this]() {
        if (m_iconGrid) {
            m_iconGrid->refreshIcons();
        }
    });
    addAction(refreshAction);
}

void GalleryMainWindow::setupMenuBar()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(m_exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_importFavoritesAction);
    fileMenu->addAction(m_exportFavoritesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_preferencesAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    // Edit menu
    QMenu* editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(m_copyNameAction);
    editMenu->addAction(m_copyCodeAction);
    editMenu->addAction(m_copySvgAction);
    editMenu->addSeparator();
    editMenu->addAction(m_toggleFavoriteAction);
    editMenu->addSeparator();
    editMenu->addAction(m_clearFiltersAction);

    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction(m_toggleSidebarAction);
    viewMenu->addAction(m_toggleDetailsPanelAction);
    viewMenu->addAction(m_toggleStatusBarAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_fullscreenAction);
    viewMenu->addSeparator();

    QMenu* gridSizeMenu = viewMenu->addMenu("Icon &Size");
    for (QAction* action : m_gridSizeActionGroup->actions()) {
        gridSizeMenu->addAction(action);
    }

    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(m_aboutAction);
    helpMenu->addAction(m_aboutQtAction);
}

void GalleryMainWindow::setupToolBars()
{
    m_mainToolBar = addToolBar("Main");
    m_mainToolBar->setObjectName("MainToolBar");
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Add main actions to toolbar
    m_mainToolBar->addAction(m_copyNameAction);
    m_mainToolBar->addAction(m_copyCodeAction);
    m_mainToolBar->addAction(m_copySvgAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_toggleFavoriteAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_clearFiltersAction);

    // View toolbar
    m_viewToolBar = addToolBar("View");
    m_viewToolBar->setObjectName("ViewToolBar");
    m_viewToolBar->addAction(m_toggleSidebarAction);
    m_viewToolBar->addAction(m_toggleDetailsPanelAction);
    m_viewToolBar->addSeparator();

    // Add grid size controls
    for (QAction* action : m_gridSizeActionGroup->actions()) {
        m_viewToolBar->addAction(action);
    }
}

void GalleryMainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Ready", this);
    m_iconCountLabel = new QLabel("0 icons", this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);

    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_progressBar);
    statusBar()->addPermanentWidget(m_iconCountLabel);

    updateStatusBar();
}

void GalleryMainWindow::setupCentralWidget()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Create main horizontal splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Create search widget
    m_searchWidget = new SearchWidget(m_metadataManager, this);

    // Create category filter
    m_categoryFilter = new CategoryFilterWidget(m_metadataManager, this);

    // Create icon grid
    m_iconGrid = new IconGridWidget(this);
    m_iconGrid->setLucide(m_lucide);
    m_iconGrid->setMetadataManager(m_metadataManager);

    // Create details panel
    m_detailsPanel = new IconDetailsPanel(m_lucide, m_metadataManager, this);

    // Create right splitter for grid and details
    m_rightSplitter = new QSplitter(Qt::Vertical, this);

    // Setup layout
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(m_searchWidget);
    leftLayout->addWidget(m_categoryFilter, 1);

    QWidget* leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);
    leftWidget->setMaximumWidth(300);
    leftWidget->setMinimumWidth(250);

    QVBoxLayout* centerLayout = new QVBoxLayout();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->addWidget(m_iconGrid, 1);

    QWidget* centerWidget = new QWidget();
    centerWidget->setLayout(centerLayout);

    m_rightSplitter->addWidget(centerWidget);
    m_rightSplitter->addWidget(m_detailsPanel);
    m_rightSplitter->setStretchFactor(0, 1);
    m_rightSplitter->setStretchFactor(1, 0);

    m_mainSplitter->addWidget(leftWidget);
    m_mainSplitter->addWidget(m_rightSplitter);
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);

    QVBoxLayout* mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->addWidget(m_mainSplitter);
}

void GalleryMainWindow::setupConnections()
{
    // Menu and toolbar actions
    connect(m_aboutAction, &QAction::triggered, this, &GalleryMainWindow::onAboutApplication);
    connect(m_aboutQtAction, &QAction::triggered, this, &GalleryMainWindow::onAboutQt);
    connect(m_preferencesAction, &QAction::triggered, this, &GalleryMainWindow::onShowPreferences);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    connect(m_fullscreenAction, &QAction::triggered, this, &GalleryMainWindow::onToggleFullscreen);
    connect(m_toggleSidebarAction, &QAction::triggered, this, &GalleryMainWindow::onToggleSidebar);
    connect(m_toggleDetailsPanelAction, &QAction::triggered, this, &GalleryMainWindow::onToggleDetailsPanel);
    connect(m_toggleStatusBarAction, &QAction::triggered, this, &GalleryMainWindow::onToggleStatusBar);

    connect(m_gridSizeActionGroup, QOverload<QAction*>::of(&QActionGroup::triggered),
            this, [this](QAction* action) {
                int size = action->data().toInt();
                onChangeGridSize(size);
            });

    connect(m_exportAction, &QAction::triggered, this, &GalleryMainWindow::onExportIcons);
    connect(m_importFavoritesAction, &QAction::triggered, this, &GalleryMainWindow::onImportFavorites);
    connect(m_exportFavoritesAction, &QAction::triggered, this, &GalleryMainWindow::onExportFavorites);

    connect(m_copyNameAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconName);
    connect(m_copyCodeAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconCode);
    connect(m_copySvgAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconSvg);
    connect(m_toggleFavoriteAction, &QAction::triggered, this,
            static_cast<void (GalleryMainWindow::*)()>(&GalleryMainWindow::onToggleFavorite));
    connect(m_clearFiltersAction, &QAction::triggered, this, &GalleryMainWindow::onClearAllFilters);

    // Widget connections
    if (m_searchWidget) {
        connect(m_searchWidget, &SearchWidget::searchChanged,
                this, &GalleryMainWindow::onSearchTextChanged);
        connect(m_searchWidget, &SearchWidget::filtersChanged,
                this, [this](const IconFilterCriteria& criteria) {
                    m_metadataManager->applyFilter(criteria);
                });
    }

    if (m_categoryFilter) {
        connect(m_categoryFilter, &CategoryFilterWidget::categorySelectionChanged,
                this, &GalleryMainWindow::onCategoryFilterChanged);
        connect(m_categoryFilter, &CategoryFilterWidget::tagSelectionChanged,
                this, &GalleryMainWindow::onTagFilterChanged);
    }

    if (m_iconGrid) {
        connect(m_iconGrid, &IconGridWidget::iconSelected,
                this, &GalleryMainWindow::onIconSelected);
        connect(m_iconGrid, &IconGridWidget::iconDoubleClicked,
                this, &GalleryMainWindow::onIconDoubleClicked);
        connect(m_iconGrid, &IconGridWidget::favoriteToggled,
                this, [this](const QString& iconName, bool favorite) {
                    Q_UNUSED(favorite)
                    onToggleFavorite(iconName);
                });
    }

    if (m_detailsPanel) {
        connect(m_detailsPanel, &IconDetailsPanel::favoriteToggled,
                this, [this](const QString& iconName, bool favorite) {
                    Q_UNUSED(iconName)
                    Q_UNUSED(favorite)
                    updateActions();
                });
    }
}

void GalleryMainWindow::setupApplicationIcon()
{
    if (m_lucide) {
        QIcon appIcon = m_lucide->icon("image");
        if (!appIcon.isNull()) {
            setWindowIcon(appIcon);
            QApplication::setWindowIcon(appIcon);
        }
    }
}

void GalleryMainWindow::loadSettings()
{
    // Restore window geometry
    QMainWindow::restoreGeometry(m_settings->value("geometry").toByteArray());
    QMainWindow::restoreState(m_settings->value("windowState").toByteArray());

    // Restore splitter states
    if (m_mainSplitter) {
        m_mainSplitter->restoreState(m_settings->value("mainSplitter").toByteArray());
    }
    if (m_rightSplitter) {
        m_rightSplitter->restoreState(m_settings->value("rightSplitter").toByteArray());
    }

    // Restore UI state
    bool sidebarVisible = m_settings->value("sidebarVisible", true).toBool();
    bool detailsPanelVisible = m_settings->value("detailsPanelVisible", true).toBool();
    bool statusBarVisible = m_settings->value("statusBarVisible", true).toBool();
    int gridSize = m_settings->value("gridSize", DEFAULT_GRID_SIZE).toInt();

    m_toggleSidebarAction->setChecked(sidebarVisible);
    m_toggleDetailsPanelAction->setChecked(detailsPanelVisible);
    m_toggleStatusBarAction->setChecked(statusBarVisible);

    onToggleSidebar();
    onToggleDetailsPanel();
    onToggleStatusBar();
    onChangeGridSize(gridSize);

    // Set grid size action
    for (QAction* action : m_gridSizeActionGroup->actions()) {
        if (action->data().toInt() == gridSize) {
            action->setChecked(true);
            break;
        }
    }
}

void GalleryMainWindow::saveSettings()
{
    // Save window geometry
    m_settings->setValue("geometry", QMainWindow::saveGeometry());
    m_settings->setValue("windowState", QMainWindow::saveState());

    // Save splitter states
    if (m_mainSplitter) {
        m_settings->setValue("mainSplitter", m_mainSplitter->saveState());
    }
    if (m_rightSplitter) {
        m_settings->setValue("rightSplitter", m_rightSplitter->saveState());
    }

    // Save UI state
    m_settings->setValue("sidebarVisible", m_toggleSidebarAction->isChecked());
    m_settings->setValue("detailsPanelVisible", m_toggleDetailsPanelAction->isChecked());
    m_settings->setValue("statusBarVisible", m_toggleStatusBarAction->isChecked());

    if (m_iconGrid) {
        m_settings->setValue("gridSize", m_iconGrid->iconSize());
    }

    // Save favorites and usage data
    if (m_metadataManager) {
        m_metadataManager->saveFavorites();
        m_metadataManager->saveUsageData();
    }
}

void GalleryMainWindow::updateActions()
{
    bool hasSelection = !m_currentIconName.isEmpty();

    m_copyNameAction->setEnabled(hasSelection);
    m_copyCodeAction->setEnabled(hasSelection);
    m_copySvgAction->setEnabled(hasSelection);
    m_toggleFavoriteAction->setEnabled(hasSelection);

    if (hasSelection && m_metadataManager) {
        bool isFavorite = m_metadataManager->isFavorite(m_currentIconName);
        m_toggleFavoriteAction->setText(isFavorite ? "Remove from Favorites" : "Add to Favorites");

        // Update toolbar icon
        if (m_lucide) {
            QString iconName = isFavorite ? "heart" : "heart-off";
            QIcon icon = m_lucide->icon(iconName);
            m_toggleFavoriteAction->setIcon(icon);
        }
    }
}

void GalleryMainWindow::updateStatusBar()
{
    if (!m_metadataManager) return;

    int totalIcons = m_metadataManager->getTotalIconCount();
    int visibleIcons = 0;

    if (m_iconGrid) {
        visibleIcons = m_iconGrid->iconNames().size();
    }

    QString statusText;
    if (visibleIcons == totalIcons) {
        statusText = QString("Showing all %1 icons").arg(totalIcons);
    } else {
        statusText = QString("Showing %1 of %2 icons").arg(visibleIcons).arg(totalIcons);
    }

    m_statusLabel->setText(statusText);
    m_iconCountLabel->setText(QString("%1 icons").arg(visibleIcons));
}

void GalleryMainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}

void GalleryMainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    m_statusUpdateTimer->start();
}

// Slot implementations
void GalleryMainWindow::onAboutApplication()
{
    QString aboutText = QString(
        "<h3>QtLucide Gallery</h3>"
        "<p>Version 1.0.0</p>"
        "<p>A comprehensive icon gallery application for browsing and using "
        "Lucide icons in Qt applications.</p>"
        "<p>Built with Qt %1 and QtLucide library.</p>"
        "<p>Total Icons: %2</p>"
        "<p>Categories: %3</p>"
        "<p>Tags: %4</p>"
    ).arg(QT_VERSION_STR)
     .arg(m_metadataManager ? m_metadataManager->getTotalIconCount() : 0)
     .arg(m_metadataManager ? m_metadataManager->getCategoryCount() : 0)
     .arg(m_metadataManager ? m_metadataManager->getTagCount() : 0);

    QMessageBox::about(this, "About QtLucide Gallery", aboutText);
}

void GalleryMainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this);
}

void GalleryMainWindow::onShowPreferences()
{
    PreferencesDialog dialog(this);

    // Connect signals to update the application when settings change
    connect(&dialog, &PreferencesDialog::themeChanged,
            this, [this](const QString& theme) {
                // Apply theme changes
                Q_UNUSED(theme)
                // TODO: Implement theme switching
            });

    connect(&dialog, &PreferencesDialog::iconSizeChanged,
            this, [this](int size) {
                if (m_iconGrid) {
                    m_iconGrid->setIconSize(size);
                }
            });

    connect(&dialog, &PreferencesDialog::viewModeChanged,
            this, [this](const QString& mode) {
                if (m_iconGrid) {
                    // TODO: Implement view mode switching
                    Q_UNUSED(mode)
                }
            });

    connect(&dialog, &PreferencesDialog::settingsChanged,
            this, [this]() {
                // Refresh the application with new settings
                if (m_iconGrid) {
                    m_iconGrid->refreshIcons();
                }
                updateActions();
            });

    if (dialog.exec() == QDialog::Accepted) {
        // Settings have been applied
        m_statusLabel->setText("Preferences updated successfully");
    }
}

void GalleryMainWindow::onToggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
        m_fullscreenAction->setChecked(false);
    } else {
        showFullScreen();
        m_fullscreenAction->setChecked(true);
    }
}

void GalleryMainWindow::onExportIcons()
{
    IconExportDialog dialog(m_lucide, m_metadataManager, this);

    // Set current icon if one is selected
    if (!m_currentIconName.isEmpty()) {
        dialog.setIconName(m_currentIconName);
    }

    // Connect signals for progress feedback
    connect(&dialog, &IconExportDialog::exportStarted,
            this, [this]() {
                m_statusLabel->setText("Starting icon export...");
            });

    connect(&dialog, &IconExportDialog::exportProgress,
            this, [this](int current, int total, const QString& currentIcon) {
                m_statusLabel->setText(QString("Exporting %1... (%2/%3)")
                                     .arg(currentIcon).arg(current + 1).arg(total));
            });

    connect(&dialog, &IconExportDialog::exportFinished,
            this, [this](bool success, const QString& message) {
                m_statusLabel->setText(message);
                if (success) {
                    // Show success message briefly
                    QTimer::singleShot(3000, this, [this]() {
                        updateStatusBar();
                    });
                }
            });

    dialog.exec();
}

void GalleryMainWindow::onImportFavorites()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Import Favorites", QString(), "JSON Files (*.json)");

    if (!fileName.isEmpty() && m_metadataManager) {
        if (m_metadataManager->loadFavorites(fileName)) {
            QMessageBox::information(this, "Import Successful", "Favorites imported successfully.");
            updateActions();
        } else {
            QMessageBox::warning(this, "Import Failed", "Failed to import favorites.");
        }
    }
}

void GalleryMainWindow::onExportFavorites()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Export Favorites", "favorites.json", "JSON Files (*.json)");

    if (!fileName.isEmpty() && m_metadataManager) {
        if (m_metadataManager->saveFavorites(fileName)) {
            QMessageBox::information(this, "Export Successful", "Favorites exported successfully.");
        } else {
            QMessageBox::warning(this, "Export Failed", "Failed to export favorites.");
        }
    }
}

void GalleryMainWindow::onToggleSidebar()
{
    bool visible = m_toggleSidebarAction->isChecked();
    if (m_categoryFilter) {
        m_categoryFilter->setVisible(visible);
    }
    if (m_searchWidget) {
        m_searchWidget->setVisible(visible);
    }
}

void GalleryMainWindow::onToggleDetailsPanel()
{
    bool visible = m_toggleDetailsPanelAction->isChecked();
    if (m_detailsPanel) {
        m_detailsPanel->setVisible(visible);
    }
}

void GalleryMainWindow::onToggleStatusBar()
{
    bool visible = m_toggleStatusBarAction->isChecked();
    statusBar()->setVisible(visible);
}

void GalleryMainWindow::onChangeGridSize(int size)
{
    if (m_iconGrid) {
        m_iconGrid->setIconSize(size);
    }
}

void GalleryMainWindow::onChangeViewMode(int mode)
{
    if (!m_iconGrid) return;

    // Set the specified view mode
    IconGridWidget::ViewMode newMode = static_cast<IconGridWidget::ViewMode>(mode);
    m_iconGrid->setViewMode(newMode);

    // Update status bar
    QString modeText;
    switch (newMode) {
        case IconGridWidget::GridView:
            modeText = "Grid View";
            break;
        case IconGridWidget::ListView:
            modeText = "List View";
            break;
        case IconGridWidget::CompactView:
            modeText = "Compact View";
            break;
    }

    m_statusLabel->setText(QString("Switched to %1").arg(modeText));

    // Update actions if needed
    updateActions();
}

void GalleryMainWindow::onIconSelected(const QString& iconName)
{
    m_currentIconName = iconName;
    updateActions();

    if (m_detailsPanel) {
        m_detailsPanel->setIconName(iconName);
    }

    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onIconDoubleClicked(const QString& iconName)
{
    onIconSelected(iconName);

    // Show details panel if hidden
    if (!m_toggleDetailsPanelAction->isChecked()) {
        m_toggleDetailsPanelAction->setChecked(true);
        onToggleDetailsPanel();
    }
}

void GalleryMainWindow::onCopyIconName()
{
    if (!m_currentIconName.isEmpty()) {
        QApplication::clipboard()->setText(m_currentIconName);
        m_statusLabel->setText(QString("Copied icon name: %1").arg(m_currentIconName));
    }
}

void GalleryMainWindow::onCopyIconCode()
{
    if (!m_currentIconName.isEmpty()) {
        QString code = QString("lucide->icon(\"%1\")").arg(m_currentIconName);
        QApplication::clipboard()->setText(code);
        m_statusLabel->setText("Copied icon code to clipboard");
    }
}

void GalleryMainWindow::onCopyIconSvg()
{
    if (!m_currentIconName.isEmpty() && m_lucide) {
        QByteArray svgData = m_lucide->svgData(m_currentIconName);
        QApplication::clipboard()->setText(QString::fromUtf8(svgData));
        m_statusLabel->setText("Copied SVG data to clipboard");
    }
}

void GalleryMainWindow::onToggleFavorite()
{
    if (!m_currentIconName.isEmpty() && m_metadataManager) {
        bool isFavorite = m_metadataManager->isFavorite(m_currentIconName);

        if (isFavorite) {
            m_metadataManager->removeFromFavorites(m_currentIconName);
            m_statusLabel->setText(QString("Removed %1 from favorites").arg(m_currentIconName));
        } else {
            m_metadataManager->addToFavorites(m_currentIconName);
            m_statusLabel->setText(QString("Added %1 to favorites").arg(m_currentIconName));
        }

        updateActions();

        if (m_detailsPanel) {
            m_detailsPanel->updateFavoriteStatus();
        }
    }
}

void GalleryMainWindow::onToggleFavorite(const QString& iconName)
{
    if (!iconName.isEmpty() && m_metadataManager) {
        bool isFavorite = m_metadataManager->isFavorite(iconName);

        if (isFavorite) {
            m_metadataManager->removeFromFavorites(iconName);
            m_statusLabel->setText(QString("Removed %1 from favorites").arg(iconName));
        } else {
            m_metadataManager->addToFavorites(iconName);
            m_statusLabel->setText(QString("Added %1 to favorites").arg(iconName));
        }

        updateActions();

        if (m_detailsPanel) {
            m_detailsPanel->updateFavoriteStatus();
        }
    }
}

void GalleryMainWindow::onSearchTextChanged(const QString& text)
{
    Q_UNUSED(text)
    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onCategoryFilterChanged(const QStringList& categories)
{
    Q_UNUSED(categories)
    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onTagFilterChanged(const QStringList& tags)
{
    Q_UNUSED(tags)
    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onClearAllFilters()
{
    if (m_searchWidget) {
        m_searchWidget->clearAllFilters();
    }
    if (m_categoryFilter) {
        m_categoryFilter->clearSelection();
    }
    m_statusLabel->setText("All filters cleared");
}

void GalleryMainWindow::onIconsLoaded(int totalCount)
{
    GALLERY_END_TIMER("Metadata loading");
    GALLERY_LOG_INFO(galleryMetadata, QString("Icons loaded successfully: %1 total").arg(totalCount));
    GALLERY_LOG_MEMORY("After metadata loading");

    m_progressBar->setVisible(false);
    m_statusLabel->setText(QString("Loaded %1 icons successfully").arg(totalCount));

    // Initialize icon grid with all icons
    if (m_iconGrid && m_metadataManager) {
        GALLERY_LOG_DEBUG(galleryUI, "Initializing icon grid with all icons");
        GALLERY_START_TIMER("Icon grid initialization");
        QStringList allIcons = m_metadataManager->getAllIconNames();
        m_iconGrid->setIconNames(allIcons);
        GALLERY_END_TIMER("Icon grid initialization");
        GALLERY_LOG_INFO(galleryUI, QString("Icon grid initialized with %1 icons").arg(allIcons.size()));
    } else {
        GALLERY_LOG_WARNING(galleryUI, "Icon grid or metadata manager is null");
    }

    // Update category filter
    if (m_categoryFilter) {
        GALLERY_LOG_DEBUG(galleryUI, "Refreshing category filter");
        m_categoryFilter->refreshCategories();
        GALLERY_LOG_INFO(galleryUI, QString("Category filter updated with %1 categories")
                         .arg(m_metadataManager->getCategoryCount()));
    } else {
        GALLERY_LOG_WARNING(galleryUI, "Category filter is null");
    }

    updateStatusBar();
    GALLERY_LOG_INFO(galleryInit, "Gallery initialization completed successfully");
}

void GalleryMainWindow::onFilteredIconsChanged(int visibleCount, int totalCount)
{
    Q_UNUSED(totalCount)

    if (m_iconGrid && m_metadataManager) {
        IconFilterCriteria criteria; // Get current filter criteria
        QStringList filteredIcons = m_metadataManager->searchIcons(criteria);
        m_iconGrid->setIconNames(filteredIcons);
    }

    updateStatusBar();
}

void GalleryMainWindow::onApplicationStateChanged()
{
    updateActions();
    updateStatusBar();
}

// Missing methods declared in headers but not implemented (Part 1)
void GalleryMainWindow::onImportSettings() { /* TODO */ }
void GalleryMainWindow::onExportSettings() { /* TODO */ }
void GalleryMainWindow::onResetSettings() { /* TODO */ }
void GalleryMainWindow::onCheckForUpdates() { /* TODO */ }
void GalleryMainWindow::onShowShortcuts() { /* TODO */ }
void GalleryMainWindow::onShowPerformanceMetrics() { /* TODO */ }
void GalleryMainWindow::onToggleToolbar() { /* TODO */ }
void GalleryMainWindow::onToggleMenuBar() { /* TODO */ }
void GalleryMainWindow::onZoomIn()
{
    if (m_thumbnailSize < MAX_GRID_SIZE) {
        m_thumbnailSize = qMin(m_thumbnailSize + GRID_SIZE_STEP, MAX_GRID_SIZE);
        if (m_thumbnailGrid) {
            // Update thumbnail grid size
        }
        updateStatusBar();
    }
}

void GalleryMainWindow::onZoomOut()
{
    if (m_thumbnailSize > MIN_GRID_SIZE) {
        m_thumbnailSize = qMax(m_thumbnailSize - GRID_SIZE_STEP, MIN_GRID_SIZE);
        if (m_thumbnailGrid) {
            // Update thumbnail grid size
        }
        updateStatusBar();
    }
}

void GalleryMainWindow::onResetZoom()
{
    m_thumbnailSize = DEFAULT_GRID_SIZE;
    if (m_thumbnailGrid) {
        // Reset thumbnail grid size
    }
    updateStatusBar();
}
void GalleryMainWindow::onToggleCompactMode() { /* TODO */ }
void GalleryMainWindow::onToggleAnimations() { /* TODO */ }
void GalleryMainWindow::onIconHovered(const QString& iconName) { Q_UNUSED(iconName) /* TODO */ }
void GalleryMainWindow::onIconContextMenu(const QString& iconName, const QPoint& position) { Q_UNUSED(iconName) Q_UNUSED(position) /* TODO */ }
void GalleryMainWindow::onCopyIconPath() { /* TODO */ }
void GalleryMainWindow::onPreviewIcon() { /* TODO */ }
void GalleryMainWindow::onEditIconMetadata() { /* TODO */ }
void GalleryMainWindow::onImageSelected(const QString& imagePath) { Q_UNUSED(imagePath) /* TODO */ }
void GalleryMainWindow::onImageDoubleClicked(const QString& imagePath) { Q_UNUSED(imagePath) /* TODO */ }
void GalleryMainWindow::onDirectorySelected(const QString& dirPath) { Q_UNUSED(dirPath) /* TODO */ }
void GalleryMainWindow::onToggleGalleryMode() { /* TODO */ }
void GalleryMainWindow::onOpenImageDirectory() { /* TODO */ }
void GalleryMainWindow::onImageViewerModeChanged() { /* TODO */ }
void GalleryMainWindow::onStartSlideshow()
{
    if (!m_slideshowActive && m_slideshowTimer) {
        m_slideshowActive = true;
        m_slideshowTimer->start(m_slideshowInterval);
        GALLERY_LOG_INFO(galleryUI, "Slideshow started");
        updateStatusBar();
    }
}

// Missing methods declared in headers but not implemented (Part 2)
void GalleryMainWindow::onStopSlideshow()
{
    if (m_slideshowActive && m_slideshowTimer) {
        m_slideshowActive = false;
        m_slideshowTimer->stop();
        GALLERY_LOG_INFO(galleryUI, "Slideshow stopped");
        updateStatusBar();
    }
}
void GalleryMainWindow::onImageRotateLeft() { /* TODO */ }
void GalleryMainWindow::onImageRotateRight() { /* TODO */ }
void GalleryMainWindow::onImageFlipHorizontal() { /* TODO */ }
void GalleryMainWindow::onImageFlipVertical() { /* TODO */ }
void GalleryMainWindow::onImageZoomIn() { /* TODO */ }
void GalleryMainWindow::onImageZoomOut() { /* TODO */ }
void GalleryMainWindow::onImageFitToWindow() { /* TODO */ }
void GalleryMainWindow::onImageActualSize() { /* TODO */ }
void GalleryMainWindow::onAdvancedSearchRequested() { /* TODO */ }
void GalleryMainWindow::onContributorFilterChanged(const QStringList& contributors) { Q_UNUSED(contributors) /* TODO */ }
void GalleryMainWindow::onFavoritesFilterToggled(bool enabled) { Q_UNUSED(enabled) /* TODO */ }
void GalleryMainWindow::onRecentFilterToggled(bool enabled) { Q_UNUSED(enabled) /* TODO */ }
void GalleryMainWindow::onSaveCurrentFilter() { /* TODO */ }
void GalleryMainWindow::onLoadSavedFilter() { /* TODO */ }
void GalleryMainWindow::onIconsLoadProgress(int current, int total) { Q_UNUSED(current) Q_UNUSED(total) /* TODO */ }
void GalleryMainWindow::onThemeChanged() { /* TODO */ }
void GalleryMainWindow::onLanguageChanged() { /* TODO */ }
void GalleryMainWindow::onPerformanceMetricsUpdated(const QMap<QString, QVariant>& metrics) { Q_UNUSED(metrics) /* TODO */ }
void GalleryMainWindow::onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason) { Q_UNUSED(reason) /* TODO */ }
void GalleryMainWindow::onSystemTrayMessageClicked() { /* TODO */ }
void GalleryMainWindow::onFadeInFinished() { /* TODO */ }
void GalleryMainWindow::onFadeOutFinished() { /* TODO */ }
void GalleryMainWindow::onSlideAnimationFinished() { /* TODO */ }
void GalleryMainWindow::onAutoSaveTimer() { /* TODO */ }
void GalleryMainWindow::onBackupTimer() { /* TODO */ }

// Event handlers
void GalleryMainWindow::keyPressEvent(QKeyEvent *event) { QMainWindow::keyPressEvent(event); }
void GalleryMainWindow::showEvent(QShowEvent *event) { QMainWindow::showEvent(event); }
void GalleryMainWindow::changeEvent(QEvent *event) { QMainWindow::changeEvent(event); }

void GalleryMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void GalleryMainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QString filePath = url.toLocalFile();
                // Handle dropped files (images or directories)
                QFileInfo fileInfo(filePath);
                if (fileInfo.isDir()) {
                    loadImageDirectory(filePath);
                } else {
                    QString suffix = fileInfo.suffix().toLower();
                    if (suffix == "jpg" || suffix == "jpeg" || suffix == "png" ||
                        suffix == "gif" || suffix == "bmp" || suffix == "svg") {
                        showImage(filePath);
                    }
                }
            }
        }
        event->acceptProposedAction();
    }
}

// Enhanced methods from consolidated design
void GalleryMainWindow::setViewMode(ViewMode mode)
{
    if (m_currentViewModeEnum == mode) return;

    m_currentViewModeEnum = mode;
    updateViewMode();
    updateUI();
}

void GalleryMainWindow::setTheme(ThemeMode theme)
{
    if (m_currentTheme == theme) return;

    m_currentTheme = theme;
    applyTheme();
}

void GalleryMainWindow::updateUI()
{
    updateStatusBar();
    updateWindowTitle();
    updateActions();
}

void GalleryMainWindow::setupCategoryShortcuts()
{
    // Setup Ctrl+1-9 shortcuts for categories
    for (int i = 1; i <= 9; ++i) {
        QShortcut* shortcut = new QShortcut(QKeySequence(QString("Ctrl+%1").arg(i)), this);
        connect(shortcut, &QShortcut::activated, this, [this, i]() {
            onCategoryShortcut(i - 1);  // Convert to 0-based index
        });
        m_categoryShortcuts.append(shortcut);
    }
}

void GalleryMainWindow::onCategoryShortcut(int categoryIndex)
{
    // Handle category shortcut activation
    if (m_categorySidebar) {
        // Implementation would depend on CategorySidebarWidget interface
        GALLERY_LOG_DEBUG(galleryUI, QString("Category shortcut %1 activated").arg(categoryIndex + 1));
    }
}

// Enhanced methods from EnhancedGalleryMainWindow
void GalleryMainWindow::onZoomFit()
{
    if (m_imageViewer) {
        // Implement zoom to fit functionality
        GALLERY_LOG_DEBUG(galleryUI, "Zoom to fit requested");
    }
}

void GalleryMainWindow::onZoomActual()
{
    if (m_imageViewer) {
        // Implement zoom to actual size functionality
        GALLERY_LOG_DEBUG(galleryUI, "Zoom to actual size requested");
    }
}

void GalleryMainWindow::onGoToIcon()
{
    // Implement go to specific icon functionality
    GALLERY_LOG_DEBUG(galleryUI, "Go to icon requested");
}

void GalleryMainWindow::onNextIcon()
{
    // Implement next icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "Next icon requested");
    }
}

void GalleryMainWindow::onPreviousIcon()
{
    // Implement previous icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "Previous icon requested");
    }
}

void GalleryMainWindow::onFirstIcon()
{
    // Implement first icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "First icon requested");
    }
}

void GalleryMainWindow::onLastIcon()
{
    // Implement last icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "Last icon requested");
    }
}

void GalleryMainWindow::onToggleSlideshow()
{
    if (m_slideshowActive) {
        onStopSlideshow();
    } else {
        onStartSlideshow();
    }
}

void GalleryMainWindow::onOpenImages()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Open Images"), QString(),
        tr("Image Files (*.png *.jpg *.jpeg *.gif *.bmp *.svg)"));

    if (!fileNames.isEmpty()) {
        // Handle opening multiple images
        for (const QString& fileName : fileNames) {
            showImage(fileName);
        }
    }
}

void GalleryMainWindow::onOpenDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this,
        tr("Open Directory"), QString());

    if (!dirPath.isEmpty()) {
        loadImageDirectory(dirPath);
    }
}

void GalleryMainWindow::onExportIcon()
{
    if (!m_currentIconName.isEmpty()) {
        // Use existing export functionality
        onExportIcons();
    }
}

void GalleryMainWindow::onExportBatch()
{
    // Implement batch export functionality
    onExportIcons();
}

void GalleryMainWindow::onCopyIcon()
{
    // Copy current icon to clipboard
    onCopyIconSvg();
}

void GalleryMainWindow::onGenerateCode()
{
    if (!m_currentIconName.isEmpty()) {
        // Generate code for current icon
        onCopyIconCode();
    }
}



void GalleryMainWindow::onShowFavorites()
{
    if (m_favoritesManager) {
        // Show favorites view
        GALLERY_LOG_DEBUG(galleryUI, "Show favorites requested");
    }
}

void GalleryMainWindow::onClearFavorites()
{
    if (m_favoritesManager) {
        // Clear all favorites
        GALLERY_LOG_DEBUG(galleryUI, "Clear favorites requested");
    }
}

void GalleryMainWindow::onSettings()
{
    onShowPreferences();
}

void GalleryMainWindow::onAbout()
{
    onAboutApplication();
}

void GalleryMainWindow::onShowKeyboardShortcuts()
{
    onShowShortcuts();
}

void GalleryMainWindow::onContentLoaded()
{
    updateStatusBar();
    updateUI();
}

void GalleryMainWindow::onMetadataLoaded(int totalIcons)
{
    onIconsLoaded(totalIcons);
}

void GalleryMainWindow::onLoadingProgress(int current, int total)
{
    onIconsLoadProgress(current, total);
}

void GalleryMainWindow::onThumbnailSizeChanged(int size)
{
    m_thumbnailSize = size;
    if (m_thumbnailGrid) {
        // Update thumbnail grid size
    }
    updateStatusBar();
}

// Missing method implementations that are causing linker errors
void GalleryMainWindow::setupShortcuts()
{
    // Setup keyboard shortcuts
    setupCategoryShortcuts();

    // Add other shortcuts as needed
    GALLERY_LOG_DEBUG(galleryUI, "Keyboard shortcuts setup completed");
}

void GalleryMainWindow::applyTheme()
{
    // Apply the current theme
    switch (m_currentTheme) {
        case LightTheme:
            // Apply light theme
            setStyleSheet("");  // Default light theme
            break;
        case DarkTheme:
            // Apply dark theme
            setStyleSheet("QMainWindow { background-color: #2b2b2b; color: #ffffff; }");
            break;
        case SystemTheme:
        default:
            // Use system theme
            setStyleSheet("");
            break;
    }
    GALLERY_LOG_DEBUG(galleryUI, "Theme applied");
}

void GalleryMainWindow::loadImageDirectory(const QString& directoryPath)
{
    if (directoryPath.isEmpty()) return;

    if (m_imageMetadataManager) {
        // Load images from directory
        GALLERY_LOG_INFO(galleryUI, QString("Loading images from directory: %1").arg(directoryPath));
        // Implementation would depend on ImageMetadataManager interface
    }

    updateStatusBar();
}

void GalleryMainWindow::showImage(const QString& imagePath)
{
    if (imagePath.isEmpty()) return;

    if (m_imageViewer) {
        // Show image in viewer
        GALLERY_LOG_INFO(galleryUI, QString("Showing image: %1").arg(imagePath));
        // Implementation would depend on ImageViewerWidget interface
    }
}

void GalleryMainWindow::updateViewMode()
{
    // Update the view mode based on current settings
    if (m_iconGrid) {
        // Update icon grid view mode
    }
    if (m_thumbnailGrid) {
        // Update thumbnail grid view mode
    }
    updateStatusBar();
}

void GalleryMainWindow::updateWindowTitle()
{
    QString title = "QtLucide Enhanced Icon & Image Gallery";
    if (!m_currentIconName.isEmpty()) {
        title += QString(" - %1").arg(m_currentIconName);
    }
    setWindowTitle(title);
}

// Additional missing methods that are declared in header but not implemented
// (Only adding methods that actually exist in the header file)
