/**
 * QtLucide Gallery Application - Import Dialog
 *
 * A comprehensive import dialog for importing settings and configurations:
 * - Settings import/export (themes, preferences, layouts)
 * - Favorites and collections import/export
 * - Search history import/export
 * - Configuration validation and migration
 * - Backup and restore functionality
 * - Selective import options
 * - Preview before import
 * - Theme-aware styling
 */

#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSettings>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "../../core/managers/ManagerStubs.h"
#include "../themes/ThemeManager.h"

// Forward declarations
class ImportOptionsWidget;
class ImportPreviewWidget;

/**
 * @brief Import data types
 */
enum class ImportDataType {
    All = 0,
    Settings = 1,
    Themes = 2,
    Favorites = 3,
    SearchHistory = 4,
    WindowLayout = 5,
    UserPreferences = 6
};

/**
 * @brief Import configuration
 */
struct ImportConfig {
    QString sourceFile;
    QList<ImportDataType> dataTypes;
    bool mergeWithExisting = true;
    bool createBackup = true;
    bool validateBeforeImport = true;
    QString backupDirectory;

    // Conflict resolution
    enum ConflictResolution {
        Ask = 0,
        OverwriteExisting = 1,
        KeepExisting = 2,
        MergeData = 3
    } conflictResolution = Ask;

    // Validation options
    bool strictValidation = false;
    bool skipInvalidEntries = true;
};

/**
 * @brief Import data structure
 */
struct ImportData {
    QString version;
    QDateTime exportedAt;
    QString exportedFrom;

    // Data sections
    QJsonObject settings;
    QJsonObject themes;
    QJsonArray favorites;
    QJsonArray searchHistory;
    QJsonObject windowLayout;
    QJsonObject userPreferences;

    // Metadata
    QJsonObject metadata;
    QString checksum;
    bool isValid = false;
    QStringList validationErrors;
};

/**
 * @brief Import options widget
 */
class ImportOptionsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImportOptionsWidget(QWidget* parent = nullptr);
    ~ImportOptionsWidget();

    // Configuration
    void setImportConfig(const ImportConfig& config);
    ImportConfig importConfig() const;
    void resetToDefaults();

    // File handling
    void setSourceFile(const QString& filePath);
    QString sourceFile() const;

    // Validation
    bool validateConfig() const;
    QStringList getValidationErrors() const;

    // Theme integration
    void applyTheme();

signals:
    void configChanged(const ImportConfig& config);
    void sourceFileChanged(const QString& filePath);
    void dataTypesChanged(const QList<ImportDataType>& types);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onBrowseSourceFile();
    void onDataTypeChanged();
    void onConflictResolutionChanged();
    void onBackupSettingsChanged();
    void onValidationSettingsChanged();

private:
    void setupUI();
    void setupFileSection();
    void setupDataTypeSection();
    void setupOptionsSection();
    void updateDataTypeOptions();
    void updateConflictOptions();
    void updateBackupOptions();

    // UI components
    QVBoxLayout* m_mainLayout;
    QTabWidget* m_tabWidget;

    // File tab
    QWidget* m_fileTab;
    QVBoxLayout* m_fileLayout;
    QGroupBox* m_fileGroup;
    QLineEdit* m_sourceFileEdit;
    QPushButton* m_browseButton;
    QLabel* m_fileInfoLabel;

    // Data types tab
    QWidget* m_dataTab;
    QVBoxLayout* m_dataLayout;
    QGroupBox* m_dataTypeGroup;
    QCheckBox* m_allDataCheck;
    QCheckBox* m_settingsCheck;
    QCheckBox* m_themesCheck;
    QCheckBox* m_favoritesCheck;
    QCheckBox* m_searchHistoryCheck;
    QCheckBox* m_windowLayoutCheck;
    QCheckBox* m_userPreferencesCheck;

    // Options tab
    QWidget* m_optionsTab;
    QVBoxLayout* m_optionsLayout;
    QGroupBox* m_conflictGroup;
    QButtonGroup* m_conflictButtonGroup;
    QRadioButton* m_askRadio;
    QRadioButton* m_overwriteRadio;
    QRadioButton* m_keepExistingRadio;
    QRadioButton* m_mergeRadio;
    QGroupBox* m_backupGroup;
    QCheckBox* m_createBackupCheck;
    QLineEdit* m_backupDirectoryEdit;
    QPushButton* m_browseBackupButton;
    QGroupBox* m_validationGroup;
    QCheckBox* m_validateBeforeImportCheck;
    QCheckBox* m_strictValidationCheck;
    QCheckBox* m_skipInvalidEntriesCheck;

    // Data
    ImportConfig m_config;

    // Theme
    ThemeAwareWidget* m_themeWidget;
};

/**
 * @brief Import preview widget
 */
class ImportPreviewWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImportPreviewWidget(QWidget* parent = nullptr);
    ~ImportPreviewWidget();

    // Preview management
    void setImportData(const ImportData& data);
    ImportData importData() const { return m_importData; }
    void clearPreview();
    void refreshPreview();

    // Display options
    void setShowDetails(bool show);
    bool showDetails() const { return m_showDetails; }

signals:
    void dataItemClicked(ImportDataType type, const QString& key);
    void validationStatusChanged(bool isValid, const QStringList& errors);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onShowDetailsToggled(bool show);

private:
    void setupUI();
    void populateTree();
    void addDataTypeNode(const QString& title, ImportDataType type, const QJsonValue& data);
    void addJsonObjectToTree(QTreeWidgetItem* parent, const QJsonObject& obj);
    void addJsonArrayToTree(QTreeWidgetItem* parent, const QJsonArray& array);
    QString formatValue(const QJsonValue& value) const;
    QIcon getDataTypeIcon(ImportDataType type) const;

    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_headerLayout;
    QLabel* m_titleLabel;
    QCheckBox* m_showDetailsCheck;
    QTreeWidget* m_previewTree;
    QTextEdit* m_detailsText;
    QLabel* m_statusLabel;

    // Data
    ImportData m_importData;
    bool m_showDetails;

    // Theme
    ThemeAwareWidget* m_themeWidget;
};

/**
 * @brief Main import dialog
 */
class ImportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImportDialog(QWidget* parent = nullptr);
    ~ImportDialog();

    // Configuration
    void setImportConfig(const ImportConfig& config);
    ImportConfig importConfig() const;
    void setSourceFile(const QString& filePath);
    QString sourceFile() const;

    // Integration
    void setThemeManager(ThemeManager* themeManager);
    void setSettingsManager(SettingsManager* settingsManager);

    // Dialog result
    bool wasImportSuccessful() const { return m_importSuccessful; }
    QStringList getImportedDataTypes() const { return m_importedDataTypes; }

public slots:
    void accept() override;
    void reject() override;

signals:
    void importStarted();
    void importProgress(const QString& currentItem);
    void importFinished(bool success, const QStringList& importedTypes);
    void importCancelled();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    void onImportClicked();
    void onCancelClicked();
    void onConfigChanged(const ImportConfig& config);
    void onSourceFileChanged(const QString& filePath);
    void onPreviewDataItem(ImportDataType type, const QString& key);
    void loadAndPreviewFile();

private:
    void setupUI();
    void setupLayout();
    void setupConnections();
    void updateUI();
    void startImport();
    void cancelImport();
    bool validateImportConfig();
    bool loadImportData(const QString& filePath);
    bool performImport();
    bool createBackup();
    bool importDataType(ImportDataType type, const QJsonValue& data);
    void applyTheme();

    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_contentLayout;
    QVBoxLayout* m_leftLayout;
    QVBoxLayout* m_rightLayout;

    QLabel* m_titleLabel;
    QLabel* m_sourceLabel;
    ImportOptionsWidget* m_optionsWidget;
    ImportPreviewWidget* m_previewWidget;

    QHBoxLayout* m_buttonLayout;
    QPushButton* m_importButton;
    QPushButton* m_cancelButton;
    QPushButton* m_closeButton;

    // Data
    ImportConfig m_config;
    ImportData m_importData;
    QString m_sourceFile;
    bool m_importInProgress;
    bool m_importSuccessful;
    QStringList m_importedDataTypes;

    // Integration
    ThemeManager* m_themeManager;
    SettingsManager* m_settingsManager;

    // Theme
    ThemeAwareWidget* m_themeWidget;
};

/**
 * @brief Settings manager for handling import/export
 */
class ImportSettingsManager : public QObject {
    Q_OBJECT

public:
    explicit ImportSettingsManager(QObject* parent = nullptr);
    ~ImportSettingsManager();

    // Export functionality
    bool exportSettings(const QString& filePath, const QList<ImportDataType>& dataTypes);
    ImportData createExportData(const QList<ImportDataType>& dataTypes);

    // Import functionality
    bool importSettings(const ImportData& data, const ImportConfig& config);
    ImportData loadImportData(const QString& filePath);

    // Validation
    bool validateImportData(const ImportData& data, bool strict = false);
    QStringList getValidationErrors(const ImportData& data) const;

    // Backup
    bool createBackup(const QString& backupDirectory);
    bool restoreBackup(const QString& backupFile);

    // Integration
    void setThemeManager(ThemeManager* themeManager) { m_themeManager = themeManager; }

signals:
    void exportProgress(const QString& currentItem);
    void importProgress(const QString& currentItem);
    void backupCreated(const QString& backupFile);
    void settingsImported(ImportDataType type);

private:
    QJsonObject exportSettingsData();
    QJsonObject exportThemesData();
    QJsonArray exportFavoritesData();
    QJsonArray exportSearchHistoryData();
    QJsonObject exportWindowLayoutData();
    QJsonObject exportUserPreferencesData();

    bool importSettingsData(const QJsonObject& data);
    bool importThemesData(const QJsonObject& data);
    bool importFavoritesData(const QJsonArray& data);
    bool importSearchHistoryData(const QJsonArray& data);
    bool importWindowLayoutData(const QJsonObject& data);
    bool importUserPreferencesData(const QJsonObject& data);

    QString calculateChecksum(const QJsonDocument& doc) const;
    bool verifyChecksum(const ImportData& data) const;

    // Integration
    ThemeManager* m_themeManager;
    QSettings* m_settings;

    // Constants
    static const QString EXPORT_VERSION;
    static const QString EXPORT_FORMAT_VERSION;
};

#endif // IMPORTDIALOG_H
