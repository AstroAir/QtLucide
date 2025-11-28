/**
 * @file GalleryTypes.h
 * @brief Common type definitions and enumerations for the QtLucide gallery
 * @details This file contains all type definitions, structures, and enumerations
 *          used throughout the gallery application for consistent type usage.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef GALLERY_TYPES_H
#define GALLERY_TYPES_H

#include <QString>
#include <QColor>

namespace gallery {

/**
 * @brief Export format enumeration for icon export operations
 * @details Defines the supported formats for exporting icons from the gallery
 */
enum class ExportFormat {
    SVG = 0,    ///< Scalable Vector Graphics format
    PNG = 1,    ///< Portable Network Graphics format
    ICO = 2,    ///< Windows Icon format
    ICNS = 3    ///< macOS Icon format
};

/**
 * @brief View mode enumeration for displaying icons
 * @details Defines different presentation modes for icon display
 */
enum class ViewMode {
    Grid = 0,       ///< Grid view with multiple columns
    List = 1,       ///< List view with icon details
    Thumbnail = 2   ///< Thumbnail/gallery view with large previews
};

/**
 * @brief Sort order enumeration for organizing icons
 * @details Defines different sorting options for icon lists
 */
enum class SortOrder {
    NameAscending = 0,      ///< Alphabetical ascending (A-Z)
    NameDescending = 1,     ///< Alphabetical descending (Z-A)
    DateAdded = 2,          ///< By date icon was added
    Category = 3,           ///< By category name
    Favorites = 4,          ///< Favorites first, then alphabetical
    MostUsed = 5            ///< By usage frequency
};

/**
 * @brief Icon color scheme enumeration
 * @details Defines preset color schemes for rendering icons
 */
enum class ColorScheme {
    Monochrome = 0,     ///< Single color
    Duotone = 1,        ///< Two colors
    Multicolor = 2      ///< Multiple colors
};

/**
 * @brief Icon rendering options structure
 * @details Contains all customization options for rendering a single icon
 * @ingroup Gallery
 */
struct IconOptions {
    /**
     * @brief Primary icon color
     * @details The main color used for rendering the icon
     * @default Black (0, 0, 0)
     */
    QColor color{Qt::black};

    /**
     * @brief Stroke width for outlines
     * @details Width of icon strokes in pixels (where applicable)
     * @default 2.0
     */
    double strokeWidth{2.0};

    /**
     * @brief Icon size in pixels
     * @details The rendered size of the icon
     * @default 48
     */
    int size{48};

    /**
     * @brief Opacity or alpha value
     * @details Transparency level from 0.0 (transparent) to 1.0 (opaque)
     * @default 1.0
     */
    double opacity{1.0};

    /**
     * @brief Scale factor for the icon
     * @details Multiplier for the base size (1.0 = normal, 2.0 = double)
     * @default 1.0
     */
    double scaleFactor{1.0};

    /**
     * @brief Background color
     * @details Color of the icon background (if used)
     * @default Transparent white
     */
    QColor backgroundColor{255, 255, 255, 0};  // Transparent white

    /**
     * @brief Enable drop shadow effect
     * @details Whether to apply drop shadow to the icon
     * @default false
     */
    bool enableShadow{false};

    /**
     * @brief Shadow color
     * @details Color of the drop shadow effect
     * @default Semi-transparent black
     */
    QColor shadowColor{0, 0, 0, 100};

    /**
     * @brief Custom rotation angle in degrees
     * @details Rotation applied to the icon (0-360)
     * @default 0.0
     */
    double rotation{0.0};

    /**
     * @brief Horizontal flip
     * @details Whether to flip the icon horizontally
     * @default false
     */
    bool flipHorizontal{false};

    /**
     * @brief Vertical flip
     * @details Whether to flip the icon vertically
     * @default false
     */
    bool flipVertical{false};

    /**
     * @brief Constructor with default values
     */
    IconOptions() = default;

    /**
     * @brief Equality comparison operator
     * @param other The other IconOptions to compare with
     * @return true if all properties are equal, false otherwise
     */
    bool operator==(const IconOptions& other) const noexcept {
        return color == other.color &&
               strokeWidth == other.strokeWidth &&
               size == other.size &&
               opacity == other.opacity &&
               scaleFactor == other.scaleFactor &&
               backgroundColor == other.backgroundColor &&
               enableShadow == other.enableShadow &&
               shadowColor == other.shadowColor &&
               rotation == other.rotation &&
               flipHorizontal == other.flipHorizontal &&
               flipVertical == other.flipVertical;
    }

    /**
     * @brief Inequality comparison operator
     * @param other The other IconOptions to compare with
     * @return true if any property differs, false otherwise
     */
    bool operator!=(const IconOptions& other) const noexcept {
        return !(*this == other);
    }
};

/**
 * @brief Export options structure for batch export operations
 * @details Contains all options for exporting multiple icons
 */
struct ExportOptions {
    /**
     * @brief The format to export icons to
     * @default SVG
     */
    ExportFormat format{ExportFormat::SVG};

    /**
     * @brief Size(s) to export at
     * @details List of pixel sizes for multi-size exports
     * @default [48, 96, 192] for PNG/ICO, [1] for SVG
     */
    int exportSize{48};

    /**
     * @brief Export directory path
     * @details Where to save exported icons
     * @default Current working directory
     */
    QString exportPath;

    /**
     * @brief File naming pattern
     * @details Pattern for output filenames (use {name} for icon name)
     * @default "{name}.{ext}"
     */
    QString filePattern{"{name}.{ext}"};

    /**
     * @brief Whether to create subdirectories by category
     * @default false
     */
    bool groupByCategory{false};

    /**
     * @brief Whether to overwrite existing files
     * @default false
     */
    bool overwriteExisting{false};

    /**
     * @brief JPEG quality (for applicable formats)
     * @details Quality value from 0-100
     * @default 95
     */
    int jpegQuality{95};

    /**
     * @brief Whether to include transparency
     * @default true
     */
    bool includeTransparency{true};

    /**
     * @brief Equality comparison operator
     * @param other The other ExportOptions to compare with
     * @return true if all properties are equal, false otherwise
     */
    bool operator==(const ExportOptions& other) const noexcept {
        return format == other.format &&
               exportSize == other.exportSize &&
               exportPath == other.exportPath &&
               filePattern == other.filePattern &&
               groupByCategory == other.groupByCategory &&
               overwriteExisting == other.overwriteExisting &&
               jpegQuality == other.jpegQuality &&
               includeTransparency == other.includeTransparency;
    }

    /**
     * @brief Inequality comparison operator
     * @param other The other ExportOptions to compare with
     * @return true if any property differs, false otherwise
     */
    bool operator!=(const ExportOptions& other) const noexcept {
        return !(*this == other);
    }
};

/**
 * @brief Search filter options
 * @details Contains all filtering criteria for icon searches
 */
struct SearchFilter {
    /**
     * @brief Search query string
     * @details Text to search for in icon names and tags
     */
    QString query;

    /**
     * @brief Selected categories to filter by
     * @details Only show icons in these categories
     */
    QStringList categories;

    /**
     * @brief Selected tags to filter by
     * @details Only show icons with these tags
     */
    QStringList tags;

    /**
     * @brief Show only favorite icons
     * @default false
     */
    bool favoritesOnly{false};

    /**
     * @brief Current sort order
     * @default NameAscending
     */
    SortOrder sortBy{SortOrder::NameAscending};

    /**
     * @brief Case sensitive search
     * @default false
     */
    bool caseSensitive{false};

    /**
     * @brief Search in tags (not just names)
     * @default true
     */
    bool searchInTags{true};

    /**
     * @brief Equality comparison operator
     * @param other The other SearchFilter to compare with
     * @return true if all properties are equal, false otherwise
     */
    bool operator==(const SearchFilter& other) const noexcept {
        return query == other.query &&
               categories == other.categories &&
               tags == other.tags &&
               favoritesOnly == other.favoritesOnly &&
               sortBy == other.sortBy &&
               caseSensitive == other.caseSensitive &&
               searchInTags == other.searchInTags;
    }

    /**
     * @brief Inequality comparison operator
     * @param other The other SearchFilter to compare with
     * @return true if any property differs, false otherwise
     */
    bool operator!=(const SearchFilter& other) const noexcept {
        return !(*this == other);
    }
};

} // namespace gallery

#endif // GALLERY_TYPES_H
