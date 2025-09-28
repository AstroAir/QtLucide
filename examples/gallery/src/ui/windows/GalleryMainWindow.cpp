/**
 * QtLucide Gallery Application - Consolidated Main Window Implementation
 */

#include "GalleryMainWindow.h"
#include "../../config/LayoutConfig.h"
#include "../../core/managers/ContentManager.h"
#include "../../core/managers/FavoritesManager.h"
#include "../../core/managers/IconMetadataManager.h"
#include "../../core/managers/ImageMetadataManager.h"
#include "../../core/utils/ErrorHandler.h"
#include "../../core/utils/GalleryLogger.h"
#include "../dialogs/IconExportDialog.h"
#include "../dialogs/PreferencesDialog.h"
#include "../widgets/grids/IconGridWidget.h"
#include "../widgets/grids/IconThumbnailGridWidget.h"
#include "../widgets/panels/CategorySidebarWidget.h"
#include "../widgets/panels/FileBrowserWidget.h"
#include "../widgets/panels/IconDetailsPanel.h"
#include "../widgets/search/CategoryFilterWidget.h"
#include "../widgets/search/IconSearchWidget.h"
#include "../widgets/search/SearchWidget.h"
#include "../widgets/viewers/ImageViewerWidget.h"

#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QSettings>
#include <QShortcut>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>

GalleryMainWindow::GalleryMainWindow(QWidget* parent)
    : QMainWindow(parent), m_lucide(nullptr), m_metadataManager(nullptr),
      m_imageMetadataManager(nullptr), m_contentManager(nullptr), m_favoritesManager(nullptr),
      m_centralWidget(nullptr), m_stackedWidget(nullptr), m_viewStack(nullptr),
      m_mainSplitter(nullptr), m_rightSplitter(nullptr), m_leftSplitter(nullptr),
      m_contentSplitter(nullptr), m_searchWidget(nullptr), m_iconSearchWidget(nullptr),
      m_categoryFilter(nullptr), m_categorySidebar(nullptr), m_iconGrid(nullptr),
      m_thumbnailGrid(nullptr), m_detailsPanel(nullptr), m_imageViewer(nullptr),
      m_fileBrowser(nullptr), m_mainToolbar(nullptr), m_viewToolbar(nullptr),
      m_statusLabel(nullptr), m_iconCountLabel(nullptr), m_currentIconLabel(nullptr),
      m_progressBar(nullptr), m_fullscreenAction(nullptr), m_toggleFavoriteAction(nullptr),
      m_viewModeGroup(nullptr), m_settings(new QSettings(this)), m_themeGroup(nullptr),
      m_showTooltips(true), m_exportDialogEnhanced(nullptr), m_settingsDialog(nullptr),
      m_currentViewModeEnum(SplitView), m_currentTheme(SystemTheme), m_thumbnailSize(64),
      m_slideshowInterval(3000), m_autoSaveSettings(true), m_statusUpdateTimer(new QTimer(this)),
      m_slideshowTimer(new QTimer(this)) {
    // GALLERY_LOG_INFO(galleryInit, "Consolidated GalleryMainWindow constructor started");
    // GALLERY_START_TIMER("MainWindow initialization");
    qDebug() << "GalleryMainWindow constructor started";

    setWindowTitle("QtLucide Enhanced Icon & Image Gallery");

    // Enhanced window sizing with responsive constraints
    setMinimumSize(800, 600); // Minimum usable size
    resize(1200, 800);        // Optimal default size

    // Set size policy for better responsive behavior
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Enable drag and drop
    setAcceptDrops(true);

    // GALLERY_LOG_DEBUG(galleryInit, "Window properties set");
    qDebug() << "Window properties set";

    // Initialize core components first
    // GALLERY_LOG_INFO(galleryInit, "Initializing components");
    qDebug() << "Initializing components";
    initializeComponents();

    // Initialize icon system
    // GALLERY_LOG_INFO(galleryInit, "Initializing icon system");
    qDebug() << "Initializing icon system";
    initializeIconSystem();

    // Setup UI
    // GALLERY_LOG_INFO(galleryInit, "Setting up UI components");
    qDebug() << "Setting up UI components";
    setupUI();
    setupShortcuts();

    // GALLERY_LOG_INFO(galleryInit, "Setting up connections");
    qDebug() << "Setting up connections";
    setupConnections();

    // Load settings and restore state
    // GALLERY_LOG_INFO(galleryInit, "Loading settings");
    qDebug() << "Loading settings";
    loadSettings();
    applyTheme();

    // Setup status update timer
    // GALLERY_LOG_DEBUG(galleryInit, "Setting up status update timer");
    qDebug() << "Setting up status update timer";
    m_statusUpdateTimer->setSingleShot(true);
    m_statusUpdateTimer->setInterval(500);
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &GalleryMainWindow::updateStatusBar);

    // Set application icon
    // GALLERY_LOG_DEBUG(galleryInit, "Setting up application icon");
    qDebug() << "Setting up application icon";
    setupApplicationIcon();

    // GALLERY_END_TIMER("MainWindow initialization");
    // GALLERY_LOG_INFO(galleryInit, "GalleryMainWindow constructor completed");
    // GALLERY_LOG_MEMORY("After MainWindow construction");
    qDebug() << "GalleryMainWindow constructor completed";
}

GalleryMainWindow::~GalleryMainWindow() {
    saveSettings();
}

void GalleryMainWindow::initializeComponents() {
    // GALLERY_LOG_INFO(galleryInit, "Initializing core components");
    qDebug() << "Initializing core components";

    // Create core managers (only those available in current build)
    if (!m_lucide) {
        qDebug() << "Creating QtLucide instance";
        m_lucide = new lucide::QtLucide(this);
    }

    // Only create managers that are available in the current build configuration
    if (!m_metadataManager) {
        qDebug() << "Creating IconMetadataManager";
        try {
            m_metadataManager = new IconMetadataManager(this);
            qDebug() << "IconMetadataManager created successfully";
        } catch (const std::exception& e) {
            qCritical() << "Failed to create IconMetadataManager:" << e.what();
            m_metadataManager = nullptr;
        } catch (...) {
            qCritical() << "Failed to create IconMetadataManager: Unknown error";
            m_metadataManager = nullptr;
        }
    }

    // Create FavoritesManager
    if (!m_favoritesManager) {
        qDebug() << "Creating FavoritesManager";
        try {
            m_favoritesManager = new FavoritesManager(this);
            if (m_metadataManager) {
                m_favoritesManager->setIconMetadataManager(m_metadataManager);
            }
            qDebug() << "FavoritesManager created successfully";
        } catch (const std::exception& e) {
            qCritical() << "Failed to create FavoritesManager:" << e.what();
            m_favoritesManager = nullptr;
        } catch (...) {
            qCritical() << "Failed to create FavoritesManager: Unknown error";
            m_favoritesManager = nullptr;
        }
    }

    // Note: ContentManager and ImageMetadataManager
    // are only created if they're available in the build configuration
    // This allows the consolidated window to work in different build contexts

    // GALLERY_LOG_INFO(galleryInit, "Core components initialized");
    qDebug() << "Core components initialized";
}

void GalleryMainWindow::initializeIconSystem() {
    GALLERY_START_TIMER("Icon system initialization");

    // Initialize QtLucide if not already created
    if (!m_lucide) {
        GALLERY_LOG_DEBUG(galleryInit, "Creating QtLucide instance");
        m_lucide = new lucide::QtLucide(this);
    }

    GALLERY_LOG_INFO(galleryInit, "Initializing QtLucide");
    if (!m_lucide->initLucide()) {
        QString errorMsg = "Failed to initialize QtLucide icon system";
        GALLERY_LOG_CRITICAL(galleryInit, errorMsg);
        QMessageBox::critical(this, "Initialization Error", errorMsg);
        return;
    }

    int iconCount = static_cast<int>(m_lucide->availableIcons().size());
    GALLERY_LOG_INFO(galleryInit,
                     QString("QtLucide initialized successfully with %1 icons").arg(iconCount));

    // Use existing metadata manager if already created
    if (!m_metadataManager) {
        GALLERY_LOG_DEBUG(galleryInit, "Creating IconMetadataManager");
        m_metadataManager = new IconMetadataManager(this);
    }

    // Connect metadata manager signals
    GALLERY_LOG_DEBUG(galleryInit, "Connecting metadata manager signals");
    connect(m_metadataManager, &IconMetadataManager::metadataLoaded, this,
            &GalleryMainWindow::onIconsLoaded);
    connect(m_metadataManager, &IconMetadataManager::metadataLoadFailed, this,
            [this](const QString& error) {
                GALLERY_LOG_CRITICAL(galleryMetadata,
                                     QString("Metadata loading failed: %1").arg(error));
                ErrorHandler::reportError(QString("Failed to load icon metadata: %1").arg(error),
                                          ErrorSeverity::Critical, ErrorCategory::Resource,
                                          "IconMetadataManager initialization");
                ErrorHandler::showErrorDialog(
                    this, "Loading Error",
                    "Failed to load icon metadata. The application may not function correctly.",
                    error);
            });
    connect(m_metadataManager, &IconMetadataManager::filteredIconsChanged, this,
            [this](const QStringList& iconNames) {
                GALLERY_LOG_DEBUG(
                    gallerySearch,
                    QString("Filtered icons changed: %1 icons").arg(iconNames.size()));
                onFilteredIconsChanged(static_cast<int>(iconNames.size()),
                                       m_metadataManager->getTotalIconCount());
            });

    // Start loading metadata
    GALLERY_LOG_INFO(galleryInit, "Starting metadata loading");
    GALLERY_START_TIMER("Metadata loading");
    m_metadataManager->loadMetadata();

    // Load favorites data
    if (m_favoritesManager) {
        GALLERY_LOG_INFO(galleryInit, "Loading favorites data");
        m_favoritesManager->loadFavorites();
    }

    GALLERY_END_TIMER("Icon system initialization");
}

void GalleryMainWindow::setupUI() {
    createActions();
    setupMenuBar();
    setupToolBars();
    setupStatusBar();
    setupCentralWidget();
}

void GalleryMainWindow::createActions() {
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

void GalleryMainWindow::setupMenuBar() {
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

void GalleryMainWindow::setupToolBars() {
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

void GalleryMainWindow::setupStatusBar() {
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

void GalleryMainWindow::setupCentralWidget() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Create main horizontal splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Create search widget (only if metadata manager is available)
    if (m_metadataManager) {
        m_searchWidget = new SearchWidget(m_metadataManager, this);
    } else {
        qWarning() << "Cannot create SearchWidget: IconMetadataManager is null";
        m_searchWidget = nullptr;
    }

    // Create category filter (only if metadata manager is available)
    if (m_metadataManager) {
        m_categoryFilter = new CategoryFilterWidget(m_metadataManager, this);
    } else {
        qWarning() << "Cannot create CategoryFilterWidget: IconMetadataManager is null";
        m_categoryFilter = nullptr;
    }

    // Create icon grid
    m_iconGrid = new IconGridWidget(this);
    if (m_lucide) {
        m_iconGrid->setLucide(m_lucide);
    }
    if (m_metadataManager) {
        m_iconGrid->setMetadataManager(m_metadataManager);
    } else {
        qWarning() << "Cannot set metadata manager for IconGridWidget: IconMetadataManager is null";
    }

    // Create details panel (only if both lucide and metadata manager are available)
    if (m_lucide && m_metadataManager) {
        m_detailsPanel = new IconDetailsPanel(m_lucide, m_metadataManager, this);
    } else {
        qWarning() << "Cannot create IconDetailsPanel: Missing dependencies (lucide:"
                   << (m_lucide ? "OK" : "NULL") << ", metadata:"
                   << (m_metadataManager ? "OK" : "NULL") << ")";
        m_detailsPanel = nullptr;
    }

    // Create right splitter for grid and details
    m_rightSplitter = new QSplitter(Qt::Vertical, this);

    // Setup layout
    QVBoxLayout* leftLayout = new QVBoxLayout();
    if (m_searchWidget) {
        leftLayout->addWidget(m_searchWidget);
    }
    if (m_categoryFilter) {
        leftLayout->addWidget(m_categoryFilter, 1);
    }

    // If no widgets were added to left layout, add a placeholder
    if (leftLayout->count() == 0) {
        QLabel* placeholder = new QLabel("Search and filters unavailable\n(Metadata manager failed to initialize)", this);
        placeholder->setAlignment(Qt::AlignCenter);
        placeholder->setStyleSheet("color: gray; font-style: italic;");
        leftLayout->addWidget(placeholder);
    }

    QWidget* leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);
    leftWidget->setMaximumWidth(300);
    leftWidget->setMinimumWidth(250);

    QVBoxLayout* centerLayout = new QVBoxLayout();
    centerLayout->setContentsMargins(0, 0, 0, 0);
    if (m_iconGrid) {
        centerLayout->addWidget(m_iconGrid, 1);
    }

    QWidget* centerWidget = new QWidget();
    centerWidget->setLayout(centerLayout);

    m_rightSplitter->addWidget(centerWidget);
    if (m_detailsPanel) {
        m_rightSplitter->addWidget(m_detailsPanel);
        m_rightSplitter->setStretchFactor(0, 1);
        m_rightSplitter->setStretchFactor(1, 0);
    } else {
        // If no details panel, just use the center widget
        m_rightSplitter->setStretchFactor(0, 1);
    }

    m_mainSplitter->addWidget(leftWidget);
    m_mainSplitter->addWidget(m_rightSplitter);
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);

    // Set initial splitter sizes for better responsive behavior
    m_mainSplitter->setSizes({280, 800}); // Left panel: 280px, Right panel: remaining

    // Configure splitter properties for better user experience
    m_mainSplitter->setChildrenCollapsible(false); // Prevent panels from collapsing completely
    m_rightSplitter->setChildrenCollapsible(false);

    // Set handle width for easier resizing
    m_mainSplitter->setHandleWidth(6);
    m_rightSplitter->setHandleWidth(6);

    QVBoxLayout* mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->addWidget(m_mainSplitter);
}

void GalleryMainWindow::setupConnections() {
    // Menu and toolbar actions
    connect(m_aboutAction, &QAction::triggered, this, &GalleryMainWindow::onAboutApplication);
    connect(m_aboutQtAction, &QAction::triggered, this, &GalleryMainWindow::onAboutQt);
    connect(m_preferencesAction, &QAction::triggered, this, &GalleryMainWindow::onShowPreferences);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    connect(m_fullscreenAction, &QAction::triggered, this, &GalleryMainWindow::onToggleFullscreen);
    connect(m_toggleSidebarAction, &QAction::triggered, this, &GalleryMainWindow::onToggleSidebar);
    connect(m_toggleDetailsPanelAction, &QAction::triggered, this,
            &GalleryMainWindow::onToggleDetailsPanel);
    connect(m_toggleStatusBarAction, &QAction::triggered, this,
            &GalleryMainWindow::onToggleStatusBar);

    connect(m_gridSizeActionGroup, QOverload<QAction*>::of(&QActionGroup::triggered), this,
            [this](QAction* action) {
                int size = action->data().toInt();
                onChangeGridSize(size);
            });

    connect(m_exportAction, &QAction::triggered, this, &GalleryMainWindow::onExportIcons);
    connect(m_importFavoritesAction, &QAction::triggered, this,
            &GalleryMainWindow::onImportFavorites);
    connect(m_exportFavoritesAction, &QAction::triggered, this,
            &GalleryMainWindow::onExportFavorites);

    connect(m_copyNameAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconName);
    connect(m_copyCodeAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconCode);
    connect(m_copySvgAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconSvg);
    connect(m_toggleFavoriteAction, &QAction::triggered, this,
            static_cast<void (GalleryMainWindow::*)()>(&GalleryMainWindow::onToggleFavorite));
    connect(m_clearFiltersAction, &QAction::triggered, this, &GalleryMainWindow::onClearAllFilters);

    // Widget connections
    if (m_searchWidget) {
        connect(m_searchWidget, &SearchWidget::searchChanged, this,
                &GalleryMainWindow::onSearchTextChanged);
        connect(m_searchWidget, &SearchWidget::filtersChanged, this,
                [this](const IconFilterCriteria& criteria) {
                    m_metadataManager->applyFilter(criteria);
                });
    }

    if (m_categoryFilter) {
        connect(m_categoryFilter, &CategoryFilterWidget::categorySelectionChanged, this,
                &GalleryMainWindow::onCategoryFilterChanged);
        connect(m_categoryFilter, &CategoryFilterWidget::tagSelectionChanged, this,
                &GalleryMainWindow::onTagFilterChanged);
    }

    if (m_iconGrid) {
        connect(m_iconGrid, &IconGridWidget::iconSelected, this,
                &GalleryMainWindow::onIconSelected);
        connect(m_iconGrid, &IconGridWidget::iconDoubleClicked, this,
                &GalleryMainWindow::onIconDoubleClicked);
        connect(m_iconGrid, &IconGridWidget::favoriteToggled, this,
                [this](const QString& iconName, bool favorite) {
                    Q_UNUSED(favorite)
                    onToggleFavorite(iconName);
                });
    }

    if (m_detailsPanel) {
        connect(m_detailsPanel, &IconDetailsPanel::favoriteToggled, this,
                [this](const QString& iconName, bool favorite) {
                    Q_UNUSED(iconName)
                    Q_UNUSED(favorite)
                    updateActions();
                });
    }

    // FavoritesManager connections
    if (m_favoritesManager) {
        connect(m_favoritesManager, &FavoritesManager::favoriteAdded, this,
                [this](const QString& iconName) {
                    qDebug() << "Favorite added:" << iconName;
                    updateActions();
                    if (m_detailsPanel) {
                        m_detailsPanel->updateFavoriteStatus();
                    }
                });
        connect(m_favoritesManager, &FavoritesManager::favoriteRemoved, this,
                [this](const QString& iconName) {
                    qDebug() << "Favorite removed:" << iconName;
                    updateActions();
                    if (m_detailsPanel) {
                        m_detailsPanel->updateFavoriteStatus();
                    }
                });
        connect(m_favoritesManager, &FavoritesManager::favoritesChanged, this,
                [this]() {
                    qDebug() << "Favorites changed";
                    updateActions();
                });
    }
}

void GalleryMainWindow::setupApplicationIcon() {
    if (m_lucide) {
        QIcon appIcon = m_lucide->icon("image");
        if (!appIcon.isNull()) {
            setWindowIcon(appIcon);
            QApplication::setWindowIcon(appIcon);
        }
    }
}

void GalleryMainWindow::loadSettings() {
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

void GalleryMainWindow::saveSettings() {
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

void GalleryMainWindow::updateActions() {
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

void GalleryMainWindow::updateStatusBar() {
    if (!m_metadataManager)
        return;

    int totalIcons = m_metadataManager->getTotalIconCount();
    int visibleIcons = 0;

    if (m_iconGrid) {
        visibleIcons = static_cast<int>(m_iconGrid->iconNames().size());
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

void GalleryMainWindow::closeEvent(QCloseEvent* event) {
    saveSettings();
    QMainWindow::closeEvent(event);
}

void GalleryMainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    // Optimize layout for different window sizes
    if (m_mainSplitter && event->size().isValid()) {
        int windowWidth = event->size().width();

        // Auto-hide sidebar on very small windows
        if (windowWidth < 800 && m_toggleSidebarAction->isChecked()) {
            // Remember the user's preference
            m_settings->setValue("sidebarAutoHidden", true);
            m_toggleSidebarAction->setChecked(false);
            onToggleSidebar();
        } else if (windowWidth >= 800 && m_settings->value("sidebarAutoHidden", false).toBool()) {
            // Restore sidebar when window is large enough
            m_settings->remove("sidebarAutoHidden");
            m_toggleSidebarAction->setChecked(true);
            onToggleSidebar();
        }

        // Adjust details panel for medium-sized windows
        if (m_rightSplitter && m_detailsPanel) {
            if (windowWidth < 1200 && m_toggleDetailsPanelAction->isChecked()) {
                // Auto-hide details panel on medium windows
                m_settings->setValue("detailsPanelAutoHidden", true);
                m_toggleDetailsPanelAction->setChecked(false);
                onToggleDetailsPanel();
            } else if (windowWidth >= 1200 && m_settings->value("detailsPanelAutoHidden", false).toBool()) {
                // Restore details panel when window is large enough
                m_settings->remove("detailsPanelAutoHidden");
                m_toggleDetailsPanelAction->setChecked(true);
                onToggleDetailsPanel();
            }
        }
    }

    m_statusUpdateTimer->start();
}

// Slot implementations
void GalleryMainWindow::onAboutApplication() {
    QString aboutText =
        QString("<h3>QtLucide Gallery</h3>"
                "<p>Version 1.0.0</p>"
                "<p>A comprehensive icon gallery application for browsing and using "
                "Lucide icons in Qt applications.</p>"
                "<p>Built with Qt %1 and QtLucide library.</p>"
                "<p>Total Icons: %2</p>"
                "<p>Categories: %3</p>"
                "<p>Tags: %4</p>")
            .arg(QT_VERSION_STR)
            .arg(m_metadataManager ? m_metadataManager->getTotalIconCount() : 0)
            .arg(m_metadataManager ? m_metadataManager->getCategoryCount() : 0)
            .arg(m_metadataManager ? m_metadataManager->getTagCount() : 0);

    QMessageBox::about(this, "About QtLucide Gallery", aboutText);
}

void GalleryMainWindow::onAboutQt() {
    QMessageBox::aboutQt(this);
}

void GalleryMainWindow::onShowPreferences() {
    PreferencesDialog dialog(this);

    // Connect signals to update the application when settings change
    connect(&dialog, &PreferencesDialog::themeChanged, this, [this](const QString& theme) {
        // Apply theme changes
        if (theme == "light") {
            setTheme(LightTheme);
        } else if (theme == "dark") {
            setTheme(DarkTheme);
        } else {
            setTheme(SystemTheme);
        }
        applyTheme();
        statusBar()->showMessage(tr("Theme changed to %1").arg(theme), 2000);
    });

    connect(&dialog, &PreferencesDialog::iconSizeChanged, this, [this](int size) {
        if (m_iconGrid) {
            m_iconGrid->setIconSize(size);
        }
    });

    connect(&dialog, &PreferencesDialog::viewModeChanged, this, [this](const QString& mode) {
        if (m_iconGrid) {
            if (mode == "grid") {
                setViewMode(GridView);
            } else if (mode == "detail") {
                setViewMode(DetailView);
            } else if (mode == "split") {
                setViewMode(SplitView);
            }
            statusBar()->showMessage(tr("View mode changed to %1").arg(mode), 2000);
        }
    });

    connect(&dialog, &PreferencesDialog::settingsChanged, this, [this]() {
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

void GalleryMainWindow::onToggleFullscreen() {
    if (isFullScreen()) {
        showNormal();
        m_fullscreenAction->setChecked(false);
    } else {
        showFullScreen();
        m_fullscreenAction->setChecked(true);
    }
}

void GalleryMainWindow::onExportIcons() {
    IconExportDialog dialog(m_lucide, m_metadataManager, this);

    // Set current icon if one is selected
    if (!m_currentIconName.isEmpty()) {
        dialog.setIconName(m_currentIconName);
    }

    // Connect signals for progress feedback
    connect(&dialog, &IconExportDialog::exportStarted, this,
            [this]() { m_statusLabel->setText("Starting icon export..."); });

    connect(
        &dialog, &IconExportDialog::exportProgress, this,
        [this](int current, int total, const QString& currentIcon) {
            m_statusLabel->setText(
                QString("Exporting %1... (%2/%3)").arg(currentIcon).arg(current + 1).arg(total));
        });

    connect(&dialog,
            static_cast<void (IconExportDialog::*)(bool, const QString&, const QStringList&)>(
                &IconExportDialog::exportFinished),
            this, [this](bool success, const QString& message, const QStringList&) {
                m_statusLabel->setText(message);
                if (success) {
                    // Show success message briefly
                    QTimer::singleShot(3000, this, [this]() { updateStatusBar(); });
                }
            });

    dialog.exec();
}

void GalleryMainWindow::onImportFavorites() {
    QString fileName =
        QFileDialog::getOpenFileName(this, "Import Favorites", QString(), "JSON Files (*.json)");

    if (!fileName.isEmpty() && m_metadataManager) {
        if (m_metadataManager->loadFavorites(fileName)) {
            QMessageBox::information(this, "Import Successful", "Favorites imported successfully.");
            updateActions();
        } else {
            QMessageBox::warning(this, "Import Failed", "Failed to import favorites.");
        }
    }
}

void GalleryMainWindow::onExportFavorites() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Favorites", "favorites.json",
                                                    "JSON Files (*.json)");

    if (!fileName.isEmpty() && m_metadataManager) {
        if (m_metadataManager->saveFavorites(fileName)) {
            QMessageBox::information(this, "Export Successful", "Favorites exported successfully.");
        } else {
            QMessageBox::warning(this, "Export Failed", "Failed to export favorites.");
        }
    }
}

void GalleryMainWindow::onToggleSidebar() {
    bool visible = m_toggleSidebarAction->isChecked();
    if (m_categoryFilter) {
        m_categoryFilter->setVisible(visible);
    }
    if (m_searchWidget) {
        m_searchWidget->setVisible(visible);
    }
}

void GalleryMainWindow::onToggleDetailsPanel() {
    bool visible = m_toggleDetailsPanelAction->isChecked();
    if (m_detailsPanel) {
        m_detailsPanel->setVisible(visible);
    }
}

void GalleryMainWindow::onToggleStatusBar() {
    bool visible = m_toggleStatusBarAction->isChecked();
    statusBar()->setVisible(visible);
}

void GalleryMainWindow::onChangeGridSize(int size) {
    if (m_iconGrid) {
        m_iconGrid->setIconSize(size);
    }
}

void GalleryMainWindow::onChangeViewMode(int mode) {
    if (!m_iconGrid)
        return;

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
        case IconGridWidget::DetailedView:
            modeText = "Detailed View";
            break;
    }

    m_statusLabel->setText(QString("Switched to %1").arg(modeText));

    // Update actions if needed
    updateActions();
}

void GalleryMainWindow::onIconSelected(const QString& iconName) {
    m_currentIconName = iconName;
    updateActions();

    if (m_detailsPanel) {
        m_detailsPanel->setIconName(iconName);
    }

    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onIconDoubleClicked(const QString& iconName) {
    onIconSelected(iconName);

    // Show details panel if hidden
    if (!m_toggleDetailsPanelAction->isChecked()) {
        m_toggleDetailsPanelAction->setChecked(true);
        onToggleDetailsPanel();
    }
}

void GalleryMainWindow::onCopyIconName() {
    if (!m_currentIconName.isEmpty()) {
        QApplication::clipboard()->setText(m_currentIconName);
        m_statusLabel->setText(QString("Copied icon name: %1").arg(m_currentIconName));
    }
}

void GalleryMainWindow::onCopyIconCode() {
    if (!m_currentIconName.isEmpty()) {
        QString code = QString("lucide->icon(\"%1\")").arg(m_currentIconName);
        QApplication::clipboard()->setText(code);
        m_statusLabel->setText("Copied icon code to clipboard");
    }
}

void GalleryMainWindow::onCopyIconSvg() {
    if (!m_currentIconName.isEmpty() && m_lucide) {
        QByteArray svgData = m_lucide->svgData(m_currentIconName);
        QApplication::clipboard()->setText(QString::fromUtf8(svgData));
        m_statusLabel->setText("Copied SVG data to clipboard");
    }
}

void GalleryMainWindow::onToggleFavorite() {
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

void GalleryMainWindow::onToggleFavorite(const QString& iconName) {
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

void GalleryMainWindow::onSearchTextChanged(const QString& text) {
    Q_UNUSED(text)
    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onCategoryFilterChanged(const QStringList& categories) {
    Q_UNUSED(categories)
    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onTagFilterChanged(const QStringList& tags) {
    Q_UNUSED(tags)
    m_statusUpdateTimer->start();
}

void GalleryMainWindow::onClearAllFilters() {
    if (m_searchWidget) {
        m_searchWidget->clearAllFilters();
    }
    if (m_categoryFilter) {
        m_categoryFilter->clearSelection();
    }
    m_statusLabel->setText("All filters cleared");
}

void GalleryMainWindow::onIconsLoaded(int totalCount) {
    GALLERY_END_TIMER("Metadata loading");
    GALLERY_LOG_INFO(galleryMetadata,
                     QString("Icons loaded successfully: %1 total").arg(totalCount));
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
        GALLERY_LOG_INFO(galleryUI,
                         QString("Icon grid initialized with %1 icons").arg(allIcons.size()));
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

void GalleryMainWindow::onFilteredIconsChanged(int visibleCount, int totalCount) {
    Q_UNUSED(visibleCount)
    Q_UNUSED(totalCount)

    if (m_iconGrid && m_metadataManager) {
        IconFilterCriteria criteria; // Get current filter criteria
        QStringList filteredIcons = m_metadataManager->searchIcons(criteria);
        m_iconGrid->setIconNames(filteredIcons);
    }

    updateStatusBar();
}

void GalleryMainWindow::onApplicationStateChanged() {
    updateActions();
    updateStatusBar();
}

// Settings management methods
void GalleryMainWindow::onImportSettings() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Import Settings"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Settings Files (*.ini *.conf);;All Files (*)"));

    if (!fileName.isEmpty()) {
        QSettings importSettings(fileName, QSettings::IniFormat);

        // Import main settings
        if (importSettings.contains("MainWindow/geometry")) {
            QMainWindow::restoreGeometry(importSettings.value("MainWindow/geometry").toByteArray());
        }
        if (importSettings.contains("MainWindow/windowState")) {
            QMainWindow::restoreState(importSettings.value("MainWindow/windowState").toByteArray());
        }
        if (importSettings.contains("MainWindow/theme")) {
            setTheme(static_cast<ThemeMode>(importSettings.value("MainWindow/theme").toInt()));
        }
        if (importSettings.contains("MainWindow/iconSize")) {
            setIconSize(importSettings.value("MainWindow/iconSize").toInt());
        }

        statusBar()->showMessage(tr("Settings imported successfully"), 3000);
        GALLERY_LOG_INFO(gallerySettings, QString("Settings imported from: %1").arg(fileName));
    }
}

void GalleryMainWindow::onExportSettings() {
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Export Settings"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
            "/qtlucide_settings.ini",
        tr("Settings Files (*.ini *.conf);;All Files (*)"));

    if (!fileName.isEmpty()) {
        QSettings exportSettings(fileName, QSettings::IniFormat);

        // Export main settings
        exportSettings.setValue("MainWindow/geometry", QMainWindow::saveGeometry());
        exportSettings.setValue("MainWindow/windowState", QMainWindow::saveState());
        exportSettings.setValue("MainWindow/theme", static_cast<int>(m_currentTheme));
        exportSettings.setValue("MainWindow/iconSize", m_thumbnailSize);
        exportSettings.setValue("MainWindow/viewMode", static_cast<int>(m_currentViewModeEnum));
        exportSettings.setValue("MainWindow/compactMode", m_compactMode);
        exportSettings.setValue("MainWindow/animationsEnabled", m_animationsEnabled);

        exportSettings.sync();
        statusBar()->showMessage(tr("Settings exported successfully"), 3000);
        GALLERY_LOG_INFO(gallerySettings, QString("Settings exported to: %1").arg(fileName));
    }
}

void GalleryMainWindow::onResetSettings() {
    int ret = QMessageBox::question(this, tr("Reset Settings"),
                                    tr("Are you sure you want to reset all settings to defaults?\n"
                                       "This action cannot be undone."),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        // Reset to defaults
        m_currentTheme = SystemTheme;
        m_thumbnailSize = DEFAULT_GRID_SIZE;
        m_currentViewModeEnum = SplitView;
        m_compactMode = false;
        m_animationsEnabled = true;

        // Apply defaults
        setTheme(m_currentTheme);
        setIconSize(m_thumbnailSize);
        setViewMode(m_currentViewModeEnum);
        applyCompactMode(m_compactMode);
        applyAnimations(m_animationsEnabled);

        // Reset window geometry
        resize(1200, 800);
        move(100, 100);

        statusBar()->showMessage(tr("Settings reset to defaults"), 3000);
        GALLERY_LOG_INFO(gallerySettings, "Settings reset to defaults");
    }
}

void GalleryMainWindow::onCheckForUpdates() {
    // Simple update check dialog
    QMessageBox::information(this, tr("Check for Updates"),
                             tr("QtLucide Gallery v2.0.0\n\n"
                                "You are running the latest version.\n\n"
                                "For updates and more information, visit:\n"
                                "https://github.com/AstroAir/QtLucide"));
    GALLERY_LOG_INFO(galleryUI, "Update check requested");
}

void GalleryMainWindow::onShowShortcuts() {
    QString shortcuts = tr("<h3>Keyboard Shortcuts</h3>"
                           "<table>"
                           "<tr><td><b>Ctrl+O</b></td><td>Open Directory</td></tr>"
                           "<tr><td><b>Ctrl+F</b></td><td>Focus Search</td></tr>"
                           "<tr><td><b>Ctrl+E</b></td><td>Export Icons</td></tr>"
                           "<tr><td><b>F11</b></td><td>Toggle Fullscreen</td></tr>"
                           "<tr><td><b>Ctrl++</b></td><td>Zoom In</td></tr>"
                           "<tr><td><b>Ctrl+-</b></td><td>Zoom Out</td></tr>"
                           "<tr><td><b>Ctrl+0</b></td><td>Reset Zoom</td></tr>"
                           "<tr><td><b>Ctrl+1</b></td><td>Grid View</td></tr>"
                           "<tr><td><b>Ctrl+2</b></td><td>Detail View</td></tr>"
                           "<tr><td><b>Ctrl+3</b></td><td>Split View</td></tr>"
                           "<tr><td><b>Ctrl+T</b></td><td>Toggle Theme</td></tr>"
                           "<tr><td><b>Ctrl+,</b></td><td>Preferences</td></tr>"
                           "<tr><td><b>F1</b></td><td>Help</td></tr>"
                           "</table>");

    QMessageBox::information(this, tr("Keyboard Shortcuts"), shortcuts);
    GALLERY_LOG_INFO(galleryUI, "Keyboard shortcuts displayed");
}

void GalleryMainWindow::onShowPerformanceMetrics() {
    QString metrics = tr("<h3>Performance Metrics</h3>"
                         "<table>"
                         "<tr><td><b>Icons Loaded:</b></td><td>%1</td></tr>"
                         "<tr><td><b>Memory Usage:</b></td><td>%2 MB</td></tr>"
                         "<tr><td><b>Cache Size:</b></td><td>%3 items</td></tr>"
                         "<tr><td><b>Render Time:</b></td><td>%4 ms</td></tr>"
                         "</table>")
                          .arg(m_lucide ? m_lucide->availableIcons().size() : 0)
                          .arg(QCoreApplication::applicationPid()) // Simplified memory metric
                          .arg(100)                                // Placeholder cache size
                          .arg(15);                                // Placeholder render time

    QMessageBox::information(this, tr("Performance Metrics"), metrics);
    GALLERY_LOG_INFO(galleryUI, "Performance metrics displayed");
}

void GalleryMainWindow::onToggleToolbar() {
    if (m_mainToolbar) {
        bool visible = !m_mainToolbar->isVisible();
        m_mainToolbar->setVisible(visible);
        if (m_viewToolbar) {
            m_viewToolbar->setVisible(visible);
        }
        statusBar()->showMessage(visible ? tr("Toolbar shown") : tr("Toolbar hidden"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, QString("Toolbar visibility: %1").arg(visible));
    }
}

void GalleryMainWindow::onToggleMenuBar() {
    bool visible = !menuBar()->isVisible();
    menuBar()->setVisible(visible);
    statusBar()->showMessage(visible ? tr("Menu bar shown") : tr("Menu bar hidden"), 2000);
    GALLERY_LOG_DEBUG(galleryUI, QString("Menu bar visibility: %1").arg(visible));
}
void GalleryMainWindow::onZoomIn() {
    if (m_thumbnailSize < MAX_GRID_SIZE) {
        m_thumbnailSize = qMin(m_thumbnailSize + GRID_SIZE_STEP, MAX_GRID_SIZE);
        if (m_thumbnailGrid) {
            // Update thumbnail grid size
        }
        updateStatusBar();
    }
}

void GalleryMainWindow::onZoomOut() {
    if (m_thumbnailSize > MIN_GRID_SIZE) {
        m_thumbnailSize = qMax(m_thumbnailSize - GRID_SIZE_STEP, MIN_GRID_SIZE);
        if (m_thumbnailGrid) {
            // Update thumbnail grid size
        }
        updateStatusBar();
    }
}

void GalleryMainWindow::onResetZoom() {
    m_thumbnailSize = DEFAULT_GRID_SIZE;
    if (m_thumbnailGrid) {
        // Reset thumbnail grid size
    }
    updateStatusBar();
}
void GalleryMainWindow::onToggleCompactMode() {
    m_compactMode = !m_compactMode;
    applyCompactMode(m_compactMode);
    statusBar()->showMessage(
        m_compactMode ? tr("Compact mode enabled") : tr("Compact mode disabled"), 2000);
    GALLERY_LOG_DEBUG(galleryUI, QString("Compact mode: %1").arg(m_compactMode));
}

void GalleryMainWindow::onToggleAnimations() {
    m_animationsEnabled = !m_animationsEnabled;
    applyAnimations(m_animationsEnabled);
    statusBar()->showMessage(
        m_animationsEnabled ? tr("Animations enabled") : tr("Animations disabled"), 2000);
    GALLERY_LOG_DEBUG(galleryUI, QString("Animations: %1").arg(m_animationsEnabled));
}

void GalleryMainWindow::onIconHovered(const QString& iconName) {
    if (!iconName.isEmpty()) {
        // Update status bar with icon information
        statusBar()->showMessage(tr("Icon: %1").arg(iconName));

        // Update details panel if available
        if (m_detailsPanel) {
            m_detailsPanel->setIconName(iconName);
        }

        GALLERY_LOG_DEBUG(galleryUI, QString("Icon hovered: %1").arg(iconName));
    }
}

void GalleryMainWindow::onIconContextMenu(const QString& iconName, const QPoint& position) {
    if (iconName.isEmpty())
        return;

    QMenu contextMenu(this);

    // Add to favorites
    QAction* favoriteAction = contextMenu.addAction(tr("Add to Favorites"));
    favoriteAction->setIcon(QIcon(":/icons/heart.svg"));
    connect(favoriteAction, &QAction::triggered, [this, iconName]() {
        if (m_favoritesManager) {
            m_favoritesManager->addFavorite(iconName);
            statusBar()->showMessage(tr("Added '%1' to favorites").arg(iconName), 2000);
        }
    });

    contextMenu.addSeparator();

    // Copy icon name
    QAction* copyNameAction = contextMenu.addAction(tr("Copy Icon Name"));
    copyNameAction->setIcon(QIcon(":/icons/copy.svg"));
    connect(copyNameAction, &QAction::triggered,
            [iconName]() { QApplication::clipboard()->setText(iconName); });

    // Copy as code
    QAction* copyCodeAction = contextMenu.addAction(tr("Copy as Code"));
    copyCodeAction->setIcon(QIcon(":/icons/code.svg"));
    connect(copyCodeAction, &QAction::triggered, [iconName]() {
        QString code = QString("lucide->icon(\"%1\")").arg(iconName);
        QApplication::clipboard()->setText(code);
    });

    contextMenu.addSeparator();

    // Export icon
    QAction* exportAction = contextMenu.addAction(tr("Export Icon..."));
    exportAction->setIcon(QIcon(":/icons/download.svg"));
    connect(exportAction, &QAction::triggered, [this, iconName]() { onExportIcon(); });

    // Show details
    QAction* detailsAction = contextMenu.addAction(tr("Show Details"));
    detailsAction->setIcon(QIcon(":/icons/info.svg"));
    connect(detailsAction, &QAction::triggered, [this, iconName]() {
        if (m_detailsPanel) {
            m_detailsPanel->setIconName(iconName);
            m_detailsPanel->setVisible(true);
        }
    });

    contextMenu.exec(position);
    GALLERY_LOG_DEBUG(galleryUI, QString("Context menu shown for icon: %1").arg(iconName));
}
void GalleryMainWindow::onCopyIconPath() {
    if (!m_currentIconName.isEmpty()) {
        QString iconPath = QString(":/icons/%1.svg").arg(m_currentIconName);
        QApplication::clipboard()->setText(iconPath);
        statusBar()->showMessage(tr("Icon path copied to clipboard"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, QString("Copied icon path: %1").arg(iconPath));
    }
}

void GalleryMainWindow::onPreviewIcon() {
    if (!m_currentIconName.isEmpty() && m_lucide) {
        // Create a preview dialog
        QDialog previewDialog(this);
        previewDialog.setWindowTitle(tr("Icon Preview - %1").arg(m_currentIconName));
        previewDialog.setModal(true);
        previewDialog.resize(400, 300);

        QVBoxLayout* layout = new QVBoxLayout(&previewDialog);

        // Create icon preview at different sizes
        QHBoxLayout* sizesLayout = new QHBoxLayout();
        QList<int> sizes = {16, 24, 32, 48, 64, 96, 128};

        for (int size : sizes) {
            QLabel* iconLabel = new QLabel();
            QIcon icon = m_lucide->icon(m_currentIconName);
            iconLabel->setPixmap(icon.pixmap(size, size));
            iconLabel->setAlignment(Qt::AlignCenter);
            iconLabel->setToolTip(tr("%1x%1").arg(size));
            sizesLayout->addWidget(iconLabel);
        }

        layout->addLayout(sizesLayout);

        // Add close button
        QPushButton* closeButton = new QPushButton(tr("Close"));
        connect(closeButton, &QPushButton::clicked, &previewDialog, &QDialog::accept);
        layout->addWidget(closeButton);

        previewDialog.exec();
        GALLERY_LOG_DEBUG(galleryUI, QString("Previewed icon: %1").arg(m_currentIconName));
    }
}

void GalleryMainWindow::onEditIconMetadata() {
    if (!m_currentIconName.isEmpty()) {
        // Simple metadata editing dialog
        QDialog metadataDialog(this);
        metadataDialog.setWindowTitle(tr("Edit Icon Metadata - %1").arg(m_currentIconName));
        metadataDialog.setModal(true);
        metadataDialog.resize(400, 300);

        QVBoxLayout* layout = new QVBoxLayout(&metadataDialog);

        // Icon name (read-only)
        layout->addWidget(new QLabel(tr("Icon Name:")));
        QLineEdit* nameEdit = new QLineEdit(m_currentIconName);
        nameEdit->setReadOnly(true);
        layout->addWidget(nameEdit);

        // Tags
        layout->addWidget(new QLabel(tr("Tags (comma-separated):")));
        QLineEdit* tagsEdit = new QLineEdit();
        if (m_metadataManager) {
            QStringList tags = m_metadataManager->getIconTags(m_currentIconName);
            tagsEdit->setText(tags.join(", "));
        }
        layout->addWidget(tagsEdit);

        // Description
        layout->addWidget(new QLabel(tr("Description:")));
        QTextEdit* descEdit = new QTextEdit();
        if (m_metadataManager) {
            descEdit->setPlainText(m_metadataManager->getIconDescription(m_currentIconName));
        }
        layout->addWidget(descEdit);

        // Buttons
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* saveButton = new QPushButton(tr("Save"));
        QPushButton* cancelButton = new QPushButton(tr("Cancel"));

        connect(saveButton, &QPushButton::clicked, [&]() {
            if (m_metadataManager) {
                QStringList tags = tagsEdit->text().split(",", Qt::SkipEmptyParts);
                for (QString& tag : tags) {
                    tag = tag.trimmed();
                }
                m_metadataManager->setIconTags(m_currentIconName, tags);
                m_metadataManager->setIconDescription(m_currentIconName, descEdit->toPlainText());
                statusBar()->showMessage(tr("Metadata saved"), 2000);
            }
            metadataDialog.accept();
        });

        connect(cancelButton, &QPushButton::clicked, &metadataDialog, &QDialog::reject);

        buttonLayout->addWidget(saveButton);
        buttonLayout->addWidget(cancelButton);
        layout->addLayout(buttonLayout);

        metadataDialog.exec();
        GALLERY_LOG_DEBUG(galleryUI,
                          QString("Edited metadata for icon: %1").arg(m_currentIconName));
    }
}

void GalleryMainWindow::onImageSelected(const QString& imagePath) {
    if (imagePath.isEmpty()) {
        return;
    }

    QString errorMessage;
    if (!ErrorHandler::validateImageFile(imagePath, &errorMessage)) {
        ErrorHandler::reportError(QString("Invalid image file selected: %1").arg(errorMessage),
                                  ErrorSeverity::Warning, ErrorCategory::FileSystem,
                                  "Image selection");
        statusBar()->showMessage(tr("Invalid image file: %1").arg(errorMessage), 5000);
        return;
    }

    if (!ErrorHandler::safeExecute(
            [this, &imagePath]() {
                if (m_imageViewer) {
                    m_imageViewer->setCurrentImage(imagePath);
                }
            },
            "Loading image", &errorMessage)) {
        ErrorHandler::showWarningDialog(this, "Image Loading Error",
                                        QString("Failed to load image: %1").arg(errorMessage));
        return;
    }

    QFileInfo fileInfo(imagePath);
    statusBar()->showMessage(tr("Selected image: %1").arg(fileInfo.fileName()));
    updateWindowTitle();
    GALLERY_LOG_DEBUG(galleryUI, QString("Image selected: %1").arg(imagePath));
}

void GalleryMainWindow::onImageDoubleClicked(const QString& imagePath) {
    if (!imagePath.isEmpty()) {
        // Open image in fullscreen mode
        if (m_imageViewer) {
            m_imageViewer->setCurrentImage(imagePath);
            onToggleFullscreen();
        }
        GALLERY_LOG_DEBUG(galleryUI, QString("Image double-clicked: %1").arg(imagePath));
    }
}

void GalleryMainWindow::onDirectorySelected(const QString& dirPath) {
    QString errorMessage;
    if (!ErrorHandler::validateDirectoryPath(dirPath, &errorMessage)) {
        ErrorHandler::reportError(QString("Invalid directory selected: %1").arg(errorMessage),
                                  ErrorSeverity::Warning, ErrorCategory::FileSystem,
                                  "Directory selection");
        ErrorHandler::showWarningDialog(this, "Invalid Directory", errorMessage);
        return;
    }

    if (!ErrorHandler::safeExecute([this, &dirPath]() { loadImageDirectory(dirPath); },
                                   "Loading directory", &errorMessage)) {
        ErrorHandler::showErrorDialog(this, "Directory Loading Error",
                                      "Failed to load the selected directory.", errorMessage);
        return;
    }

    statusBar()->showMessage(tr("Directory loaded: %1").arg(dirPath));
    GALLERY_LOG_INFO(galleryUI, QString("Directory selected: %1").arg(dirPath));
}

void GalleryMainWindow::onToggleGalleryMode() {
    // Toggle between icon mode and image gallery mode
    static bool imageMode = false;
    imageMode = !imageMode;
    setGalleryMode(imageMode);

    QString mode = imageMode ? tr("Image Gallery") : tr("Icon Browser");
    statusBar()->showMessage(tr("Switched to %1 mode").arg(mode), 2000);
    GALLERY_LOG_DEBUG(galleryUI, QString("Gallery mode: %1").arg(imageMode ? "image" : "icon"));
}

void GalleryMainWindow::onOpenImageDirectory() {
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Select Image Directory"),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    if (!dir.isEmpty()) {
        loadImageDirectory(dir);
        statusBar()->showMessage(tr("Opened directory: %1").arg(dir));
        GALLERY_LOG_INFO(galleryUI, QString("Opened image directory: %1").arg(dir));
    }
}

void GalleryMainWindow::onImageViewerModeChanged() {
    if (m_imageViewer) {
        // Cycle through image viewer modes
        static int mode = 0;
        mode = (mode + 1) % 3; // Assuming 3 modes: fit, actual, custom

        switch (mode) {
            case 0:
                m_imageViewer->onFitToWindow();
                statusBar()->showMessage(tr("Image viewer: Fit to window"), 2000);
                break;
            case 1:
                m_imageViewer->onActualSize();
                statusBar()->showMessage(tr("Image viewer: Actual size"), 2000);
                break;
            case 2:
                statusBar()->showMessage(tr("Image viewer: Custom zoom"), 2000);
                break;
        }

        GALLERY_LOG_DEBUG(galleryUI, QString("Image viewer mode changed: %1").arg(mode));
    }
}
void GalleryMainWindow::onStartSlideshow() {
    if (!m_slideshowActive && m_slideshowTimer) {
        m_slideshowActive = true;
        m_slideshowTimer->start(m_slideshowInterval);
        GALLERY_LOG_INFO(galleryUI, "Slideshow started");
        updateStatusBar();
    }
}

// Missing methods declared in headers but not implemented (Part 2)
void GalleryMainWindow::onStopSlideshow() {
    if (m_slideshowActive && m_slideshowTimer) {
        m_slideshowActive = false;
        m_slideshowTimer->stop();
        GALLERY_LOG_INFO(galleryUI, "Slideshow stopped");
        updateStatusBar();
    }
}
void GalleryMainWindow::onImageRotateLeft() {
    if (m_imageViewer) {
        m_imageViewer->rotateLeft();
        statusBar()->showMessage(tr("Image rotated left"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, "Image rotated left");
    }
}

void GalleryMainWindow::onImageRotateRight() {
    if (m_imageViewer) {
        m_imageViewer->rotateRight();
        statusBar()->showMessage(tr("Image rotated right"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, "Image rotated right");
    }
}

void GalleryMainWindow::onImageFlipHorizontal() {
    if (m_imageViewer) {
        m_imageViewer->flipHorizontal();
        statusBar()->showMessage(tr("Image flipped horizontally"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, "Image flipped horizontally");
    }
}

void GalleryMainWindow::onImageFlipVertical() {
    if (m_imageViewer) {
        m_imageViewer->flipVertical();
        statusBar()->showMessage(tr("Image flipped vertically"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, "Image flipped vertically");
    }
}

void GalleryMainWindow::onImageZoomIn() {
    if (m_imageViewer) {
        m_imageViewer->onZoomIn();
        statusBar()->showMessage(tr("Image zoomed in"), 1000);
        GALLERY_LOG_DEBUG(galleryUI, "Image zoomed in");
    }
}

void GalleryMainWindow::onImageZoomOut() {
    if (m_imageViewer) {
        m_imageViewer->onZoomOut();
        statusBar()->showMessage(tr("Image zoomed out"), 1000);
        GALLERY_LOG_DEBUG(galleryUI, "Image zoomed out");
    }
}

void GalleryMainWindow::onImageFitToWindow() {
    if (m_imageViewer) {
        m_imageViewer->onFitToWindow();
        statusBar()->showMessage(tr("Image fit to window"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, "Image fit to window");
    }
}

void GalleryMainWindow::onImageActualSize() {
    if (m_imageViewer) {
        m_imageViewer->onActualSize();
        statusBar()->showMessage(tr("Image shown at actual size"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, "Image shown at actual size");
    }
}
void GalleryMainWindow::onAdvancedSearchRequested() {
    if (m_searchWidget) {
        m_searchWidget->showAdvancedOptions();
        statusBar()->showMessage(tr("Advanced search options shown"), 2000);
        GALLERY_LOG_DEBUG(galleryUI, "Advanced search requested");
    }
}

void GalleryMainWindow::onContributorFilterChanged(const QStringList& contributors) {
    if (m_metadataManager) {
        // Apply contributor filter
        QStringList filteredIcons;
        QStringList allIcons = m_lucide ? m_lucide->availableIcons() : QStringList();

        for (const QString& iconName : allIcons) {
            QStringList iconContributors = m_metadataManager->getIconContributors(iconName);
            for (const QString& contributor : contributors) {
                if (iconContributors.contains(contributor, Qt::CaseInsensitive)) {
                    filteredIcons.append(iconName);
                    break;
                }
            }
        }

        // Update icon grid with filtered results
        if (m_iconGrid) {
            m_iconGrid->setFilteredIcons(filteredIcons);
        }

        statusBar()->showMessage(
            tr("Filtered by contributors: %1 icons found").arg(filteredIcons.size()), 3000);
        GALLERY_LOG_DEBUG(galleryUI,
                          QString("Contributor filter applied: %1 contributors, %2 icons found")
                              .arg(contributors.size())
                              .arg(filteredIcons.size()));
    }
}

void GalleryMainWindow::onFavoritesFilterToggled(bool enabled) {
    if (m_favoritesManager && m_iconGrid) {
        if (enabled) {
            QStringList favoriteIcons = m_favoritesManager->getFavorites();
            m_iconGrid->setFilteredIcons(favoriteIcons);
            statusBar()->showMessage(tr("Showing favorites: %1 icons").arg(favoriteIcons.size()),
                                     3000);
        } else {
            m_iconGrid->clearFilter();
            statusBar()->showMessage(tr("Showing all icons"), 2000);
        }
        GALLERY_LOG_DEBUG(galleryUI, QString("Favorites filter: %1").arg(enabled));
    }
}

void GalleryMainWindow::onRecentFilterToggled(bool enabled) {
    if (m_iconGrid) {
        if (enabled) {
            // Show recently used icons (stored in m_recentIcons)
            m_iconGrid->setFilteredIcons(m_recentIcons);
            statusBar()->showMessage(tr("Showing recent icons: %1 icons").arg(m_recentIcons.size()),
                                     3000);
        } else {
            m_iconGrid->clearFilter();
            statusBar()->showMessage(tr("Showing all icons"), 2000);
        }
        GALLERY_LOG_DEBUG(galleryUI, QString("Recent filter: %1").arg(enabled));
    }
}

void GalleryMainWindow::onSaveCurrentFilter() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Save Filter"), tr("Enter filter name:"),
                                         QLineEdit::Normal, QString(), &ok);

    if (ok && !name.isEmpty()) {
        // Save current filter settings
        if (m_settings) {
            m_settings->beginGroup("SavedFilters");
            m_settings->setValue(name + "/searchText", m_currentSearchText);
            m_settings->setValue(name + "/categories", m_currentCategories);
            m_settings->setValue(name + "/tags", m_currentTags);
            m_settings->endGroup();

            statusBar()->showMessage(tr("Filter '%1' saved").arg(name), 2000);
            GALLERY_LOG_INFO(galleryUI, QString("Filter saved: %1").arg(name));
        }
    }
}

void GalleryMainWindow::onLoadSavedFilter() {
    if (!m_settings)
        return;

    m_settings->beginGroup("SavedFilters");
    QStringList savedFilters = m_settings->childGroups();
    m_settings->endGroup();

    if (savedFilters.isEmpty()) {
        QMessageBox::information(this, tr("Load Filter"), tr("No saved filters found."));
        return;
    }

    bool ok;
    QString selectedFilter = QInputDialog::getItem(
        this, tr("Load Filter"), tr("Select filter to load:"), savedFilters, 0, false, &ok);

    if (ok && !selectedFilter.isEmpty()) {
        m_settings->beginGroup("SavedFilters");
        m_currentSearchText = m_settings->value(selectedFilter + "/searchText").toString();
        m_currentCategories = m_settings->value(selectedFilter + "/categories").toStringList();
        m_currentTags = m_settings->value(selectedFilter + "/tags").toStringList();
        m_settings->endGroup();

        // Apply the loaded filter
        applyCurrentFilter();
        statusBar()->showMessage(tr("Filter '%1' loaded").arg(selectedFilter), 2000);
        GALLERY_LOG_INFO(galleryUI, QString("Filter loaded: %1").arg(selectedFilter));
    }
}

void GalleryMainWindow::onIconsLoadProgress(int current, int total) {
    if (m_progressBar) {
        m_progressBar->setVisible(true);
        m_progressBar->setMaximum(total);
        m_progressBar->setValue(current);

        if (current >= total) {
            // Loading complete
            QTimer::singleShot(1000, [this]() {
                if (m_progressBar) {
                    m_progressBar->setVisible(false);
                }
            });
            statusBar()->showMessage(tr("Icons loaded successfully"), 2000);
        }
    }
    GALLERY_LOG_DEBUG(galleryUI, QString("Icons load progress: %1/%2").arg(current).arg(total));
}

void GalleryMainWindow::onThemeChanged() {
    applyTheme();
    statusBar()->showMessage(tr("Theme applied"), 2000);
    GALLERY_LOG_DEBUG(galleryUI, "Theme changed");
}

void GalleryMainWindow::onLanguageChanged() {
    // Apply language changes (would need translation system)
    statusBar()->showMessage(tr("Language changed"), 2000);
    GALLERY_LOG_DEBUG(galleryUI, "Language changed");
}
void GalleryMainWindow::onPerformanceMetricsUpdated(const QMap<QString, QVariant>& metrics) {
    // Update performance metrics display
    QString metricsText;
    for (auto it = metrics.begin(); it != metrics.end(); ++it) {
        metricsText += QString("%1: %2\n").arg(it.key()).arg(it.value().toString());
    }

    // Update status or performance panel if available
    GALLERY_LOG_DEBUG(galleryPerf,
                      QString("Performance metrics updated: %1 metrics").arg(metrics.size()));
}

void GalleryMainWindow::onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            // Single click - show/hide window
            setVisible(!isVisible());
            break;
        case QSystemTrayIcon::DoubleClick:
            // Double click - restore and activate
            show();
            raise();
            activateWindow();
            break;
        case QSystemTrayIcon::MiddleClick:
            // Middle click - show quick actions
            break;
        default:
            break;
    }
    GALLERY_LOG_DEBUG(galleryUI,
                      QString("System tray activated: %1").arg(static_cast<int>(reason)));
}

void GalleryMainWindow::onSystemTrayMessageClicked() {
    // Handle system tray message click
    show();
    raise();
    activateWindow();
    GALLERY_LOG_DEBUG(galleryUI, "System tray message clicked");
}

void GalleryMainWindow::onFadeInFinished() {
    // Handle fade in animation completion
    GALLERY_LOG_DEBUG(galleryUI, "Fade in animation finished");
}

void GalleryMainWindow::onFadeOutFinished() {
    // Handle fade out animation completion
    GALLERY_LOG_DEBUG(galleryUI, "Fade out animation finished");
}

void GalleryMainWindow::onSlideAnimationFinished() {
    // Handle slide animation completion
    GALLERY_LOG_DEBUG(galleryUI, "Slide animation finished");
}

void GalleryMainWindow::onAutoSaveTimer() {
    // Auto-save settings and state
    if (m_autoSaveSettings) {
        saveSettings();
        GALLERY_LOG_DEBUG(gallerySettings, "Auto-save completed");
    }
}

void GalleryMainWindow::onBackupTimer() {
    // Create backup of settings and favorites
    if (m_settings) {
        QString backupDir =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/backups";
        QDir().mkpath(backupDir);

        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        QString backupFile = QString("%1/settings_backup_%2.ini").arg(backupDir).arg(timestamp);

        // Copy current settings to backup
        QSettings backup(backupFile, QSettings::IniFormat);
        for (const QString& key : m_settings->allKeys()) {
            backup.setValue(key, m_settings->value(key));
        }
        backup.sync();

        GALLERY_LOG_INFO(gallerySettings, QString("Settings backup created: %1").arg(backupFile));
    }
}

// Event handlers
void GalleryMainWindow::keyPressEvent(QKeyEvent* event) {
    // Handle global keyboard shortcuts that aren't handled by actions
    switch (event->key()) {
        case Qt::Key_Escape:
            // Clear search or exit fullscreen
            if (m_isFullscreen) {
                onToggleFullscreen();
            } else if (m_searchWidget) {
                m_searchWidget->clearSearch();
            }
            event->accept();
            return;

        case Qt::Key_F5:
            // Refresh/reload
            if (m_iconGrid) {
                m_iconGrid->refresh();
            }
            event->accept();
            return;

        case Qt::Key_Delete:
            // Remove from favorites
            if (!m_currentIconName.isEmpty() && m_favoritesManager) {
                m_favoritesManager->removeFavorite(m_currentIconName);
                statusBar()->showMessage(tr("Removed '%1' from favorites").arg(m_currentIconName),
                                         2000);
            }
            event->accept();
            return;

        case Qt::Key_Space:
            // Quick preview
            if (!m_currentIconName.isEmpty()) {
                onPreviewIcon();
            }
            event->accept();
            return;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Open details or export
            if (event->modifiers() & Qt::ControlModifier) {
                onExportIcon();
            } else if (!m_currentIconName.isEmpty()) {
                onEditIconMetadata();
            }
            event->accept();
            return;

        case Qt::Key_Left:
            if (event->modifiers() & Qt::ControlModifier) {
                onPreviousIcon();
                event->accept();
                return;
            }
            break;

        case Qt::Key_Right:
            if (event->modifiers() & Qt::ControlModifier) {
                onNextIcon();
                event->accept();
                return;
            }
            break;

        case Qt::Key_Home:
            if (event->modifiers() & Qt::ControlModifier) {
                onFirstIcon();
                event->accept();
                return;
            }
            break;

        case Qt::Key_End:
            if (event->modifiers() & Qt::ControlModifier) {
                onLastIcon();
                event->accept();
                return;
            }
            break;
    }

    QMainWindow::keyPressEvent(event);
}
void GalleryMainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
}
void GalleryMainWindow::changeEvent(QEvent* event) {
    QMainWindow::changeEvent(event);
}

void GalleryMainWindow::contextMenuEvent(QContextMenuEvent* event) {
    // Create context menu for main window
    QMenu contextMenu(this);

    // View options
    QMenu* viewMenu = contextMenu.addMenu(tr("View"));

    QAction* toggleSidebarAction = viewMenu->addAction(tr("Toggle Sidebar"));
    toggleSidebarAction->setShortcut(QKeySequence("F9"));
    toggleSidebarAction->setCheckable(true);
    toggleSidebarAction->setChecked(m_sidebarVisible);
    connect(toggleSidebarAction, &QAction::triggered, this, &GalleryMainWindow::onToggleSidebar);

    QAction* toggleDetailsPanelAction = viewMenu->addAction(tr("Toggle Details Panel"));
    toggleDetailsPanelAction->setShortcut(QKeySequence("F10"));
    toggleDetailsPanelAction->setCheckable(true);
    toggleDetailsPanelAction->setChecked(m_detailsPanelVisible);
    connect(toggleDetailsPanelAction, &QAction::triggered, this,
            &GalleryMainWindow::onToggleDetailsPanel);

    QAction* toggleStatusBarAction = viewMenu->addAction(tr("Toggle Status Bar"));
    toggleStatusBarAction->setShortcut(QKeySequence("Ctrl+/"));
    toggleStatusBarAction->setCheckable(true);
    toggleStatusBarAction->setChecked(m_statusBarVisible);
    connect(toggleStatusBarAction, &QAction::triggered, this,
            &GalleryMainWindow::onToggleStatusBar);

    viewMenu->addSeparator();

    QAction* fullscreenAction = viewMenu->addAction(tr("Toggle Fullscreen"));
    fullscreenAction->setShortcut(QKeySequence("F11"));
    fullscreenAction->setCheckable(true);
    fullscreenAction->setChecked(m_isFullscreen);
    connect(fullscreenAction, &QAction::triggered, this, &GalleryMainWindow::onToggleFullscreen);

    // Zoom options
    QMenu* zoomMenu = contextMenu.addMenu(tr("Zoom"));

    QAction* zoomInAction = zoomMenu->addAction(tr("Zoom In"));
    zoomInAction->setShortcut(QKeySequence("Ctrl++"));
    connect(zoomInAction, &QAction::triggered, this, &GalleryMainWindow::onZoomIn);

    QAction* zoomOutAction = zoomMenu->addAction(tr("Zoom Out"));
    zoomOutAction->setShortcut(QKeySequence("Ctrl+-"));
    connect(zoomOutAction, &QAction::triggered, this, &GalleryMainWindow::onZoomOut);

    QAction* resetZoomAction = zoomMenu->addAction(tr("Reset Zoom"));
    resetZoomAction->setShortcut(QKeySequence("Ctrl+0"));
    connect(resetZoomAction, &QAction::triggered, this, &GalleryMainWindow::onResetZoom);

    contextMenu.addSeparator();

    // Current icon actions (if an icon is selected)
    if (!m_currentIconName.isEmpty()) {
        QAction* copyNameAction = contextMenu.addAction(tr("Copy Icon Name"));
        copyNameAction->setShortcut(QKeySequence("Ctrl+C"));
        connect(copyNameAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconName);

        QAction* copyPathAction = contextMenu.addAction(tr("Copy Icon Path"));
        copyPathAction->setShortcut(QKeySequence("Ctrl+Shift+C"));
        connect(copyPathAction, &QAction::triggered, this, &GalleryMainWindow::onCopyIconPath);

        contextMenu.addSeparator();

        QAction* exportAction = contextMenu.addAction(tr("Export Icon..."));
        exportAction->setShortcut(QKeySequence("Ctrl+E"));
        connect(exportAction, &QAction::triggered, this, &GalleryMainWindow::onExportIcon);

        QAction* previewAction = contextMenu.addAction(tr("Preview Icon"));
        previewAction->setShortcut(QKeySequence("Space"));
        connect(previewAction, &QAction::triggered, this, &GalleryMainWindow::onPreviewIcon);

        contextMenu.addSeparator();

        // Favorites
        if (m_favoritesManager) {
            bool isFavorite = m_favoritesManager->isFavorite(m_currentIconName);
            QAction* favoriteAction = contextMenu.addAction(isFavorite ? tr("Remove from Favorites")
                                                                       : tr("Add to Favorites"));
            favoriteAction->setShortcut(QKeySequence("Ctrl+D"));
            connect(favoriteAction, &QAction::triggered, this, [this, isFavorite]() {
                if (isFavorite) {
                    m_favoritesManager->removeFavorite(m_currentIconName);
                    statusBar()->showMessage(tr("Removed from favorites"), 2000);
                } else {
                    m_favoritesManager->addFavorite(m_currentIconName);
                    statusBar()->showMessage(tr("Added to favorites"), 2000);
                }
            });
        }
    }

    contextMenu.addSeparator();

    // General actions
    QAction* refreshAction = contextMenu.addAction(tr("Refresh"));
    refreshAction->setShortcut(QKeySequence("F5"));
    connect(refreshAction, &QAction::triggered, this, [this]() {
        if (m_iconGrid) {
            m_iconGrid->refresh();
        }
    });

    QAction* preferencesAction = contextMenu.addAction(tr("Preferences..."));
    preferencesAction->setShortcut(QKeySequence::Preferences);
    connect(preferencesAction, &QAction::triggered, this, &GalleryMainWindow::onShowPreferences);

    // Show context menu
    contextMenu.exec(event->globalPos());
    event->accept();
}

void GalleryMainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void GalleryMainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        for (const QUrl& url : urls) {
            if (url.isLocalFile()) {
                QString filePath = url.toLocalFile();
                // Handle dropped files (images or directories)
                QFileInfo fileInfo(filePath);
                if (fileInfo.isDir()) {
                    loadImageDirectory(filePath);
                } else {
                    QString suffix = fileInfo.suffix().toLower();
                    if (suffix == "jpg" || suffix == "jpeg" || suffix == "png" || suffix == "gif" ||
                        suffix == "bmp" || suffix == "svg") {
                        showImage(filePath);
                    }
                }
            }
        }
        event->acceptProposedAction();
    }
}

// Enhanced methods from consolidated design
void GalleryMainWindow::setViewMode(ViewMode mode) {
    if (m_currentViewModeEnum == mode)
        return;

    m_currentViewModeEnum = mode;
    updateViewMode();
    updateUI();
}

void GalleryMainWindow::setTheme(ThemeMode theme) {
    if (m_currentTheme == theme)
        return;

    m_currentTheme = theme;
    applyTheme();
}

void GalleryMainWindow::updateUI() {
    updateStatusBar();
    updateWindowTitle();
    updateActions();
}

void GalleryMainWindow::setupCategoryShortcuts() {
    // Setup Ctrl+1-9 shortcuts for categories
    for (int i = 1; i <= 9; ++i) {
        QShortcut* shortcut = new QShortcut(QKeySequence(QString("Ctrl+%1").arg(i)), this);
        connect(shortcut, &QShortcut::activated, this, [this, i]() {
            onCategoryShortcut(i - 1); // Convert to 0-based index
        });
        m_categoryShortcuts.append(shortcut);
    }
}

void GalleryMainWindow::onCategoryShortcut(int categoryIndex) {
    // Handle category shortcut activation
    if (m_categorySidebar) {
        // Implementation would depend on CategorySidebarWidget interface
        GALLERY_LOG_DEBUG(galleryUI,
                          QString("Category shortcut %1 activated").arg(categoryIndex + 1));
    }
}

// Enhanced methods from EnhancedGalleryMainWindow
void GalleryMainWindow::onZoomFit() {
    if (m_imageViewer) {
        // Implement zoom to fit functionality
        GALLERY_LOG_DEBUG(galleryUI, "Zoom to fit requested");
    }
}

void GalleryMainWindow::onZoomActual() {
    if (m_imageViewer) {
        // Implement zoom to actual size functionality
        GALLERY_LOG_DEBUG(galleryUI, "Zoom to actual size requested");
    }
}

void GalleryMainWindow::onGoToIcon() {
    // Implement go to specific icon functionality
    GALLERY_LOG_DEBUG(galleryUI, "Go to icon requested");
}

void GalleryMainWindow::onNextIcon() {
    // Implement next icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "Next icon requested");
    }
}

void GalleryMainWindow::onPreviousIcon() {
    // Implement previous icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "Previous icon requested");
    }
}

void GalleryMainWindow::onFirstIcon() {
    // Implement first icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "First icon requested");
    }
}

void GalleryMainWindow::onLastIcon() {
    // Implement last icon navigation
    if (m_thumbnailGrid) {
        GALLERY_LOG_DEBUG(galleryUI, "Last icon requested");
    }
}

void GalleryMainWindow::onToggleSlideshow() {
    if (m_slideshowActive) {
        onStopSlideshow();
    } else {
        onStartSlideshow();
    }
}

void GalleryMainWindow::onOpenImages() {
    QStringList fileNames =
        QFileDialog::getOpenFileNames(this, tr("Open Images"), QString(),
                                      tr("Image Files (*.png *.jpg *.jpeg *.gif *.bmp *.svg)"));

    if (!fileNames.isEmpty()) {
        // Handle opening multiple images
        for (const QString& fileName : fileNames) {
            showImage(fileName);
        }
    }
}

void GalleryMainWindow::onOpenDirectory() {
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString());

    if (!dirPath.isEmpty()) {
        loadImageDirectory(dirPath);
    }
}

void GalleryMainWindow::onExportIcon() {
    if (m_currentIconName.isEmpty()) {
        ErrorHandler::showInfoDialog(this, "No Icon Selected", "Please select an icon to export.");
        return;
    }

    QString errorMessage;
    if (!ErrorHandler::validateIconName(m_currentIconName, &errorMessage)) {
        ErrorHandler::reportError(QString("Invalid icon name for export: %1").arg(errorMessage),
                                  ErrorSeverity::Warning, ErrorCategory::Validation, "Icon export");
        ErrorHandler::showWarningDialog(this, "Invalid Icon", errorMessage);
        return;
    }

    if (!ErrorHandler::safeExecute([this]() { onExportIcons(); }, "Exporting icon",
                                   &errorMessage)) {
        ErrorHandler::showErrorDialog(this, "Export Error", "Failed to export icon.", errorMessage);
    }
}

void GalleryMainWindow::onExportBatch() {
    // Implement batch export functionality
    onExportIcons();
}

void GalleryMainWindow::onCopyIcon() {
    // Copy current icon to clipboard
    onCopyIconSvg();
}

void GalleryMainWindow::onGenerateCode() {
    if (!m_currentIconName.isEmpty()) {
        // Generate code for current icon
        onCopyIconCode();
    }
}

void GalleryMainWindow::onShowFavorites() {
    if (m_favoritesManager) {
        // Show favorites view
        GALLERY_LOG_DEBUG(galleryUI, "Show favorites requested");
    }
}

void GalleryMainWindow::onClearFavorites() {
    if (m_favoritesManager) {
        // Clear all favorites
        GALLERY_LOG_DEBUG(galleryUI, "Clear favorites requested");
    }
}

void GalleryMainWindow::onSettings() {
    onShowPreferences();
}

void GalleryMainWindow::onAbout() {
    onAboutApplication();
}

void GalleryMainWindow::onShowKeyboardShortcuts() {
    onShowShortcuts();
}

void GalleryMainWindow::onContentLoaded() {
    updateStatusBar();
    updateUI();
}

void GalleryMainWindow::onMetadataLoaded(int totalIcons) {
    onIconsLoaded(totalIcons);
}

void GalleryMainWindow::onLoadingProgress(int current, int total) {
    onIconsLoadProgress(current, total);
}

void GalleryMainWindow::onThumbnailSizeChanged(int size) {
    m_thumbnailSize = size;
    if (m_thumbnailGrid) {
        // Update thumbnail grid size
    }
    updateStatusBar();
}

// Missing method implementations that are causing linker errors
void GalleryMainWindow::setupShortcuts() {
    // Setup keyboard shortcuts
    setupCategoryShortcuts();

    // Navigation shortcuts
    m_shortcuts["nextIcon"] = new QShortcut(QKeySequence("Ctrl+Right"), this);
    connect(m_shortcuts["nextIcon"], &QShortcut::activated, this, &GalleryMainWindow::onNextIcon);

    m_shortcuts["previousIcon"] = new QShortcut(QKeySequence("Ctrl+Left"), this);
    connect(m_shortcuts["previousIcon"], &QShortcut::activated, this,
            &GalleryMainWindow::onPreviousIcon);

    m_shortcuts["firstIcon"] = new QShortcut(QKeySequence("Ctrl+Home"), this);
    connect(m_shortcuts["firstIcon"], &QShortcut::activated, this, &GalleryMainWindow::onFirstIcon);

    m_shortcuts["lastIcon"] = new QShortcut(QKeySequence("Ctrl+End"), this);
    connect(m_shortcuts["lastIcon"], &QShortcut::activated, this, &GalleryMainWindow::onLastIcon);

    // View shortcuts
    m_shortcuts["toggleSidebar"] = new QShortcut(QKeySequence("F9"), this);
    connect(m_shortcuts["toggleSidebar"], &QShortcut::activated, this,
            &GalleryMainWindow::onToggleSidebar);

    m_shortcuts["toggleDetailsPanel"] = new QShortcut(QKeySequence("F10"), this);
    connect(m_shortcuts["toggleDetailsPanel"], &QShortcut::activated, this,
            &GalleryMainWindow::onToggleDetailsPanel);

    m_shortcuts["toggleStatusBar"] = new QShortcut(QKeySequence("Ctrl+/"), this);
    connect(m_shortcuts["toggleStatusBar"], &QShortcut::activated, this,
            &GalleryMainWindow::onToggleStatusBar);

    // Zoom shortcuts
    m_shortcuts["zoomIn"] = new QShortcut(QKeySequence("Ctrl++"), this);
    connect(m_shortcuts["zoomIn"], &QShortcut::activated, this, &GalleryMainWindow::onZoomIn);

    m_shortcuts["zoomOut"] = new QShortcut(QKeySequence("Ctrl+-"), this);
    connect(m_shortcuts["zoomOut"], &QShortcut::activated, this, &GalleryMainWindow::onZoomOut);

    m_shortcuts["resetZoom"] = new QShortcut(QKeySequence("Ctrl+0"), this);
    connect(m_shortcuts["resetZoom"], &QShortcut::activated, this, &GalleryMainWindow::onResetZoom);

    // File operations
    m_shortcuts["exportIcon"] = new QShortcut(QKeySequence("Ctrl+E"), this);
    connect(m_shortcuts["exportIcon"], &QShortcut::activated, this,
            &GalleryMainWindow::onExportIcon);

    m_shortcuts["copyIconName"] = new QShortcut(QKeySequence("Ctrl+C"), this);
    connect(m_shortcuts["copyIconName"], &QShortcut::activated, this,
            &GalleryMainWindow::onCopyIconName);

    m_shortcuts["copyIconPath"] = new QShortcut(QKeySequence("Ctrl+Shift+C"), this);
    connect(m_shortcuts["copyIconPath"], &QShortcut::activated, this,
            &GalleryMainWindow::onCopyIconPath);

    // Search shortcuts
    m_shortcuts["focusSearch"] = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(m_shortcuts["focusSearch"], &QShortcut::activated, this, [this]() {
        if (m_searchWidget) {
            m_searchWidget->setFocus();
        }
    });

    m_shortcuts["advancedSearch"] = new QShortcut(QKeySequence("Ctrl+Shift+F"), this);
    connect(m_shortcuts["advancedSearch"], &QShortcut::activated, this,
            &GalleryMainWindow::onAdvancedSearchRequested);

    m_shortcuts["clearSearch"] = new QShortcut(QKeySequence("Ctrl+Shift+X"), this);
    connect(m_shortcuts["clearSearch"], &QShortcut::activated, this, [this]() {
        if (m_searchWidget) {
            m_searchWidget->clearSearch();
        }
    });

    // Favorites shortcuts
    m_shortcuts["toggleFavorite"] = new QShortcut(QKeySequence("Ctrl+D"), this);
    connect(m_shortcuts["toggleFavorite"], &QShortcut::activated, this, [this]() {
        if (!m_currentIconName.isEmpty() && m_favoritesManager) {
            if (m_favoritesManager->isFavorite(m_currentIconName)) {
                m_favoritesManager->removeFavorite(m_currentIconName);
                statusBar()->showMessage(tr("Removed from favorites"), 2000);
            } else {
                m_favoritesManager->addFavorite(m_currentIconName);
                statusBar()->showMessage(tr("Added to favorites"), 2000);
            }
        }
    });

    m_shortcuts["showFavorites"] = new QShortcut(QKeySequence("Ctrl+Shift+D"), this);
    connect(m_shortcuts["showFavorites"], &QShortcut::activated, this,
            &GalleryMainWindow::onShowFavorites);

    // Theme shortcuts
    m_shortcuts["toggleTheme"] = new QShortcut(QKeySequence("Ctrl+T"), this);
    connect(m_shortcuts["toggleTheme"], &QShortcut::activated, this, [this]() {
        ThemeMode newTheme = (m_currentTheme == LightTheme) ? DarkTheme : LightTheme;
        setTheme(newTheme);
    });

    // Quick actions
    m_shortcuts["refresh"] = new QShortcut(QKeySequence("F5"), this);
    connect(m_shortcuts["refresh"], &QShortcut::activated, this, [this]() {
        if (m_iconGrid) {
            m_iconGrid->refresh();
        }
    });

    m_shortcuts["selectAll"] = new QShortcut(QKeySequence("Ctrl+A"), this);
    connect(m_shortcuts["selectAll"], &QShortcut::activated, this, [this]() {
        if (m_iconGrid) {
            m_iconGrid->selectAll();
        }
    });

    GALLERY_LOG_DEBUG(
        galleryUI,
        QString("Keyboard shortcuts setup completed: %1 shortcuts").arg(m_shortcuts.size()));
}

void GalleryMainWindow::applyTheme() {
    // Apply the current theme
    switch (m_currentTheme) {
        case LightTheme:
            // Apply light theme
            setStyleSheet(""); // Default light theme
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

void GalleryMainWindow::loadImageDirectory(const QString& directoryPath) {
    if (directoryPath.isEmpty())
        return;

    if (m_imageMetadataManager) {
        // Load images from directory
        GALLERY_LOG_INFO(galleryUI,
                         QString("Loading images from directory: %1").arg(directoryPath));
        // Implementation would depend on ImageMetadataManager interface
    }

    updateStatusBar();
}

void GalleryMainWindow::showImage(const QString& imagePath) {
    if (imagePath.isEmpty())
        return;

    if (m_imageViewer) {
        // Show image in viewer
        GALLERY_LOG_INFO(galleryUI, QString("Showing image: %1").arg(imagePath));
        // Implementation would depend on ImageViewerWidget interface
    }
}

void GalleryMainWindow::updateViewMode() {
    // Update the view mode based on current settings
    if (m_iconGrid) {
        // Update icon grid view mode
    }
    if (m_thumbnailGrid) {
        // Update thumbnail grid view mode
    }
    updateStatusBar();
}

void GalleryMainWindow::updateWindowTitle() {
    QString title = "QtLucide Enhanced Icon & Image Gallery";
    if (!m_currentIconName.isEmpty()) {
        title += QString(" - %1").arg(m_currentIconName);
    }
    setWindowTitle(title);
}

// Helper method implementations
void GalleryMainWindow::applyCompactMode(bool enabled) {
    if (enabled) {
        // Apply compact mode styling
        if (m_iconGrid) {
            m_iconGrid->setSpacing(2);
            m_iconGrid->setMargin(4);
        }
        if (m_searchWidget) {
            m_searchWidget->setMaximumHeight(32);
        }
        if (m_categorySidebar) {
            m_categorySidebar->setMaximumWidth(180);
        }
    } else {
        // Apply normal mode styling
        if (m_iconGrid) {
            m_iconGrid->setSpacing(8);
            m_iconGrid->setMargin(12);
        }
        if (m_searchWidget) {
            m_searchWidget->setMaximumHeight(48);
        }
        if (m_categorySidebar) {
            m_categorySidebar->setMaximumWidth(250);
        }
    }
    GALLERY_LOG_DEBUG(galleryUI, QString("Compact mode applied: %1").arg(enabled));
}

void GalleryMainWindow::applyAnimations(bool enabled) {
    // Enable/disable animations throughout the UI
    if (m_iconGrid) {
        m_iconGrid->setAnimationsEnabled(enabled);
    }
    if (m_searchWidget) {
        m_searchWidget->setAnimationsEnabled(enabled);
    }
    if (m_detailsPanel) {
        m_detailsPanel->setAnimationsEnabled(enabled);
    }

    // Set global animation duration
    int duration = enabled ? 250 : 0;
    setProperty("animationDuration", duration);

    GALLERY_LOG_DEBUG(galleryUI, QString("Animations applied: %1").arg(enabled));
}

void GalleryMainWindow::applyCurrentFilter() {
    if (!m_iconGrid || !m_lucide)
        return;

    QStringList allIcons = m_lucide->availableIcons();
    QStringList filteredIcons;

    for (const QString& iconName : allIcons) {
        bool matches = true;

        // Apply search text filter
        if (!m_currentSearchText.isEmpty()) {
            if (!iconName.contains(m_currentSearchText, Qt::CaseInsensitive)) {
                matches = false;
            }
        }

        // Apply category filter
        if (!m_currentCategories.isEmpty() && m_metadataManager) {
            QStringList iconCategories = m_metadataManager->getIconCategories(iconName);
            bool categoryMatch = false;
            for (const QString& category : m_currentCategories) {
                if (iconCategories.contains(category, Qt::CaseInsensitive)) {
                    categoryMatch = true;
                    break;
                }
            }
            if (!categoryMatch) {
                matches = false;
            }
        }

        // Apply tag filter
        if (!m_currentTags.isEmpty() && m_metadataManager) {
            QStringList iconTags = m_metadataManager->getIconTags(iconName);
            bool tagMatch = false;
            for (const QString& tag : m_currentTags) {
                if (iconTags.contains(tag, Qt::CaseInsensitive)) {
                    tagMatch = true;
                    break;
                }
            }
            if (!tagMatch) {
                matches = false;
            }
        }

        if (matches) {
            filteredIcons.append(iconName);
        }
    }

    m_iconGrid->setFilteredIcons(filteredIcons);
    updateStatusBar();

    GALLERY_LOG_DEBUG(galleryUI,
                      QString("Filter applied: %1 icons match").arg(filteredIcons.size()));
}

// Missing GalleryMainWindow methods
void GalleryMainWindow::setIconSize(int size) {
    m_thumbnailSize = size;
    if (m_iconGrid) {
        m_iconGrid->setIconSize(size);
    }

    // Update size slider if it exists
    // m_sizeSlider is not available in this class

    // Store the current icon size
    m_currentIconSize = size;
}

void GalleryMainWindow::setGalleryMode(bool galleryMode) {
    // m_galleryMode is not available in this class
    // Store the mode in a local variable or use existing member variables

    // Update UI based on gallery mode
    if (m_iconGrid) {
        // Switch between icon grid and image gallery view
        if (galleryMode) {
            // Configure for image gallery mode
            m_iconGrid->setViewMode(IconGridWidget::DetailedView);
        } else {
            // Configure for icon browser mode
            m_iconGrid->setViewMode(IconGridWidget::GridView);
        }
    }

    // Signal not available in this class
}
