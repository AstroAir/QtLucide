/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "QtLucide/QtLucideIconPainter.h"
#include "QtLucide/QtLucide.h"

#include <QDebug>
#include <QPainter>
#include <QRegularExpression>
#include <QSvgRenderer>

namespace lucide {

QtLucideIconPainter::~QtLucideIconPainter() = default;

// QtLucideSvgIconPainter implementation

QtLucideSvgIconPainter::QtLucideSvgIconPainter() = default;

QtLucideSvgIconPainter::~QtLucideSvgIconPainter() = default;

QtLucideIconPainter* QtLucideSvgIconPainter::clone() const {
    return new QtLucideSvgIconPainter();
}

QString QtLucideSvgIconPainter::iconText() const {
    return QStringLiteral("svg-icon-painter");
}

void QtLucideSvgIconPainter::paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
                                   QIcon::Mode mode, QIcon::State state,
                                   const QVariantMap& options) {
    if (!lucide || !painter) {
        return;
    }

    // Get the icon ID from options
    bool isValid;
    int iconIdInt = options.value("iconId", -1).toInt(&isValid);
    if (!isValid || iconIdInt < 0) {
        qWarning() << "Invalid icon ID in QtLucideSvgIconPainter::paint";
        return;
    }

    auto iconId = static_cast<Icons>(iconIdInt);

    // Get SVG data
    QByteArray svgData = lucide->svgData(iconId);
    if (svgData.isEmpty()) {
        qWarning() << "No SVG data available for icon ID:" << iconIdInt;
        return;
    }

    // Get color for current mode and state
    QVariant colorVariant = optionValueForModeAndState("color", mode, state, options);
    QColor color = colorVariant.value<QColor>();

    // Fallback color handling for disabled mode if no specific color set
    if (!color.isValid()) {
        color = Qt::black; // Default fallback
    } else if (mode == QIcon::Disabled && !options.contains("color-disabled")) {
        // Auto-generate disabled color if not explicitly set
        color = color.lighter(150);
        color.setAlpha(128);
    }

    // Process SVG to apply color
    svgData = processColorizedSvg(svgData, color);

    // Process SVG to apply stroke width
    double strokeWidth = options.value("stroke-width", 2.0).toDouble();
    strokeWidth = qBound(0.5, strokeWidth, 4.0); // Clamp to valid range
    if (strokeWidth != 2.0) {                    // Only process if different from default
        svgData = processStrokeWidth(svgData, strokeWidth);
    }

    // Create SVG renderer
    QSvgRenderer renderer(svgData);
    if (!renderer.isValid()) {
        qWarning() << "Invalid SVG data for icon ID:" << iconIdInt;
        return;
    }

    // Apply opacity if specified
    qreal opacity = options.value("opacity", 1.0).toReal();
    if (opacity < 1.0 && opacity >= 0.0) {
        painter->setOpacity(painter->opacity() * opacity);
    }

    // Apply scale factor
    constexpr double defaultScaleFactor = 0.9;
    double scaleFactor = options.value("scale-factor", defaultScaleFactor).toDouble();

    // Clamp scale factor to reasonable range
    scaleFactor = qBound(0.1, scaleFactor, 10.0);

    QRect targetRect = rect;
    if (scaleFactor != 1.0) {
        int scaledWidth = static_cast<int>(rect.width() * scaleFactor);
        int scaledHeight = static_cast<int>(rect.height() * scaleFactor);

        targetRect =
            QRect(rect.x() + ((rect.width() - scaledWidth) / 2),
                  rect.y() + ((rect.height() - scaledHeight) / 2), scaledWidth, scaledHeight);
    }

    // Render the SVG
    renderer.render(painter, targetRect);
}

QStringList QtLucideSvgIconPainter::optionKeysForModeAndState(const QString& key, QIcon::Mode mode,
                                                              QIcon::State state) {
    QString modePostfix;
    switch (mode) {
        case QIcon::Disabled:
            modePostfix = "-disabled";
            break;
        case QIcon::Active:
            modePostfix = "-active";
            break;
        case QIcon::Selected:
            modePostfix = "-selected";
            break;
        default:
            break;
    }

    QString statePostfix;
    if (state == QIcon::Off) {
        statePostfix = "-off";
    }

    // Generate keys in priority order: key-mode-state | key-mode | key-state | key
    QStringList result;
    if (!modePostfix.isEmpty()) {
        if (!statePostfix.isEmpty()) {
            result.push_back(key + modePostfix + statePostfix);
        }
        result.push_back(key + modePostfix);
    }
    if (!statePostfix.isEmpty()) {
        result.push_back(key + statePostfix);
    }
    result.push_back(key);

    return result;
}

QVariant QtLucideSvgIconPainter::optionValueForModeAndState(const QString& baseKey,
                                                            QIcon::Mode mode, QIcon::State state,
                                                            const QVariantMap& options) {
    QStringList keys = optionKeysForModeAndState(baseKey, mode, state);

    for (const QString& key : keys) {
        if (options.contains(key)) {
            QVariant value = options.value(key);
            if (value.isValid() && !value.toString().isEmpty()) {
                return value;
            }
        }
    }

    return QVariant();
}

QByteArray QtLucideSvgIconPainter::processColorizedSvg(const QByteArray& svgData,
                                                       const QColor& color) {
    QString svgString = QString::fromUtf8(svgData);

    // Replace stroke="currentColor" with the actual color
    QString colorString = color.name();
    svgString.replace("stroke=\"currentColor\"", QString("stroke=\"%1\"").arg(colorString));
    svgString.replace("fill=\"currentColor\"", QString("fill=\"%1\"").arg(colorString));

    // Handle stroke and fill attributes that might use currentColor
    QRegularExpression strokeRegex(R"(stroke\s*=\s*["']currentColor["'])");
    svgString.replace(strokeRegex, QString("stroke=\"%1\"").arg(colorString));

    QRegularExpression fillRegex(R"(fill\s*=\s*["']currentColor["'])");
    svgString.replace(fillRegex, QString("fill=\"%1\"").arg(colorString));

    return svgString.toUtf8();
}

QByteArray QtLucideSvgIconPainter::processStrokeWidth(const QByteArray& svgData,
                                                      double strokeWidth) {
    QString svgString = QString::fromUtf8(svgData);

    // Replace stroke-width attribute value
    // Matches patterns like: stroke-width="2" or stroke-width='2' or stroke-width="2.0"
    QRegularExpression strokeWidthRegex(R"(stroke-width\s*=\s*["'](\d+\.?\d*)["'])");
    svgString.replace(strokeWidthRegex, QString("stroke-width=\"%1\"").arg(strokeWidth, 0, 'f', 2));

    return svgString.toUtf8();
}

} // namespace lucide
