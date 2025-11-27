/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "QtLucide/QtLucideIconEngine.h"
#include "QtLucide/QtLucide.h"
#include "QtLucide/QtLucideIconPainter.h"

#include <QDebug>
#include <QGuiApplication>
#include <QList>
#include <QPainter>
#include <QPixmap>
#include <QScreen>

namespace lucide {

QtLucideIconEngine::QtLucideIconEngine(QtLucide* lucide, QtLucideIconPainter* painter,
                                       const QVariantMap& options)
    : m_lucide(lucide), m_painter(painter), m_options(options) {}

QtLucideIconEngine::~QtLucideIconEngine() {
    // Note: We don't own m_painter - it's shared with QtLucide or managed externally
    // The painter lifetime is managed by QtLucide::m_svgIconPainter or m_customPainters
}

void QtLucideIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode,
                               QIcon::State state) {
    if (m_painter == nullptr || m_lucide == nullptr) {
        return;
    }

    // Use the painter to render the icon
    m_painter->paint(m_lucide, painter, rect, mode, state, m_options);
}

QPixmap QtLucideIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    // Check cache first
    const QString key = cacheKey(size, mode, state);
    if (m_pixmapCache.contains(key)) {
        return m_pixmapCache[key];
    }

    // Render new pixmap
    QPixmap pixmap = renderPixmap(size, mode, state);

    // Cache the result (with reasonable cache size limit)
    if (m_pixmapCache.size() < 100) { // Limit cache size
        m_pixmapCache[key] = pixmap;
    }

    return pixmap;
}

QIconEngine* QtLucideIconEngine::clone() const {
    // Clone uses the same painter reference since painters are shared
    return new QtLucideIconEngine(m_lucide, m_painter, m_options);
}

QSize QtLucideIconEngine::actualSize(const QSize& size, QIcon::Mode /*mode*/,
                                     QIcon::State /*state*/) {
    // SVG icons are scalable, so we return the requested size
    // Apply scale factor if present
    constexpr double defaultScaleFactor = 0.9;
    double scaleFactor = m_options.value("scale-factor", defaultScaleFactor).toDouble();
    scaleFactor = qBound(0.1, scaleFactor, 10.0);

    return QSize(static_cast<int>(size.width() * scaleFactor),
                 static_cast<int>(size.height() * scaleFactor));
}

QList<QSize> QtLucideIconEngine::availableSizes(QIcon::Mode /*mode*/, QIcon::State /*state*/) {
    // SVG icons are infinitely scalable, return empty list per Qt convention
    // An empty list indicates the icon can be rendered at any size
    return {};
}

QString QtLucideIconEngine::iconName() {
    // Return the icon name from options if available
    int iconId = m_options.value("iconId", -1).toInt();
    if (iconId >= 0 && m_lucide != nullptr) {
        // Get icon name from lucide instance
        QStringList icons = m_lucide->availableIcons();
        // The iconId is the enum value, we need to find the corresponding name
        // For now, return a generic identifier
        return QString("lucide-%1").arg(iconId);
    }
    return QString();
}

bool QtLucideIconEngine::isNull() {
    // Check if we have valid painter and lucide instance
    if (m_painter == nullptr || m_lucide == nullptr) {
        return true;
    }

    // For custom painters, we don't require an iconId - the painter itself is valid
    // Only check iconId for built-in SVG icons (when using QtLucideSvgIconPainter)
    // Custom painters registered via give() are always valid if the painter exists
    if (m_options.contains("customPainter") && m_options.value("customPainter").toBool()) {
        return false; // Custom painters are valid if painter is not null
    }

    // For built-in icons, check if we have a valid icon ID
    int iconId = m_options.value("iconId", -1).toInt();
    return iconId < 0;
}

QString QtLucideIconEngine::key() const {
    return QStringLiteral("QtLucideIconEngine");
}

QString QtLucideIconEngine::cacheKey(const QSize& size, QIcon::Mode mode,
                                     QIcon::State state) const {
    // Create a unique key for caching that includes all relevant options
    const QString iconKey = m_options.value("iconId", "unknown").toString();

    // Include color for the specific mode/state
    QString colorKey;
    if (mode == QIcon::Disabled && m_options.contains("color-disabled")) {
        colorKey = m_options.value("color-disabled").toString();
    } else if (mode == QIcon::Active && m_options.contains("color-active")) {
        colorKey = m_options.value("color-active").toString();
    } else if (mode == QIcon::Selected && m_options.contains("color-selected")) {
        colorKey = m_options.value("color-selected").toString();
    } else {
        colorKey = m_options.value("color", "default").toString();
    }

    // Include scale factor and opacity in cache key
    const QString scaleKey = m_options.value("scale-factor", "0.9").toString();
    const QString opacityKey = m_options.value("opacity", "1.0").toString();

    return QString("%1_%2x%3_%4_%5_%6_%7_%8")
        .arg(iconKey)
        .arg(size.width())
        .arg(size.height())
        .arg(static_cast<int>(mode))
        .arg(static_cast<int>(state))
        .arg(colorKey)
        .arg(scaleKey)
        .arg(opacityKey);
}

QPixmap QtLucideIconEngine::renderPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    if (m_painter == nullptr || m_lucide == nullptr) {
        return {};
    }

    // Create pixmap with device pixel ratio support
    qreal devicePixelRatio = 1.0;

    // Try to get device pixel ratio from current screen
    if (QGuiApplication::primaryScreen() != nullptr) {
        devicePixelRatio = QGuiApplication::primaryScreen()->devicePixelRatio();
    }

    const QSize pixmapSize = size * devicePixelRatio;
    QPixmap pixmap(pixmapSize);
    pixmap.setDevicePixelRatio(devicePixelRatio);
    pixmap.fill(Qt::transparent);

    // Create painter and render
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const QRect rect(QPoint(0, 0), size);
    m_painter->paint(m_lucide, &painter, rect, mode, state, m_options);

    return pixmap;
}

} // namespace lucide
