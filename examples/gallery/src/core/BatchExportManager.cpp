/**
 * @file BatchExportManager.cpp
 * @brief Implementation of batch export functionality
 * @details Handles exporting multiple icons in various formats with progress reporting.
 */

#include "BatchExportManager.h"
#include "ExportUtils.h"

#include <QtLucide/QtLucide.h>

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTimer>

namespace gallery {

ExportProgressWidget::ExportProgressWidget(QObject* parent) : QObject(parent), m_cancelled(false) {}

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

BatchExportManager::BatchExportManager(QObject* parent)
    : QObject(parent), m_lucide(nullptr), m_exporting(false), m_shouldCancel(false),
      m_exportFormat(ExportFormat::PNG), m_exportSize(48), m_exportedCount(0), m_failedCount(0) {}

BatchExportManager::~BatchExportManager() {
    if (m_workerThread && m_workerThread->isRunning()) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void BatchExportManager::setLucideInstance(lucide::QtLucide* lucide) {
    m_lucide = lucide;
}

bool BatchExportManager::exportIcons(const QStringList& iconNames, ExportFormat format, int size,
                                     const QString& outputDir) {
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

    // Create output directory if it doesn't exist
    QDir dir(outputDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        emit exportFinished(false, 0, static_cast<int>(iconNames.size()),
                            "Failed to create output directory");
        return false;
    }

    m_exporting = true;
    m_shouldCancel = false;
    m_pendingIcons = iconNames;
    m_exportFormat = format;
    m_exportSize = size;
    m_outputDir = outputDir;
    m_exportedCount = 0;
    m_failedCount = 0;

    // Start export using timer for non-blocking operation
    QTimer::singleShot(0, this, &BatchExportManager::doExportStep);
    return true;
}

void BatchExportManager::doExportStep() {
    if (m_shouldCancel) {
        m_exporting = false;
        emit exportFinished(false, m_exportedCount, static_cast<int>(m_pendingIcons.size()),
                            "Export cancelled by user");
        m_pendingIcons.clear();
        return;
    }

    if (m_pendingIcons.isEmpty()) {
        m_exporting = false;
        QString errorMsg =
            m_failedCount > 0 ? QString("Failed to export %1 icons").arg(m_failedCount) : QString();
        emit exportFinished(m_failedCount == 0, m_exportedCount, m_failedCount, errorMsg);
        return;
    }

    // Export one icon
    QString iconName = m_pendingIcons.takeFirst();
    int totalIcons = m_exportedCount + m_failedCount + static_cast<int>(m_pendingIcons.size()) + 1;
    int currentIndex = m_exportedCount + m_failedCount;

    bool success = false;
    QString filePath;

    switch (m_exportFormat) {
        case ExportFormat::SVG:
            filePath = m_outputDir + "/" + iconName + ".svg";
            success = ExportUtils::saveAsSvg(m_lucide, iconName, filePath);
            break;
        case ExportFormat::PNG:
            filePath = m_outputDir + "/" + iconName + ".png";
            success = ExportUtils::saveAsPng(m_lucide, iconName, filePath, m_exportSize);
            break;
        case ExportFormat::ICO:
        case ExportFormat::ICNS:
            filePath = m_outputDir + "/" + iconName + ".ico";
            success = ExportUtils::saveAsIco(m_lucide, iconName, filePath, m_exportSize);
            break;
    }

    if (success) {
        m_exportedCount++;
    } else {
        m_failedCount++;
        qWarning() << "Failed to export icon:" << iconName;
    }

    emit progressChanged(currentIndex + 1, totalIcons);

    // Schedule next step
    QTimer::singleShot(0, this, &BatchExportManager::doExportStep);
}

bool BatchExportManager::isExporting() const {
    return m_exporting;
}

void BatchExportManager::cancel() {
    if (m_exporting) {
        m_shouldCancel = true;
    }
}

void BatchExportManager::setExportFormat(ExportFormat format) {
    m_exportFormat = format;
}

void BatchExportManager::addTask(const ExportTask& task) {
    Q_UNUSED(task);
    // Placeholder for task-based API compatibility
}

void BatchExportManager::startExport() {
    // Placeholder for task-based API compatibility
}

} // namespace gallery
