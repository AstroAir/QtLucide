/**
 * @file GalleryMainWindow.cpp
 * @brief Implementation of GalleryMainWindow
 * @details Main window implementation with layout management and menu setup.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "GalleryMainWindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QScrollArea>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QUrl>
#include <QVBoxLayout>

// Include dialogs
#include "ExportDialog.h"
#include "IconExportDialog.h"
#include "PreferencesDialog.h"

// Include managers and widgets
#include "CategorySidebarWidget.h"
#include "ContentManager.h"
#include "GalleryTypes.h"
#include "IconDetailsPanel.h"
#include "IconGridWidget.h"
#include "IconMetadataManager.h"
#include "LayoutConfig.h"
#include "ThemeManager.h"

// QtLucide library
#include <QtLucide/QtLucide.h>

namespace gallery {

GalleryMainWindow::GalleryMainWindow(QWidget* parent)
    : QMainWindow(parent), m_lucide(std::make_unique<lucide::QtLucide>()),
      m_contentManager(std::make_unique<ContentManager>(this)),
      m_themeManager(std::make_unique<ThemeManager>(this)) {
    // Initialize QtLucide before using icons
    if (!m_lucide->initLucide()) {
        qWarning() << "Failed to initialize QtLucide";
    }

    setWindowTitle(QStringLiteral("QtLucide Gallery"));
    // Use a Lucide icon as window icon
    setWindowIcon(m_lucide->icon("layout-grid"));

    // Set window properties
    setMinimumSize(LayoutConfig::WINDOW_MIN_WIDTH, LayoutConfig::WINDOW_MIN_HEIGHT);
    resize(LayoutConfig::WINDOW_DEFAULT_WIDTH, LayoutConfig::WINDOW_DEFAULT_HEIGHT);

    // Initialize UI components
    initializeUI();

    // Create menus and actions
    createActions();
    createMenuBar();
    createStatusBar();

    // Setup keyboard shortcuts
    setupShortcuts();

    // Setup theme management
    m_themeManager->setTheme(ThemeMode::Dark);
    connect(m_themeManager.get(), &ThemeManager::themeChanged, this,
            [this](ThemeMode) { onThemeChanged(); });

    // Connect signals before loading data
    connectSignals();

    // Initialize metadata loading
    initializeMetadata();

    // Load saved settings
    loadSettings();
}

GalleryMainWindow::~GalleryMainWindow() {
    saveSettings();
}

void GalleryMainWindow::initializeUI() {
    // Create main widget and central layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create main splitter for three-panel layout
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    m_mainSplitter->setOpaqueResize(false);
    m_mainSplitter->setHandleWidth(4);

    // Create category sidebar (left panel)
    m_categoryPanel = new CategorySidebarWidget(this);
    m_categoryPanel->setObjectName("categorySidebar");
    m_categoryPanel->setMinimumWidth(LayoutConfig::SIDEBAR_MIN_WIDTH);
    m_categoryPanel->setMaximumWidth(LayoutConfig::SIDEBAR_MAX_WIDTH);

    // Create icon grid (center panel) with scroll area
    m_iconGridScrollArea = new QScrollArea(this);
    m_iconGridScrollArea->setObjectName("iconGridArea");
    m_iconGridScrollArea->setWidgetResizable(true);
    m_iconGridScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_iconGridScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_iconGridScrollArea->setFrameShape(QFrame::NoFrame);
    m_iconGridScrollArea->setMinimumWidth(400);

    m_iconGrid = new IconGridWidget();
    m_iconGridScrollArea->setWidget(m_iconGrid);

    // Create details panel (right panel)
    m_detailsPanel = new IconDetailsPanel(this);
    m_detailsPanel->setObjectName("detailsPanel");
    m_detailsPanel->setMinimumWidth(LayoutConfig::DETAILS_PANEL_MIN_WIDTH);
    m_detailsPanel->setMaximumWidth(LayoutConfig::DETAILS_PANEL_MAX_WIDTH);

    // Add panels to splitter
    m_mainSplitter->addWidget(m_categoryPanel);
    m_mainSplitter->addWidget(m_iconGridScrollArea);
    m_mainSplitter->addWidget(m_detailsPanel);

    // Set splitter sizes: category (20%), grid (50%), details (30%)
    QList<int> sizes;
    int totalWidth = LayoutConfig::WINDOW_DEFAULT_WIDTH;
    sizes << (totalWidth / 5)       // Category sidebar
          << (totalWidth / 2)       // Icon grid
          << (totalWidth * 3 / 10); // Details panel
    m_mainSplitter->setSizes(sizes);

    mainLayout->addWidget(m_mainSplitter);
    setCentralWidget(centralWidget);

    // Inject QtLucide instance to widgets that need it
    m_iconGrid->setLucideInstance(m_lucide.get());
    m_detailsPanel->setLucideInstance(m_lucide.get());
}

void GalleryMainWindow::createActions() {
    // File menu actions
    m_exportAction =
        new QAction(m_lucide->icon("download"), QStringLiteral("&Export Icons..."), this);
    m_exportAction->setShortcut(QKeySequence::Save);
    m_exportAction->setStatusTip(QStringLiteral("Export selected icons"));
    connect(m_exportAction, &QAction::triggered, this, &GalleryMainWindow::onExportTriggered);

    m_quitAction = new QAction(m_lucide->icon("log-out"), QStringLiteral("&Quit"), this);
    m_quitAction->setShortcut(QKeySequence::Quit);
    m_quitAction->setStatusTip(QStringLiteral("Quit the application"));
    connect(m_quitAction, &QAction::triggered, this, &QMainWindow::close);

    // Edit menu actions
    m_preferencesAction =
        new QAction(m_lucide->icon("settings"), QStringLiteral("&Preferences..."), this);
    m_preferencesAction->setShortcut(QKeySequence::Preferences);
    m_preferencesAction->setStatusTip(QStringLiteral("Open preferences dialog"));
    connect(m_preferencesAction, &QAction::triggered, this,
            &GalleryMainWindow::onPreferencesTriggered);

    // View menu actions
    m_toggleDetailsPanelAction =
        new QAction(m_lucide->icon("panel-right"), QStringLiteral("Toggle &Details Panel"), this);
    m_toggleDetailsPanelAction->setShortcut(Qt::CTRL | Qt::Key_D);
    m_toggleDetailsPanelAction->setStatusTip(QStringLiteral("Toggle details panel visibility"));
    m_toggleDetailsPanelAction->setCheckable(true);
    m_toggleDetailsPanelAction->setChecked(true);
    connect(m_toggleDetailsPanelAction, &QAction::triggered, this,
            &GalleryMainWindow::onToggleDetailsPanel);

    // Help menu actions
    m_aboutAction = new QAction(m_lucide->icon("info"), QStringLiteral("&About"), this);
    m_aboutAction->setStatusTip(QStringLiteral("About this application"));
    connect(m_aboutAction, &QAction::triggered, this, &GalleryMainWindow::onAboutTriggered);

    m_aboutQtAction = new QAction(m_lucide->icon("heart"), QStringLiteral("About &Qt"), this);
    m_aboutQtAction->setStatusTip(QStringLiteral("About Qt framework"));
    connect(m_aboutQtAction, &QAction::triggered, this, &GalleryMainWindow::onAboutQtTriggered);
}

void GalleryMainWindow::createMenuBar() {
    // File menu
    m_fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    m_fileMenu->addAction(m_exportAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_quitAction);

    // Edit menu
    m_editMenu = menuBar()->addMenu(QStringLiteral("&Edit"));
    m_editMenu->addAction(m_preferencesAction);

    // View menu
    m_viewMenu = menuBar()->addMenu(QStringLiteral("&View"));
    m_viewMenu->addAction(m_toggleDetailsPanelAction);

    // Help menu
    m_helpMenu = menuBar()->addMenu(QStringLiteral("&Help"));
    m_helpMenu->addAction(m_aboutAction);
    m_helpMenu->addAction(m_aboutQtAction);
}

void GalleryMainWindow::createStatusBar() {
    m_statusLabel = new QLabel(this);
    m_statusLabel->setText(QStringLiteral("Ready"));
    statusBar()->addWidget(m_statusLabel);
}

void GalleryMainWindow::setupShortcuts() {
    // Ctrl+K to focus search
    auto* searchShortcut = new QAction(this);
    searchShortcut->setShortcut(Qt::CTRL | Qt::Key_K);
    addAction(searchShortcut);
    connect(searchShortcut, &QAction::triggered, this, [this]() {
        // Emit signal or call method to focus search
        // This will be connected to the search widget when implemented
    });
}

void GalleryMainWindow::loadSettings() {
    QSettings settings(QStringLiteral("QtLucide"), QStringLiteral("Gallery"));

    // Restore window geometry
    if (settings.contains(QStringLiteral("geometry"))) {
        restoreGeometry(settings.value(QStringLiteral("geometry")).toByteArray());
    }

    if (settings.contains(QStringLiteral("windowState"))) {
        restoreState(settings.value(QStringLiteral("windowState")).toByteArray());
    }

    if (settings.contains(QStringLiteral("splitterState"))) {
        m_mainSplitter->restoreState(settings.value(QStringLiteral("splitterState")).toByteArray());
    }

    // Restore details panel visibility
    bool detailsVisible = settings.value(QStringLiteral("detailsPanelVisible"), true).toBool();
    if (!detailsVisible) {
        onToggleDetailsPanel();
    }
}

void GalleryMainWindow::saveSettings() const {
    QSettings settings(QStringLiteral("QtLucide"), QStringLiteral("Gallery"));

    settings.setValue(QStringLiteral("geometry"), saveGeometry());
    settings.setValue(QStringLiteral("windowState"), saveState());
    settings.setValue(QStringLiteral("splitterState"), m_mainSplitter->saveState());
    settings.setValue(QStringLiteral("detailsPanelVisible"), m_detailsPanelVisible);
}

void GalleryMainWindow::initializeMetadata() {
    // Get resource paths from QtLucide library resources
    QString categoriesPath = QStringLiteral(":/lucide/metadata/categories.json");
    QString iconsPath = QStringLiteral(":/lucide/metadata/icons.json");

    // Initialize content manager
    if (!m_contentManager->initialize(categoriesPath, iconsPath)) {
        QMessageBox::warning(this, QStringLiteral("Warning"),
                             QStringLiteral("Failed to load icon metadata from resources.\n"
                                            "Make sure QtLucide resources are properly linked."));
        return;
    }

    // Update category list in sidebar
    updateCategoryList();

    // Trigger initial icon loading by resetting filters (emits iconFilterChanged)
    m_contentManager->resetFilters();
}

void GalleryMainWindow::closeEvent(QCloseEvent* event) {
    saveSettings();
    event->accept();
}

void GalleryMainWindow::onExportTriggered() {
    // Create and configure export dialog
    ExportDialog dialog(this);

    // Set QtLucide instance for icon rendering
    dialog.setLucideInstance(m_lucide.get());

    // Get current filtered icons from content manager
    QStringList availableIcons = m_contentManager->getFilteredIcons();
    dialog.setAvailableIcons(availableIcons);

    // Show dialog
    dialog.exec();
}

void GalleryMainWindow::onPreferencesTriggered() {
    PreferencesDialog dialog(this);
    dialog.loadSettings();

    if (dialog.exec() == QDialog::Accepted) {
        dialog.saveSettings();

        // Apply preferences
        PreferencesSettings prefs = dialog.getPreferences();

        // Apply theme
        ThemeMode themeMode;
        switch (prefs.themeMode) {
            case 0:
                themeMode = ThemeMode::Dark;
                break;
            case 1:
                themeMode = ThemeMode::Light;
                break;
            default:
                themeMode = ThemeMode::System;
                break;
        }
        m_themeManager->setTheme(themeMode);

        // Update sidebar theme
        m_categoryPanel->setTheme(themeMode == ThemeMode::Dark);

        // Apply default icon options
        IconOptions options = m_categoryPanel->getIconOptions();
        options.size = prefs.defaultIconSize;
        options.strokeWidth = prefs.defaultStrokeWidth;
        m_categoryPanel->setIconOptions(options);

        // Refresh icon grid
        m_iconGrid->setIconSize(prefs.defaultIconSize);
        m_iconGrid->update();
    }
}

void GalleryMainWindow::onToggleDetailsPanel() {
    m_detailsPanelVisible = !m_detailsPanelVisible;
    m_detailsPanel->setVisible(m_detailsPanelVisible);
    m_toggleDetailsPanelAction->setChecked(m_detailsPanelVisible);
}

void GalleryMainWindow::onAboutTriggered() {
    QMessageBox::about(
        this, QStringLiteral("About QtLucide Gallery"),
        QStringLiteral("QtLucide Gallery\n\n"
                       "A modern gallery application showcasing 1634+ Lucide icons.\n\n"
                       "Built with Qt6 and designed as a QtAwesome replacement.\n\n"
                       "Copyright 2025 Max Qian\n"
                       "Licensed under MIT License"));
}

void GalleryMainWindow::onAboutQtTriggered() {
    QApplication::aboutQt();
}

void GalleryMainWindow::onIconSelected(const QString& iconName) {
    m_contentManager->selectIcon(iconName);
    // Get metadata from content manager if available
    IconMetadata metadata;
    if (m_contentManager->iconMetadata()) {
        metadata = m_contentManager->iconMetadata()->getIconMetadata(iconName);
    }
    m_detailsPanel->setSelectedIcon(iconName, metadata);
    m_statusLabel->setText(QStringLiteral("Selected: %1").arg(iconName));
}

void GalleryMainWindow::onIconsFiltered(const QStringList& icons) {
    m_statusLabel->setText(QStringLiteral("Showing %1 icons").arg(icons.count()));
}

void GalleryMainWindow::onThemeChanged() {
    // Theme change is handled automatically by QApplication stylesheet
}

void GalleryMainWindow::onOptionsChanged(const IconOptions& options) {
    // Update icon grid with new options
    m_iconGrid->setIconSize(options.size);
    m_iconGrid->update();

    // Update details panel preview if an icon is selected
    if (!m_contentManager->getSelectedIcon().isEmpty()) {
        IconMetadata metadata;
        if (m_contentManager->iconMetadata()) {
            metadata = m_contentManager->iconMetadata()->getIconMetadata(
                m_contentManager->getSelectedIcon());
        }
        m_detailsPanel->setSelectedIcon(m_contentManager->getSelectedIcon(), metadata);
    }
}

void GalleryMainWindow::connectSignals() {
    // ContentManager filtered icons -> IconGrid display
    connect(m_contentManager.get(), &ContentManager::iconFilterChanged, m_iconGrid,
            &IconGridWidget::setIcons);

    // ContentManager filtered icons -> Status bar update
    connect(m_contentManager.get(), &ContentManager::iconFilterChanged, this,
            &GalleryMainWindow::onIconsFiltered);

    // CategorySidebar category selection -> ContentManager filter
    connect(m_categoryPanel, &CategorySidebarWidget::categorySelected, m_contentManager.get(),
            &ContentManager::setCategory);

    // CategorySidebar show all -> ContentManager clear category filter
    connect(m_categoryPanel, &CategorySidebarWidget::showAllClicked, this,
            [this]() { m_contentManager->setCategory(QString()); });

    // CategorySidebar options changed -> Update icon display
    connect(m_categoryPanel, &CategorySidebarWidget::optionsChanged, this,
            &GalleryMainWindow::onOptionsChanged);

    // IconGrid click -> Icon selection
    connect(m_iconGrid, &IconGridWidget::iconClicked, this, &GalleryMainWindow::onIconSelected);

    // IconDetailsPanel export request
    connect(m_detailsPanel, &IconDetailsPanel::exportRequested, this,
            [this](const QString& iconName) {
                IconExportDialog dialog(this);
                dialog.setLucideInstance(m_lucide.get());
                dialog.setIconName(iconName);
                dialog.exec();
            });

    // IconDetailsPanel favorite toggle
    connect(m_detailsPanel, &IconDetailsPanel::favoriteToggled, this,
            [this](const QString& iconName, bool isFavorite) {
                if (isFavorite) {
                    m_contentManager->favorites()->addFavorite(iconName);
                } else {
                    m_contentManager->favorites()->removeFavorite(iconName);
                }
            });

    // IconDetailsPanel tag click -> Search by tag
    connect(m_detailsPanel, &IconDetailsPanel::tagClicked, this,
            [this](const QString& tag) { m_contentManager->setSearchText(tag); });
}

void GalleryMainWindow::updateCategoryList() {
    if (!m_contentManager->iconMetadata()) {
        return;
    }

    // Get categories and their icon counts
    QStringList categories = m_contentManager->iconMetadata()->getCategories();
    QMap<QString, int> categoryIconCounts;

    for (const QString& category : categories) {
        int count = m_contentManager->iconMetadata()->getCategoryIconCount(category);
        categoryIconCounts[category] = count;
    }

    // Update sidebar with categories
    m_categoryPanel->setCategories(categories, categoryIconCounts);

    // Update status bar with total count
    int totalCount = m_contentManager->iconMetadata()->getTotalIconCount();
    m_statusLabel->setText(QStringLiteral("Total: %1 icons").arg(totalCount));
}

} // namespace gallery
