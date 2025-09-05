/**
 * QtLucide Gallery Application - Icon Export Dialog Implementation
 */

#include "IconExportDialog.h"
#include "IconMetadataManager.h"
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QStandardPaths>
#include <QDir>
#include <QPainter>
#include <QSvgRenderer>
#include <QBuffer>
#include <QImageWriter>
#include <QDebug>

IconExportDialog::IconExportDialog(lucide::QtLucide* lucide,
                                 IconMetadataManager* metadataManager,
                                 QWidget *parent)
    : QDialog(parent)
    , m_lucide(lucide)
    , m_metadataManager(metadataManager)
    , m_mainLayout(nullptr)
    , m_exporting(false)
    , m_previewTimer(new QTimer(this))
{
    setWindowTitle("Export Icons");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setModal(true);
    resize(800, 600);

    // Initialize default options
    m_options.outputDirectory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);

    setupUI();
    connectSignals();
    updatePreview();

    // Setup preview timer
    m_previewTimer->setSingleShot(true);
    m_previewTimer->setInterval(500);
    connect(m_previewTimer, &QTimer::timeout, this, &IconExportDialog::updatePreview);
}

IconExportDialog::~IconExportDialog()
{
}

void IconExportDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_contentLayout = new QHBoxLayout();

    // Left side - options
    QVBoxLayout* leftLayout = new QVBoxLayout();
    setupFormatOptions();
    setupSizeOptions();
    setupOutputOptions();
    setupAdvancedOptions();

    leftLayout->addWidget(m_formatGroup);
    leftLayout->addWidget(m_sizeGroup);
    leftLayout->addWidget(m_outputGroup);
    leftLayout->addWidget(m_advancedGroup);
    leftLayout->addStretch();

    // Right side - preview
    QVBoxLayout* rightLayout = new QVBoxLayout();
    setupPreview();
    rightLayout->addWidget(m_previewGroup);
    rightLayout->addStretch();

    m_contentLayout->addLayout(leftLayout, 2);
    m_contentLayout->addLayout(rightLayout, 1);

    m_mainLayout->addLayout(m_contentLayout);

    // Progress bar
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_progressLabel = new QLabel();
    m_progressLabel->setVisible(false);

    m_mainLayout->addWidget(m_progressBar);
    m_mainLayout->addWidget(m_progressLabel);

    setupButtonBox();
}

void IconExportDialog::setupFormatOptions()
{
    m_formatGroup = new QGroupBox("Format & Mode");
    QFormLayout* layout = new QFormLayout(m_formatGroup);

    m_formatCombo = new QComboBox();
    m_formatCombo->addItems({"PNG", "SVG", "ICO", "PDF", "ICNS"});
    layout->addRow("Format:", m_formatCombo);

    m_modeCombo = new QComboBox();
    m_modeCombo->addItems({"Single Icon", "Selected Icons", "All Icons",
                          "Favorite Icons", "Category Icons"});
    layout->addRow("Export Mode:", m_modeCombo);
}

void IconExportDialog::setupSizeOptions()
{
    m_sizeGroup = new QGroupBox("Size Options");
    QVBoxLayout* layout = new QVBoxLayout(m_sizeGroup);

    // Size list
    QHBoxLayout* sizeListLayout = new QHBoxLayout();
    m_sizeList = new QListWidget();
    m_sizeList->setMaximumHeight(100);

    QVBoxLayout* sizeButtonLayout = new QVBoxLayout();
    m_sizeSpinBox = new QSpinBox();
    m_sizeSpinBox->setRange(16, 2048);
    m_sizeSpinBox->setValue(256);
    m_sizeSpinBox->setSuffix("px");

    m_addSizeButton = new QPushButton("Add");
    m_removeSizeButton = new QPushButton("Remove");

    sizeButtonLayout->addWidget(m_sizeSpinBox);
    sizeButtonLayout->addWidget(m_addSizeButton);
    sizeButtonLayout->addWidget(m_removeSizeButton);
    sizeButtonLayout->addStretch();

    sizeListLayout->addWidget(m_sizeList);
    sizeListLayout->addLayout(sizeButtonLayout);

    layout->addLayout(sizeListLayout);

    m_aspectRatioCheck = new QCheckBox("Maintain aspect ratio");
    m_aspectRatioCheck->setChecked(true);
    layout->addWidget(m_aspectRatioCheck);

    // Add default sizes
    m_options.sizes = {16, 32, 48, 64, 128, 256};
    updateSizeList();
}

void IconExportDialog::setupOutputOptions()
{
    m_outputGroup = new QGroupBox("Output Options");
    QFormLayout* layout = new QFormLayout(m_outputGroup);

    // Output directory
    QHBoxLayout* dirLayout = new QHBoxLayout();
    m_outputDirEdit = new QLineEdit(m_options.outputDirectory);
    m_browseButton = new QPushButton("Browse...");
    dirLayout->addWidget(m_outputDirEdit);
    dirLayout->addWidget(m_browseButton);
    layout->addRow("Output Directory:", dirLayout);

    // Filename template
    m_filenameTemplateEdit = new QLineEdit(m_options.filenameTemplate);
    layout->addRow("Filename Template:", m_filenameTemplateEdit);

    m_filenamePreviewLabel = new QLabel();
    m_filenamePreviewLabel->setStyleSheet("color: gray; font-style: italic;");
    layout->addRow("Preview:", m_filenamePreviewLabel);

    m_subfoldersCheck = new QCheckBox("Create subfolders by size");
    m_overwriteCheck = new QCheckBox("Overwrite existing files");

    layout->addRow(m_subfoldersCheck);
    layout->addRow(m_overwriteCheck);

    updateFilenamePreview();
}

void IconExportDialog::setupAdvancedOptions()
{
    m_advancedGroup = new QGroupBox("Advanced Options");
    QFormLayout* layout = new QFormLayout(m_advancedGroup);

    m_backgroundColorButton = new QPushButton("Transparent");
    m_backgroundColorButton->setStyleSheet("QPushButton { background-color: transparent; }");
    layout->addRow("Background Color:", m_backgroundColorButton);

    m_useOriginalColorsCheck = new QCheckBox("Use original colors");
    m_useOriginalColorsCheck->setChecked(true);
    layout->addRow(m_useOriginalColorsCheck);

    m_customColorButton = new QPushButton("Black");
    m_customColorButton->setStyleSheet("QPushButton { background-color: black; color: white; }");
    m_customColorButton->setEnabled(false);
    layout->addRow("Custom Color:", m_customColorButton);

    m_antialiasingCheck = new QCheckBox("Enable antialiasing");
    m_antialiasingCheck->setChecked(true);
    layout->addRow(m_antialiasingCheck);

    m_optimizeSvgCheck = new QCheckBox("Optimize SVG output");
    layout->addRow(m_optimizeSvgCheck);

    m_embedFontsCheck = new QCheckBox("Embed fonts in SVG");
    layout->addRow(m_embedFontsCheck);

    m_includeMetadataCheck = new QCheckBox("Include metadata");
    layout->addRow(m_includeMetadataCheck);
}

void IconExportDialog::setupPreview()
{
    m_previewGroup = new QGroupBox("Preview");
    QVBoxLayout* layout = new QVBoxLayout(m_previewGroup);

    m_previewLabel = new QLabel();
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setMinimumSize(200, 200);
    m_previewLabel->setStyleSheet("QLabel { border: 1px solid gray; background-color: white; }");

    m_previewInfoLabel = new QLabel();
    m_previewInfoLabel->setAlignment(Qt::AlignCenter);
    m_previewInfoLabel->setStyleSheet("color: gray; font-size: 10px;");

    layout->addWidget(m_previewLabel);
    layout->addWidget(m_previewInfoLabel);
}

void IconExportDialog::setupButtonBox()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    m_previewButton = new QPushButton("Preview");
    m_exportButton = new QPushButton("Export");
    m_exportButton->setDefault(true);
    m_cancelButton = new QPushButton("Cancel");
    m_closeButton = new QPushButton("Close");

    buttonLayout->addWidget(m_previewButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_exportButton);
    buttonLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(buttonLayout);
}

void IconExportDialog::connectSignals()
{
    connect(m_formatCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &IconExportDialog::onFormatChanged);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &IconExportDialog::onModeChanged);

    connect(m_addSizeButton, &QPushButton::clicked, this, &IconExportDialog::onAddSize);
    connect(m_removeSizeButton, &QPushButton::clicked, this, &IconExportDialog::onRemoveSize);

    connect(m_browseButton, &QPushButton::clicked, this, &IconExportDialog::onBrowseOutputDirectory);
    connect(m_outputDirEdit, &QLineEdit::textChanged, [this](const QString& text) {
        m_options.outputDirectory = text;
    });

    connect(m_filenameTemplateEdit, &QLineEdit::textChanged,
            this, &IconExportDialog::onFilenameTemplateChanged);

    connect(m_backgroundColorButton, &QPushButton::clicked,
            this, &IconExportDialog::onBackgroundColorChanged);
    connect(m_customColorButton, &QPushButton::clicked,
            this, &IconExportDialog::onCustomColorChanged);

    connect(m_useOriginalColorsCheck, &QCheckBox::toggled, [this](bool checked) {
        m_customColorButton->setEnabled(!checked);
        m_options.useOriginalColors = checked;
        m_previewTimer->start();
    });

    connect(m_exportButton, &QPushButton::clicked, this, &IconExportDialog::startExport);
    connect(m_cancelButton, &QPushButton::clicked, this, &IconExportDialog::cancelExport);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_previewButton, &QPushButton::clicked, this, &IconExportDialog::onPreviewExport);
}

void IconExportDialog::setIconName(const QString& iconName)
{
    m_options.iconNames = {iconName};
    m_options.mode = SingleIcon;
    m_modeCombo->setCurrentIndex(0);
    updatePreview();
}

void IconExportDialog::setIconNames(const QStringList& iconNames)
{
    m_options.iconNames = iconNames;
    m_options.mode = SelectedIcons;
    m_modeCombo->setCurrentIndex(1);
    updatePreview();
}

void IconExportDialog::setExportMode(ExportMode mode)
{
    m_options.mode = mode;
    m_modeCombo->setCurrentIndex(static_cast<int>(mode));
    updatePreview();
}

void IconExportDialog::setCategory(const QString& category)
{
    m_options.category = category;
    if (m_metadataManager) {
        m_options.iconNames = m_metadataManager->getIconsByCategory(category);
    }
    updatePreview();
}

IconExportDialog::ExportOptions IconExportDialog::getExportOptions() const
{
    return m_options;
}

void IconExportDialog::onFormatChanged()
{
    m_options.format = static_cast<ExportFormat>(m_formatCombo->currentIndex());

    // Enable/disable format-specific options
    bool isSvg = (m_options.format == SVG);
    m_optimizeSvgCheck->setEnabled(isSvg);
    m_embedFontsCheck->setEnabled(isSvg);

    bool isRaster = (m_options.format == PNG || m_options.format == ICO || m_options.format == ICNS);
    m_backgroundColorButton->setEnabled(isRaster);
    m_antialiasingCheck->setEnabled(isRaster);

    m_previewTimer->start();
}

void IconExportDialog::onModeChanged()
{
    m_options.mode = static_cast<ExportMode>(m_modeCombo->currentIndex());

    // Update icon list based on mode
    switch (m_options.mode) {
        case AllIcons:
            if (m_metadataManager) {
                m_options.iconNames = m_metadataManager->getAllIconNames();
            }
            break;
        case FavoriteIcons:
            if (m_metadataManager) {
                m_options.iconNames = m_metadataManager->getFavoriteIcons();
            }
            break;
        case CategoryIcons:
            // Will be set when category is selected
            break;
        default:
            // SingleIcon and SelectedIcons are set externally
            break;
    }

    updatePreview();
}

void IconExportDialog::onAddSize()
{
    int size = m_sizeSpinBox->value();
    if (!m_options.sizes.contains(size)) {
        m_options.sizes.append(size);
        std::sort(m_options.sizes.begin(), m_options.sizes.end());
        updateSizeList();
        m_previewTimer->start();
    }
}

void IconExportDialog::onRemoveSize()
{
    int currentRow = m_sizeList->currentRow();
    if (currentRow >= 0 && currentRow < m_options.sizes.size()) {
        m_options.sizes.removeAt(currentRow);
        updateSizeList();
        m_previewTimer->start();
    }
}

void IconExportDialog::onSizeChanged()
{
    // Update size options when size controls change
    m_options.maintainAspectRatio = m_aspectRatioCheck->isChecked();
    m_previewTimer->start();
}

void IconExportDialog::onBrowseOutputDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Choose Output Directory",
                                                   m_options.outputDirectory);
    if (!dir.isEmpty()) {
        m_options.outputDirectory = dir;
        m_outputDirEdit->setText(dir);
    }
}

void IconExportDialog::onBackgroundColorChanged()
{
    QColor color = QColorDialog::getColor(m_options.backgroundColor, this,
                                        "Choose Background Color",
                                        QColorDialog::ShowAlphaChannel);
    if (color.isValid()) {
        m_options.backgroundColor = color;

        QString colorName = color.name();
        if (color.alpha() < 255) {
            colorName = "Transparent";
        }

        QString style = QString("QPushButton { background-color: %1; }").arg(color.name());
        m_backgroundColorButton->setStyleSheet(style);
        m_backgroundColorButton->setText(colorName);

        m_previewTimer->start();
    }
}

void IconExportDialog::onCustomColorChanged()
{
    QColor color = QColorDialog::getColor(m_options.customColor, this, "Choose Custom Color");
    if (color.isValid()) {
        m_options.customColor = color;

        QString style = QString("QPushButton { background-color: %1; color: %2; }")
                       .arg(color.name())
                       .arg(color.lightness() > 128 ? "black" : "white");
        m_customColorButton->setStyleSheet(style);
        m_customColorButton->setText(color.name());

        m_previewTimer->start();
    }
}

void IconExportDialog::onFilenameTemplateChanged()
{
    m_options.filenameTemplate = m_filenameTemplateEdit->text();
    updateFilenamePreview();
}

void IconExportDialog::updateSizeList()
{
    m_sizeList->clear();
    for (int size : m_options.sizes) {
        m_sizeList->addItem(QString("%1px").arg(size));
    }
}

void IconExportDialog::updateFilenamePreview()
{
    QString preview = generateFilename("example-icon", 256, "png");
    m_filenamePreviewLabel->setText(preview);
}

QString IconExportDialog::generateFilename(const QString& iconName, int size, const QString& format) const
{
    QString filename = m_options.filenameTemplate;
    filename.replace("{name}", iconName);
    filename.replace("{size}", QString::number(size));
    filename.replace("{format}", format.toLower());

    if (!filename.contains('.')) {
        filename += "." + format.toLower();
    }

    return filename;
}

void IconExportDialog::updatePreview()
{
    if (!m_lucide || m_options.iconNames.isEmpty()) {
        m_previewLabel->setText("No icon selected");
        m_previewInfoLabel->clear();
        return;
    }

    QString iconName = m_options.iconNames.first();
    int previewSize = 128;

    QPixmap pixmap = renderIcon(iconName, previewSize, m_options);
    if (!pixmap.isNull()) {
        m_previewLabel->setPixmap(pixmap);

        QString info = QString("Icon: %1\nSize: %2x%3\nFormat: %4")
                      .arg(iconName)
                      .arg(pixmap.width())
                      .arg(pixmap.height())
                      .arg(m_formatCombo->currentText());
        m_previewInfoLabel->setText(info);
    } else {
        m_previewLabel->setText("Preview not available");
        m_previewInfoLabel->clear();
    }
}

QPixmap IconExportDialog::renderIcon(const QString& iconName, int size, const ExportOptions& options) const
{
    if (!m_lucide) {
        return QPixmap();
    }

    QIcon icon = m_lucide->icon(iconName);
    if (icon.isNull()) {
        return QPixmap();
    }

    QPixmap pixmap(size, size);
    pixmap.fill(options.backgroundColor);

    QPainter painter(&pixmap);
    if (options.antialiasing) {
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
    }

    // Render the icon
    QRect iconRect(0, 0, size, size);
    if (options.maintainAspectRatio) {
        // Center the icon maintaining aspect ratio
        int margin = size / 10; // 10% margin
        iconRect = iconRect.adjusted(margin, margin, -margin, -margin);
    }

    if (options.useOriginalColors) {
        icon.paint(&painter, iconRect);
    } else {
        // Apply custom color
        QPixmap coloredPixmap = icon.pixmap(size, size);
        QPainter colorPainter(&coloredPixmap);
        colorPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        colorPainter.fillRect(coloredPixmap.rect(), options.customColor);
        colorPainter.end();

        painter.drawPixmap(iconRect, coloredPixmap);
    }

    return pixmap;
}

void IconExportDialog::startExport()
{
    if (m_exporting) {
        return;
    }

    // Validate options
    if (m_options.iconNames.isEmpty()) {
        QMessageBox::warning(this, "Export Error", "No icons selected for export.");
        return;
    }

    if (m_options.outputDirectory.isEmpty()) {
        QMessageBox::warning(this, "Export Error", "Please select an output directory.");
        return;
    }

    if (m_options.sizes.isEmpty()) {
        QMessageBox::warning(this, "Export Error", "Please specify at least one size.");
        return;
    }

    // Create output directory if it doesn't exist
    QDir outputDir(m_options.outputDirectory);
    if (!outputDir.exists()) {
        if (!outputDir.mkpath(".")) {
            QMessageBox::critical(this, "Export Error",
                                "Failed to create output directory.");
            return;
        }
    }

    m_exporting = true;
    m_progressBar->setVisible(true);
    m_progressLabel->setVisible(true);
    m_exportButton->setEnabled(false);
    m_cancelButton->setText("Cancel");

    emit exportStarted();

    // Calculate total operations
    int totalOperations = m_options.iconNames.size() * m_options.sizes.size();
    m_progressBar->setRange(0, totalOperations);
    m_progressBar->setValue(0);

    int currentOperation = 0;
    int successCount = 0;
    int errorCount = 0;

    // Export each icon in each size
    for (const QString& iconName : m_options.iconNames) {
        if (!m_exporting) break; // Check for cancellation

        for (int size : m_options.sizes) {
            if (!m_exporting) break; // Check for cancellation

            emit exportProgress(currentOperation, totalOperations, iconName);
            m_progressLabel->setText(QString("Exporting %1 (%2px)...").arg(iconName).arg(size));

            QApplication::processEvents(); // Keep UI responsive

            if (exportIcon(iconName, m_options)) {
                successCount++;
            } else {
                errorCount++;
            }

            currentOperation++;
            m_progressBar->setValue(currentOperation);
        }
    }

    m_exporting = false;
    m_progressBar->setVisible(false);
    m_progressLabel->setVisible(false);
    m_exportButton->setEnabled(true);
    m_cancelButton->setText("Close");

    QString message;
    if (errorCount == 0) {
        message = QString("Successfully exported %1 icons.").arg(successCount);
        emit exportFinished(true, message);
        QMessageBox::information(this, "Export Complete", message);
    } else {
        message = QString("Export completed with %1 successes and %2 errors.")
                 .arg(successCount).arg(errorCount);
        emit exportFinished(false, message);
        QMessageBox::warning(this, "Export Complete", message);
    }
}

void IconExportDialog::cancelExport()
{
    if (m_exporting) {
        m_exporting = false;
        m_progressLabel->setText("Cancelling export...");
    } else {
        reject();
    }
}

bool IconExportDialog::exportIcon(const QString& iconName, const ExportOptions& options)
{
    QMutexLocker locker(&m_exportMutex);

    try {
        for (int size : options.sizes) {
            QString formatStr;
            switch (options.format) {
                case PNG: formatStr = "png"; break;
                case SVG: formatStr = "svg"; break;
                case ICO: formatStr = "ico"; break;
                case PDF: formatStr = "pdf"; break;
                case ICNS: formatStr = "icns"; break;
            }

            QString filename = generateFilename(iconName, size, formatStr);
            QString fullPath = QDir(options.outputDirectory).filePath(filename);

            // Create subfolder if requested
            if (options.createSubfolders) {
                QString subfolder = QString("%1px").arg(size);
                QDir subDir(QDir(options.outputDirectory).filePath(subfolder));
                if (!subDir.exists()) {
                    subDir.mkpath(".");
                }
                fullPath = subDir.filePath(filename);
            }

            // Check if file exists and overwrite setting
            if (QFile::exists(fullPath) && !options.overwriteExisting) {
                continue; // Skip existing files
            }

            bool success = false;

            if (options.format == SVG) {
                // Export as SVG
                if (m_lucide) {
                    QByteArray svgData = m_lucide->svgData(iconName);
                    if (!svgData.isEmpty()) {
                        QFile file(fullPath);
                        if (file.open(QIODevice::WriteOnly)) {
                            file.write(svgData);
                            success = true;
                        }
                    }
                }
            } else {
                // Export as raster format
                QPixmap pixmap = renderIcon(iconName, size, options);
                if (!pixmap.isNull()) {
                    success = pixmap.save(fullPath, formatStr.toUpper().toLocal8Bit().data());
                }
            }

            if (!success) {
                qWarning() << "Failed to export icon:" << iconName << "size:" << size;
                return false;
            }
        }

        return true;
    } catch (const std::exception& e) {
        qWarning() << "Exception during export:" << e.what();
        return false;
    }
}

void IconExportDialog::onPreviewExport()
{
    if (m_options.iconNames.isEmpty()) {
        QMessageBox::information(this, "Preview", "No icons selected for preview.");
        return;
    }

    QString iconName = m_options.iconNames.first();
    int previewSize = m_options.sizes.isEmpty() ? 256 : m_options.sizes.first();

    QPixmap pixmap = renderIcon(iconName, previewSize, m_options);
    if (!pixmap.isNull()) {
        // Show preview in a separate dialog
        QDialog previewDialog(this);
        previewDialog.setWindowTitle(QString("Preview: %1").arg(iconName));
        previewDialog.resize(pixmap.width() + 40, pixmap.height() + 80);

        QVBoxLayout* layout = new QVBoxLayout(&previewDialog);
        QLabel* label = new QLabel();
        label->setPixmap(pixmap);
        label->setAlignment(Qt::AlignCenter);

        QLabel* infoLabel = new QLabel(QString("Size: %1x%2 pixels").arg(pixmap.width()).arg(pixmap.height()));
        infoLabel->setAlignment(Qt::AlignCenter);

        layout->addWidget(label);
        layout->addWidget(infoLabel);

        previewDialog.exec();
    } else {
        QMessageBox::warning(this, "Preview Error", "Failed to generate preview.");
    }
}
