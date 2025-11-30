/**
 * @file BatchExportWorker.h
 * @brief Worker class for batch export operations
 */

#ifndef BATCHEXPORTWORKER_H
#define BATCHEXPORTWORKER_H

#include <QDir>
#include <QFile>
#include <QImage>
#include <QObject>
#include <QPainter>
#include <QString>
#include <QStringList>
#include <QSvgGenerator>

#include "GalleryTypes.h"

namespace gallery {

class BatchExportWorker : public QObject {
    Q_OBJECT

public:
    explicit BatchExportWorker(QObject* parent = nullptr)
        : QObject(parent), m_shouldCancel(false) {}

    void setExportParams(const QStringList& iconNames, ExportFormat format, int size,
                         const QString& outputDir) {
        m_iconNames = iconNames;
        m_format = format;
        m_size = size;
        m_outputDir = outputDir;
    }

    void setShouldCancel(bool cancel) { m_shouldCancel = cancel; }

public Q_SLOTS:
    void doExport() {
        int exported = 0;
        int failed = 0;

        // Create output directory if it doesn't exist
        QDir outputDir(m_outputDir);
        if (!outputDir.exists() && !outputDir.mkpath(".")) {
            emit exportFinished(false, 0, m_iconNames.size(), "Failed to create output directory");
            return;
        }

        // Export each icon
        for (int i = 0; i < m_iconNames.size(); ++i) {
            if (m_shouldCancel) {
                emit exportFinished(false, exported, m_iconNames.size() - exported,
                                    "Export cancelled by user");
                return;
            }

            const QString& iconName = m_iconNames[i];
            bool success = false;

            switch (m_format) {
                case ExportFormat::SVG:
                    success = exportAsSvg(iconName, outputDir);
                    break;
                case ExportFormat::PNG:
                    success = exportAsPng(iconName, outputDir);
                    break;
            }

            if (success) {
                exported++;
            } else {
                failed++;
            }

            emit progressChanged(i, m_iconNames.size());
        }

        emit exportFinished(failed == 0, exported, failed,
                            failed > 0 ? QString("Failed to export %1 icons").arg(failed) : "");
    }

Q_SIGNALS:
    void progressChanged(int current, int total);
    void exportFinished(bool success, int exported, int failed, const QString& errorMessage);

private:
    bool exportAsSvg(const QString& iconName, const QDir& outputDir) {
        // Create SVG file
        QString filename = outputDir.filePath(iconName + ".svg");
        QSvgGenerator generator;
        generator.setFileName(filename);
        generator.setSize(QSize(m_size, m_size));
        generator.setViewBox(QRect(0, 0, m_size, m_size));
        generator.setTitle(QString("Lucide Icon: %1").arg(iconName));
        generator.setDescription(QString("Exported from QtLucide Gallery"));

        // Paint placeholder SVG content
        // In a real implementation, this would render the actual icon
        QPainter painter(&generator);
        painter.fillRect(0, 0, m_size, m_size, Qt::white);
        painter.drawText(QRect(0, 0, m_size, m_size), Qt::AlignCenter,
                         QString("Icon: %1").arg(iconName));
        painter.end();

        return QFile::exists(filename);
    }

    bool exportAsPng(const QString& iconName, const QDir& outputDir) {
        // Create PNG file
        QString filename = outputDir.filePath(iconName + ".png");
        QImage image(m_size, m_size, QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        // Paint placeholder PNG content
        // In a real implementation, this would render the actual icon
        QPainter painter(&image);
        painter.fillRect(0, 0, m_size, m_size, QColor(200, 200, 200));
        painter.drawText(QRect(0, 0, m_size, m_size), Qt::AlignCenter, QString("%1").arg(iconName));
        painter.end();

        return image.save(filename, "PNG");
    }

    QStringList m_iconNames;
    ExportFormat m_format{ExportFormat::PNG};
    int m_size{48};
    QString m_outputDir;
    bool m_shouldCancel{false};
};

} // namespace gallery

#endif // BATCHEXPORTWORKER_H
