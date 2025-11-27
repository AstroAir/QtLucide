/**
 * QtLucide Gallery Application - Enhanced Export and Import System
 *
 * A comprehensive, high-performance export and import system with:
 * - Multi-format icon export (PNG, SVG, ICO, PDF, WEBP, TIFF, BMP)
 * - Batch export with parallel processing and progress tracking
 * - Advanced export options (sizes, colors, backgrounds, quality)
 * - Favorites and settings import/export with metadata preservation
 * - Template-based export with custom naming patterns
 * - Cloud integration for backup and sync
 * - Export presets and automation
 * - Performance optimization with memory management
 * - Comprehensive error handling and recovery
 * - Integration with external design tools
 * - Export analytics and reporting
 */

#ifndef ICONEXPORTDIALOG_H
#define ICONEXPORTDIALOG_H

#include <QButtonGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QDoubleSpinBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QElapsedTimer>
#include <QFileDialog>
#include <QFontDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMimeData>
#include <QMutex>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSettings>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedLayout>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QThread>
#include <QThreadPool>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <QtLucide/QtLucide.h>
#include <memory>

// Forward declarations
class IconMetadataManager;

// Include manager stubs and export worker
#include "../../core/BatchExportManager.h"
#include "../../core/managers/ManagerStubs.h"

/**
 * @brief Enhanced export dialog with comprehensive export and import capabilities
 */
class IconExportDialog : public QDialog {
    Q_OBJECT

public:
    enum ExportFormat {
        PNG = 0,
        SVG = 1,
        ICO = 2,
        ICNS = 3, // Added for implementation compatibility
        PDF = 4,
        WEBP = 5,
        TIFF = 6,
        BMP = 7,
        JPEG = 8,
        GIF = 9,
        EPS = 10,
        AI = 11
    };
    Q_ENUM(ExportFormat)

    enum ExportMode {
        SingleIcon = 0,
        MultipleIcons = 1,
        SelectedIcons = 2, // Added for implementation compatibility
        AllIcons = 3,
        FavoriteIcons = 4, // Added for implementation compatibility
        FavoritesOnly = 5,
        CategoryIcons = 6, // Added for implementation compatibility
        CategoryBased = 7,
        CustomSelection = 8
    };
    Q_ENUM(ExportMode)

    enum BackgroundType {
        Transparent = 0,
        Solid = 1,
        Gradient = 2,
        Pattern = 3,
        Image = 4,
        // Aliases for compatibility
        TransparentBackground = Transparent,
        SolidBackground = Solid,
        GradientBackground = Gradient,
        ImageBackground = Image
    };
    Q_ENUM(BackgroundType)

    enum NamingPattern {
        IconName = 0,
        IconNameSize = 1,
        CategoryIconName = 2,
        CustomPattern = 3,
        Sequential = 4
    };
    Q_ENUM(NamingPattern)

    /**
     * @brief Enhanced export options structure with comprehensive settings
     */
    struct ExportOptions {
        // Basic settings
        ExportFormat format = PNG;
        ExportMode mode = SingleIcon;
        QStringList iconNames;
        QString outputDirectory;
        QString filenameTemplate = "{name}_{size}"; // {name}, {size}, {format}, {category}, {index}

        // Size and quality options
        QList<int> sizes = {256};
        bool maintainAspectRatio = true;
        int quality = 95; // For JPEG/WEBP
        bool antialiasing = true;
        bool highQualityScaling = true;

        // Background options
        BackgroundType backgroundType = Transparent;
        QColor backgroundColor = Qt::transparent;
        QColor gradientStartColor = Qt::white;
        QColor gradientEndColor = Qt::black;
        QString backgroundImagePath;
        QString backgroundPatternName;

        // Color options
        bool useOriginalColors = true;
        QColor customColor = Qt::black;
        bool preserveColorProfile = false;
        QString colorSpace = "sRGB";

        // SVG specific options
        bool optimizeSvg = true;
        bool embedFonts = false;
        bool convertTextToPaths = false;
        bool removeMetadata = false;
        double svgPrecision = 2.0;

        // PDF specific options
        QString pdfTitle;
        QString pdfAuthor;
        QString pdfSubject;
        bool pdfEmbedFonts = true;
        bool pdfCompress = true;

        // Batch and organization options
        bool createSubfolders = false;
        bool organizeByCategory = false;
        bool organizeBySize = false;
        bool overwriteExisting = false;
        bool includeMetadata = false;
        bool createManifest = false;
        QString manifestFormat = "JSON"; // JSON, XML, CSV

        // Advanced options
        QString category;
        QStringList tags;
        QStringList contributors;
        bool includeSourceInfo = false;
        bool includeUsageStats = false;
        QString license;
        QString copyright;

        // Performance options
        int maxThreads = 0; // 0 = auto-detect
        bool enableParallelProcessing = true;
        int memoryLimit = 1024; // MB
        bool enableProgressReporting = true;

        // Cloud and sharing options
        bool uploadToCloud = false;
        QString cloudProvider;
        QString cloudPath;
        bool sharePublicly = false;
        QString shareDescription;
    };

    explicit IconExportDialog(lucide::QtLucide* lucide, IconMetadataManager* metadataManager,
                              QWidget* parent = nullptr);
    ~IconExportDialog();

    // Enhanced export parameters
    void setIconName(const QString& iconName);
    void setIconNames(const QStringList& iconNames);
    void setExportMode(ExportMode mode);
    void setCategory(const QString& category);
    void setExportOptions(const ExportOptions& options);

    // Get export options and results
    ExportOptions getExportOptions() const;
    QStringList getExportedFiles() const;
    QString getExportSummary() const;

    // Preset management
    void savePreset(const QString& name);
    void loadPreset(const QString& name);
    void deletePreset(const QString& name);
    QStringList getPresets() const;

    // Import functionality
    void importFavorites(const QString& filePath);
    void exportFavorites(const QString& filePath);
    void importSettings(const QString& filePath);
    void exportSettings(const QString& filePath);

    // Advanced features
    void setCloudProvider(const QString& provider);
    void enableBatchMode(bool enabled);
    void setProgressCallback(std::function<void(int, int, const QString&)> callback);

public slots:
    void startExport();
    void pauseExport();
    void resumeExport();
    void cancelExport();
    void previewExport();
    void validateOptions();
    void resetToDefaults();

signals:
    void exportStarted();
    void exportPaused();
    void exportResumed();
    void exportProgress(int current, int total, const QString& currentIcon);
    void exportFinished(bool success, const QString& message, const QStringList& exportedFiles);
    void exportFinished(); // Added for compatibility
    void exportCancelled();
    void previewReady(const QPixmap& preview);
    void validationResult(bool valid, const QStringList& errors);
    void presetSaved(const QString& name);
    void presetLoaded(const QString& name);
    void cloudUploadProgress(int current, int total);
    void cloudUploadFinished(bool success, const QString& url);

    // Additional signals for compatibility
    void optionsReset();
    void qualityChanged(int quality);
    void backgroundTypeChanged(BackgroundType type);
    void gradientColorsChanged(const QColor& start, const QColor& end);
    void backgroundImageChanged(const QString& path);
    void namingPatternChanged(const QString& pattern);
    void advancedOptionsToggled(bool visible);
    void presetChanged(const QString& name);
    void presetDeleted(const QString& name);
    void favoritesImported(int count);
    void favoritesExported(int count);
    void settingsImported();
    void settingsExported();
    void cloudProviderChanged(const QString& provider);
    void cloudUploadRequested(const QString& provider);
    void exportError(const QString& error);

private slots:
    // Enhanced private slots
    void onFormatChanged();
    void onModeChanged();
    void onSizeChanged();
    void onQualityChanged();
    void onBrowseOutputDirectory();
    void onPreviewExport();
    void onAddSize();
    void onRemoveSize();
    void onBackgroundTypeChanged();
    void onBackgroundColorChanged();
    void onGradientColorsChanged();
    void onBackgroundImageChanged();
    void onCustomColorChanged();
    void onFilenameTemplateChanged();
    void onNamingPatternChanged();
    void onAdvancedOptionsToggled();
    void onPresetChanged();
    void onSavePreset();
    void onDeletePreset();
    void onImportFavorites();
    void onExportFavorites();
    void onImportSettings();
    void onExportSettings();
    void onCloudProviderChanged();
    void onUploadToCloud();
    void onExportWorkerFinished();
    void onExportWorkerProgress(int current, int total, const QString& iconName);
    void onExportWorkerError(const QString& error);

private:
    // Enhanced UI setup methods
    void setupUI();
    void setupTabWidget();
    void setupBasicTab();
    void setupAdvancedTab();
    void setupPresetTab();
    void setupImportExportTab();
    void setupFormatOptions();
    void setupSizeOptions();
    void setupBackgroundOptions();
    void setupColorOptions();
    void setupOutputOptions();
    void setupAdvancedOptions(); // Added for implementation compatibility
    void setupNamingOptions();
    void setupBatchOptions();
    void setupCloudOptions();
    void setupPreview();
    void setupProgressArea();
    void setupButtonBox();

    // Update and validation methods
    void updatePreview();
    void updateSizeList();
    void updateFilenamePreview();
    void updateFormatOptions();
    void updateBackgroundPreview();
    void updateEstimatedSize();
    void updateProgressDisplay();
    void validateExportOptions();
    void connectSignals();
    void disconnectSignals();
    void updateUI();
    void updateBackgroundControls();
    void updateIconList();
    QString generateFileName(const QString& iconName) const;

    // Export processing methods
    QString generateFilename(const QString& iconName, int size, const QString& format) const;
    QString processNamingPattern(const QString& pattern, const QString& iconName, int size,
                                 const QString& format, int index = 0) const;
    QPixmap renderIcon(const QString& iconName, int size, const ExportOptions& options) const;
    QPixmap applyBackground(const QPixmap& icon, const ExportOptions& options) const;
    bool exportIcon(const QString& iconName, const ExportOptions& options);
    bool exportToFormat(const QPixmap& pixmap, const QString& filePath, ExportFormat format,
                        const ExportOptions& options);

    // Batch processing methods
    void startBatchExport();
    void processBatchQueue();
    void finalizeBatchExport();

    // Import/Export methods
    bool importFavoritesFromFile(const QString& filePath);
    bool exportFavoritesToFile(const QString& filePath);
    bool importSettingsFromFile(const QString& filePath);
    bool exportSettingsToFile(const QString& filePath);

    // Preset management methods
    void loadPresets();
    void savePresets();
    ExportOptions getPresetOptions(const QString& name) const;
    void setPresetOptions(const QString& name, const ExportOptions& options);

    // Cloud integration methods (placeholder for future implementation)
    void initializeCloudProviders();
    void uploadToCloudProvider(const QStringList& filePaths);

    // Utility methods
    QString formatFileSize(qint64 bytes) const;
    QString getFormatExtension(ExportFormat format) const;
    QStringList getSupportedFormats() const;
    bool isFormatSupported(ExportFormat format) const;
    void showErrorMessage(const QString& title, const QString& message);
    void showSuccessMessage(const QString& message);

    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    std::unique_ptr<ExportPresetManager> m_presetManager;
    std::unique_ptr<CloudSyncManager> m_cloudManager;

    // Enhanced UI Components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_contentLayout; // Added for implementation compatibility
    QSplitter* m_mainSplitter;
    QTabWidget* m_tabWidget;

    // Basic tab
    QWidget* m_basicTab;
    QScrollArea* m_basicScrollArea;
    QGroupBox* m_formatGroup;
    QComboBox* m_formatCombo;
    QComboBox* m_modeCombo;
    QSlider* m_qualitySlider;
    QSpinBox* m_qualitySpinBox; // Added for compatibility
    QLabel* m_qualityLabel;

    // Size options
    QGroupBox* m_sizeGroup;
    QListWidget* m_sizeList;
    QSpinBox* m_sizeSpinBox;
    QPushButton* m_addSizeButton;
    QPushButton* m_removeSizeButton;
    QPushButton* m_presetSizesButton;
    QCheckBox* m_aspectRatioCheck;
    QCheckBox* m_highQualityScalingCheck;
    QCheckBox* m_antialiasingCheck; // Added for implementation compatibility

    // Background options
    QGroupBox* m_backgroundGroup;
    QComboBox* m_backgroundTypeCombo;
    QPushButton* m_backgroundColorButton;
    QPushButton* m_gradientStartButton;
    QPushButton* m_gradientEndButton;
    QLineEdit* m_backgroundImageEdit;
    QPushButton* m_browseBackgroundButton;
    QPushButton* m_backgroundImageButton; // Added for compatibility
    QComboBox* m_backgroundPatternCombo;

    // Color options
    QGroupBox* m_colorGroup;
    QCheckBox* m_useOriginalColorsCheck;
    QPushButton* m_customColorButton;
    QCheckBox* m_preserveColorProfileCheck;
    QComboBox* m_colorSpaceCombo;

    // Output options
    QGroupBox* m_outputGroup;
    QLineEdit* m_outputDirEdit;
    QPushButton* m_browseButton;
    QComboBox* m_namingPatternCombo;
    QLineEdit* m_namingPatternEdit; // Added for compatibility
    QLineEdit* m_customPatternEdit;
    QLineEdit* m_outputDirectoryEdit;  // Added for compatibility
    QLineEdit* m_filenameTemplateEdit; // Added for implementation compatibility
    QLabel* m_filenamePreviewLabel;
    QCheckBox* m_subfoldersCheck;
    QCheckBox* m_organizeByCategoryCheck;
    QCheckBox* m_organizeBySizeCheck;
    QCheckBox* m_overwriteCheck;
    QCheckBox* m_includeMetadataCheck;
    QCheckBox* m_createManifestCheck;
    QComboBox* m_manifestFormatCombo;

    // Advanced tab
    QWidget* m_advancedTab;
    QScrollArea* m_advancedScrollArea;
    QGroupBox* m_advancedGroup;           // Added for implementation compatibility
    QGroupBox* m_advancedOptionsGroup;    // Added for compatibility
    QPushButton* m_advancedOptionsButton; // Added for compatibility

    // SVG specific options
    QGroupBox* m_svgGroup;
    QCheckBox* m_optimizeSvgCheck;
    QCheckBox* m_embedFontsCheck;
    QCheckBox* m_convertTextToPathsCheck;
    QCheckBox* m_removeMetadataCheck;
    QDoubleSpinBox* m_svgPrecisionSpin;

    // PDF specific options
    QGroupBox* m_pdfGroup;
    QLineEdit* m_pdfTitleEdit;
    QLineEdit* m_pdfAuthorEdit;
    QLineEdit* m_pdfSubjectEdit;
    QCheckBox* m_pdfEmbedFontsCheck;
    QCheckBox* m_pdfCompressCheck;

    // Performance options
    QGroupBox* m_performanceGroup;
    QSpinBox* m_maxThreadsSpin;
    QCheckBox* m_parallelProcessingCheck;
    QSpinBox* m_memoryLimitSpin;
    QCheckBox* m_progressReportingCheck;

    // Preset tab
    QWidget* m_presetTab;
    QListWidget* m_presetList;
    QLineEdit* m_presetNameEdit;
    QPushButton* m_savePresetButton;
    QPushButton* m_loadPresetButton;
    QPushButton* m_deletePresetButton;
    QTextEdit* m_presetDescriptionEdit;

    // Import/Export tab
    QWidget* m_importExportTab;
    QGroupBox* m_favoritesGroup;
    QPushButton* m_importFavoritesButton;
    QPushButton* m_exportFavoritesButton;
    QGroupBox* m_settingsGroup;
    QPushButton* m_importSettingsButton;
    QPushButton* m_exportSettingsButton;

    // Cloud options
    QGroupBox* m_cloudGroup;
    QComboBox* m_cloudProviderCombo;
    QLineEdit* m_cloudPathEdit;
    QCheckBox* m_uploadToCloudCheck;
    QCheckBox* m_sharePubliclyCheck;
    QTextEdit* m_shareDescriptionEdit;
    QPushButton* m_cloudConfigButton;

    // Preview area
    QGroupBox* m_previewGroup;
    QLabel* m_previewLabel;
    QScrollArea* m_previewScrollArea;
    QLabel* m_previewInfoLabel;
    QLabel* m_estimatedSizeLabel;
    QPushButton* m_previewButton;

    // Preset management
    QComboBox* m_presetCombo; // Added for compatibility

    // Status and list components
    QLabel* m_statusLabel;         // Added for compatibility
    QListWidget* m_iconListWidget; // Added for compatibility
    QLabel* m_iconCountLabel;      // Added for compatibility

    // Cloud upload components
    QPushButton* m_uploadToCloudButton; // Added for compatibility

    // Progress area
    QFrame* m_progressFrame;
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;
    QLabel* m_currentFileLabel;
    QLabel* m_timeRemainingLabel;
    QLabel* m_speedLabel;
    QPushButton* m_pauseButton;
    QPushButton* m_cancelProgressButton;

    // Main action buttons
    QFrame* m_buttonFrame;
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_exportButton;
    QPushButton* m_previewMainButton;
    QPushButton* m_validateButton;
    QPushButton* m_resetButton;
    QPushButton* m_cancelButton; // Added for implementation compatibility
    QPushButton* m_closeButton;

    // Enhanced export state and management
    ExportOptions m_options;
    ExportOptions m_backupOptions; // For cancel/reset functionality
    QStringList m_exportQueue;
    QStringList m_exportedFiles;
    QStringList m_failedFiles;

    // Convenience member variables for compatibility
    int m_iconSize;
    QString m_exportFormat;
    int m_quality;
    QColor m_backgroundColor;
    BackgroundType m_backgroundType;
    QString m_namingPattern;
    QString m_outputDirectory;
    QStringList m_selectedIcons;
    QColor m_gradientStartColor;
    QColor m_gradientEndColor;
    QString m_backgroundImagePath;
    bool m_exportInProgress;
    bool m_exportPaused;

    // State flags
    bool m_exporting;
    bool m_paused;
    bool m_cancelled;
    bool m_previewMode;
    bool m_batchMode;

    // Timers and performance
    QTimer* m_previewTimer;
    QTimer* m_progressUpdateTimer;
    QElapsedTimer m_exportTimer;
    QElapsedTimer m_estimationTimer;

    // Export processing
    QThreadPool* m_threadPool;
    std::unique_ptr<ExportWorker> m_exportWorker;
    QThread* m_exportThread;
    std::atomic<int> m_currentProgress{0};
    std::atomic<int> m_totalProgress{0};

    // Cloud integration (placeholder for future implementation)
    QHash<QString, QString> m_cloudProviders;
    QString m_currentCloudProvider;

    // Settings and persistence
    QSettings* m_settings;
    QHash<QString, ExportOptions> m_presets;
    QString m_currentPreset;

    // Performance monitoring
    qint64 m_totalBytesProcessed;
    qint64 m_estimatedTotalSize;
    double m_averageProcessingSpeed;

    // Thread safety
    QMutex m_exportMutex;
    QMutex m_progressMutex;
    QMutex m_queueMutex;

    // Animation and effects
    QPropertyAnimation* m_progressAnimation;
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;

    // Constants
    static constexpr int PREVIEW_UPDATE_DELAY = 300;     // ms
    static constexpr int PROGRESS_UPDATE_INTERVAL = 100; // ms
    static constexpr int MAX_CONCURRENT_EXPORTS = 4;
    static constexpr qint64 DEFAULT_MEMORY_LIMIT = 1024 * 1024 * 1024; // 1GB
    static constexpr const char* SETTINGS_GROUP = "IconExportDialog";
    static constexpr const char* PRESETS_GROUP = "ExportPresets";
};

#endif // ICONEXPORTDIALOG_H
