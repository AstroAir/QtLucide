/**
 * @file ResponsiveLayoutManager.cpp
 * @brief Implementation of ResponsiveLayoutManager
 * @details Manages responsive grid layout calculations and updates.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "ResponsiveLayoutManager.h"
#include <algorithm>
#include <cmath>

namespace gallery {

ResponsiveLayoutManager::ResponsiveLayoutManager(QObject* parent)
    : QObject(parent), m_viewportWidth(1200), m_viewportHeight(800), m_iconSize(48), m_spacing(12),
      m_minimumIconWidth(24), m_paddingLeft(8), m_paddingTop(8), m_paddingRight(8),
      m_paddingBottom(8), m_cachedColumnCount(0) {
    m_cachedColumnCount = calculateColumnCount();
}

void ResponsiveLayoutManager::setViewportSize(int width, int height) {
    if (m_viewportWidth == width && m_viewportHeight == height) {
        return;
    }

    int previousColumns = m_cachedColumnCount;
    m_viewportWidth = std::max(0, width);
    m_viewportHeight = std::max(0, height);

    emit viewportSizeChanged(m_viewportWidth, m_viewportHeight);

    int newColumns = calculateColumnCount();
    emitLayoutChangedIfNeeded(previousColumns, newColumns);
}

void ResponsiveLayoutManager::setIconSize(int size) {
    if (m_iconSize == size) {
        return;
    }

    int previousColumns = m_cachedColumnCount;
    m_iconSize = std::max(m_minimumIconWidth, size);

    emit iconSizeChanged(m_iconSize);

    int newColumns = calculateColumnCount();
    emitLayoutChangedIfNeeded(previousColumns, newColumns);
}

void ResponsiveLayoutManager::setSpacing(int spacing) {
    if (m_spacing == spacing) {
        return;
    }

    int previousColumns = m_cachedColumnCount;
    m_spacing = std::max(0, spacing);

    int newColumns = calculateColumnCount();
    emitLayoutChangedIfNeeded(previousColumns, newColumns);
}

void ResponsiveLayoutManager::setMinimumIconWidth(int minWidth) {
    if (m_minimumIconWidth == minWidth) {
        return;
    }

    m_minimumIconWidth = std::max(1, minWidth);

    // Ensure icon size is still valid
    if (m_iconSize < m_minimumIconWidth) {
        setIconSize(m_minimumIconWidth);
    }
}

void ResponsiveLayoutManager::setPadding(int left, int top, int right, int bottom) {
    if (m_paddingLeft == left && m_paddingTop == top && m_paddingRight == right &&
        m_paddingBottom == bottom) {
        return;
    }

    int previousColumns = m_cachedColumnCount;
    m_paddingLeft = std::max(0, left);
    m_paddingTop = std::max(0, top);
    m_paddingRight = std::max(0, right);
    m_paddingBottom = std::max(0, bottom);

    int newColumns = calculateColumnCount();
    emitLayoutChangedIfNeeded(previousColumns, newColumns);
}

int ResponsiveLayoutManager::getColumnCount() const {
    return m_cachedColumnCount;
}

int ResponsiveLayoutManager::getAvailableWidth() const {
    return m_viewportWidth - m_paddingLeft - m_paddingRight;
}

int ResponsiveLayoutManager::getRowCount(int totalItems) const {
    if (m_cachedColumnCount <= 0 || totalItems <= 0) {
        return 0;
    }

    return (totalItems + m_cachedColumnCount - 1) / m_cachedColumnCount;
}

bool ResponsiveLayoutManager::needsUpdate(int newWidth, int newHeight) const {
    if (newWidth == m_viewportWidth && newHeight == m_viewportHeight) {
        return false;
    }

    // Temporarily calculate what the column count would be with the new size
    int availableWidth = newWidth - m_paddingLeft - m_paddingRight;
    int itemWithSpacing = m_iconSize + m_spacing;

    if (itemWithSpacing <= 0) {
        return false;
    }

    int newColumnCount = std::max(1, availableWidth / itemWithSpacing);
    return newColumnCount != m_cachedColumnCount;
}

int ResponsiveLayoutManager::getWidthForColumns(int columns) const {
    if (columns <= 0) {
        return 0;
    }

    int itemWithSpacing = m_iconSize + m_spacing;
    int requiredWidth = (columns * itemWithSpacing) - m_spacing;
    return requiredWidth + m_paddingLeft + m_paddingRight;
}

int ResponsiveLayoutManager::calculateColumnCount() const {
    int availableWidth = getAvailableWidth();

    // Ensure we have at least 1 column
    if (availableWidth <= 0 || m_iconSize <= 0) {
        return 1;
    }

    // Calculate item width including spacing
    // Formula: (columns * (iconSize + spacing)) - spacing <= availableWidth
    // Rearranged: columns <= (availableWidth + spacing) / (iconSize + spacing)

    int itemWithSpacing = m_iconSize + m_spacing;
    int columnCount = (availableWidth + m_spacing) / itemWithSpacing;

    // Ensure minimum of 1 column
    return std::max(1, columnCount);
}

void ResponsiveLayoutManager::emitLayoutChangedIfNeeded(int previousColumns, int newColumns) {
    m_cachedColumnCount = newColumns;

    if (previousColumns != newColumns) {
        emit layoutChanged(newColumns, 0);
    }
}

} // namespace gallery
