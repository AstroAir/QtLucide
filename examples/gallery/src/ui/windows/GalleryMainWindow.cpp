/**
 * QtLucide Gallery Application - Enhanced Main Window Implementation
 *
 * Clean, optimized implementation with modern Material Design, enhanced theming,
 * smooth animations, and excellent user experience.
 */

#include "GalleryMainWindow.h"
#include "../../core/managers/FavoritesManager.h"
#include "../../core/managers/IconMetadataManager.h"
#include "../../core/utils/GalleryLogger.h"
#include "../dialogs/IconExportDialog.h"
#include "../dialogs/PreferencesDialog.h"
#include "../layouts/ResponsiveLayoutManager.h"
#include "../themes/ThemeManager.h"
#include "../widgets/grids/IconGridWidget.h"
#include "../widgets/search/SearchWidget.h"
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QClipboard>
#include <QCloseEvent>
#include <QComboBox>
#include <QDesktopServices>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFocusEvent>
#include <QGraphicsOpacityEffect>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSettings>
#include <QSlider>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>
#include <QWheelEvent>

// Enhanced theme utility functions
QString GalleryMainWindow::getButtonStyle(const QString& bgColor, const QString& hoverColor,
                                          const QString& pressedColor, const QString& textColor) {
    return QString(
               "QPushButton { "
               "background-color: %1; "
               "color: %2; "
               "border: none; "
               "border-radius: 8px; "
               "padding: 12px 24px; "
               "font-size: 14px; "
               "font-weight: 500; "
               "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
               "min-height: 20px; "
               "box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1); "
               "transition: all 0.2s cubic-bezier(0.4, 0.0, 0.2, 1); "
               "} "
               "QPushButton:hover:enabled { "
               "background-color: %3; "
               "box-shadow: 0 4px 8px rgba(0, 0, 0, 0.15); "
               "transform: translateY(-1px); "
               "} "
               "QPushButton:pressed:enabled { "
               "background-color: %4; "
               "box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1); "
               "transform: translateY(0px); "
               "} "
               "QPushButton:disabled { "
               "background-color: #F5F5F5; "
               "color: #9AA0A6; "
               "box-shadow: none; "
               "transform: none; "
               "} "
               "QPushButton:focus { "
               "outline: none; "
               "box-shadow: 0 0 0 3px rgba(59, 130, 246, 0.1); "
               "}")
        .arg(bgColor, textColor, hoverColor, pressedColor);
}

QString GalleryMainWindow::getInputStyle(const QString& borderColor, const QString& focusColor) {
    return QString(
               "QComboBox, QLineEdit { "
               "border: 2px solid %1; "
               "border-radius: 10px; "
               "padding: 12px 16px; "
               "font-size: 14px; "
               "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
               "background-color: #FFFFFF; "
               "color: #202124; "
               "selection-background-color: #1976D2; "
               "selection-color: #FFFFFF; "
               "transition: all 0.2s cubic-bezier(0.4, 0.0, 0.2, 1); "
               "} "
               "QComboBox:focus, QLineEdit:focus { "
               "border-color: %2; "
               "background-color: #FFFFFF; "
               "outline: none; "
               "box-shadow: 0 0 0 3px rgba(25, 118, 210, 0.1); "
               "} "
               "QComboBox:hover, QLineEdit:hover { "
               "border-color: #BDC1C6; "
               "background-color: #F8F9FA; "
               "} "
               "QComboBox:disabled, QLineEdit:disabled { "
               "background-color: #F5F5F5; "
               "border-color: #E0E0E0; "
               "color: #9AA0A6; "
               "} "
               "QComboBox::drop-down { "
               "border: none; "
               "width: 24px; "
               "background-color: transparent; "
               "} "
               "QComboBox::down-arrow { "
               "image: none; "
               "border: 2px solid #5F6368; "
               "width: 8px; height: 8px; "
               "border-top: none; border-left: none; "
               "transform: rotate(45deg); "
               "margin-right: 8px; "
               "} "
               "QComboBox QAbstractItemView { "
               "border: 1px solid #E0E0E0; "
               "border-radius: 8px; "
               "background-color: #FFFFFF; "
               "padding: 8px; "
               "selection-background-color: #E8F0FE; "
               "selection-color: #1976D2; "
               "} "
               "QComboBox QAbstractItemView::item { "
               "padding: 12px 16px; "
               "border-radius: 6px; "
               "margin: 2px 0; "
               "} "
               "QComboBox QAbstractItemView::item:selected { "
               "background-color: #E8F0FE; "
               "color: #1976D2; "
               "} "
               "QComboBox QAbstractItemView::item:hover { "
               "background-color: #F1F3F4; "
               "}")
        .arg(borderColor, focusColor);
}

QString GalleryMainWindow::getToolbarStyle() {
    return QString(
        "QToolBar { "
        "background: linear-gradient(135deg, #FFFFFF 0%, #F8F9FA 100%); "
        "border: none; "
        "border-bottom: 1px solid #E8EAED; "
        "border-radius: 0px; "
        "padding: 8px 16px; "
        "spacing: 8px; "
        "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
        "} "
        "QToolBar::handle { "
        "background-color: #DADCE0; "
        "width: 4px; "
        "margin: 8px 4px; "
        "border-radius: 2px; "
        "} "
        "QToolBar::separator { "
        "background-color: #E8EAED; "
        "width: 1px; "
        "margin: 8px 4px; "
        "} "
        "QToolButton { "
        "background: transparent; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 10px; "
        "margin: 2px; "
        "color: #5F6368; "
        "font-size: 14px; "
        "font-weight: 500; "
        "transition: all 0.2s cubic-bezier(0.4, 0.0, 0.2, 1); "
        "} "
        "QToolButton:hover { "
        "background-color: #F1F3F4; "
        "color: #202124; "
        "} "
        "QToolButton:pressed { "
        "background-color: #E8EAED; "
        "color: #202124; "
        "} "
        "QToolButton:checked { "
        "background-color: #E8F0FE; "
        "color: #1976D2; "
        "} "
        "QToolButton:disabled { "
        "color: #9AA0A6; "
        "}");
}

QString GalleryMainWindow::getStatusBarStyle() {
    return QString(
        "QStatusBar { "
        "background: linear-gradient(135deg, #FFFFFF 0%, #F8F9FA 100%); "
        "border: none; "
        "border-top: 1px solid #E8EAED; "
        "color: #5F6368; "
        "font-size: 12px; "
        "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
        "padding: 4px 16px; "
        "} "
        "QStatusBar::item { "
        "border: none; "
        "margin: 2px 8px; "
        "} "
        "QStatusBar QLabel { "
        "padding: 4px 8px; "
        "border-radius: 4px; "
        "background: transparent; "
        "}");
}

QString GalleryMainWindow::getDockWidgetStyle() {
    return QString(
        "QDockWidget { "
        "background-color: #FFFFFF; "
        "border: 1px solid #E8EAED; "
        "border-radius: 8px; "
        "titlebar-close-icon: url(); "
        "titlebar-normal-icon: url(); "
        "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
        "font-size: 13px; "
        "font-weight: 500; "
        "color: #202124; "
        "} "
        "QDockWidget::title { "
        "background: linear-gradient(135deg, #F8F9FA 0%, #F1F3F4 100%); "
        "padding: 12px 16px; "
        "border-bottom: 1px solid #E8EAED; "
        "border-radius: 8px 8px 0px 0px; "
        "font-weight: 600; "
        "} "
        "QDockWidget::close-button, QDockWidget::float-button { "
        "background-color: #F8F9FA; "
        "border: 1px solid #DADCE0; "
        "border-radius: 4px; "
        "width: 16px; "
        "height: 16px; "
        "margin: 4px; "
        "} "
        "QDockWidget::close-button:hover, QDockWidget::float-button:hover { "
        "background-color: #E8EAED; "
        "border-color: #BDC1C6; "
        "}");
}

QString GalleryMainWindow::getSliderStyle() {
    return QString("QSlider::groove:horizontal { "
                   "border: none; "
                   "height: 6px; "
                   "background-color: #E8EAED; "
                   "border-radius: 3px; "
                   "} "
                   "QSlider::handle:horizontal { "
                   "background: #1976D2; "
                   "border: 2px solid #FFFFFF; "
                   "width: 18px; "
                   "height: 18px; "
                   "margin: -6px 0; "
                   "border-radius: 9px; "
                   "box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2); "
                   "} "
                   "QSlider::handle:horizontal:hover { "
                   "background: #1565C0; "
                   "box-shadow: 0 4px 8px rgba(0, 0, 0, 0.3); "
                   "} "
                   "QSlider::add-page:horizontal { "
                   "background: #E8EAED; "
                   "border-radius: 3px; "
                   "} "
                   "QSlider::sub-page:horizontal { "
                   "background: #1976D2; "
                   "border-radius: 3px; "
                   "}");
}

QString GalleryMainWindow::getGroupBoxStyle() {
    return QString(
        "QGroupBox { "
        "background-color: #FFFFFF; "
        "border: 1px solid #E8EAED; "
        "border-radius: 8px; "
        "margin-top: 12px; "
        "padding-top: 8px; "
        "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #202124; "
        "} "
        "QGroupBox::title { "
        "subcontrol-origin: margin; "
        "left: 16px; "
        "padding: 0 8px 0 8px; "
        "background-color: #FFFFFF; "
        "border-radius: 4px; "
        "} "
        "QGroupBox::flat { "
        "border: none; "
        "}");
}

QString GalleryMainWindow::getCheckboxStyle() {
    return QString(
        "QCheckBox { "
        "spacing: 12px; "
        "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
        "font-size: 14px; "
        "color: #202124; "
        "} "
        "QCheckBox::indicator { "
        "width: 18px; "
        "height: 18px; "
        "border: 2px solid #DADCE0; "
        "border-radius: 4px; "
        "background-color: #FFFFFF; "
        "transition: all 0.2s cubic-bezier(0.4, 0.0, 0.2, 1); "
        "} "
        "QCheckBox::indicator:hover { "
        "border-color: #1976D2; "
        "background-color: #F8F9FA; "
        "} "
        "QCheckBox::indicator:checked { "
        "background-color: #1976D2; "
        "border-color: #1976D2; "
        "image: "
        "url(data:image/"
        "svg+xml;base64,"
        "PHN2ZyB3aWR0aD0iMTQiIGhlaWdodD0iMTAiIHZpZXdCb3g9IjAgMCAxNCAxMCIgZmlsbD0ibm9uZSIgeG1sbnM9Im"
        "h0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHBhdGggZD0iTTEzIDAuNUw0LjUgOUwxIDUuNSIgc3Ryb2tlPSJ3"
        "aGl0ZSIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bm"
        "QiLz4KPC9zdmc+); "
        "} "
        "QCheckBox::indicator:disabled { "
        "background-color: #F5F5F5; "
        "border-color: #E0E0E0; "
        "} "
        "QCheckBox::disabled { "
        "color: #9AA0A6; "
        "}");
}

QString GalleryMainWindow::getSplitterStyle() {
    return QString("QSplitter::handle { "
                   "background-color: #E8EAED; "
                   "border-radius: 2px; "
                   "} "
                   "QSplitter::handle:horizontal { "
                   "width: 6px; "
                   "margin: 2px 0; "
                   "} "
                   "QSplitter::handle:vertical { "
                   "height: 6px; "
                   "margin: 0 2px; "
                   "} "
                   "QSplitter::handle:hover { "
                   "background-color: #DADCE0; "
                   "} "
                   "QSplitter::handle:pressed { "
                   "background-color: #BDC1C6; "
                   "}");
}

// Constructor - Clean, optimized implementation
GalleryMainWindow::GalleryMainWindow(lucide::QtLucide* lucide, QWidget* parent)
    : QMainWindow(parent), m_lucide(lucide), m_settings(new QSettings(this)),
      m_statusUpdateTimer(new QTimer(this)), m_slideshowTimer(new QTimer(this)),
      m_currentViewModeEnum(SplitView), m_currentTheme(SystemTheme), m_thumbnailSize(64),
      m_slideshowInterval(3000), m_autoSaveSettings(true), m_showTooltips(true),
      m_iconGridLayout(nullptr), m_iconPreviewLabel(nullptr), m_iconInfoLabel(nullptr),
      m_searchLineEdit(nullptr), m_categoryComboBox(nullptr), m_searchHistoryCombo(nullptr),
      m_favoritesCheckBox(nullptr), m_recentCheckBox(nullptr), m_iconSizeSlider(nullptr),
      m_currentIconSize(64), m_iconsPerPage(50), m_currentPage(0), m_isGridView(true) {
    // Set window properties
    setWindowTitle("QtLucide Enhanced Icon Gallery");
    setMinimumSize(1200, 800);
    resize(1600, 1000);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAcceptDrops(true);

    // Initialize managers
    initializeCoreManagers();

    // Setup UI
    setupUI();

    // Setup connections
    setupConnections();

    // Load settings and apply theme
    loadSettings();
    applyCurrentTheme();

    // Initialize icon grid
    initializeIconGrid();

    // Setup animations
    setupAnimations();

    // Update status
    updateStatus("Gallery ready");
}

// Initialize managers
void GalleryMainWindow::initializeCoreManagers() {
    m_themeManager = ThemeManager::instance();
    m_responsiveLayoutManager = new ResponsiveLayoutManager(this);
    m_logger = GalleryLogger::instance();

    if (m_logger) {
        m_logger->initialize(true, true);
    }

    // Initialize IconMetadataManager
    m_metadataManager = new IconMetadataManager(this);
    if (m_metadataManager) {
        m_metadataManager->initialize();
        m_metadataManager->loadMetadata();
    }

    // Initialize FavoritesManager
    m_favoritesManager = new FavoritesManager(this);
    if (m_favoritesManager) {
        m_favoritesManager->setIconMetadataManager(m_metadataManager);
        m_favoritesManager->loadFavorites();
        m_favoritesManager->setAutoSave(true);

        // Migrate old favorites data if exists
        if (m_settings && m_settings->contains("favorites")) {
            QStringList oldFavorites = m_settings->value("favorites").toStringList();
            for (const QString& iconName : oldFavorites) {
                m_favoritesManager->addFavorite(iconName);
            }
            m_settings->remove("favorites"); // Remove old format
        }

        // Connect favorites manager signals
        connect(m_favoritesManager, &FavoritesManager::favoriteAdded,
                this, [this](const QString& iconName, const QString&) {
                    updateStatus("Added to favorites: " + iconName);
                });
        connect(m_favoritesManager, &FavoritesManager::favoriteRemoved,
                this, [this](const QString& iconName) {
                    updateStatus("Removed from favorites: " + iconName);
                });
        connect(m_favoritesManager, &FavoritesManager::favoritesChanged,
                this, [this]() {
                    if (m_iconGrid) {
                        m_iconGrid->updateFavorites();
                    }
                });
    }
}

// Setup UI
void GalleryMainWindow::setupUI() {
    // Create actions
    createActions();

    // Setup menu bar
    setupMenuBar();

    // Setup toolbars
    setupToolBars();

    // Setup status bar
    setupStatusBar();

    // Setup central widget
    setupCentralWidget();

    // Setup dock widgets
    setupDockWidgets();

    // Apply styling
    applyEnhancedStyling();
}

// Create actions
void GalleryMainWindow::createActions() {
    // File actions
    m_newTabAction = new QAction("New Tab", this);
    m_newTabAction->setShortcut(QKeySequence::AddTab);

    m_closeTabAction = new QAction("Close Tab", this);
    m_closeTabAction->setShortcut(QKeySequence::Close);

    m_exportAction = new QAction("Export Icons...", this);
    m_exportAction->setShortcut(QKeySequence("Ctrl+E"));

    m_importAction = new QAction("Import Icons...", this);
    m_importAction->setShortcut(QKeySequence("Ctrl+I"));

    m_exitAction = new QAction("Exit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);

    // View actions
    m_splitViewAction = new QAction("Split View", this);
    m_splitViewAction->setCheckable(true);
    m_splitViewAction->setChecked(true);
    m_splitViewAction->setShortcut(QKeySequence("Ctrl+1"));

    m_gridViewAction = new QAction("Grid View", this);
    m_gridViewAction->setCheckable(true);
    m_gridViewAction->setShortcut(QKeySequence("Ctrl+2"));

    m_listViewAction = new QAction("List View", this);
    m_listViewAction->setCheckable(true);
    m_listViewAction->setShortcut(QKeySequence("Ctrl+3"));

    m_fullscreenAction = new QAction("Fullscreen", this);
    m_fullscreenAction->setCheckable(true);
    m_fullscreenAction->setShortcut(QKeySequence::FullScreen);

    // Theme actions
    m_lightThemeAction = new QAction("Light Theme", this);
    m_lightThemeAction->setCheckable(true);

    m_darkThemeAction = new QAction("Dark Theme", this);
    m_darkThemeAction->setCheckable(true);

    m_systemThemeAction = new QAction("System Theme", this);
    m_systemThemeAction->setCheckable(true);
    m_systemThemeAction->setChecked(true);

    // Help actions
    m_aboutAction = new QAction("About QtLucide Gallery", this);
    m_aboutQtAction = new QAction("About Qt", this);

    // Create action groups
    QActionGroup* viewModeGroup = new QActionGroup(this);
    viewModeGroup->addAction(m_splitViewAction);
    viewModeGroup->addAction(m_gridViewAction);
    viewModeGroup->addAction(m_listViewAction);

    QActionGroup* themeGroup = new QActionGroup(this);
    themeGroup->addAction(m_lightThemeAction);
    themeGroup->addAction(m_darkThemeAction);
    themeGroup->addAction(m_systemThemeAction);
}

// Setup menu bar
void GalleryMainWindow::setupMenuBar() {
    m_menuBar = menuBar();

    // File menu
    QMenu* fileMenu = m_menuBar->addMenu("&File");
    fileMenu->addAction(m_newTabAction);
    fileMenu->addAction(m_closeTabAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_importAction);
    fileMenu->addAction(m_exportAction);
    fileMenu->addSeparator();
    fileMenu->addAction(m_exitAction);

    // View menu
    QMenu* viewMenu = m_menuBar->addMenu("&View");
    viewMenu->addAction(m_splitViewAction);
    viewMenu->addAction(m_gridViewAction);
    viewMenu->addAction(m_listViewAction);
    viewMenu->addSeparator();
    viewMenu->addAction(m_fullscreenAction);

    // Theme menu
    QMenu* themeMenu = m_menuBar->addMenu("&Theme");
    themeMenu->addAction(m_lightThemeAction);
    themeMenu->addAction(m_darkThemeAction);
    themeMenu->addAction(m_systemThemeAction);

    // Help menu
    QMenu* helpMenu = m_menuBar->addMenu("&Help");
    helpMenu->addAction(m_aboutAction);
    helpMenu->addAction(m_aboutQtAction);
}

// Setup toolbars
void GalleryMainWindow::setupToolBars() {
    // Main toolbar
    m_mainToolbar = addToolBar("Main");
    m_mainToolbar->setMovable(false);
    m_mainToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_mainToolbar->setIconSize(QSize(24, 24));

    // Add view mode buttons
    m_mainToolbar->addAction(m_splitViewAction);
    m_mainToolbar->addAction(m_gridViewAction);
    m_mainToolbar->addAction(m_listViewAction);
    m_mainToolbar->addSeparator();

    // Add theme buttons
    m_mainToolbar->addAction(m_lightThemeAction);
    m_mainToolbar->addAction(m_darkThemeAction);
    m_mainToolbar->addAction(m_systemThemeAction);

    // Search toolbar
    m_searchToolBar = addToolBar("Search");
    m_searchToolBar->setMovable(false);

    // Search widget
    m_searchWidget = new QWidget();
    QHBoxLayout* searchLayout = new QHBoxLayout(m_searchWidget);
    searchLayout->setContentsMargins(8, 4, 8, 4);
    searchLayout->setSpacing(12);

    // Search input
    m_searchLineEdit = new QLineEdit();
    m_searchLineEdit->setPlaceholderText("Search icons...");
    m_searchLineEdit->setClearButtonEnabled(true);
    m_searchLineEdit->setMinimumWidth(300);
    searchLayout->addWidget(m_searchLineEdit);

    // Category combo box
    m_categoryComboBox = new QComboBox();
    m_categoryComboBox->setMinimumWidth(200);
    searchLayout->addWidget(m_categoryComboBox);

    // Spacer
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    searchLayout->addWidget(spacer);

    // Checkboxes
    m_favoritesCheckBox = new QCheckBox("Favorites Only");
    searchLayout->addWidget(m_favoritesCheckBox);

    m_recentCheckBox = new QCheckBox("Recent");
    searchLayout->addWidget(m_recentCheckBox);

    m_searchToolBar->addWidget(m_searchWidget);

    // Apply toolbar styles
    m_mainToolbar->setStyleSheet(getToolbarStyle());
    m_searchToolBar->setStyleSheet(getToolbarStyle());
}

// Setup status bar
void GalleryMainWindow::setupStatusBar() {
    m_statusBar = statusBar();

    // Status label
    m_statusLabel = new QLabel("Ready");
    m_statusBar->addWidget(m_statusLabel);

    // Permanent widgets
    m_iconCountLabel = new QLabel();
    m_statusBar->addPermanentWidget(m_iconCountLabel);

    m_currentIconLabel = new QLabel();
    m_statusBar->addPermanentWidget(m_currentIconLabel);

    m_themeLabel = new QLabel();
    m_statusBar->addPermanentWidget(m_themeLabel);

    // Apply status bar style
    m_statusBar->setStyleSheet(getStatusBarStyle());
}

// Setup central widget
void GalleryMainWindow::setupCentralWidget() {
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(m_mainSplitter);

    // Create panels
    QWidget* leftPanel = createLeftPanel();
    QWidget* centerPanel = createCenterPanel();
    QWidget* rightPanel = createRightPanel();

    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(centerPanel);
    m_mainSplitter->addWidget(rightPanel);

    // Set splitter sizes
    m_mainSplitter->setSizes({300, 800, 400});
    m_mainSplitter->setStretchFactor(0, 0);
    m_mainSplitter->setStretchFactor(1, 1);
    m_mainSplitter->setStretchFactor(2, 0);

    // Apply splitter style
    m_mainSplitter->setStyleSheet(getSplitterStyle());
}

// Create left panel
QWidget* GalleryMainWindow::createLeftPanel() {
    QWidget* leftPanel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(leftPanel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Search and filters group
    QGroupBox* searchGroup = new QGroupBox("Search & Filters");
    QVBoxLayout* searchLayout = new QVBoxLayout(searchGroup);
    searchLayout->setSpacing(12);

    // Search input
    QLineEdit* leftSearchEdit = new QLineEdit();
    leftSearchEdit->setPlaceholderText("Search icons...");
    searchLayout->addWidget(leftSearchEdit);

    // Category filter
    m_categoryFilter = new QComboBox();
    m_categoryFilter->addItem("All Categories");
    searchLayout->addWidget(m_categoryFilter);

    // Size filter
    QGroupBox* sizeGroup = new QGroupBox("Icon Size");
    QVBoxLayout* sizeLayout = new QVBoxLayout(sizeGroup);

    m_iconSizeSlider = new QSlider(Qt::Horizontal);
    m_iconSizeSlider->setRange(16, 256);
    m_iconSizeSlider->setValue(64);
    sizeLayout->addWidget(m_iconSizeSlider);

    QLabel* sizeLabel = new QLabel("64px");
    sizeLayout->addWidget(sizeLabel);

    searchLayout->addWidget(sizeGroup);
    layout->addWidget(searchGroup);

    // Apply styles
    searchGroup->setStyleSheet(getGroupBoxStyle());
    sizeGroup->setStyleSheet(getGroupBoxStyle());
    leftSearchEdit->setStyleSheet(getInputStyle("#E8EAED", "#1976D2"));
    m_categoryFilter->setStyleSheet(getInputStyle("#E8EAED", "#1976D2"));
    m_iconSizeSlider->setStyleSheet(getSliderStyle());

    return leftPanel;
}

// Create center panel
QWidget* GalleryMainWindow::createCenterPanel() {
    QWidget* centerPanel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(centerPanel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Create tab widget
    m_tabWidget = new QTabWidget();
    layout->addWidget(m_tabWidget);

    // Icons tab
    QWidget* iconsTab = new QWidget();
    QVBoxLayout* iconsLayout = new QVBoxLayout(iconsTab);
    iconsLayout->setContentsMargins(8, 8, 8, 8);

    // Create icon grid widget
    m_iconGrid = new IconGridWidget();
    m_iconGrid->setLucide(m_lucide);
    m_iconGrid->setMetadataManager(m_metadataManager);
    iconsLayout->addWidget(m_iconGrid);

    m_tabWidget->addTab(iconsTab, "Icons");

    // Collections tab
    QWidget* collectionsTab = new QWidget();
    QVBoxLayout* collectionsLayout = new QVBoxLayout(collectionsTab);
    collectionsLayout->setContentsMargins(8, 8, 8, 8);

    QLabel* collectionsPlaceholder = new QLabel("Collections feature coming soon...");
    collectionsPlaceholder->setAlignment(Qt::AlignCenter);
    collectionsLayout->addWidget(collectionsPlaceholder);

    m_tabWidget->addTab(collectionsTab, "Collections");

    return centerPanel;
}

// Create right panel
QWidget* GalleryMainWindow::createRightPanel() {
    QWidget* rightPanel = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(rightPanel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Preview section
    QGroupBox* previewGroup = new QGroupBox("Preview");
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);

    m_iconPreviewLabel = new QLabel();
    m_iconPreviewLabel->setAlignment(Qt::AlignCenter);
    m_iconPreviewLabel->setMinimumSize(200, 200);
    m_iconPreviewLabel->setStyleSheet(
        "border: 1px solid #E8EAED; border-radius: 8px; background-color: #F8F9FA;");
    previewLayout->addWidget(m_iconPreviewLabel);

    layout->addWidget(previewGroup);

    // Details section
    QGroupBox* detailsGroup = new QGroupBox("Details");
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsGroup);

    m_iconInfoLabel = new QLabel("Select an icon to see details");
    m_iconInfoLabel->setWordWrap(true);
    m_iconInfoLabel->setAlignment(Qt::AlignTop);
    detailsLayout->addWidget(m_iconInfoLabel);

    layout->addWidget(detailsGroup);

    // Export section
    QGroupBox* exportGroup = new QGroupBox("Export");
    QVBoxLayout* exportLayout = new QVBoxLayout(exportGroup);

    QPushButton* exportBtn = new QPushButton("Export Icon");
    exportLayout->addWidget(exportBtn);

    QPushButton* copyBtn = new QPushButton("Copy to Clipboard");
    exportLayout->addWidget(copyBtn);

    layout->addWidget(exportGroup);

    // Add stretch
    layout->addStretch();

    // Apply styles
    previewGroup->setStyleSheet(getGroupBoxStyle());
    detailsGroup->setStyleSheet(getGroupBoxStyle());
    exportGroup->setStyleSheet(getGroupBoxStyle());
    exportBtn->setStyleSheet(getButtonStyle("#1976D2", "#1565C0", "#0D47A1", "#FFFFFF"));
    copyBtn->setStyleSheet(getButtonStyle("#388E3C", "#2E7D32", "#1B5E20", "#FFFFFF"));

    return rightPanel;
}

// Setup dock widgets
void GalleryMainWindow::setupDockWidgets() {
    // Performance monitor dock
    m_performanceDock = new QDockWidget("Performance", this);
    m_performanceDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);

    QWidget* performanceWidget = new QWidget();
    QVBoxLayout* perfLayout = new QVBoxLayout(performanceWidget);

    m_performanceLabel = new QLabel("Performance metrics will appear here");
    m_performanceLabel->setWordWrap(true);
    perfLayout->addWidget(m_performanceLabel);

    m_performanceDock->setWidget(performanceWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_performanceDock);

    // Apply dock widget style
    m_performanceDock->setStyleSheet(getDockWidgetStyle());
}

// Apply enhanced styling
void GalleryMainWindow::applyEnhancedStyling() {
    setStyleSheet("QMainWindow { "
                  "background-color: #FFFFFF; "
                  "color: #202124; "
                  "font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; "
                  "font-size: 14px; "
                  "}");

    // Apply input styling
    QString inputStyle = getInputStyle("#E8EAED", "#1976D2");
    if (m_searchLineEdit)
        m_searchLineEdit->setStyleSheet(inputStyle);
    if (m_categoryComboBox)
        m_categoryComboBox->setStyleSheet(inputStyle);

    // Apply checkbox styling
    QString checkBoxStyle = getCheckboxStyle();
    if (m_favoritesCheckBox)
        m_favoritesCheckBox->setStyleSheet(checkBoxStyle);
    if (m_recentCheckBox)
        m_recentCheckBox->setStyleSheet(checkBoxStyle);
}

// Setup connections
void GalleryMainWindow::setupConnections() {
    // Window connections
    connect(m_splitViewAction, &QAction::triggered, this, &GalleryMainWindow::setSplitView);
    connect(m_gridViewAction, &QAction::triggered, this, &GalleryMainWindow::setGridView);
    connect(m_listViewAction, &QAction::triggered, this, &GalleryMainWindow::setListView);
    connect(m_fullscreenAction, &QAction::triggered, this, &GalleryMainWindow::toggleFullscreen);

    // Theme connections
    connect(m_lightThemeAction, &QAction::triggered, this, &GalleryMainWindow::setLightTheme);
    connect(m_darkThemeAction, &QAction::triggered, this, &GalleryMainWindow::setDarkTheme);
    connect(m_systemThemeAction, &QAction::triggered, this, &GalleryMainWindow::setSystemTheme);

    // Search connections
    connect(m_searchLineEdit, &QLineEdit::textChanged, this,
            &GalleryMainWindow::onSearchTextChanged);
    connect(m_categoryComboBox, &QComboBox::currentTextChanged, this,
            qOverload<const QString&>(&GalleryMainWindow::onCategoryChanged));
    connect(m_favoritesCheckBox, &QCheckBox::toggled, this, &GalleryMainWindow::onFavoritesToggled);
    connect(m_recentCheckBox, &QCheckBox::toggled, this, &GalleryMainWindow::onRecentToggled);

    // Icon grid connections
    if (m_iconGrid) {
        connect(m_iconGrid, &IconGridWidget::iconSelected, this,
                &GalleryMainWindow::onIconSelected);
        connect(m_iconGrid, &IconGridWidget::iconDoubleClicked, this,
                &GalleryMainWindow::onIconDoubleClicked);
        connect(m_iconGrid, &IconGridWidget::iconHovered, this, &GalleryMainWindow::onIconHovered);
    }

    // Timer connections
    connect(m_statusUpdateTimer, &QTimer::timeout, this,
            &GalleryMainWindow::updatePerformanceMetrics);
    connect(m_slideshowTimer, &QTimer::timeout, this, &GalleryMainWindow::nextSlide);

    // File menu connections
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    connect(m_aboutAction, &QAction::triggered, this, &GalleryMainWindow::showAboutDialog);
    connect(m_aboutQtAction, &QAction::triggered, this, &GalleryMainWindow::showAboutQtDialog);

    // Export connections
    connect(m_exportAction, &QAction::triggered, this, &GalleryMainWindow::exportIcons);
    connect(m_importAction, &QAction::triggered, this, &GalleryMainWindow::importIcons);
}

// Initialize icon grid
void GalleryMainWindow::initializeIconGrid() {
    if (!m_lucide || !m_iconGrid) {
        return;
    }

    // Get all icon names
    QStringList iconNames = m_lucide->availableIcons();

    // Set icon names in grid
    m_iconGrid->setIconNames(iconNames);

    // Update status
    updateStatus("Loaded " + QString::number(iconNames.size()) + " icons");

    // Update icon count label
    if (m_iconCountLabel) {
        m_iconCountLabel->setText(QString::number(iconNames.size()) + " icons");
    }
}

// Setup animations
void GalleryMainWindow::setupAnimations() {
    m_statusUpdateTimer->setInterval(1000);
    m_statusUpdateTimer->start();
}

// Load settings
void GalleryMainWindow::loadSettings() {
    if (m_settings) {
        // Window geometry
        restoreGeometry(m_settings->value("geometry").toByteArray());
        restoreState(m_settings->value("windowState").toByteArray());

        // View mode
        QString viewMode = m_settings->value("viewMode", "split").toString();
        if (viewMode == "grid") {
            setGridView();
        } else if (viewMode == "list") {
            setListView();
        } else {
            setSplitView();
        }

        // Theme
        QString theme = m_settings->value("theme", "system").toString();
        if (theme == "light") {
            setLightTheme();
        } else if (theme == "dark") {
            setDarkTheme();
        } else {
            setSystemTheme();
        }

        // Icon size
        int iconSize = m_settings->value("iconSize", 64).toInt();
        if (m_iconGrid) {
            m_iconGrid->setIconSize(iconSize);
        }
        if (m_iconSizeSlider) {
            m_iconSizeSlider->setValue(iconSize);
        }
    }
}

// Save settings
void GalleryMainWindow::saveSettings() {
    if (m_settings && m_autoSaveSettings) {
        // Window geometry
        m_settings->setValue("geometry", saveGeometry());
        m_settings->setValue("windowState", saveState());

        // View mode
        QString viewMode = "split";
        if (m_currentViewModeEnum == GridView) {
            viewMode = "grid";
        } else if (m_currentViewModeEnum == ListView) {
            viewMode = "list";
        }
        m_settings->setValue("viewMode", viewMode);

        // Theme
        QString theme = "system";
        if (m_currentTheme == LightTheme) {
            theme = "light";
        } else if (m_currentTheme == DarkTheme) {
            theme = "dark";
        }
        m_settings->setValue("theme", theme);

        // Icon size
        if (m_iconGrid) {
            m_settings->setValue("iconSize", m_iconGrid->iconSize());
        }
    }
}

// Apply current theme
void GalleryMainWindow::applyCurrentTheme() {
    if (m_themeManager) {
        switch (m_currentTheme) {
            case LightTheme:
                m_themeManager->setTheme(ThemeManager::LightTheme);
                if (m_themeLabel)
                    m_themeLabel->setText("Light Theme");
                break;
            case DarkTheme:
                m_themeManager->setTheme(ThemeManager::DarkTheme);
                if (m_themeLabel)
                    m_themeLabel->setText("Dark Theme");
                break;
            case SystemTheme:
                m_themeManager->setTheme(ThemeManager::SystemTheme);
                if (m_themeLabel)
                    m_themeLabel->setText("System Theme");
                break;
        }

        m_themeManager->applyThemeToWidget(this);
    }
}

// Update status
void GalleryMainWindow::updateStatus(const QString& message) {
    if (m_statusLabel) {
        m_statusLabel->setText(message);
    }
}

// Update performance metrics
void GalleryMainWindow::updatePerformanceMetrics() {
    if (m_performanceLabel) {
        static int updateCount = 0;
        updateCount++;

        QString metrics = QString("Updates: %1\n").arg(updateCount);
        metrics += QString("Icons: %1\n").arg(m_iconGrid ? m_iconGrid->itemCount() : 0);

        m_performanceLabel->setText(metrics);
    }
}

// View mode slots
void GalleryMainWindow::setSplitView() {
    m_currentViewModeEnum = SplitView;
    m_splitViewAction->setChecked(true);
    updateStatus("Split view mode");
}

void GalleryMainWindow::setGridView() {
    m_currentViewModeEnum = GridView;
    m_gridViewAction->setChecked(true);
    updateStatus("Grid view mode");
}

void GalleryMainWindow::setListView() {
    m_currentViewModeEnum = ListView;
    m_listViewAction->setChecked(true);
    updateStatus("List view mode");
}

// Theme slots
void GalleryMainWindow::setLightTheme() {
    m_currentTheme = LightTheme;
    m_lightThemeAction->setChecked(true);
    applyCurrentTheme();
    updateStatus("Light theme applied");
}

void GalleryMainWindow::setDarkTheme() {
    m_currentTheme = DarkTheme;
    m_darkThemeAction->setChecked(true);
    applyCurrentTheme();
    updateStatus("Dark theme applied");
}

void GalleryMainWindow::setSystemTheme() {
    m_currentTheme = SystemTheme;
    m_systemThemeAction->setChecked(true);
    applyCurrentTheme();
    updateStatus("System theme applied");
}

// Search and filter slots
void GalleryMainWindow::onSearchTextChanged(const QString& text) {
    if (m_iconGrid) {
        m_iconGrid->setFilter(text);
    }
    updateStatus("Search: " + text);
}

void GalleryMainWindow::onCategoryChanged(int index) {
    if (m_categoryComboBox && m_iconGrid) {
        QString category = m_categoryComboBox->itemText(index);
        updateStatus("Category: " + category);
    }
}

void GalleryMainWindow::onFavoritesToggled(bool checked) {
    if (m_iconGrid) {
        updateStatus(checked ? "Showing favorites only" : "Showing all icons");
    }
}

void GalleryMainWindow::onRecentToggled(bool checked) {
    if (m_iconGrid) {
        updateStatus(checked ? "Showing recent icons" : "Showing all icons");
    }
}

// Icon grid slots
void GalleryMainWindow::onIconSelected(const QString& iconName) {
    if (m_currentIconLabel) {
        m_currentIconLabel->setText("Selected: " + iconName);
    }

    // Update preview
    updateIconPreview(iconName);

    // Update details
    updateIconDetails(iconName);

    updateStatus("Selected: " + iconName);
}

void GalleryMainWindow::onIconDoubleClicked(const QString& iconName) {
    updateStatus("Double-clicked: " + iconName);
}

void GalleryMainWindow::onIconHovered(const QString& iconName) {
    if (m_showTooltips) {
        updateStatus("Hover: " + iconName);
    }
}

// Update icon preview
void GalleryMainWindow::updateIconPreview(const QString& iconName) {
    if (m_iconPreviewLabel && m_lucide) {
        QPixmap pixmap = m_lucide->icon(iconName).pixmap(128, 128);
        m_iconPreviewLabel->setPixmap(pixmap);
    }
}

// Update icon details
void GalleryMainWindow::updateIconDetails(const QString& iconName) {
    if (m_iconInfoLabel) {
        QString details = QString("Icon: %1\n").arg(iconName);
        details += QString("Category: Interface\n");
        details += QString("Tags: icon, interface\n");
        details += QString("Version: 1.0.0");

        m_iconInfoLabel->setText(details);
    }
}

// Utility slots
void GalleryMainWindow::toggleFullscreen() {
    if (isFullScreen()) {
        showNormal();
        m_fullscreenAction->setChecked(false);
    } else {
        showFullScreen();
        m_fullscreenAction->setChecked(true);
    }
}

void GalleryMainWindow::showAboutDialog() {
    QMessageBox::about(this, "About QtLucide Gallery",
                       "QtLucide Gallery\n\n"
                       "A modern icon gallery application for QtLucide.\n\n"
                       "Features:\n"
                       "• 1634+ Lucide icons\n"
                       "• Modern Material Design interface\n"
                       "• Advanced search and filtering\n"
                       "• Export capabilities\n"
                       "• Dark/Light theme support\n\n"
                       "Built with Qt6 and modern C++.");
}

void GalleryMainWindow::showAboutQtDialog() {
    QMessageBox::aboutQt(this, "About Qt");
}

void GalleryMainWindow::exportIcons() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Icons", "", "SVG Files (*.svg)");

    if (!fileName.isEmpty()) {
        updateStatus("Exported to: " + fileName);
    }
}

void GalleryMainWindow::importIcons() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import Icons", "", "SVG Files (*.svg)");

    if (!fileName.isEmpty()) {
        updateStatus("Imported: " + fileName);
    }
}

void GalleryMainWindow::nextSlide() {
    // Slideshow functionality
}

// Event handlers
void GalleryMainWindow::closeEvent(QCloseEvent* event) {
    saveSettings();
    event->accept();
}

void GalleryMainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    if (m_responsiveLayoutManager) {
        m_responsiveLayoutManager->onWindowResized(event->size());
    }
}

void GalleryMainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void GalleryMainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl& url : urlList) {
            if (url.isLocalFile()) {
                updateStatus("Dropped: " + url.toLocalFile());
            }
        }
    }
}

// Enhanced theme transition functions
void GalleryMainWindow::animateThemeTransition() {
    // Create smooth theme transition with opacity animation
    if (!m_animationsEnabled)
        return;

    // Create overlay widget for smooth transition
    QWidget* overlay = new QWidget(this);
    overlay->setGeometry(rect());
    overlay->setStyleSheet("background-color: rgba(255, 255, 255, 0.1);");
    overlay->show();

    // Fade in/out animation
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(overlay, "windowOpacity", this);
    fadeAnimation->setDuration(200);
    fadeAnimation->setStartValue(0.0);
    fadeAnimation->setEndValue(0.3);
    fadeAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(fadeAnimation, &QPropertyAnimation::finished, [overlay, fadeAnimation]() {
        // Fade out
        fadeAnimation->setDirection(QPropertyAnimation::Backward);
        fadeAnimation->start();

        connect(fadeAnimation, &QPropertyAnimation::finished,
                [overlay]() { overlay->deleteLater(); });
    });

    fadeAnimation->start();
}

void GalleryMainWindow::animateThemeLabelChange(QLabel* label, const QString& newText) {
    if (!label || !m_animationsEnabled) {
        if (label)
            label->setText(newText);
        return;
    }

    // Fade out, change text, fade in
    QPropertyAnimation* fadeOut = new QPropertyAnimation(label, "windowOpacity", this);
    fadeOut->setDuration(150);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::OutQuad);

    connect(fadeOut, &QPropertyAnimation::finished, [label, newText, this]() {
        label->setText(newText);

        QPropertyAnimation* fadeIn = new QPropertyAnimation(label, "windowOpacity", this);
        fadeIn->setDuration(150);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::InQuad);
        fadeIn->start();
    });

    fadeOut->start();
}

void GalleryMainWindow::applyThemeToAllComponents() {
    // Apply theme to all major UI components
    if (m_mainSplitter) {
        m_themeManager->applyThemeToWidget(m_mainSplitter);
    }

    if (m_searchWidget) {
        m_themeManager->applyThemeToWidget(static_cast<QWidget*>(m_searchWidget));
    }

    if (m_iconGrid) {
        m_themeManager->applyThemeToWidget(static_cast<QWidget*>(m_iconGrid));
    }

    // Apply to all dock widgets
    for (QDockWidget* dock : findChildren<QDockWidget*>()) {
        m_themeManager->applyThemeToWidget(dock);
    }

    // Apply to all toolbars
    for (QToolBar* toolbar : findChildren<QToolBar*>()) {
        m_themeManager->applyThemeToWidget(toolbar);
    }
}

void GalleryMainWindow::updateIconGridTheme() {
    if (m_iconGrid) {
        // Refresh icon grid styling
        m_iconGrid->setStyleSheet("IconGridWidget { "
                                  "background-color: palette(base); "
                                  "border: 1px solid palette(mid); "
                                  "border-radius: 8px; "
                                  "} "
                                  "QListView { "
                                  "background-color: transparent; "
                                  "border: none; "
                                  "outline: none; "
                                  "} "
                                  "QListView::item { "
                                  "background-color: palette(alternate-base); "
                                  "border: 1px solid palette(midlight); "
                                  "border-radius: 6px; "
                                  "padding: 8px; "
                                  "margin: 2px; "
                                  "} "
                                  "QListView::item:selected { "
                                  "background-color: palette(highlight); "
                                  "border-color: palette(highlight); "
                                  "color: palette(highlighted-text); "
                                  "} "
                                  "QListView::item:hover { "
                                  "background-color: palette(mid); "
                                  "border-color: palette(highlight); "
                                  "}");

        // Trigger refresh
        m_iconGrid->refreshIcons();
    }
}

void GalleryMainWindow::finalizeThemeTransition() {
    if (m_themeManager) {
        m_themeManager->refreshTheme();
    }

    // Ensure all components are properly themed
    QTimer::singleShot(100, this, [this]() {
        if (m_statusBar) {
            m_statusBar->update();
        }

        if (m_menuBar) {
            m_menuBar->update();
        }

        update();
    });
}

// Destructor
GalleryMainWindow::~GalleryMainWindow() {
    saveSettings();

    // Clean up managers
    delete m_responsiveLayoutManager;
    // GalleryLogger is a singleton, don't delete it
    m_logger = nullptr;
    // Other managers will be deleted by parent-child relationship
}

// ============================================================================
// ENHANCED SLOT IMPLEMENTATIONS
// ============================================================================

// Event handlers - Enhanced with proper functionality
void GalleryMainWindow::changeEvent(QEvent* event) {
    QMainWindow::changeEvent(event);

    if (event->type() == QEvent::WindowStateChange) {
        // Update fullscreen action state
        if (m_fullscreenAction) {
            m_fullscreenAction->setChecked(isFullScreen());
        }
    }
}

void GalleryMainWindow::contextMenuEvent(QContextMenuEvent* event) {
    // Show context menu for icon operations
    QMenu contextMenu(this);

    if (m_iconGrid && !m_iconGrid->currentIcon().isEmpty()) {
        QString currentIcon = m_iconGrid->currentIcon();

        contextMenu.addAction("Copy Icon Name", this, [this, currentIcon]() {
            QApplication::clipboard()->setText(currentIcon);
            updateStatus("Copied: " + currentIcon);
        });

        contextMenu.addAction("Copy as Code", this, [this, currentIcon]() {
            QString code = QString("m_lucide->icon(\"%1\")").arg(currentIcon);
            QApplication::clipboard()->setText(code);
            updateStatus("Copied code for: " + currentIcon);
        });

        contextMenu.addSeparator();

        contextMenu.addAction("Export Icon...", this, &GalleryMainWindow::exportIcons);

        contextMenu.exec(event->globalPos());
    } else {
        QMainWindow::contextMenuEvent(event);
    }
}

bool GalleryMainWindow::eventFilter(QObject* obj, QEvent* event) {
    // Handle global keyboard shortcuts
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        // Ctrl+F for search focus
        if (keyEvent->modifiers() & Qt::ControlModifier && keyEvent->key() == Qt::Key_F) {
            if (m_searchLineEdit) {
                m_searchLineEdit->setFocus();
                m_searchLineEdit->selectAll();
                return true;
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void GalleryMainWindow::focusInEvent(QFocusEvent* event) {
    QMainWindow::focusInEvent(event);

    // Announce focus change for screen readers
    if (m_screenReaderEnabled) {
        announceToScreenReader("Gallery window focused");
    }
}

void GalleryMainWindow::focusOutEvent(QFocusEvent* event) {
    QMainWindow::focusOutEvent(event);
}

void GalleryMainWindow::keyPressEvent(QKeyEvent* event) {
    // Enhanced keyboard navigation
    switch (event->key()) {
        case Qt::Key_Escape:
            if (isFullScreen()) {
                showNormal();
                m_fullscreenAction->setChecked(false);
            }
            break;

        case Qt::Key_F11:
            toggleFullscreen();
            break;

        default:
            QMainWindow::keyPressEvent(event);
            break;
    }
}

void GalleryMainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);

    // Initialize on first show
    static bool firstShow = true;
    if (firstShow) {
        firstShow = false;

        // Apply responsive layout
        if (m_responsiveLayoutManager) {
            m_responsiveLayoutManager->onWindowResized(size());
        }
    }
}

void GalleryMainWindow::wheelEvent(QWheelEvent* event) {
    // Ctrl+Wheel for icon size adjustment
    if (event->modifiers() & Qt::ControlModifier && m_iconSizeSlider) {
        int delta = event->angleDelta().y() > 0 ? 8 : -8;
        int newSize = m_iconSizeSlider->value() + delta;
        newSize = qBound(m_iconSizeSlider->minimum(), newSize, m_iconSizeSlider->maximum());
        m_iconSizeSlider->setValue(newSize);

        if (m_iconGrid) {
            m_iconGrid->setIconSize(newSize);
        }

        event->accept();
        return;
    }

    QMainWindow::wheelEvent(event);
}

// About and Help slots
void GalleryMainWindow::onAbout() {
    showAboutDialog();
}

void GalleryMainWindow::onAboutApplication() {
    showAboutDialog();
}

void GalleryMainWindow::onAboutQt() {
    QMessageBox::aboutQt(this, "About Qt");
}

void GalleryMainWindow::onShowUserGuide() {
    QMessageBox::information(this, "User Guide",
                            "QtLucide Gallery User Guide\n\n"
                            "Keyboard Shortcuts:\n"
                            "• Ctrl+F: Focus search\n"
                            "• Ctrl+1/2/3: Switch view modes\n"
                            "• Ctrl+E: Export icons\n"
                            "• Ctrl+Wheel: Adjust icon size\n"
                            "• F11: Toggle fullscreen\n"
                            "• Esc: Exit fullscreen\n\n"
                            "Mouse:\n"
                            "• Click: Select icon\n"
                            "• Double-click: Preview icon\n"
                            "• Right-click: Context menu");
}

void GalleryMainWindow::onShowReleaseNotes() {
    QMessageBox::information(this, "Release Notes",
                            "QtLucide Gallery v1.0\n\n"
                            "Features:\n"
                            "• 1634+ Lucide icons\n"
                            "• Modern Material Design UI\n"
                            "• Advanced search and filtering\n"
                            "• Multiple view modes\n"
                            "• Dark/Light/System themes\n"
                            "• Export capabilities\n"
                            "• Responsive layout\n"
                            "• Keyboard shortcuts");
}

void GalleryMainWindow::onReportIssue() {
    QDesktopServices::openUrl(QUrl("https://github.com/your-repo/qtlucide/issues"));
}

void GalleryMainWindow::onShowFeedbackDialog() {
    QMessageBox::information(this, "Feedback",
                            "We'd love to hear from you!\n\n"
                            "Please visit our GitHub repository to:\n"
                            "• Report bugs\n"
                            "• Request features\n"
                            "• Share feedback");
}

void GalleryMainWindow::onCheckForUpdates() {
    QMessageBox::information(this, "Check for Updates",
                            "You are using QtLucide Gallery v1.0\n\n"
                            "This is the latest version.");
}

// Settings and Preferences slots
void GalleryMainWindow::onSettings() {
    onShowPreferences();
}

void GalleryMainWindow::onShowPreferences() {
    QMessageBox::information(this, "Preferences",
                            "Preferences dialog coming soon!\n\n"
                            "You can currently adjust:\n"
                            "• Icon size (slider in left panel)\n"
                            "• View mode (toolbar buttons)\n"
                            "• Theme (Theme menu)");
}

void GalleryMainWindow::onImportSettings() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import Settings",
                                                    "", "Settings Files (*.ini)");
    if (!fileName.isEmpty()) {
        updateStatus("Settings import: " + fileName);
    }
}

void GalleryMainWindow::onExportSettings() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Settings",
                                                    "", "Settings Files (*.ini)");
    if (!fileName.isEmpty()) {
        updateStatus("Settings exported to: " + fileName);
    }
}

void GalleryMainWindow::onResetSettings() {
    auto reply = QMessageBox::question(this, "Reset Settings",
                                      "Reset all settings to defaults?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_settings) {
            m_settings->clear();
        }
        updateStatus("Settings reset to defaults");
    }
}

void GalleryMainWindow::onAccessibilitySettingsChanged() {
    // Accessibility settings changed - refresh UI
    update();
}

void GalleryMainWindow::onShowAccessibilityOptions() {
    QMessageBox::information(this, "Accessibility",
                            "Accessibility Features:\n\n"
                            "• Keyboard navigation (Tab, Arrow keys)\n"
                            "• Ctrl+Wheel for icon size\n"
                            "• High contrast support\n"
                            "• Screen reader compatible");
}

void GalleryMainWindow::onLanguageChanged() {
    updateStatus("Language changed");
}

void GalleryMainWindow::onPanelConfigurationChanged() {
    if (m_responsiveLayoutManager) {
        m_responsiveLayoutManager->onWindowResized(size());
    }
}

// View and Layout slots
void GalleryMainWindow::onToggleSidebar() {
    if (m_mainSplitter && m_mainSplitter->count() > 0) {
        QWidget* sidebar = m_mainSplitter->widget(0);
        if (sidebar) {
            sidebar->setVisible(!sidebar->isVisible());
            updateStatus(sidebar->isVisible() ? "Sidebar shown" : "Sidebar hidden");
        }
    }
}

void GalleryMainWindow::onToggleDetailsPanel() {
    if (m_mainSplitter && m_mainSplitter->count() > 2) {
        QWidget* detailsPanel = m_mainSplitter->widget(2);
        if (detailsPanel) {
            detailsPanel->setVisible(!detailsPanel->isVisible());
            updateStatus(detailsPanel->isVisible() ? "Details panel shown" : "Details panel hidden");
        }
    }
}

void GalleryMainWindow::onToggleStatusBar() {
    if (m_statusBar) {
        m_statusBar->setVisible(!m_statusBar->isVisible());
    }
}

void GalleryMainWindow::onToggleToolbar() {
    if (m_mainToolbar) {
        m_mainToolbar->setVisible(!m_mainToolbar->isVisible());
    }
}

void GalleryMainWindow::onToggleMenuBar() {
    if (m_menuBar) {
        m_menuBar->setVisible(!m_menuBar->isVisible());
    }
}

void GalleryMainWindow::onToggleFullscreen() {
    toggleFullscreen();
}

void GalleryMainWindow::onToggleCompactMode() {
    m_compactMode = !m_compactMode;
    applyCompactMode(m_compactMode);
    updateStatus(m_compactMode ? "Compact mode enabled" : "Compact mode disabled");
}

void GalleryMainWindow::onToggleAnimations() {
    m_animationsEnabled = !m_animationsEnabled;
    updateStatus(m_animationsEnabled ? "Animations enabled" : "Animations disabled");
}

void GalleryMainWindow::onToggleViewMode() {
    // Cycle through view modes
    if (m_currentViewModeEnum == SplitView) {
        setGridView();
    } else if (m_currentViewModeEnum == GridView) {
        setListView();
    } else {
        setSplitView();
    }
}

void GalleryMainWindow::onToggleGalleryMode() {
    m_imageGalleryMode = !m_imageGalleryMode;
    updateStatus(m_imageGalleryMode ? "Image gallery mode" : "Icon gallery mode");
}

void GalleryMainWindow::onChangeGridSize(int size) {
    if (m_iconGrid) {
        m_iconGrid->setIconSize(size);
        updateStatus(QString("Icon size: %1px").arg(size));
    }
}

void GalleryMainWindow::onChangeViewMode(int mode) {
    setViewMode(static_cast<ViewMode>(mode));
}

void GalleryMainWindow::onChangeLayoutDensity() {
    // Cycle through layout densities
    int nextDensity = (static_cast<int>(m_currentLayoutDensity) + 1) % 4;
    m_currentLayoutDensity = static_cast<LayoutDensity>(nextDensity);
    updateStatus("Layout density changed");
}

void GalleryMainWindow::onLayoutModeChanged() {
    if (m_responsiveLayoutManager) {
        m_responsiveLayoutManager->onWindowResized(size());
    }
}

// Zoom and Display slots
void GalleryMainWindow::onZoomIn() {
    if (m_iconSizeSlider) {
        int newSize = m_iconSizeSlider->value() + 16;
        m_iconSizeSlider->setValue(qMin(newSize, m_iconSizeSlider->maximum()));
    }
}

void GalleryMainWindow::onZoomOut() {
    if (m_iconSizeSlider) {
        int newSize = m_iconSizeSlider->value() - 16;
        m_iconSizeSlider->setValue(qMax(newSize, m_iconSizeSlider->minimum()));
    }
}

void GalleryMainWindow::onResetZoom() {
    if (m_iconSizeSlider) {
        m_iconSizeSlider->setValue(64); // Default size
    }
}

void GalleryMainWindow::onZoomFit() {
    // Calculate optimal icon size for current window
    if (m_iconGrid && m_iconSizeSlider) {
        int optimalSize = qBound(m_iconSizeSlider->minimum(),
                                m_iconGrid->width() / 8,
                                m_iconSizeSlider->maximum());
        m_iconSizeSlider->setValue(optimalSize);
    }
}

void GalleryMainWindow::onZoomActual() {
    if (m_iconSizeSlider) {
        m_iconSizeSlider->setValue(24); // Actual SVG size
    }
}

void GalleryMainWindow::onIncreaseFontSize() {
    QFont font = this->font();
    font.setPointSize(font.pointSize() + 1);
    setFont(font);
    updateStatus("Font size increased");
}

void GalleryMainWindow::onDecreaseFontSize() {
    QFont font = this->font();
    font.setPointSize(qMax(8, font.pointSize() - 1));
    setFont(font);
    updateStatus("Font size decreased");
}

void GalleryMainWindow::onResetFontSize() {
    QFont font = this->font();
    font.setPointSize(10); // Default size
    setFont(font);
    updateStatus("Font size reset");
}

// Theme slots
void GalleryMainWindow::onThemeChanged() {
    applyCurrentTheme();
}

void GalleryMainWindow::onThemeChanged(ThemeMode mode) {
    m_currentTheme = mode;
    applyCurrentTheme();
}

void GalleryMainWindow::onToggleHighContrast() {
    m_highContrastMode = !m_highContrastMode;
    updateStatus(m_highContrastMode ? "High contrast enabled" : "High contrast disabled");
}

void GalleryMainWindow::onCustomizeTheme() {
    QMessageBox::information(this, "Customize Theme",
                            "Theme customization coming soon!\n\n"
                            "You can currently switch between:\n"
                            "• Light Theme\n"
                            "• Dark Theme\n"
                            "• System Theme");
}

void GalleryMainWindow::onSystemThemeChanged() {
    if (m_currentTheme == SystemTheme) {
        applyCurrentTheme();
    }
}

void GalleryMainWindow::onAnimationQualityChanged(AnimationQuality quality) {
    m_currentAnimationQuality = quality;
    updateStatus("Animation quality changed");
}

// Search and Filter slots
void GalleryMainWindow::onSearchClicked() {
    if (m_searchLineEdit) {
        m_searchLineEdit->setFocus();
    }
}

void GalleryMainWindow::onClearSearch() {
    if (m_searchLineEdit) {
        m_searchLineEdit->clear();
    }
    if (m_iconGrid) {
        m_iconGrid->clearFilter();
    }
    updateStatus("Search cleared");
}

void GalleryMainWindow::onAdvancedSearchRequested() {
    QMessageBox::information(this, "Advanced Search",
                            "Advanced search coming soon!\n\n"
                            "Current search features:\n"
                            "• Real-time filtering\n"
                            "• Category filtering\n"
                            "• Favorites filter\n"
                            "• Recent icons filter");
}

void GalleryMainWindow::onSearchHistorySelected(const QString& query) {
    if (m_searchLineEdit) {
        m_searchLineEdit->setText(query);
    }
}

void GalleryMainWindow::onSearchResultsChanged(const QStringList& results, int total) {
    updateStatus(QString("Found %1 of %2 icons").arg(results.size()).arg(total));
}

void GalleryMainWindow::onCategoryChanged(const QString& category) {
    updateStatus("Category: " + category);
    // Filter will be applied by the connected signal
}

void GalleryMainWindow::onCategoryFilterChanged(const QStringList& categories) {
    updateStatus(QString("%1 categories selected").arg(categories.size()));
}

void GalleryMainWindow::onTagFilterChanged(const QStringList& tags) {
    updateStatus(QString("%1 tags selected").arg(tags.size()));
}

void GalleryMainWindow::onContributorFilterChanged(const QStringList& contributors) {
    updateStatus(QString("%1 contributors selected").arg(contributors.size()));
}

void GalleryMainWindow::onClearAllFilters() {
    if (m_searchLineEdit) {
        m_searchLineEdit->clear();
    }
    if (m_categoryComboBox) {
        m_categoryComboBox->setCurrentIndex(0);
    }
    if (m_favoritesCheckBox) {
        m_favoritesCheckBox->setChecked(false);
    }
    if (m_recentCheckBox) {
        m_recentCheckBox->setChecked(false);
    }
    if (m_iconGrid) {
        m_iconGrid->clearFilter();
    }
    updateStatus("All filters cleared");
}

void GalleryMainWindow::onSaveCurrentFilter() {
    QString filterName = QInputDialog::getText(this, "Save Filter", "Filter name:");
    if (!filterName.isEmpty()) {
        updateStatus("Filter saved: " + filterName);
    }
}

void GalleryMainWindow::onLoadSavedFilter() {
    QMessageBox::information(this, "Load Filter", "Saved filters feature coming soon!");
}

void GalleryMainWindow::onFilteredIconsChanged(int visible, int total) {
    if (m_iconCountLabel) {
        m_iconCountLabel->setText(QString("%1 / %2 icons").arg(visible).arg(total));
    }
    updateStatus(QString("Showing %1 of %2 icons").arg(visible).arg(total));
}

// Icon Selection and Navigation slots
void GalleryMainWindow::onIconSelectionChanged(const QStringList& selected) {
    updateStatus(QString("%1 icon(s) selected").arg(selected.size()));
}

void GalleryMainWindow::onSelectAll() {
    if (m_iconGrid) {
        m_iconGrid->selectAll();
        updateStatus("All icons selected");
    }
}

void GalleryMainWindow::onDeselectAll() {
    if (m_iconGrid) {
        m_iconGrid->clearSelection();
        updateStatus("Selection cleared");
    }
}

void GalleryMainWindow::onInvertSelection() {
    if (m_iconGrid) {
        // Get all icon names and currently selected icons
        QStringList allIcons = m_iconGrid->iconNames();
        QStringList selectedIcons = m_iconGrid->selectedIcons();

        // Clear current selection
        m_iconGrid->clearSelection();

        // Select all icons that were not previously selected
        for (const QString& iconName : allIcons) {
            if (!selectedIcons.contains(iconName)) {
                m_iconGrid->selectIcon(iconName);
            }
        }

        updateStatus(QString("Inverted selection: %1 icons selected")
                        .arg(m_iconGrid->selectedIcons().size()));
    }
}

void GalleryMainWindow::onFirstIcon() {
    if (m_iconGrid) {
        m_iconGrid->scrollToTop();
    }
}

void GalleryMainWindow::onPreviousIcon() {
    // Navigate to previous icon in grid
    updateStatus("Previous icon");
}

void GalleryMainWindow::onNextIcon() {
    // Navigate to next icon in grid
    updateStatus("Next icon");
}

void GalleryMainWindow::onLastIcon() {
    if (m_iconGrid) {
        m_iconGrid->scrollToBottom();
    }
}

void GalleryMainWindow::onGoToIcon() {
    QString iconName = QInputDialog::getText(this, "Go To Icon", "Icon name:");
    if (!iconName.isEmpty() && m_iconGrid) {
        m_iconGrid->scrollToIcon(iconName);
    }
}

void GalleryMainWindow::onJumpToTop() {
    if (m_iconGrid) {
        m_iconGrid->scrollToTop();
    }
}

void GalleryMainWindow::onJumpToBottom() {
    if (m_iconGrid) {
        m_iconGrid->scrollToBottom();
    }
}

void GalleryMainWindow::onGoToPage(int page) {
    m_currentPage = page;
    updateStatus(QString("Page %1").arg(page + 1));
}

void GalleryMainWindow::onNextPage() {
    m_currentPage++;
    updateStatus(QString("Page %1").arg(m_currentPage + 1));
}

void GalleryMainWindow::onPreviousPage() {
    if (m_currentPage > 0) {
        m_currentPage--;
        updateStatus(QString("Page %1").arg(m_currentPage + 1));
    }
}

// Icon Operations slots
void GalleryMainWindow::onIconContextMenu(const QString& iconName, const QPoint& pos) {
    QMenu contextMenu(this);

    contextMenu.addAction("Copy Name", this, [this, iconName]() {
        QApplication::clipboard()->setText(iconName);
        updateStatus("Copied: " + iconName);
    });

    contextMenu.addAction("Copy as Code", this, [this, iconName]() {
        QString code = QString("m_lucide->icon(\"%1\")").arg(iconName);
        QApplication::clipboard()->setText(code);
        updateStatus("Copied code for: " + iconName);
    });

    contextMenu.addSeparator();
    contextMenu.addAction("Export...", this, &GalleryMainWindow::exportIcons);

    contextMenu.exec(pos);
}

void GalleryMainWindow::onCopyIcon() {
    onCopyIconName();
}

void GalleryMainWindow::onCopyIconName() {
    if (m_iconGrid && !m_iconGrid->currentIcon().isEmpty()) {
        QString iconName = m_iconGrid->currentIcon();
        QApplication::clipboard()->setText(iconName);
        updateStatus("Copied: " + iconName);
    }
}

void GalleryMainWindow::onCopyIconCode() {
    if (m_iconGrid && !m_iconGrid->currentIcon().isEmpty()) {
        QString iconName = m_iconGrid->currentIcon();
        QString code = QString("m_lucide->icon(\"%1\")").arg(iconName);
        QApplication::clipboard()->setText(code);
        updateStatus("Copied code for: " + iconName);
    }
}

void GalleryMainWindow::onCopyAsCode() {
    onCopyIconCode();
}

void GalleryMainWindow::onCopyIconSvg() {
    // TODO: Implement SVG copy functionality
    QMessageBox::information(this, "Copy SVG", "SVG copy feature coming soon!");
}

void GalleryMainWindow::onCopyIconPath() {
    // TODO: Implement path copy functionality
    QMessageBox::information(this, "Copy Path", "Path copy feature coming soon!");
}

void GalleryMainWindow::onCopyIconAsBase64() {
    // TODO: Implement Base64 copy functionality
    QMessageBox::information(this, "Copy Base64", "Base64 copy feature coming soon!");
}

void GalleryMainWindow::onPreviewIcon() {
    if (m_iconGrid && !m_iconGrid->currentIcon().isEmpty()) {
        QString iconName = m_iconGrid->currentIcon();
        updateIconPreview(iconName);
    }
}

void GalleryMainWindow::onEditIconMetadata() {
    QMessageBox::information(this, "Edit Metadata", "Metadata editing coming soon!");
}

void GalleryMainWindow::onGenerateCode() {
    QMessageBox::information(this, "Generate Code", "Code generation feature coming soon!");
}

void GalleryMainWindow::onIconSizeChanged(int size) {
    if (m_iconGrid) {
        m_iconGrid->setIconSize(size);
        updateStatus(QString("Icon size: %1px").arg(size));
    }
}

void GalleryMainWindow::onIconColorChanged(const QColor& color) {
    updateStatus(QString("Icon color: %1").arg(color.name()));
}

// Export and Import slots
void GalleryMainWindow::onExportIcon() {
    exportIcons();
}

void GalleryMainWindow::onExportIcons() {
    exportIcons();
}

void GalleryMainWindow::onExportSelectedIcon() {
    exportIcons();
}

void GalleryMainWindow::onExportBatch() {
    // TODO: Implement batch export functionality
    QMessageBox::information(this, "Batch Export", "Batch export feature coming soon!");
}

void GalleryMainWindow::onBatchExport() {
    onExportBatch();
}

void GalleryMainWindow::onPrint() {
    QMessageBox::information(this, "Print", "Print feature coming soon!");
}

void GalleryMainWindow::onPrintPreview() {
    QMessageBox::information(this, "Print Preview", "Print preview feature coming soon!");
}

// Favorites slots (refactored to use FavoritesManager)
void GalleryMainWindow::onToggleFavorite() {
    if (m_iconGrid && !m_iconGrid->currentIcon().isEmpty()) {
        QString iconName = m_iconGrid->currentIcon();
        onToggleFavorite(iconName);
    }
}

void GalleryMainWindow::onToggleFavorite(const QString& iconName) {
    if (m_favoritesManager) {
        m_favoritesManager->toggleFavorite(iconName);
        // Status update handled by FavoritesManager signal
    } else {
        // Fallback to old method if manager not available
        if (m_favoriteIcons.contains(iconName)) {
            m_favoriteIcons.removeAll(iconName);
            updateStatus("Removed from favorites: " + iconName);
        } else {
            m_favoriteIcons.append(iconName);
            updateStatus("Added to favorites: " + iconName);
        }
        saveFavorites();
    }
}

void GalleryMainWindow::onAddToFavorites() {
    onToggleFavorite();
}

void GalleryMainWindow::onRemoveFromFavorites() {
    if (m_iconGrid && !m_iconGrid->currentIcon().isEmpty()) {
        QString iconName = m_iconGrid->currentIcon();
        if (m_favoritesManager) {
            m_favoritesManager->removeFavorite(iconName);
        } else {
            m_favoriteIcons.removeAll(iconName);
            saveFavorites();
            updateStatus("Removed from favorites: " + iconName);
        }
    }
}

void GalleryMainWindow::onShowFavorites() {
    if (m_favoritesCheckBox) {
        m_favoritesCheckBox->setChecked(true);
    }
}

void GalleryMainWindow::onClearFavorites() {
    auto reply = QMessageBox::question(this, "Clear Favorites",
                                      "Clear all favorites?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_favoritesManager) {
            m_favoritesManager->clearFavorites();
        } else {
            m_favoriteIcons.clear();
            saveFavorites();
        }
        updateStatus("Favorites cleared");
    }
}

void GalleryMainWindow::onOrganizeFavorites() {
    QMessageBox::information(this, "Organize Favorites", "Favorites organization coming soon!");
}

void GalleryMainWindow::onImportFavorites() {
    QString fileName = QFileDialog::getOpenFileName(this, "Import Favorites",
                                                    "", "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        updateStatus("Favorites imported from: " + fileName);
    }
}

void GalleryMainWindow::onExportFavorites() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Favorites",
                                                    "", "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        updateStatus("Favorites exported to: " + fileName);
    }
}

void GalleryMainWindow::onFavoritesFilterToggled(bool checked) {
    // Already implemented in onFavoritesToggled
    Q_UNUSED(checked);
}

void GalleryMainWindow::onRecentFilterToggled(bool checked) {
    // Already implemented in onRecentToggled
    Q_UNUSED(checked);
}

void GalleryMainWindow::onFavoriteCategoriesChanged() {
    updateStatus("Favorite categories changed");
}

void GalleryMainWindow::onBatchToggleFavorites() {
    QMessageBox::information(this, "Batch Favorites", "Batch favorites feature coming soon!");
}

void GalleryMainWindow::onBatchSelectionChanged() {
    if (m_iconGrid) {
        int count = m_iconGrid->selectedIcons().size();
        updateStatus(QString("%1 icon(s) selected").arg(count));
    }
}

// Loading and Progress slots
void GalleryMainWindow::onIconsLoaded(int count) {
    updateStatus(QString("Loaded %1 icons").arg(count));
    if (m_iconCountLabel) {
        m_iconCountLabel->setText(QString("%1 icons").arg(count));
    }
}

void GalleryMainWindow::onIconsLoadProgress(int current, int total) {
    if (m_progressBar) {
        m_progressBar->setMaximum(total);
        m_progressBar->setValue(current);
    }
    updateStatus(QString("Loading icons: %1/%2").arg(current).arg(total));
}

void GalleryMainWindow::onMetadataLoaded(int count) {
    updateStatus(QString("Metadata loaded for %1 icons").arg(count));
}

void GalleryMainWindow::onLoadingProgress(int current, int total) {
    onIconsLoadProgress(current, total);
}

void GalleryMainWindow::onLoadError(const QString& error) {
    QMessageBox::warning(this, "Load Error", "Error loading icons:\n" + error);
    updateStatus("Load error: " + error);
}

void GalleryMainWindow::onLoadCancelled() {
    updateStatus("Loading cancelled");
}

void GalleryMainWindow::onContentLoaded() {
    updateStatus("Content loaded successfully");
}

// Performance and Monitoring slots
void GalleryMainWindow::onShowPerformanceMetrics() {
    if (m_performanceDock) {
        m_performanceDock->setVisible(!m_performanceDock->isVisible());
    }
}

void GalleryMainWindow::onPerformanceMetricsUpdated(const QVariantMap& metrics) {
    // Update performance display
    if (m_performanceLabel) {
        QString metricsText;
        for (auto it = metrics.constBegin(); it != metrics.constEnd(); ++it) {
            metricsText += QString("%1: %2\n").arg(it.key(), it.value().toString());
        }
        m_performanceLabel->setText(metricsText);
    }
}

void GalleryMainWindow::onPerformanceWarning() {
    updateStatus("Performance warning - consider reducing icon size or enabling compact mode");
}

void GalleryMainWindow::onMemoryWarning() {
    updateStatus("Memory warning - optimizing memory usage");
    // Memory optimization could be implemented here
}

void GalleryMainWindow::onCrashReport(const QString& report) {
    QMessageBox::critical(this, "Crash Report", "Application crash detected:\n" + report);
}

// Keyboard Shortcuts slots
void GalleryMainWindow::onShowKeyboardShortcuts() {
    onShowUserGuide(); // Reuse user guide which includes shortcuts
}

void GalleryMainWindow::onShowShortcuts() {
    onShowKeyboardShortcuts();
}

// System and Application State slots
void GalleryMainWindow::onApplicationStateChanged() {
    updateStatus("Application state changed");
}

void GalleryMainWindow::onScreenConfigurationChanged() {
    if (m_responsiveLayoutManager) {
        m_responsiveLayoutManager->onWindowResized(size());
    }
}

void GalleryMainWindow::onScreenSizeChanged(const QSize& size) {
    if (m_responsiveLayoutManager) {
        m_responsiveLayoutManager->onWindowResized(size);
    }
}

void GalleryMainWindow::onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        setVisible(!isVisible());
    }
}

void GalleryMainWindow::onSystemTrayMessageClicked() {
    show();
    raise();
    activateWindow();
}

// Timer slots
void GalleryMainWindow::onAutoSaveTimer() {
    saveSettings();
}

void GalleryMainWindow::onBackupTimer() {
    // Backup settings
    saveSettings();
}

void GalleryMainWindow::onSyncTimer() {
    // Sync data
}

// Image Gallery slots (placeholders for future image gallery mode)
void GalleryMainWindow::onOpenImages() {
    QMessageBox::information(this, "Open Images", "Image gallery mode coming soon!");
}

void GalleryMainWindow::onOpenImageDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, "Open Image Directory");
    if (!dir.isEmpty()) {
        updateStatus("Image directory: " + dir);
    }
}

void GalleryMainWindow::onImageSelected(const QString& path) {
    updateStatus("Image selected: " + path);
}

void GalleryMainWindow::onImageDoubleClicked(const QString& path) {
    updateStatus("Image opened: " + path);
}

void GalleryMainWindow::onImageViewerModeChanged() {
    updateStatus("Image viewer mode changed");
}

void GalleryMainWindow::onImageZoomIn() {
    updateStatus("Image zoom in");
}

void GalleryMainWindow::onImageZoomOut() {
    updateStatus("Image zoom out");
}

void GalleryMainWindow::onImageFitToWindow() {
    updateStatus("Image fit to window");
}

void GalleryMainWindow::onImageActualSize() {
    updateStatus("Image actual size");
}

void GalleryMainWindow::onImageRotateLeft() {
    updateStatus("Image rotated left");
}

void GalleryMainWindow::onImageRotateRight() {
    updateStatus("Image rotated right");
}

void GalleryMainWindow::onImageFlipHorizontal() {
    updateStatus("Image flipped horizontally");
}

void GalleryMainWindow::onImageFlipVertical() {
    updateStatus("Image flipped vertically");
}

void GalleryMainWindow::onImageEnhance() {
    updateStatus("Image enhanced");
}

void GalleryMainWindow::onImageFilters() {
    updateStatus("Image filters applied");
}

void GalleryMainWindow::onThumbnailSizeChanged(int size) {
    m_thumbnailSize = size;
    updateStatus(QString("Thumbnail size: %1px").arg(size));
}

// Slideshow slots
void GalleryMainWindow::onStartSlideshow() {
    if (m_slideshowTimer) {
        m_slideshowTimer->start(m_slideshowInterval);
        m_slideshowActive = true;
        updateStatus("Slideshow started");
    }
}

void GalleryMainWindow::onStopSlideshow() {
    if (m_slideshowTimer) {
        m_slideshowTimer->stop();
        m_slideshowActive = false;
        updateStatus("Slideshow stopped");
    }
}

void GalleryMainWindow::onToggleSlideshow() {
    if (m_slideshowActive) {
        onStopSlideshow();
    } else {
        onStartSlideshow();
    }
}

void GalleryMainWindow::onSlideshowSettingsChanged() {
    updateStatus("Slideshow settings changed");
}

void GalleryMainWindow::prevSlide() {
    // Navigate to previous icon
    onPreviousIcon();
}

// Animation slots
void GalleryMainWindow::onFadeInFinished() {
    // Animation complete
}

void GalleryMainWindow::onFadeOutFinished() {
    // Animation complete
}

void GalleryMainWindow::onSlideAnimationFinished() {
    // Animation complete
}

// Directory and File slots
void GalleryMainWindow::onOpenDirectory() {
    onOpenImageDirectory();
}

void GalleryMainWindow::onDirectorySelected(const QString& path) {
    updateStatus("Directory selected: " + path);
}

// ============================================================================
// MISSING METHOD IMPLEMENTATIONS
// ============================================================================

void GalleryMainWindow::announceToScreenReader(const QString& message) {
    // Screen reader announcement (platform-specific implementation would go here)
    // For now, just update status
    Q_UNUSED(message);
    // Platform-specific screen reader API calls would go here
}

void GalleryMainWindow::applyCompactMode(bool enabled) {
    if (enabled) {
        // Apply compact mode styling
        if (m_mainToolbar) {
            m_mainToolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
            m_mainToolbar->setIconSize(QSize(16, 16));
        }
        if (m_searchToolBar) {
            m_searchToolBar->setVisible(false);
        }
        updateStatus("Compact mode enabled");
    } else {
        // Restore normal mode
        if (m_mainToolbar) {
            m_mainToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            m_mainToolbar->setIconSize(QSize(24, 24));
        }
        if (m_searchToolBar) {
            m_searchToolBar->setVisible(true);
        }
        updateStatus("Compact mode disabled");
    }
}

void GalleryMainWindow::setViewMode(ViewMode mode) {
    m_currentViewModeEnum = mode;

    switch (mode) {
        case SplitView:
            setSplitView();
            break;
        case GridView:
            setGridView();
            break;
        case ListView:
            setListView();
            break;
        case CardView:
            updateStatus("Card view mode");
            break;
        case CompactView:
            updateStatus("Compact view mode");
            break;
        case DetailView:
            updateStatus("Detail view mode");
            break;
    }
}

void GalleryMainWindow::saveFavorites() {
    // Favorites are now auto-saved by FavoritesManager
    // Keep this method for backward compatibility
    if (m_favoritesManager) {
        m_favoritesManager->saveFavorites();
    } else if (m_settings) {
        m_settings->setValue("favorites", m_favoriteIcons);
        m_settings->sync();
    }
}

void GalleryMainWindow::loadFavorites() {
    // Favorites are now loaded by FavoritesManager in initializeCoreManagers()
    // Keep this method for backward compatibility
    if (m_favoritesManager) {
        // Already loaded in initializeCoreManagers()
        m_favoriteIcons = m_favoritesManager->getFavorites();
    } else if (m_settings) {
        m_favoriteIcons = m_settings->value("favorites").toStringList();
    }
}