/**
 * QtLucide Gallery Application - Icon Export Dialog Header
 * 
 * Comprehensive dialog for exporting icons in various formats with
 * customizable options including size, format, and batch operations.
 */

#ifndef ICONEXPORTDIALOG_H
#define ICONEXPORTDIALOG_H

#include <QDialog>
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
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QTreeWidget>
#include <QProgressBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QThread>
#include <QMutex>
#include <QTimer>

#include <QtLucide/QtLucide.h>

// Forward declarations
class IconMetadataManager;

class IconExportDialog : public QDialog
{
    Q_OBJECT

public:
    enum ExportFormat {
        PNG,
        SVG,
        ICO,
        PDF,
        ICNS
    };

    enum ExportMode {
        SingleIcon,
        SelectedIcons,
        AllIcons,
        FavoriteIcons,
        CategoryIcons
    };

    struct ExportOptions {
        ExportFormat format = PNG;
        ExportMode mode = SingleIcon;
        QStringList iconNames;
        QString outputDirectory;
        QString filenameTemplate = "{name}";  // {name}, {size}, {format}
        
        // Size options
        QList<int> sizes = {256};
        bool maintainAspectRatio = true;
        
        // PNG/ICO specific
        QColor backgroundColor = Qt::transparent;
        bool antialiasing = true;
        
        // SVG specific
        bool optimizeSvg = false;
        bool embedFonts = false;
        
        // Batch options
        bool createSubfolders = false;
        bool overwriteExisting = false;
        bool includeMetadata = false;
        
        // Advanced options
        QString category;
        QStringList tags;
        bool useOriginalColors = true;
        QColor customColor = Qt::black;
    };

    explicit IconExportDialog(lucide::QtLucide* lucide, 
                             IconMetadataManager* metadataManager,
                             QWidget *parent = nullptr);
    ~IconExportDialog();

    // Set export parameters
    void setIconName(const QString& iconName);
    void setIconNames(const QStringList& iconNames);
    void setExportMode(ExportMode mode);
    void setCategory(const QString& category);

    // Get export options
    ExportOptions getExportOptions() const;

public slots:
    void startExport();
    void cancelExport();

signals:
    void exportStarted();
    void exportProgress(int current, int total, const QString& currentIcon);
    void exportFinished(bool success, const QString& message);

private slots:
    void onFormatChanged();
    void onModeChanged();
    void onSizeChanged();
    void onBrowseOutputDirectory();
    void onPreviewExport();
    void onAddSize();
    void onRemoveSize();
    void onBackgroundColorChanged();
    void onCustomColorChanged();
    void onFilenameTemplateChanged();

private:
    void setupUI();
    void setupFormatOptions();
    void setupSizeOptions();
    void setupOutputOptions();
    void setupAdvancedOptions();
    void setupPreview();
    void setupButtonBox();
    
    void updatePreview();
    void updateSizeList();
    void updateFilenamePreview();
    void validateOptions();
    void connectSignals();
    
    QString generateFilename(const QString& iconName, int size, const QString& format) const;
    QPixmap renderIcon(const QString& iconName, int size, const ExportOptions& options) const;
    bool exportIcon(const QString& iconName, const ExportOptions& options);
    
    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    
    // UI Components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_contentLayout;
    
    // Format options
    QGroupBox* m_formatGroup;
    QComboBox* m_formatCombo;
    QComboBox* m_modeCombo;
    
    // Size options
    QGroupBox* m_sizeGroup;
    QListWidget* m_sizeList;
    QSpinBox* m_sizeSpinBox;
    QPushButton* m_addSizeButton;
    QPushButton* m_removeSizeButton;
    QCheckBox* m_aspectRatioCheck;
    
    // Output options
    QGroupBox* m_outputGroup;
    QLineEdit* m_outputDirEdit;
    QPushButton* m_browseButton;
    QLineEdit* m_filenameTemplateEdit;
    QLabel* m_filenamePreviewLabel;
    QCheckBox* m_subfoldersCheck;
    QCheckBox* m_overwriteCheck;
    
    // Advanced options
    QGroupBox* m_advancedGroup;
    QPushButton* m_backgroundColorButton;
    QPushButton* m_customColorButton;
    QCheckBox* m_antialiasingCheck;
    QCheckBox* m_optimizeSvgCheck;
    QCheckBox* m_embedFontsCheck;
    QCheckBox* m_includeMetadataCheck;
    QCheckBox* m_useOriginalColorsCheck;
    
    // Preview
    QGroupBox* m_previewGroup;
    QLabel* m_previewLabel;
    QLabel* m_previewInfoLabel;
    
    // Progress
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;
    
    // Buttons
    QPushButton* m_exportButton;
    QPushButton* m_previewButton;
    QPushButton* m_cancelButton;
    QPushButton* m_closeButton;
    
    // Export state
    ExportOptions m_options;
    bool m_exporting;
    QTimer* m_previewTimer;
    
    // Thread safety
    QMutex m_exportMutex;
};

#endif // ICONEXPORTDIALOG_H
