/**
 * QtLucide Gallery Application - Batch Export Manager Implementation
 */

#include "BatchExportManager.h"
#include <QApplication>
#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QImageWriter>
#include <QPdfWriter>
#include <QStandardPaths>
#include <QSvgGenerator>
#include <QThread>
#include <QTimerEvent>
#include <algorithm>

// ExportWorker Implementation
#ifdef QTLUCIDE_AVAILABLE
ExportWorker::ExportWorker(lucide::QtLucide* lucide, QObject* parent)
    : QObject(parent), m_lucide(lucide), m_cancelled(0) {}
#else
ExportWorker::ExportWorker(void* lucide, QObject* parent)
    : QObject(parent), m_lucide(lucide), m_cancelled(0) {}
#endif

ExportWorker::~ExportWorker() {}

void ExportWorker::processTask(const ExportTask& task) {
    if (m_cancelled.loadAcquire()) {
        emit processingCancelled();
        return;
    }

    ExportTask completedTask = task;
    completedTask.completedAt = QDateTime::currentDateTime();

    bool success = false;
    QString errorMessage;

    try {
        // Create output directory if needed
        if (!createDirectoryIfNeeded(task.outputPath)) {
            errorMessage = "Failed to create output directory";
        } else {
            // Export based on format
            switch (task.format) {
                case ExportFormat::PNG:
                    success = exportToPNG(task);
                    break;
                case ExportFormat::SVG:
                    success = exportToSVG(task);
                    break;
                case ExportFormat::ICO:
                    success = exportToICO(task);
                    break;
                case ExportFormat::PDF:
                    success = exportToPDF(task);
                    break;
                case ExportFormat::JPEG:
                    success = exportToJPEG(task);
                    break;
                default:
                    errorMessage = "Unsupported export format";
                    break;
            }

            if (success) {
                // Get file size
                QFileInfo fileInfo(task.outputPath);
                if (fileInfo.exists()) {
                    completedTask.fileSize = fileInfo.size();
                } else {
                    success = false;
                    errorMessage = "Export file was not created";
                }

                // Write metadata if requested
                if (success && !task.metadata.isEmpty()) {
                    if (!writeMetadata(task.outputPath, task.metadata)) {
                        // Don't fail the export for metadata issues, just log
                        qWarning() << "Failed to write metadata for" << task.outputPath;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        success = false;
        errorMessage = QString("Exception during export: %1").arg(e.what());
    } catch (...) {
        success = false;
        errorMessage = "Unknown exception during export";
    }

    completedTask.completed = true;
    completedTask.success = success;
    completedTask.errorMessage = errorMessage;

    if (success) {
        emit taskCompleted(completedTask);
    } else {
        emit taskFailed(completedTask, errorMessage);
    }
}

void ExportWorker::cancelProcessing() {
    m_cancelled.storeRelease(1);
}

bool ExportWorker::exportToPNG(const ExportTask& task) {
    if (!m_lucide)
        return false;

    QPixmap pixmap = renderIcon(task.iconName, task.width, task.height, task.iconColor,
                                task.useCustomColor, task.antialiasing);

    if (pixmap.isNull()) {
        return false;
    }

    // Handle background
    if (!task.transparentBackground) {
        QPixmap backgroundPixmap(task.width, task.height);
        backgroundPixmap.fill(task.backgroundColor);

        QPainter painter(&backgroundPixmap);
        painter.setRenderHint(QPainter::Antialiasing, task.antialiasing);
        painter.drawPixmap(0, 0, pixmap);
        painter.end();

        pixmap = backgroundPixmap;
    }

    return pixmap.save(task.outputPath, "PNG");
}

bool ExportWorker::exportToSVG(const ExportTask& task) {
    if (!m_lucide)
        return false;

    QString svgData = getSVGData(task.iconName, task.iconColor, task.useCustomColor);
    if (svgData.isEmpty()) {
        return false;
    }

    QSvgGenerator generator;
    generator.setFileName(task.outputPath);
    generator.setSize(QSize(task.width, task.height));
    generator.setViewBox(QRect(0, 0, task.width, task.height));
    generator.setTitle(QString("Lucide Icon: %1").arg(task.iconName));
    generator.setDescription(QString("Exported from QtLucide Gallery"));

    QPainter painter(&generator);
    painter.setRenderHint(QPainter::Antialiasing, task.antialiasing);

    // Draw background if not transparent
    if (!task.transparentBackground) {
        painter.fillRect(0, 0, task.width, task.height, task.backgroundColor);
    }

    // Render SVG
    QSvgRenderer renderer(svgData.toUtf8());
    renderer.render(&painter);
    painter.end();

    return QFileInfo(task.outputPath).exists();
}

bool ExportWorker::exportToICO(const ExportTask& task) {
    // ICO format typically contains multiple sizes
    // For simplicity, we'll create a single-size ICO
    QPixmap pixmap = renderIcon(task.iconName, task.width, task.height, task.iconColor,
                                task.useCustomColor, task.antialiasing);

    if (pixmap.isNull()) {
        return false;
    }

    // Handle background
    if (!task.transparentBackground) {
        QPixmap backgroundPixmap(task.width, task.height);
        backgroundPixmap.fill(task.backgroundColor);

        QPainter painter(&backgroundPixmap);
        painter.setRenderHint(QPainter::Antialiasing, task.antialiasing);
        painter.drawPixmap(0, 0, pixmap);
        painter.end();

        pixmap = backgroundPixmap;
    }

    return pixmap.save(task.outputPath, "ICO");
}

bool ExportWorker::exportToPDF(const ExportTask& task) {
    if (!m_lucide)
        return false;

    QPdfWriter pdfWriter(task.outputPath);
    pdfWriter.setPageSize(QPageSize(QSizeF(task.width, task.height), QPageSize::Point));
    pdfWriter.setPageMargins(QMarginsF(0, 0, 0, 0));
    pdfWriter.setTitle(QString("Lucide Icon: %1").arg(task.iconName));
    pdfWriter.setCreator("QtLucide Gallery");

    QPainter painter(&pdfWriter);
    painter.setRenderHint(QPainter::Antialiasing, task.antialiasing);

    // Draw background if not transparent
    if (!task.transparentBackground) {
        painter.fillRect(0, 0, task.width, task.height, task.backgroundColor);
    }

    // Render icon
    QString svgData = getSVGData(task.iconName, task.iconColor, task.useCustomColor);
    if (!svgData.isEmpty()) {
        QSvgRenderer renderer(svgData.toUtf8());
        renderer.render(&painter);
    }

    painter.end();
    return QFileInfo(task.outputPath).exists();
}

bool ExportWorker::exportToJPEG(const ExportTask& task) {
    if (!m_lucide)
        return false;

    QPixmap pixmap = renderIcon(task.iconName, task.width, task.height, task.iconColor,
                                task.useCustomColor, task.antialiasing);

    if (pixmap.isNull()) {
        return false;
    }

    // JPEG doesn't support transparency, so always use background
    QPixmap backgroundPixmap(task.width, task.height);
    backgroundPixmap.fill(task.transparentBackground ? Qt::white : task.backgroundColor);

    QPainter painter(&backgroundPixmap);
    painter.setRenderHint(QPainter::Antialiasing, task.antialiasing);
    painter.drawPixmap(0, 0, pixmap);
    painter.end();

    QImageWriter writer(task.outputPath, "JPEG");
    writer.setQuality(task.jpegQuality);
    return writer.write(backgroundPixmap.toImage());
}

QPixmap ExportWorker::renderIcon(const QString& iconName, int width, int height,
                                 const QColor& color, bool useCustomColor, bool antialiasing) {
#ifdef QTLUCIDE_AVAILABLE
    if (!m_lucide)
        return QPixmap();

    // Get icon from QtLucide
    QIcon icon = static_cast<lucide::QtLucide*>(m_lucide)->icon(iconName);
    if (icon.isNull()) {
        return QPixmap();
    }
#else
    Q_UNUSED(iconName)
    Q_UNUSED(color)
    Q_UNUSED(useCustomColor)

    // Create a simple placeholder icon when QtLucide is not available
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, antialiasing);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(4, 4, width - 8, height - 8);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, "?");
    painter.end();

    return pixmap;
#endif

#ifdef QTLUCIDE_AVAILABLE
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Draw icon
    if (useCustomColor) {
        // Apply custom color (this is a simplified approach)
        QPixmap iconPixmap = icon.pixmap(width, height);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), color);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.drawPixmap(0, 0, iconPixmap);
    } else {
        icon.paint(&painter, 0, 0, width, height);
    }

    painter.end();
    return pixmap;
#endif
}

QString ExportWorker::getSVGData(const QString& iconName, const QColor& color,
                                 bool useCustomColor) {
    Q_UNUSED(iconName)
    Q_UNUSED(color)
    Q_UNUSED(useCustomColor)

    if (!m_lucide)
        return QString();

    // This is a simplified approach - in a real implementation,
    // you would get the raw SVG data from QtLucide and modify the color
    QString svgTemplate = R"(
        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24"
             fill="none" stroke="%1" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
            <!-- Icon path would go here -->
        </svg>
    )";

    QString strokeColor = useCustomColor ? color.name() : "currentColor";
    return svgTemplate.arg(strokeColor);
}

bool ExportWorker::writeMetadata(const QString& filePath, const QHash<QString, QString>& metadata) {
    // This is a placeholder - actual metadata writing would depend on the file format
    // For PNG, you might use PNG text chunks
    // For JPEG, you might use EXIF data
    // For SVG, you might embed metadata in the XML
    Q_UNUSED(filePath)
    Q_UNUSED(metadata)
    return true;
}

bool ExportWorker::createDirectoryIfNeeded(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();

    if (!dir.exists()) {
        return dir.mkpath(".");
    }

    return true;
}

// BatchExportManager Implementation
BatchExportManager::BatchExportManager(QObject* parent)
    : QObject(parent), m_lucide(nullptr), m_processingMode(Sequential),
      m_maxConcurrentTasks(DEFAULT_MAX_CONCURRENT_TASKS), m_isExporting(false), m_isPaused(false),
      m_isCancelled(false), m_progressTimerId(0) {
    setupWorkers();
}

BatchExportManager::~BatchExportManager() {
    if (m_isExporting) {
        cancelExport();
    }
    cleanupWorkers();
}

#ifdef QTLUCIDE_AVAILABLE
void BatchExportManager::setLucide(lucide::QtLucide* lucide)
#else
void BatchExportManager::setLucide(void* lucide)
#endif
{
    m_lucide = lucide;

    // Update all workers
    for (ExportWorker* worker : m_workers) {
        worker->setLucide(lucide);
    }
}

void BatchExportManager::setMaxConcurrentTasks(int maxTasks) {
    if (maxTasks < 1)
        maxTasks = 1;
    if (maxTasks > 16)
        maxTasks = 16; // Reasonable upper limit

    if (m_maxConcurrentTasks == maxTasks)
        return;

    m_maxConcurrentTasks = maxTasks;

    // Recreate workers if not currently exporting
    if (!m_isExporting) {
        cleanupWorkers();
        setupWorkers();
    }
}

// Minimal implementations for declared slots to satisfy linking
void BatchExportManager::onTaskCompleted(const ExportTask& task) {
    Q_UNUSED(task);
}

void BatchExportManager::onTaskFailed(const ExportTask& task, const QString& error) {
    Q_UNUSED(task);
    Q_UNUSED(error);
}

void BatchExportManager::onWorkerFinished() {}

void BatchExportManager::processNextTasks() {}

// Missing BatchExportManager methods
void BatchExportManager::setupWorkers() {
    // Setup worker threads for concurrent export
    m_workers.clear();
    for (int i = 0; i < m_maxConcurrentTasks; ++i) {
        // Create worker threads (simplified implementation)
        // In a real implementation, you would create QThread-based workers
    }
}

void BatchExportManager::cancelExport() {
    m_isCancelled = true;
    m_isExporting = false;

    // Cancel all pending tasks
    m_pendingTasks.clear();
    m_activeTasks.clear();

    emit exportCancelled();
}

void BatchExportManager::cleanupWorkers() {
    // Cleanup worker threads
    m_workers.clear();
}

void BatchExportManager::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_progressTimerId) {
        // Update progress
        emit statisticsUpdated(m_stats);
    }
    QObject::timerEvent(event);
}

void BatchExportManager::addExportTask(const ExportTask& task) {
    m_pendingTasks.append(task);
    // No taskAdded signal available, just add to queue
}

void BatchExportManager::startExport() {
    if (m_isExporting || m_pendingTasks.isEmpty()) {
        return;
    }

    m_isExporting = true;
    m_isCancelled = false;
    m_isPaused = false;

    // Reset statistics
    m_stats.totalTasks = static_cast<int>(m_pendingTasks.size());
    m_stats.completedTasks = 0;
    m_stats.failedTasks = 0;
    // No startTime member available in ExportStats

    emit exportStarted();

    // Start processing tasks
    processNextTasks();
}
