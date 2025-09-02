/**
 * QtLucide Basic Usage Example - Main Window Implementation
 */

#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_lucide(nullptr)
    , m_centralWidget(nullptr)
    , m_currentColor(Qt::black)
    , m_iconSize(32)
    , m_scaleFactor(0.9)
{
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    if (!m_lucide->initLucide()) {
        QMessageBox::critical(this, "Error", "Failed to initialize QtLucide");
        return;
    }
    
    // Setup sample icons to display
    m_sampleIcons << "activity" << "alert-circle" << "archive" << "arrow-right"
                  << "bell" << "bookmark" << "calendar" << "camera" << "check"
                  << "clock" << "cloud" << "code" << "coffee" << "copy"
                  << "download" << "edit" << "eye" << "file" << "folder"
                  << "heart" << "home" << "info" << "lock" << "mail"
                  << "map-pin" << "menu" << "message-circle" << "phone" << "play"
                  << "plus" << "refresh-cw" << "save" << "search" << "settings"
                  << "share" << "star" << "trash" << "user" << "x";
    
    setupUI();
    createIconGrid();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
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

void MainWindow::setupControls()
{
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
    connect(m_sizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onIconSizeChanged);
    
    // Scale factor control
    QLabel* scaleLabel = new QLabel("Scale:", this);
    m_scaleSpinBox = new QSpinBox(this);
    m_scaleSpinBox->setRange(50, 150);
    m_scaleSpinBox->setValue(static_cast<int>(m_scaleFactor * 100));
    m_scaleSpinBox->setSuffix(" %");
    connect(m_scaleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int value) { onScaleFactorChanged(value / 100.0); });
    
    controlsLayout->addWidget(m_colorButton);
    controlsLayout->addWidget(sizeLabel);
    controlsLayout->addWidget(m_sizeSpinBox);
    controlsLayout->addWidget(scaleLabel);
    controlsLayout->addWidget(m_scaleSpinBox);
    controlsLayout->addStretch();
}

void MainWindow::onColorButtonClicked()
{
    QColor color = QColorDialog::getColor(m_currentColor, this, "Choose Icon Color");
    if (color.isValid()) {
        m_currentColor = color;
        m_colorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
        updateIconColors();
    }
}

void MainWindow::onIconSizeChanged(int size)
{
    m_iconSize = size;
    createIconGrid();
}

void MainWindow::onScaleFactorChanged(double factor)
{
    m_scaleFactor = factor;
    m_lucide->setDefaultOption("scale-factor", factor);
    createIconGrid();
}

void MainWindow::updateIconColors()
{
    m_lucide->setDefaultOption("color", m_currentColor);
    createIconGrid();
}

void MainWindow::createIconGrid()
{
    // Clear existing icons
    QLayoutItem* item;
    while ((item = m_iconLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // Create new icon grid
    int columns = 8;
    int row = 0, col = 0;
    
    for (const QString& iconName : m_sampleIcons) {
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
            
            col++;
            if (col >= columns) {
                col = 0;
                row++;
            }
        }
    }
    
    // Add stretch to fill remaining space
    m_iconLayout->setRowStretch(row + 1, 1);
}
