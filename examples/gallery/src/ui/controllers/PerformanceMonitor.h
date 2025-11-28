/**
 * @file PerformanceMonitor.h
 * @brief Performance monitoring for rendering and memory tracking
 * @details This file contains the PerformanceMonitor class which tracks render times,
 *          frame rates, and optional memory usage for performance analysis.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <QObject>
#include <QString>
#include <QElapsedTimer>
#include <QList>
#include <cstdint>

namespace gallery {

/**
 * @class PerformanceMonitor
 * @brief Monitors application rendering and memory performance
 * @details This class provides performance measurement capabilities for tracking
 *          render times, calculating average performance metrics, and monitoring
 *          frame rates. It can optionally track memory usage.
 *
 * @par Features:
 * - Measurement start/stop with elapsed time tracking
 * - Rolling average calculations for render times
 * - Frame rate calculations (FPS)
 * - Optional memory usage tracking
 * - Performance statistics and reporting
 * - Signal-based performance notifications
 *
 * @par Usage:
 * @code
 * PerformanceMonitor monitor;
 * monitor.setMaxSamples(60);  // Keep last 60 samples
 *
 * // Measure rendering
 * monitor.startMeasurement("icon_render");
 * renderIcons();
 * monitor.endMeasurement("icon_render");
 *
 * // Get statistics
 * double avgTime = monitor.getAverageRenderTime("icon_render");
 * double fps = monitor.getFrameRate();
 *
 * connect(&monitor, &PerformanceMonitor::performanceWarning,
 *         this, &MyWidget::handleSlowPerformance);
 * @endcode
 */
class PerformanceMonitor : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct PerformanceMonitor
     * @param parent The parent QObject
     */
    explicit PerformanceMonitor(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~PerformanceMonitor() override;

    /**
     * @brief Set the maximum number of samples to keep
     * @param maxSamples Maximum number of measurements to store (default: 100)
     */
    void setMaxSamples(int maxSamples);

    /**
     * @brief Set the target FPS for performance baseline
     * @param fps Target frames per second (default: 60)
     */
    void setTargetFPS(int fps);

    /**
     * @brief Start a performance measurement
     * @param label Label for this measurement (e.g., "icon_render")
     *
     * This method starts timing a measurement. Call endMeasurement() to complete it.
     */
    void startMeasurement(const QString &label = QString());

    /**
     * @brief End the current measurement
     * @param label Label matching the startMeasurement call
     * @return Elapsed time in milliseconds
     *
     * This method ends timing and records the measurement.
     */
    double endMeasurement(const QString &label = QString());

    /**
     * @brief Get the average render time for a given label
     * @param label The measurement label
     * @return Average time in milliseconds
     */
    [[nodiscard]] double getAverageRenderTime(const QString &label = QString()) const;

    /**
     * @brief Get the minimum render time recorded
     * @param label The measurement label
     * @return Minimum time in milliseconds
     */
    [[nodiscard]] double getMinimumRenderTime(const QString &label = QString()) const;

    /**
     * @brief Get the maximum render time recorded
     * @param label The measurement label
     * @return Maximum time in milliseconds
     */
    [[nodiscard]] double getMaximumRenderTime(const QString &label = QString()) const;

    /**
     * @brief Get the last recorded render time
     * @param label The measurement label
     * @return Last time in milliseconds
     */
    [[nodiscard]] double getLastRenderTime(const QString &label = QString()) const;

    /**
     * @brief Calculate the current frame rate
     * @return Frames per second based on recent measurements
     */
    [[nodiscard]] double getFrameRate() const;

    /**
     * @brief Get total number of measurements recorded
     * @return Number of measurements
     */
    [[nodiscard]] int getMeasurementCount() const;

    /**
     * @brief Get the number of samples in the buffer
     * @param label The measurement label
     * @return Number of samples for this label
     */
    [[nodiscard]] int getSampleCount(const QString &label = QString()) const;

    /**
     * @brief Check if performance is meeting target FPS
     * @return true if average frame rate meets target, false otherwise
     */
    [[nodiscard]] bool isPerformanceGood() const;

    /**
     * @brief Get performance status as a percentage
     * @return 100 = perfect, <100 = slow, >100 = fast
     */
    [[nodiscard]] int getPerformancePercentage() const;

    /**
     * @brief Get optional memory usage in MB
     * @return Memory usage, or -1 if not supported on this platform
     */
    [[nodiscard]] double getMemoryUsageMB() const;

    /**
     * @brief Get process memory usage (includes heap)
     * @return Memory in MB, or -1 if not available
     */
    [[nodiscard]] double getProcessMemoryMB() const;

    /**
     * @brief Reset all measurements
     */
    void reset();

    /**
     * @brief Reset measurements for a specific label
     * @param label The measurement label
     */
    void resetLabel(const QString &label);

    /**
     * @brief Enable or disable memory tracking
     * @param enabled true to enable memory tracking
     */
    void setMemoryTrackingEnabled(bool enabled);

    /**
     * @brief Get a formatted performance report
     * @return String containing performance statistics
     */
    [[nodiscard]] QString getPerformanceReport() const;

signals:
    /**
     * @brief Emitted when performance drops below target
     * @param currentFPS Current frames per second
     * @param targetFPS Target frames per second
     */
    void performanceWarning(double currentFPS, double targetFPS);

    /**
     * @brief Emitted when a measurement is completed
     * @param label The measurement label
     * @param timeMs Time taken in milliseconds
     */
    void measurementCompleted(const QString &label, double timeMs);

    /**
     * @brief Emitted when performance improves
     * @param currentFPS Current frames per second
     */
    void performanceImproved(double currentFPS);

private:
    /**
     * @brief Check and emit performance warnings if needed
     */
    void checkPerformanceStatus();

    /**
     * @brief Get the current process memory usage
     * @return Memory in bytes
     */
    [[nodiscard]] uint64_t getCurrentMemoryBytes() const;

    struct MeasurementSample {
        QString label;
        double timeMs = 0.0;
        uint64_t timestamp = 0;
    };

    // Performance tracking
    QElapsedTimer m_currentTimer;                           ///< Current measurement timer
    QList<MeasurementSample> m_samples;                     ///< Sample history buffer
    int m_maxSamples = 100;                                 ///< Maximum samples to keep
    int m_targetFPS = 60;                                   ///< Target FPS for baseline

    // Statistics
    QString m_currentLabel;                                 ///< Current measurement label
    double m_lastFPS = 60.0;                                ///< Last calculated FPS
    bool m_wasPerformanceGood = true;                       ///< Track performance state

    // Memory tracking
    bool m_memoryTrackingEnabled = false;                   ///< Whether to track memory
    double m_peakMemoryMB = 0.0;                            ///< Peak memory recorded

    // Performance thresholds
    static constexpr double PERFORMANCE_WARNING_THRESHOLD = 0.85;  ///< 85% of target FPS
};

}  // namespace gallery

#endif  // PERFORMANCE_MONITOR_H
