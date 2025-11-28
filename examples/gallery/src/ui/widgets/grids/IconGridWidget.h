/**
 * @file IconGridWidget.h
 * @brief Icon grid widget with virtual scrolling support
 * @details Provides an efficient grid view for displaying large numbers of icons
 *          with virtual scrolling that only renders visible icons plus a buffer.
 *          Icons are displayed as clickable buttons with hover tooltips.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef ICONGRIDWIDGET_H
#define ICONGRIDWIDGET_H

#include <QWidget>
#include <QStringList>
#include <QMap>
#include <QPixmap>
#include <QScrollArea>
#include <memory>

namespace lucide {
class QtLucide;
}

namespace gallery {

/**
 * @class IconGridWidget
 * @brief Efficient grid widget for displaying icons with virtual scrolling
 * @details IconGridWidget displays a large collection of icons in a responsive grid layout
 *          with virtual scrolling support. Only visible icons (plus a buffer) are rendered,
 *          making it suitable for displaying thousands of icons efficiently.
 *
 * @par Features:
 * - Virtual scrolling: only render visible + 2-3 rows buffer
 * - Responsive grid layout that adapts to window width
 * - Pixmap caching for visible icons
 * - Click selection with signal emission
 * - Double-click to open details
 * - Hover tooltips showing icon names
 * - Customizable icon size and spacing
 *
 * @par Usage:
 * @code
 * IconGridWidget gridWidget;
 * gridWidget.setIcons(iconNames);
 * gridWidget.setIconSize(48);
 * gridWidget.setSpacing(12);
 *
 * connect(&gridWidget, &IconGridWidget::iconClicked,
 *         this, &MyWindow::onIconClicked);
 * connect(&gridWidget, &IconGridWidget::iconDoubleClicked,
 *         this, &MyWindow::onIconDoubleClicked);
 * @endcode
 */
class IconGridWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct an IconGridWidget
     * @param parent Parent widget
     */
    explicit IconGridWidget(QWidget* parent = nullptr);

    /**
     * @brief Destroy the widget
     */
    ~IconGridWidget() override;

    /**
     * @brief Set the list of icon names to display
     * @param icons List of icon names in kebab-case format
     * @note The widget will clear any previously displayed icons
     */
    void setIcons(const QStringList& icons);

    /**
     * @brief Set the size of icons in pixels
     * @param size Icon size (default 48)
     * @note Valid range: 16-256 pixels
     * @note Automatically triggers recalculation of grid layout
     */
    void setIconSize(int size);

    /**
     * @brief Get the current icon size
     * @return Icon size in pixels
     */
    [[nodiscard]] int iconSize() const;

    /**
     * @brief Set the spacing between icons in pixels
     * @param spacing Spacing in pixels (default 12)
     * @note Automatically triggers recalculation of grid layout
     */
    void setSpacing(int spacing);

    /**
     * @brief Get the current spacing
     * @return Spacing in pixels
     */
    [[nodiscard]] int spacing() const;

    /**
     * @brief Get the number of columns in the grid
     * @return Number of columns (auto-calculated based on width and icon size)
     */
    [[nodiscard]] int columnCount() const;

    /**
     * @brief Get the range of visible icon indices
     * @param visibleStart Output: index of first visible icon
     * @param visibleEnd Output: index of last visible icon (inclusive)
     * @return true if icons are set, false if empty
     * @note Range includes buffer icons above and below visible area
     */
    [[nodiscard]] bool getVisibleRange(int& visibleStart, int& visibleEnd) const;

    /**
     * @brief Set the currently selected icon
     * @param iconName The icon name to select
     * @note Emits iconClicked() signal
     */
    void setSelectedIcon(const QString& iconName);

    /**
     * @brief Get the currently selected icon
     * @return Name of selected icon, or empty string if none selected
     */
    [[nodiscard]] QString selectedIcon() const;

    /**
     * @brief Clear selection
     */
    void clearSelection();

    /**
     * @brief Get total number of icons
     * @return Number of icons in the grid
     */
    [[nodiscard]] int iconCount() const;

    /**
     * @brief Set the QtLucide instance to use for icon rendering
     * @param lucide QtLucide instance (must remain valid during widget lifetime)
     */
    void setLucideInstance(lucide::QtLucide* lucide);

Q_SIGNALS:
    /**
     * @brief Emitted when an icon is clicked
     * @param iconName The name of the clicked icon
     */
    void iconClicked(const QString& iconName);

    /**
     * @brief Emitted when an icon is double-clicked
     * @param iconName The name of the double-clicked icon
     */
    void iconDoubleClicked(const QString& iconName);

    /**
     * @brief Emitted when the mouse hovers over an icon
     * @param iconName The name of the hovered icon, or empty string if leaving
     */
    void iconHovered(const QString& iconName);

protected:
    /**
     * @brief Handle paint events for efficient rendering
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief Handle resize events to recalculate layout
     */
    void resizeEvent(QResizeEvent* event) override;

    /**
     * @brief Handle mouse press events for selection
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief Handle double-click events
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;

    /**
     * @brief Handle mouse move events for hover tracking
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /**
     * @brief Handle mouse leave events
     */
    void leaveEvent(QEvent* event) override;

    /**
     * @brief Calculate the preferred size
     */
    [[nodiscard]] QSize sizeHint() const override;

private:
    /**
     * @brief Recalculate grid layout based on widget size
     */
    void recalculateLayout();

    /**
     * @brief Get the icon index at the given position
     * @param pos The position in widget coordinates
     * @return Icon index, or -1 if no icon at position
     */
    [[nodiscard]] int indexAtPosition(const QPoint& pos) const;

    /**
     * @brief Get the rectangle for an icon at the given index
     * @param index The icon index
     * @return Rectangle in widget coordinates
     */
    [[nodiscard]] QRect iconRectAt(int index) const;

    /**
     * @brief Get or create a cached pixmap for an icon
     * @param iconName The icon name
     * @return Cached pixmap, or null pixmap if loading failed
     */
    [[nodiscard]] QPixmap getIconPixmap(const QString& iconName);

    /**
     * @brief Clear the pixmap cache
     */
    void clearPixmapCache();

    // Member variables
    lucide::QtLucide* m_lucide{nullptr};
    QStringList m_icons;
    QString m_selectedIcon;
    QString m_hoveredIcon;
    int m_iconSize{48};
    int m_spacing{12};
    int m_columns{1};
    int m_rows{0};
    QMap<QString, QPixmap> m_pixmapCache;
};

} // namespace gallery

#endif // ICONGRIDWIDGET_H
