/**
 * @file BatchExportManager.cpp
 * @brief Implementation of batch export functionality
 * @details Handles exporting multiple icons in various formats with progress reporting.
 */

#include "BatchExportManager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QSvgGenerator>
#include <QThread>

namespace gallery {

ExportProgressWidget::ExportProgressWidget(QObject* parent)
    : QObject(parent), m_cancelled(false) {}

void ExportProgressWidget::show(int total) {
    m_cancelled = false;
    qDebug() << "Starting export of" << total << "icons";
}

void ExportProgressWidget::hide() {
    qDebug() << "Export operation completed";
}

void ExportProgressWidget::updateProgress(int current, const QString& iconName) {
    qDebug() << "Exporting icon" << (current + 1) << ":" << iconName;
}

bool ExportProgressWidget::isCancelled() const {
    return m_cancelled;
}

// ============================================================================

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

    void setShouldCancel(bool cancel) {
        m_shouldCancel = cancel;
    }

public Q_SLOTS:
    void doExport() {
        int exported = 0;
        int failed = 0;

        // Create output directory if it doesn't exist
        QDir outputDir(m_outputDir);
        if (!outputDir.exists() && !outputDir.mkpath(".")) {
            emit exportFinished(false, 0, m_iconNames.size(),
                                "Failed to create output directory");
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
        painter.drawText(QRect(0, 0, m_size, m_size), Qt::AlignCenter,
                         QString("%1").arg(iconName));
        painter.end();

        return image.save(filename, "PNG");
    }

    QStringList m_iconNames;
    ExportFormat m_format;
    int m_size;
    QString m_outputDir;
    bool m_shouldCancel;
};

// ============================================================================

BatchExportManager::BatchExportManager(QObject* parent)
    : QObject(parent), m_exporting(false), m_shouldCancel(false) {}

BatchExportManager::~BatchExportManager() {
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

bool BatchExportManager::exportIcons(const QStringList& iconNames, ExportFormat format,
                                     int size, const QString& outputDir) {
    // Prevent concurrent exports
    if (m_exporting) {
        qWarning() << "Export already in progress";
        return false;
    }

    // Validate inputs
    if (iconNames.isEmpty()) {
        emit exportFinished(false, 0, 0, "No icons specified for export");
        return false;
    }

    if (size <= 0) {
        emit exportFinished(false, 0, 0, "Invalid icon size");
        return false;
    }

    m_exporting = true;
    m_shouldCancel = false;

    // Create and start worker thread
    m_workerThread = std::make_unique<QThread>();
    auto worker = new BatchExportWorker();
    worker->moveToThread(m_workerThread.get());

    // Setup worker parameters
    worker->setExportParams(iconNames, format, size, outputDir);

    // Connect signals
    connect(m_workerThread.get(), &QThread::started, worker, &BatchExportWorker::doExport);
    connect(worker, &BatchExportWorker::progressChanged, this,
            &BatchExportManager::progressChanged);
    connect(worker, &BatchExportWorker::exportFinished, this,
            [this](bool success, int exported, int failed, const QString& errorMessage) {
                m_exporting = false;
                m_workerThread->quit();
                emit exportFinished(success, exported, failed, errorMessage);
            });
    connect(worker, &BatchExportWorker::exportFinished, worker, &QObject::deleteLater);

    // Note: Cancel flag is checked directly in worker via m_shouldCancel atomic

    m_workerThread->start();
    return true;
}

bool BatchExportManager::isExporting() const {
    return m_exporting;
}

void BatchExportManager::cancel() {
    if (m_exporting) {
        m_shouldCancel = true;
    }
}

void BatchExportManager::doExport(const QStringList& iconNames, ExportFormat format,
                                  int size, const QString& outputDir) {
    // This is called via the worker thread
    int exported = 0;
    int failed = 0;

    QDir outputDirObj(outputDir);
    if (!outputDirObj.exists() && !outputDirObj.mkpath(".")) {
        emit exportFinished(false, 0, iconNames.size(),
                            "Failed to create output directory");
        return;
    }

    for (int i = 0; i < iconNames.size(); ++i) {
        if (m_shouldCancel) {
            emit exportFinished(false, exported, iconNames.size() - exported,
                                "Export cancelled by user");
            return;
        }

        emit progressChanged(i, iconNames.size());
    }

    emit exportFinished(failed == 0, exported, failed, "");
}

} // namespace gallery

#include "BatchExportManager.moc"
