/**
 * @file IconExportDialog.cpp
 * @brief Implementation of IconExportDialog
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "IconExportDialog.h"
#include "ExportUtils.h"
#include "GalleryTypes.h"

#include <QtLucide/QtLucide.h>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QStandardPaths>
#include <QSvgRenderer>
#include <QVBoxLayout>

namespace gallery {

IconExportDialog::IconExportDialog(QWidget* parent)
    : QDialog(parent), m_selectedColor(Qt::black), m_lastPreviewSize(0) {
    setWindowTitle("Export Icon");
    setMinimumWidth(600);
    setMinimumHeight(500);
    setupUI();
}

IconExportDialog::~IconExportDialog() = default;

void IconExportDialog::setIconName(const QString& iconName) {
    m_currentIconName = iconName;
    setWindowTitle(QString("Export Icon: %1").arg(iconName));
    updatePreview();
}

void IconExportDialog::setLucideInstance(lucide::QtLucide* lucide) {
    m_lucide = lucide;
    updatePreview();
}

IconExportSettings IconExportDialog::getExportSettings() const {
    IconExportSettings settings;
    settings.iconName = m_currentIconName;
    settings.format = static_cast<ExportFormat>(m_formatComboBox->currentIndex());
    settings.useCustomSize = (m_sizeComboBox->currentIndex() == m_sizeComboBox->count() - 1);
    settings.customSize = m_customSizeSpinBox->value();
    settings.size =
        settings.useCustomSize ? settings.customSize : m_sizeComboBox->currentText().toInt();
    settings.colorOverride = m_selectedColor;
    settings.useColorOverride = m_useColorOverrideCheckBox->isChecked();
    settings.strokeWidth = m_strokeWidthSpinBox->value();
    settings.outputPath = m_outputPathEdit->text();
    return settings;
}

void IconExportDialog::onFormatChanged(int index) {
    // Enable/disable appropriate size options based on format
    Q_UNUSED(index);
    updatePreview();
}

void IconExportDialog::onSizeChanged(int index) {
    bool isCustom = (index == m_sizeComboBox->count() - 1);
    m_customSizeSpinBox->setEnabled(isCustom);
    updatePreview();
}

void IconExportDialog::onCustomSizeChanged(int value) {
    Q_UNUSED(value);
    updatePreview();
}

void IconExportDialog::onColorButtonClicked() {
    QColor color = QColorDialog::getColor(m_selectedColor, this, "Select Icon Color");
    if (color.isValid()) {
        m_selectedColor = color;
        // Update color button to show selected color
        m_colorButton->setStyleSheet(
            QString("background-color: %1; border: 1px solid #ccc;").arg(m_selectedColor.name()));
        updatePreview();
    }
}

void IconExportDialog::onBrowseOutputClicked() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QString("%1.svg").arg(m_currentIconName);

    QString selectedFile =
        QFileDialog::getSaveFileName(this, "Save Icon As", dir + "/" + fileName,
                                     "SVG Files (*.svg);;PNG Files (*.png);;ICO Files (*.ico)");

    if (!selectedFile.isEmpty()) {
        m_outputPathEdit->setText(selectedFile);
    }
}

void IconExportDialog::onCopySvgCodeClicked() {
    copySvgCodeToClipboard();
    QMessageBox::information(this, "Success", "SVG code copied to clipboard!");
}

void IconExportDialog::onCopyPngClicked() {
    copyPngToClipboard();
    QMessageBox::information(this, "Success", "PNG copied to clipboard!");
}

void IconExportDialog::updatePreview() {
    if (m_currentIconName.isEmpty() || !m_lucide) {
        return;
    }

    int previewSize = 128;
    if (m_sizeComboBox->currentIndex() == m_sizeComboBox->count() - 1) {
        previewSize = m_customSizeSpinBox->value();
    } else {
        previewSize = m_sizeComboBox->currentText().toInt();
    }

    // Clamp preview size for display
    int displaySize = qMin(previewSize, 150);

    // Get color override if enabled
    QColor color;
    if (m_useColorOverrideCheckBox->isChecked()) {
        color = m_selectedColor;
    }

    // Render the actual icon
    QPixmap pixmap = ExportUtils::renderToPixmap(m_lucide, m_currentIconName, displaySize, color);
    if (!pixmap.isNull()) {
        m_iconPreviewLabel->setPixmap(pixmap);
    } else {
        // Fallback to placeholder
        QPixmap placeholder(displaySize, displaySize);
        placeholder.fill(Qt::lightGray);
        m_iconPreviewLabel->setPixmap(placeholder);
    }
    m_lastPreviewSize = previewSize;
}

void IconExportDialog::onExportClicked() {
    if (!validateOutputPath()) {
        QMessageBox::warning(this, "Invalid Output", "Please specify a valid output path.");
        return;
    }

    if (!m_lucide) {
        QMessageBox::warning(this, "Error", "Icon renderer not available.");
        return;
    }

    IconExportSettings settings = getExportSettings();
    bool success = false;

    // Get color override
    QColor color;
    if (settings.useColorOverride) {
        color = settings.colorOverride;
    }

    // Export based on format
    switch (settings.format) {
        case ExportFormat::SVG:
            success = ExportUtils::saveAsSvg(m_lucide, settings.iconName, settings.outputPath,
                                             color, settings.strokeWidth);
            break;
        case ExportFormat::PNG:
            success = ExportUtils::saveAsPng(m_lucide, settings.iconName, settings.outputPath,
                                             settings.size, color);
            break;
        case ExportFormat::ICO:
        case ExportFormat::ICNS:
            success = ExportUtils::saveAsIco(m_lucide, settings.iconName, settings.outputPath,
                                             settings.size, color);
            break;
    }

    if (success) {
        QMessageBox::information(this, "Export Successful",
                                 QString("Icon '%1' exported successfully to:\n%2")
                                     .arg(settings.iconName, settings.outputPath));
        accept();
    } else {
        QMessageBox::warning(this, "Export Failed",
                             QString("Failed to export icon '%1'.").arg(settings.iconName));
    }
}

void IconExportDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Preview section
    mainLayout->addWidget(createPreviewSection());

    // Format and size section
    mainLayout->addWidget(createFormatSizeSection());

    // Customization section
    mainLayout->addWidget(createCustomizationSection());

    // Output section
    mainLayout->addWidget(createOutputSection());

    // Buttons
    mainLayout->addLayout(createButtonSection());

    setLayout(mainLayout);
}

QGroupBox* IconExportDialog::createPreviewSection() {
    QGroupBox* group = new QGroupBox("Preview", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    m_iconPreviewLabel = new QLabel(this);
    m_iconPreviewLabel->setAlignment(Qt::AlignCenter);
    m_iconPreviewLabel->setMinimumHeight(150);
    m_iconPreviewLabel->setStyleSheet("border: 1px solid #ccc; background-color: #f5f5f5;");

    layout->addWidget(m_iconPreviewLabel);
    group->setLayout(layout);
    return group;
}

QGroupBox* IconExportDialog::createFormatSizeSection() {
    QGroupBox* group = new QGroupBox("Format & Size", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    // Format selection
    QHBoxLayout* formatLayout = new QHBoxLayout();
    formatLayout->addWidget(new QLabel("Format:", this));

    m_formatComboBox = new QComboBox(this);
    m_formatComboBox->addItems({"SVG", "PNG", "ICO"});
    connect(m_formatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &IconExportDialog::onFormatChanged);
    formatLayout->addWidget(m_formatComboBox);
    formatLayout->addStretch();

    layout->addLayout(formatLayout);

    // Size selection
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Size:", this));

    m_sizeComboBox = new QComboBox(this);
    m_sizeComboBox->addItems({"16", "24", "32", "48", "64", "128", "256", "512", "Custom"});
    connect(m_sizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &IconExportDialog::onSizeChanged);
    sizeLayout->addWidget(m_sizeComboBox);

    m_customSizeSpinBox = new QSpinBox(this);
    m_customSizeSpinBox->setMinimum(16);
    m_customSizeSpinBox->setMaximum(2048);
    m_customSizeSpinBox->setValue(128);
    m_customSizeSpinBox->setEnabled(false);
    connect(m_customSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &IconExportDialog::onCustomSizeChanged);
    sizeLayout->addWidget(m_customSizeSpinBox);
    sizeLayout->addStretch();

    layout->addLayout(sizeLayout);

    group->setLayout(layout);
    return group;
}

QGroupBox* IconExportDialog::createCustomizationSection() {
    QGroupBox* group = new QGroupBox("Customization", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    // Color override
    QHBoxLayout* colorLayout = new QHBoxLayout();
    m_useColorOverrideCheckBox = new QCheckBox("Color Override:", this);
    colorLayout->addWidget(m_useColorOverrideCheckBox);

    m_colorButton = new QPushButton("Pick Color", this);
    m_colorButton->setMaximumWidth(120);
    m_colorButton->setStyleSheet("background-color: #000000; color: #ffffff;");
    connect(m_colorButton, &QPushButton::clicked, this, &IconExportDialog::onColorButtonClicked);
    colorLayout->addWidget(m_colorButton);
    colorLayout->addStretch();

    layout->addLayout(colorLayout);

    // Stroke width
    QHBoxLayout* strokeLayout = new QHBoxLayout();
    strokeLayout->addWidget(new QLabel("Stroke Width:", this));

    m_strokeWidthSpinBox = new QDoubleSpinBox(this);
    m_strokeWidthSpinBox->setMinimum(0.0);
    m_strokeWidthSpinBox->setMaximum(10.0);
    m_strokeWidthSpinBox->setValue(2.0);
    m_strokeWidthSpinBox->setSingleStep(0.1);
    strokeLayout->addWidget(m_strokeWidthSpinBox);
    strokeLayout->addStretch();

    layout->addLayout(strokeLayout);

    group->setLayout(layout);
    return group;
}

QGroupBox* IconExportDialog::createOutputSection() {
    QGroupBox* group = new QGroupBox("Output", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->addWidget(new QLabel("Path:", this));

    m_outputPathEdit = new QLineEdit(this);
    m_outputPathEdit->setPlaceholderText("Select output file path...");
    pathLayout->addWidget(m_outputPathEdit);

    m_browseButton = new QPushButton("Browse...", this);
    m_browseButton->setMaximumWidth(100);
    connect(m_browseButton, &QPushButton::clicked, this, &IconExportDialog::onBrowseOutputClicked);
    pathLayout->addWidget(m_browseButton);

    layout->addLayout(pathLayout);

    // Clipboard buttons
    QHBoxLayout* clipboardLayout = new QHBoxLayout();
    m_copySvgButton = new QPushButton("Copy SVG Code", this);
    connect(m_copySvgButton, &QPushButton::clicked, this, &IconExportDialog::onCopySvgCodeClicked);
    clipboardLayout->addWidget(m_copySvgButton);

    m_copyPngButton = new QPushButton("Copy PNG to Clipboard", this);
    connect(m_copyPngButton, &QPushButton::clicked, this, &IconExportDialog::onCopyPngClicked);
    clipboardLayout->addWidget(m_copyPngButton);
    clipboardLayout->addStretch();

    layout->addLayout(clipboardLayout);

    group->setLayout(layout);
    return group;
}

QHBoxLayout* IconExportDialog::createButtonSection() {
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addStretch();

    m_exportButton = new QPushButton("Export", this);
    m_exportButton->setMinimumWidth(100);
    connect(m_exportButton, &QPushButton::clicked, this, &IconExportDialog::onExportClicked);
    layout->addWidget(m_exportButton);

    m_cancelButton = new QPushButton("Cancel", this);
    m_cancelButton->setMinimumWidth(100);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(m_cancelButton);

    return layout;
}

IconOptions IconExportDialog::getCurrentIconOptions() const {
    IconOptions options;
    options.color = m_selectedColor;
    options.strokeWidth = m_strokeWidthSpinBox->value();

    if (m_sizeComboBox->currentIndex() == m_sizeComboBox->count() - 1) {
        options.size = m_customSizeSpinBox->value();
    } else {
        options.size = m_sizeComboBox->currentText().toInt();
    }

    return options;
}

void IconExportDialog::copySvgCodeToClipboard() {
    if (!m_lucide || m_currentIconName.isEmpty()) {
        return;
    }

    // Get color and stroke width overrides
    QColor color;
    double strokeWidth = 0.0;
    if (m_useColorOverrideCheckBox->isChecked()) {
        color = m_selectedColor;
    }
    strokeWidth = m_strokeWidthSpinBox->value();

    ExportUtils::copySvgToClipboard(m_lucide, m_currentIconName, color, strokeWidth);
}

void IconExportDialog::copyPngToClipboard() {
    if (!m_lucide || m_currentIconName.isEmpty()) {
        return;
    }

    IconOptions options = getCurrentIconOptions();
    QColor color;
    if (m_useColorOverrideCheckBox->isChecked()) {
        color = m_selectedColor;
    }

    ExportUtils::copyPngToClipboard(m_lucide, m_currentIconName, options.size, color);
}

bool IconExportDialog::validateOutputPath() const {
    return !m_outputPathEdit->text().isEmpty();
}

} // namespace gallery
