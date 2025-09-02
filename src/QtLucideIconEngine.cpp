/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "QtLucide/QtLucideIconEngine.h"
#include "QtLucide/QtLucideIconPainter.h"
#include "QtLucide/QtLucide.h"

#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>

namespace lucide {

QtLucideIconEngine::QtLucideIconEngine(QtLucide* lucide, QtLucideIconPainter* painter, const QVariantMap& options)
    : m_lucide(lucide)
    , m_painter(painter)
    , m_options(options)
{
}

QtLucideIconEngine::~QtLucideIconEngine()
{
    // Note: We don't delete m_lucide or m_painter as they are owned by QtLucide
}

void QtLucideIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)
{
    if (!m_painter || !m_lucide) {
        return;
    }

    // Use the painter to render the icon
    m_painter->paint(m_lucide, painter, rect, mode, state, m_options);
}

QPixmap QtLucideIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    // Check cache first
    QString key = cacheKey(size, mode, state);
    if (m_pixmapCache.contains(key)) {
        return m_pixmapCache[key];
    }

    // Render new pixmap
    QPixmap pixmap = renderPixmap(size, mode, state);
    
    // Cache the result (with reasonable cache size limit)
    if (m_pixmapCache.size() < 100) {  // Limit cache size
        m_pixmapCache[key] = pixmap;
    }
    
    return pixmap;
}

QIconEngine* QtLucideIconEngine::clone() const
{
    return new QtLucideIconEngine(m_lucide, m_painter, m_options);
}

QString QtLucideIconEngine::key() const
{
    return QStringLiteral("QtLucideIconEngine");
}

QString QtLucideIconEngine::cacheKey(const QSize& size, QIcon::Mode mode, QIcon::State state) const
{
    // Create a unique key for caching
    QString iconKey = m_options.value("iconId", "unknown").toString();
    QString colorKey = m_options.value("color", "default").toString();
    
    return QString("%1_%2x%3_%4_%5_%6")
           .arg(iconKey)
           .arg(size.width())
           .arg(size.height())
           .arg(static_cast<int>(mode))
           .arg(static_cast<int>(state))
           .arg(colorKey);
}

QPixmap QtLucideIconEngine::renderPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)
{
    if (!m_painter || !m_lucide) {
        return QPixmap();
    }

    // Create pixmap with device pixel ratio support
    qreal devicePixelRatio = 1.0;
    
    // Try to get device pixel ratio from current screen
    if (QGuiApplication::primaryScreen()) {
        devicePixelRatio = QGuiApplication::primaryScreen()->devicePixelRatio();
    }

    QSize pixmapSize = size * devicePixelRatio;
    QPixmap pixmap(pixmapSize);
    pixmap.setDevicePixelRatio(devicePixelRatio);
    pixmap.fill(Qt::transparent);

    // Create painter and render
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QRect rect(QPoint(0, 0), size);
    m_painter->paint(m_lucide, &painter, rect, mode, state, m_options);

    return pixmap;
}

} // namespace lucide
