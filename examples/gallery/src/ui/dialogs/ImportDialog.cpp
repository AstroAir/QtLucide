/**
 * QtLucide Gallery Application - Import Dialog Implementation
 */

#include "ImportDialog.h"
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QShowEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QDebug>

// Static constants for SettingsManager
const QString SettingsManager::EXPORT_VERSION = "2.1.0";
const QString SettingsManager::EXPORT_FORMAT_VERSION = "1.0";

// ImportOptionsWidget Implementation
ImportOptionsWidget::ImportOptionsWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_fileTab(nullptr)
    , m_dataTab(nullptr)
    , m_optionsTab(nullptr)
{
    setupUI();

    // Create theme-aware widget
    m_themeWidget = new ThemeAwareWidget(this);

    // Set default configuration
    resetToDefaults();

    // Apply initial theme
    applyTheme();

    // Enable drag and drop
    setAcceptDrops(true);
}

ImportOptionsWidget::~ImportOptionsWidget()
{
}

void ImportOptionsWidget::setImportConfig(const ImportConfig& config)
{
    if (m_config.sourceFile == config.sourceFile &&
        m_config.dataTypes == config.dataTypes &&
        m_config.conflictResolution == config.conflictResolution) {
        return; // No changes
    }

    m_config = config;

    // Update UI to reflect new config
    updateDataTypeOptions();
    updateConflictOptions();
    updateBackupOptions();

    emit configChanged(m_config);
}

ImportConfig ImportOptionsWidget::importConfig() const
{
    return m_config;
}

void ImportOptionsWidget::resetToDefaults()
{
    ImportConfig defaultConfig;
    defaultConfig.backupDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/QtLucide/Backups";
    setImportConfig(defaultConfig);
}

void ImportOptionsWidget::setSourceFile(const QString& filePath)
{
    if (m_config.sourceFile == filePath) return;

    m_config.sourceFile = filePath;
    m_sourceFileEdit->setText(filePath);

    // Update file info
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        QString info = QString("File: %1\nSize: %2 bytes\nModified: %3")
                      .arg(fileInfo.fileName())
                      .arg(fileInfo.size())
                      .arg(fileInfo.lastModified().toString());
        m_fileInfoLabel->setText(info);
    } else {
        m_fileInfoLabel->setText("File not found");
    }

    emit sourceFileChanged(filePath);
    emit configChanged(m_config);
}

QString ImportOptionsWidget::sourceFile() const
{
    return m_config.sourceFile;
}

bool ImportOptionsWidget::validateConfig() const
{
    // Check if source file is valid
    if (m_config.sourceFile.isEmpty()) {
        return false;
    }

    QFileInfo fileInfo(m_config.sourceFile);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        return false;
    }

    // Check if at least one data type is selected
    if (m_config.dataTypes.isEmpty()) {
        return false;
    }

    // Check backup directory if backup is enabled
    if (m_config.createBackup) {
        if (m_config.backupDirectory.isEmpty()) {
            return false;
        }

        QDir backupDir(m_config.backupDirectory);
        if (!backupDir.exists() && !backupDir.mkpath(".")) {
            return false;
        }
    }

    return true;
}

QStringList ImportOptionsWidget::getValidationErrors() const
{
    QStringList errors;

    if (m_config.sourceFile.isEmpty()) {
        errors << "Source file is not specified";
    } else {
        QFileInfo fileInfo(m_config.sourceFile);
        if (!fileInfo.exists()) {
            errors << "Source file does not exist";
        } else if (!fileInfo.isReadable()) {
            errors << "Source file is not readable";
        }
    }

    if (m_config.dataTypes.isEmpty()) {
        errors << "No data types selected for import";
    }

    if (m_config.createBackup) {
        if (m_config.backupDirectory.isEmpty()) {
            errors << "Backup directory is not specified";
        } else {
            QDir backupDir(m_config.backupDirectory);
            if (!backupDir.exists() && !backupDir.mkpath(".")) {
                errors << "Cannot create backup directory";
            }
        }
    }

    return errors;
}

void ImportOptionsWidget::applyTheme()
{
    if (!ThemeManager::instance()) return;

    // Apply theme to the options widget
    QString optionsStyle = QString(
        "ImportOptionsWidget { "
        "    background-color: %1; "
        "    border: none; "
        "} "
        "QTabWidget::pane { "
        "    border: 1px solid %2; "
        "    border-radius: 4px; "
        "    background-color: %3; "
        "} "
        "QTabBar::tab { "
        "    background-color: %4; "
        "    border: 1px solid %5; "
        "    padding: 8px 16px; "
        "    margin-right: 2px; "
        "} "
        "QTabBar::tab:selected { "
        "    background-color: %6; "
        "    border-bottom-color: %6; "
        "} "
        "QGroupBox { "
        "    font-weight: bold; "
        "    border: 1px solid %7; "
        "    border-radius: 4px; "
        "    margin-top: 8px; "
        "    padding-top: 4px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 8px; "
        "    padding: 0 4px 0 4px; "
        "}"
    ).arg(
        THEME_COLOR(WindowBackground).name(),
        THEME_COLOR(BorderColor).name(),
        THEME_COLOR(PanelBackground).name(),
        THEME_COLOR(PanelBackground).name(),
        THEME_COLOR(BorderColor).name(),
        THEME_COLOR(WindowBackground).name(),
        THEME_COLOR(BorderColor).name()
    );

    setStyleSheet(optionsStyle);
}

void ImportOptionsWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            QFileInfo fileInfo(filePath);
            if (fileInfo.suffix().toLower() == "json" || fileInfo.suffix().toLower() == "qtlucide") {
                event->acceptProposedAction();
                return;
            }
        }
    }
    event->ignore();
}

void ImportOptionsWidget::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            setSourceFile(filePath);
            event->acceptProposedAction();
            return;
        }
    }
    event->ignore();
}

void ImportOptionsWidget::onBrowseSourceFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Select Import File",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        "QtLucide Settings (*.qtlucide);;JSON Files (*.json);;All Files (*.*)"
    );

    if (!filePath.isEmpty()) {
        setSourceFile(filePath);
    }
}

void ImportOptionsWidget::onDataTypeChanged()
{
    // Update config based on checkboxes
    m_config.dataTypes.clear();

    if (m_allDataCheck->isChecked()) {
        m_config.dataTypes << ImportDataType::All;
    } else {
        if (m_settingsCheck->isChecked()) m_config.dataTypes << ImportDataType::Settings;
        if (m_themesCheck->isChecked()) m_config.dataTypes << ImportDataType::Themes;
        if (m_favoritesCheck->isChecked()) m_config.dataTypes << ImportDataType::Favorites;
        if (m_searchHistoryCheck->isChecked()) m_config.dataTypes << ImportDataType::SearchHistory;
        if (m_windowLayoutCheck->isChecked()) m_config.dataTypes << ImportDataType::WindowLayout;
        if (m_userPreferencesCheck->isChecked()) m_config.dataTypes << ImportDataType::UserPreferences;
    }

    emit dataTypesChanged(m_config.dataTypes);
    emit configChanged(m_config);
}

void ImportOptionsWidget::onConflictResolutionChanged()
{
    int checkedId = m_conflictButtonGroup->checkedId();
    m_config.conflictResolution = static_cast<ImportConfig::ConflictResolution>(checkedId);
    emit configChanged(m_config);
}

void ImportOptionsWidget::onBackupSettingsChanged()
{
    m_config.createBackup = m_createBackupCheck->isChecked();
    m_config.backupDirectory = m_backupDirectoryEdit->text();

    m_backupDirectoryEdit->setEnabled(m_config.createBackup);
    m_browseBackupButton->setEnabled(m_config.createBackup);

    emit configChanged(m_config);
}

void ImportOptionsWidget::onValidationSettingsChanged()
{
    m_config.validateBeforeImport = m_validateBeforeImportCheck->isChecked();
    m_config.strictValidation = m_strictValidationCheck->isChecked();
    m_config.skipInvalidEntries = m_skipInvalidEntriesCheck->isChecked();

    m_strictValidationCheck->setEnabled(m_config.validateBeforeImport);
    m_skipInvalidEntriesCheck->setEnabled(m_config.validateBeforeImport);

    emit configChanged(m_config);
}

void ImportOptionsWidget::setupUI()
{
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    m_mainLayout->addWidget(m_tabWidget);

    // Setup tabs
    setupFileSection();
    setupDataTypeSection();
    setupOptionsSection();
}

void ImportOptionsWidget::setupFileSection()
{
    m_fileTab = new QWidget();
    m_fileLayout = new QVBoxLayout(m_fileTab);
    m_fileLayout->setContentsMargins(12, 12, 12, 12);
    m_fileLayout->setSpacing(8);

    // File selection group
    m_fileGroup = new QGroupBox("Import File", m_fileTab);
    QVBoxLayout* fileGroupLayout = new QVBoxLayout(m_fileGroup);
    fileGroupLayout->setSpacing(6);

    QHBoxLayout* fileLayout = new QHBoxLayout();
    m_sourceFileEdit = new QLineEdit(m_fileGroup);
    m_sourceFileEdit->setPlaceholderText("Select import file or drag and drop here...");
    m_browseButton = new QPushButton("Browse...", m_fileGroup);

    fileLayout->addWidget(m_sourceFileEdit);
    fileLayout->addWidget(m_browseButton);
    fileGroupLayout->addLayout(fileLayout);

    m_fileInfoLabel = new QLabel("No file selected", m_fileGroup);
    m_fileInfoLabel->setStyleSheet("color: gray; font-style: italic;");
    fileGroupLayout->addWidget(m_fileInfoLabel);

    m_fileLayout->addWidget(m_fileGroup);
    m_fileLayout->addStretch();

    // Connect signals
    connect(m_browseButton, &QPushButton::clicked, this, &ImportOptionsWidget::onBrowseSourceFile);
    connect(m_sourceFileEdit, &QLineEdit::textChanged, this, &ImportOptionsWidget::setSourceFile);

    m_tabWidget->addTab(m_fileTab, "File");
}

void ImportOptionsWidget::setupDataTypeSection()
{
    m_dataTab = new QWidget();
    m_dataLayout = new QVBoxLayout(m_dataTab);
    m_dataLayout->setContentsMargins(12, 12, 12, 12);
    m_dataLayout->setSpacing(8);

    // Data type selection group
    m_dataTypeGroup = new QGroupBox("Data Types to Import", m_dataTab);
    QVBoxLayout* dataTypeLayout = new QVBoxLayout(m_dataTypeGroup);
    dataTypeLayout->setSpacing(6);

    m_allDataCheck = new QCheckBox("Import all data", m_dataTypeGroup);
    m_allDataCheck->setChecked(true);
    dataTypeLayout->addWidget(m_allDataCheck);

    dataTypeLayout->addWidget(new QLabel("Or select specific data types:", m_dataTypeGroup));

    m_settingsCheck = new QCheckBox("Application Settings", m_dataTypeGroup);
    m_themesCheck = new QCheckBox("Themes and Appearance", m_dataTypeGroup);
    m_favoritesCheck = new QCheckBox("Favorites and Collections", m_dataTypeGroup);
    m_searchHistoryCheck = new QCheckBox("Search History", m_dataTypeGroup);
    m_windowLayoutCheck = new QCheckBox("Window Layout", m_dataTypeGroup);
    m_userPreferencesCheck = new QCheckBox("User Preferences", m_dataTypeGroup);

    // Initially disabled when "all data" is checked
    m_settingsCheck->setEnabled(false);
    m_themesCheck->setEnabled(false);
    m_favoritesCheck->setEnabled(false);
    m_searchHistoryCheck->setEnabled(false);
    m_windowLayoutCheck->setEnabled(false);
    m_userPreferencesCheck->setEnabled(false);

    dataTypeLayout->addWidget(m_settingsCheck);
    dataTypeLayout->addWidget(m_themesCheck);
    dataTypeLayout->addWidget(m_favoritesCheck);
    dataTypeLayout->addWidget(m_searchHistoryCheck);
    dataTypeLayout->addWidget(m_windowLayoutCheck);
    dataTypeLayout->addWidget(m_userPreferencesCheck);

    m_dataLayout->addWidget(m_dataTypeGroup);
    m_dataLayout->addStretch();

    // Connect signals
    connect(m_allDataCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_settingsCheck->setEnabled(!checked);
        m_themesCheck->setEnabled(!checked);
        m_favoritesCheck->setEnabled(!checked);
        m_searchHistoryCheck->setEnabled(!checked);
        m_windowLayoutCheck->setEnabled(!checked);
        m_userPreferencesCheck->setEnabled(!checked);
        onDataTypeChanged();
    });

    connect(m_settingsCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onDataTypeChanged);
    connect(m_themesCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onDataTypeChanged);
    connect(m_favoritesCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onDataTypeChanged);
    connect(m_searchHistoryCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onDataTypeChanged);
    connect(m_windowLayoutCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onDataTypeChanged);
    connect(m_userPreferencesCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onDataTypeChanged);

    m_tabWidget->addTab(m_dataTab, "Data Types");
}

void ImportOptionsWidget::setupOptionsSection()
{
    m_optionsTab = new QWidget();
    m_optionsLayout = new QVBoxLayout(m_optionsTab);
    m_optionsLayout->setContentsMargins(12, 12, 12, 12);
    m_optionsLayout->setSpacing(8);

    // Conflict resolution group
    m_conflictGroup = new QGroupBox("Conflict Resolution", m_optionsTab);
    QVBoxLayout* conflictLayout = new QVBoxLayout(m_conflictGroup);

    m_conflictButtonGroup = new QButtonGroup(this);

    m_askRadio = new QRadioButton("Ask for each conflict", m_conflictGroup);
    m_askRadio->setChecked(true);
    m_overwriteRadio = new QRadioButton("Overwrite existing data", m_conflictGroup);
    m_keepExistingRadio = new QRadioButton("Keep existing data", m_conflictGroup);
    m_mergeRadio = new QRadioButton("Merge data when possible", m_conflictGroup);

    m_conflictButtonGroup->addButton(m_askRadio, static_cast<int>(ImportConfig::Ask));
    m_conflictButtonGroup->addButton(m_overwriteRadio, static_cast<int>(ImportConfig::OverwriteExisting));
    m_conflictButtonGroup->addButton(m_keepExistingRadio, static_cast<int>(ImportConfig::KeepExisting));
    m_conflictButtonGroup->addButton(m_mergeRadio, static_cast<int>(ImportConfig::MergeData));

    conflictLayout->addWidget(m_askRadio);
    conflictLayout->addWidget(m_overwriteRadio);
    conflictLayout->addWidget(m_keepExistingRadio);
    conflictLayout->addWidget(m_mergeRadio);

    m_optionsLayout->addWidget(m_conflictGroup);

    // Backup group
    m_backupGroup = new QGroupBox("Backup", m_optionsTab);
    QVBoxLayout* backupLayout = new QVBoxLayout(m_backupGroup);

    m_createBackupCheck = new QCheckBox("Create backup before import", m_backupGroup);
    m_createBackupCheck->setChecked(true);
    backupLayout->addWidget(m_createBackupCheck);

    QHBoxLayout* backupDirLayout = new QHBoxLayout();
    QLabel* backupDirLabel = new QLabel("Backup directory:", m_backupGroup);
    m_backupDirectoryEdit = new QLineEdit(m_backupGroup);
    m_browseBackupButton = new QPushButton("Browse...", m_backupGroup);

    backupDirLayout->addWidget(backupDirLabel);
    backupDirLayout->addWidget(m_backupDirectoryEdit);
    backupDirLayout->addWidget(m_browseBackupButton);
    backupLayout->addLayout(backupDirLayout);

    m_optionsLayout->addWidget(m_backupGroup);

    // Validation group
    m_validationGroup = new QGroupBox("Validation", m_optionsTab);
    QVBoxLayout* validationLayout = new QVBoxLayout(m_validationGroup);

    m_validateBeforeImportCheck = new QCheckBox("Validate data before import", m_validationGroup);
    m_validateBeforeImportCheck->setChecked(true);
    m_strictValidationCheck = new QCheckBox("Use strict validation", m_validationGroup);
    m_skipInvalidEntriesCheck = new QCheckBox("Skip invalid entries", m_validationGroup);
    m_skipInvalidEntriesCheck->setChecked(true);

    validationLayout->addWidget(m_validateBeforeImportCheck);
    validationLayout->addWidget(m_strictValidationCheck);
    validationLayout->addWidget(m_skipInvalidEntriesCheck);

    m_optionsLayout->addWidget(m_validationGroup);
    m_optionsLayout->addStretch();

    // Connect signals
    connect(m_conflictButtonGroup, &QButtonGroup::idClicked,
            this, &ImportOptionsWidget::onConflictResolutionChanged);
    connect(m_createBackupCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onBackupSettingsChanged);
    connect(m_backupDirectoryEdit, &QLineEdit::textChanged, this, &ImportOptionsWidget::onBackupSettingsChanged);
    connect(m_browseBackupButton, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Backup Directory", m_backupDirectoryEdit->text());
        if (!dir.isEmpty()) {
            m_backupDirectoryEdit->setText(dir);
        }
    });
    connect(m_validateBeforeImportCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onValidationSettingsChanged);
    connect(m_strictValidationCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onValidationSettingsChanged);
    connect(m_skipInvalidEntriesCheck, &QCheckBox::toggled, this, &ImportOptionsWidget::onValidationSettingsChanged);

    m_tabWidget->addTab(m_optionsTab, "Options");
}

void ImportOptionsWidget::updateDataTypeOptions()
{
    // Update checkboxes based on config
    bool hasAll = m_config.dataTypes.contains(ImportDataType::All);
    m_allDataCheck->setChecked(hasAll);

    if (!hasAll) {
        m_settingsCheck->setChecked(m_config.dataTypes.contains(ImportDataType::Settings));
        m_themesCheck->setChecked(m_config.dataTypes.contains(ImportDataType::Themes));
        m_favoritesCheck->setChecked(m_config.dataTypes.contains(ImportDataType::Favorites));
        m_searchHistoryCheck->setChecked(m_config.dataTypes.contains(ImportDataType::SearchHistory));
        m_windowLayoutCheck->setChecked(m_config.dataTypes.contains(ImportDataType::WindowLayout));
        m_userPreferencesCheck->setChecked(m_config.dataTypes.contains(ImportDataType::UserPreferences));
    }
}

void ImportOptionsWidget::updateConflictOptions()
{
    QAbstractButton* button = m_conflictButtonGroup->button(static_cast<int>(m_config.conflictResolution));
    if (button) {
        button->setChecked(true);
    }
}

void ImportOptionsWidget::updateBackupOptions()
{
    m_createBackupCheck->setChecked(m_config.createBackup);
    m_backupDirectoryEdit->setText(m_config.backupDirectory);
    m_backupDirectoryEdit->setEnabled(m_config.createBackup);
    m_browseBackupButton->setEnabled(m_config.createBackup);
}
