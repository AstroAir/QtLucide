/**
 * QtLucide Gallery Application - Export Dialog Implementation
 */

#include "ExportDialog.h"
#include "../core/BatchExportManager.h"
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
#include <QImageWriter>
#include <QSvgGenerator>
#include <QPdfWriter>
#include <QBuffer>
#include <QDebug>

// Static constants for ExportOptionsWidget
const QList<int> ExportOptionsWidget::PRESET_SIZES = {16, 32, 64, 128, 256, 512};
const QStringList ExportOptionsWidget::TEMPLATE_VARIABLES = {
    "{name}", "{size}", "{width}", "{height}", "{format}", "{date}", "{time}"
};

// ExportOptionsWidget Implementation
ExportOptionsWidget::ExportOptionsWidget(QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_tabWidget(nullptr)
    , m_formatTab(nullptr)
    , m_formatButtonGroup(nullptr)
    , m_sizeTab(nullptr)
    , m_colorTab(nullptr)
    , m_outputTab(nullptr)
    , m_advancedTab(nullptr)

{
    setupUI();

    // Create theme-aware widget
    m_themeWidget = new ThemeAwareWidget(this);

    // Set default configuration
    resetToDefaults();

    // Apply initial theme
    applyTheme();
}

ExportOptionsWidget::~ExportOptionsWidget()
{
}

void ExportOptionsWidget::setExportConfig(const ExportConfig& config)
{
    if (m_config.format == config.format &&
        m_config.sizes == config.sizes &&
        m_config.iconColor == config.iconColor &&
        m_config.outputDirectory == config.outputDirectory) {
        return; // No changes
    }

    m_config = config;

    // Update UI to reflect new config
    updateFormatOptions();
    updateSizeOptions();
    updateColorOptions();
    updatePreview();

    emit configChanged(m_config);
}

ExportConfig ExportOptionsWidget::exportConfig() const
{
    return m_config;
}

void ExportOptionsWidget::resetToDefaults()
{
    ExportConfig defaultConfig;
    defaultConfig.outputDirectory = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    setExportConfig(defaultConfig);
}

bool ExportOptionsWidget::validateConfig() const
{
    // Check if output directory is valid
    if (m_config.outputDirectory.isEmpty()) {
        return false;
    }

    QDir outputDir(m_config.outputDirectory);
    if (!outputDir.exists()) {
        return false;
    }

    // Check if at least one size is selected
    if (m_config.sizes.isEmpty() && !m_config.customSize) {
        return false;
    }

    // Check custom size validity
    if (m_config.customSize) {
        if (m_config.customWidth <= 0 || m_config.customHeight <= 0) {
            return false;
        }
    }

    return true;
}

QStringList ExportOptionsWidget::getValidationErrors() const
{
    QStringList errors;

    if (m_config.outputDirectory.isEmpty()) {
        errors << "Output directory is not specified";
    } else {
        QDir outputDir(m_config.outputDirectory);
        if (!outputDir.exists()) {
            errors << "Output directory does not exist";
        }
    }

    if (m_config.sizes.isEmpty() && !m_config.customSize) {
        errors << "No export sizes selected";
    }

    if (m_config.customSize) {
        if (m_config.customWidth <= 0 || m_config.customHeight <= 0) {
            errors << "Invalid custom size dimensions";
        }
    }

    if (m_config.fileNameTemplate.isEmpty()) {
        errors << "File name template is empty";
    }

    return errors;
}

void ExportOptionsWidget::applyTheme()
{
    if (!ThemeManager::instance()) return;

    // Apply theme to the options widget
    QString optionsStyle = QString(
        "ExportOptionsWidget { "
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

void ExportOptionsWidget::setupUI()
{
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);

    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    m_mainLayout->addWidget(m_tabWidget);

    // Setup tabs
    setupFormatSection();
    setupSizeSection();
    setupColorSection();
    setupOutputSection();
    setupAdvancedSection();
}

void ExportOptionsWidget::setupFormatSection()
{
    m_formatTab = new QWidget();
    m_formatLayout = new QVBoxLayout(m_formatTab);
    m_formatLayout->setContentsMargins(12, 12, 12, 12);
    m_formatLayout->setSpacing(8);

    // Format selection group
    m_formatGroup = new QGroupBox("Export Format", m_formatTab);
    QVBoxLayout* formatGroupLayout = new QVBoxLayout(m_formatGroup);
    formatGroupLayout->setSpacing(6);

    m_formatButtonGroup = new QButtonGroup(this);

    m_pngRadio = new QRadioButton("PNG - Portable Network Graphics", m_formatGroup);
    m_pngRadio->setToolTip("Best for icons with transparency and sharp edges");
    m_pngRadio->setChecked(true);
    m_formatButtonGroup->addButton(m_pngRadio, static_cast<int>(ExportFormat::PNG));
    formatGroupLayout->addWidget(m_pngRadio);

    m_svgRadio = new QRadioButton("SVG - Scalable Vector Graphics", m_formatGroup);
    m_svgRadio->setToolTip("Vector format, perfect for scalable icons");
    m_formatButtonGroup->addButton(m_svgRadio, static_cast<int>(ExportFormat::SVG));
    formatGroupLayout->addWidget(m_svgRadio);

    m_icoRadio = new QRadioButton("ICO - Windows Icon", m_formatGroup);
    m_icoRadio->setToolTip("Windows icon format with multiple sizes");
    m_formatButtonGroup->addButton(m_icoRadio, static_cast<int>(ExportFormat::ICO));
    formatGroupLayout->addWidget(m_icoRadio);

    m_pdfRadio = new QRadioButton("PDF - Portable Document Format", m_formatGroup);
    m_pdfRadio->setToolTip("Vector format suitable for printing");
    m_formatButtonGroup->addButton(m_pdfRadio, static_cast<int>(ExportFormat::PDF));
    formatGroupLayout->addWidget(m_pdfRadio);

    m_jpegRadio = new QRadioButton("JPEG - Joint Photographic Experts Group", m_formatGroup);
    m_jpegRadio->setToolTip("Compressed format, not recommended for icons");
    m_formatButtonGroup->addButton(m_jpegRadio, static_cast<int>(ExportFormat::JPEG));
    formatGroupLayout->addWidget(m_jpegRadio);

    m_formatLayout->addWidget(m_formatGroup);
    m_formatLayout->addStretch();

    // Connect signals
    connect(m_formatButtonGroup, &QButtonGroup::idClicked,
            this, &ExportOptionsWidget::onFormatChanged);

    m_tabWidget->addTab(m_formatTab, "Format");
}

void ExportOptionsWidget::setupSizeSection()
{
    m_sizeTab = new QWidget();
    m_sizeLayout = new QVBoxLayout(m_sizeTab);
    m_sizeLayout->setContentsMargins(12, 12, 12, 12);
    m_sizeLayout->setSpacing(8);

    // Preset sizes group
    m_sizeGroup = new QGroupBox("Export Sizes", m_sizeTab);
    QGridLayout* sizeGroupLayout = new QGridLayout(m_sizeGroup);
    sizeGroupLayout->setSpacing(6);

    m_size16 = new QCheckBox("16×16 px", m_sizeGroup);
    m_size32 = new QCheckBox("32×32 px", m_sizeGroup);
    m_size64 = new QCheckBox("64×64 px", m_sizeGroup);
    m_size128 = new QCheckBox("128×128 px", m_sizeGroup);
    m_size256 = new QCheckBox("256×256 px", m_sizeGroup);
    m_size512 = new QCheckBox("512×512 px", m_sizeGroup);

    // Default to 64x64
    m_size64->setChecked(true);

    sizeGroupLayout->addWidget(m_size16, 0, 0);
    sizeGroupLayout->addWidget(m_size32, 0, 1);
    sizeGroupLayout->addWidget(m_size64, 1, 0);
    sizeGroupLayout->addWidget(m_size128, 1, 1);
    sizeGroupLayout->addWidget(m_size256, 2, 0);
    sizeGroupLayout->addWidget(m_size512, 2, 1);

    m_sizeLayout->addWidget(m_sizeGroup);

    // Custom size group
    QGroupBox* customSizeGroup = new QGroupBox("Custom Size", m_sizeTab);
    QHBoxLayout* customSizeLayout = new QHBoxLayout(customSizeGroup);

    m_customSizeCheck = new QCheckBox("Use custom size:", customSizeGroup);
    m_customWidthSpin = new QSpinBox(customSizeGroup);
    m_customWidthSpin->setRange(1, 2048);
    m_customWidthSpin->setValue(64);
    m_customWidthSpin->setSuffix(" px");
    m_customWidthSpin->setEnabled(false);

    QLabel* xLabel = new QLabel("×", customSizeGroup);

    m_customHeightSpin = new QSpinBox(customSizeGroup);
    m_customHeightSpin->setRange(1, 2048);
    m_customHeightSpin->setValue(64);
    m_customHeightSpin->setSuffix(" px");
    m_customHeightSpin->setEnabled(false);

    m_maintainAspectRatio = new QCheckBox("Maintain aspect ratio", customSizeGroup);
    m_maintainAspectRatio->setChecked(true);
    m_maintainAspectRatio->setEnabled(false);

    customSizeLayout->addWidget(m_customSizeCheck);
    customSizeLayout->addWidget(m_customWidthSpin);
    customSizeLayout->addWidget(xLabel);
    customSizeLayout->addWidget(m_customHeightSpin);
    customSizeLayout->addWidget(m_maintainAspectRatio);
    customSizeLayout->addStretch();

    m_sizeLayout->addWidget(customSizeGroup);
    m_sizeLayout->addStretch();

    // Connect signals
    connect(m_size16, &QCheckBox::toggled, this, &ExportOptionsWidget::onSizeChanged);
    connect(m_size32, &QCheckBox::toggled, this, &ExportOptionsWidget::onSizeChanged);
    connect(m_size64, &QCheckBox::toggled, this, &ExportOptionsWidget::onSizeChanged);
    connect(m_size128, &QCheckBox::toggled, this, &ExportOptionsWidget::onSizeChanged);
    connect(m_size256, &QCheckBox::toggled, this, &ExportOptionsWidget::onSizeChanged);
    connect(m_size512, &QCheckBox::toggled, this, &ExportOptionsWidget::onSizeChanged);
    connect(m_customSizeCheck, &QCheckBox::toggled, this, &ExportOptionsWidget::onCustomSizeToggled);
    connect(m_customWidthSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ExportOptionsWidget::onSizeChanged);
    connect(m_customHeightSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &ExportOptionsWidget::onSizeChanged);

    m_tabWidget->addTab(m_sizeTab, "Size");
}

void ExportOptionsWidget::setupColorSection()
{
    m_colorTab = new QWidget();
    m_colorLayout = new QVBoxLayout(m_colorTab);
    m_colorLayout->setContentsMargins(12, 12, 12, 12);
    m_colorLayout->setSpacing(8);

    // Icon color group
    QGroupBox* iconColorGroup = new QGroupBox("Icon Color", m_colorTab);
    QVBoxLayout* iconColorLayout = new QVBoxLayout(iconColorGroup);

    m_useCustomColorCheck = new QCheckBox("Use custom color", iconColorGroup);
    iconColorLayout->addWidget(m_useCustomColorCheck);

    QHBoxLayout* colorPickerLayout = new QHBoxLayout();
    m_colorPickerButton = new QPushButton("Choose Color", iconColorGroup);
    m_colorPickerButton->setEnabled(false);
    m_colorPreview = new QLabel(iconColorGroup);
    m_colorPreview->setFixedSize(32, 32);
    m_colorPreview->setStyleSheet("border: 1px solid gray; background-color: black;");
    m_colorPreview->setEnabled(false);

    colorPickerLayout->addWidget(m_colorPickerButton);
    colorPickerLayout->addWidget(m_colorPreview);
    colorPickerLayout->addStretch();
    iconColorLayout->addLayout(colorPickerLayout);

    m_colorLayout->addWidget(iconColorGroup);

    // Background group
    QGroupBox* backgroundGroup = new QGroupBox("Background", m_colorTab);
    QVBoxLayout* backgroundLayout = new QVBoxLayout(backgroundGroup);

    m_transparentBackgroundCheck = new QCheckBox("Transparent background", backgroundGroup);
    m_transparentBackgroundCheck->setChecked(true);
    backgroundLayout->addWidget(m_transparentBackgroundCheck);

    QHBoxLayout* backgroundColorLayout = new QHBoxLayout();
    m_backgroundColorButton = new QPushButton("Background Color", backgroundGroup);
    m_backgroundColorButton->setEnabled(false);
    m_backgroundColorPreview = new QLabel(backgroundGroup);
    m_backgroundColorPreview->setFixedSize(32, 32);
    m_backgroundColorPreview->setStyleSheet("border: 1px solid gray; background-color: white;");
    m_backgroundColorPreview->setEnabled(false);

    backgroundColorLayout->addWidget(m_backgroundColorButton);
    backgroundColorLayout->addWidget(m_backgroundColorPreview);
    backgroundColorLayout->addStretch();
    backgroundLayout->addLayout(backgroundColorLayout);

    // Opacity slider
    QHBoxLayout* opacityLayout = new QHBoxLayout();
    QLabel* opacityTitleLabel = new QLabel("Opacity:", backgroundGroup);
    m_opacitySlider = new QSlider(Qt::Horizontal, backgroundGroup);
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setValue(100);
    m_opacityLabel = new QLabel("100%", backgroundGroup);
    m_opacityLabel->setMinimumWidth(40);

    opacityLayout->addWidget(opacityTitleLabel);
    opacityLayout->addWidget(m_opacitySlider);
    opacityLayout->addWidget(m_opacityLabel);
    backgroundLayout->addLayout(opacityLayout);

    m_colorLayout->addWidget(backgroundGroup);
    m_colorLayout->addStretch();

    // Connect signals
    connect(m_useCustomColorCheck, &QCheckBox::toggled, this, &ExportOptionsWidget::onColorChanged);
    connect(m_colorPickerButton, &QPushButton::clicked, this, &ExportOptionsWidget::onColorPickerClicked);
    connect(m_transparentBackgroundCheck, &QCheckBox::toggled, this, &ExportOptionsWidget::onColorChanged);
    connect(m_backgroundColorButton, &QPushButton::clicked, this, &ExportOptionsWidget::onBackgroundColorPickerClicked);
    connect(m_opacitySlider, &QSlider::valueChanged, this, [this](int value) {
        m_opacityLabel->setText(QString("%1%").arg(value));
        onColorChanged();
    });

    m_tabWidget->addTab(m_colorTab, "Color");
}

void ExportOptionsWidget::setupOutputSection()
{
    m_outputTab = new QWidget();
    m_outputLayout = new QVBoxLayout(m_outputTab);
    m_outputLayout->setContentsMargins(12, 12, 12, 12);
    m_outputLayout->setSpacing(8);

    // Output directory group
    QGroupBox* outputDirGroup = new QGroupBox("Output Directory", m_outputTab);
    QVBoxLayout* outputDirLayout = new QVBoxLayout(outputDirGroup);

    QHBoxLayout* dirLayout = new QHBoxLayout();
    m_outputDirectoryEdit = new QLineEdit(outputDirGroup);
    m_outputDirectoryEdit->setPlaceholderText("Select output directory...");
    m_browseButton = new QPushButton("Browse...", outputDirGroup);

    dirLayout->addWidget(m_outputDirectoryEdit);
    dirLayout->addWidget(m_browseButton);
    outputDirLayout->addLayout(dirLayout);

    m_outputLayout->addWidget(outputDirGroup);

    // File naming group
    QGroupBox* namingGroup = new QGroupBox("File Naming", m_outputTab);
    QVBoxLayout* namingLayout = new QVBoxLayout(namingGroup);

    QHBoxLayout* templateLayout = new QHBoxLayout();
    QLabel* templateLabel = new QLabel("File name template:", namingGroup);
    m_fileNameTemplateEdit = new QLineEdit(namingGroup);
    m_fileNameTemplateEdit->setText("{name}_{size}x{size}");
    m_previewTemplateButton = new QPushButton("Preview", namingGroup);

    templateLayout->addWidget(templateLabel);
    templateLayout->addWidget(m_fileNameTemplateEdit);
    templateLayout->addWidget(m_previewTemplateButton);
    namingLayout->addLayout(templateLayout);

    m_templatePreviewLabel = new QLabel("Preview: heart_64x64.png", namingGroup);
    m_templatePreviewLabel->setStyleSheet("color: gray; font-style: italic;");
    namingLayout->addWidget(m_templatePreviewLabel);

    // Subfolder options
    m_createSubfoldersCheck = new QCheckBox("Create subfolders", namingGroup);
    namingLayout->addWidget(m_createSubfoldersCheck);

    QHBoxLayout* subfolderLayout = new QHBoxLayout();
    QLabel* subfolderLabel = new QLabel("Subfolder template:", namingGroup);
    m_subfolderTemplateEdit = new QLineEdit(namingGroup);
    m_subfolderTemplateEdit->setText("{format}");
    m_subfolderTemplateEdit->setEnabled(false);

    subfolderLayout->addWidget(subfolderLabel);
    subfolderLayout->addWidget(m_subfolderTemplateEdit);
    namingLayout->addLayout(subfolderLayout);

    // Other options
    m_overwriteExistingCheck = new QCheckBox("Overwrite existing files", namingGroup);
    namingLayout->addWidget(m_overwriteExistingCheck);

    m_outputLayout->addWidget(namingGroup);
    m_outputLayout->addStretch();

    // Connect signals
    connect(m_browseButton, &QPushButton::clicked, this, &ExportOptionsWidget::onBrowseOutputDirectory);
    connect(m_fileNameTemplateEdit, &QLineEdit::textChanged, this, &ExportOptionsWidget::onFileNameTemplateChanged);
    connect(m_previewTemplateButton, &QPushButton::clicked, this, &ExportOptionsWidget::onPreviewTemplate);
    connect(m_createSubfoldersCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_subfolderTemplateEdit->setEnabled(checked);
        onOutputSettingsChanged();
    });
    connect(m_subfolderTemplateEdit, &QLineEdit::textChanged, this, &ExportOptionsWidget::onOutputSettingsChanged);
    connect(m_overwriteExistingCheck, &QCheckBox::toggled, this, &ExportOptionsWidget::onOutputSettingsChanged);

    m_tabWidget->addTab(m_outputTab, "Output");
}

void ExportOptionsWidget::setupAdvancedSection()
{
    m_advancedTab = new QWidget();
    m_advancedLayout = new QVBoxLayout(m_advancedTab);
    m_advancedLayout->setContentsMargins(12, 12, 12, 12);
    m_advancedLayout->setSpacing(8);

    // Quality settings
    m_qualityGroup = new QGroupBox("Quality Settings", m_advancedTab);
    QVBoxLayout* qualityLayout = new QVBoxLayout(m_qualityGroup);

    // JPEG quality
    QHBoxLayout* jpegQualityLayout = new QHBoxLayout();
    QLabel* jpegQualityTitleLabel = new QLabel("JPEG Quality:", m_qualityGroup);
    m_jpegQualitySlider = new QSlider(Qt::Horizontal, m_qualityGroup);
    m_jpegQualitySlider->setRange(1, 100);
    m_jpegQualitySlider->setValue(90);
    m_jpegQualityLabel = new QLabel("90%", m_qualityGroup);
    m_jpegQualityLabel->setMinimumWidth(40);

    jpegQualityLayout->addWidget(jpegQualityTitleLabel);
    jpegQualityLayout->addWidget(m_jpegQualitySlider);
    jpegQualityLayout->addWidget(m_jpegQualityLabel);
    qualityLayout->addLayout(jpegQualityLayout);

    m_antialiasingCheck = new QCheckBox("Enable antialiasing", m_qualityGroup);
    m_antialiasingCheck->setChecked(true);
    qualityLayout->addWidget(m_antialiasingCheck);

    m_advancedLayout->addWidget(m_qualityGroup);

    // Metadata settings
    m_metadataGroup = new QGroupBox("Metadata", m_advancedTab);
    QVBoxLayout* metadataLayout = new QVBoxLayout(m_metadataGroup);

    m_includeMetadataCheck = new QCheckBox("Include metadata in exported files", m_metadataGroup);
    metadataLayout->addWidget(m_includeMetadataCheck);

    QFormLayout* metadataFormLayout = new QFormLayout();
    m_authorEdit = new QLineEdit(m_metadataGroup);
    m_authorEdit->setEnabled(false);
    m_descriptionEdit = new QLineEdit(m_metadataGroup);
    m_descriptionEdit->setEnabled(false);
    m_copyrightEdit = new QLineEdit(m_metadataGroup);
    m_copyrightEdit->setEnabled(false);

    metadataFormLayout->addRow("Author:", m_authorEdit);
    metadataFormLayout->addRow("Description:", m_descriptionEdit);
    metadataFormLayout->addRow("Copyright:", m_copyrightEdit);
    metadataLayout->addLayout(metadataFormLayout);

    m_advancedLayout->addWidget(m_metadataGroup);
    m_advancedLayout->addStretch();

    // Connect signals
    connect(m_jpegQualitySlider, &QSlider::valueChanged, this, [this](int value) {
        m_jpegQualityLabel->setText(QString("%1%").arg(value));
    });
    connect(m_includeMetadataCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_authorEdit->setEnabled(checked);
        m_descriptionEdit->setEnabled(checked);
        m_copyrightEdit->setEnabled(checked);
    });

    m_tabWidget->addTab(m_advancedTab, "Advanced");
}

// Minimal stub implementations for ExportOptionsWidget slots
void ExportOptionsWidget::onFormatChanged() { emit configChanged(m_config); }
void ExportOptionsWidget::onSizeChanged() { emit configChanged(m_config); }
void ExportOptionsWidget::onCustomSizeToggled() { emit configChanged(m_config); }
void ExportOptionsWidget::onColorChanged() { emit configChanged(m_config); }
void ExportOptionsWidget::onColorPickerClicked() {}
void ExportOptionsWidget::onBackgroundColorPickerClicked() {}
void ExportOptionsWidget::onBrowseOutputDirectory() {}
void ExportOptionsWidget::onFileNameTemplateChanged() { emit configChanged(m_config); }
void ExportOptionsWidget::onPreviewTemplate() {}
void ExportOptionsWidget::updatePreview() {}

// Missing ExportOptionsWidget methods
void ExportOptionsWidget::onOutputSettingsChanged() {
    emit configChanged(m_config);
}

void ExportOptionsWidget::onPresetSizeToggled() {
    emit configChanged(m_config);
}

void ExportOptionsWidget::updateFormatOptions() {
    // Update UI based on selected format
}

void ExportOptionsWidget::updateSizeOptions() {
    // Update size options UI
}

void ExportOptionsWidget::updateColorOptions() {
    // Update color options UI
}

void ExportOptionsWidget::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
}

// ExportPreviewWidget Implementation
ExportPreviewWidget::ExportPreviewWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

ExportPreviewWidget::~ExportPreviewWidget() = default;

void ExportPreviewWidget::setupUI() {
    setMinimumSize(300, 400);
}

void ExportPreviewWidget::onConfigChanged() {
    // Update preview based on config changes
    update();
}

void ExportPreviewWidget::updateLayout() {
    // Update layout based on current configuration
}

void ExportPreviewWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.fillRect(rect(), QColor(240, 240, 240));
    painter.drawText(rect(), Qt::AlignCenter, "Preview");
}

void ExportPreviewWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateLayout();
}

void ExportPreviewWidget::mousePressEvent(QMouseEvent* event) {
    QWidget::mousePressEvent(event);
}

void ExportPreviewWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    QWidget::mouseDoubleClickEvent(event);
}



// ExportDialog Implementation
ExportDialog::ExportDialog(QWidget* parent)
    : QDialog(parent)
    , m_mainLayout(nullptr)
    , m_contentLayout(nullptr)
    , m_leftLayout(nullptr)
    , m_rightLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_iconCountLabel(nullptr)
    , m_optionsWidget(nullptr)
    , m_previewWidget(nullptr)
    , m_progressWidget(nullptr)
    , m_buttonLayout(nullptr)
    , m_exportButton(nullptr)
    , m_cancelButton(nullptr)
    , m_closeButton(nullptr)
    , m_exportInProgress(false)
    , m_exportSuccessful(false)
    , m_lucide(nullptr)
    , m_themeManager(nullptr)
    , m_exportManager(nullptr)
    , m_themeWidget(nullptr)
{
    setupUI();
    setupLayout();
    setupConnections();
}

ExportDialog::~ExportDialog() = default;

void ExportDialog::setupUI() {
    setWindowTitle("Export Icons");
    setModal(true);
    resize(800, 600);

    // Create main components
    m_titleLabel = new QLabel("Export Icons", this);
    m_iconCountLabel = new QLabel("0 icons selected", this);
    m_optionsWidget = new ExportOptionsWidget(this);
    m_previewWidget = new ExportPreviewWidget(this);
    m_progressWidget = new ExportProgressWidget(this);

    // Create buttons
    m_exportButton = new QPushButton("Export", this);
    m_cancelButton = new QPushButton("Cancel", this);
    m_closeButton = new QPushButton("Close", this);

    m_progressWidget->hide();
}

void ExportDialog::setupLayout() {
    m_mainLayout = new QVBoxLayout(this);
    m_contentLayout = new QHBoxLayout();
    m_leftLayout = new QVBoxLayout();
    m_rightLayout = new QVBoxLayout();
    m_buttonLayout = new QHBoxLayout();

    // Left side - options
    m_leftLayout->addWidget(m_titleLabel);
    m_leftLayout->addWidget(m_iconCountLabel);
    m_leftLayout->addWidget(m_optionsWidget);

    // Right side - preview
    m_rightLayout->addWidget(m_previewWidget);

    // Content layout
    m_contentLayout->addLayout(m_leftLayout, 1);
    m_contentLayout->addLayout(m_rightLayout, 1);

    // Button layout
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_exportButton);
    m_buttonLayout->addWidget(m_cancelButton);
    m_buttonLayout->addWidget(m_closeButton);

    // Main layout
    m_mainLayout->addLayout(m_contentLayout);
    m_mainLayout->addWidget(m_progressWidget);
    m_mainLayout->addLayout(m_buttonLayout);
}

void ExportDialog::setupConnections() {
    connect(m_exportButton, &QPushButton::clicked, this, &ExportDialog::onExportClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &ExportDialog::onCancelClicked);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::reject);

    if (m_optionsWidget) {
        connect(m_optionsWidget, &ExportOptionsWidget::configChanged,
                this, &ExportDialog::onConfigChanged);
    }
}

// ExportDialog slot implementations
void ExportDialog::onExportClicked() {
    if (validateExportConfig()) {
        startExport();
    }
}

void ExportDialog::onCancelClicked() {
    if (m_exportInProgress) {
        cancelExport();
    } else {
        reject();
    }
}

void ExportDialog::onConfigChanged(const ExportConfig& config) {
    m_config = config;
    updateUI();
}

void ExportDialog::onExportProgress(int current, int total, const QString& currentFile) {
    emit exportProgress(current, total, currentFile);
    if (m_progressWidget) {
        // Update progress widget
    }
}

void ExportDialog::onExportFinished(bool success) {
    m_exportInProgress = false;
    m_exportSuccessful = success;

    if (success) {
        emit exportFinished(true, m_exportedFiles);
        accept();
    } else {
        emit exportFinished(false, QStringList());
    }
}

void ExportDialog::onPreviewIconClicked(const QString& iconName) {
    // Handle preview icon click
    Q_UNUSED(iconName)
}

// ExportDialog utility methods
void ExportDialog::updateUI() {
    if (m_iconCountLabel) {
        m_iconCountLabel->setText(QString("%1 icons selected").arg(m_iconNames.size()));
    }
}

bool ExportDialog::validateExportConfig() {
    return !m_iconNames.isEmpty();
}

void ExportDialog::startExport() {
    m_exportInProgress = true;
    m_exportSuccessful = false;
    m_exportedFiles.clear();

    if (m_progressWidget) {
        m_progressWidget->show();
    }

    emit exportStarted();

    // Simulate export completion for now
    QTimer::singleShot(1000, this, [this]() {
        onExportFinished(true);
    });
}

void ExportDialog::cancelExport() {
    m_exportInProgress = false;
    emit exportCancelled();

    if (m_progressWidget) {
        m_progressWidget->hide();
    }
}

// ExportDialog public methods
void ExportDialog::setIconNames(const QStringList& iconNames) {
    m_iconNames = iconNames;
    updateUI();
}

QStringList ExportDialog::iconNames() const {
    return m_iconNames;
}

void ExportDialog::addIconName(const QString& iconName) {
    if (!m_iconNames.contains(iconName)) {
        m_iconNames.append(iconName);
        updateUI();
    }
}

void ExportDialog::removeIconName(const QString& iconName) {
    m_iconNames.removeAll(iconName);
    updateUI();
}

void ExportDialog::clearIconNames() {
    m_iconNames.clear();
    updateUI();
}

void ExportDialog::setExportConfig(const ExportConfig& config) {
    m_config = config;
    if (m_optionsWidget) {
        m_optionsWidget->setExportConfig(config);
    }
}

ExportConfig ExportDialog::exportConfig() const {
    return m_config;
}

#ifdef QTLUCIDE_AVAILABLE
void ExportDialog::setLucide(lucide::QtLucide* lucide) {
    m_lucide = lucide;
}
#else
void ExportDialog::setLucide(void* lucide) {
    m_lucide = lucide;
}
#endif

void ExportDialog::setThemeManager(ThemeManager* themeManager) {
    m_themeManager = themeManager;
    applyTheme();
}

void ExportDialog::applyTheme() {
    if (m_themeManager) {
        // Apply theme styling
    }
}

// ExportDialog event handlers
void ExportDialog::showEvent(QShowEvent* event) {
    QDialog::showEvent(event);
    updateUI();
}

void ExportDialog::closeEvent(QCloseEvent* event) {
    if (m_exportInProgress) {
        cancelExport();
    }
    QDialog::closeEvent(event);
}

void ExportDialog::accept() {
    QDialog::accept();
}

void ExportDialog::reject() {
    if (m_exportInProgress) {
        cancelExport();
    }
    QDialog::reject();
}
