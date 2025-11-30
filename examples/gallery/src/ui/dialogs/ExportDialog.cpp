/**
 * @file ExportDialog.cpp
 * @brief Implementation of ExportDialog
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "ExportDialog.h"
#include "BatchExportManager.h"
#include "GalleryTypes.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStandardPaths>
#include <QVBoxLayout>

namespace gallery {

ExportDialog::ExportDialog(QWidget* parent)
    : QDialog(parent), m_exportManager(std::make_unique<BatchExportManager>()),
      m_isExporting(false) {
    setWindowTitle("Batch Export Icons");
    setMinimumWidth(700);
    setMinimumHeight(600);
    setupUI();

    // Connect export manager signals
    connect(m_exportManager.get(), &BatchExportManager::progressChanged, this,
            &ExportDialog::onProgressChanged);
    connect(m_exportManager.get(), &BatchExportManager::exportFinished, this,
            &ExportDialog::onExportFinished);
}

ExportDialog::~ExportDialog() = default;

void ExportDialog::setAvailableIcons(const QStringList& iconNames) {
    m_availableIcons = iconNames;
    m_iconListWidget->clear();

    for (const QString& iconName : iconNames) {
        QListWidgetItem* item = new QListWidgetItem(iconName, m_iconListWidget);
        item->setCheckState(Qt::Checked);
    }

    m_exportAllCheckBox->setChecked(true);
}

void ExportDialog::setLucideInstance(lucide::QtLucide* lucide) {
    m_lucide = lucide;
    m_exportManager->setLucideInstance(lucide);
}

BatchExportSettings ExportDialog::getExportSettings() const {
    BatchExportSettings settings;
    settings.format = static_cast<ExportFormat>(m_formatComboBox->currentIndex());
    settings.size = m_sizeComboBox->currentText().toInt();
    settings.outputDirectory = m_outputDirEdit->text();
    settings.filePattern = m_filePatternEdit->text();
    settings.groupByCategory = m_groupByCategoryCheckBox->isChecked();
    settings.overwriteExisting = true;

    // Collect selected icons
    if (!m_exportAllCheckBox->isChecked()) {
        for (int i = 0; i < m_iconListWidget->count(); ++i) {
            QListWidgetItem* item = m_iconListWidget->item(i);
            if (item->checkState() == Qt::Checked) {
                settings.iconNames.append(item->text());
            }
        }
    } else {
        settings.iconNames = m_availableIcons;
    }

    return settings;
}

void ExportDialog::onExportAllToggled(bool checked) {
    m_iconListWidget->setEnabled(!checked);

    if (checked) {
        // Check all items
        for (int i = 0; i < m_iconListWidget->count(); ++i) {
            m_iconListWidget->item(i)->setCheckState(Qt::Checked);
        }
    }
}

void ExportDialog::onFormatChanged(int index) {
    Q_UNUSED(index);
    // Could adjust available sizes based on format
}

void ExportDialog::onSizeChanged(int index) {
    Q_UNUSED(index);
}

void ExportDialog::onBrowseOutputClicked() {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString selectedDir = QFileDialog::getExistingDirectory(this, "Select Output Directory", dir);

    if (!selectedDir.isEmpty()) {
        m_outputDirEdit->setText(selectedDir);
    }
}

void ExportDialog::onGroupByCategoryToggled(bool checked) {
    Q_UNUSED(checked);
    // Update file pattern example or validation as needed
}

void ExportDialog::onExportFinished(bool success, int exported, int failed,
                                    const QString& errorMessage) {
    setExportingState(false);

    if (success) {
        m_statusLabel->setText(QString("Export completed: %1 icons exported").arg(exported));
        QMessageBox::information(this, "Export Successful",
                                 QString("Successfully exported %1 icons.").arg(exported));
    } else {
        m_statusLabel->setText(QString("Export failed: %1").arg(errorMessage));
        QMessageBox::warning(
            this, "Export Failed",
            QString("Export failed: %1\nExported: %2, Failed: %3")
                .arg(errorMessage, QString::number(exported), QString::number(failed)));
    }
}

void ExportDialog::onProgressChanged(int current, int total) {
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(current);
    m_statusLabel->setText(QString("Exporting: %1 of %2").arg(current).arg(total));
}

void ExportDialog::onExportClicked() {
    if (!validateSettings()) {
        return;
    }

    BatchExportSettings settings = getExportSettings();
    if (settings.iconNames.isEmpty()) {
        QMessageBox::warning(this, "No Icons Selected",
                             "Please select at least one icon to export.");
        return;
    }

    // Start export
    bool started = m_exportManager->exportIcons(settings.iconNames, settings.format, settings.size,
                                                settings.outputDirectory);

    if (started) {
        setExportingState(true);
        m_statusLabel->setText("Starting export...");
    } else {
        QMessageBox::warning(this, "Export Error",
                             "Failed to start export. Another export may be in progress.");
    }
}

void ExportDialog::onCancelClicked() {
    if (m_isExporting) {
        m_exportManager->cancel();
        setExportingState(false);
        m_statusLabel->setText("Export cancelled");
    } else {
        reject();
    }
}

void ExportDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Icon selection section
    mainLayout->addWidget(createIconSelectionSection());

    // Format and size section
    mainLayout->addWidget(createFormatSizeSection());

    // Output section
    mainLayout->addWidget(createOutputSection());

    // Progress section
    mainLayout->addWidget(createProgressSection());

    // Buttons
    mainLayout->addLayout(createButtonSection());

    setLayout(mainLayout);
}

QGroupBox* ExportDialog::createIconSelectionSection() {
    QGroupBox* group = new QGroupBox("Icon Selection", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    m_exportAllCheckBox = new QCheckBox("Export All Icons", this);
    m_exportAllCheckBox->setChecked(true);
    connect(m_exportAllCheckBox, &QCheckBox::toggled, this, &ExportDialog::onExportAllToggled);
    layout->addWidget(m_exportAllCheckBox);

    m_iconListWidget = new QListWidget(this);
    m_iconListWidget->setMaximumHeight(150);
    m_iconListWidget->setEnabled(false);
    layout->addWidget(m_iconListWidget);

    group->setLayout(layout);
    return group;
}

QGroupBox* ExportDialog::createFormatSizeSection() {
    QGroupBox* group = new QGroupBox("Format & Size", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    // Format selection
    QHBoxLayout* formatLayout = new QHBoxLayout();
    formatLayout->addWidget(new QLabel("Format:", this));

    m_formatComboBox = new QComboBox(this);
    m_formatComboBox->addItems({"SVG", "PNG", "ICO"});
    connect(m_formatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ExportDialog::onFormatChanged);
    formatLayout->addWidget(m_formatComboBox);

    formatLayout->addWidget(new QLabel("Size:", this));

    m_sizeComboBox = new QComboBox(this);
    m_sizeComboBox->addItems({"16", "24", "32", "48", "64", "128", "256", "512"});
    m_sizeComboBox->setCurrentText("48");
    connect(m_sizeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ExportDialog::onSizeChanged);
    formatLayout->addWidget(m_sizeComboBox);
    formatLayout->addStretch();

    layout->addLayout(formatLayout);

    group->setLayout(layout);
    return group;
}

QGroupBox* ExportDialog::createOutputSection() {
    QGroupBox* group = new QGroupBox("Output Settings", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    // Output directory
    QHBoxLayout* dirLayout = new QHBoxLayout();
    dirLayout->addWidget(new QLabel("Output Directory:", this));

    m_outputDirEdit = new QLineEdit(this);
    m_outputDirEdit->setPlaceholderText("Select output directory...");
    m_outputDirEdit->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    dirLayout->addWidget(m_outputDirEdit);

    m_browseButton = new QPushButton("Browse...", this);
    m_browseButton->setMaximumWidth(100);
    connect(m_browseButton, &QPushButton::clicked, this, &ExportDialog::onBrowseOutputClicked);
    dirLayout->addWidget(m_browseButton);

    layout->addLayout(dirLayout);

    // Grouping option
    m_groupByCategoryCheckBox = new QCheckBox("Group by Category (create subdirectories)", this);
    connect(m_groupByCategoryCheckBox, &QCheckBox::toggled, this,
            &ExportDialog::onGroupByCategoryToggled);
    layout->addWidget(m_groupByCategoryCheckBox);

    // File pattern
    QHBoxLayout* patternLayout = new QHBoxLayout();
    patternLayout->addWidget(new QLabel("File Pattern:", this));

    m_filePatternEdit = new QLineEdit(this);
    m_filePatternEdit->setText("{name}.{ext}");
    m_filePatternEdit->setToolTip(
        "Use {name} for icon name, {category} for category, {ext} for extension");
    patternLayout->addWidget(m_filePatternEdit);

    layout->addLayout(patternLayout);

    group->setLayout(layout);
    return group;
}

QGroupBox* ExportDialog::createProgressSection() {
    QGroupBox* group = new QGroupBox("Progress", this);
    QVBoxLayout* layout = new QVBoxLayout(group);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    layout->addWidget(m_progressBar);

    m_statusLabel = new QLabel("Ready", this);
    m_statusLabel->setStyleSheet("color: #666666; font-size: 11px;");
    layout->addWidget(m_statusLabel);

    group->setLayout(layout);
    return group;
}

QHBoxLayout* ExportDialog::createButtonSection() {
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addStretch();

    m_exportButton = new QPushButton("Export", this);
    m_exportButton->setMinimumWidth(100);
    connect(m_exportButton, &QPushButton::clicked, this, &ExportDialog::onExportClicked);
    layout->addWidget(m_exportButton);

    m_cancelButton = new QPushButton("Cancel", this);
    m_cancelButton->setMinimumWidth(100);
    connect(m_cancelButton, &QPushButton::clicked, this, &ExportDialog::onCancelClicked);
    layout->addWidget(m_cancelButton);

    return layout;
}

bool ExportDialog::validateSettings() const {
    if (m_outputDirEdit->text().isEmpty()) {
        QMessageBox::warning(const_cast<ExportDialog*>(this), "Invalid Settings",
                             "Please select an output directory.");
        return false;
    }

    return true;
}

void ExportDialog::setExportingState(bool exporting) {
    m_isExporting = exporting;
    m_exportAllCheckBox->setEnabled(!exporting);
    m_iconListWidget->setEnabled(!exporting);
    m_formatComboBox->setEnabled(!exporting);
    m_sizeComboBox->setEnabled(!exporting);
    m_outputDirEdit->setEnabled(!exporting);
    m_browseButton->setEnabled(!exporting);
    m_groupByCategoryCheckBox->setEnabled(!exporting);
    m_filePatternEdit->setEnabled(!exporting);
    m_exportButton->setEnabled(!exporting);

    if (exporting) {
        m_cancelButton->setText("Cancel");
    } else {
        m_cancelButton->setText("Close");
    }
}

} // namespace gallery
