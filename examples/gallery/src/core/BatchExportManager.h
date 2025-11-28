/**
 * @file BatchExportManager.h
 * @brief Batch icon export functionality for the gallery application
 * @details Provides batch export of multiple icons in various formats (SVG, PNG)
 *          with progress reporting and error handling.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef BATCHEXPORTMANAGER_H
#define BATCHEXPORTMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QThread>
#include <memory>

#include "GalleryTypes.h"

namespace gallery {

// ExportFormat is defined in GalleryTypes.h

/**
 * @class ExportProgressWidget
 * @brief Simple progress dialog widget for batch export operations
 * @details Displays export progress with a progress bar, current icon name,
 *          and cancel button for the user to stop the operation.
 */
class ExportProgressWidget : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct an ExportProgressWidget
     * @param parent Parent QObject
     */
    explicit ExportProgressWidget(QObject* parent = nullptr);

    /**
     * @brief Destroy the widget
     */
    ~ExportProgressWidget() override = default;

    /**
     * @brief Show the progress dialog
     * @param total Total number of items to export
     */
    void show(int total);

    /**
     * @brief Hide the progress dialog
     */
    void hide();

    /**
     * @brief Update progress with current item information
     * @param current Current item number (1-based)
     * @param iconName Name of the icon being exported
     */
    void updateProgress(int current, const QString& iconName);

    /**
     * @brief Check if the user has requested cancellation
     * @return true if cancel was requested, false otherwise
     */
    [[nodiscard]] bool isCancelled() const;

Q_SIGNALS:
    /**
     * @brief Emitted when the user clicks the cancel button
     */
    void cancelRequested();

private:
    bool m_cancelled{false}; ///< Whether cancellation was requested
};

/**
 * @class BatchExportManager
 * @brief Manages batch export of multiple icons in specified formats
 * @details Provides functionality to export multiple icons from the QtLucide
 *          library at once. Supports multiple formats (SVG, PNG) and provides
 *          progress reporting via signals.
 *
 * @par Usage:
 * @code
 * BatchExportManager exporter;
 * QStringList icons = {"house", "settings", "user"};
 *
 * // Connect to progress signals
 * connect(&exporter, &BatchExportManager::progressChanged,
 *         this, [](int current, int total) {
 *             qDebug() << "Progress:" << current << "/" << total;
 *         });
 *
 * // Export icons
 * exporter.exportIcons(icons, ExportFormat::PNG, 64, "/output/path");
 * @endcode
 *
 * @see ExportFormat, ExportProgressWidget
 */
class BatchExportManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a BatchExportManager instance
     * @param parent Parent QObject for memory management
     */
    explicit BatchExportManager(QObject* parent = nullptr);

    /**
     * @brief Destroy the BatchExportManager instance
     */
    ~BatchExportManager() override;

    /**
     * @brief Export multiple icons in the specified format
     * @param iconNames List of icon names to export
     * @param format Export format (SVG or PNG)
     * @param size Icon size in pixels (used for PNG and raster rendering)
     * @param outputDir Output directory path
     * @return true if export started successfully, false if already exporting
     * @details Begins exporting the specified icons to the output directory.
     *          The export runs in a background thread to prevent UI blocking.
     *          Progress is reported via the progressChanged() signal.
     *
     * @note For SVG format, size parameter is used for viewport dimensions
     * @note For PNG format, size determines the pixel dimensions of output
     * @note The output directory must exist and be writable
     *
     * @see progressChanged(), exportFinished()
     */
    bool exportIcons(const QStringList& iconNames, ExportFormat format, int size,
                     const QString& outputDir);

    /**
     * @brief Check if an export operation is currently in progress
     * @return true if exporting, false otherwise
     */
    [[nodiscard]] bool isExporting() const;

Q_SIGNALS:
    /**
     * @brief Emitted periodically during export to report progress
     * @param current Number of icons exported so far (0-based index)
     * @param total Total number of icons to export
     * @details This signal is emitted after each icon is successfully exported.
     *          Use this to update progress bars or status displays.
     *
     * @see exportFinished()
     */
    void progressChanged(int current, int total);

    /**
     * @brief Emitted when the export operation completes
     * @param success true if all icons exported successfully, false if cancelled or error
     * @param exported Number of icons successfully exported
     * @param failed Number of icons that failed to export
     * @param errorMessage Error message if applicable (empty if success)
     * @details This signal indicates the export operation has finished.
     *          Check the success flag and message for operation status.
     *
     * @see exportIcons()
     */
    void exportFinished(bool success, int exported, int failed, const QString& errorMessage);

public Q_SLOTS:
    /**
     * @brief Cancel the current export operation
     * @details Stops the currently running export operation and emits exportFinished()
     *          with success=false.
     *
     * @see exportIcons(), isExporting()
     */
    void cancel();

private:
    /**
     * @brief Perform the actual export work (runs in worker thread)
     * @param iconNames List of icon names to export
     * @param format Export format
     * @param size Icon size
     * @param outputDir Output directory
     */
    void doExport(const QStringList& iconNames, ExportFormat format, int size,
                  const QString& outputDir);

    bool m_exporting{false}; ///< Whether an export is currently in progress
    bool m_shouldCancel{false}; ///< Flag to request cancellation
    std::unique_ptr<QThread> m_workerThread; ///< Worker thread for background export
};

} // namespace gallery

#endif // BATCHEXPORTMANAGER_H
