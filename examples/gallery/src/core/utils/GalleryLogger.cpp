/**
 * QtLucide Gallery Application - Enhanced Logging System Implementation
 *
 * A modern, high-performance logging system with improved architecture,
 * better performance monitoring, and enhanced thread safety.
 */

#include "GalleryLogger.h"
#include <QApplication>
#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QStringConverter>
#include <QThread>

// Define enhanced logging categories
Q_LOGGING_CATEGORY(galleryMain, "gallery.main")
Q_LOGGING_CATEGORY(galleryInit, "gallery.init")
Q_LOGGING_CATEGORY(galleryUI, "gallery.ui")
Q_LOGGING_CATEGORY(galleryIcon, "gallery.icon")
Q_LOGGING_CATEGORY(gallerySearch, "gallery.search")
Q_LOGGING_CATEGORY(galleryFilter, "gallery.filter")
Q_LOGGING_CATEGORY(galleryExport, "gallery.export")
Q_LOGGING_CATEGORY(gallerySettings, "gallery.settings")
Q_LOGGING_CATEGORY(galleryPerf, "gallery.perf")
Q_LOGGING_CATEGORY(galleryMemory, "gallery.memory")
Q_LOGGING_CATEGORY(galleryCache, "gallery.cache")
Q_LOGGING_CATEGORY(galleryAnimation, "gallery.animation")
Q_LOGGING_CATEGORY(galleryMetadata, "gallery.metadata")

/**
 * @brief Background logging worker for asynchronous log processing
 */
class LogWorker : public QObject {
    Q_OBJECT

public:
    explicit LogWorker(QObject* parent = nullptr)
        : QObject(parent), m_processTimer(new QTimer(this)) {
        m_processTimer->setInterval(GalleryLogger::LOG_PROCESS_INTERVAL_MS);
        connect(m_processTimer, &QTimer::timeout, this, &LogWorker::processLogQueue);
        m_processTimer->start();
    }

    ~LogWorker() = default;

    void enqueueLogEntry(const QString& formattedMessage) {
        QMutexLocker locker(&m_queueMutex);
        m_logQueue.enqueue(formattedMessage);
        m_queueCondition.wakeOne();
    }

    void stop() {
        m_stopRequested = true;
        m_queueCondition.wakeAll();
        m_processTimer->stop();
    }

public slots:
    void processLogQueue() {
        QMutexLocker locker(&m_queueMutex);
        while (!m_logQueue.isEmpty() && !m_stopRequested) {
            QString message = m_logQueue.dequeue();
            locker.unlock();
            emit logProcessed(message);
            locker.relock();
        }
    }

signals:
    void logProcessed(const QString& message);

private:
    QQueue<QString> m_logQueue;
    QMutex m_queueMutex;
    QWaitCondition m_queueCondition;
    bool m_stopRequested = false;
    QTimer* m_processTimer;
};

// Static members
GalleryLogger* GalleryLogger::s_instance = nullptr;
QMutex GalleryLogger::s_instanceMutex;

GalleryLogger* GalleryLogger::instance() {
    QMutexLocker locker(&s_instanceMutex);
    if (!s_instance) {
        s_instance = new GalleryLogger();
    }
    return s_instance;
}

void GalleryLogger::cleanup() {
    QMutexLocker locker(&s_instanceMutex);
    delete s_instance;
    s_instance = nullptr;
}

GalleryLogger::GalleryLogger(QObject* parent)
    : QObject(parent), m_logLevel(Info), m_outputTargets(Both), m_logFormat(Detailed),
      m_maxFileSize(DEFAULT_MAX_FILE_SIZE), m_maxBackupFiles(DEFAULT_MAX_BACKUP_FILES),
      m_asyncLogging(true), m_logThread(nullptr), m_logWorker(nullptr),
      m_flushTimer(new QTimer(this)), m_logCount(0) {
    // Set up log directory and file path
    m_logDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(m_logDirectory);
    m_logFilePath = generateLogFileName();

    // Setup auto-flush timer
    m_flushTimer->setInterval(FLUSH_INTERVAL_MS);
    m_flushTimer->setSingleShot(false);
    connect(m_flushTimer, &QTimer::timeout, this, &GalleryLogger::flush);

    // Start uptime timer
    m_uptimeTimer.start();
}

GalleryLogger::~GalleryLogger() {
    if (m_logWorker) {
        m_logWorker->stop();
    }

    if (m_logThread) {
        m_logThread->quit();
        m_logThread->wait(5000);
        delete m_logThread;
    }

    flush();
}

bool GalleryLogger::initialize(bool enableFileLogging, bool enableConsoleLogging) {
    // Set output targets
    if (enableFileLogging && enableConsoleLogging) {
        m_outputTargets = Both;
    } else if (enableFileLogging) {
        m_outputTargets = File;
    } else {
        m_outputTargets = Console;
    }

    // Setup file logging
    if (enableFileLogging) {
        setupLogFile();
    }

    // Setup asynchronous logging
    if (m_asyncLogging) {
        setupAsyncLogging();
    }

    // Start auto-flush timer
    m_flushTimer->start();

    // Log initialization
    info(galleryMain(),
         QString("Enhanced Gallery Logger initialized - File: %1, Console: %2, Async: %3")
             .arg(enableFileLogging ? "enabled" : "disabled")
             .arg(enableConsoleLogging ? "enabled" : "disabled")
             .arg(m_asyncLogging ? "enabled" : "disabled"));

    info(galleryMain(), QString("Log file path: %1").arg(m_logFilePath));
    info(galleryMain(), QString("Qt version: %1").arg(QT_VERSION_STR));

    if (QCoreApplication::instance()) {
        info(galleryMain(),
             QString("Application version: %1").arg(QCoreApplication::applicationVersion()));
    }

    logMemoryUsage("Logger initialization");
    return true;
}

void GalleryLogger::setLogLevel(LogLevel level) {
    m_logLevel = level;
    info(galleryMain(), QString("Log level set to: %1").arg(levelToString(level)));
}

void GalleryLogger::setOutputTargets(OutputTargets targets) {
    m_outputTargets = targets;
    info(galleryMain(), QString("Output targets updated"));
}

void GalleryLogger::setLogFormat(LogFormat format) {
    m_logFormat = format;
    info(galleryMain(), QString("Log format set to: %1").arg(static_cast<int>(format)));
}

void GalleryLogger::setMaxFileSize(qint64 maxSize) {
    m_maxFileSize = maxSize;
    info(galleryMain(), QString("Max file size set to: %1 bytes").arg(maxSize));
}

void GalleryLogger::setMaxBackupFiles(int maxFiles) {
    m_maxBackupFiles = maxFiles;
    info(galleryMain(), QString("Max backup files set to: %1").arg(maxFiles));
}

void GalleryLogger::setAsyncLogging(bool enabled) {
    if (m_asyncLogging != enabled) {
        m_asyncLogging = enabled;
        if (enabled) {
            setupAsyncLogging();
        } else if (m_logWorker) {
            m_logWorker->stop();
        }
        info(galleryMain(), QString("Async logging %1").arg(enabled ? "enabled" : "disabled"));
    }
}

void GalleryLogger::startTimer(const QString& name, const QString& context) {
    QMutexLocker locker(&m_timerMutex);
    TimerInfo& info = m_timers[name];
    info.timer.start();
    info.context = context;
    info.callCount++;
    debug(galleryPerf(), QString("Timer started: %1 %2").arg(name, context));
}

qint64 GalleryLogger::endTimer(const QString& name) {
    QMutexLocker locker(&m_timerMutex);

    if (m_timers.contains(name)) {
        TimerInfo& info = m_timers[name];
        if (info.timer.isValid()) {
            qint64 elapsed = info.timer.elapsed();
            info.totalTime += elapsed;
            this->info(galleryPerf(),
                       QString("Timer '%1' completed in %2ms (total: %3ms, calls: %4)")
                           .arg(name)
                           .arg(elapsed)
                           .arg(info.totalTime)
                           .arg(info.callCount));
            return elapsed;
        }
    }

    warning(galleryPerf(), QString("Timer '%1' was not started or already ended").arg(name));
    return -1;
}

// Core logging methods
void GalleryLogger::log(LogLevel level, const QLoggingCategory& category, const QString& message) {
    if (level < m_logLevel) {
        return;
    }

    m_logCount.fetchAndAddOrdered(1);

    QString formattedMessage = formatMessage(level, category, message);

    // Add to recent logs
    {
        QMutexLocker locker(&m_historyMutex);
        m_recentLogs.enqueue(formattedMessage);
        if (m_recentLogs.size() > MAX_RECENT_LOGS) {
            m_recentLogs.dequeue();
        }
    }

    // Output to targets
    if (m_asyncLogging && m_logWorker) {
        m_logWorker->enqueueLogEntry(formattedMessage);
    } else {
        if (m_outputTargets & Console) {
            writeToConsole(level, formattedMessage);
        }
        if (m_outputTargets & File) {
            writeToFile(formattedMessage);
        }
    }

    emit logMessage(level, category.categoryName(), message);
}

void GalleryLogger::trace(const QLoggingCategory& category, const QString& message) {
    log(Trace, category, message);
}

void GalleryLogger::debug(const QLoggingCategory& category, const QString& message) {
    log(Debug, category, message);
}

void GalleryLogger::info(const QLoggingCategory& category, const QString& message) {
    log(Info, category, message);
}

void GalleryLogger::warning(const QLoggingCategory& category, const QString& message) {
    log(Warning, category, message);
}

void GalleryLogger::error(const QLoggingCategory& category, const QString& message) {
    log(Error, category, message);
}

void GalleryLogger::critical(const QLoggingCategory& category, const QString& message) {
    log(Critical, category, message);
}

void GalleryLogger::logMemoryUsage(const QString& context) {
#ifdef Q_OS_WIN
    // Windows memory usage
    QProcess process;
    process.start("tasklist", QStringList()
                                  << "/FI"
                                  << QString("PID eq %1").arg(QCoreApplication::applicationPid())
                                  << "/FO" << "CSV");
    if (process.waitForFinished(3000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        if (lines.size() > 1) {
            QStringList fields = lines[1].split(',');
            if (fields.size() > 4) {
                QString memUsage = fields[4].remove('"').remove(' ');
                info(galleryMemory(), QString("Memory usage (%1): %2").arg(context, memUsage));
            }
        }
    }
#else
    // Unix-like systems
    QFile statusFile(QString("/proc/%1/status").arg(QCoreApplication::applicationPid()));
    if (statusFile.open(QIODevice::ReadOnly)) {
        QTextStream stream(&statusFile);
        QString line;
        while (stream.readLineInto(&line)) {
            if (line.startsWith("VmRSS:")) {
                info(galleryMemory(),
                     QString("Memory usage (%1): %2").arg(context, line.mid(6).trimmed()));
                break;
            }
        }
    }
#endif
}

void GalleryLogger::logPerformanceMetrics() {
    QVariantMap metrics;
    metrics["uptime_ms"] = m_uptimeTimer.elapsed();
    metrics["log_count"] = m_logCount.loadAcquire();
    metrics["timer_count"] = m_timers.size();

    info(galleryPerf(), QString("Performance metrics: uptime=%1ms, logs=%2, timers=%3")
                            .arg(metrics["uptime_ms"].toLongLong())
                            .arg(metrics["log_count"].toInt())
                            .arg(metrics["timer_count"].toInt()));

    emit performanceMetricsUpdated(metrics);
}

// Advanced features
void GalleryLogger::setLogFilter(const QString& filter) {
    m_logFilter = filter;
    info(galleryMain(), QString("Log filter set: %1").arg(filter));
}

void GalleryLogger::clearLogFilter() {
    m_logFilter.clear();
    info(galleryMain(), "Log filter cleared");
}

QStringList GalleryLogger::getRecentLogs(int count) const {
    QMutexLocker locker(&m_historyMutex);
    QStringList result;
    int start = qMax(0, static_cast<int>(m_recentLogs.size()) - count);
    for (int i = start; i < m_recentLogs.size(); ++i) {
        result.append(m_recentLogs.at(i));
    }
    return result;
}

void GalleryLogger::exportLogs(const QString& filePath, const QDateTime& from,
                               const QDateTime& to) {
    // Implementation for log export functionality
    Q_UNUSED(filePath)
    Q_UNUSED(from)
    Q_UNUSED(to)
    info(galleryMain(), "Log export functionality not yet implemented");
}

// Slot implementations
void GalleryLogger::flush() {
    QMutexLocker locker(&m_logMutex);
    if (m_logStream) {
        m_logStream->flush();
    }
}

void GalleryLogger::rotateLogFile() {
    QMutexLocker locker(&m_logMutex);
    checkLogRotation();
}

void GalleryLogger::clearLogFile() {
    QMutexLocker locker(&m_logMutex);
    if (m_logFile && m_logFile->isOpen()) {
        m_logFile->resize(0);
        info(galleryMain(), "Log file cleared");
    }
}

// Private helper methods
void GalleryLogger::setupLogFile() {
    m_logFile = std::make_unique<QFile>(m_logFilePath);
    if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
        m_logStream = std::make_unique<QTextStream>(m_logFile.get());
        m_logStream->setEncoding(QStringConverter::Utf8);
        info(galleryMain(), QString("Log file opened: %1").arg(m_logFilePath));
    } else {
        warning(galleryMain(), QString("Failed to open log file: %1").arg(m_logFilePath));
        m_logFile.reset();
    }
}

void GalleryLogger::setupAsyncLogging() {
    if (!m_logThread) {
        m_logThread = new QThread(this);
        m_logWorker = new LogWorker();
        m_logWorker->moveToThread(m_logThread);

        connect(m_logWorker, &LogWorker::logProcessed, this, [this](const QString& message) {
            if (m_outputTargets & Console) {
                // Extract level from message for console formatting
                writeToConsole(Info, message); // Default to Info level for async messages
            }
            if (m_outputTargets & File) {
                writeToFile(message);
            }
        });

        m_logThread->start();
        info(galleryMain(), "Asynchronous logging initialized");
    }
}

void GalleryLogger::writeToFile(const QString& formattedMessage) {
    QMutexLocker locker(&m_logMutex);

    if (m_logStream) {
        *m_logStream << formattedMessage << Qt::endl;
        m_logStream->flush();

        // Check for log rotation
        checkLogRotation();
    }
}

void GalleryLogger::writeToConsole(LogLevel level, const QString& formattedMessage) {
    // Use different colors for different log levels
    switch (level) {
        case Critical:
        case Error:
            qDebug().noquote() << "\033[31m" << formattedMessage << "\033[0m"; // Red
            break;
        case Warning:
            qDebug().noquote() << "\033[33m" << formattedMessage << "\033[0m"; // Yellow
            break;
        case Info:
            qDebug().noquote() << "\033[32m" << formattedMessage << "\033[0m"; // Green
            break;
        case Debug:
        case Trace:
        default:
            qDebug().noquote() << formattedMessage;
            break;
    }
}

QString GalleryLogger::formatMessage(LogLevel level, const QLoggingCategory& category,
                                     const QString& message) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString categoryStr = category.categoryName();
    QString threadId =
        QString("0x%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()), 0, 16);

    switch (m_logFormat) {
        case Json:
            return formatMessageJson(level, category, message);
        case Simple:
            return QString("[%1] %2: %3").arg(levelStr, categoryStr, message);
        case Detailed:
        default:
            return QString("[%1] [%2] [%3] [%4] %5")
                .arg(timestamp, levelStr, categoryStr, threadId, message);
    }
}

QString GalleryLogger::formatMessageJson(LogLevel level, const QLoggingCategory& category,
                                         const QString& message) {
    QJsonObject obj;
    obj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    obj["level"] = levelToString(level);
    obj["category"] = category.categoryName();
    obj["thread"] =
        QString("0x%1").arg(reinterpret_cast<quintptr>(QThread::currentThreadId()), 0, 16);
    obj["message"] = message;

    return QJsonDocument(obj).toJson(QJsonDocument::Compact);
}

QString GalleryLogger::levelToString(LogLevel level) {
    switch (level) {
        case Trace:
            return "TRACE";
        case Debug:
            return "DEBUG";
        case Info:
            return "INFO ";
        case Warning:
            return "WARN ";
        case Error:
            return "ERROR";
        case Critical:
            return "CRIT ";
        default:
            return "UNKN ";
    }
}

void GalleryLogger::checkLogRotation() {
    if (m_logFile && m_logFile->size() > m_maxFileSize) {
        performLogRotation();
    }
}

void GalleryLogger::performLogRotation() {
    if (!m_logFile)
        return;

    // Close current file
    m_logStream.reset();
    m_logFile->close();

    // Rotate backup files
    for (int i = m_maxBackupFiles - 1; i >= 1; --i) {
        QString oldFile = QString("%1.%2").arg(m_logFilePath).arg(i);
        QString newFile = QString("%1.%2").arg(m_logFilePath).arg(i + 1);

        if (QFile::exists(newFile)) {
            QFile::remove(newFile);
        }
        if (QFile::exists(oldFile)) {
            QFile::rename(oldFile, newFile);
        }
    }

    // Move current log to .1
    QString backupFile = QString("%1.1").arg(m_logFilePath);
    if (QFile::exists(backupFile)) {
        QFile::remove(backupFile);
    }
    QFile::rename(m_logFilePath, backupFile);

    // Reopen log file
    setupLogFile();

    info(galleryMain(), QString("Log file rotated. Backup: %1").arg(backupFile));
    emit logFileRotated(m_logFilePath);
}

QString GalleryLogger::generateLogFileName() {
    QString appName = QCoreApplication::applicationName();
    if (appName.isEmpty()) {
        appName = "QtLucideGallery";
    }

    return QDir(m_logDirectory).filePath(QString("%1.log").arg(appName));
}

void GalleryLogger::updatePerformanceMetrics() {
    // This method can be called periodically to update performance metrics
    // Implementation can be expanded as needed
}

// Include the MOC file for the LogWorker class
#include "GalleryLogger.moc"
