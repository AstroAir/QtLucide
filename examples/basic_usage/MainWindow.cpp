/**
 * QtLucide Basic Usage Example - Main Window Implementation
 */

#include "MainWindow.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_lucide(nullptr), m_centralWidget(nullptr), m_currentColor(Qt::black),
      m_iconSize(32), m_scaleFactor(0.9), m_showAllIcons(false) {
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    if (!m_lucide->initLucide()) {
        QMessageBox::critical(this, "Error", "Failed to initialize QtLucide");
        return;
    }

    // Setup sample icons to display - use a larger subset for better demonstration
    // Note: Icon names must match exactly with Lucide icon names (kebab-case)
    // Fixed: alert-circle -> circle-alert, bar-chart -> chart-bar, pie-chart -> chart-pie
    // Fixed: edit -> pencil, home -> house
    m_sampleIcons << "activity" << "circle-alert" << "archive" << "arrow-right"
                  << "bell" << "bookmark" << "calendar" << "camera" << "check"
                  << "clock" << "cloud" << "code" << "coffee" << "copy"
                  << "download" << "pencil" << "eye" << "file" << "folder"
                  << "heart" << "house" << "info" << "lock" << "mail"
                  << "map-pin" << "menu" << "message-circle" << "phone" << "play"
                  << "plus" << "refresh-cw" << "save" << "search" << "settings"
                  << "share" << "star" << "trash" << "user"
                  << "x"
                  // Add more icons to demonstrate the fix
                  << "shield" << "sun" << "moon" << "battery" << "wifi" << "bluetooth"
                  << "volume-2" << "mic" << "headphones" << "camera-off" << "video"
                  << "image" << "music" << "film" << "book" << "bookmark-plus"
                  << "tag" << "flag" << "map" << "compass" << "navigation"
                  << "target" << "award" << "gift" << "shopping-cart" << "credit-card"
                  << "dollar-sign" << "trending-up" << "chart-bar" << "chart-pie" << "activity"
                  << "zap" << "cpu" << "hard-drive" << "smartphone" << "tablet"
                  << "laptop" << "monitor" << "server" << "database" << "cloud-upload"
                  << "cloud-download" << "upload" << "inbox" << "send";

    setupUI();
    validateIconNames();
    createIconGrid();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    setWindowTitle("QtLucide Example - Lucide Icons for Qt");
    setMinimumSize(800, 600);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);

    setupControls();

    // Create icons group
    m_iconsGroup = new QGroupBox("Sample Icons", this);
    m_scrollArea = new QScrollArea(this);
    m_iconContainer = new QWidget();
    m_iconLayout = new QGridLayout(m_iconContainer);

    m_scrollArea->setWidget(m_iconContainer);
    m_scrollArea->setWidgetResizable(true);

    QVBoxLayout* iconsLayout = new QVBoxLayout(m_iconsGroup);
    iconsLayout->addWidget(m_scrollArea);

    m_mainLayout->addWidget(m_controlsGroup);
    m_mainLayout->addWidget(m_iconsGroup, 1);
}

void MainWindow::setupControls() {
    m_controlsGroup = new QGroupBox("Icon Controls", this);
    QHBoxLayout* controlsLayout = new QHBoxLayout(m_controlsGroup);

    // Color control
    m_colorButton = new QPushButton("Choose Color", this);
    m_colorButton->setStyleSheet(QString("background-color: %1").arg(m_currentColor.name()));
    connect(m_colorButton, &QPushButton::clicked, this, &MainWindow::onColorButtonClicked);

    // Size control
    QLabel* sizeLabel = new QLabel("Size:", this);
    m_sizeSpinBox = new QSpinBox(this);
    m_sizeSpinBox->setRange(16, 128);
    m_sizeSpinBox->setValue(m_iconSize);
    m_sizeSpinBox->setSuffix(" px");
    connect(m_sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::onIconSizeChanged);

    // Scale factor control
    QLabel* scaleLabel = new QLabel("Scale:", this);
    m_scaleSpinBox = new QSpinBox(this);
    m_scaleSpinBox->setRange(50, 150);
    m_scaleSpinBox->setValue(static_cast<int>(m_scaleFactor * 100));
    m_scaleSpinBox->setSuffix(" %");
    connect(m_scaleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [this](int value) { onScaleFactorChanged(value / 100.0); });

    // Search control
    QLabel* searchLabel = new QLabel("Search:", this);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Filter icons...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    // Show all icons checkbox
    m_showAllCheckBox = new QCheckBox("Show all icons", this);
    connect(m_showAllCheckBox, &QCheckBox::toggled, this, &MainWindow::onShowAllIconsToggled);

    controlsLayout->addWidget(m_colorButton);
    controlsLayout->addWidget(sizeLabel);
    controlsLayout->addWidget(m_sizeSpinBox);
    controlsLayout->addWidget(scaleLabel);
    controlsLayout->addWidget(m_scaleSpinBox);
    controlsLayout->addWidget(searchLabel);
    controlsLayout->addWidget(m_searchEdit);
    controlsLayout->addWidget(m_showAllCheckBox);
    controlsLayout->addStretch();
}

void MainWindow::onColorButtonClicked() {
    QColor color = QColorDialog::getColor(m_currentColor, this, "Choose Icon Color");
    if (color.isValid()) {
        m_currentColor = color;
        m_colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        updateIconColors();
    }
}

void MainWindow::onIconSizeChanged(int size) {
    m_iconSize = size;
    createIconGrid();
}

void MainWindow::onScaleFactorChanged(double factor) {
    m_scaleFactor = factor;
    m_lucide->setDefaultOption("scale-factor", factor);
    createIconGrid();
}

void MainWindow::updateIconColors() {
    m_lucide->setDefaultOption("color", m_currentColor);
    createIconGrid();
}

void MainWindow::onSearchTextChanged(const QString& text) {
    m_searchFilter = text.toLower();
    createIconGrid();
}

void MainWindow::onShowAllIconsToggled(bool checked) {
    m_showAllIcons = checked;
    createIconGrid();
}

void MainWindow::createIconGrid() {
    // Clear existing icons
    QLayoutItem* item;
    while ((item = m_iconLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Determine which icons to display
    QStringList iconsToDisplay;
    if (m_showAllIcons) {
        iconsToDisplay = m_lucide->availableIcons();
    } else {
        iconsToDisplay = m_sampleIcons;
    }

    // Apply search filter if any
    if (!m_searchFilter.isEmpty()) {
        QStringList filteredIcons;
        for (const QString& iconName : iconsToDisplay) {
            if (iconName.toLower().contains(m_searchFilter)) {
                filteredIcons.append(iconName);
            }
        }
        iconsToDisplay = filteredIcons;
    }

    // Create new icon grid
    int columns = 8;
    int row = 0, col = 0;
    int loadedCount = 0;
    int failedCount = 0;

    for (const QString& iconName : iconsToDisplay) {
        // Create button with icon
        QPushButton* button = new QPushButton(this);
        QIcon icon = m_lucide->icon(iconName);

        if (!icon.isNull()) {
            button->setIcon(icon);
            button->setIconSize(QSize(m_iconSize, m_iconSize));
            button->setFixedSize(m_iconSize + 20, m_iconSize + 20);
            button->setToolTip(iconName);

            // Add click handler to show icon info
            connect(button, &QPushButton::clicked, this, [this, iconName]() {
                QMessageBox::information(this, "Icon Info",
                                         QString("Icon: %1\nSize: %2x%2 px\nColor: %3\nScale: %4%")
                                             .arg(iconName)
                                             .arg(m_iconSize)
                                             .arg(m_currentColor.name())
                                             .arg(static_cast<int>(m_scaleFactor * 100)));
            });

            m_iconLayout->addWidget(button, row, col);
            loadedCount++;

            col++;
            if (col >= columns) {
                col = 0;
                row++;
            }
        } else {
            qWarning() << "Failed to load icon:" << iconName;
            failedCount++;
        }
    }

    // Update window title with loading statistics
    QString titleSuffix;
    if (m_showAllIcons) {
        titleSuffix = QString(" (All Icons)");
    } else {
        titleSuffix = QString(" (Sample Icons)");
    }

    if (!m_searchFilter.isEmpty()) {
        titleSuffix += QString(" - Filter: '%1'").arg(m_searchFilter);
    }

    setWindowTitle(QString("QtLucide Example - Loaded: %1/%2 icons%3")
                       .arg(loadedCount)
                       .arg(iconsToDisplay.size())
                       .arg(titleSuffix));

    // Log summary if there were any failures
    if (failedCount > 0) {
        qWarning() << "Failed to load" << failedCount << "out of" << iconsToDisplay.size()
                   << "icons";
    } else {
        qDebug() << "Successfully loaded all" << loadedCount << "icons"
                 << (m_showAllIcons ? "(all available)" : "(sample set)");
    }

    // Add stretch to fill remaining space
    m_iconLayout->setRowStretch(row + 1, 1);
}

void MainWindow::validateIconNames() {
    // Get list of all available icons for validation
    QStringList availableIcons = m_lucide->availableIcons();
    QStringList invalidIcons;

    qDebug() << "Validating" << m_sampleIcons.size() << "icon names...";

    for (const QString& iconName : m_sampleIcons) {
        if (!availableIcons.contains(iconName)) {
            invalidIcons.append(iconName);
        }
    }

    if (!invalidIcons.isEmpty()) {
        qWarning() << "Found" << invalidIcons.size() << "invalid icon names:" << invalidIcons;
        qDebug() << "Available icons count:" << availableIcons.size();

        // Suggest similar icon names for common mistakes
        for (const QString& invalid : invalidIcons) {
            QStringList suggestions;
            for (const QString& available : availableIcons) {
                if (available.contains(invalid.split('-').first()) ||
                    invalid.contains(available.split('-').first())) {
                    suggestions.append(available);
                    if (suggestions.size() >= 3)
                        break; // Limit suggestions
                }
            }
            if (!suggestions.isEmpty()) {
                qDebug() << "Suggestions for '" << invalid << "':" << suggestions;
            }
        }
    } else {
        qDebug() << "✓ All icon names are valid!";
    }
}
