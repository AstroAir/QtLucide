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

    if (color.isValid()) {
        // Process SVG to apply color
        svgData = processColorizedSvg(svgData, color);
    }

    // Create SVG renderer
    QSvgRenderer renderer(svgData);
    if (!renderer.isValid()) {
        qWarning() << "Invalid SVG data for icon ID:" << iconIdInt;
        return;
    }

    // Apply scale factor
    constexpr double defaultScaleFactor = 0.9;
    double scaleFactor = options.value("scale-factor", defaultScaleFactor).toDouble();
    if (scaleFactor > 0.0 && scaleFactor != 1.0) {
        int scaledWidth = static_cast<int>(rect.width() * scaleFactor);
        int scaledHeight = static_cast<int>(rect.height() * scaleFactor);

        QRect scaledRect(rect.x() + ((rect.width() - scaledWidth) / 2),
                         rect.y() + ((rect.height() - scaledHeight) / 2), scaledWidth, scaledHeight);

        renderer.render(painter, scaledRect);
    } else {
        renderer.render(painter, rect);
    }
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

} // namespace lucide
