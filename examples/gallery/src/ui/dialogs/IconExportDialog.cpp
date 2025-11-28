/**
 * @file IconExportDialog.cpp
 * @brief Implementation of IconExportDialog
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "IconExportDialog.h"
#include "GalleryTypes.h"

#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QIcon>
#include <QApplication>
#include <QClipboard>
#include <QSvgRenderer>
#include <QPainter>
#include <QStandardPaths>
#include <QFile>
#include <QDebug>

namespace gallery {

IconExportDialog::IconExportDialog(QWidget* parent)
    : QDialog(parent)
    , m_selectedColor(Qt::black)
    , m_lastPreviewSize(0)
{
    setWindowTitle("Export Icon");
    setMinimumWidth(600);
    setMinimumHeight(500);
    setupUI();
}

IconExportDialog::~IconExportDialog() = default;

void IconExportDialog::setIconName(const QString& iconName)
{
    m_currentIconName = iconName;
    setWindowTitle(QString("Export Icon: %1").arg(iconName));
    updatePreview();
}

IconExportSettings IconExportDialog::getExportSettings() const
{
    IconExportSettings settings;
    settings.iconName = m_currentIconName;
    settings.format = static_cast<ExportFormat>(m_formatComboBox->currentIndex());
    settings.useCustomSize = (m_sizeComboBox->currentIndex() == m_sizeComboBox->count() - 1);
    settings.customSize = m_customSizeSpinBox->value();
    settings.size = settings.useCustomSize ? settings.customSize : m_sizeComboBox->currentText().toInt();
    settings.colorOverride = m_selectedColor;
    settings.useColorOverride = m_useColorOverrideCheckBox->isChecked();
    settings.strokeWidth = m_strokeWidthSpinBox->value();
    settings.outputPath = m_outputPathEdit->text();
    return settings;
}

void IconExportDialog::onFormatChanged(int index)
{
    // Enable/disable appropriate size options based on format
    Q_UNUSED(index);
    updatePreview();
}

void IconExportDialog::onSizeChanged(int index)
{
    bool isCustom = (index == m_sizeComboBox->count() - 1);
    m_customSizeSpinBox->setEnabled(isCustom);
    updatePreview();
}

void IconExportDialog::onCustomSizeChanged(int value)
{
    Q_UNUSED(value);
    updatePreview();
}

void IconExportDialog::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_selectedColor, this, "Select Icon Color");
    if (color.isValid()) {
        m_selectedColor = color;
        // Update color button to show selected color
        m_colorButton->setStyleSheet(
            QString("background-color: %1; border: 1px solid #ccc;")
            .arg(m_selectedColor.name())
        );
        updatePreview();
    }
}

void IconExportDialog::onBrowseOutputClicked()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString fileName = QString("%1.svg").arg(m_currentIconName);

    QString selectedFile = QFileDialog::getSaveFileName(
        this,
        "Save Icon As",
        dir + "/" + fileName,
        "SVG Files (*.svg);;PNG Files (*.png);;ICO Files (*.ico)"
    );

    if (!selectedFile.isEmpty()) {
        m_outputPathEdit->setText(selectedFile);
    }
}

void IconExportDialog::onCopySvgCodeClicked()
{
    copySvgCodeToClipboard();
    QMessageBox::information(this, "Success", "SVG code copied to clipboard!");
}

void IconExportDialog::onCopyPngClicked()
{
    copyPngToClipboard();
    QMessageBox::information(this, "Success", "PNG copied to clipboard!");
}

void IconExportDialog::updatePreview()
{
    if (m_currentIconName.isEmpty()) {
        return;
    }

    int previewSize = 128;
    if (m_sizeComboBox->currentIndex() == m_sizeComboBox->count() - 1) {
        previewSize = m_customSizeSpinBox->value();
    } else {
        previewSize = m_sizeComboBox->currentText().toInt();
    }

    // For now, just display placeholder
    // In real implementation, would render actual icon
    if (m_lastPreviewSize != previewSize) {
        QPixmap pixmap(previewSize, previewSize);
        pixmap.fill(Qt::transparent);
        m_iconPreviewLabel->setPixmap(pixmap);
        m_lastPreviewSize = previewSize;
    }
}

void IconExportDialog::onExportClicked()
{
    if (!validateOutputPath()) {
        QMessageBox::warning(this, "Invalid Output", "Please specify a valid output path.");
        return;
    }

    accept();
}

void IconExportDialog::setupUI()
{
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

QGroupBox* IconExportDialog::createPreviewSection()
{
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

QGroupBox* IconExportDialog::createFormatSizeSection()
{
    QGroupBox* group = new QGroupBox("Format & Size", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    // Format selection
    QHBoxLayout* formatLayout = new QHBoxLayout();
    formatLayout->addWidget(new QLabel("Format:", this));

    m_formatComboBox = new QComboBox(this);
    m_formatComboBox->addItems({"SVG", "PNG", "ICO"});
    connect(m_formatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &IconExportDialog::onFormatChanged);
    formatLayout->addWidget(m_formatComboBox);
    formatLayout->addStretch();

    layout->addLayout(formatLayout);

    // Size selection
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(new QLabel("Size:", this));

    m_sizeComboBox = new QComboBox(this);
    m_sizeComboBox->addItems({"16", "24", "32", "48", "64", "128", "256", "512", "Custom"});
    connect(m_sizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &IconExportDialog::onSizeChanged);
    sizeLayout->addWidget(m_sizeComboBox);

    m_customSizeSpinBox = new QSpinBox(this);
    m_customSizeSpinBox->setMinimum(16);
    m_customSizeSpinBox->setMaximum(2048);
    m_customSizeSpinBox->setValue(128);
    m_customSizeSpinBox->setEnabled(false);
    connect(m_customSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &IconExportDialog::onCustomSizeChanged);
    sizeLayout->addWidget(m_customSizeSpinBox);
    sizeLayout->addStretch();

    layout->addLayout(sizeLayout);

    group->setLayout(layout);
    return group;
}

QGroupBox* IconExportDialog::createCustomizationSection()
{
    QGroupBox* group = new QGroupBox("Customization", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    // Color override
    QHBoxLayout* colorLayout = new QHBoxLayout();
    m_useColorOverrideCheckBox = new QCheckBox("Color Override:", this);
    colorLayout->addWidget(m_useColorOverrideCheckBox);

    m_colorButton = new QPushButton("Pick Color", this);
    m_colorButton->setMaximumWidth(120);
    m_colorButton->setStyleSheet("background-color: #000000; color: #ffffff;");
    connect(m_colorButton, &QPushButton::clicked,
            this, &IconExportDialog::onColorButtonClicked);
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

QGroupBox* IconExportDialog::createOutputSection()
{
    QGroupBox* group = new QGroupBox("Output", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathLayout->addWidget(new QLabel("Path:", this));

    m_outputPathEdit = new QLineEdit(this);
    m_outputPathEdit->setPlaceholderText("Select output file path...");
    pathLayout->addWidget(m_outputPathEdit);

    m_browseButton = new QPushButton("Browse...", this);
    m_browseButton->setMaximumWidth(100);
    connect(m_browseButton, &QPushButton::clicked,
            this, &IconExportDialog::onBrowseOutputClicked);
    pathLayout->addWidget(m_browseButton);

    layout->addLayout(pathLayout);

    // Clipboard buttons
    QHBoxLayout* clipboardLayout = new QHBoxLayout();
    m_copySvgButton = new QPushButton("Copy SVG Code", this);
    connect(m_copySvgButton, &QPushButton::clicked,
            this, &IconExportDialog::onCopySvgCodeClicked);
    clipboardLayout->addWidget(m_copySvgButton);

    m_copyPngButton = new QPushButton("Copy PNG to Clipboard", this);
    connect(m_copyPngButton, &QPushButton::clicked,
            this, &IconExportDialog::onCopyPngClicked);
    clipboardLayout->addWidget(m_copyPngButton);
    clipboardLayout->addStretch();

    layout->addLayout(clipboardLayout);

    group->setLayout(layout);
    return group;
}

QHBoxLayout* IconExportDialog::createButtonSection()
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addStretch();

    m_exportButton = new QPushButton("Export", this);
    m_exportButton->setMinimumWidth(100);
    connect(m_exportButton, &QPushButton::clicked,
            this, &IconExportDialog::onExportClicked);
    layout->addWidget(m_exportButton);

    m_cancelButton = new QPushButton("Cancel", this);
    m_cancelButton->setMinimumWidth(100);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &QDialog::reject);
    layout->addWidget(m_cancelButton);

    return layout;
}

IconOptions IconExportDialog::getCurrentIconOptions() const
{
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

void IconExportDialog::copySvgCodeToClipboard()
{
    // In real implementation, would generate actual SVG code
    // For now, just a placeholder
    QString svgCode = QString("<svg><!-- SVG for %1 --></svg>").arg(m_currentIconName);
    QApplication::clipboard()->setText(svgCode);
}

void IconExportDialog::copyPngToClipboard()
{
    // In real implementation, would render PNG and copy to clipboard
    // For now, just a placeholder
    IconOptions options = getCurrentIconOptions();
    QPixmap pixmap(options.size, options.size);
    pixmap.fill(Qt::transparent);
    QApplication::clipboard()->setPixmap(pixmap);
}

bool IconExportDialog::validateOutputPath() const
{
    return !m_outputPathEdit->text().isEmpty();
}

} // namespace gallery
