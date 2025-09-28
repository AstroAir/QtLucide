/**
 * QtLucide Gallery Application - Export Dialog
 *
 * A modern export dialog with comprehensive options for exporting icons:
 * - Multiple format support (PNG, SVG, ICO, PDF)
 * - Flexible size options (preset and custom sizes)
 * - Color customization and transparency
 * - Batch export capabilities
 * - Progress tracking and cancellation
 * - Template-based file naming
 * - Preview functionality
 * - Theme-aware styling
 */

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QBuffer>
#include <QButtonGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QFormLayout>
#include <QFuture>
#include <QFutureWatcher>
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
#include <QMutex>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QStandardPaths>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QTabWidget>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QtConcurrent>

#ifdef QTLUCIDE_AVAILABLE
    #include <QtLucide/QtLucide.h>
#endif
#include "../themes/ThemeManager.h"

// Forward declarations
class ExportOptionsWidget;
class ExportPreviewWidget;
class ExportProgressWidget;
class BatchExportManager;

/**
 * @brief Export format enumeration
 */
enum class ExportFormat { PNG = 0, SVG = 1, ICO = 2, PDF = 3, JPEG = 4 };

/**
 * @brief Export configuration structure
 */
struct ExportConfig {
    // Format settings
    ExportFormat format = ExportFormat::PNG;
    QList<int> sizes = {64}; // Default 64x64
    bool customSize = false;
    int customWidth = 64;
    int customHeight = 64;

    // Color settings
    QColor iconColor = QColor("#000000");
    bool useCustomColor = false;
    bool transparentBackground = true;
    QColor backgroundColor = QColor("#ffffff");

    // Output settings
    QString outputDirectory;
    QString fileNameTemplate = "{name}_{size}x{size}";
    bool createSubfolders = false;
    QString subfolderTemplate = "{format}";

    // Quality settings
    int jpegQuality = 90;
    bool antialiasing = true;

    // Batch settings
    bool overwriteExisting = false;
    bool skipErrors = true;

    // Metadata
    bool includeMetadata = false;
    QString author;
    QString description;
    QString copyright;
};

/**
 * @brief Export options widget with comprehensive settings
 */
class ExportOptionsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExportOptionsWidget(QWidget* parent = nullptr);
    ~ExportOptionsWidget();

    // Configuration
    void setExportConfig(const ExportConfig& config);
    ExportConfig exportConfig() const;
    void resetToDefaults();

    // Validation
    bool validateConfig() const;
    QStringList getValidationErrors() const;

    // Theme integration
    void applyTheme();

signals:
    void configChanged(const ExportConfig& config);
    void formatChanged(ExportFormat format);
    void sizesChanged(const QList<int>& sizes);
    void outputDirectoryChanged(const QString& directory);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onFormatChanged();
    void onSizeChanged();
    void onColorChanged();
    void onOutputSettingsChanged();
    void onBrowseOutputDirectory();
    void onPresetSizeToggled();
    void onCustomSizeToggled();
    void onColorPickerClicked();
    void onBackgroundColorPickerClicked();
    void onFileNameTemplateChanged();
    void onPreviewTemplate();

private:
    void setupUI();
    void setupFormatSection();
    void setupSizeSection();
    void setupColorSection();
    void setupOutputSection();
    void setupAdvancedSection();
    void updateFormatOptions();
    void updateSizeOptions();
    void updateColorOptions();
    void updatePreview();
    QString generateFileName(const QString& iconName, int size) const;

    // UI components
    QVBoxLayout* m_mainLayout;
    QTabWidget* m_tabWidget;

    // Format tab
    QWidget* m_formatTab;
    QVBoxLayout* m_formatLayout;
    QGroupBox* m_formatGroup;
    QButtonGroup* m_formatButtonGroup;
    QRadioButton* m_pngRadio;
    QRadioButton* m_svgRadio;
    QRadioButton* m_icoRadio;
    QRadioButton* m_pdfRadio;
    QRadioButton* m_jpegRadio;

    // Size tab
    QWidget* m_sizeTab;
    QVBoxLayout* m_sizeLayout;
    QGroupBox* m_sizeGroup;
    QCheckBox* m_size16;
    QCheckBox* m_size32;
    QCheckBox* m_size64;
    QCheckBox* m_size128;
    QCheckBox* m_size256;
    QCheckBox* m_size512;
    QCheckBox* m_customSizeCheck;
    QSpinBox* m_customWidthSpin;
    QSpinBox* m_customHeightSpin;
    QLabel* m_aspectRatioLabel;
    QCheckBox* m_maintainAspectRatio;

    // Color tab
    QWidget* m_colorTab;
    QVBoxLayout* m_colorLayout;
    QGroupBox* m_colorGroup;
    QCheckBox* m_useCustomColorCheck;
    QPushButton* m_colorPickerButton;
    QLabel* m_colorPreview;
    QCheckBox* m_transparentBackgroundCheck;
    QPushButton* m_backgroundColorButton;
    QLabel* m_backgroundColorPreview;
    QSlider* m_opacitySlider;
    QLabel* m_opacityLabel;

    // Output tab
    QWidget* m_outputTab;
    QVBoxLayout* m_outputLayout;
    QGroupBox* m_outputGroup;
    QLineEdit* m_outputDirectoryEdit;
    QPushButton* m_browseButton;
    QLineEdit* m_fileNameTemplateEdit;
    QPushButton* m_previewTemplateButton;
    QLabel* m_templatePreviewLabel;
    QCheckBox* m_createSubfoldersCheck;
    QLineEdit* m_subfolderTemplateEdit;
    QCheckBox* m_overwriteExistingCheck;

    // Advanced tab
    QWidget* m_advancedTab;
    QVBoxLayout* m_advancedLayout;
    QGroupBox* m_qualityGroup;
    QSlider* m_jpegQualitySlider;
    QLabel* m_jpegQualityLabel;
    QCheckBox* m_antialiasingCheck;
    QGroupBox* m_metadataGroup;
    QCheckBox* m_includeMetadataCheck;
    QLineEdit* m_authorEdit;
    QLineEdit* m_descriptionEdit;
    QLineEdit* m_copyrightEdit;

    // Data
    ExportConfig m_config;

    // Theme
    ThemeAwareWidget* m_themeWidget;

    // Constants
    static const QList<int> PRESET_SIZES;
    static const QStringList TEMPLATE_VARIABLES;
};

/**
 * @brief Export preview widget showing icon previews
 */
class ExportPreviewWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExportPreviewWidget(QWidget* parent = nullptr);
    ~ExportPreviewWidget();

    // Preview management
    void setIconNames(const QStringList& iconNames);
    QStringList iconNames() const { return m_iconNames; }
    void setExportConfig(const ExportConfig& config);
    void refreshPreviews();
    void clearPreviews();

    // Integration
#ifdef QTLUCIDE_AVAILABLE
    void setLucide(lucide::QtLucide* lucide);
#else
    void setLucide(void* lucide);
#endif

signals:
    void previewClicked(const QString& iconName);
    void previewDoubleClicked(const QString& iconName);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onConfigChanged();
    void updateLayout();

private:
    void setupUI();
    void generatePreviews();
    QPixmap generateIconPreview(const QString& iconName, int size) const;
    void drawPreviewGrid(QPainter* painter);
    int getPreviewItemAt(const QPoint& pos) const;

    // UI components
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;

    // Data
    QStringList m_iconNames;
    ExportConfig m_config;
#ifdef QTLUCIDE_AVAILABLE
    lucide::QtLucide* m_lucide;
#else
    void* m_lucide;
#endif
    QList<QPixmap> m_previews;

    // Layout
    int m_itemSize;
    int m_itemSpacing;
    int m_columns;
    int m_hoveredItem;

    // Constants
    static constexpr int DEFAULT_ITEM_SIZE = 80;
    static constexpr int DEFAULT_SPACING = 8;
    static constexpr int MIN_COLUMNS = 2;
};

/**
 * @brief Main export dialog
 */
class ExportDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportDialog(QWidget* parent = nullptr);
    ~ExportDialog();

    // Icon management
    void setIconNames(const QStringList& iconNames);
    QStringList iconNames() const;
    void addIconName(const QString& iconName);
    void removeIconName(const QString& iconName);
    void clearIconNames();

    // Configuration
    void setExportConfig(const ExportConfig& config);
    ExportConfig exportConfig() const;

    // Integration
#ifdef QTLUCIDE_AVAILABLE
    void setLucide(lucide::QtLucide* lucide);
#else
    void setLucide(void* lucide);
#endif
    void setThemeManager(ThemeManager* themeManager);

    // Dialog result
    bool wasExportSuccessful() const { return m_exportSuccessful; }
    QStringList getExportedFiles() const { return m_exportedFiles; }

public slots:
    void accept() override;
    void reject() override;

signals:
    void exportStarted();
    void exportProgress(int current, int total, const QString& currentFile);
    void exportFinished(bool success, const QStringList& exportedFiles);
    void exportCancelled();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onExportClicked();
    void onCancelClicked();
    void onConfigChanged(const ExportConfig& config);
    void onExportProgress(int current, int total, const QString& currentFile);
    void onExportFinished(bool success);
    void onPreviewIconClicked(const QString& iconName);

private:
    void setupUI();
    void setupLayout();
    void setupConnections();
    void updateUI();
    void startExport();
    void cancelExport();
    bool validateExportConfig();
    void applyTheme();

    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_contentLayout;
    QVBoxLayout* m_leftLayout;
    QVBoxLayout* m_rightLayout;

    QLabel* m_titleLabel;
    QLabel* m_iconCountLabel;
    ExportOptionsWidget* m_optionsWidget;
    ExportPreviewWidget* m_previewWidget;
    ExportProgressWidget* m_progressWidget;

    QHBoxLayout* m_buttonLayout;
    QPushButton* m_exportButton;
    QPushButton* m_cancelButton;
    QPushButton* m_closeButton;

    // Data
    QStringList m_iconNames;
    ExportConfig m_config;
    bool m_exportInProgress;
    bool m_exportSuccessful;
    QStringList m_exportedFiles;

    // Integration
#ifdef QTLUCIDE_AVAILABLE
    lucide::QtLucide* m_lucide;
#else
    void* m_lucide;
#endif
    ThemeManager* m_themeManager;
    BatchExportManager* m_exportManager;

    // Theme
    ThemeAwareWidget* m_themeWidget;
};

#endif // EXPORTDIALOG_H
