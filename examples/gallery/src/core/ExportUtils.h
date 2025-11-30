/**
 * @file ExportUtils.h
 * @brief Shared export utilities for icon export operations
 * @details Provides common functions for exporting icons in various formats
 *          (SVG, PNG, ICO) with color and stroke customization.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef EXPORT_UTILS_H
#define EXPORT_UTILS_H

#include <QByteArray>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QString>

namespace lucide {
class QtLucide;
}

namespace gallery {

/**
 * @class ExportUtils
 * @brief Utility class for icon export operations
 * @details Provides static methods for:
 *          - Rendering icons to various formats
 *          - Applying color and stroke width overrides
 *          - Saving icons to files
 *          - Copying icons to clipboard
 */
class ExportUtils {
public:
    /**
     * @brief Get SVG data for an icon with optional customization
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param color Optional color override (invalid color = no override)
     * @param strokeWidth Optional stroke width override (0 = no override)
     * @return SVG data as QByteArray
     */
    static QByteArray getSvgData(lucide::QtLucide* lucide, const QString& iconName,
                                 const QColor& color = QColor(), double strokeWidth = 0.0);

    /**
     * @brief Render icon to QPixmap
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param size Size in pixels
     * @param color Optional color override
     * @return Rendered pixmap
     */
    static QPixmap renderToPixmap(lucide::QtLucide* lucide, const QString& iconName, int size,
                                  const QColor& color = QColor());

    /**
     * @brief Render icon to QImage
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param size Size in pixels
     * @param color Optional color override
     * @return Rendered image
     */
    static QImage renderToImage(lucide::QtLucide* lucide, const QString& iconName, int size,
                                const QColor& color = QColor());

    /**
     * @brief Save icon as SVG file
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param filePath Output file path
     * @param color Optional color override
     * @param strokeWidth Optional stroke width override
     * @return true if successful, false otherwise
     */
    static bool saveAsSvg(lucide::QtLucide* lucide, const QString& iconName,
                          const QString& filePath, const QColor& color = QColor(),
                          double strokeWidth = 0.0);

    /**
     * @brief Save icon as PNG file
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param filePath Output file path
     * @param size Size in pixels
     * @param color Optional color override
     * @return true if successful, false otherwise
     */
    static bool saveAsPng(lucide::QtLucide* lucide, const QString& iconName,
                          const QString& filePath, int size, const QColor& color = QColor());

    /**
     * @brief Save icon as ICO file
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param filePath Output file path
     * @param size Size in pixels
     * @param color Optional color override
     * @return true if successful, false otherwise
     */
    static bool saveAsIco(lucide::QtLucide* lucide, const QString& iconName,
                          const QString& filePath, int size, const QColor& color = QColor());

    /**
     * @brief Copy SVG code to clipboard
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param color Optional color override
     * @param strokeWidth Optional stroke width override
     */
    static void copySvgToClipboard(lucide::QtLucide* lucide, const QString& iconName,
                                   const QColor& color = QColor(), double strokeWidth = 0.0);

    /**
     * @brief Copy PNG to clipboard
     * @param lucide QtLucide instance
     * @param iconName Name of the icon
     * @param size Size in pixels
     * @param color Optional color override
     */
    static void copyPngToClipboard(lucide::QtLucide* lucide, const QString& iconName, int size,
                                   const QColor& color = QColor());

private:
    /**
     * @brief Apply color override to SVG data
     * @param svgData Original SVG data
     * @param color Color to apply
     * @return Modified SVG data
     */
    static QByteArray applyColorToSvg(const QByteArray& svgData, const QColor& color);

    /**
     * @brief Apply stroke width override to SVG data
     * @param svgData Original SVG data
     * @param strokeWidth Stroke width to apply
     * @return Modified SVG data
     */
    static QByteArray applyStrokeWidthToSvg(const QByteArray& svgData, double strokeWidth);
};

} // namespace gallery

#endif // EXPORT_UTILS_H
