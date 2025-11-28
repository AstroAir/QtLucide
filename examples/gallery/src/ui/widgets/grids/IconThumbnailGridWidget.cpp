/**
 * @file IconThumbnailGridWidget.cpp
 * @brief Implementation of IconThumbnailGridWidget
 * @details Handles rendering of icon previews at multiple sizes
 */

#include "IconThumbnailGridWidget.h"

#include <QtLucide/QtLucide.h>

#include <QPainter>
#include <QDebug>
#include <algorithm>

namespace gallery {

IconThumbnailGridWidget::IconThumbnailGridWidget(QWidget* parent)
    : QWidget(parent)
    , m_lucide(nullptr)
    , m_iconName()
    , m_previewSizes{{24, 48, 96, 192}}
    , m_spacing(8)
    , m_columns(2)
    , m_preferredWidth(200)
    , m_preferredHeight(200) {
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setStyleSheet("QWidget { background-color: transparent; }");
    recalculateLayout();
}

IconThumbnailGridWidget::~IconThumbnailGridWidget() = default;

void IconThumbnailGridWidget::setIconName(const QString& iconName) {
    if (m_iconName != iconName) {
        m_iconName = iconName;
        update();
    }
}

QString IconThumbnailGridWidget::iconName() const {
    return m_iconName;
}

void IconThumbnailGridWidget::setPreviewSizes(const QList<int>& sizes) {
    if (m_previewSizes != sizes) {
        m_previewSizes = sizes;
        // Clamp sizes to reasonable range
        for (int& size : m_previewSizes) {
            size = std::clamp(size, 16, 256);
        }
        recalculateLayout();
        updateGeometry();
    }
}

QList<int> IconThumbnailGridWidget::previewSizes() const {
    return m_previewSizes;
}

void IconThumbnailGridWidget::setLucideInstance(lucide::QtLucide* lucide) {
    m_lucide = lucide;
}

void IconThumbnailGridWidget::setSpacing(int spacing) {
    spacing = std::max(0, spacing);
    if (m_spacing != spacing) {
        m_spacing = spacing;
        recalculateLayout();
        updateGeometry();
    }
}

int IconThumbnailGridWidget::spacing() const {
    return m_spacing;
}

void IconThumbnailGridWidget::paintEvent(QPaintEvent* event) {
    if (!m_lucide || m_iconName.isEmpty() || m_previewSizes.isEmpty()) {
        QWidget::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    // Get the icon once
    QIcon icon = m_lucide->icon(m_iconName);
    if (icon.isNull()) {
        QWidget::paintEvent(event);
        return;
    }

    // Calculate layout
    int cellWidth = (width() - m_spacing * (m_columns + 1)) / m_columns;
    int cellWidth_clamped = std::max(24, cellWidth);

    // Draw thumbnails
    for (int i = 0; i < m_previewSizes.size(); ++i) {
        int row = i / m_columns;
        int col = i % m_columns;

        int x = col * (cellWidth_clamped + m_spacing) + m_spacing;
        int y = row * (cellWidth_clamped + m_spacing + 20) + m_spacing;

        int size = m_previewSizes.at(i);
        int actualSize = std::min({size, cellWidth_clamped, m_previewSizes.at(i)});

        // Center the icon in the cell
        int iconX = x + (cellWidth_clamped - actualSize) / 2;
        int iconY = y;

        QRect iconRect(iconX, iconY, actualSize, actualSize);

        // Draw background
        painter.fillRect(iconRect.adjusted(-2, -2, 2, 2), QColor(240, 240, 240));
        painter.drawRect(iconRect.adjusted(-2, -2, 2, 2));

        // Draw icon
        QPixmap pixmap = icon.pixmap(QSize(actualSize, actualSize));
        if (!pixmap.isNull()) {
            painter.drawPixmap(iconRect, pixmap);
        }

        // Draw size label
        QRect labelRect(x, iconY + actualSize + 4, cellWidth_clamped, 16);
        painter.drawText(labelRect, Qt::AlignHCenter | Qt::AlignTop, QString::number(size));
    }

    painter.end();
}

QSize IconThumbnailGridWidget::sizeHint() const {
    return QSize(m_preferredWidth, m_preferredHeight);
}

QSize IconThumbnailGridWidget::minimumSizeHint() const {
    if (m_previewSizes.isEmpty()) {
        return QSize(100, 100);
    }
    int minWidth = m_columns * 32 + (m_columns + 1) * m_spacing;
    int rows = (m_previewSizes.size() + m_columns - 1) / m_columns;
    int minHeight = rows * (32 + 20) + m_spacing;
    return QSize(minWidth, minHeight);
}

void IconThumbnailGridWidget::recalculateLayout() {
    if (m_previewSizes.isEmpty()) {
        m_preferredWidth = 100;
        m_preferredHeight = 100;
        return;
    }

    // Calculate preferred dimensions
    int maxSize = *std::max_element(m_previewSizes.begin(), m_previewSizes.end());
    int cellWidth = maxSize + 16; // Add padding

    m_columns = 2;
    int rows = (m_previewSizes.size() + m_columns - 1) / m_columns;

    m_preferredWidth = m_columns * cellWidth + (m_columns + 1) * m_spacing;
    m_preferredHeight = rows * (cellWidth + 20) + m_spacing;
}

} // namespace gallery
