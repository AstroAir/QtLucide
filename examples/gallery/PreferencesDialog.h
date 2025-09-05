/**
 * QtLucide Gallery Application - Preferences Dialog Header
 *
 * Comprehensive preferences dialog for customizing the gallery experience
 * including themes, view options, performance settings, and more.
 */

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QColorDialog>
#include <QFontComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QSplitter>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog();

    // Settings access
    struct AppearanceSettings {
        QString theme = "system";           // system, light, dark
        int iconSize = 64;                  // 32, 48, 64, 96, 128
        QString viewMode = "grid";          // grid, list, compact
        bool showIconNames = true;
        bool showTooltips = true;
        bool useAnimations = true;
        QColor accentColor = QColor("#007ACC");
        QString fontFamily = "system";
        int fontSize = 9;
    };

    struct SearchSettings {
        bool enableFuzzySearch = true;
        bool searchInTags = true;
        bool searchInCategories = true;
        bool showSearchSuggestions = true;
        int maxSuggestions = 10;
        bool highlightMatches = true;
        bool caseSensitive = false;
    };

    struct PerformanceSettings {
        bool enableLazyLoading = true;
        int cacheSize = 1000;               // Number of icons to cache
        bool preloadFavorites = true;
        bool enableVirtualization = true;
        int renderThreads = 4;
        bool enableGPUAcceleration = false;
    };

    struct ExportSettings {
        QString defaultFormat = "PNG";      // PNG, SVG, ICO, PDF
        int defaultSize = 256;              // Export size in pixels
        QString defaultPath = "";           // Default export directory
        bool preserveAspectRatio = true;
        bool includeMetadata = false;
        QColor backgroundColor = Qt::transparent;
    };

    struct GeneralSettings {
        bool checkForUpdates = true;
        bool showWelcomeScreen = true;
        bool rememberWindowState = true;
        bool enableTelemetry = false;
        QString language = "en";
        bool showStatusBar = true;
        bool showToolBar = true;
    };

    // Get current settings
    AppearanceSettings getAppearanceSettings() const;
    SearchSettings getSearchSettings() const;
    PerformanceSettings getPerformanceSettings() const;
    ExportSettings getExportSettings() const;
    GeneralSettings getGeneralSettings() const;

    // Set settings
    void setAppearanceSettings(const AppearanceSettings& settings);
    void setSearchSettings(const SearchSettings& settings);
    void setPerformanceSettings(const PerformanceSettings& settings);
    void setExportSettings(const ExportSettings& settings);
    void setGeneralSettings(const GeneralSettings& settings);

public slots:
    void accept() override;
    void reject() override;
    void resetToDefaults();
    void importSettings();
    void exportSettings();

signals:
    void settingsChanged();
    void themeChanged(const QString& theme);
    void iconSizeChanged(int size);
    void viewModeChanged(const QString& mode);

private slots:
    void onThemeChanged();
    void onIconSizeChanged();
    void onViewModeChanged();
    void onAccentColorChanged();
    void onFontChanged();
    void onPreviewSettings();
    void onRestoreDefaults();

private:
    void setupUI();
    void setupAppearanceTab();
    void setupSearchTab();
    void setupPerformanceTab();
    void setupExportTab();
    void setupGeneralTab();
    void setupButtonBox();

    void loadSettings();
    void saveSettings();
    void applySettings();
    void updatePreview();
    void connectSignals();
    void updateUIFromSettings();
    void updateSettingsFromUI();
    void updateBackgroundColorButton();

    // UI Components
    QTabWidget* m_tabWidget;
    QVBoxLayout* m_mainLayout;

    // Appearance Tab
    QWidget* m_appearanceTab;
    QComboBox* m_themeCombo;
    QSlider* m_iconSizeSlider;
    QLabel* m_iconSizeLabel;
    QComboBox* m_viewModeCombo;
    QCheckBox* m_showIconNamesCheck;
    QCheckBox* m_showTooltipsCheck;
    QCheckBox* m_useAnimationsCheck;
    QPushButton* m_accentColorButton;
    QFontComboBox* m_fontCombo;
    QSpinBox* m_fontSizeSpin;

    // Search Tab
    QWidget* m_searchTab;
    QCheckBox* m_fuzzySearchCheck;
    QCheckBox* m_searchTagsCheck;
    QCheckBox* m_searchCategoriesCheck;
    QCheckBox* m_showSuggestionsCheck;
    QSpinBox* m_maxSuggestionsSpin;
    QCheckBox* m_highlightMatchesCheck;
    QCheckBox* m_caseSensitiveCheck;

    // Performance Tab
    QWidget* m_performanceTab;
    QCheckBox* m_lazyLoadingCheck;
    QSpinBox* m_cacheSizeSpin;
    QCheckBox* m_preloadFavoritesCheck;
    QCheckBox* m_virtualizationCheck;
    QSpinBox* m_renderThreadsSpin;
    QCheckBox* m_gpuAccelerationCheck;

    // Export Tab
    QWidget* m_exportTab;
    QComboBox* m_defaultFormatCombo;
    QSpinBox* m_defaultSizeSpin;
    QLineEdit* m_defaultPathEdit;
    QPushButton* m_browsePathButton;
    QCheckBox* m_preserveAspectCheck;
    QCheckBox* m_includeMetadataCheck;
    QPushButton* m_backgroundColorButton;

    // General Tab
    QWidget* m_generalTab;
    QCheckBox* m_checkUpdatesCheck;
    QCheckBox* m_showWelcomeCheck;
    QCheckBox* m_rememberWindowCheck;
    QCheckBox* m_enableTelemetryCheck;
    QComboBox* m_languageCombo;
    QCheckBox* m_showStatusBarCheck;
    QCheckBox* m_showToolBarCheck;

    // Button Box
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    QPushButton* m_applyButton;
    QPushButton* m_resetButton;
    QPushButton* m_importButton;
    QPushButton* m_exportButton;

    // Settings storage
    QSettings* m_settings;

    // Current settings
    AppearanceSettings m_appearanceSettings;
    SearchSettings m_searchSettings;
    PerformanceSettings m_performanceSettings;
    ExportSettings m_exportSettings;
    GeneralSettings m_generalSettings;

    // Preview widget
    QWidget* m_previewWidget;
    QLabel* m_previewLabel;

    bool m_settingsChanged;
};

#endif // PREFERENCESDIALOG_H
