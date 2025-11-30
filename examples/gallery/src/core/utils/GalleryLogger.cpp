/**
 * @file GalleryLogger.cpp
 * @brief Implementation of the GalleryLogger utility class
 * @details Provides logging functionality for the gallery application
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "GalleryLogger.h"

#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>

namespace gallery {

// Initialize static members
bool GalleryLogger::s_enabled = true;
LogLevel GalleryLogger::s_minimumLevel = LogLevel::Debug;

// Create logging categories for different subsystems
Q_LOGGING_CATEGORY(lcGalleryCore, "gallery.core")
Q_LOGGING_CATEGORY(lcGalleryUI, "gallery.ui")
Q_LOGGING_CATEGORY(lcGallerySearch, "gallery.search")
Q_LOGGING_CATEGORY(lcGalleryExport, "gallery.export")
Q_LOGGING_CATEGORY(lcGalleryTheme, "gallery.theme")
Q_LOGGING_CATEGORY(lcGalleryManager, "gallery.manager")

namespace {

const QLoggingCategory& getCategoryObject(const QString& category) {
    const QString normalized = category.trimmed().toLower();

    if (normalized == QLatin1String("ui")) {
        return lcGalleryUI();
    }
    if (normalized == QLatin1String("search")) {
        return lcGallerySearch();
    }
    if (normalized == QLatin1String("export")) {
        return lcGalleryExport();
    }
    if (normalized == QLatin1String("theme")) {
        return lcGalleryTheme();
    }
    if (normalized == QLatin1String("manager")) {
        return lcGalleryManager();
    }

    return lcGalleryCore();
}

} // namespace

void GalleryLogger::debug(const QString& category, const QString& message) {
    if (!shouldLog(LogLevel::Debug)) {
        return;
    }
    logInternal(LogLevel::Debug, category, message);
}

void GalleryLogger::info(const QString& category, const QString& message) {
    if (!shouldLog(LogLevel::Info)) {
        return;
    }
    logInternal(LogLevel::Info, category, message);
}

void GalleryLogger::warning(const QString& category, const QString& message) {
    if (!shouldLog(LogLevel::Warning)) {
        return;
    }
    logInternal(LogLevel::Warning, category, message);
}

void GalleryLogger::error(const QString& category, const QString& message) {
    if (!shouldLog(LogLevel::Error)) {
        return;
    }
    logInternal(LogLevel::Error, category, message);
}

void GalleryLogger::critical(const QString& category, const QString& message) {
    if (!shouldLog(LogLevel::Critical)) {
        return;
    }
    logInternal(LogLevel::Critical, category, message);
}

void GalleryLogger::log(LogLevel level, const QString& category, const QString& message) {
    if (!shouldLog(level)) {
        return;
    }
    logInternal(level, category, message);
}

QString GalleryLogger::formatMessage(const QString& format) {
    return format;
}

void GalleryLogger::setEnabled(bool enabled) {
    s_enabled = enabled;
}

bool GalleryLogger::isEnabled() {
    return s_enabled;
}

void GalleryLogger::setMinimumLevel(LogLevel level) {
    s_minimumLevel = level;
}

LogLevel GalleryLogger::minimumLevel() {
    return s_minimumLevel;
}

bool GalleryLogger::shouldLog(LogLevel level) {
    if (!s_enabled) {
        return false;
    }
    return static_cast<int>(level) >= static_cast<int>(s_minimumLevel);
}

void GalleryLogger::logInternal(LogLevel level, const QString& category, const QString& message) {
    // Format: [TIMESTAMP] [LEVEL] [CATEGORY] Message
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr;
    const QLoggingCategory& categoryObject = getCategoryObject(category);

    switch (level) {
        case LogLevel::Debug:
            levelStr = "DEBUG";
            qDebug(categoryObject)
                << QString("[%1] [%2] [%3] %4").arg(timestamp, levelStr, category, message);
            break;

        case LogLevel::Info:
            levelStr = "INFO";
            qInfo(categoryObject)
                << QString("[%1] [%2] [%3] %4").arg(timestamp, levelStr, category, message);
            break;

        case LogLevel::Warning:
            levelStr = "WARNING";
            qWarning(categoryObject)
                << QString("[%1] [%2] [%3] %4").arg(timestamp, levelStr, category, message);
            break;

        case LogLevel::Error:
            levelStr = "ERROR";
            qCritical(categoryObject)
                << QString("[%1] [%2] [%3] %4").arg(timestamp, levelStr, category, message);
            break;

        case LogLevel::Critical:
            levelStr = "CRITICAL";
            qCritical(categoryObject)
                << QString("[%1] [%2] [%3] %4").arg(timestamp, levelStr, category, message);
            break;

        default:
            qDebug(categoryObject) << message;
            break;
    }
}

} // namespace gallery
