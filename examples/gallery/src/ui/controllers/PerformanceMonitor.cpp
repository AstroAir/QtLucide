/**
 * @file PerformanceMonitor.cpp
 * @brief Implementation of PerformanceMonitor
 * @details Performance measurement and tracking implementation.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "PerformanceMonitor.h"
#include <QCoreApplication>
#include <QProcess>
#include <algorithm>
#include <numeric>
#include <cmath>

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <psapi.h>
#elif defined(__unix__) || defined(__APPLE__)
    #include <unistd.h>
    #include <fstream>
    #include <sstream>
#endif

namespace gallery {

PerformanceMonitor::PerformanceMonitor(QObject *parent)
    : QObject(parent)
    , m_maxSamples(100)
    , m_targetFPS(60)
    , m_lastFPS(60.0)
    , m_wasPerformanceGood(true)
    , m_memoryTrackingEnabled(false)
    , m_peakMemoryMB(0.0)
{
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::setMaxSamples(int maxSamples)
{
    m_maxSamples = std::max(1, maxSamples);

    // Trim existing samples if necessary
    while (m_samples.size() > m_maxSamples) {
        m_samples.pop_front();
    }
}

void PerformanceMonitor::setTargetFPS(int fps)
{
    m_targetFPS = std::max(1, fps);
}

void PerformanceMonitor::startMeasurement(const QString &label)
{
    m_currentLabel = label.isEmpty() ? QStringLiteral("default") : label;
    m_currentTimer.start();
}

double PerformanceMonitor::endMeasurement(const QString &label)
{
    if (!m_currentTimer.isValid()) {
        return 0.0;
    }

    double elapsed = m_currentTimer.elapsed();
    QString measurementLabel = label.isEmpty() ? m_currentLabel : label;

    // Record sample
    MeasurementSample sample;
    sample.label = measurementLabel;
    sample.timeMs = elapsed;
    sample.timestamp = m_currentTimer.elapsed();

    m_samples.append(sample);

    // Trim old samples if exceeding max
    while (m_samples.size() > m_maxSamples) {
        m_samples.pop_front();
    }

    emit measurementCompleted(measurementLabel, elapsed);

    // Check performance
    checkPerformanceStatus();

    return elapsed;
}

double PerformanceMonitor::getAverageRenderTime(const QString &label) const
{
    QString targetLabel = label.isEmpty() ? QStringLiteral("default") : label;

    QVector<double> relevantSamples;
    for (const auto &sample : m_samples) {
        if (sample.label == targetLabel) {
            relevantSamples.append(sample.timeMs);
        }
    }

    if (relevantSamples.isEmpty()) {
        return 0.0;
    }

    double sum = std::accumulate(relevantSamples.begin(), relevantSamples.end(), 0.0);
    return sum / relevantSamples.size();
}

double PerformanceMonitor::getMinimumRenderTime(const QString &label) const
{
    QString targetLabel = label.isEmpty() ? QStringLiteral("default") : label;

    double minTime = std::numeric_limits<double>::max();
    bool found = false;

    for (const auto &sample : m_samples) {
        if (sample.label == targetLabel) {
            minTime = std::min(minTime, sample.timeMs);
            found = true;
        }
    }

    return found ? minTime : 0.0;
}

double PerformanceMonitor::getMaximumRenderTime(const QString &label) const
{
    QString targetLabel = label.isEmpty() ? QStringLiteral("default") : label;

    double maxTime = 0.0;
    for (const auto &sample : m_samples) {
        if (sample.label == targetLabel) {
            maxTime = std::max(maxTime, sample.timeMs);
        }
    }

    return maxTime;
}

double PerformanceMonitor::getLastRenderTime(const QString &label) const
{
    QString targetLabel = label.isEmpty() ? QStringLiteral("default") : label;

    for (auto it = m_samples.rbegin(); it != m_samples.rend(); ++it) {
        if (it->label == targetLabel) {
            return it->timeMs;
        }
    }

    return 0.0;
}

double PerformanceMonitor::getFrameRate() const
{
    if (m_samples.isEmpty()) {
        return 0.0;
    }

    // Calculate average frame time and convert to FPS
    double avgTime = 0.0;
    int count = 0;

    for (const auto &sample : m_samples) {
        avgTime += sample.timeMs;
        count++;
    }

    if (count == 0 || avgTime == 0.0) {
        return 0.0;
    }

    avgTime /= count;
    return 1000.0 / avgTime;  // Convert ms to FPS
}

int PerformanceMonitor::getMeasurementCount() const
{
    return m_samples.size();
}

int PerformanceMonitor::getSampleCount(const QString &label) const
{
    QString targetLabel = label.isEmpty() ? QStringLiteral("default") : label;

    int count = 0;
    for (const auto &sample : m_samples) {
        if (sample.label == targetLabel) {
            count++;
        }
    }

    return count;
}

bool PerformanceMonitor::isPerformanceGood() const
{
    double fps = getFrameRate();
    double threshold = m_targetFPS * PERFORMANCE_WARNING_THRESHOLD;
    return fps >= threshold;
}

int PerformanceMonitor::getPerformancePercentage() const
{
    double fps = getFrameRate();
    if (m_targetFPS <= 0) {
        return 100;
    }

    return static_cast<int>((fps / m_targetFPS) * 100.0);
}

double PerformanceMonitor::getMemoryUsageMB() const
{
    uint64_t bytes = getCurrentMemoryBytes();
    return bytes / (1024.0 * 1024.0);
}

double PerformanceMonitor::getProcessMemoryMB() const
{
    return getMemoryUsageMB();
}

void PerformanceMonitor::reset()
{
    m_samples.clear();
    m_lastFPS = 60.0;
    m_wasPerformanceGood = true;
}

void PerformanceMonitor::resetLabel(const QString &label)
{
    QString targetLabel = label.isEmpty() ? QStringLiteral("default") : label;

    auto it = m_samples.begin();
    while (it != m_samples.end()) {
        if (it->label == targetLabel) {
            it = m_samples.erase(it);
        } else {
            ++it;
        }
    }
}

void PerformanceMonitor::setMemoryTrackingEnabled(bool enabled)
{
    m_memoryTrackingEnabled = enabled;
}

QString PerformanceMonitor::getPerformanceReport() const
{
    QString report;
    report += QStringLiteral("=== Performance Report ===\n");
    report += QStringLiteral("Frame Rate: %1 FPS (Target: %2)\n").arg(getFrameRate(), 0, 'f', 2).arg(m_targetFPS);
    report += QStringLiteral("Performance: %1%\n").arg(getPerformancePercentage());
    report += QStringLiteral("Measurements: %1\n").arg(getMeasurementCount());
    report += QStringLiteral("Avg Frame Time: %1 ms\n").arg(getAverageRenderTime(), 0, 'f', 2);
    report += QStringLiteral("Min/Max Frame Time: %1 / %2 ms\n")
              .arg(getMinimumRenderTime(), 0, 'f', 2)
              .arg(getMaximumRenderTime(), 0, 'f', 2);

    if (m_memoryTrackingEnabled) {
        report += QStringLiteral("Memory Usage: %1 MB\n").arg(getMemoryUsageMB(), 0, 'f', 2);
    }

    return report;
}

void PerformanceMonitor::checkPerformanceStatus()
{
    double currentFPS = getFrameRate();
    m_lastFPS = currentFPS;

    bool isGood = isPerformanceGood();

    if (!isGood && m_wasPerformanceGood) {
        // Performance dropped
        emit performanceWarning(currentFPS, m_targetFPS);
        m_wasPerformanceGood = false;
    } else if (isGood && !m_wasPerformanceGood) {
        // Performance improved
        emit performanceImproved(currentFPS);
        m_wasPerformanceGood = true;
    }
}

uint64_t PerformanceMonitor::getCurrentMemoryBytes() const
{
#ifdef _WIN32
    HANDLE process = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;

    if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#elif defined(__unix__) || defined(__APPLE__)
    // Read from /proc/self/status on Linux
    std::ifstream stat("/proc/self/status");
    if (stat.is_open()) {
        std::string line;
        while (std::getline(stat, line)) {
            if (line.find("VmRSS:") != std::string::npos) {
                std::istringstream iss(line);
                std::string key;
                uint64_t value;
                iss >> key >> value;
                return value * 1024;  // VmRSS is in KB
            }
        }
    }
#endif

    return 0;  // Not supported on this platform
}

}  // namespace gallery
