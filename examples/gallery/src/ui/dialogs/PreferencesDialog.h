/**
 * QtLucide Gallery Application - Enhanced Preferences and Settings System
 *
 * A comprehensive, modern preferences dialog with advanced customization options:
 * - Multi-category tabbed interface with search and filtering
 * - Theme management with custom theme creation and import/export
 * - Advanced appearance customization with live preview
 * - Performance optimization settings with real-time monitoring
 * - Keyboard shortcuts customization with conflict detection
 * - Plugin and extension management
 * - User profile management with cloud sync support
 * - Accessibility options with compliance checking
 * - Advanced developer settings and debugging tools
 * - Import/export of complete preference sets
 * - Backup and restore functionality
 * - Multi-language support with custom translations
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFontComboBox>
#include <QFontDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSettings>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#ifdef QTLUCIDE_HAS_NETWORK
    #include <QNetworkAccessManager>
    #include <QNetworkReply>
#endif
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>

#include <memory>

// Include theme manager
#include "../themes/ThemeManager.h"

// Include manager stubs
#include "../../core/managers/ManagerStubs.h"

/**
 * @brief Enhanced preferences dialog with comprehensive customization options
 */
class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget* parent = nullptr);
    ~PreferencesDialog();

    // Enhanced settings structures
    struct GeneralSettings {
        bool checkForUpdates = true;
        bool showWelcomeScreen = true;
        bool rememberWindowState = true;
        bool enableTelemetry = false;
        bool showStatusBar = true;
        bool showToolBar = true;
        QString language = "en"; // Added for implementation compatibility
    };

    struct AppearanceSettings {
        QString theme = "system";     // system, light, dark, custom
        QString customThemePath = ""; // Path to custom theme file
        int iconSize = 64;            // 16, 24, 32, 48, 64, 96, 128, 256
        QString viewMode = "grid";    // grid, list, compact, detailed
        bool showIconNames = true;
        bool showTooltips = true;
        bool useAnimations = true;
        bool useTransitions = true;
        QColor accentColor = QColor("#007ACC");
        QColor backgroundColor = QColor("#2D2D30");
        QColor textColor = QColor("#FFFFFF");
        QString fontFamily = "system";
        int fontSize = 9;
        bool useSystemFont = true;
        double uiScale = 1.0; // UI scaling factor
        bool compactMode = false;
        bool showStatusBar = true;
        bool showToolBar = true;
        bool showSidebar = true;
        QString iconStyle = "filled"; // filled, outlined, rounded
    };

    struct SearchSettings {
        bool enableFuzzySearch = true;
        bool searchInTags = true;
        bool searchInCategories = true;
        bool searchInContributors = true;
        bool showSearchSuggestions = true;
        int maxSuggestions = 20;
        bool highlightMatches = true;
        bool caseSensitive = false;
        bool useRegex = false;
        bool searchHistory = true;
        int maxHistorySize = 100;
        int searchDelay = 300; // Milliseconds
        bool instantSearch = true;
        bool searchInDescription = true;
        bool searchInAliases = true;
    };

    struct PerformanceSettings {
        bool enableLazyLoading = true;
        int cacheSize = 2000; // Number of icons to cache
        bool preloadFavorites = true;
        bool enableVirtualization = true;
        int renderThreads = 0; // 0 = auto-detect
        bool enableGPUAcceleration = false;
        bool enableMultisampling = true;
        int maxFPS = 60;
        bool enableVSync = true;
        bool optimizeMemory = true;
        bool enableProfiling = false;
        int gcInterval = 30000; // Garbage collection interval (ms)
        bool enableBackgroundProcessing = true;
    };

    struct ExportSettings {
        QString defaultFormat = "PNG"; // PNG, SVG, ICO, PDF, WEBP
        int defaultSize = 256;         // Export size in pixels
        QString defaultPath = "";      // Default export directory
        bool preserveAspectRatio = true;
        bool includeMetadata = false;
        QColor backgroundColor = Qt::transparent;
        int quality = 95; // JPEG/WEBP quality
        bool embedColorProfile = false;
        QString namingPattern = "{name}_{size}"; // File naming pattern
        bool createSubfolders = false;
        bool overwriteExisting = false;
    };

    struct KeyboardSettings {
        QHash<QString, QKeySequence> shortcuts;
        bool enableGlobalShortcuts = false;
        bool enableContextMenus = true;
        bool enableTooltipShortcuts = true;
        int keyRepeatDelay = 500;
        int keyRepeatRate = 30;
    };

    struct AccessibilitySettings {
        bool enableScreenReader = false;
        bool highContrastMode = false;
        bool largeIcons = false;
        bool reduceMotion = false;
        bool enableSoundEffects = false;
        double textScale = 1.0;
        bool enableMagnifier = false;
        bool enableColorBlindSupport = false;
        QString colorBlindType = "none"; // none, protanopia, deuteranopia, tritanopia
    };

    struct NetworkSettings {
        bool enableAutoUpdates = true;
        bool enableTelemetry = false;
        bool enableCloudSync = false;
        QString proxyType = "none"; // none, http, socks5
        QString proxyHost = "";
        int proxyPort = 0;
        QString proxyUsername = "";
        QString proxyPassword = "";
        int connectionTimeout = 30000; // Milliseconds
        bool enableSSLVerification = true;
    };

    struct DeveloperSettings {
        bool enableDebugMode = false;
        bool showPerformanceOverlay = false;
        bool enableConsoleLogging = true;
        bool enableFileLogging = false;
        QString logLevel = "Info"; // Trace, Debug, Info, Warning, Error, Critical
        bool enableHotReload = false;
        bool showMemoryUsage = false;
        bool enableApiAccess = false;
        int apiPort = 8080;
        QString customCssPath = "";
    };

    // Enhanced public interface

    // Get current settings
    AppearanceSettings getAppearanceSettings() const;
    SearchSettings getSearchSettings() const;
    PerformanceSettings getPerformanceSettings() const;
    ExportSettings getExportSettings() const;
    KeyboardSettings getKeyboardSettings() const;
    AccessibilitySettings getAccessibilitySettings() const;
    NetworkSettings getNetworkSettings() const;
    DeveloperSettings getDeveloperSettings() const;

    // Set settings
    void setAppearanceSettings(const AppearanceSettings& settings);
    void setSearchSettings(const SearchSettings& settings);
    void setPerformanceSettings(const PerformanceSettings& settings);
    void setExportSettings(const ExportSettings& settings);
    void setKeyboardSettings(const KeyboardSettings& settings);
    void setAccessibilitySettings(const AccessibilitySettings& settings);
    void setNetworkSettings(const NetworkSettings& settings);
    void setDeveloperSettings(const DeveloperSettings& settings);

    // Enhanced functionality
    void showCategory(const QString& categoryName);
    void searchSettings(const QString& searchTerm);
    void resetCategory(const QString& categoryName);
    void createCustomTheme();
    void importTheme(const QString& filePath);
    void exportTheme(const QString& filePath);
    void backupSettings(const QString& filePath);
    void restoreSettings(const QString& filePath);

    // Live preview
    void enableLivePreview(bool enabled);
    void applyPreview();
    void cancelPreview();

    // Getter methods
    GeneralSettings getGeneralSettings() const;

public slots:
    void accept() override;
    void reject() override;
    void resetToDefaults();
    void resetCurrentCategory();
    void importSettings();
    void exportSettings();
    void checkForUpdates();
    void openUserManual();
    void reportBug();
    void showAbout();

signals:
    void settingsChanged();
    void settingsApplied();
    void themeChanged(const QString& theme);
    void iconSizeChanged(int size);
    void viewModeChanged(const QString& mode);
    void shortcutsChanged(const QHash<QString, QKeySequence>& shortcuts);
    void accessibilityChanged(const AccessibilitySettings& settings);
    void performanceChanged(const PerformanceSettings& settings);
    void networkChanged(const NetworkSettings& settings);
    void livePreviewToggled(bool enabled);

private slots:
    // Enhanced private slots
    void onThemeChanged();
    void onCustomThemeClicked();
    void onIconSizeChanged();
    void onViewModeChanged();
    void onAccentColorChanged();
    void onBackgroundColorChanged();
    void onTextColorChanged();
    void onFontChanged();
    void onUIScaleChanged();
    void onSearchSettingsChanged();
    void onPerformanceSettingsChanged();
    void onShortcutChanged();
    void onAccessibilityChanged();
    void onNetworkSettingsChanged();
    void onDeveloperSettingsChanged();
    void onPreviewSettings();
    void onRestoreDefaults();
    void onCategoryChanged(int index);
    void onSearchTextChanged(const QString& text);
    void onLivePreviewToggled(bool enabled);
    void onImportTheme();
    void onExportTheme();
    void onCreateCustomTheme();
    void onTestShortcut();
    void onResetShortcuts();
    void onCheckUpdates();

private:
    // Enhanced UI setup methods
    void setupUI();
    void setupSidebar();
    void setupContentArea();
    void setupGeneralTab();
    void setupAppearanceTab();
    void setupSearchTab();
    void setupPerformanceTab();
    void setupExportTab();
    void setupKeyboardTab();
    void setupAccessibilityTab();
    void setupNetworkTab();
    void setupDeveloperTab();
    void setupButtonBox();
    void setupLivePreview();
    void setupSearchFilter();

    // Settings management
    void loadSettings();
    void saveSettings();
    void applySettings();
    void validateSettings();
    void migrateSettings();
    void backupCurrentSettings();
    void restoreBackupSettings();

    // UI updates
    void updatePreview();
    void updateCategoryList();
    void updateSearchResults();
    void connectSignals();
    void disconnectSignals();
    void updateUIFromSettings();
    void updateSettingsFromUI();
    void updateColorButtons();
    void updateBackgroundColorButton();
    void updateFontPreview();
    void updatePerformanceMetrics();

    // Theme management
    void loadThemes();
    void applyTheme(const QString& themeName);
    void createThemePreview();
    void validateTheme(const QString& themePath);

    // Utility methods
    void showRestartRequired();
    void showValidationErrors(const QStringList& errors);
    bool hasUnsavedChanges() const;
    void markAsModified();
    void clearModified();

    // Enhanced UI Components
    QSplitter* m_mainSplitter;
    QVBoxLayout* m_mainLayout;

    // Sidebar for category navigation
    QListWidget* m_categoryList;
    QLineEdit* m_searchEdit;

    // Content area
    QStackedWidget* m_contentStack;
    QScrollArea* m_scrollArea;

    // General Tab (missing from original)
    QWidget* m_generalTab;
    QCheckBox* m_checkUpdatesCheck;
    QCheckBox* m_showWelcomeCheck;
    QCheckBox* m_rememberWindowCheck;
    QCheckBox* m_enableTelemetryCheck;
    QCheckBox* m_showStatusBarCheck;
    QCheckBox* m_showToolBarCheck;
    QComboBox* m_languageCombo; // Added for implementation compatibility

    // Appearance Tab
    QWidget* m_appearanceTab;
    QComboBox* m_themeCombo;
    QPushButton* m_customThemeButton;
    QPushButton* m_importThemeButton;
    QPushButton* m_exportThemeButton;
    QSlider* m_iconSizeSlider;
    QLabel* m_iconSizeLabel;
    QComboBox* m_viewModeCombo;
    QComboBox* m_iconStyleCombo;
    QCheckBox* m_showIconNamesCheck;
    QCheckBox* m_showTooltipsCheck;
    QCheckBox* m_useAnimationsCheck;
    QCheckBox* m_useTransitionsCheck;
    QCheckBox* m_compactModeCheck;
    QPushButton* m_accentColorButton;
    QPushButton* m_backgroundColorButton;
    QPushButton* m_textColorButton;
    QFontComboBox* m_fontCombo;
    QSpinBox* m_fontSizeSpin;
    QCheckBox* m_useSystemFontCheck;
    QSlider* m_uiScaleSlider;
    QLabel* m_uiScaleLabel;

    // Search Tab
    QWidget* m_searchTab;
    QCheckBox* m_fuzzySearchCheck;
    QCheckBox* m_searchTagsCheck;
    QCheckBox* m_searchCategoriesCheck;
    QCheckBox* m_searchContributorsCheck;
    QCheckBox* m_searchDescriptionCheck;
    QCheckBox* m_searchAliasesCheck;
    QCheckBox* m_showSuggestionsCheck;
    QSpinBox* m_maxSuggestionsSpin;
    QCheckBox* m_highlightMatchesCheck;
    QCheckBox* m_caseSensitiveCheck;
    QCheckBox* m_useRegexCheck;
    QCheckBox* m_searchHistoryCheck;
    QSpinBox* m_maxHistorySizeSpin;
    QSpinBox* m_searchDelaySpin;
    QCheckBox* m_instantSearchCheck;

    // Performance Tab
    QWidget* m_performanceTab;
    QCheckBox* m_lazyLoadingCheck;
    QSpinBox* m_cacheSizeSpin;
    QCheckBox* m_preloadFavoritesCheck;
    QCheckBox* m_virtualizationCheck;
    QSpinBox* m_renderThreadsSpin;
    QCheckBox* m_gpuAccelerationCheck;
    QCheckBox* m_multisamplingCheck;
    QSpinBox* m_maxFPSSpin;
    QCheckBox* m_vSyncCheck;
    QCheckBox* m_optimizeMemoryCheck;
    QCheckBox* m_enableProfilingCheck;
    QSpinBox* m_gcIntervalSpin;
    QCheckBox* m_backgroundProcessingCheck;
    QProgressBar* m_performanceBar;
    QLabel* m_performanceLabel;

    // Export Tab
    QWidget* m_exportTab;
    QComboBox* m_defaultFormatCombo;
    QSpinBox* m_defaultSizeSpin;
    QLineEdit* m_defaultPathEdit;
    QPushButton* m_browsePathButton;
    QCheckBox* m_preserveAspectCheck;
    QCheckBox* m_includeMetadataCheck;
    QPushButton* m_exportBackgroundColorButton;
    QSlider* m_qualitySlider;
    QLabel* m_qualityLabel;
    QCheckBox* m_embedColorProfileCheck;
    QLineEdit* m_namingPatternEdit;
    QCheckBox* m_createSubfoldersCheck;
    QCheckBox* m_overwriteExistingCheck;

    // Keyboard Tab
    QWidget* m_keyboardTab;
    QTreeWidget* m_shortcutsTree;
    QKeySequenceEdit* m_shortcutEdit;
    QPushButton* m_assignShortcutButton;
    QPushButton* m_clearShortcutButton;
    QPushButton* m_resetShortcutsButton;
    QCheckBox* m_globalShortcutsCheck;
    QCheckBox* m_contextMenusCheck;
    QCheckBox* m_tooltipShortcutsCheck;

    // Accessibility Tab
    QWidget* m_accessibilityTab;
    QCheckBox* m_screenReaderCheck;
    QCheckBox* m_highContrastCheck;
    QCheckBox* m_largeIconsCheck;
    QCheckBox* m_reduceMotionCheck;
    QCheckBox* m_soundEffectsCheck;
    QSlider* m_textScaleSlider;
    QLabel* m_textScaleLabel;
    QCheckBox* m_magnifierCheck;
    QCheckBox* m_colorBlindSupportCheck;
    QComboBox* m_colorBlindTypeCombo;

    // Network Tab
    QWidget* m_networkTab;
    QCheckBox* m_autoUpdatesCheck;
    QCheckBox* m_telemetryCheck;
    QCheckBox* m_cloudSyncCheck;
    QComboBox* m_proxyTypeCombo;
    QLineEdit* m_proxyHostEdit;
    QSpinBox* m_proxyPortSpin;
    QLineEdit* m_proxyUsernameEdit;
    QLineEdit* m_proxyPasswordEdit;
    QSpinBox* m_connectionTimeoutSpin;
    QCheckBox* m_sslVerificationCheck;

    // Developer Tab
    QWidget* m_developerTab;
    QCheckBox* m_debugModeCheck;
    QCheckBox* m_performanceOverlayCheck;
    QCheckBox* m_consoleLoggingCheck;
    QCheckBox* m_fileLoggingCheck;
    QComboBox* m_logLevelCombo;
    QCheckBox* m_hotReloadCheck;
    QCheckBox* m_showMemoryUsageCheck;
    QCheckBox* m_apiAccessCheck;
    QSpinBox* m_apiPortSpin;
    QLineEdit* m_customCssPathEdit;
    QPushButton* m_browseCssButton;

    // Enhanced button box
    QFrame* m_buttonFrame;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
    QPushButton* m_resetButton;
    QPushButton* m_resetCategoryButton;
    QPushButton* m_importButton;
    QPushButton* m_exportButton;
    QPushButton* m_backupButton;
    QPushButton* m_restoreButton;
    QPushButton* m_helpButton;

    // Main UI components
    QTabWidget* m_tabWidget;

    // Enhanced settings storage and management
    QSettings* m_settings;
    std::unique_ptr<ThemeManager> m_themeManager;
    std::unique_ptr<ShortcutManager> m_shortcutManager;
    std::unique_ptr<PluginManager> m_pluginManager;
    std::unique_ptr<UserProfileManager> m_profileManager;

    // Current settings (enhanced)
    GeneralSettings m_generalSettings;
    AppearanceSettings m_appearanceSettings;
    SearchSettings m_searchSettings;
    PerformanceSettings m_performanceSettings;
    ExportSettings m_exportSettings;
    KeyboardSettings m_keyboardSettings;
    AccessibilitySettings m_accessibilitySettings;
    NetworkSettings m_networkSettings;
    DeveloperSettings m_developerSettings;

    // Backup settings for cancel/reset functionality
    AppearanceSettings m_backupAppearanceSettings;
    SearchSettings m_backupSearchSettings;
    PerformanceSettings m_backupPerformanceSettings;
    ExportSettings m_backupExportSettings;
    KeyboardSettings m_backupKeyboardSettings;
    AccessibilitySettings m_backupAccessibilitySettings;
    NetworkSettings m_backupNetworkSettings;
    DeveloperSettings m_backupDeveloperSettings;

    // Enhanced preview and live preview
    QWidget* m_previewWidget;
    QLabel* m_previewLabel;
    QFrame* m_previewFrame;
    QTimer* m_previewTimer;
    bool m_livePreviewEnabled;

    // State management
    bool m_settingsChanged;
    bool m_restartRequired;
    QString m_currentCategory;
    QStringList m_modifiedCategories;
    QHash<QString, QVariant> m_originalValues;

    // Performance monitoring
    QTimer* m_performanceTimer;
    QElapsedTimer m_updateTimer;
    QVariantMap m_performanceMetrics;

    // Animation and effects
    QPropertyAnimation* m_fadeAnimation;
    QPropertyAnimation* m_slideAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;

    // Network manager for updates and cloud sync
#ifdef QTLUCIDE_HAS_NETWORK
    QNetworkAccessManager* m_networkManager;
#endif

    // Constants
    static constexpr int PREVIEW_UPDATE_DELAY = 500;         // ms
    static constexpr int PERFORMANCE_UPDATE_INTERVAL = 1000; // ms
    static constexpr int ANIMATION_DURATION = 250;           // ms
    static constexpr const char* SETTINGS_GROUP = "PreferencesDialog";
    static constexpr const char* BACKUP_SUFFIX = ".backup";
};

#endif // PREFERENCESDIALOG_H
