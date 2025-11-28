/**
 * @file ResponsiveLayoutManager.h
 * @brief Responsive layout management for dynamic column calculation
 * @details This file contains the ResponsiveLayoutManager class which calculates
 *          optimal grid column counts based on viewport width and icon size.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef RESPONSIVE_LAYOUT_MANAGER_H
#define RESPONSIVE_LAYOUT_MANAGER_H

#include <QObject>
#include <QSize>

namespace gallery {

/**
 * @class ResponsiveLayoutManager
 * @brief Manages responsive grid layout calculations
 * @details This class calculates the optimal number of columns for an icon grid
 *          based on the viewport width, icon size, and spacing constraints.
 *          It emits signals when the layout needs to be updated.
 *
 * @par Features:
 * - Automatic column calculation based on viewport size
 * - Minimum width constraints for icons
 * - Responsive behavior on window resize
 * - Signal-based notification system
 *
 * @par Usage:
 * @code
 * ResponsiveLayoutManager layoutMgr;
 * layoutMgr.setViewportSize(1200, 800);
 * layoutMgr.setIconSize(48);
 * int cols = layoutMgr.getColumnCount();
 *
 * connect(&layoutMgr, &ResponsiveLayoutManager::layoutChanged,
 *         this, &MyWidget::updateLayout);
 * @endcode
 */
class ResponsiveLayoutManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct ResponsiveLayoutManager
     * @param parent The parent QObject
     */
    explicit ResponsiveLayoutManager(QObject *parent = nullptr);

    /**
     * @brief Set the viewport/container size
     * @param width The viewport width in pixels
     * @param height The viewport height in pixels
     */
    void setViewportSize(int width, int height);

    /**
     * @brief Set the icon size
     * @param size The icon size in pixels (both width and height)
     */
    void setIconSize(int size);

    /**
     * @brief Set the spacing between icons
     * @param spacing The spacing in pixels
     */
    void setSpacing(int spacing);

    /**
     * @brief Set the minimum icon width
     * @param minWidth Minimum icon width in pixels (default: 24)
     */
    void setMinimumIconWidth(int minWidth);

    /**
     * @brief Set the padding around the grid
     * @param left Padding on the left
     * @param top Padding on the top
     * @param right Padding on the right
     * @param bottom Padding on the bottom
     */
    void setPadding(int left, int top, int right, int bottom);

    /**
     * @brief Get the optimal column count for the current configuration
     * @return The calculated number of columns
     */
    [[nodiscard]] int getColumnCount() const;

    /**
     * @brief Get the current viewport width
     * @return Width in pixels
     */
    [[nodiscard]] int getViewportWidth() const { return m_viewportWidth; }

    /**
     * @brief Get the current viewport height
     * @return Height in pixels
     */
    [[nodiscard]] int getViewportHeight() const { return m_viewportHeight; }

    /**
     * @brief Get the current icon size
     * @return Icon size in pixels
     */
    [[nodiscard]] int getIconSize() const { return m_iconSize; }

    /**
     * @brief Get the current spacing
     * @return Spacing in pixels
     */
    [[nodiscard]] int getSpacing() const { return m_spacing; }

    /**
     * @brief Get the actual width available for the grid
     * @return Available width after padding
     */
    [[nodiscard]] int getAvailableWidth() const;

    /**
     * @brief Get the row count based on total items and column count
     * @param totalItems Total number of items to display
     * @return Number of rows needed
     */
    [[nodiscard]] int getRowCount(int totalItems) const;

    /**
     * @brief Check if layout needs updating
     * @param newWidth New viewport width
     * @param newHeight New viewport height
     * @return true if column count would change, false otherwise
     */
    [[nodiscard]] bool needsUpdate(int newWidth, int newHeight) const;

    /**
     * @brief Calculate the ideal viewport width for a given column count
     * @param columns Number of columns desired
     * @return Recommended viewport width
     */
    [[nodiscard]] int getWidthForColumns(int columns) const;

signals:
    /**
     * @brief Emitted when the responsive layout changes
     * @param columnCount The new column count
     * @param rowCount The number of rows (if item count is known)
     *
     * This signal indicates that the grid layout needs to be recalculated
     * due to a viewport size change that would affect the column count.
     */
    void layoutChanged(int columnCount, int rowCount = 0);

    /**
     * @brief Emitted when viewport size changes
     * @param width New viewport width
     * @param height New viewport height
     */
    void viewportSizeChanged(int width, int height);

    /**
     * @brief Emitted when icon size changes
     * @param size New icon size
     */
    void iconSizeChanged(int size);

private:
    /**
     * @brief Calculate the column count
     * @return The optimal column count
     */
    [[nodiscard]] int calculateColumnCount() const;

    /**
     * @brief Emit layout changed signal if needed
     * @param previousColumns The previous column count
     * @param newColumns The new column count
     */
    void emitLayoutChangedIfNeeded(int previousColumns, int newColumns);

    // Viewport configuration
    int m_viewportWidth = 1200;                 ///< Current viewport width
    int m_viewportHeight = 800;                 ///< Current viewport height

    // Icon configuration
    int m_iconSize = 48;                        ///< Current icon size
    int m_spacing = 12;                         ///< Spacing between icons
    int m_minimumIconWidth = 24;                ///< Minimum icon width

    // Padding configuration
    int m_paddingLeft = 8;                      ///< Left padding
    int m_paddingTop = 8;                       ///< Top padding
    int m_paddingRight = 8;                     ///< Right padding
    int m_paddingBottom = 8;                    ///< Bottom padding

    // Cached values
    mutable int m_cachedColumnCount = 0;        ///< Cached column count
};

}  // namespace gallery

#endif  // RESPONSIVE_LAYOUT_MANAGER_H
