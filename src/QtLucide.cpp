/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "QtLucide/QtLucide.h"
#include "QtLucide/QtLucideIconEngine.h"
#include "QtLucide/QtLucideStrings.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QPalette>
#include <QResource>
#include <QSet>
#include <QThread>

// Forward declare resource initialization functions
extern int qInitResources_lucide_icons();
extern int qCleanupResources_lucide_icons();

namespace lucide {

// Static mutex for thread-safe resource initialization
static QMutex s_resourceMutex;
static bool s_resourcesInitialized = false;

QtLucide::QtLucide(QObject* parent) : QObject(parent) {
    // Thread-safe resource initialization (only once across all instances)
    {
        QMutexLocker locker(&s_resourceMutex);
        if (!s_resourcesInitialized) {
            const int resourceResult = qInitResources_lucide_icons();
            if (resourceResult == 0) {
                qWarning() << "QtLucide: Failed to initialize icon resources";
            }
            s_resourcesInitialized = true;
        }
    }

    // Initialize default options
    resetDefaultOptions();
}

QtLucide::~QtLucide() {
    delete m_svgIconPainter;
    m_svgIconPainter = nullptr;

    // Clean up custom painters
    qDeleteAll(m_customPainters);
    m_customPainters.clear();

    // Note: We don't cleanup resources here as other QtLucide instances may still use them
    // Resources are cleaned up when the application exits
}

bool QtLucide::initLucide() {
    if (m_initialized) {
        return true;
    }

    // Initialize icon mappings
    initializeIconMap();

    // Create the SVG icon painter
    m_svgIconPainter = new QtLucideSvgIconPainter();

    m_initialized = true;

    qDebug() << "QtLucide initialized with" << ICON_COUNT << "icons";
    return true;
}

void QtLucide::setDefaultOption(const QString& name, const QVariant& value) {
    m_defaultOptions[name] = value;
}

QVariant QtLucide::defaultOption(const QString& name) const {
    return m_defaultOptions.value(name);
}

QIcon QtLucide::icon(Icons iconId, const QVariantMap& options) {
    if (!m_initialized) {
        qWarning() << "QtLucide not initialized. Call initLucide() first.";
        return {};
    }

    // Merge default options with provided options
    QVariantMap mergedOptions = m_defaultOptions;
    for (auto it = options.begin(); it != options.end(); ++it) {
        mergedOptions[it.key()] = it.value();
    }

    // Store the icon ID for the painter
    mergedOptions["iconId"] = static_cast<int>(iconId);

    return QIcon(new QtLucideIconEngine(this, m_svgIconPainter, mergedOptions));
}

QIcon QtLucide::icon(const QString& name, const QVariantMap& options) {
    // Check for custom painters first
    if (m_customPainters.contains(name)) {
        QVariantMap mergedOptions = m_defaultOptions;
        for (auto it = options.begin(); it != options.end(); ++it) {
            mergedOptions[it.key()] = it.value();
        }
        // Mark as custom painter so isNull() works correctly
        mergedOptions["customPainter"] = true;
        mergedOptions["customPainterName"] = name;
        return QIcon(new QtLucideIconEngine(this, m_customPainters[name], mergedOptions));
    }

    // Check if QtLucide is initialized for built-in icons
    if (!m_initialized) {
        qWarning() << "QtLucide not initialized. Call initLucide() first.";
        return {};
    }

    // Convert name to icon ID
    const Icons iconId = stringToIconId(name);
    if (iconId == Icons::a_arrow_down && !m_nameToIconMap.contains(name)) {
        qWarning() << "Unknown icon name:" << name;
        return {};
    }

    return icon(iconId, options);
}

QIcon QtLucide::icon(QtLucideIconPainter* painter, const QVariantMap& options) {
    if (painter == nullptr) {
        qWarning() << "Null painter provided to QtLucide::icon()";
        return {};
    }

    QVariantMap mergedOptions = m_defaultOptions;
    for (auto it = options.begin(); it != options.end(); ++it) {
        mergedOptions[it.key()] = it.value();
    }

    // Mark as custom painter so isNull() works correctly
    mergedOptions["customPainter"] = true;

    return QIcon(new QtLucideIconEngine(this, painter, mergedOptions));
}

void QtLucide::give(const QString& name, QtLucideIconPainter* painter) {
    if (name.isEmpty()) {
        qWarning() << "QtLucide::give() called with empty name";
        delete painter; // Clean up since we're not storing it
        return;
    }

    if (painter == nullptr) {
        qWarning() << "QtLucide::give() called with null painter for name:" << name;
        return;
    }

    // Delete existing painter with same name if present
    if (m_customPainters.contains(name)) {
        delete m_customPainters.take(name);
    }
    m_customPainters.insert(name, painter);
}

QByteArray QtLucide::svgData(Icons iconId) const {
    const QString iconName = iconIdToString(iconId);
    if (iconName.isEmpty()) {
        return {};
    }

    return svgData(iconName);
}

QByteArray QtLucide::svgData(const QString& name) const {
    if (name.isEmpty()) {
        return {};
    }

    const QString resourcePath = QString(":/lucide/%1").arg(name);

    // Try QResource first (more reliable for Qt resources)
    const QResource resource(resourcePath);
    if (resource.isValid()) {
        const uchar* data = resource.data();
        const qint64 size = resource.size();
        if (data != nullptr && size > 0) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            // Qt6: Handle compressed resources
            if (resource.compressionAlgorithm() != QResource::NoCompression) {
                return qUncompress(data, static_cast<int>(size));
            }
#endif
            return QByteArray(reinterpret_cast<const char*>(data), static_cast<int>(size));
        }
    }

    // Fallback to QFile if QResource fails (can happen under stress)
    QFile file(resourcePath);
    if (file.open(QIODevice::ReadOnly)) {
        const QByteArray data = file.readAll();
        file.close();
        if (!data.isEmpty()) {
            return data;
        }
    }

    // Only warn once per path to avoid log spam
    static QSet<QString> warnedPaths;
    static QMutex warnMutex;
    {
        QMutexLocker locker(&warnMutex);
        if (!warnedPaths.contains(resourcePath)) {
            qWarning() << "SVG resource could not be loaded:" << resourcePath;
            warnedPaths.insert(resourcePath);
        }
    }

    return {};
}

QStringList QtLucide::availableIcons() const {
    QStringList icons = m_nameToIconMap.keys();
    icons.sort();
    return icons;
}

void QtLucide::resetDefaultOptions() {
    m_defaultOptions.clear();

    // Set default colors based on application palette
    // Use QGuiApplication if QApplication is not available
    QPalette palette;
    if (qApp != nullptr) {
        palette = qApp->palette();
    }

    m_defaultOptions["color"] = palette.color(QPalette::Normal, QPalette::Text);
    m_defaultOptions["color-disabled"] = palette.color(QPalette::Disabled, QPalette::Text);
    m_defaultOptions["color-active"] = palette.color(QPalette::Active, QPalette::Text);
    m_defaultOptions["color-selected"] = palette.color(QPalette::Active, QPalette::HighlightedText);

    m_defaultOptions["scale-factor"] = 0.9;
    m_defaultOptions["opacity"] = 1.0;

    emit defaultOptionsReset();
}

void QtLucide::initializeIconMap() {
    m_nameToIconMap = STRING_TO_ICON_MAP;
    m_iconToNameMap = ICON_TO_STRING_MAP;
}

Icons QtLucide::stringToIconId(const QString& name) const {
    return m_nameToIconMap.value(name, Icons::a_arrow_down);
}

QString QtLucide::iconIdToString(Icons iconId) const {
    return m_iconToNameMap.value(iconId, QString());
}

} // namespace lucide
