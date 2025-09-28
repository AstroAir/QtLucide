/**
 * QtLucide Gallery Application - Consolidated Main Window
 *
 * A unified main window combining the best features from both GalleryMainWindow
 * and EnhancedGalleryMainWindow implementations, providing comprehensive
 * gallery functionality with modern UI/UX design and enhanced user experience.
 *
 * Features:
 * - Modern, responsive layout with smooth animations
 * - Advanced toolbar and menu system with multiple view modes
 * - Real-time search and filtering with category shortcuts
 * - Customizable workspace with dockable panels and resizable sidebars
 * - Keyboard shortcuts and accessibility support
 * - Performance monitoring and optimization
 * - Theme and appearance customization (Light/Dark/System themes)
 * - Favorites/bookmarks system with import/export
 * - Slideshow functionality with configurable intervals
 * - Drag and drop support for enhanced usability
 * - Thumbnail grid with virtual scrolling and size control
 * - Export and code generation capabilities
 * - Advanced navigation with recent icons tracking
 */

#ifndef GALLERYMAINWINDOW_H
#define GALLERYMAINWINDOW_H

#include <QActionGroup>
#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDockWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>


#include <QtLucide/QtLucide.h>
#include <memory>

// Forward declarations
class IconGridWidget;
class IconThumbnailGridWidget;
class IconDetailsPanel;
class SearchWidget;
class IconSearchWidget;
class CategoryFilterWidget;
class CategorySidebarWidget;
class IconMetadataManager;
class PreferencesDialog;
class IconExportDialog;
class ExportDialog;
class SettingsDialog;
class ImageMetadataManager;
class ContentManager;
class ImageViewerWidget;
class FileBrowserWidget;
class FavoritesManager;

/**
 * @brief Consolidated main window class with comprehensive gallery features
 */
class GalleryMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GalleryMainWindow(QWidget* parent = nullptr);
    ~GalleryMainWindow();

    // View modes (from EnhancedGalleryMainWindow)
    enum ViewMode { GridView, DetailView, SplitView };

    // Theme modes (from EnhancedGalleryMainWindow)
    enum ThemeMode { LightTheme, DarkTheme, SystemTheme };

    // Public interface for external control
    void showIcon(const QString& iconName);
    void showImage(const QString& imagePath);
    void showContent(const QString& identifier);
    void applySearchFilter(const QString& searchText);
    void setViewMode(int mode);
    void setViewMode(ViewMode mode);
    void setIconSize(int size);
    void setGalleryMode(bool imageMode);
    void loadImageDirectory(const QString& directoryPath);
    void setTheme(ThemeMode theme);

protected:
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void changeEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    // Enhanced menu and toolbar actions
    void onAboutApplication();
    void onAboutQt();
    void onShowPreferences();
    void onToggleFullscreen();
    void onExportIcons();
    void onImportFavorites();
    void onExportFavorites();
    void onImportSettings();
    void onExportSettings();
    void onResetSettings();
    void onCheckForUpdates();
    void onShowShortcuts();
    void onShowPerformanceMetrics();

    // Enhanced view actions
    void onToggleSidebar();
    void onToggleDetailsPanel();
    void onToggleStatusBar();
    void onToggleToolbar();
    void onToggleMenuBar();
    void onChangeGridSize(int size);
    void onChangeViewMode(int mode);
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();
    void onToggleCompactMode();
    void onToggleAnimations();
    void onZoomFit();
    void onZoomActual();

    // Enhanced icon selection and interaction
    void onIconSelected(const QString& iconName);
    void onIconDoubleClicked(const QString& iconName);
    void onIconHovered(const QString& iconName);
    void onIconContextMenu(const QString& iconName, const QPoint& position);
    void onCopyIconName();
    void onCopyIconCode();
    void onCopyIconSvg();
    void onCopyIconPath();
    void onToggleFavorite();
    void onToggleFavorite(const QString& iconName);
    void onPreviewIcon();
    void onEditIconMetadata();

    // Image gallery actions
    void onImageSelected(const QString& imagePath);
    void onImageDoubleClicked(const QString& imagePath);
    void onDirectorySelected(const QString& directoryPath);
    void onToggleGalleryMode();
    void onOpenImageDirectory();
    void onImageViewerModeChanged();
    void onStartSlideshow();
    void onStopSlideshow();
    void onImageRotateLeft();
    void onImageRotateRight();
    void onImageFlipHorizontal();
    void onImageFlipVertical();
    void onImageZoomIn();
    void onImageZoomOut();
    void onImageFitToWindow();
    void onImageActualSize();

    // Enhanced navigation (from EnhancedGalleryMainWindow)
    void onGoToIcon();
    void onNextIcon();
    void onPreviousIcon();
    void onFirstIcon();
    void onLastIcon();
    void onToggleSlideshow();

    // File operations (from EnhancedGalleryMainWindow)
    void onOpenImages();
    void onOpenDirectory();
    void onExportIcon();
    void onExportBatch();
    void onCopyIcon();
    void onGenerateCode();

    // Favorites (from EnhancedGalleryMainWindow)
    void onShowFavorites();
    void onClearFavorites();

    // Settings and help (from EnhancedGalleryMainWindow)
    void onSettings();
    void onAbout();
    void onShowKeyboardShortcuts();

    // Content management (from EnhancedGalleryMainWindow)
    void onContentLoaded();
    void onMetadataLoaded(int totalIcons);
    void onLoadingProgress(int current, int total);

    // Theme and appearance (from EnhancedGalleryMainWindow)
    void onThumbnailSizeChanged(int size);

    // Enhanced search and filtering
    void onSearchTextChanged(const QString& text);
    void onAdvancedSearchRequested();
    void onCategoryFilterChanged(const QStringList& categories);
    void onTagFilterChanged(const QStringList& tags);
    void onContributorFilterChanged(const QStringList& contributors);
    void onFavoritesFilterToggled(bool enabled);
    void onRecentFilterToggled(bool enabled);
    void onClearAllFilters();
    void onSaveCurrentFilter();
    void onLoadSavedFilter();

    // Enhanced application state management
    void onIconsLoaded(int totalCount);
    void onIconsLoadProgress(int current, int total);
    void onFilteredIconsChanged(int visibleCount, int totalCount);
    void onApplicationStateChanged();
    void onThemeChanged();
    void onLanguageChanged();
    void onPerformanceMetricsUpdated(const QVariantMap& metrics);

    // System integration
    void onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onSystemTrayMessageClicked();

    // Animation and effects
    void onFadeInFinished();
    void onFadeOutFinished();
    void onSlideAnimationFinished();

    // Auto-save and backup
    void onAutoSaveTimer();
    void onBackupTimer();

private:
    // Enhanced UI setup methods
    void setupUI();
    void setupMenuBar();
    void setupToolBars();
    void setupStatusBar();
    void setupCentralWidget();
    void setupDockWidgets();
    void setupSystemTray();
    void setupConnections();
    void setupShortcuts();
    void setupAnimations();
    void setupAccessibility();

    // Enhanced initialization (from EnhancedGalleryMainWindow)
    void initializeComponents();
    void updateUI();

    // Category shortcuts (from EnhancedGalleryMainWindow)
    void setupCategoryShortcuts();
    void onCategoryShortcut(int categoryIndex);

    // Enhanced action management
    void createActions();
    void createMenus();
    void createToolBars();
    void updateActions();
    void updateMenus();
    void updateToolBars();
    void updateStatusBar();
    void updateWindowTitle();

    // Enhanced settings management
    void loadSettings();
    void saveSettings();
    void restoreWindowGeometry();
    void saveWindowGeometry();
    void restoreWindowState();
    void saveWindowState();
    void applyTheme();
    void applyLanguage();

    // Enhanced initialization
    void initializeIconSystem();
    void initializeMetadataManager();
    void setupApplicationIcon();
    void setupPerformanceMonitoring();
    void validateConfiguration();

    // Layout and appearance
    void updateLayout();
    void updateSplitterSizes();
    void updateIconSizes();
    void updateViewMode();
    void applyCompactMode(bool enabled);
    void applyAnimations(bool enabled);

    // Search and filtering helpers
    void applyCurrentFilter();
    void updateFilterUI();
    void saveCurrentFilter();
    void loadSavedFilters();

    // Animation helpers
    void fadeIn(QWidget* widget, int duration = 300);
    void fadeOut(QWidget* widget, int duration = 300);
    void slideIn(QWidget* widget, int duration = 300);
    void slideOut(QWidget* widget, int duration = 300);

    // Performance and monitoring
    void logPerformanceMetrics();
    void updatePerformanceDisplay();
    void optimizePerformance();

    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    ImageMetadataManager* m_imageMetadataManager;
    ContentManager* m_contentManager;
    FavoritesManager* m_favoritesManager;

    // Enhanced UI Components
    QWidget* m_centralWidget;
    QStackedWidget* m_stackedWidget;
    QStackedWidget* m_viewStack;
    QSplitter* m_mainSplitter;
    QSplitter* m_rightSplitter;
    QSplitter* m_leftSplitter;
    QSplitter* m_contentSplitter;

    // Main widgets (unified from both classes)
    SearchWidget* m_searchWidget;
    IconSearchWidget* m_iconSearchWidget;
    CategoryFilterWidget* m_categoryFilter;
    CategorySidebarWidget* m_categorySidebar;
    IconGridWidget* m_iconGrid;
    IconThumbnailGridWidget* m_thumbnailGrid;
    IconDetailsPanel* m_detailsPanel;

    // Image gallery widgets
    ImageViewerWidget* m_imageViewer;
    FileBrowserWidget* m_fileBrowser;

    // Dock widgets
    QDockWidget* m_searchDock;
    QDockWidget* m_filterDock;
    QDockWidget* m_detailsDock;
    QDockWidget* m_favoritesDock;
    QDockWidget* m_performanceDock;

    // Menu and toolbar system
    QMenuBar* m_menuBar;
    QToolBar* m_mainToolBar;
    QToolBar* m_mainToolbar; // From Enhanced version
    QToolBar* m_viewToolBar;
    QToolBar* m_viewToolbar; // From Enhanced version
    QToolBar* m_searchToolBar;
    QToolBar* m_quickAccessToolBar;
    QStatusBar* m_statusBar;

    // Enhanced status bar widgets
    QLabel* m_statusLabel;
    QLabel* m_iconCountLabel;
    QLabel* m_currentIconLabel; // From Enhanced version
    QLabel* m_filterStatusLabel;
    QLabel* m_performanceLabel;
    QProgressBar* m_progressBar;
    QProgressBar* m_loadingProgressBar;
    QPushButton* m_performanceButton;

    // Enhanced action system
    QHash<QString, QAction*> m_actions; // For dynamic action management

    // File menu actions
    QAction* m_newWindowAction;
    QAction* m_importSettingsAction;
    QAction* m_exportSettingsAction;
    QAction* m_resetSettingsAction;
    QAction* m_exitAction;

    // Edit menu actions
    QAction* m_copyNameAction;
    QAction* m_copyCodeAction;
    QAction* m_copySvgAction;
    QAction* m_copyPathAction;
    QAction* m_selectAllAction;
    QAction* m_deselectAllAction;

    // View menu actions
    QAction* m_toggleSidebarAction;
    QAction* m_toggleDetailsPanelAction;
    QAction* m_toggleStatusBarAction;
    QAction* m_toggleToolbarAction;
    QAction* m_toggleMenuBarAction;
    QAction* m_fullscreenAction;
    QAction* m_compactModeAction;
    QAction* m_animationsAction;

    // Search menu actions
    QAction* m_advancedSearchAction;
    QAction* m_clearFiltersAction;
    QAction* m_saveFilterAction;
    QAction* m_loadFilterAction;

    // Tools menu actions
    QAction* m_preferencesAction;
    QAction* m_exportAction;
    QAction* m_importFavoritesAction;
    QAction* m_exportFavoritesAction;
    QAction* m_performanceMetricsAction;
    QAction* m_shortcutsAction;

    // Help menu actions
    QAction* m_aboutAction;
    QAction* m_aboutQtAction;
    QAction* m_checkUpdatesAction;

    // Context menu actions
    QAction* m_toggleFavoriteAction;
    QAction* m_previewIconAction;
    QAction* m_editMetadataAction;

    // Action groups (unified from both classes)
    QActionGroup* m_gridSizeActionGroup;
    QActionGroup* m_viewModeActionGroup;
    QActionGroup* m_viewModeGroup; // From Enhanced version
    QActionGroup* m_themeActionGroup;
    QActionGroup* m_languageActionGroup;

    // Enhanced settings and state management
    QSettings* m_settings;
    QActionGroup* m_themeGroup; // From Enhanced version
    QString m_currentIconName;
    QString m_currentIcon; // From Enhanced version
    QString m_currentSearchText;
    QStringList m_currentCategories;
    QStringList m_currentTags;
    QStringList m_filteredIcons; // From Enhanced version
    QStringList m_recentIcons;   // From Enhanced version
    int m_currentViewMode;
    int m_currentIconSize;
    bool m_compactMode;
    bool m_animationsEnabled;
    bool m_showTooltips; // From Enhanced version

    // Timers and automation
    QTimer* m_autoSaveTimer;
    QTimer* m_backupTimer;
    QTimer* m_performanceTimer;

    // System integration
    QSystemTrayIcon* m_systemTrayIcon;
    QMenu* m_systemTrayMenu;

    // Animation system
    QPropertyAnimation* m_fadeAnimation;
    QPropertyAnimation* m_slideAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;

    // Dialogs (lazy-loaded and from Enhanced version) - order matches constructor
    std::unique_ptr<PreferencesDialog> m_preferencesDialog;
    std::unique_ptr<IconExportDialog> m_exportDialog;
    ExportDialog* m_exportDialogEnhanced; // From Enhanced version
    SettingsDialog* m_settingsDialog;     // From Enhanced version

    // View and theme modes (moved to match constructor order)
    ViewMode m_currentViewModeEnum; // From Enhanced version
    ThemeMode m_currentTheme;       // From Enhanced version
    int m_thumbnailSize;            // From Enhanced version
    int m_slideshowInterval;        // From Enhanced version
    bool m_autoSaveSettings;        // From Enhanced version
    QTimer* m_statusUpdateTimer;
    QTimer* m_slideshowTimer; // From Enhanced version

    // Keyboard shortcuts
    QHash<QString, QShortcut*> m_shortcuts;
    QList<QShortcut*> m_categoryShortcuts; // From Enhanced version (Ctrl+1-9)

    // Performance monitoring
    QElapsedTimer m_performanceTimer_internal;
    QVariantMap m_performanceMetrics;
    bool m_performanceMonitoringEnabled;

    // State flags (unified from both classes)
    bool m_isInitialized;
    bool m_isClosing;
    bool m_isFullscreen;
    bool m_sidebarVisible;
    bool m_detailsPanelVisible;
    bool m_statusBarVisible;
    bool m_imageGalleryMode;
    bool m_slideshowActive;

    // Enhanced configuration
    static constexpr int DEFAULT_GRID_SIZE = 64;
    static constexpr int MIN_GRID_SIZE = 24;
    static constexpr int MAX_GRID_SIZE = 256;
    static constexpr int GRID_SIZE_STEP = 8;
    static constexpr int DEFAULT_VIEW_MODE = 0;              // Grid view
    static constexpr int AUTO_SAVE_INTERVAL = 30000;         // 30 seconds
    static constexpr int BACKUP_INTERVAL = 300000;           // 5 minutes
    static constexpr int PERFORMANCE_UPDATE_INTERVAL = 1000; // 1 second
    static constexpr const char* SETTINGS_GROUP = "MainWindow";
    static constexpr const char* GEOMETRY_KEY = "geometry";
    static constexpr const char* STATE_KEY = "windowState";
    static constexpr const char* SPLITTER_KEY = "splitterSizes";
};

#endif // GALLERYMAINWINDOW_H
