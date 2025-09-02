/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDEICONPAINTER_H
#define QTLUCIDEICONPAINTER_H

#include <QIcon>
#include <QPainter>
#include <QRect>
#include <QVariantMap>

namespace lucide {

class QtLucide;

/**
 * @brief Abstract base class for custom icon painters
 * 
 * This class allows you to implement custom icon rendering logic.
 * Similar to QtAwesome's QtAwesomeIconPainter but designed for SVG-based icons.
 */
class QtLucideIconPainter
{
public:
    virtual ~QtLucideIconPainter();

    /**
     * @brief Paint the icon
     * @param lucide QtLucide instance
     * @param painter QPainter to draw with
     * @param rect Rectangle to draw in
     * @param mode Icon mode (Normal, Disabled, Active, Selected)
     * @param state Icon state (On, Off)
     * @param options Rendering options
     */
    virtual void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
                      QIcon::Mode mode, QIcon::State state,
                      const QVariantMap& options) = 0;
};

/**
 * @brief SVG-based icon painter
 * 
 * This painter renders icons from SVG data with color and style customization.
 */
class QtLucideSvgIconPainter : public QtLucideIconPainter
{
public:
    QtLucideSvgIconPainter();
    virtual ~QtLucideSvgIconPainter();

    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
              QIcon::Mode mode, QIcon::State state,
              const QVariantMap& options) override;

private:
    QStringList optionKeysForModeAndState(const QString& key, QIcon::Mode mode, QIcon::State state);
    QVariant optionValueForModeAndState(const QString& baseKey, QIcon::Mode mode, QIcon::State state,
                                       const QVariantMap& options);
    QByteArray processColorizedSvg(const QByteArray& svgData, const QColor& color);
};

} // namespace lucide

#endif // QTLUCIDEICONPAINTER_H
