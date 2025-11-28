/**
 * @file LayoutConfig.h
 * @brief Layout configuration constants for the QtLucide gallery application
 * @details This file contains all layout-related constants used throughout
 *          the gallery UI, including window sizes, widget dimensions, and spacing values.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef GALLERY_LAYOUTCONFIG_H
#define GALLERY_LAYOUTCONFIG_H

namespace gallery {

/**
 * @brief Layout configuration constants for the gallery application
 * @details Contains all layout-related constants organized by category
 */
class LayoutConfig {
public:
    // Window dimensions
    static constexpr int WINDOW_MIN_WIDTH = 1024;           ///< Minimum window width
    static constexpr int WINDOW_MIN_HEIGHT = 768;           ///< Minimum window height
    static constexpr int WINDOW_DEFAULT_WIDTH = 1400;       ///< Default window width
    static constexpr int WINDOW_DEFAULT_HEIGHT = 900;       ///< Default window height

    // Sidebar configuration
    static constexpr int SIDEBAR_DEFAULT_WIDTH = 280;       ///< Default sidebar width in pixels
    static constexpr int SIDEBAR_MIN_WIDTH = 200;           ///< Minimum sidebar width
    static constexpr int SIDEBAR_MAX_WIDTH = 400;           ///< Maximum sidebar width

    // Icon grid configuration
    static constexpr int ICON_GRID_DEFAULT_SIZE = 48;       ///< Default icon size in pixels
    static constexpr int ICON_GRID_MIN_SIZE = 24;           ///< Minimum icon size (24x24)
    static constexpr int ICON_GRID_MAX_SIZE = 128;          ///< Maximum icon size (128x128)
    static constexpr int ICON_GRID_SPACING = 12;            ///< Spacing between grid items

    // Thumbnail grid configuration
    static constexpr int ICON_THUMBNAIL_SIZE = 64;          ///< Default thumbnail icon size
    static constexpr int ICON_THUMBNAIL_MIN_SIZE = 32;      ///< Minimum thumbnail size
    static constexpr int ICON_THUMBNAIL_MAX_SIZE = 256;     ///< Maximum thumbnail size
    static constexpr int ICON_THUMBNAIL_SPACING = 16;       ///< Spacing between thumbnails

    // Icon slider configuration
    static constexpr int ICON_SIZE_SLIDER_MIN = ICON_GRID_MIN_SIZE;  ///< Icon size slider minimum
    static constexpr int ICON_SIZE_SLIDER_MAX = ICON_GRID_MAX_SIZE;  ///< Icon size slider maximum
    static constexpr int ICON_SIZE_SLIDER_DEFAULT = ICON_GRID_DEFAULT_SIZE; ///< Icon size slider default

    // Thumbnail slider configuration
    static constexpr int THUMBNAIL_SIZE_SLIDER_MIN = ICON_THUMBNAIL_MIN_SIZE;
    static constexpr int THUMBNAIL_SIZE_SLIDER_MAX = ICON_THUMBNAIL_MAX_SIZE;
    static constexpr int THUMBNAIL_SIZE_SLIDER_DEFAULT = ICON_THUMBNAIL_SIZE;

    // Padding and margins
    static constexpr int STANDARD_PADDING = 8;              ///< Standard padding
    static constexpr int STANDARD_MARGIN = 8;               ///< Standard margin
    static constexpr int LARGE_PADDING = 16;                ///< Large padding
    static constexpr int LARGE_MARGIN = 16;                 ///< Large margin
    static constexpr int SMALL_PADDING = 4;                 ///< Small padding
    static constexpr int SMALL_MARGIN = 4;                  ///< Small margin

    // Search and filter panel
    static constexpr int SEARCH_PANEL_HEIGHT = 60;          ///< Search panel height
    static constexpr int SEARCH_INPUT_HEIGHT = 32;          ///< Search input height

    // Details panel configuration
    static constexpr int DETAILS_PANEL_MIN_WIDTH = 280;     ///< Details panel minimum width
    static constexpr int DETAILS_PANEL_MAX_WIDTH = 400;     ///< Details panel maximum width
    static constexpr int DETAILS_PANEL_DEFAULT_WIDTH = 320; ///< Details panel default width

    // Icon preview area
    static constexpr int PREVIEW_AREA_SIZE = 96;            ///< Icon preview area size (96x96)
    static constexpr int PREVIEW_PADDING = 16;              ///< Padding around preview

    // Animation and scrolling
    static constexpr int ANIMATION_DURATION = 200;          ///< Animation duration in milliseconds
    static constexpr int SCROLL_BAR_WIDTH = 12;             ///< Scrollbar width

    // Font sizes (in points)
    static constexpr int FONT_SIZE_SMALL = 9;               ///< Small font size
    static constexpr int FONT_SIZE_NORMAL = 10;             ///< Normal font size
    static constexpr int FONT_SIZE_LARGE = 12;              ///< Large font size
    static constexpr int FONT_SIZE_TITLE = 14;              ///< Title font size
    static constexpr int FONT_SIZE_HEADING = 16;            ///< Heading font size

    // Row heights
    static constexpr int LIST_ITEM_HEIGHT = 32;             ///< List item height
    static constexpr int TABLE_ROW_HEIGHT = 28;             ///< Table row height
    static constexpr int CATEGORY_ITEM_HEIGHT = 36;         ///< Category item height

    // Dialog configuration
    static constexpr int DIALOG_MIN_WIDTH = 400;            ///< Dialog minimum width
    static constexpr int DIALOG_MIN_HEIGHT = 300;           ///< Dialog minimum height
    static constexpr int DIALOG_DEFAULT_WIDTH = 600;        ///< Dialog default width
    static constexpr int DIALOG_DEFAULT_HEIGHT = 400;       ///< Dialog default height

    // Export dialog specific
    static constexpr int EXPORT_DIALOG_MIN_WIDTH = 500;     ///< Export dialog minimum width
    static constexpr int EXPORT_DIALOG_DEFAULT_WIDTH = 700; ///< Export dialog default width

    // Button configuration
    static constexpr int BUTTON_MIN_WIDTH = 80;             ///< Button minimum width
    static constexpr int BUTTON_HEIGHT = 32;                ///< Button height
    static constexpr int BUTTON_ICON_SIZE = 16;             ///< Button icon size

private:
    /**
     * @brief Private constructor to prevent instantiation
     * @details This class is designed to be used as a namespace for constants only
     */
    LayoutConfig() = delete;

    /**
     * @brief Deleted copy constructor
     */
    LayoutConfig(const LayoutConfig&) = delete;

    /**
     * @brief Deleted assignment operator
     */
    LayoutConfig& operator=(const LayoutConfig&) = delete;
};

} // namespace gallery

#endif // GALLERY_LAYOUTCONFIG_H
