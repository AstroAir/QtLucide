/**
 * @file QtLucideIconEngine.h
 * @brief Custom QIconEngine implementation for QtLucide icons
 * @details This file contains the QtLucideIconEngine class which implements
 *          Qt's QIconEngine interface to provide efficient, cached rendering
 *          of Lucide icons with full support for Qt's icon system including
 *          multiple sizes, states, and modes.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDEICONENGINE_H
#define QTLUCIDEICONENGINE_H

#include <QHash>
#include <QIconEngine>
#include <QPixmap>
#include <QVariantMap>

/**
 * @defgroup QtLucideCore Core Classes
 * @brief Core implementation classes for QtLucide functionality
 * @details This group contains the internal implementation classes that
 *          provide the core functionality of QtLucide, including icon
 *          engines and rendering infrastructure.
 * @ingroup QtLucideAPI
 */

namespace lucide {

class QtLucide;
class QtLucideIconPainter;

/**
 * @brief Custom QIconEngine implementation for efficient Lucide icon rendering
 * @details QtLucideIconEngine implements Qt's QIconEngine interface to provide
 *          high-performance, cached rendering of Lucide icons. It integrates
 *          seamlessly with Qt's icon system while providing advanced features
 *          like custom painters, extensive caching, and state-aware rendering.
 *
 *          Key features:
 *          - Efficient pixmap caching with automatic cache key generation
 *          - Support for all Qt icon modes and states
 *          - Integration with QtLucideIconPainter system
 *          - Proper handling of high-DPI displays
 *          - Thread-safe rendering operations
 *
 * @par Caching Strategy:
 * The engine maintains an internal cache of rendered pixmaps keyed by size,
 * mode, and state. This ensures that identical icon requests are served
 * from cache, providing excellent performance for applications with many
 * icon instances.
 *
 * @par Integration with Qt:
 * This engine is automatically used when creating QIcon instances through
 * QtLucide methods. It provides full compatibility with Qt's icon system
 * including support for:
 * - QIcon::Mode (Normal, Disabled, Active, Selected)
 * - QIcon::State (On, Off)
 * - Multiple icon sizes with automatic scaling
 * - High-DPI rendering
 *
 * @note This class is typically not used directly by application code
 * @note The engine maintains references to QtLucide and painter instances
 *
 * @see QtLucide::icon(), QtLucideIconPainter, QIconEngine
 * @ingroup QtLucideCore
 * @since 1.0
 */
class QtLucideIconEngine : public QIconEngine {
public:
    /**
     * @brief Constructs a QtLucide icon engine
     * @param lucide QtLucide instance providing icon data and context
     * @param painter Icon painter for rendering (engine takes ownership)
     * @param options Rendering options for this icon
     * @details Creates a new icon engine that uses the specified painter and
     *          options for rendering. The engine maintains references to the
     *          QtLucide instance and takes ownership of the painter.
     *
     * @note The engine does not take ownership of the QtLucide instance
     * @note The painter will be deleted when the engine is destroyed
     * @since 1.0
     */
    QtLucideIconEngine(QtLucide* lucide, QtLucideIconPainter* painter, const QVariantMap& options);

    /**
     * @brief Destroys the icon engine
     * @details Cleans up the painter and clears the pixmap cache. The QtLucide
     *          instance is not affected.
     * @since 1.0
     */
    ~QtLucideIconEngine() override;

    // QIconEngine interface implementation

    /**
     * @brief Paint the icon directly to a QPainter
     * @param painter QPainter to render with
     * @param rect Target rectangle for the icon
     * @param mode Icon mode (Normal, Disabled, Active, Selected)
     * @param state Icon state (On, Off)
     * @details Renders the icon directly using the associated QtLucideIconPainter.
     *          This method bypasses pixmap caching and renders directly to the
     *          provided painter, which can be more efficient for one-time rendering.
     *
     * @note This is part of the QIconEngine interface
     * @see pixmap(), QtLucideIconPainter::paint()
     * @since 1.0
     */
    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;

    /**
     * @brief Generate a pixmap for the icon
     * @param size Desired pixmap size
     * @param mode Icon mode (Normal, Disabled, Active, Selected)
     * @param state Icon state (On, Off)
     * @return Rendered pixmap, potentially from cache
     * @details Creates or retrieves a cached pixmap for the icon with the specified
     *          parameters. This method implements intelligent caching to avoid
     *          redundant rendering operations.
     *
     * @note Pixmaps are cached based on size, mode, and state
     * @note Cache keys include option fingerprints for correctness
     * @see paint(), renderPixmap()
     * @since 1.0
     */
    [[nodiscard]] QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;

    /**
     * @brief Create a copy of this icon engine
     * @return New QtLucideIconEngine instance with identical configuration
     * @details Creates a deep copy of this icon engine including the painter
     *          and options. The new engine shares the same QtLucide instance
     *          but has its own pixmap cache.
     *
     * @note This is required by the QIconEngine interface
     * @note The cloned engine starts with an empty pixmap cache
     * @since 1.0
     */
    [[nodiscard]] QIconEngine* clone() const override;

    /**
     * @brief Get a unique key identifying this engine type
     * @return String key for this engine type
     * @details Returns a unique string that identifies QtLucideIconEngine
     *          instances. This is used by Qt's icon system for engine
     *          identification and serialization.
     *
     * @note This is part of the QIconEngine interface
     * @since 1.0
     */
    [[nodiscard]] QString key() const override;

    /**
     * @brief Returns the actual size of the icon for the given parameters
     * @param size Requested size
     * @param mode Icon mode
     * @param state Icon state
     * @return The actual size the icon will be rendered at
     * @details For vector-based icons like SVG, this typically returns the
     *          requested size since SVGs scale perfectly.
     * @since 1.0
     */
    [[nodiscard]] QSize actualSize(const QSize& size, QIcon::Mode mode,
                                   QIcon::State state) override;

    /**
     * @brief Returns a list of available icon sizes
     * @return List of sizes; empty for scalable icons
     * @details Since Lucide icons are SVG-based and infinitely scalable,
     *          this returns an empty list indicating any size is supported.
     * @since 1.0
     */
    [[nodiscard]] QList<QSize> availableSizes(QIcon::Mode mode = QIcon::Normal,
                                              QIcon::State state = QIcon::Off) override;

    /**
     * @brief Returns the icon name if available
     * @return Icon name string or empty string
     * @since 1.0
     */
    [[nodiscard]] QString iconName() const override;

    /**
     * @brief Check if this icon engine is null (has no valid icon)
     * @return true if the engine has no valid icon data
     * @since 1.0
     */
    [[nodiscard]] bool isNull() const override;

private:
    /**
     * @brief Generate a cache key for the given parameters
     * @param size Icon size
     * @param mode Icon mode
     * @param state Icon state
     * @return Unique cache key string
     */
    [[nodiscard]] QString cacheKey(const QSize& size, QIcon::Mode mode, QIcon::State state) const;

    /**
     * @brief Render a new pixmap with the given parameters
     * @param size Target pixmap size
     * @param mode Icon mode
     * @param state Icon state
     * @return Newly rendered pixmap
     */
    [[nodiscard]] QPixmap renderPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state);

    QtLucide* m_lucide;                            ///< @brief QtLucide instance (not owned)
    QtLucideIconPainter* m_painter;                ///< @brief Icon painter (owned)
    QVariantMap m_options;                         ///< @brief Rendering options
    mutable QHash<QString, QPixmap> m_pixmapCache; ///< @brief Pixmap cache for performance
};

} // namespace lucide

#endif // QTLUCIDEICONENGINE_H
