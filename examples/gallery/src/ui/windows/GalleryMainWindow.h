/**
 * QtLucide Gallery Application - Main Window
 *
 * A comprehensive main window providing complete gallery functionality
 * with modern UI/UX design and enhanced user experience.
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
#include <QElapsedTimer>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QParallelAnimationGroup>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSequentialAnimationGroup>
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

// Centralized Theme Color System
namespace Theme {
// Primary Colors
const QString PRIMARY = "#0d6efd";
const QString PRIMARY_HOVER = "#0b5ed7";
const QString PRIMARY_PRESSED = "#0a58ca";

// Secondary Colors
const QString SECONDARY = "#6c757d";
const QString SECONDARY_HOVER = "#5c636a";
const QString SECONDARY_PRESSED = "#565e64";

// Semantic Colors
const QString SUCCESS = "#198754";
const QString SUCCESS_HOVER = "#157347";
const QString SUCCESS_PRESSED = "#146c43";

const QString DANGER = "#dc3545";
const QString DANGER_HOVER = "#c82333";
const QString DANGER_PRESSED = "#bd2130";

const QString WARNING = "#fd7e14";
const QString WARNING_HOVER = "#e8650e";
const QString WARNING_PRESSED = "#d35400";

const QString INFO = "#17a2b8";
const QString INFO_HOVER = "#138496";
const QString INFO_PRESSED = "#117a8b";

// Background Colors
const QString BG_PRIMARY = "#ffffff";
const QString BG_SECONDARY = "#f8f9fa";
const QString BG_TERTIARY = "#e9ecef";
const QString BG_QUATERNARY = "#dee2e6";

// Text Colors
const QString TEXT_PRIMARY = "#2c3e50";
const QString TEXT_SECONDARY = "#495057";
const QString TEXT_MUTED = "#6c757d";
const QString TEXT_DISABLED = "#adb5bd";

// Border Colors
const QString BORDER_PRIMARY = "#dee2e6";
const QString BORDER_SECONDARY = "#ced4da";
const QString BORDER_FOCUS = PRIMARY;

// Disabled States
const QString DISABLED_BG = "#adb5bd";
const QString DISABLED_TEXT = "#6c757d";
const QString DISABLED_BORDER = "#ced4da";

// Gradient Definitions
const QString GRADIENT_LIGHT =
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #f8f9fa)";
const QString GRADIENT_SECONDARY =
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f8f9fa, stop:1 #e9ecef)";
const QString GRADIENT_HOVER =
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #e3f2fd, stop:1 #bbdefb)";
const QString GRADIENT_FAVORITE =
    "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fff5f5, stop:1 #ffe6e6)";
} // namespace Theme

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
class ThemeManager;
class ResponsiveLayoutManager;
class GalleryLogger;
class SearchController;
class PerformanceMonitor;

/**
 * @brief Consolidated main window class with comprehensive gallery features
 */
class GalleryMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GalleryMainWindow(lucide::QtLucide* lucide = nullptr, QWidget* parent = nullptr);
    ~GalleryMainWindow();

    // Modern view modes with enhanced functionality
    enum ViewMode {
        GridView = 0,
        DetailView = 1,
        SplitView = 2,
        CardView = 3,
        ListView = 4,
        CompactView = 5
    };

    // Enhanced theme modes with system integration
    enum ThemeMode {
        SystemTheme = 0,
        LightTheme = 1,
        DarkTheme = 2,
        HighContrastTheme = 3,
        CustomTheme = 4
    };

    // Layout density modes for different use cases
    enum LayoutDensity {
        ComfortableDensity = 0, // Generous spacing, large touch targets
        StandardDensity = 1,    // Balanced spacing and density
        CompactDensity = 2,     // Minimal spacing, information dense
        UltraCompactDensity = 3 // Maximum information density
    };

    // Animation quality levels for performance tuning
    enum AnimationQuality {
        NoAnimations = 0,
        MinimalAnimations = 1,  // Essential transitions only
        StandardAnimations = 2, // Full feature set
        EnhancedAnimations = 3  // All effects and micro-interactions
    };

    // Theme utility functions
    static QString getButtonStyle(const QString& bgColor, const QString& hoverColor,
                                  const QString& pressedColor,
                                  const QString& textColor = "#ffffff");
    static QString getToolbarStyle();
    static QString getStatusBarStyle();
    static QString getDockWidgetStyle();
    static QString getSliderStyle();
    static QString getInputStyle(const QString& borderColor = Theme::BORDER_SECONDARY,
                                 const QString& focusColor = Theme::BORDER_FOCUS);
    static QString getCheckboxStyle();
    static QString getGroupBoxStyle();
    static QString getSplitterStyle();
    static QString getProgressBarStyle();
    static QString getTabWidgetStyle();
    static QString getLabelStyle(const QString& bgColor = Theme::BG_SECONDARY,
                                 const QString& textColor = Theme::TEXT_SECONDARY);
    static QString getEnhancedCardStyle();
    static QString getEnhancedToolbarStyle();

    // Public interface for external control and automation
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
    void setLayoutDensity(LayoutDensity density);
    void setAnimationQuality(AnimationQuality quality);

    // Advanced theming and appearance methods
    void applyThemeFromConfig(const QString& themeConfig);
    void exportCurrentTheme(const QString& filePath);
    void importTheme(const QString& filePath);
    void resetThemeToDefaults();

    // Layout and responsiveness methods
    void optimizeForScreenSize(const QSize& screenSize);
    void setResponsiveMode(bool enabled);
    void saveLayoutProfile(const QString& profileName);
    void loadLayoutProfile(const QString& profileName);

    // Accessibility and usability methods
    void enableHighContrastMode(bool enabled);
    void setFontSize(int fontSize);
    void enableScreenReaderSupport(bool enabled);
    void announceToScreenReader(const QString& message);

protected:
    // Enhanced event handling with accessibility support
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void changeEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private slots:
    // Controller slots
    void onSearchResultsChanged(const QStringList& filteredIcons, int totalIcons);
    void onPerformanceMetricsUpdated(const QVariantMap& metrics);

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

    // Enhanced view and layout actions
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
    void onChangeLayoutDensity();

    // Enhanced theme and appearance actions
    void onThemeChanged(ThemeMode theme);
    void onToggleHighContrast();
    void onIncreaseFontSize();
    void onDecreaseFontSize();
    void onResetFontSize();
    void onCustomizeTheme();

    // Enhanced icon selection and interaction
    void onIconSelected(const QString& iconName);
    void onIconDoubleClicked(const QString& iconName);
    void onIconHovered(const QString& iconName);
    void onIconContextMenu(const QString& iconName, const QPoint& position);
    void onIconSelectionChanged(const QStringList& selectedIcons);
    void onBatchSelectionChanged();
    void onSelectAll();
    void onDeselectAll();
    void onInvertSelection();

    // Enhanced icon operations
    void onCopyIconName();
    void onCopyIconCode();
    void onCopyIconSvg();
    void onCopyIconPath();
    void onCopyIconAsBase64();
    void onToggleFavorite();
    void onToggleFavorite(const QString& iconName);
    void onBatchToggleFavorites();
    void onPreviewIcon();
    void onEditIconMetadata();
    void onIconColorChanged(const QColor& color);
    void onIconSizeChanged(int size);

    // Image gallery actions
    void onImageSelected(const QString& imagePath);
    void onImageDoubleClicked(const QString& imagePath);
    void onDirectorySelected(const QString& directoryPath);
    void onToggleGalleryMode();
    void onOpenImageDirectory();
    void onImageViewerModeChanged();
    void onStartSlideshow();
    void onStopSlideshow();
    void onSlideshowSettingsChanged();
    void prevSlide();
    void nextSlide();
    void onImageRotateLeft();
    void onImageRotateRight();
    void onImageFlipHorizontal();
    void onImageFlipVertical();
    void onImageZoomIn();
    void onImageZoomOut();
    void onImageFitToWindow();
    void onImageActualSize();
    void onImageEnhance();
    void onImageFilters();

    // Enhanced navigation
    void onGoToIcon();
    void onNextIcon();
    void onPreviousIcon();
    void onFirstIcon();
    void onLastIcon();
    void onToggleSlideshow();
    void onGoToPage(int page);
    void onNextPage();
    void onPreviousPage();
    void onJumpToTop();
    void onJumpToBottom();

    // File operations
    void onOpenImages();
    void onOpenDirectory();
    void onExportIcon();
    void onExportBatch();
    void onCopyIcon();
    void onGenerateCode();
    void onPrintPreview();
    void onPrint();

    // Enhanced favorites management
    void onShowFavorites();
    void onOrganizeFavorites();
    void onClearFavorites();
    void onFavoriteCategoriesChanged();

    // Settings and help
    void onSettings();
    void onAbout();
    void onShowKeyboardShortcuts();
    void onShowUserGuide();
    void onShowReleaseNotes();
    void onReportIssue();
    void onShowFeedbackDialog();
    void onShowAccessibilityOptions();

    // Content management
    void onContentLoaded();
    void onMetadataLoaded(int totalIcons);
    void onLoadingProgress(int current, int total);
    void onLoadError(const QString& error);
    void onLoadCancelled();

    // Performance and monitoring
    void onMemoryWarning();
    void onPerformanceWarning();
    void onCrashReport(const QString& report);

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
    void onSearchClicked();
    void onClearSearch();
    void onCategoryChanged(const QString& category);
    void onFavoritesToggled(bool checked);
    void onRecentToggled(bool checked);
    void onExportSelectedIcon();
    void onBatchExport();
    void onCopyAsCode();
    void onToggleViewMode();
    void onAddToFavorites();
    void onRemoveFromFavorites();
    void onSearchHistorySelected(const QString& search);

    // Enhanced application state management
    void onIconsLoaded(int totalCount);
    void onIconsLoadProgress(int current, int total);
    void onFilteredIconsChanged(int visibleCount, int totalCount);
    void onApplicationStateChanged();
    void onThemeChanged();
    void onLanguageChanged();

    // System integration
    void onSystemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onSystemTrayMessageClicked();
    void onSystemThemeChanged();
    void onScreenConfigurationChanged();
    void onAccessibilitySettingsChanged();

    // Animation and effects
    void onFadeInFinished();
    void onFadeOutFinished();
    void onSlideAnimationFinished();
    void onAnimationQualityChanged(AnimationQuality quality);
    void importIcons();
    void exportIcons();

    // Auto-save and backup
    void onAutoSaveTimer();
    void onBackupTimer();
    void onSyncTimer();

    // Responsive layout
    void onScreenSizeChanged(const QSize& newSize);
    void onLayoutModeChanged();
    void onPanelConfigurationChanged();

private:
    // Enhanced UI initialization methods
    void initializeBasicUI();
    void initializeEnhancedUI();
    void setupBasicConnections();
    void setupEnhancedConnections();
    void createBasicMenuBar();
    void createModernMenuBar();
    void createEnhancedToolBars();
    void setupEnhancedStatusBar();
    void setupKeyboardShortcuts();
    void setupAdvancedShortcuts();
    void setupAccessibilityFeatures();
    void setupPerformanceMonitoring();
    void initializeCoreManagers();

    // Layout and spacing methods
    void applyConsistentSpacing();
    void setupModernLayout();
    void optimizeLayoutForCurrentScreen();
    void updateLayoutConstraints();
    void animateLayoutTransition();

    // Enhanced theming methods
    void applyModernTheme();
    void setupThemeSystem();
    void connectThemeSignals();
    void updateThemeColors();
    void updateIconTheme();
    void updateComponentThemes();

    // Typography and visual hierarchy
    void applyTypographyHierarchy();
    void setupComponentTypography();
    void updateFontSize(int delta);
    void resetTypographyToDefaults();

    // Enhanced splitter and dock management
    void setupEnhancedSplitters();
    void configureSplitterBehavior();
    void setupEnhancedDockWidgets();
    void saveSplitterStates();
    void restoreSplitterStates();
    void optimizeSplitterSizes();
    void applySplitterStyling();
    void animateSplitterChanges();

    // Modern button styling
    void applyModernButtonStyles();
    void setupButtonAnimations();
    void updateButtonStates();
    void setupButtonGroups();

    // Enhanced search functionality
    void setupAdvancedSearch();
    void setupSearchHistory();
    void updateSearchUI();
    void processSearchQuery(const QString& query);
    void updateSearchResults();
    void enhanceSearchWidgetStyling();
    void setupSearchWidgetAnimations();
    void applyModernSearchStyling();
    void enhanceSearchFeedback();
    void setupSearchSuggestions();

    // Icon grid enhancements
    void enhanceIconGridAppearance();
    void setupIconGridAnimations();
    void optimizeIconRendering();
    void updateIconDisplay();
    void setupIconSelection();

    // Status bar, toolbar, and menu enhancements
    void enhanceStatusBarStyling();
    void enhanceToolbarStyling();
    void enhanceMenuStyling();
    void applyConsistentChromeStyling();
    void updateStatusBarInfo();

    // Component integration and consistency
    void ensureAllComponentsUseThemeSystem();
    void applyThemeToCustomComponents();
    void validateThemeIntegration();
    void setupThemeAwareComponents();
    void updateComponentStates();

    // Widget-specific theme application helpers
    void applyLabelTheme(QWidget* widget);
    void applyButtonTheme(QWidget* widget);
    void applyLineEditTheme(QWidget* widget);
    void applyTextEditTheme(QWidget* widget);
    void applyComboBoxTheme(QWidget* widget);
    void applySpinBoxTheme(QWidget* widget);
    void applySliderTheme(QWidget* widget);
    void applyProgressBarTheme(QWidget* widget);
    void applyCheckableTheme(QWidget* widget);
    void applyGroupBoxTheme(QWidget* widget);
    void applyTabTheme(QWidget* widget);
    void applyScrollTheme(QWidget* widget);
    void applySplitterTheme(QWidget* widget);
    void applyFrameTheme(QWidget* widget);
    void validateColorConsistency();
    void validateTypographyConsistency();

    // Animation and transition methods
    void implementSmoothAnimationsAndTransitions();
    void setupGlobalAnimations();
    void setupWidgetAnimations();
    void setupLayoutTransitions();
    void setupStateChangeAnimations();
    void setupHoverEffectAnimations();
    void setupFocusAnimations();
    void setupLoadingAnimations();
    void updateAnimationQuality();

    // Animation helpers
    void createFadeAnimation(QWidget* widget, int duration = 300);
    void createSlideAnimation(QWidget* widget, const QPoint& startPos, const QPoint& endPos,
                              int duration = 300);
    void createScaleAnimation(QWidget* widget, qreal startScale, qreal endScale,
                              int duration = 200);
    void createOpacityAnimation(QWidget* widget, qreal startOpacity, qreal endOpacity,
                                int duration = 250);
    void createRotationAnimation(QWidget* widget, qreal startAngle, qreal endAngle,
                                 int duration = 300);
    void createPathAnimation(QWidget* widget, const QPainterPath& path, int duration = 500);

    // Widget-specific animation helpers
    void setupButtonHoverAnimation(QPushButton* button);
    void setupToolButtonHoverAnimation(QToolButton* button);
    void setupLabelHoverAnimation(QLabel* label);
    void setupFrameHoverAnimation(QWidget* frame);
    void setupDockWidgetAnimation(QDockWidget* dock);
    void setupSplitterAnimation(QSplitter* splitter);

    // Event handling for animations
    void handleWidgetHoverEnter(QWidget* widget);
    void handleWidgetHoverLeave(QWidget* widget);
    void handleWidgetFocusIn(QWidget* widget);
    void handleWidgetFocusOut(QWidget* widget);
    void handleWidgetResize(QWidget* widget);
    void handleWidgetMove(QWidget* widget);

    // Accessibility and keyboard navigation methods
    void addAccessibilityImprovements();
    void setupKeyboardNavigation();
    void setupScreenReaderSupport();
    void setupFocusIndicators();
    void setupAccessibleLabels();
    void setupAccessibilityKeyboardShortcuts();
    void setupTabOrder();
    void addAriaLabels();
    void setupHighContrastSupport();
    void validateAccessibilityCompliance();

    // Accessibility helper methods
    void toggleHighContrastMode();
    void announceCurrentFocus();
    void showKeyboardShortcutsDialog();
    void updateAccessibilityProperties();
    void setupScreenReaderAnnouncements();

    // Performance optimization methods
    void optimizePerformanceAndUserExperience();
    void setupAdvancedPerformanceMonitoring();
    void optimizeAnimationPerformance();
    void optimizeRenderingPerformance();
    void optimizeMemoryUsage();
    void setupResourceManagement();
    void enableHardwareAcceleration();
    void optimizeEventHandling();
    void setupPerformanceMetrics();
    void validatePerformanceTargets();

    // Performance helper methods
    void monitorFrameRate();
    void monitorMemoryUsage();
    void monitorCPUUsage();
    void cleanupUnusedResources();
    qint64 getCurrentMemoryUsage();
    void optimizeBasedOnPerformance();
    void monitorEventPerformance();
    void handlePerformanceWarnings();
    void enablePerformanceProfiling();

    // Testing and validation methods
    void testThemeSwitchingFunctionality();
    void validateThemeConsistency();
    void testThemeAnimations();
    void validateComponentThemeApplication();
    void testThemeColorRoles();
    void validateThemeResourceLoading();
    void testThemePersistence();
    void runThemeSwitchingTests();

    // Responsive layout testing methods
    void validateResponsiveLayoutBehavior();
    void testLayoutBreakpoints();
    void validateLayoutTransitions();
    void testSplitterBehavior();
    void validateDockWidgetResponsiveness();
    void testMobileLayoutMode();
    void testTabletLayoutMode();
    void testDesktopLayoutMode();
    void simulateScreenSizeChange(const QSize& size);
    void runResponsiveLayoutTests();
    void validateLayoutAtCurrentSize();

    // Animation and transition testing methods
    void ensureAnimationsWorkSmoothly();
    void testAnimationPerformance();
    void validateTransitionSmoothness();
    void testHoverAnimations();
    void testThemeTransitionAnimations();
    void testLayoutTransitionAnimations();
    void validateAnimationTiming();
    void testAnimationMemoryUsage();
    void runAnimationStressTests();
    void validateAnimationAccessibility();

    // Accessibility and usability testing methods
    void verifyAccessibilityAndUsabilityImprovements();
    void testKeyboardNavigation();
    void validateScreenReaderSupport();
    void testFocusManagement();
    void validateAccessibleLabels();
    void testHighContrastSupport();
    void validateKeyboardShortcuts();
    void testTabOrder();
    void validateColorContrast();
    void testUsabilityFeatures();
    void runAccessibilityComplianceTests();
    void filterIcons();
    void refreshIconGrid();
    void updateIconDetails(const QString& iconName);
    void updateIconPreviewWithColor(const QColor& color);
    void initializeIconCategories();
    void loadFavorites();
    void saveFavorites();
    void addToFavorites(const QString& iconName);
    void removeFromFavorites(const QString& iconName);
    void addToRecent(const QString& iconName);
    void updateIconCount();
    QString categorizeIcon(const QString& iconName);
    QStringList fuzzySearch(const QString& query, const QStringList& items);
    void createBasicStatusBar();

    // Responsive layout methods
    void setupResponsiveLayout();
    void updateResponsiveLayout();
    void handleBreakpointChange();
    void optimizeForMobileLayout();
    void optimizeForTabletLayout();
    void optimizeForDesktopLayout();
    void animateLayoutChanges();

    // State management and persistence
    void saveApplicationState();
    void restoreApplicationState();
    void saveUserPreferences();
    void restoreUserPreferences();
    void saveSessionData();
    void restoreSessionData();
    void exportSettings();
    void importSettings();

    // Error handling and recovery
    void setupErrorHandling();
    void handleCriticalError(const QString& error);
    void handleRecoverableError(const QString& error);
    void attemptErrorRecovery();
    void reportError(const QString& error);
    void logError(const QString& error);

    // Icon system methods
    void initializeIconSystem();
    void createIconGrid();
    void loadSampleIcons();
    void optimizeIconLoading();

    // Enhanced UI setup methods
    void setupUI();
    void setupMenuBar();
    void setupToolBars();
    void setupStatusBar();
    void setupCentralWidget();
    void setupDockWidgets();
    void applyEnhancedStyling();
    void setupSystemTray();
    void setupConnections();
    void setupShortcuts();
    void setupAnimations();
    void setupAccessibility();

    // Panel creation methods
    QWidget* createLeftPanel();
    QWidget* createCenterPanel();
    QWidget* createRightPanel();

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
    void updateStatus(const QString& message);
    void updateIconPreview(const QString& iconName);
    void updateWindowTitle();
    void toggleFullscreen();
    void showAboutDialog();
    void showAboutQtDialog();
    void updatePerformanceMetrics();
    void setSplitView();
    void setGridView();
    void setListView();
    void setLightTheme();
    void setDarkTheme();
    void setSystemTheme();
    void onCategoryChanged(int index);
    void initializeIconGrid();
    void applyCurrentTheme();

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
    void initializeMetadataManager();
    void setupApplicationIcon();
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
    void clearAllFilters();
    void updateSearchHistory();

    // Enhanced UI component management
    void updateComponentVisibility();
    void reorganizeLayout();
    void optimizeComponentLayout();
    void refreshAllComponents();

    // Animation system
    void fadeWidget(QWidget* widget, qreal startOpacity, qreal endOpacity, int duration = 300);
    void slideWidget(QWidget* widget, const QPoint& startPos, const QPoint& endPos,
                     int duration = 300);
    void scaleWidget(QWidget* widget, qreal startScale, qreal endScale, int duration = 200);
    void rotateWidget(QWidget* widget, qreal startAngle, qreal endAngle, int duration = 300);
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
    ThemeManager* m_themeManager;
    ResponsiveLayoutManager* m_responsiveLayoutManager;
    GalleryLogger* m_logger;

    // Controllers
    class SearchController* m_searchController;
    class PerformanceMonitor* m_performanceMonitor;

    // Enhanced UI Components
    QWidget* m_centralWidget;
    QStackedWidget* m_stackedWidget;
    QStackedWidget* m_viewStack;
    QSplitter* m_mainSplitter;
    QSplitter* m_rightSplitter;
    QSplitter* m_leftSplitter;
    QSplitter* m_contentSplitter;

    // Basic icon display widgets
    QScrollArea* m_iconScrollArea;
    QGridLayout* m_iconGridLayout;

    // Main widgets (unified from both classes)
    QWidget* m_searchWidget; // Simple search widget container
    IconSearchWidget* m_iconSearchWidget;
    QComboBox* m_categoryFilter; // Simple category filter combobox
    CategorySidebarWidget* m_categorySidebar;
    IconGridWidget* m_iconGrid;
    IconThumbnailGridWidget* m_thumbnailGrid;
    IconDetailsPanel* m_detailsPanel;
    QTabWidget* m_tabWidget; // Tab widget for center panel

    // Image gallery widgets
    ImageViewerWidget* m_imageViewer;
    FileBrowserWidget* m_fileBrowser;

    // Dock widgets with enhanced features
    QDockWidget* m_searchDock;
    QDockWidget* m_filterDock;
    QDockWidget* m_detailsDock;
    QDockWidget* m_favoritesDock;
    QDockWidget* m_performanceDock;
    QDockWidget* m_accessibilityDock;

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
    QLabel* m_currentIconLabel;
    QLabel* m_filterStatusLabel;
    QLabel* m_performanceLabel;
    QLabel* m_accessibilityLabel;
    QLabel* m_themeLabel;
    QProgressBar* m_progressBar;
    QProgressBar* m_loadingProgressBar;
    QPushButton* m_performanceButton;
    QPushButton* m_accessibilityButton;

    // Enhanced action system
    QHash<QString, QAction*> m_actions; // For dynamic action management

    // File menu actions
    QAction* m_newWindowAction;
    QAction* m_newTabAction;
    QAction* m_closeTabAction;
    QAction* m_importAction;
    QAction* m_importSettingsAction;
    QAction* m_exportSettingsAction;
    QAction* m_resetSettingsAction;
    QAction* m_exitAction;

    // Edit menu actions
    QAction* m_copyNameAction;
    QAction* m_copyCodeAction;
    QAction* m_copySvgAction;
    QAction* m_copyPathAction;
    QAction* m_copyBase64Action;
    QAction* m_selectAllAction;
    QAction* m_deselectAllAction;
    QAction* m_invertSelectionAction;

    // View menu actions
    QAction* m_splitViewAction;
    QAction* m_gridViewAction;
    QAction* m_listViewAction;
    QAction* m_toggleSidebarAction;
    QAction* m_toggleDetailsPanelAction;
    QAction* m_toggleStatusBarAction;
    QAction* m_toggleToolbarAction;
    QAction* m_toggleMenuBarAction;
    QAction* m_fullscreenAction;
    QAction* m_compactModeAction;
    QAction* m_animationsAction;

    // Theme menu actions
    QAction* m_lightThemeAction;
    QAction* m_darkThemeAction;
    QAction* m_systemThemeAction;
    QAction* m_highContrastThemeAction;
    QAction* m_customThemeAction;
    QAction* m_customizeThemeAction;

    // Layout density actions
    QAction* m_comfortableLayoutAction;
    QAction* m_standardLayoutAction;
    QAction* m_compactLayoutAction;
    QAction* m_ultraCompactLayoutAction;

    // Search menu actions
    QAction* m_advancedSearchAction;
    QAction* m_clearFiltersAction;
    QAction* m_saveFilterAction;
    QAction* m_loadFilterAction;
    QAction* m_searchHistoryAction;

    // Tools menu actions
    QAction* m_preferencesAction;
    QAction* m_exportAction;
    QAction* m_importFavoritesAction;
    QAction* m_exportFavoritesAction;
    QAction* m_performanceMetricsAction;
    QAction* m_shortcutsAction;
    QAction* m_accessibilityOptionsAction;

    // Help menu actions
    QAction* m_aboutAction;
    QAction* m_aboutQtAction;
    QAction* m_checkUpdatesAction;
    QAction* m_userGuideAction;
    QAction* m_releaseNotesAction;
    QAction* m_reportIssueAction;
    QAction* m_feedbackAction;

    // Context menu actions
    QAction* m_toggleFavoriteAction;
    QAction* m_previewIconAction;
    QAction* m_editMetadataAction;
    QAction* m_copyAsCodeAction;
    QAction* m_batchExportAction;

    // Action groups
    QActionGroup* m_gridSizeActionGroup;
    QActionGroup* m_viewModeActionGroup;
    QActionGroup* m_themeActionGroup;
    QActionGroup* m_languageActionGroup;
    QActionGroup* m_layoutDensityActionGroup;
    QActionGroup* m_animationQualityActionGroup;

    // Enhanced settings and state management
    QSettings* m_settings;
    QString m_currentIconName;
    QString m_currentSearchText;
    QStringList m_currentCategories;
    QStringList m_currentTags;
    QStringList m_filteredIcons;
    QStringList m_recentIcons;
    QStringList m_searchHistory;
    QStringList m_favoriteCategories;
    QStringList m_savedFilters;

    // View and layout state
    ViewMode m_currentViewModeEnum;
    ThemeMode m_currentTheme;
    LayoutDensity m_currentLayoutDensity;
    AnimationQuality m_currentAnimationQuality;
    int m_currentViewMode;
    int m_currentIconSize;
    int m_currentFontSize;
    int m_thumbnailSize;
    int m_slideshowInterval;
    bool m_compactMode;
    bool m_animationsEnabled;
    bool m_showTooltips;
    bool m_autoSaveSettings;
    bool m_highContrastMode;
    bool m_screenReaderEnabled;
    bool m_responsiveMode;

    // Timers and automation
    QTimer* m_autoSaveTimer;
    QTimer* m_backupTimer;
    QTimer* m_performanceTimer;
    QTimer* m_statusUpdateTimer;
    QTimer* m_slideshowTimer;
    QTimer* m_syncTimer;

    // System integration
    QSystemTrayIcon* m_systemTrayIcon;
    QMenu* m_systemTrayMenu;

    // Animation system
    QPropertyAnimation* m_fadeAnimation;
    QPropertyAnimation* m_slideAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
    QList<QPropertyAnimation*> m_activeAnimations;
    QMap<QWidget*, QGraphicsOpacityEffect*> m_opacityEffects;
    QMap<QWidget*, QGraphicsDropShadowEffect*> m_shadowEffects;
    QParallelAnimationGroup* m_layoutAnimationGroup;
    QSequentialAnimationGroup* m_stateAnimationGroup;

    // Dialogs (lazy-loaded)
    std::unique_ptr<PreferencesDialog> m_preferencesDialog;
    std::unique_ptr<IconExportDialog> m_exportDialog;
    ExportDialog* m_exportDialogEnhanced;
    SettingsDialog* m_settingsDialog;

    // Keyboard shortcuts
    QHash<QString, QShortcut*> m_shortcuts;
    QList<QShortcut*> m_categoryShortcuts; // From Enhanced version (Ctrl+1-9)

    // Performance monitoring
    QElapsedTimer m_performanceTimer_internal;
    QVariantMap m_performanceMetrics;
    bool m_performanceMonitoringEnabled;
    bool m_highPerformanceMode;
    bool m_accessibilityMode;

    // State flags (unified from both classes)
    bool m_isInitialized;
    bool m_isClosing;
    bool m_isFullscreen;
    bool m_sidebarVisible;
    bool m_detailsPanelVisible;
    bool m_statusBarVisible;
    bool m_imageGalleryMode;
    bool m_slideshowActive;

    // Gallery functionality
    QStringList m_allIcons;
    QStringList m_filteredIconList;
    QStringList m_favoriteIcons;
    QString m_selectedIconName;
    QLabel* m_iconPreviewLabel;
    QLabel* m_iconInfoLabel;
    QLineEdit* m_searchLineEdit;
    QComboBox* m_categoryComboBox;
    QComboBox* m_searchHistoryCombo;
    QCheckBox* m_favoritesCheckBox;
    QCheckBox* m_recentCheckBox;
    QSlider* m_iconSizeSlider;

    // Icon categorization
    QMap<QString, QStringList> m_iconCategories;
    QMap<QString, QString> m_iconToCategory;

    // Display settings
    int m_iconsPerPage;
    int m_currentPage;
    bool m_isGridView;

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

    // Enhanced theme transition methods
    void animateThemeTransition();
    void animateThemeLabelChange(QLabel* label, const QString& newText);
    void applyThemeToAllComponents();
    void updateIconGridTheme();
    void finalizeThemeTransition();
};

#endif // GALLERYMAINWINDOW_H
