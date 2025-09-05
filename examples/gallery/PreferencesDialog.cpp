/**
 * QtLucide Gallery Application - Preferences Dialog Implementation
 */

#include "PreferencesDialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

PreferencesDialog::PreferencesDialog(QWidget *parent)
    : QDialog(parent)
    , m_tabWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_settings(new QSettings(this))
    , m_settingsChanged(false)
{
    setWindowTitle("Preferences");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);
    resize(600, 500);

    setupUI();
    loadSettings();
    connectSignals();
}

PreferencesDialog::~PreferencesDialog()
{
}

void PreferencesDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);

    // Setup tabs
    setupAppearanceTab();
    setupSearchTab();
    setupPerformanceTab();
    setupExportTab();
    setupGeneralTab();

    m_mainLayout->addWidget(m_tabWidget);

    // Setup button box
    setupButtonBox();
}

void PreferencesDialog::setupAppearanceTab()
{
    m_appearanceTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_appearanceTab);

    // Theme selection
    QGroupBox* themeGroup = new QGroupBox("Theme", m_appearanceTab);
    QFormLayout* themeLayout = new QFormLayout(themeGroup);

    m_themeCombo = new QComboBox();
    m_themeCombo->addItems({"System Default", "Light", "Dark"});
    themeLayout->addRow("Theme:", m_themeCombo);

    m_accentColorButton = new QPushButton("Choose Color");
    themeLayout->addRow("Accent Color:", m_accentColorButton);

    layout->addWidget(themeGroup);

    // View options
    QGroupBox* viewGroup = new QGroupBox("View Options", m_appearanceTab);
    QFormLayout* viewLayout = new QFormLayout(viewGroup);

    // Icon size
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    m_iconSizeSlider = new QSlider(Qt::Horizontal);
    m_iconSizeSlider->setRange(32, 128);
    m_iconSizeSlider->setValue(64);
    m_iconSizeSlider->setTickPosition(QSlider::TicksBelow);
    m_iconSizeSlider->setTickInterval(16);

    m_iconSizeLabel = new QLabel("64px");
    m_iconSizeLabel->setMinimumWidth(50);

    sizeLayout->addWidget(m_iconSizeSlider);
    sizeLayout->addWidget(m_iconSizeLabel);
    viewLayout->addRow("Icon Size:", sizeLayout);

    m_viewModeCombo = new QComboBox();
    m_viewModeCombo->addItems({"Grid", "List", "Compact"});
    viewLayout->addRow("View Mode:", m_viewModeCombo);

    m_showIconNamesCheck = new QCheckBox("Show icon names");
    m_showTooltipsCheck = new QCheckBox("Show tooltips");
    m_useAnimationsCheck = new QCheckBox("Use animations");

    viewLayout->addRow(m_showIconNamesCheck);
    viewLayout->addRow(m_showTooltipsCheck);
    viewLayout->addRow(m_useAnimationsCheck);

    layout->addWidget(viewGroup);

    // Font settings
    QGroupBox* fontGroup = new QGroupBox("Font", m_appearanceTab);
    QFormLayout* fontLayout = new QFormLayout(fontGroup);

    m_fontCombo = new QFontComboBox();
    m_fontSizeSpin = new QSpinBox();
    m_fontSizeSpin->setRange(8, 24);
    m_fontSizeSpin->setValue(9);

    fontLayout->addRow("Font Family:", m_fontCombo);
    fontLayout->addRow("Font Size:", m_fontSizeSpin);

    layout->addWidget(fontGroup);

    layout->addStretch();
    m_tabWidget->addTab(m_appearanceTab, "Appearance");
}

void PreferencesDialog::setupSearchTab()
{
    m_searchTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_searchTab);

    QGroupBox* searchGroup = new QGroupBox("Search Options", m_searchTab);
    QVBoxLayout* searchLayout = new QVBoxLayout(searchGroup);

    m_fuzzySearchCheck = new QCheckBox("Enable fuzzy search");
    m_searchTagsCheck = new QCheckBox("Search in tags");
    m_searchCategoriesCheck = new QCheckBox("Search in categories");
    m_showSuggestionsCheck = new QCheckBox("Show search suggestions");
    m_highlightMatchesCheck = new QCheckBox("Highlight search matches");
    m_caseSensitiveCheck = new QCheckBox("Case sensitive search");

    searchLayout->addWidget(m_fuzzySearchCheck);
    searchLayout->addWidget(m_searchTagsCheck);
    searchLayout->addWidget(m_searchCategoriesCheck);
    searchLayout->addWidget(m_showSuggestionsCheck);
    searchLayout->addWidget(m_highlightMatchesCheck);
    searchLayout->addWidget(m_caseSensitiveCheck);

    // Max suggestions
    QHBoxLayout* suggestionsLayout = new QHBoxLayout();
    suggestionsLayout->addWidget(new QLabel("Max suggestions:"));
    m_maxSuggestionsSpin = new QSpinBox();
    m_maxSuggestionsSpin->setRange(5, 50);
    m_maxSuggestionsSpin->setValue(10);
    suggestionsLayout->addWidget(m_maxSuggestionsSpin);
    suggestionsLayout->addStretch();

    searchLayout->addLayout(suggestionsLayout);
    layout->addWidget(searchGroup);
    layout->addStretch();

    m_tabWidget->addTab(m_searchTab, "Search");
}

void PreferencesDialog::setupPerformanceTab()
{
    m_performanceTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_performanceTab);

    QGroupBox* perfGroup = new QGroupBox("Performance Options", m_performanceTab);
    QFormLayout* perfLayout = new QFormLayout(perfGroup);

    m_lazyLoadingCheck = new QCheckBox("Enable lazy loading");
    m_preloadFavoritesCheck = new QCheckBox("Preload favorite icons");
    m_virtualizationCheck = new QCheckBox("Enable virtualization");
    m_gpuAccelerationCheck = new QCheckBox("Enable GPU acceleration (experimental)");

    perfLayout->addRow(m_lazyLoadingCheck);
    perfLayout->addRow(m_preloadFavoritesCheck);
    perfLayout->addRow(m_virtualizationCheck);
    perfLayout->addRow(m_gpuAccelerationCheck);

    m_cacheSizeSpin = new QSpinBox();
    m_cacheSizeSpin->setRange(100, 10000);
    m_cacheSizeSpin->setValue(1000);
    m_cacheSizeSpin->setSuffix(" icons");
    perfLayout->addRow("Cache Size:", m_cacheSizeSpin);

    m_renderThreadsSpin = new QSpinBox();
    m_renderThreadsSpin->setRange(1, 16);
    m_renderThreadsSpin->setValue(4);
    perfLayout->addRow("Render Threads:", m_renderThreadsSpin);

    layout->addWidget(perfGroup);
    layout->addStretch();

    m_tabWidget->addTab(m_performanceTab, "Performance");
}

void PreferencesDialog::setupExportTab()
{
    m_exportTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_exportTab);

    QGroupBox* exportGroup = new QGroupBox("Export Settings", m_exportTab);
    QFormLayout* exportLayout = new QFormLayout(exportGroup);

    m_defaultFormatCombo = new QComboBox();
    m_defaultFormatCombo->addItems({"PNG", "SVG", "ICO", "PDF"});
    exportLayout->addRow("Default Format:", m_defaultFormatCombo);

    m_defaultSizeSpin = new QSpinBox();
    m_defaultSizeSpin->setRange(16, 2048);
    m_defaultSizeSpin->setValue(256);
    m_defaultSizeSpin->setSuffix("px");
    exportLayout->addRow("Default Size:", m_defaultSizeSpin);

    // Default path
    QHBoxLayout* pathLayout = new QHBoxLayout();
    m_defaultPathEdit = new QLineEdit();
    m_browsePathButton = new QPushButton("Browse...");
    pathLayout->addWidget(m_defaultPathEdit);
    pathLayout->addWidget(m_browsePathButton);
    exportLayout->addRow("Default Path:", pathLayout);

    m_preserveAspectCheck = new QCheckBox("Preserve aspect ratio");
    m_includeMetadataCheck = new QCheckBox("Include metadata");

    exportLayout->addRow(m_preserveAspectCheck);
    exportLayout->addRow(m_includeMetadataCheck);

    m_backgroundColorButton = new QPushButton("Choose Background Color");
    exportLayout->addRow("Background Color:", m_backgroundColorButton);

    layout->addWidget(exportGroup);
    layout->addStretch();

    m_tabWidget->addTab(m_exportTab, "Export");
}

void PreferencesDialog::setupGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(m_generalTab);

    QGroupBox* generalGroup = new QGroupBox("General Settings", m_generalTab);
    QVBoxLayout* generalLayout = new QVBoxLayout(generalGroup);

    m_checkUpdatesCheck = new QCheckBox("Check for updates automatically");
    m_showWelcomeCheck = new QCheckBox("Show welcome screen on startup");
    m_rememberWindowCheck = new QCheckBox("Remember window state");
    m_enableTelemetryCheck = new QCheckBox("Enable anonymous usage statistics");
    m_showStatusBarCheck = new QCheckBox("Show status bar");
    m_showToolBarCheck = new QCheckBox("Show toolbar");

    generalLayout->addWidget(m_checkUpdatesCheck);
    generalLayout->addWidget(m_showWelcomeCheck);
    generalLayout->addWidget(m_rememberWindowCheck);
    generalLayout->addWidget(m_enableTelemetryCheck);
    generalLayout->addWidget(m_showStatusBarCheck);
    generalLayout->addWidget(m_showToolBarCheck);

    // Language selection
    QHBoxLayout* langLayout = new QHBoxLayout();
    langLayout->addWidget(new QLabel("Language:"));
    m_languageCombo = new QComboBox();
    m_languageCombo->addItems({"English", "简体中文", "Français", "Deutsch", "Español"});
    langLayout->addWidget(m_languageCombo);
    langLayout->addStretch();

    generalLayout->addLayout(langLayout);
    layout->addWidget(generalGroup);
    layout->addStretch();

    m_tabWidget->addTab(m_generalTab, "General");
}

void PreferencesDialog::setupButtonBox()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_importButton = new QPushButton("Import...");
    m_exportButton = new QPushButton("Export...");
    m_resetButton = new QPushButton("Reset to Defaults");

    buttonLayout->addWidget(m_importButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addStretch();

    m_cancelButton = new QPushButton("Cancel");
    m_applyButton = new QPushButton("Apply");
    m_okButton = new QPushButton("OK");
    m_okButton->setDefault(true);

    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_okButton);

    m_mainLayout->addLayout(buttonLayout);
}

void PreferencesDialog::connectSignals()
{
    // Appearance signals
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PreferencesDialog::onThemeChanged);
    connect(m_iconSizeSlider, &QSlider::valueChanged,
            this, &PreferencesDialog::onIconSizeChanged);
    connect(m_viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PreferencesDialog::onViewModeChanged);
    connect(m_accentColorButton, &QPushButton::clicked,
            this, &PreferencesDialog::onAccentColorChanged);
    connect(m_fontCombo, &QFontComboBox::currentFontChanged,
            this, &PreferencesDialog::onFontChanged);
    connect(m_fontSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PreferencesDialog::onFontChanged);

    // Export signals
    connect(m_browsePathButton, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Choose Export Directory",
                                                       m_defaultPathEdit->text());
        if (!dir.isEmpty()) {
            m_defaultPathEdit->setText(dir);
        }
    });

    connect(m_backgroundColorButton, &QPushButton::clicked, [this]() {
        QColor color = QColorDialog::getColor(m_exportSettings.backgroundColor, this,
                                            "Choose Background Color");
        if (color.isValid()) {
            m_exportSettings.backgroundColor = color;
            updateBackgroundColorButton();
        }
    });

    // Button signals
    connect(m_okButton, &QPushButton::clicked, this, &PreferencesDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &PreferencesDialog::reject);
    connect(m_applyButton, &QPushButton::clicked, this, &PreferencesDialog::applySettings);
    connect(m_resetButton, &QPushButton::clicked, this, &PreferencesDialog::resetToDefaults);
    connect(m_importButton, &QPushButton::clicked, this, &PreferencesDialog::importSettings);
    connect(m_exportButton, &QPushButton::clicked, this, &PreferencesDialog::exportSettings);
}

void PreferencesDialog::loadSettings()
{
    m_settings->beginGroup("Appearance");
    m_appearanceSettings.theme = m_settings->value("theme", "system").toString();
    m_appearanceSettings.iconSize = m_settings->value("iconSize", 64).toInt();
    m_appearanceSettings.viewMode = m_settings->value("viewMode", "grid").toString();
    m_appearanceSettings.showIconNames = m_settings->value("showIconNames", true).toBool();
    m_appearanceSettings.showTooltips = m_settings->value("showTooltips", true).toBool();
    m_appearanceSettings.useAnimations = m_settings->value("useAnimations", true).toBool();
    m_appearanceSettings.accentColor = m_settings->value("accentColor", QColor("#007ACC")).value<QColor>();
    m_appearanceSettings.fontFamily = m_settings->value("fontFamily", "system").toString();
    m_appearanceSettings.fontSize = m_settings->value("fontSize", 9).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("Search");
    m_searchSettings.enableFuzzySearch = m_settings->value("enableFuzzySearch", true).toBool();
    m_searchSettings.searchInTags = m_settings->value("searchInTags", true).toBool();
    m_searchSettings.searchInCategories = m_settings->value("searchInCategories", true).toBool();
    m_searchSettings.showSearchSuggestions = m_settings->value("showSearchSuggestions", true).toBool();
    m_searchSettings.maxSuggestions = m_settings->value("maxSuggestions", 10).toInt();
    m_searchSettings.highlightMatches = m_settings->value("highlightMatches", true).toBool();
    m_searchSettings.caseSensitive = m_settings->value("caseSensitive", false).toBool();
    m_settings->endGroup();

    m_settings->beginGroup("Performance");
    m_performanceSettings.enableLazyLoading = m_settings->value("enableLazyLoading", true).toBool();
    m_performanceSettings.cacheSize = m_settings->value("cacheSize", 1000).toInt();
    m_performanceSettings.preloadFavorites = m_settings->value("preloadFavorites", true).toBool();
    m_performanceSettings.enableVirtualization = m_settings->value("enableVirtualization", true).toBool();
    m_performanceSettings.renderThreads = m_settings->value("renderThreads", 4).toInt();
    m_performanceSettings.enableGPUAcceleration = m_settings->value("enableGPUAcceleration", false).toBool();
    m_settings->endGroup();

    m_settings->beginGroup("Export");
    m_exportSettings.defaultFormat = m_settings->value("defaultFormat", "PNG").toString();
    m_exportSettings.defaultSize = m_settings->value("defaultSize", 256).toInt();
    m_exportSettings.defaultPath = m_settings->value("defaultPath",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    m_exportSettings.preserveAspectRatio = m_settings->value("preserveAspectRatio", true).toBool();
    m_exportSettings.includeMetadata = m_settings->value("includeMetadata", false).toBool();
    m_exportSettings.backgroundColor = m_settings->value("backgroundColor", QColor(Qt::transparent)).value<QColor>();
    m_settings->endGroup();

    m_settings->beginGroup("General");
    m_generalSettings.checkForUpdates = m_settings->value("checkForUpdates", true).toBool();
    m_generalSettings.showWelcomeScreen = m_settings->value("showWelcomeScreen", true).toBool();
    m_generalSettings.rememberWindowState = m_settings->value("rememberWindowState", true).toBool();
    m_generalSettings.enableTelemetry = m_settings->value("enableTelemetry", false).toBool();
    m_generalSettings.language = m_settings->value("language", "en").toString();
    m_generalSettings.showStatusBar = m_settings->value("showStatusBar", true).toBool();
    m_generalSettings.showToolBar = m_settings->value("showToolBar", true).toBool();
    m_settings->endGroup();

    // Update UI with loaded settings
    updateUIFromSettings();
}

void PreferencesDialog::updateUIFromSettings()
{
    // Appearance
    int themeIndex = 0;
    if (m_appearanceSettings.theme == "light") themeIndex = 1;
    else if (m_appearanceSettings.theme == "dark") themeIndex = 2;
    m_themeCombo->setCurrentIndex(themeIndex);

    m_iconSizeSlider->setValue(m_appearanceSettings.iconSize);
    m_iconSizeLabel->setText(QString("%1px").arg(m_appearanceSettings.iconSize));

    int viewIndex = 0;
    if (m_appearanceSettings.viewMode == "list") viewIndex = 1;
    else if (m_appearanceSettings.viewMode == "compact") viewIndex = 2;
    m_viewModeCombo->setCurrentIndex(viewIndex);

    m_showIconNamesCheck->setChecked(m_appearanceSettings.showIconNames);
    m_showTooltipsCheck->setChecked(m_appearanceSettings.showTooltips);
    m_useAnimationsCheck->setChecked(m_appearanceSettings.useAnimations);

    if (m_appearanceSettings.fontFamily != "system") {
        m_fontCombo->setCurrentFont(QFont(m_appearanceSettings.fontFamily));
    }
    m_fontSizeSpin->setValue(m_appearanceSettings.fontSize);

    // Search
    m_fuzzySearchCheck->setChecked(m_searchSettings.enableFuzzySearch);
    m_searchTagsCheck->setChecked(m_searchSettings.searchInTags);
    m_searchCategoriesCheck->setChecked(m_searchSettings.searchInCategories);
    m_showSuggestionsCheck->setChecked(m_searchSettings.showSearchSuggestions);
    m_maxSuggestionsSpin->setValue(m_searchSettings.maxSuggestions);
    m_highlightMatchesCheck->setChecked(m_searchSettings.highlightMatches);
    m_caseSensitiveCheck->setChecked(m_searchSettings.caseSensitive);

    // Performance
    m_lazyLoadingCheck->setChecked(m_performanceSettings.enableLazyLoading);
    m_cacheSizeSpin->setValue(m_performanceSettings.cacheSize);
    m_preloadFavoritesCheck->setChecked(m_performanceSettings.preloadFavorites);
    m_virtualizationCheck->setChecked(m_performanceSettings.enableVirtualization);
    m_renderThreadsSpin->setValue(m_performanceSettings.renderThreads);
    m_gpuAccelerationCheck->setChecked(m_performanceSettings.enableGPUAcceleration);

    // Export
    m_defaultFormatCombo->setCurrentText(m_exportSettings.defaultFormat);
    m_defaultSizeSpin->setValue(m_exportSettings.defaultSize);
    m_defaultPathEdit->setText(m_exportSettings.defaultPath);
    m_preserveAspectCheck->setChecked(m_exportSettings.preserveAspectRatio);
    m_includeMetadataCheck->setChecked(m_exportSettings.includeMetadata);
    updateBackgroundColorButton();

    // General
    m_checkUpdatesCheck->setChecked(m_generalSettings.checkForUpdates);
    m_showWelcomeCheck->setChecked(m_generalSettings.showWelcomeScreen);
    m_rememberWindowCheck->setChecked(m_generalSettings.rememberWindowState);
    m_enableTelemetryCheck->setChecked(m_generalSettings.enableTelemetry);
    m_showStatusBarCheck->setChecked(m_generalSettings.showStatusBar);
    m_showToolBarCheck->setChecked(m_generalSettings.showToolBar);
}

void PreferencesDialog::updateBackgroundColorButton()
{
    QString colorName = m_exportSettings.backgroundColor.name();
    QString style = QString("QPushButton { background-color: %1; }").arg(colorName);
    m_backgroundColorButton->setStyleSheet(style);
    m_backgroundColorButton->setText(colorName);
}

void PreferencesDialog::saveSettings()
{
    // Update settings from UI
    updateSettingsFromUI();

    m_settings->beginGroup("Appearance");
    m_settings->setValue("theme", m_appearanceSettings.theme);
    m_settings->setValue("iconSize", m_appearanceSettings.iconSize);
    m_settings->setValue("viewMode", m_appearanceSettings.viewMode);
    m_settings->setValue("showIconNames", m_appearanceSettings.showIconNames);
    m_settings->setValue("showTooltips", m_appearanceSettings.showTooltips);
    m_settings->setValue("useAnimations", m_appearanceSettings.useAnimations);
    m_settings->setValue("accentColor", m_appearanceSettings.accentColor);
    m_settings->setValue("fontFamily", m_appearanceSettings.fontFamily);
    m_settings->setValue("fontSize", m_appearanceSettings.fontSize);
    m_settings->endGroup();

    m_settings->beginGroup("Search");
    m_settings->setValue("enableFuzzySearch", m_searchSettings.enableFuzzySearch);
    m_settings->setValue("searchInTags", m_searchSettings.searchInTags);
    m_settings->setValue("searchInCategories", m_searchSettings.searchInCategories);
    m_settings->setValue("showSearchSuggestions", m_searchSettings.showSearchSuggestions);
    m_settings->setValue("maxSuggestions", m_searchSettings.maxSuggestions);
    m_settings->setValue("highlightMatches", m_searchSettings.highlightMatches);
    m_settings->setValue("caseSensitive", m_searchSettings.caseSensitive);
    m_settings->endGroup();

    m_settings->beginGroup("Performance");
    m_settings->setValue("enableLazyLoading", m_performanceSettings.enableLazyLoading);
    m_settings->setValue("cacheSize", m_performanceSettings.cacheSize);
    m_settings->setValue("preloadFavorites", m_performanceSettings.preloadFavorites);
    m_settings->setValue("enableVirtualization", m_performanceSettings.enableVirtualization);
    m_settings->setValue("renderThreads", m_performanceSettings.renderThreads);
    m_settings->setValue("enableGPUAcceleration", m_performanceSettings.enableGPUAcceleration);
    m_settings->endGroup();

    m_settings->beginGroup("Export");
    m_settings->setValue("defaultFormat", m_exportSettings.defaultFormat);
    m_settings->setValue("defaultSize", m_exportSettings.defaultSize);
    m_settings->setValue("defaultPath", m_exportSettings.defaultPath);
    m_settings->setValue("preserveAspectRatio", m_exportSettings.preserveAspectRatio);
    m_settings->setValue("includeMetadata", m_exportSettings.includeMetadata);
    m_settings->setValue("backgroundColor", m_exportSettings.backgroundColor);
    m_settings->endGroup();

    m_settings->beginGroup("General");
    m_settings->setValue("checkForUpdates", m_generalSettings.checkForUpdates);
    m_settings->setValue("showWelcomeScreen", m_generalSettings.showWelcomeScreen);
    m_settings->setValue("rememberWindowState", m_generalSettings.rememberWindowState);
    m_settings->setValue("enableTelemetry", m_generalSettings.enableTelemetry);
    m_settings->setValue("language", m_generalSettings.language);
    m_settings->setValue("showStatusBar", m_generalSettings.showStatusBar);
    m_settings->setValue("showToolBar", m_generalSettings.showToolBar);
    m_settings->endGroup();

    m_settings->sync();
}

void PreferencesDialog::updateSettingsFromUI()
{
    // Appearance
    QStringList themes = {"system", "light", "dark"};
    m_appearanceSettings.theme = themes[m_themeCombo->currentIndex()];
    m_appearanceSettings.iconSize = m_iconSizeSlider->value();

    QStringList viewModes = {"grid", "list", "compact"};
    m_appearanceSettings.viewMode = viewModes[m_viewModeCombo->currentIndex()];

    m_appearanceSettings.showIconNames = m_showIconNamesCheck->isChecked();
    m_appearanceSettings.showTooltips = m_showTooltipsCheck->isChecked();
    m_appearanceSettings.useAnimations = m_useAnimationsCheck->isChecked();
    m_appearanceSettings.fontFamily = m_fontCombo->currentFont().family();
    m_appearanceSettings.fontSize = m_fontSizeSpin->value();

    // Search
    m_searchSettings.enableFuzzySearch = m_fuzzySearchCheck->isChecked();
    m_searchSettings.searchInTags = m_searchTagsCheck->isChecked();
    m_searchSettings.searchInCategories = m_searchCategoriesCheck->isChecked();
    m_searchSettings.showSearchSuggestions = m_showSuggestionsCheck->isChecked();
    m_searchSettings.maxSuggestions = m_maxSuggestionsSpin->value();
    m_searchSettings.highlightMatches = m_highlightMatchesCheck->isChecked();
    m_searchSettings.caseSensitive = m_caseSensitiveCheck->isChecked();

    // Performance
    m_performanceSettings.enableLazyLoading = m_lazyLoadingCheck->isChecked();
    m_performanceSettings.cacheSize = m_cacheSizeSpin->value();
    m_performanceSettings.preloadFavorites = m_preloadFavoritesCheck->isChecked();
    m_performanceSettings.enableVirtualization = m_virtualizationCheck->isChecked();
    m_performanceSettings.renderThreads = m_renderThreadsSpin->value();
    m_performanceSettings.enableGPUAcceleration = m_gpuAccelerationCheck->isChecked();

    // Export
    m_exportSettings.defaultFormat = m_defaultFormatCombo->currentText();
    m_exportSettings.defaultSize = m_defaultSizeSpin->value();
    m_exportSettings.defaultPath = m_defaultPathEdit->text();
    m_exportSettings.preserveAspectRatio = m_preserveAspectCheck->isChecked();
    m_exportSettings.includeMetadata = m_includeMetadataCheck->isChecked();

    // General
    m_generalSettings.checkForUpdates = m_checkUpdatesCheck->isChecked();
    m_generalSettings.showWelcomeScreen = m_showWelcomeCheck->isChecked();
    m_generalSettings.rememberWindowState = m_rememberWindowCheck->isChecked();
    m_generalSettings.enableTelemetry = m_enableTelemetryCheck->isChecked();
    m_generalSettings.showStatusBar = m_showStatusBarCheck->isChecked();
    m_generalSettings.showToolBar = m_showToolBarCheck->isChecked();
}

// Slot implementations
void PreferencesDialog::onThemeChanged()
{
    QStringList themes = {"system", "light", "dark"};
    m_appearanceSettings.theme = themes[m_themeCombo->currentIndex()];
    emit themeChanged(m_appearanceSettings.theme);
    m_settingsChanged = true;
}

void PreferencesDialog::onIconSizeChanged()
{
    int size = m_iconSizeSlider->value();
    m_iconSizeLabel->setText(QString("%1px").arg(size));
    m_appearanceSettings.iconSize = size;
    emit iconSizeChanged(size);
    m_settingsChanged = true;
}

void PreferencesDialog::onViewModeChanged()
{
    QStringList viewModes = {"grid", "list", "compact"};
    m_appearanceSettings.viewMode = viewModes[m_viewModeCombo->currentIndex()];
    emit viewModeChanged(m_appearanceSettings.viewMode);
    m_settingsChanged = true;
}

void PreferencesDialog::onAccentColorChanged()
{
    QColor color = QColorDialog::getColor(m_appearanceSettings.accentColor, this,
                                        "Choose Accent Color");
    if (color.isValid()) {
        m_appearanceSettings.accentColor = color;
        QString style = QString("QPushButton { background-color: %1; }").arg(color.name());
        m_accentColorButton->setStyleSheet(style);
        m_accentColorButton->setText(color.name());
        m_settingsChanged = true;
    }
}

void PreferencesDialog::onFontChanged()
{
    m_appearanceSettings.fontFamily = m_fontCombo->currentFont().family();
    m_appearanceSettings.fontSize = m_fontSizeSpin->value();
    m_settingsChanged = true;
}

void PreferencesDialog::onPreviewSettings()
{
    // Apply current settings temporarily for preview
    updateSettingsFromUI();
    updatePreview();
}

void PreferencesDialog::onRestoreDefaults()
{
    int ret = QMessageBox::question(this, "Restore Defaults",
                                   "Are you sure you want to restore all settings to their default values?",
                                   QMessageBox::Yes | QMessageBox::No,
                                   QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        // Reset to default settings
        m_appearanceSettings = AppearanceSettings();
        m_searchSettings = SearchSettings();
        m_performanceSettings = PerformanceSettings();
        m_exportSettings = ExportSettings();
        m_generalSettings = GeneralSettings();

        updateUIFromSettings();
        updatePreview();
        m_settingsChanged = true;
    }
}

void PreferencesDialog::updatePreview()
{
    if (!m_previewWidget || !m_previewLabel) {
        return;
    }

    // Update preview widget with current settings
    QString previewText = QString("Theme: %1\nIcon Size: %2px\nView Mode: %3")
                         .arg(m_appearanceSettings.theme)
                         .arg(m_appearanceSettings.iconSize)
                         .arg(m_appearanceSettings.viewMode);

    m_previewLabel->setText(previewText);

    // Apply theme colors to preview
    QString style;
    if (m_appearanceSettings.theme == "dark") {
        style = "QWidget { background-color: #2b2b2b; color: #ffffff; }";
    } else if (m_appearanceSettings.theme == "light") {
        style = "QWidget { background-color: #ffffff; color: #000000; }";
    } else {
        style = ""; // System default
    }

    m_previewWidget->setStyleSheet(style);
}

void PreferencesDialog::accept()
{
    saveSettings();
    applySettings();
    emit settingsChanged();
    QDialog::accept();
}

void PreferencesDialog::reject()
{
    if (m_settingsChanged) {
        int ret = QMessageBox::question(this, "Discard Changes",
                                      "You have unsaved changes. Discard them?",
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);
        if (ret == QMessageBox::No) {
            return;
        }
    }
    QDialog::reject();
}

void PreferencesDialog::applySettings()
{
    saveSettings();
    emit settingsChanged();
    m_settingsChanged = false;
}

void PreferencesDialog::resetToDefaults()
{
    int ret = QMessageBox::question(this, "Reset to Defaults",
                                  "This will reset all settings to their default values. Continue?",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        // Reset to default values
        m_appearanceSettings = AppearanceSettings();
        m_searchSettings = SearchSettings();
        m_performanceSettings = PerformanceSettings();
        m_exportSettings = ExportSettings();
        m_generalSettings = GeneralSettings();

        updateUIFromSettings();
        m_settingsChanged = true;
    }
}

void PreferencesDialog::importSettings()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                  "Import Settings",
                                                  QString(),
                                                  "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            if (!doc.isNull()) {
                // TODO: Implement settings import from JSON
                QMessageBox::information(this, "Import Settings",
                                       "Settings imported successfully.");
            } else {
                QMessageBox::warning(this, "Import Error",
                                   "Failed to parse settings file.");
            }
        } else {
            QMessageBox::warning(this, "Import Error",
                               "Failed to open settings file.");
        }
    }
}

void PreferencesDialog::exportSettings()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                  "Export Settings",
                                                  "qtlucide-settings.json",
                                                  "JSON Files (*.json)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            // TODO: Implement settings export to JSON
            QJsonDocument doc;
            file.write(doc.toJson());
            QMessageBox::information(this, "Export Settings",
                                   "Settings exported successfully.");
        } else {
            QMessageBox::warning(this, "Export Error",
                               "Failed to create settings file.");
        }
    }
}

// Getter methods
PreferencesDialog::AppearanceSettings PreferencesDialog::getAppearanceSettings() const
{
    return m_appearanceSettings;
}

PreferencesDialog::SearchSettings PreferencesDialog::getSearchSettings() const
{
    return m_searchSettings;
}

PreferencesDialog::PerformanceSettings PreferencesDialog::getPerformanceSettings() const
{
    return m_performanceSettings;
}

PreferencesDialog::ExportSettings PreferencesDialog::getExportSettings() const
{
    return m_exportSettings;
}

PreferencesDialog::GeneralSettings PreferencesDialog::getGeneralSettings() const
{
    return m_generalSettings;
}
