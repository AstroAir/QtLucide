/**
 * @file QtLucideIconPainter.h
 * @brief Icon painter classes for custom QtLucide icon rendering
 * @details This file contains the abstract QtLucideIconPainter base class and
 *          concrete implementations for rendering Lucide icons. The painter
 *          architecture allows for flexible icon rendering with support for
 *          custom drawing logic, SVG processing, and state-aware rendering.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDEICONPAINTER_H
#define QTLUCIDEICONPAINTER_H

#include <QIcon>
#include <QPainter>
#include <QRect>
#include <QVariantMap>

/**
 * @defgroup QtLucidePainters Icon Painters
 * @brief Classes for custom icon rendering and painting
 * @details This group contains the painter classes that handle the actual
 *          rendering of icons, including the abstract base class and concrete
 *          implementations for different rendering strategies.
 * @ingroup QtLucideAPI
 */

namespace lucide {

class QtLucide;

/**
 * @brief Abstract base class for custom icon painters
 * @details QtLucideIconPainter provides the interface for custom icon rendering
 *          implementations. It allows developers to create completely custom
 *          icon rendering logic while still integrating with the QtLucide
 *          system and benefiting from its option handling and caching.
 *
 *          The painter receives rendering context including the target rectangle,
 *          icon mode/state, and all rendering options. Implementations can
 *          interpret these parameters as needed for their specific rendering
 *          approach.
 *
 * @par Implementing Custom Painters:
 * @code
 * class MyCustomPainter : public QtLucideIconPainter {
 * public:
 *     void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
 *               QIcon::Mode mode, QIcon::State state,
 *               const QVariantMap& options) override {
 *         // Custom rendering logic here
 *         painter->fillRect(rect, options.value("color", Qt::black).value<QColor>());
 *     }
 * };
 * @endcode
 *
 * @see QtLucideSvgIconPainter, QtLucide::give()
 * @ingroup QtLucidePainters
 * @since 1.0
 */
class QtLucideIconPainter {
public:
    /**
     * @brief Default constructor
     * @since 1.0
     */
    QtLucideIconPainter() = default;

    /**
     * @brief Virtual destructor for proper cleanup
     * @details Ensures proper cleanup of derived painter classes when
     *          deleted through base class pointers.
     * @since 1.0
     */
    virtual ~QtLucideIconPainter();

    /**
     * @brief Create a copy of this painter
     * @return New painter instance (caller takes ownership)
     * @details Creates a deep copy of this painter for use in cloned icon engines.
     *          Derived classes must implement this to return a proper copy.
     * @since 1.0
     */
    [[nodiscard]] virtual QtLucideIconPainter* clone() const = 0;

    /**
     * @brief Get a unique identifier for this painter instance
     * @return String identifier used for caching and debugging
     * @details Returns a unique string that identifies this painter configuration.
     *          Used by the icon engine for cache key generation.
     * @since 1.0
     */
    [[nodiscard]] virtual QString iconText() const = 0;

    /**
     * @brief Paint the icon with the given parameters
     * @param lucide QtLucide instance providing context and resources
     * @param painter QPainter to render with (already configured for the target)
     * @param rect Target rectangle to render the icon within
     * @param mode Icon mode indicating the widget state
     * @param state Icon state (On/Off for checkable widgets)
     * @param options Rendering options including colors, scaling, and custom parameters
     *
     * @details This pure virtual method must be implemented by all concrete painter
     *          classes. It receives all necessary context for rendering an icon and
     *          should draw the icon within the specified rectangle.
     *
     * @par Icon Modes:
     * - QIcon::Normal: Default appearance
     * - QIcon::Disabled: Grayed out or faded appearance
     * - QIcon::Active: Highlighted appearance (mouse hover)
     * - QIcon::Selected: Selected state appearance
     *
     * @par Icon States:
     * - QIcon::Off: Default state
     * - QIcon::On: Activated/checked state (for checkable widgets)
     *
     * @par Common Options:
     * - "color" (QColor): Primary icon color
     * - "color-disabled", "color-active", "color-selected" (QColor): State-specific colors
     * - "scale-factor" (double): Size scaling factor
     * - "opacity" (double): Icon opacity (0.0-1.0)
     *
     * @note The painter is already set up with appropriate transforms and clipping
     * @note Implementations should respect the provided rectangle bounds
     * @note Options may contain custom parameters specific to the painter implementation
     *
     * @example
     * @code
     * void MyPainter::paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
     *                      QIcon::Mode mode, QIcon::State state,
     *                      const QVariantMap& options) {
     *     QColor color = options.value("color", Qt::black).value<QColor>();
     *     if (mode == QIcon::Disabled) {
     *         color = color.lighter(150);
     *     }
     *     painter->fillRect(rect, color);
     * }
     * @endcode
     *
     * @see QtLucide, QtLucideSvgIconPainter
     * @since 1.0
     */
    virtual void paint(QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                       QIcon::State state, const QVariantMap& options) = 0;
};

/**
 * @brief SVG-based icon painter for rendering Lucide icons
 * @details QtLucideSvgIconPainter is the default painter implementation used by
 *          QtLucide for rendering SVG-based Lucide icons. It provides sophisticated
 *          SVG processing including color replacement, scaling, and state-aware
 *          rendering with proper fallback handling.
 *
 *          This painter automatically handles:
 *          - SVG color replacement for customization
 *          - State-specific color selection (normal, disabled, active, selected)
 *          - Proper scaling and positioning within target rectangles
 *          - Opacity and transparency effects
 *          - High-DPI rendering support
 *
 * @par Color Processing:
 * The painter can replace colors in SVG data to match the requested appearance.
 * It supports both simple color replacement and more sophisticated processing
 * for complex SVG structures.
 *
 * @par State Handling:
 * The painter automatically selects appropriate colors based on the icon mode:
 * - Normal: Uses "color" option
 * - Disabled: Uses "color-disabled" or lightened "color"
 * - Active: Uses "color-active" or "color"
 * - Selected: Uses "color-selected" or "color"
 *
 * @see QtLucideIconPainter, QtLucide
 * @ingroup QtLucidePainters
 * @since 1.0
 */
class QtLucideSvgIconPainter : public QtLucideIconPainter {
public:
    /**
     * @brief Constructs an SVG icon painter
     * @details Initializes the SVG painter with default settings for
     *          rendering Lucide SVG icons.
     * @since 1.0
     */
    QtLucideSvgIconPainter();

    /**
     * @brief Destroys the SVG icon painter
     * @details Cleans up any internal resources used for SVG processing.
     * @since 1.0
     */
    ~QtLucideSvgIconPainter() override;

    /**
     * @brief Create a copy of this painter
     * @return New QtLucideSvgIconPainter instance
     * @since 1.0
     */
    [[nodiscard]] QtLucideIconPainter* clone() const override;

    /**
     * @brief Get a unique identifier for this painter
     * @return String "svg-icon-painter"
     * @since 1.0
     */
    [[nodiscard]] QString iconText() const override;

    /**
     * @brief Renders an SVG icon with the specified parameters
     * @param lucide QtLucide instance for accessing SVG data and resources
     * @param painter QPainter to render with
     * @param rect Target rectangle for the icon
     * @param mode Icon mode (Normal, Disabled, Active, Selected)
     * @param state Icon state (On, Off)
     * @param options Rendering options including colors and effects
     *
     * @details Renders the SVG icon by:
     * 1. Retrieving the appropriate SVG data from QtLucide
     * 2. Processing the SVG for color replacement if needed
     * 3. Scaling and positioning the SVG within the target rectangle
     * 4. Applying any additional effects (opacity, transforms)
     *
     * @note This implementation is optimized for Lucide SVG structure
     * @note Supports both simple and complex SVG color schemes
     *
     * @see QtLucideIconPainter::paint()
     * @since 1.0
     */
    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
               QIcon::State state, const QVariantMap& options) override;

private:
    /**
     * @brief Generate option key variants for mode and state
     * @param key Base option key (e.g., "color")
     * @param mode Icon mode
     * @param state Icon state
     * @return List of option keys to try in priority order
     */
    [[nodiscard]] QStringList optionKeysForModeAndState(const QString& key, QIcon::Mode mode, QIcon::State state);

    /**
     * @brief Get the best option value for the given mode and state
     * @param baseKey Base option key
     * @param mode Icon mode
     * @param state Icon state
     * @param options Available options
     * @return Best matching option value, or invalid QVariant if not found
     */
    [[nodiscard]] QVariant optionValueForModeAndState(const QString& baseKey, QIcon::Mode mode,
                                                      QIcon::State state, const QVariantMap& options);

    /**
     * @brief Process SVG data to replace colors
     * @param svgData Original SVG data
     * @param color Target color for replacement
     * @return Processed SVG data with color replacements
     */
    [[nodiscard]] QByteArray processColorizedSvg(const QByteArray& svgData, const QColor& color);
};

} // namespace lucide

#endif // QTLUCIDEICONPAINTER_H
