/**
 * @file QtLucide.h
 * @brief Main QtLucide API for using Lucide icons in Qt applications
 * @details This file contains the primary QtLucide class which provides a comprehensive
 *          API for loading, customizing, and using Lucide icons in Qt applications.
 *          The API is designed to be compatible with QtAwesome while providing
 *          modern SVG-based icon rendering with extensive customization options.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDE_H
#define QTLUCIDE_H

#include <QHash>
#include <QIcon>
#include <QObject>
#include <QString>
#include <QVariantMap>

#include "QtLucideEnums.h"
#include "QtLucideIconPainter.h"

/**
 * @defgroup QtLucideAPI QtLucide API
 * @brief Main QtLucide API classes and functions
 * @details This group contains the primary classes that form the QtLucide API,
 *          including the main QtLucide class, icon engines, and painters.
 */

namespace lucide {

class QtLucideIconEngine;

/**
 * @brief The main class for managing Lucide icons in Qt applications
 * @details QtLucide provides a comprehensive API for using Lucide icons in Qt applications.
 *          It offers QtAwesome-compatible functionality while leveraging modern SVG rendering
 *          for crisp, scalable icons. The class supports:
 *          - Over 1600 Lucide icons accessible by enum or string name
 *          - Extensive customization options (colors, scaling, effects)
 *          - Custom icon painter registration for advanced use cases
 *          - Efficient caching and rendering through QIconEngine
 *          - Thread-safe icon creation and management
 *
 * @par Basic Usage:
 * @code
 * QtLucide lucide;
 * lucide.initLucide();
 *
 * // Create icons by enum
 * QIcon homeIcon = lucide.icon(Icons::house);
 *
 * // Create icons by name with options
 * QVariantMap options;
 * options["color"] = QColor(Qt::red);
 * QIcon alertIcon = lucide.icon("alert-circle", options);
 * @endcode
 *
 * @par Advanced Usage:
 * @code
 * // Set default options for all icons
 * lucide.setDefaultOption("color", QColor(64, 64, 64));
 * lucide.setDefaultOption("scale-factor", 1.2);
 *
 * // Register custom painters
 * lucide.give("my-custom-icon", new MyCustomPainter());
 * @endcode
 *
 * @since 1.0
 * @see QtLucideIconEngine, QtLucideIconPainter
 * @ingroup QtLucideAPI
 */
class QtLucide : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a QtLucide instance
     * @param parent Parent QObject for memory management
     * @details Creates a new QtLucide instance with default settings. The instance
     *          must be initialized with initLucide() before use.
     * @see initLucide()
     * @since 1.0
     */
    explicit QtLucide(QObject* parent = nullptr);

    /**
     * @brief Destroys the QtLucide instance
     * @details Cleans up all resources including custom painters, cached icons,
     *          and internal data structures. All QIcon instances created by this
     *          QtLucide instance remain valid after destruction.
     * @since 1.0
     */
    ~QtLucide() override;

    /**
     * @brief Initialize the QtLucide icon system
     * @return true if initialization was successful, false otherwise
     * @details Initializes the internal icon mapping system and prepares the
     *          QtLucide instance for icon creation. This method must be called
     *          before using any icon creation methods. Initialization includes:
     *          - Loading icon name to enum mappings
     *          - Setting up the default SVG icon painter
     *          - Preparing internal data structures
     *
     * @note This method is virtual to allow customization in derived classes
     * @note Multiple calls to this method are safe - subsequent calls are ignored
     *
     * @example
     * @code
     * QtLucide lucide;
     * if (!lucide.initLucide()) {
     *     qWarning() << "Failed to initialize QtLucide";
     *     return;
     * }
     * // Now ready to create icons
     * QIcon icon = lucide.icon(Icons::house);
     * @endcode
     *
     * @see icon(), availableIcons()
     * @since 1.0
     */
    virtual bool initLucide();

    /**
     * @brief Set a default option for all subsequently created icons
     * @param name Option name (case-sensitive)
     * @param value Option value (type depends on option)
     * @details Sets a default rendering option that will be applied to all icons
     *          created after this call, unless overridden by icon-specific options.
     *          Common options include:
     *          - "color" (QColor): Primary icon color
     *          - "color-disabled" (QColor): Color for disabled state
     *          - "color-active" (QColor): Color for active state
     *          - "color-selected" (QColor): Color for selected state
     *          - "scale-factor" (double): Scaling factor (1.0 = normal size)
     *          - "opacity" (double): Opacity (0.0 = transparent, 1.0 = opaque)
     *
     * @note Options are stored as QVariant, so type safety is the caller's responsibility
     * @note Setting an option to an invalid QVariant removes it from defaults
     *
     * @example
     * @code
     * lucide.setDefaultOption("color", QColor(Qt::darkBlue));
     * lucide.setDefaultOption("scale-factor", 1.2);
     * // All subsequent icons will be dark blue and 20% larger
     * @endcode
     *
     * @see defaultOption(), resetDefaultOptions(), icon()
     * @since 1.0
     */
    void setDefaultOption(const QString& name, const QVariant& value);

    /**
     * @brief Get the current value of a default option
     * @param name Option name (case-sensitive)
     * @return Current option value, or invalid QVariant if not set
     * @details Retrieves the current value of a default option that was previously
     *          set with setDefaultOption(). Returns an invalid QVariant if the
     *          option has not been set.
     *
     * @example
     * @code
     * QVariant colorOption = lucide.defaultOption("color");
     * if (colorOption.isValid()) {
     *     QColor currentColor = colorOption.value<QColor>();
     *     qDebug() << "Current default color:" << currentColor;
     * }
     * @endcode
     *
     * @see setDefaultOption(), resetDefaultOptions()
     * @since 1.0
     */
    [[nodiscard]] QVariant defaultOption(const QString& name) const;

    /**
     * @brief Create an icon from a Lucide icon identifier
     * @param iconId Icon identifier from the lucide::Icons enumeration
     * @param options Custom rendering options for this specific icon
     * @return QIcon instance ready for use in Qt widgets
     * @details Creates a QIcon from a Lucide icon enum value. This is the most
     *          efficient way to create icons as it avoids string lookups.
     *          The returned QIcon uses QtLucideIconEngine for rendering and
     *          supports all Qt icon states and modes.
     *
     * @par Supported Options:
     * Options are merged with default options, with icon-specific options taking precedence:
     * - "color" (QColor): Primary icon color
     * - "color-disabled", "color-active", "color-selected" (QColor): State-specific colors
     * - "scale-factor" (double): Size multiplier (1.0 = normal)
     * - "opacity" (double): Transparency (0.0-1.0)
     *
     * @note The QIcon remains valid even after the QtLucide instance is destroyed
     * @note Invalid icon IDs will result in an empty/null QIcon
     *
     * @example
     * @code
     * // Simple icon creation
     * QIcon homeIcon = lucide.icon(Icons::house);
     *
     * // Icon with custom color
     * QVariantMap opts;
     * opts["color"] = QColor(Qt::red);
     * opts["scale-factor"] = 1.5;
     * QIcon alertIcon = lucide.icon(Icons::alert_circle, opts);
     * @endcode
     *
     * @see icon(const QString&), Icons, setDefaultOption()
     * @since 1.0
     */
    [[nodiscard]] QIcon icon(Icons iconId, const QVariantMap& options = QVariantMap());

    /**
     * @brief Create an icon from a string name
     * @param name Icon name (e.g., "house", "alert-circle", "chevron-right")
     * @param options Custom rendering options for this specific icon
     * @return QIcon instance, or null QIcon if name is not found
     * @details Creates a QIcon from a Lucide icon name string. Icon names use
     *          kebab-case format matching the official Lucide icon names.
     *          This method performs a string-to-enum lookup internally.
     *
     * @note Icon names are case-sensitive and must match exactly
     * @note Use availableIcons() to get a list of all valid icon names
     * @note For better performance, prefer the enum-based icon() method
     *
     * @example
     * @code
     * // Create icon by name
     * QIcon settingsIcon = lucide.icon("settings");
     *
     * // With custom options
     * QVariantMap opts;
     * opts["color"] = QColor("#3498db");
     * QIcon blueIcon = lucide.icon("user", opts);
     *
     * // Check if icon exists
     * QIcon maybeIcon = lucide.icon("nonexistent-icon");
     * if (maybeIcon.isNull()) {
     *     qWarning() << "Icon not found";
     * }
     * @endcode
     *
     * @see icon(Icons), availableIcons(), stringToIconId()
     * @since 1.0
     */
    [[nodiscard]] QIcon icon(const QString& name, const QVariantMap& options = QVariantMap());

    /**
     * @brief Create an icon using a custom painter
     * @param painter Custom icon painter instance (takes ownership)
     * @param options Custom rendering options for this icon
     * @return QIcon instance using the custom painter
     * @details Creates a QIcon that uses a custom QtLucideIconPainter for rendering.
     *          This allows for completely custom icon rendering logic while still
     *          benefiting from the QtLucide option system and caching.
     *
     * @note QtLucide takes ownership of the painter and will delete it when appropriate
     * @note The painter should be allocated with 'new' and not be used elsewhere
     * @note Custom painters receive all rendering options and can interpret them as needed
     *
     * @example
     * @code
     * // Create icon with custom painter
     * auto customPainter = new MyCustomIconPainter();
     * QIcon customIcon = lucide.icon(customPainter, options);
     * @endcode
     *
     * @see QtLucideIconPainter, give()
     * @since 1.0
     */
    [[nodiscard]] QIcon icon(QtLucideIconPainter* painter,
                             const QVariantMap& options = QVariantMap());

    /**
     * @brief Register a custom icon painter with a name
     * @param name Unique name for the custom icon (case-sensitive)
     * @param painter Custom painter instance (QtLucide takes ownership)
     * @details Registers a custom icon painter that can be used to create icons
     *          with completely custom rendering logic. The painter is associated
     *          with the given name and can be used with the string-based icon()
     *          method or retrieved later.
     *
     * @note QtLucide takes ownership of the painter and will delete it appropriately
     * @note If a painter with the same name already exists, it will be replaced
     * @note The painter should be allocated with 'new' and not used elsewhere
     * @note Custom painter names should not conflict with built-in Lucide icon names
     *
     * @example
     * @code
     * // Register a custom painter
     * lucide.give("my-logo", new MyLogoPainter());
     *
     * // Use the custom painter
     * QIcon logoIcon = lucide.icon("my-logo");
     * @endcode
     *
     * @see icon(const QString&), QtLucideIconPainter
     * @since 1.0
     */
    void give(const QString& name, QtLucideIconPainter* painter);

    /**
     * @brief Get the raw SVG data for a Lucide icon
     * @param iconId Icon identifier from the lucide::Icons enumeration
     * @return SVG data as QByteArray, or empty array if icon not found
     * @details Retrieves the raw SVG data for a Lucide icon without any processing
     *          or customization. This is useful for applications that need direct
     *          access to the SVG content for custom rendering or export.
     *
     * @note The returned SVG data is the original Lucide SVG without color or size modifications
     * @note Invalid icon IDs return an empty QByteArray
     * @note SVG data is embedded in the library and does not require file system access
     *
     * @example
     * @code
     * QByteArray svgData = lucide.svgData(Icons::house);
     * if (!svgData.isEmpty()) {
     *     // Save to file or process the SVG
     *     QFile file("house-icon.svg");
     *     if (file.open(QIODevice::WriteOnly)) {
     *         file.write(svgData);
     *     }
     * }
     * @endcode
     *
     * @see svgData(const QString&), icon()
     * @since 1.0
     */
    [[nodiscard]] QByteArray svgData(Icons iconId) const;

    /**
     * @brief Get the raw SVG data for a Lucide icon by name
     * @param name Icon name (e.g., "house", "alert-circle")
     * @return SVG data as QByteArray, or empty array if icon not found
     * @details Retrieves the raw SVG data for a Lucide icon by string name.
     *          This method performs a name-to-enum lookup internally.
     *
     * @note Icon names are case-sensitive and must match exactly
     * @note Returns empty QByteArray for invalid or custom painter names
     * @note For better performance with known icons, use the enum-based version
     *
     * @example
     * @code
     * QByteArray svgData = lucide.svgData("settings");
     * QString svgString = QString::fromUtf8(svgData);
     * @endcode
     *
     * @see svgData(Icons), availableIcons()
     * @since 1.0
     */
    [[nodiscard]] QByteArray svgData(const QString& name) const;

    /**
     * @brief Get a list of all available Lucide icon names
     * @return QStringList containing all valid icon names
     * @details Returns a complete list of all available Lucide icon names that
     *          can be used with the string-based icon() method. The list includes
     *          only built-in Lucide icons, not custom registered painters.
     *
     * @note The returned list is sorted alphabetically
     * @note Custom painters registered with give() are not included in this list
     * @note The list contains over 1600 icon names
     *
     * @example
     * @code
     * QStringList icons = lucide.availableIcons();
     * qDebug() << "Available icons:" << icons.size();
     *
     * // Check if an icon exists
     * if (icons.contains("my-icon")) {
     *     QIcon icon = lucide.icon("my-icon");
     * }
     * @endcode
     *
     * @see icon(const QString&), ICON_COUNT
     * @since 1.0
     */
    [[nodiscard]] QStringList availableIcons() const;

Q_SIGNALS:
    /**
     * @brief Emitted when default options are reset to system defaults
     * @details This signal is emitted whenever resetDefaultOptions() is called,
     *          allowing connected objects to respond to option changes.
     *          Note that this signal is only emitted for bulk resets, not
     *          individual option changes via setDefaultOption().
     *
     * @see resetDefaultOptions(), setDefaultOption()
     * @since 1.0
     */
    void defaultOptionsReset();

public Q_SLOTS:
    /**
     * @brief Reset all default options to system defaults
     * @details Clears all custom default options that were set with setDefaultOption(),
     *          restoring the QtLucide instance to its initial state. This affects
     *          all subsequently created icons but does not modify existing QIcon
     *          instances.
     *
     * @note Emits the defaultOptionsReset() signal when called
     * @note This operation is thread-safe
     *
     * @example
     * @code
     * // Set some custom defaults
     * lucide.setDefaultOption("color", QColor(Qt::red));
     * lucide.setDefaultOption("scale-factor", 1.5);
     *
     * // Reset to defaults
     * lucide.resetDefaultOptions();
     * // Now all new icons will use system defaults again
     * @endcode
     *
     * @see setDefaultOption(), defaultOptionsReset()
     * @since 1.0
     */
    void resetDefaultOptions();

private:
    /**
     * @brief Initialize the internal icon name mapping system
     * @details Sets up the bidirectional mapping between icon names and enum values.
     *          This method is called automatically during initLucide().
     */
    void initializeIconMap();

    /**
     * @brief Convert an icon name string to its corresponding enum value
     * @param name Icon name in kebab-case format
     * @return Corresponding Icons enum value, or invalid enum if not found
     */
    [[nodiscard]] Icons stringToIconId(const QString& name) const;

    /**
     * @brief Convert an icon enum value to its corresponding name string
     * @param iconId Icon enum value
     * @return Corresponding icon name string, or empty string if invalid
     */
    [[nodiscard]] QString iconIdToString(Icons iconId) const;

    QHash<QString, Icons> m_nameToIconMap; ///< @brief Maps icon names to enum values
    QHash<Icons, QString> m_iconToNameMap; ///< @brief Maps enum values to icon names
    QHash<QString, QtLucideIconPainter*> m_customPainters; ///< @brief Custom registered painters
    QVariantMap m_defaultOptions;                          ///< @brief Default rendering options
    QtLucideIconPainter* m_svgIconPainter{nullptr};        ///< @brief Default SVG icon painter
    bool m_initialized{false}; ///< @brief Whether initLucide() has been called
};

} // namespace lucide

#endif // QTLUCIDE_H
