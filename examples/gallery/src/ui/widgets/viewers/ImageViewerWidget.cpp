/**
 * @file ImageViewerWidget.cpp
 * @brief Implementation of ImageViewerWidget
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "ImageViewerWidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSlider>
#include <QVBoxLayout>

namespace gallery {

ImageViewerWidget::ImageViewerWidget(QWidget* parent)
    : QWidget(parent), m_zoomLevel(100), m_backgroundMode(BackgroundMode::Transparent) {
    setupUI();
}

ImageViewerWidget::~ImageViewerWidget() = default;

void ImageViewerWidget::setImage(const QPixmap& pixmap) {
    m_originalPixmap = pixmap;
    m_zoomLevel = 100;
    m_zoomSlider->setValue(100);
    updateDisplay();
}

void ImageViewerWidget::setZoomLevel(int zoomPercent) {
    if (zoomPercent < 50 || zoomPercent > 400) {
        return;
    }

    m_zoomLevel = zoomPercent;
    m_zoomSlider->blockSignals(true);
    m_zoomSlider->setValue(zoomPercent);
    m_zoomSlider->blockSignals(false);
    updateDisplay();
    emit zoomChanged(m_zoomLevel);
}

int ImageViewerWidget::getZoomLevel() const {
    return m_zoomLevel;
}

void ImageViewerWidget::setBackgroundMode(BackgroundMode mode) {
    m_backgroundMode = mode;
    m_backgroundCombo->blockSignals(true);
    m_backgroundCombo->setCurrentIndex(static_cast<int>(mode));
    m_backgroundCombo->blockSignals(false);
    updateDisplay();
    emit backgroundChanged(m_backgroundMode);
}

BackgroundMode ImageViewerWidget::getBackgroundMode() const {
    return m_backgroundMode;
}

void ImageViewerWidget::setImageInfo(const QString& info) {
    m_infoLabel->setText(info);
}

void ImageViewerWidget::onZoomIn() {
    int newZoom = m_zoomLevel + 10;
    if (newZoom > 400) {
        newZoom = 400;
    }
    setZoomLevel(newZoom);
}

void ImageViewerWidget::onZoomOut() {
    int newZoom = m_zoomLevel - 10;
    if (newZoom < 50) {
        newZoom = 50;
    }
    setZoomLevel(newZoom);
}

void ImageViewerWidget::onZoomSliderChanged(int value) {
    m_zoomLevel = value;
    m_zoomLabel->setText(QString("%1%").arg(m_zoomLevel));
    updateDisplay();
    emit zoomChanged(m_zoomLevel);
}

void ImageViewerWidget::onBackgroundChanged(int index) {
    m_backgroundMode = static_cast<BackgroundMode>(index);
    updateDisplay();
    emit backgroundChanged(m_backgroundMode);
}

void ImageViewerWidget::onZoomReset() {
    setZoomLevel(100);
}

void ImageViewerWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Info label
    m_infoLabel = new QLabel(this);
    m_infoLabel->setStyleSheet("color: #666666; font-size: 11px;");
    mainLayout->addWidget(m_infoLabel);

    // Viewer area
    QWidget* viewerContainer = new QWidget(this);
    QVBoxLayout* viewerLayout = new QVBoxLayout(viewerContainer);
    viewerLayout->setContentsMargins(0, 0, 0, 0);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setMinimumHeight(300);
    m_imageLabel->setStyleSheet("background-color: #f5f5f5; border: 1px solid #ddd;");
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    viewerLayout->addWidget(m_imageLabel);

    mainLayout->addWidget(viewerContainer, 1);

    // Zoom controls
    QHBoxLayout* zoomLayout = new QHBoxLayout();

    m_zoomOutButton = new QPushButton("Zoom -", this);
    m_zoomOutButton->setMaximumWidth(80);
    connect(m_zoomOutButton, &QPushButton::clicked, this, &ImageViewerWidget::onZoomOut);
    zoomLayout->addWidget(m_zoomOutButton);

    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setMinimum(50);
    m_zoomSlider->setMaximum(400);
    m_zoomSlider->setValue(100);
    m_zoomSlider->setTickPosition(QSlider::TicksBelow);
    m_zoomSlider->setTickInterval(50);
    connect(m_zoomSlider, &QSlider::valueChanged, this, &ImageViewerWidget::onZoomSliderChanged);
    zoomLayout->addWidget(m_zoomSlider);

    m_zoomInButton = new QPushButton("Zoom +", this);
    m_zoomInButton->setMaximumWidth(80);
    connect(m_zoomInButton, &QPushButton::clicked, this, &ImageViewerWidget::onZoomIn);
    zoomLayout->addWidget(m_zoomInButton);

    m_zoomResetButton = new QPushButton("Reset", this);
    m_zoomResetButton->setMaximumWidth(80);
    connect(m_zoomResetButton, &QPushButton::clicked, this, &ImageViewerWidget::onZoomReset);
    zoomLayout->addWidget(m_zoomResetButton);

    m_zoomLabel = new QLabel("100%", this);
    m_zoomLabel->setMinimumWidth(40);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
    zoomLayout->addWidget(m_zoomLabel);

    mainLayout->addLayout(zoomLayout);

    // Background controls
    QHBoxLayout* bgLayout = new QHBoxLayout();
    bgLayout->addWidget(new QLabel("Background:", this));

    m_backgroundCombo = new QComboBox(this);
    m_backgroundCombo->addItems({"Transparent", "White", "Black"});
    connect(m_backgroundCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &ImageViewerWidget::onBackgroundChanged);
    bgLayout->addWidget(m_backgroundCombo);
    bgLayout->addStretch();

    mainLayout->addLayout(bgLayout);

    setLayout(mainLayout);
}

void ImageViewerWidget::updateDisplay() {
    if (m_originalPixmap.isNull()) {
        m_imageLabel->clear();
        return;
    }

    // Calculate scaled size
    int newWidth = (m_originalPixmap.width() * m_zoomLevel) / 100;
    int newHeight = (m_originalPixmap.height() * m_zoomLevel) / 100;

    // Scale the pixmap
    QPixmap scaled = m_originalPixmap.scaledToWidth(newWidth, Qt::SmoothTransformation);

    // Apply background
    QPixmap displayed;
    switch (m_backgroundMode) {
        case BackgroundMode::Transparent:
            displayed = createCheckerPattern(newWidth);
            {
                QPainter painter(&displayed);
                painter.drawPixmap(0, 0, scaled);
            }
            break;
        case BackgroundMode::White:
            displayed = applyBackground(scaled, Qt::white);
            break;
        case BackgroundMode::Black:
            displayed = applyBackground(scaled, Qt::black);
            break;
    }

    m_imageLabel->setPixmap(displayed);
}

QPixmap ImageViewerWidget::createCheckerPattern(int size) const {
    QPixmap pattern(size, size);
    pattern.fill(Qt::white);

    QPainter painter(&pattern);
    const int squareSize = 8;
    const QColor darkSquare(220, 220, 220);

    for (int x = 0; x < size; x += squareSize) {
        for (int y = 0; y < size; y += squareSize) {
            if (((x / squareSize) + (y / squareSize)) % 2 == 0) {
                painter.fillRect(x, y, squareSize, squareSize, darkSquare);
            }
        }
    }

    return pattern;
}

QPixmap ImageViewerWidget::applyBackground(const QPixmap& source, const QColor& bg) const {
    QPixmap result(source.width(), source.height());
    result.fill(bg);

    QPainter painter(&result);
    painter.drawPixmap(0, 0, source);

    return result;
}

} // namespace gallery
