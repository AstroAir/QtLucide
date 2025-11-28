/**
 * @file IconThumbnailGridWidget.h
 * @brief Simplified thumbnail grid widget for preview panels
 * @details Displays 4-6 icons at different sizes without scrolling.
 *          Designed for use in the IconDetailsPanel to show a single icon
 *          at multiple preview sizes.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef ICONTHUMBNAILGRIDWIDGET_H
#define ICONTHUMBNAILGRIDWIDGET_H

#include <QWidget>
#include <QList>

namespace lucide {
class QtLucide;
}

namespace gallery {

/**
 * @class IconThumbnailGridWidget
 * @brief Compact thumbnail grid for displaying icons at multiple sizes
 * @details IconThumbnailGridWidget is a lightweight widget designed to display
 *          a single icon (or small set of icons) at various sizes. It's primarily
 *          used in preview panels to show how an icon looks at different scales.
 *
 * @par Features:
 * - Display 4-6 icons at customizable sizes
 * - Compact layout without scrolling
 * - Efficient rendering without caching
 * - Size labels below each icon
 * - Fixed grid layout (2 columns for proper alignment)
 *
 * @par Usage:
 * @code
 * IconThumbnailGridWidget thumbnailWidget;
 * thumbnailWidget.setPreviewSizes({24, 48, 96, 192});
 * thumbnailWidget.setIconName("star");
 * @endcode
 */
class IconThumbnailGridWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct an IconThumbnailGridWidget
     * @param parent Parent widget
     */
    explicit IconThumbnailGridWidget(QWidget* parent = nullptr);

    /**
     * @brief Destroy the widget
     */
    ~IconThumbnailGridWidget() override;

    /**
     * @brief Set the icon to display
     * @param iconName The icon name in kebab-case format
     * @note Will trigger a repaint with the new icon
     */
    void setIconName(const QString& iconName);

    /**
     * @brief Get the currently displayed icon
     * @return Icon name, or empty string if not set
     */
    [[nodiscard]] QString iconName() const;

    /**
     * @brief Set the preview sizes to display
     * @param sizes List of sizes in pixels (e.g., {24, 48, 96, 192})
     * @note Recommended: 4-6 sizes, preferably powers of 2
     * @note Automatically triggers layout recalculation
     */
    void setPreviewSizes(const QList<int>& sizes);

    /**
     * @brief Get the current preview sizes
     * @return List of preview sizes in pixels
     */
    [[nodiscard]] QList<int> previewSizes() const;

    /**
     * @brief Set the QtLucide instance to use for icon rendering
     * @param lucide QtLucide instance (must remain valid during widget lifetime)
     */
    void setLucideInstance(lucide::QtLucide* lucide);

    /**
     * @brief Set the spacing between preview items
     * @param spacing Spacing in pixels (default 8)
     */
    void setSpacing(int spacing);

    /**
     * @brief Get the current spacing
     * @return Spacing in pixels
     */
    [[nodiscard]] int spacing() const;

protected:
    /**
     * @brief Handle paint events
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief Calculate the preferred size
     */
    [[nodiscard]] QSize sizeHint() const override;

    /**
     * @brief Calculate the minimum size
     */
    [[nodiscard]] QSize minimumSizeHint() const override;

private:
    /**
     * @brief Recalculate layout based on preview sizes
     */
    void recalculateLayout();

    // Member variables
    lucide::QtLucide* m_lucide{nullptr};
    QString m_iconName;
    QList<int> m_previewSizes{{24, 48, 96, 192}};
    int m_spacing{8};
    int m_columns{2};
    int m_preferredWidth{200};
    int m_preferredHeight{200};
};

} // namespace gallery

#endif // ICONTHUMBNAILGRIDWIDGET_H
