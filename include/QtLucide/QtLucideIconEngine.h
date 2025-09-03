/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDEICONENGINE_H
#define QTLUCIDEICONENGINE_H

#include <QIconEngine>
#include <QVariantMap>
#include <QPixmap>
#include <QHash>

namespace lucide {

class QtLucide;
class QtLucideIconPainter;

/**
 * @brief Custom QIconEngine for rendering Lucide SVG icons
 *
 * This engine handles the rendering of SVG-based icons with caching
 * and customization support.
 */
class QtLucideIconEngine : public QIconEngine
{
public:
    QtLucideIconEngine(QtLucide* lucide, QtLucideIconPainter* painter, const QVariantMap& options);
    virtual ~QtLucideIconEngine();

    // QIconEngine interface
    void paint(QPainter* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state) override;
    QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) override;
    QIconEngine* clone() const override;
    QString key() const override;

private:
    QString cacheKey(const QSize& size, QIcon::Mode mode, QIcon::State state) const;
    QPixmap renderPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state);

    QtLucide* m_lucide;
    QtLucideIconPainter* m_painter;
    QVariantMap m_options;
    mutable QHash<QString, QPixmap> m_pixmapCache;
};

} // namespace lucide

#endif // QTLUCIDEICONENGINE_H
