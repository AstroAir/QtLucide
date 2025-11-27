/**
 * QtLucide Gallery Application - Performance Monitor
 *
 * Monitors and tracks application performance metrics including:
 * - Frame rate (FPS)
 * - Memory usage
 * - CPU usage
 * - Event processing time
 * - Resource utilization
 *
 * Provides performance warnings and optimization suggestions.
 */

#ifndef PERFORMANCEMONITOR_H
#define PERFORMANCEMONITOR_H

#include <QElapsedTimer>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantMap>

/**
 * @brief Performance monitoring and optimization controller
 *
 * This controller tracks various performance metrics and provides
 * warnings when performance thresholds are exceeded.
 */
class PerformanceMonitor : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Performance levels for adaptive quality
     */
    enum PerformanceLevel {
        HighPerformance = 0,    // >60 FPS, low memory usage
        MediumPerformance = 1,  // 30-60 FPS, moderate memory
        LowPerformance = 2,     // <30 FPS, high memory usage
        CriticalPerformance = 3 // Severe performance issues
    };
    Q_ENUM(PerformanceLevel)

    /**
     * @brief Construct a new Performance Monitor
     * @param parent Parent QObject for memory management
     */
    explicit PerformanceMonitor(QObject* parent = nullptr);
    ~PerformanceMonitor();

    // Monitoring control
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    void setMonitoringInterval(int milliseconds);

    // Metrics access
    QVariantMap getCurrentMetrics() const;
    int getCurrentFPS() const;
    qint64 getCurrentMemoryUsage() const;
    double getCurrentCPUUsage() const;
    PerformanceLevel getCurrentPerformanceLevel() const;

    // Thresholds
    void setFPSWarningThreshold(int fps);
    void setMemoryWarningThreshold(qint64 bytes);
    void setCPUWarningThreshold(double percentage);

    // Optimization
    void enableHardwareAcceleration(bool enabled);
    void setAdaptiveQuality(bool enabled);
    void cleanupUnusedResources();

    // Profiling
    void startProfiling(const QString& operation);
    void endProfiling(const QString& operation);
    qint64 getProfilingTime(const QString& operation) const;

signals:
    /**
     * @brief Emitted when performance metrics are updated
     * @param metrics Map of metric names to values
     */
    void metricsUpdated(const QVariantMap& metrics);

    /**
     * @brief Emitted when performance level changes
     * @param level New performance level
     */
    void performanceLevelChanged(PerformanceLevel level);

    /**
     * @brief Emitted when FPS drops below threshold
     * @param currentFPS Current frames per second
     * @param threshold Warning threshold
     */
    void fpsWarning(int currentFPS, int threshold);

    /**
     * @brief Emitted when memory usage exceeds threshold
     * @param currentUsage Current memory usage in bytes
     * @param threshold Warning threshold in bytes
     */
    void memoryWarning(qint64 currentUsage, qint64 threshold);

    /**
     * @brief Emitted when CPU usage exceeds threshold
     * @param currentUsage Current CPU usage percentage
     * @param threshold Warning threshold percentage
     */
    void cpuWarning(double currentUsage, double threshold);

    /**
     * @brief Emitted for general performance warnings
     * @param message Warning message
     */
    void performanceWarning(const QString& message);

private slots:
    void updateMetrics();
    void checkThresholds();

private:
    // Metric calculation
    void calculateFPS();
    void calculateMemoryUsage();
    void calculateCPUUsage();
    void updatePerformanceLevel();

    // Helper methods
    qint64 getProcessMemoryUsage() const;
    double getProcessCPUUsage() const;
    void emitWarningsIfNeeded();

    // Monitoring state
    bool m_isMonitoring;
    QTimer* m_updateTimer;
    int m_monitoringInterval;

    // Metrics
    int m_currentFPS;
    qint64 m_currentMemoryUsage;
    double m_currentCPUUsage;
    PerformanceLevel m_currentPerformanceLevel;

    // FPS tracking
    QElapsedTimer m_fpsTimer;
    int m_frameCount;
    qint64 m_lastFPSUpdate;

    // Memory tracking
    qint64 m_peakMemoryUsage;
    qint64 m_averageMemoryUsage;

    // CPU tracking
    qint64 m_lastCPUTime;
    qint64 m_lastSystemTime;

    // Thresholds
    int m_fpsWarningThreshold;
    qint64 m_memoryWarningThreshold;
    double m_cpuWarningThreshold;

    // Optimization settings
    bool m_hardwareAccelerationEnabled;
    bool m_adaptiveQualityEnabled;

    // Profiling
    QMap<QString, QElapsedTimer> m_profilingTimers;
    QMap<QString, qint64> m_profilingResults;

    // Constants
    static constexpr int DEFAULT_MONITORING_INTERVAL = 1000; // 1 second
    static constexpr int DEFAULT_FPS_THRESHOLD = 30;
    static constexpr qint64 DEFAULT_MEMORY_THRESHOLD = 512 * 1024 * 1024; // 512 MB
    static constexpr double DEFAULT_CPU_THRESHOLD = 80.0;                 // 80%
};

#endif // PERFORMANCEMONITOR_H
