/**
 * @file GalleryLogger.h
 * @brief Logging utility for the QtLucide gallery application
 * @details This file provides a simple logging interface using Qt's debug system
 *          with support for different log levels and message categories.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef GALLERY_LOGGER_H
#define GALLERY_LOGGER_H

#include <QMessageLogger>
#include <QString>

namespace gallery {

/**
 * @brief Log level enumeration
 * @details Defines different levels of logging severity
 */
enum class LogLevel {
    Debug = 0,   ///< Debug information for developers
    Info = 1,    ///< General information messages
    Warning = 2, ///< Warning messages about potential issues
    Error = 3,   ///< Error messages indicating failures
    Critical = 4 ///< Critical errors requiring immediate attention
};

/**
 * @brief Logging utility class for the gallery application
 * @details Provides static methods for logging with category support and different
 *          severity levels. Uses Qt's built-in debug system with QLoggingCategory.
 * @ingroup Gallery
 *
 * @par Usage:
 * @code
 * GalleryLogger::debug("search", "Search query received");
 * GalleryLogger::warning("export", "Export directory not found");
 * GalleryLogger::error("core", "Failed to load icons");
 * @endcode
 */
class GalleryLogger {
public:
    /**
     * @brief Log a debug message
     * @param category Message category (e.g., "search", "export", "core")
     * @param message The message to log
     * @details Debug messages are used for detailed diagnostic information
     *          and are typically only shown in debug builds
     *
     * @example
     * @code
     * GalleryLogger::debug("ui", "Window resized to 1024x768");
     * @endcode
     */
    static void debug(const QString& category, const QString& message);

    /**
     * @brief Log an info message
     * @param category Message category (e.g., "search", "export", "core")
     * @param message The message to log
     * @details Info messages provide general information about application progress
     *
     * @example
     * @code
     * GalleryLogger::info("core", "QtLucide initialized successfully");
     * @endcode
     */
    static void info(const QString& category, const QString& message);

    /**
     * @brief Log a warning message
     * @param category Message category (e.g., "search", "export", "core")
     * @param message The message to log
     * @details Warning messages indicate potential issues that should be investigated
     *
     * @example
     * @code
     * GalleryLogger::warning("export", "Export path does not exist");
     * @endcode
     */
    static void warning(const QString& category, const QString& message);

    /**
     * @brief Log an error message
     * @param category Message category (e.g., "search", "export", "core")
     * @param message The message to log
     * @details Error messages indicate failures in operations
     *
     * @example
     * @code
     * GalleryLogger::error("core", "Failed to load icon metadata");
     * @endcode
     */
    static void error(const QString& category, const QString& message);

    /**
     * @brief Log a critical message
     * @param category Message category (e.g., "search", "export", "core")
     * @param message The message to log
     * @details Critical messages indicate severe errors requiring immediate attention
     *
     * @example
     * @code
     * GalleryLogger::critical("core", "Out of memory - application may crash");
     * @endcode
     */
    static void critical(const QString& category, const QString& message);

    /**
     * @brief Log a message with specified level
     * @param level The logging level for this message
     * @param category Message category (e.g., "search", "export", "core")
     * @param message The message to log
     * @details Allows logging with explicit level specification
     *
     * @example
     * @code
     * LogLevel level = LogLevel::Warning;
     * GalleryLogger::log(level, "ui", "Widget not found");
     * @endcode
     */
    static void log(LogLevel level, const QString& category, const QString& message);

    /**
     * @brief Format a message with arguments
     * @param format Format string (like QString::arg())
     * @param args Variable arguments to substitute
     * @return Formatted message string
     * @details Helper method to create formatted messages easily
     *
     * @example
     * @code
     * QString msg = GalleryLogger::format("Loaded %1 icons from %2");
     * msg = msg.arg("1634").arg("/usr/share/icons");
     * GalleryLogger::info("core", msg);
     * @endcode
     */
    static QString formatMessage(const QString& format);

    /**
     * @brief Enable or disable logging
     * @param enabled true to enable logging, false to disable
     * @details When disabled, log messages are ignored
     * @default true
     *
     * @example
     * @code
     * GalleryLogger::setEnabled(false);  // Disable logging in release builds
     * @endcode
     */
    static void setEnabled(bool enabled);

    /**
     * @brief Check if logging is enabled
     * @return true if logging is enabled, false otherwise
     *
     * @example
     * @code
     * if (GalleryLogger::isEnabled()) {
     *     GalleryLogger::debug("core", "Debug info");
     * }
     * @endcode
     */
    static bool isEnabled();

    /**
     * @brief Set the minimum log level to display
     * @param level Messages at or above this level will be shown
     * @default Debug (all messages shown)
     *
     * @example
     * @code
     * // Only show warnings, errors, and critical messages
     * GalleryLogger::setMinimumLevel(LogLevel::Warning);
     * @endcode
     */
    static void setMinimumLevel(LogLevel level);

    /**
     * @brief Get the current minimum log level
     * @return The current minimum logging level
     *
     * @example
     * @code
     * LogLevel current = GalleryLogger::minimumLevel();
     * @endcode
     */
    static LogLevel minimumLevel();

private:
    /**
     * @brief Private constructor to prevent instantiation
     * @details This class is designed to be used as a namespace for static methods only
     */
    GalleryLogger() = delete;

    /**
     * @brief Deleted copy constructor
     */
    GalleryLogger(const GalleryLogger&) = delete;

    /**
     * @brief Deleted assignment operator
     */
    GalleryLogger& operator=(const GalleryLogger&) = delete;

    /**
     * @brief Check if a message should be logged
     * @param level The logging level of the message
     * @return true if the message should be logged, false otherwise
     */
    static bool shouldLog(LogLevel level);

    /**
     * @brief Internal logging implementation
     * @param level The logging level
     * @param category The message category
     * @param message The message to log
     */
    static void logInternal(LogLevel level, const QString& category, const QString& message);

    static bool s_enabled;          ///< Whether logging is enabled
    static LogLevel s_minimumLevel; ///< Minimum log level to display
};

} // namespace gallery

#endif // GALLERY_LOGGER_H
