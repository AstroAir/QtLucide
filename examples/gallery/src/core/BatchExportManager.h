/**
 * QtLucide Gallery Application - Batch Export Manager
 *
 * A powerful batch export manager for handling icon exports:
 * - Asynchronous processing to avoid UI blocking
 * - Multiple format support (PNG, SVG, ICO, PDF, JPEG)
 * - Progress tracking and cancellation
 * - Error handling and logging
 * - Memory-efficient processing
 * - Thread-safe operations
 * - Customizable export settings
 * - File conflict resolution
 */

#ifndef BATCHEXPORTMANAGER_H
#define BATCHEXPORTMANAGER_H

#include <QBuffer>
#include <QDateTime>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QImageWriter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QPainter>
#include <QPdfWriter>
#include <QPixmap>
#include <QQueue>
#include <QStringList>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QWaitCondition>
#include <QtConcurrent>

#ifdef QTLUCIDE_AVAILABLE
    #include <QtLucide/QtLucide.h>
#endif
#include "../ui/dialogs/ExportDialog.h"

// Forward declarations
class ExportTask;
class ExportWorker;

/**
 * @brief Individual export task
 */
struct ExportTask {
    QString iconName;
    QString outputPath;
    ExportFormat format;
    int width;
    int height;
    QColor iconColor;
    QColor backgroundColor;
    bool transparentBackground;
    bool useCustomColor;
    int jpegQuality;
    bool antialiasing;
    QHash<QString, QString> metadata;

    // Status
    bool completed = false;
    bool success = false;
    QString errorMessage;
    qint64 fileSize = 0;
    QDateTime completedAt;
};

/**
 * @brief Export statistics
 */
struct ExportStats {
    int totalTasks = 0;
    int completedTasks = 0;
    int successfulTasks = 0;
    int failedTasks = 0;
    qint64 totalFileSize = 0;
    QElapsedTimer totalTime;
    QStringList exportedFiles;
    QStringList failedFiles;
    QHash<QString, QString> errors;
};

/**
 * @brief Export worker thread
 */
class ExportWorker : public QObject {
    Q_OBJECT

public:
#ifdef QTLUCIDE_AVAILABLE
    explicit ExportWorker(lucide::QtLucide* lucide, QObject* parent = nullptr);
    void setLucide(lucide::QtLucide* lucide) { m_lucide = lucide; }
#else
    explicit ExportWorker(void* lucide, QObject* parent = nullptr);
    void setLucide(void* lucide) { m_lucide = lucide; }
#endif
    ~ExportWorker();

public slots:
    void processTask(const ExportTask& task);
    void cancelProcessing();

signals:
    void taskCompleted(const ExportTask& task);
    void taskFailed(const ExportTask& task, const QString& error);
    void processingCancelled();

private:
    bool exportToPNG(const ExportTask& task);
    bool exportToSVG(const ExportTask& task);
    bool exportToICO(const ExportTask& task);
    bool exportToPDF(const ExportTask& task);
    bool exportToJPEG(const ExportTask& task);

    QPixmap renderIcon(const QString& iconName, int width, int height, const QColor& color,
                       bool useCustomColor, bool antialiasing);
    QString getSVGData(const QString& iconName, const QColor& color, bool useCustomColor);
    bool writeMetadata(const QString& filePath, const QHash<QString, QString>& metadata);
    bool createDirectoryIfNeeded(const QString& filePath);

#ifdef QTLUCIDE_AVAILABLE
    lucide::QtLucide* m_lucide;
#else
    void* m_lucide;
#endif
    QAtomicInt m_cancelled;
};

/**
 * @brief Main batch export manager
 */
class BatchExportManager : public QObject {
    Q_OBJECT

public:
    enum ProcessingMode {
        Sequential = 0, // Process one task at a time
        Parallel = 1    // Process multiple tasks in parallel
    };

    explicit BatchExportManager(QObject* parent = nullptr);
    ~BatchExportManager();

    // Configuration
#ifdef QTLUCIDE_AVAILABLE
    void setLucide(lucide::QtLucide* lucide);
#else
    void setLucide(void* lucide);
#endif
    void setProcessingMode(ProcessingMode mode) { m_processingMode = mode; }
    ProcessingMode processingMode() const { return m_processingMode; }
    void setMaxConcurrentTasks(int maxTasks);
    int maxConcurrentTasks() const { return m_maxConcurrentTasks; }

    // Task management
    void addExportTask(const ExportTask& task);
    void addExportTasks(const QList<ExportTask>& tasks);
    void clearTasks();
    int taskCount() const;
    int pendingTaskCount() const;
    int completedTaskCount() const;

    // Processing control
    void startExport();
    void cancelExport();
    void pauseExport();
    void resumeExport();
    bool isExporting() const { return m_isExporting; }
    bool isPaused() const { return m_isPaused; }
    bool isCancelled() const { return m_isCancelled; }

    // Statistics
    ExportStats getStatistics() const;
    void resetStatistics();

    // Utility methods
    static QList<ExportTask> createExportTasks(const QStringList& iconNames,
                                               const ExportConfig& config);
    static QString generateOutputPath(const QString& iconName, const ExportConfig& config,
                                      ExportFormat format, int width, int height);
    static QString formatToString(ExportFormat format);
    static QString formatToExtension(ExportFormat format);

signals:
    void exportStarted();
    void exportProgress(int completed, int total, const QString& currentFile);
    void exportPaused();
    void exportResumed();
    void exportFinished(bool success);
    void exportCancelled();
    void taskCompleted(const ExportTask& task);
    void taskFailed(const ExportTask& task, const QString& error);
    void statisticsUpdated(const ExportStats& stats);

protected:
    void timerEvent(QTimerEvent* event) override;

private slots:
    void onTaskCompleted(const ExportTask& task);
    void onTaskFailed(const ExportTask& task, const QString& error);
    void onWorkerFinished();
    void processNextTasks();

private:
    void setupWorkers();
    void cleanupWorkers();
    void updateStatistics();
    void emitProgress();
    bool shouldProcessNextTask() const;
    ExportTask getNextTask();
    void startWorkerTask(ExportWorker* worker, const ExportTask& task);

    // Core data
#ifdef QTLUCIDE_AVAILABLE
    lucide::QtLucide* m_lucide;
#else
    void* m_lucide;
#endif
    QQueue<ExportTask> m_pendingTasks;
    QList<ExportTask> m_completedTasks;
    QHash<ExportWorker*, ExportTask> m_activeTasks;

    // Workers and threading
    QList<ExportWorker*> m_workers;
    QList<QThread*> m_workerThreads;
    ProcessingMode m_processingMode;
    int m_maxConcurrentTasks;

    // State management
    bool m_isExporting;
    bool m_isPaused;
    bool m_isCancelled;
    QMutex m_taskMutex;
    QWaitCondition m_pauseCondition;

    // Statistics and monitoring
    ExportStats m_stats;
    int m_progressTimerId;
    QElapsedTimer m_exportTimer;

    // Constants
    static constexpr int DEFAULT_MAX_CONCURRENT_TASKS = 4;
    static constexpr int PROGRESS_UPDATE_INTERVAL = 100; // ms
};

/**
 * @brief Export progress widget for showing export status
 */
class ExportProgressWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExportProgressWidget(QWidget* parent = nullptr);
    ~ExportProgressWidget();

    // Progress display
    void setProgress(int current, int total);
    void setCurrentFile(const QString& fileName);
    void setStatus(const QString& status);
    void setElapsedTime(qint64 milliseconds);
    void setEstimatedTimeRemaining(qint64 milliseconds);

    // Statistics display
    void setStatistics(const ExportStats& stats);
    void showStatistics(bool show);

    // Control
    void reset();
    void setShowCancelButton(bool show);
    void setShowPauseButton(bool show);

signals:
    void cancelRequested();
    void pauseRequested();
    void resumeRequested();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onCancelClicked();
    void onPauseClicked();
    void updateDisplay();

private:
    void setupUI();
    void updateProgressText();
    void updateTimeDisplay();
    QString formatTime(qint64 milliseconds) const;
    QString formatFileSize(qint64 bytes) const;

    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_progressLayout;
    QHBoxLayout* m_buttonLayout;

    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
    QLabel* m_progressLabel;
    QLabel* m_currentFileLabel;
    QLabel* m_timeLabel;
    QLabel* m_statsLabel;

    QPushButton* m_cancelButton;
    QPushButton* m_pauseButton;

    // Data
    int m_currentProgress;
    int m_totalProgress;
    QString m_currentFile;
    QString m_status;
    qint64 m_elapsedTime;
    qint64 m_estimatedTimeRemaining;
    ExportStats m_stats;
    bool m_showStatistics;
    bool m_isPaused;

    // Update timer
    QTimer* m_updateTimer;
};

#endif // BATCHEXPORTMANAGER_H
