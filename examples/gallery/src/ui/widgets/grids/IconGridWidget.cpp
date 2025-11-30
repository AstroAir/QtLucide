/**
 * @file IconGridWidget.cpp
 * @brief Implementation of IconGridWidget
 * @details Handles virtual scrolling, pixmap caching, and efficient rendering
 *          of large icon collections.
 */

#include "IconGridWidget.h"

#include <QtLucide/QtLucide.h>

#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QToolTip>
#include <algorithm>

namespace gallery {

IconGridWidget::IconGridWidget(QWidget* parent)
    : QWidget(parent), m_lucide(nullptr), m_selectedIcon(), m_hoveredIcon(), m_iconSize(48),
      m_spacing(12), m_columns(1), m_rows(0) {
    setMouseTracking(true);
    setFocusPolicy(Qt::ClickFocus);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setStyleSheet("QWidget { background-color: transparent; }");
}

IconGridWidget::~IconGridWidget() = default;

void IconGridWidget::setIcons(const QStringList& icons) {
    m_icons = icons;
    m_selectedIcon.clear();
    m_hoveredIcon.clear();
    clearPixmapCache();
    recalculateLayout();
    update();
}

void IconGridWidget::setIconSize(int size) {
    // Clamp to reasonable range
    size = std::clamp(size, 16, 256);
    if (m_iconSize != size) {
        m_iconSize = size;
        clearPixmapCache();
        recalculateLayout();
        update();
    }
}

int IconGridWidget::iconSize() const {
    return m_iconSize;
}

void IconGridWidget::setSpacing(int spacing) {
    spacing = std::max(0, spacing);
    if (m_spacing != spacing) {
        m_spacing = spacing;
        recalculateLayout();
        update();
    }
}

int IconGridWidget::spacing() const {
    return m_spacing;
}

int IconGridWidget::columnCount() const {
    return m_columns;
}

bool IconGridWidget::getVisibleRange(int& visibleStart, int& visibleEnd) const {
    if (m_icons.isEmpty()) {
        return false;
    }

    // Calculate how many rows are visible in the widget
    int cellHeight = m_iconSize + m_spacing;
    int visibleRows = (height() + cellHeight - 1) / cellHeight;

    // Calculate starting row with buffer
    int bufferRows = 2;
    int startRow = std::max(0, -bufferRows); // Typically 0, but allow buffer above
    int startIcon = startRow * m_columns;

    // Calculate ending icon
    int endIcon = std::min(static_cast<int>(m_icons.size()) - 1,
                           startIcon + (visibleRows + bufferRows * 2) * m_columns - 1);

    visibleStart = std::max(0, startIcon);
    visibleEnd = std::max(0, endIcon);

    return true;
}

void IconGridWidget::setSelectedIcon(const QString& iconName) {
    if (m_icons.contains(iconName)) {
        m_selectedIcon = iconName;
        emit iconClicked(iconName);
        update();
    }
}

QString IconGridWidget::selectedIcon() const {
    return m_selectedIcon;
}

void IconGridWidget::clearSelection() {
    if (!m_selectedIcon.isEmpty()) {
        m_selectedIcon.clear();
        update();
    }
}

int IconGridWidget::iconCount() const {
    return static_cast<int>(m_icons.size());
}

void IconGridWidget::setLucideInstance(lucide::QtLucide* lucide) {
    m_lucide = lucide;
}

void IconGridWidget::paintEvent(QPaintEvent* event) {
    if (!m_lucide || m_icons.isEmpty()) {
        QWidget::paintEvent(event);
        return;
    }

    int visibleStart = 0;
    int visibleEnd = -1;
    if (!getVisibleRange(visibleStart, visibleEnd)) {
        QWidget::paintEvent(event);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    int cellWidth = m_iconSize + m_spacing;
    int cellHeight = m_iconSize + m_spacing;

    // Draw icons
    const int totalIcons = static_cast<int>(m_icons.size());
    for (int i = visibleStart; i <= visibleEnd && i < totalIcons; ++i) {
        int row = i / m_columns;
        int col = i % m_columns;

        int x = col * cellWidth;
        int y = row * cellHeight;

        QRect iconRect(x, y, m_iconSize, m_iconSize);

        // Skip if outside visible area
        if (!event->rect().intersects(iconRect)) {
            continue;
        }

        const QString& iconName = m_icons.at(i);

        // Draw background for selected icon
        if (iconName == m_selectedIcon) {
            painter.fillRect(iconRect.adjusted(-2, -2, 2, 2),
                             QColor(66, 133, 244, 30)); // Light blue highlight
            painter.drawRect(iconRect.adjusted(-2, -2, 2, 2));
        }

        // Draw background for hovered icon
        if (iconName == m_hoveredIcon) {
            painter.fillRect(iconRect, QColor(200, 200, 200, 20));
        }

        // Draw icon
        QPixmap pixmap = getIconPixmap(iconName);
        if (!pixmap.isNull()) {
            painter.drawPixmap(iconRect, pixmap);
        }
    }

    painter.end();
}

void IconGridWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    recalculateLayout();
}

void IconGridWidget::mousePressEvent(QMouseEvent* event) {
    int index = indexAtPosition(event->pos());
    if (index >= 0 && index < m_icons.size()) {
        setSelectedIcon(m_icons.at(index));
    }
    QWidget::mousePressEvent(event);
}

void IconGridWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    int index = indexAtPosition(event->pos());
    if (index >= 0 && index < m_icons.size()) {
        emit iconDoubleClicked(m_icons.at(index));
    }
    QWidget::mouseDoubleClickEvent(event);
}

void IconGridWidget::mouseMoveEvent(QMouseEvent* event) {
    int index = indexAtPosition(event->pos());
    QString hoveredIcon;

    if (index >= 0 && index < m_icons.size()) {
        hoveredIcon = m_icons.at(index);
        QRect iconRect = iconRectAt(index);
        QToolTip::showText(mapToGlobal(event->pos()), hoveredIcon, this, iconRect);
    } else {
        QToolTip::hideText();
    }

    if (hoveredIcon != m_hoveredIcon) {
        m_hoveredIcon = hoveredIcon;
        emit iconHovered(hoveredIcon);
        update();
    }

    QWidget::mouseMoveEvent(event);
}

void IconGridWidget::leaveEvent(QEvent* event) {
    if (!m_hoveredIcon.isEmpty()) {
        m_hoveredIcon.clear();
        emit iconHovered(QString());
        update();
    }
    QWidget::leaveEvent(event);
}

QSize IconGridWidget::sizeHint() const {
    if (m_rows == 0) {
        return QSize(400, 400);
    }
    return QSize(width(), m_rows * (m_iconSize + m_spacing) + m_spacing);
}

void IconGridWidget::recalculateLayout() {
    if (width() <= 0) {
        m_columns = 1;
        m_rows = 0;
        return;
    }

    int cellWidth = m_iconSize + m_spacing;
    m_columns = std::max(1, (width() + m_spacing) / cellWidth);
    const qsizetype iconCount = m_icons.size();
    if (iconCount == 0) {
        m_rows = 0;
    } else {
        const qsizetype columns = static_cast<qsizetype>(m_columns);
        m_rows = static_cast<int>((iconCount + columns - 1) / columns);
    }

    // Update minimum height
    int totalHeight = m_rows * (m_iconSize + m_spacing) + m_spacing;
    setMinimumHeight(std::max(100, totalHeight));
}

int IconGridWidget::indexAtPosition(const QPoint& pos) const {
    int cellWidth = m_iconSize + m_spacing;
    int cellHeight = m_iconSize + m_spacing;

    int col = pos.x() / cellWidth;
    int row = pos.y() / cellHeight;

    if (col < 0 || col >= m_columns || row < 0) {
        return -1;
    }

    int index = row * m_columns + col;
    if (index >= static_cast<int>(m_icons.size())) {
        return -1;
    }

    // Check if position is within the icon rect, not just the cell
    QRect iconRect = iconRectAt(index);
    if (iconRect.contains(pos)) {
        return index;
    }

    return -1;
}

QRect IconGridWidget::iconRectAt(int index) const {
    if (index < 0 || index >= static_cast<int>(m_icons.size())) {
        return QRect();
    }

    int cellWidth = m_iconSize + m_spacing;
    int cellHeight = m_iconSize + m_spacing;

    int row = index / m_columns;
    int col = index % m_columns;

    return QRect(col * cellWidth, row * cellHeight, m_iconSize, m_iconSize);
}

QPixmap IconGridWidget::getIconPixmap(const QString& iconName) {
    // Check cache first
    auto it = m_pixmapCache.find(iconName);
    if (it != m_pixmapCache.end()) {
        return it.value();
    }

    // Create new pixmap from QtLucide
    if (!m_lucide) {
        return QPixmap();
    }

    QIcon icon = m_lucide->icon(iconName);
    if (icon.isNull()) {
        return QPixmap();
    }

    QPixmap pixmap = icon.pixmap(QSize(m_iconSize, m_iconSize));

    // Cache it
    m_pixmapCache.insert(iconName, pixmap);

    return pixmap;
}

void IconGridWidget::clearPixmapCache() {
    m_pixmapCache.clear();
}

} // namespace gallery
