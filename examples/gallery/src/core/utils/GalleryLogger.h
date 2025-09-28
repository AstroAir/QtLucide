/**
 * QtLucide Gallery Application - Enhanced Logging System
 *
 * A modern, high-performance logging system with improved architecture,
 * better performance monitoring, and enhanced thread safety.
 */

#ifndef GALLERYLOGGER_H
#define GALLERYLOGGER_H

#include <QAtomicInt>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QHash>
#include <QLoggingCategory>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QStandardPaths>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>

#include <memory>

// Enhanced logging categories for different components
Q_DECLARE_LOGGING_CATEGORY(galleryMain)
Q_DECLARE_LOGGING_CATEGORY(galleryInit)
Q_DECLARE_LOGGING_CATEGORY(galleryUI)
Q_DECLARE_LOGGING_CATEGORY(galleryIcon)
Q_DECLARE_LOGGING_CATEGORY(gallerySearch)
Q_DECLARE_LOGGING_CATEGORY(galleryFilter)
Q_DECLARE_LOGGING_CATEGORY(galleryExport)
Q_DECLARE_LOGGING_CATEGORY(gallerySettings)
Q_DECLARE_LOGGING_CATEGORY(galleryPerf)
Q_DECLARE_LOGGING_CATEGORY(galleryMemory)
Q_DECLARE_LOGGING_CATEGORY(galleryCache)
Q_DECLARE_LOGGING_CATEGORY(galleryAnimation)
Q_DECLARE_LOGGING_CATEGORY(galleryMetadata)

// Forward declaration
class LogWorker;

/**
 * @brief Enhanced logging system for the Gallery application
 *
 * Features:
 * - Asynchronous logging for better performance
 * - Multiple log levels with fine-grained control
 * - Console and file output with formatting options
 * - Advanced performance timing measurements
 * - Memory usage tracking with detailed statistics
 * - Thread-safe operation with minimal contention
 * - Automatic log rotation with compression
 * - Structured logging with enhanced categories
 * - Real-time log filtering and searching
 * - Integration with Qt's logging framework
 */
class GalleryLogger : public QObject {
    Q_OBJECT

public:
    enum LogLevel { Trace = 0, Debug = 1, Info = 2, Warning = 3, Error = 4, Critical = 5 };
    Q_ENUM(LogLevel)

    enum OutputTarget {
        Console = 0x01,
        File = 0x02,
        Network = 0x04,
        Both = Console | File,
        All = Console | File | Network
    };
    Q_DECLARE_FLAGS(OutputTargets, OutputTarget)

    enum LogFormat { Simple, Detailed, Json, Xml };
    Q_ENUM(LogFormat)

    // Constants
    static constexpr qint64 DEFAULT_MAX_FILE_SIZE = 50 * 1024 * 1024; // 50MB
    static constexpr int DEFAULT_MAX_BACKUP_FILES = 10;
    static constexpr int FLUSH_INTERVAL_MS = 3000;      // 3 seconds
    static constexpr int LOG_PROCESS_INTERVAL_MS = 100; // 100ms

    // Singleton access
    static GalleryLogger* instance();
    static void cleanup();

    // Initialization and configuration
    bool initialize(bool enableFileLogging = true, bool enableConsoleLogging = true);
    void setLogLevel(LogLevel level);
    void setOutputTargets(OutputTargets targets);
    void setLogFormat(LogFormat format);
    void setMaxFileSize(qint64 maxSize);
    void setMaxBackupFiles(int maxFiles);
    void setAsyncLogging(bool enabled);

    // Enhanced logging methods
    void log(LogLevel level, const QLoggingCategory& category, const QString& message);
    void trace(const QLoggingCategory& category, const QString& message);
    void debug(const QLoggingCategory& category, const QString& message);
    void info(const QLoggingCategory& category, const QString& message);
    void warning(const QLoggingCategory& category, const QString& message);
    void error(const QLoggingCategory& category, const QString& message);
    void critical(const QLoggingCategory& category, const QString& message);

    // Performance monitoring with enhanced features
    void startTimer(const QString& name, const QString& context = QString());
    qint64 endTimer(const QString& name);
    void logMemoryUsage(const QString& context = QString());
    void logPerformanceMetrics();

    // Advanced features
    void setLogFilter(const QString& filter);
    void clearLogFilter();
    QStringList getRecentLogs(int count = 100) const;
    void exportLogs(const QString& filePath, const QDateTime& from = QDateTime(),
                    const QDateTime& to = QDateTime());

    // Getters
    LogLevel logLevel() const { return m_logLevel; }
    QString getLogFilePath() const { return m_logFilePath; }
    bool isFileLoggingEnabled() const { return m_outputTargets & File; }
    bool isConsoleLoggingEnabled() const { return m_outputTargets & Console; }
    bool isAsyncLoggingEnabled() const { return m_asyncLogging; }
    LogFormat logFormat() const { return m_logFormat; }

public slots:
    void flush();
    void rotateLogFile();
    void clearLogFile();

signals:
    void logMessage(GalleryLogger::LogLevel level, const QString& category, const QString& message);
    void logFileRotated(const QString& newFilePath);
    void performanceMetricsUpdated(const QVariantMap& metrics);

private:
    explicit GalleryLogger(QObject* parent = nullptr);
    ~GalleryLogger();

    void setupLogFile();
    void setupAsyncLogging();
    void writeToFile(const QString& formattedMessage);
    void writeToConsole(LogLevel level, const QString& formattedMessage);
    QString formatMessage(LogLevel level, const QLoggingCategory& category, const QString& message);
    QString formatMessageJson(LogLevel level, const QLoggingCategory& category,
                              const QString& message);
    QString levelToString(LogLevel level);
    void checkLogRotation();
    void performLogRotation();
    QString generateLogFileName();
    void updatePerformanceMetrics();

    static GalleryLogger* s_instance;
    static QMutex s_instanceMutex;

    // Configuration
    LogLevel m_logLevel;
    OutputTargets m_outputTargets;
    LogFormat m_logFormat;
    qint64 m_maxFileSize;
    int m_maxBackupFiles;
    bool m_asyncLogging;

    // File handling
    QString m_logFilePath;
    QString m_logDirectory;
    std::unique_ptr<QFile> m_logFile;
    std::unique_ptr<QTextStream> m_logStream;

    // Thread safety
    mutable QMutex m_logMutex;

    // Asynchronous logging
    QThread* m_logThread;
    LogWorker* m_logWorker;

    // Performance monitoring
    struct TimerInfo {
        QElapsedTimer timer;
        QString context;
        qint64 totalTime = 0;
        int callCount = 0;
    };
    QHash<QString, TimerInfo> m_timers;
    QMutex m_timerMutex;

    // Log filtering and history
    QString m_logFilter;
    QQueue<QString> m_recentLogs;
    mutable QMutex m_historyMutex;
    static constexpr int MAX_RECENT_LOGS = 1000;

    // Auto-flush timer
    QTimer* m_flushTimer;

    // Performance metrics
    QAtomicInt m_logCount;
    QElapsedTimer m_uptimeTimer;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GalleryLogger::OutputTargets)

// Enhanced convenience macros for logging
#define GALLERY_LOG_TRACE(category, message) GalleryLogger::instance()->trace(category(), message)

#define GALLERY_LOG_DEBUG(category, message) GalleryLogger::instance()->debug(category(), message)

#define GALLERY_LOG_INFO(category, message) GalleryLogger::instance()->info(category(), message)

#define GALLERY_LOG_WARNING(category, message)                                                     \
    GalleryLogger::instance()->warning(category(), message)

#define GALLERY_LOG_ERROR(category, message) GalleryLogger::instance()->error(category(), message)

#define GALLERY_LOG_CRITICAL(category, message)                                                    \
    GalleryLogger::instance()->critical(category(), message)

#define GALLERY_START_TIMER(name) GalleryLogger::instance()->startTimer(name)

#define GALLERY_START_TIMER_CTX(name, context) GalleryLogger::instance()->startTimer(name, context)

#define GALLERY_END_TIMER(name) GalleryLogger::instance()->endTimer(name)

#define GALLERY_LOG_MEMORY(context) GalleryLogger::instance()->logMemoryUsage(context)

#define GALLERY_LOG_PERF() GalleryLogger::instance()->logPerformanceMetrics()

#endif // GALLERYLOGGER_H
