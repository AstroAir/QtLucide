/**
 * @file ExportUtils.cpp
 * @brief Implementation of ExportUtils
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "ExportUtils.h"

#include <QtLucide/QtLucide.h>

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QPainter>
#include <QRegularExpression>
#include <QSvgRenderer>

namespace gallery {

QByteArray ExportUtils::getSvgData(lucide::QtLucide* lucide, const QString& iconName,
                                   const QColor& color, double strokeWidth) {
    if (!lucide || iconName.isEmpty()) {
        return QByteArray();
    }

    QByteArray svgData = lucide->svgData(iconName);
    if (svgData.isEmpty()) {
        qWarning() << "Failed to get SVG data for icon:" << iconName;
        return QByteArray();
    }

    // Apply color override if valid
    if (color.isValid()) {
        svgData = applyColorToSvg(svgData, color);
    }

    // Apply stroke width override if specified
    if (strokeWidth > 0.0) {
        svgData = applyStrokeWidthToSvg(svgData, strokeWidth);
    }

    return svgData;
}

QPixmap ExportUtils::renderToPixmap(lucide::QtLucide* lucide, const QString& iconName, int size,
                                    const QColor& color) {
    if (!lucide || iconName.isEmpty() || size <= 0) {
        return QPixmap();
    }

    // Get icon with optional color
    QIcon icon;
    if (color.isValid()) {
        QVariantMap options;
        options["color"] = color;
        icon = lucide->icon(iconName, options);
    } else {
        icon = lucide->icon(iconName);
    }

    if (icon.isNull()) {
        qWarning() << "Failed to get icon:" << iconName;
        return QPixmap();
    }

    return icon.pixmap(QSize(size, size));
}

QImage ExportUtils::renderToImage(lucide::QtLucide* lucide, const QString& iconName, int size,
                                  const QColor& color) {
    QPixmap pixmap = renderToPixmap(lucide, iconName, size, color);
    if (pixmap.isNull()) {
        return QImage();
    }
    return pixmap.toImage();
}

bool ExportUtils::saveAsSvg(lucide::QtLucide* lucide, const QString& iconName,
                            const QString& filePath, const QColor& color, double strokeWidth) {
    QByteArray svgData = getSvgData(lucide, iconName, color, strokeWidth);
    if (svgData.isEmpty()) {
        return false;
    }

    // Ensure directory exists
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Failed to create directory:" << dir.absolutePath();
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    qint64 bytesWritten = file.write(svgData);
    file.close();

    return bytesWritten == svgData.size();
}

bool ExportUtils::saveAsPng(lucide::QtLucide* lucide, const QString& iconName,
                            const QString& filePath, int size, const QColor& color) {
    QImage image = renderToImage(lucide, iconName, size, color);
    if (image.isNull()) {
        return false;
    }

    // Ensure directory exists
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Failed to create directory:" << dir.absolutePath();
        return false;
    }

    return image.save(filePath, "PNG");
}

bool ExportUtils::saveAsIco(lucide::QtLucide* lucide, const QString& iconName,
                            const QString& filePath, int size, const QColor& color) {
    QImage image = renderToImage(lucide, iconName, size, color);
    if (image.isNull()) {
        return false;
    }

    // Ensure directory exists
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Failed to create directory:" << dir.absolutePath();
        return false;
    }

    // ICO format may not be supported on all platforms
    // Try to save as ICO, fall back to PNG if not supported
    if (!image.save(filePath, "ICO")) {
        // Try with BMP format which is similar
        QString bmpPath = filePath;
        if (bmpPath.endsWith(".ico", Qt::CaseInsensitive)) {
            bmpPath = bmpPath.left(bmpPath.length() - 4) + ".png";
        }
        qWarning() << "ICO format not supported, saving as PNG:" << bmpPath;
        return image.save(bmpPath, "PNG");
    }

    return true;
}

void ExportUtils::copySvgToClipboard(lucide::QtLucide* lucide, const QString& iconName,
                                     const QColor& color, double strokeWidth) {
    QByteArray svgData = getSvgData(lucide, iconName, color, strokeWidth);
    if (svgData.isEmpty()) {
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();
    if (clipboard) {
        clipboard->setText(QString::fromUtf8(svgData));
    }
}

void ExportUtils::copyPngToClipboard(lucide::QtLucide* lucide, const QString& iconName, int size,
                                     const QColor& color) {
    QPixmap pixmap = renderToPixmap(lucide, iconName, size, color);
    if (pixmap.isNull()) {
        return;
    }

    QClipboard* clipboard = QApplication::clipboard();
    if (clipboard) {
        clipboard->setPixmap(pixmap);
    }
}

QByteArray ExportUtils::applyColorToSvg(const QByteArray& svgData, const QColor& color) {
    QString svgString = QString::fromUtf8(svgData);
    QString colorString = color.name();

    // Replace stroke="currentColor" with the actual color
    svgString.replace("stroke=\"currentColor\"", QString("stroke=\"%1\"").arg(colorString));
    svgString.replace("fill=\"currentColor\"", QString("fill=\"%1\"").arg(colorString));

    // Handle stroke and fill attributes that might use currentColor with different quote styles
    QRegularExpression strokeRegex(R"(stroke\s*=\s*["']currentColor["'])");
    svgString.replace(strokeRegex, QString("stroke=\"%1\"").arg(colorString));

    QRegularExpression fillRegex(R"(fill\s*=\s*["']currentColor["'])");
    svgString.replace(fillRegex, QString("fill=\"%1\"").arg(colorString));

    return svgString.toUtf8();
}

QByteArray ExportUtils::applyStrokeWidthToSvg(const QByteArray& svgData, double strokeWidth) {
    QString svgString = QString::fromUtf8(svgData);

    // Replace stroke-width attribute value
    // Matches patterns like: stroke-width="2" or stroke-width='2' or stroke-width="2.0"
    QRegularExpression strokeWidthRegex(R"(stroke-width\s*=\s*["'](\d+\.?\d*)["'])");
    svgString.replace(strokeWidthRegex, QString("stroke-width=\"%1\"").arg(strokeWidth, 0, 'f', 2));

    return svgString.toUtf8();
}

} // namespace gallery
