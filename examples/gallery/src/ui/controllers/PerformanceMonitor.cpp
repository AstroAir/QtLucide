/**
 * QtLucide Gallery Application - Performance Monitor Implementation
 */

#include "PerformanceMonitor.h"
#include <QApplication>
#include <QDebug>

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <psapi.h>
#elif defined(Q_OS_LINUX)
    #include <fstream>
    #include <unistd.h>
#elif defined(Q_OS_MAC)
    #include <mach/mach.h>
#endif

PerformanceMonitor::PerformanceMonitor(QObject* parent)
    : QObject(parent), m_isMonitoring(false), m_updateTimer(new QTimer(this)),
      m_monitoringInterval(DEFAULT_MONITORING_INTERVAL), m_currentFPS(0), m_currentMemoryUsage(0),
      m_currentCPUUsage(0.0), m_currentPerformanceLevel(HighPerformance), m_frameCount(0),
      m_lastFPSUpdate(0), m_peakMemoryUsage(0), m_averageMemoryUsage(0), m_lastCPUTime(0),
      m_lastSystemTime(0), m_fpsWarningThreshold(DEFAULT_FPS_THRESHOLD),
      m_memoryWarningThreshold(DEFAULT_MEMORY_THRESHOLD),
      m_cpuWarningThreshold(DEFAULT_CPU_THRESHOLD), m_hardwareAccelerationEnabled(true),
      m_adaptiveQualityEnabled(false) {
    // Connect timer to update slot
    connect(m_updateTimer, &QTimer::timeout, this, &PerformanceMonitor::updateMetrics);

    // Start FPS timer
    m_fpsTimer.start();
}

PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

// Monitoring control

void PerformanceMonitor::startMonitoring() {
    if (m_isMonitoring) {
        return;
    }

    m_isMonitoring = true;
    m_updateTimer->start(m_monitoringInterval);
    m_fpsTimer.restart();
    m_frameCount = 0;
    m_lastFPSUpdate = 0;

    qDebug() << "Performance monitoring started";
}

void PerformanceMonitor::stopMonitoring() {
    if (!m_isMonitoring) {
        return;
    }

    m_isMonitoring = false;
    m_updateTimer->stop();

    qDebug() << "Performance monitoring stopped";
}

bool PerformanceMonitor::isMonitoring() const {
    return m_isMonitoring;
}

void PerformanceMonitor::setMonitoringInterval(int milliseconds) {
    m_monitoringInterval = milliseconds;
    if (m_isMonitoring) {
        m_updateTimer->setInterval(milliseconds);
    }
}

// Metrics access

QVariantMap PerformanceMonitor::getCurrentMetrics() const {
    QVariantMap metrics;
    metrics["fps"] = m_currentFPS;
    metrics["memoryUsage"] = static_cast<qulonglong>(m_currentMemoryUsage);
    metrics["memoryUsageMB"] = m_currentMemoryUsage / (1024.0 * 1024.0);
    metrics["peakMemoryUsage"] = static_cast<qulonglong>(m_peakMemoryUsage);
    metrics["peakMemoryUsageMB"] = m_peakMemoryUsage / (1024.0 * 1024.0);
    metrics["cpuUsage"] = m_currentCPUUsage;
    metrics["performanceLevel"] = static_cast<int>(m_currentPerformanceLevel);
    metrics["hardwareAcceleration"] = m_hardwareAccelerationEnabled;
    metrics["adaptiveQuality"] = m_adaptiveQualityEnabled;
    return metrics;
}

int PerformanceMonitor::getCurrentFPS() const {
    return m_currentFPS;
}

qint64 PerformanceMonitor::getCurrentMemoryUsage() const {
    return m_currentMemoryUsage;
}

double PerformanceMonitor::getCurrentCPUUsage() const {
    return m_currentCPUUsage;
}

PerformanceMonitor::PerformanceLevel PerformanceMonitor::getCurrentPerformanceLevel() const {
    return m_currentPerformanceLevel;
}

// Thresholds

void PerformanceMonitor::setFPSWarningThreshold(int fps) {
    m_fpsWarningThreshold = fps;
}

void PerformanceMonitor::setMemoryWarningThreshold(qint64 bytes) {
    m_memoryWarningThreshold = bytes;
}

void PerformanceMonitor::setCPUWarningThreshold(double percentage) {
    m_cpuWarningThreshold = percentage;
}

// Optimization

void PerformanceMonitor::enableHardwareAcceleration(bool enabled) {
    m_hardwareAccelerationEnabled = enabled;
    qDebug() << "Hardware acceleration" << (enabled ? "enabled" : "disabled");
}

void PerformanceMonitor::setAdaptiveQuality(bool enabled) {
    m_adaptiveQualityEnabled = enabled;
    qDebug() << "Adaptive quality" << (enabled ? "enabled" : "disabled");
}

void PerformanceMonitor::cleanupUnusedResources() {
    // Trigger garbage collection if available
    // This is a placeholder - actual implementation would depend on resource types
    qDebug() << "Cleaning up unused resources";
}

// Profiling

void PerformanceMonitor::startProfiling(const QString& operation) {
    if (!m_profilingTimers.contains(operation)) {
        m_profilingTimers[operation] = QElapsedTimer();
    }
    m_profilingTimers[operation].start();
}

void PerformanceMonitor::endProfiling(const QString& operation) {
    if (m_profilingTimers.contains(operation)) {
        qint64 elapsed = m_profilingTimers[operation].elapsed();
        m_profilingResults[operation] = elapsed;
        qDebug() << "Profiling:" << operation << "took" << elapsed << "ms";
    }
}

qint64 PerformanceMonitor::getProfilingTime(const QString& operation) const {
    return m_profilingResults.value(operation, -1);
}

// Private slots

void PerformanceMonitor::updateMetrics() {
    calculateFPS();
    calculateMemoryUsage();
    calculateCPUUsage();
    updatePerformanceLevel();
    checkThresholds();

    emit metricsUpdated(getCurrentMetrics());
}

void PerformanceMonitor::checkThresholds() {
    emitWarningsIfNeeded();
}

// Private implementation

void PerformanceMonitor::calculateFPS() {
    m_frameCount++;

    qint64 elapsed = m_fpsTimer.elapsed();
    if (elapsed - m_lastFPSUpdate >= 1000) { // Update every second
        m_currentFPS = static_cast<int>((m_frameCount * 1000.0) / (elapsed - m_lastFPSUpdate));
        m_frameCount = 0;
        m_lastFPSUpdate = elapsed;
    }
}

void PerformanceMonitor::calculateMemoryUsage() {
    m_currentMemoryUsage = getProcessMemoryUsage();

    // Update peak
    if (m_currentMemoryUsage > m_peakMemoryUsage) {
        m_peakMemoryUsage = m_currentMemoryUsage;
    }

    // Update average (simple moving average)
    static int sampleCount = 0;
    static qint64 totalMemory = 0;
    totalMemory += m_currentMemoryUsage;
    sampleCount++;
    m_averageMemoryUsage = totalMemory / sampleCount;
}

void PerformanceMonitor::calculateCPUUsage() {
    m_currentCPUUsage = getProcessCPUUsage();
}

void PerformanceMonitor::updatePerformanceLevel() {
    PerformanceLevel oldLevel = m_currentPerformanceLevel;

    // Determine performance level based on metrics
    if (m_currentFPS >= 60 && m_currentMemoryUsage < m_memoryWarningThreshold / 2) {
        m_currentPerformanceLevel = HighPerformance;
    } else if (m_currentFPS >= 30 && m_currentMemoryUsage < m_memoryWarningThreshold) {
        m_currentPerformanceLevel = MediumPerformance;
    } else if (m_currentFPS >= 15) {
        m_currentPerformanceLevel = LowPerformance;
    } else {
        m_currentPerformanceLevel = CriticalPerformance;
    }

    // Emit signal if level changed
    if (m_currentPerformanceLevel != oldLevel) {
        emit performanceLevelChanged(m_currentPerformanceLevel);

        // Apply adaptive quality if enabled
        if (m_adaptiveQualityEnabled) {
            QString message = QString("Performance level changed to %1")
                                  .arg(static_cast<int>(m_currentPerformanceLevel));
            qDebug() << message;
        }
    }
}

qint64 PerformanceMonitor::getProcessMemoryUsage() const {
#ifdef Q_OS_WIN
    PROCESS_MEMORY_COUNTERS pmc;
    if (K32GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return static_cast<qint64>(pmc.WorkingSetSize);
    }
#elif defined(Q_OS_LINUX)
    std::ifstream statm("/proc/self/statm");
    long pages = 0;
    if (statm >> pages) {
        long pageSize = sysconf(_SC_PAGESIZE);
        return static_cast<qint64>(pages * pageSize);
    }
#elif defined(Q_OS_MAC)
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &size) == KERN_SUCCESS) {
        return static_cast<qint64>(info.resident_size);
    }
#endif
    return 0;
}

double PerformanceMonitor::getProcessCPUUsage() const {
    // Simplified CPU usage calculation
    // A full implementation would track CPU time deltas
    return 0.0; // Placeholder
}

void PerformanceMonitor::emitWarningsIfNeeded() {
    // Check FPS threshold
    if (m_currentFPS < m_fpsWarningThreshold && m_currentFPS > 0) {
        emit fpsWarning(m_currentFPS, m_fpsWarningThreshold);
    }

    // Check memory threshold
    if (m_currentMemoryUsage > m_memoryWarningThreshold) {
        emit memoryWarning(m_currentMemoryUsage, m_memoryWarningThreshold);
    }

    // Check CPU threshold
    if (m_currentCPUUsage > m_cpuWarningThreshold) {
        emit cpuWarning(m_currentCPUUsage, m_cpuWarningThreshold);
    }

    // Emit general warning for critical performance
    if (m_currentPerformanceLevel == CriticalPerformance) {
        emit performanceWarning("Critical performance level detected");
    }
}
