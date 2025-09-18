/**
 * QtLucide Performance Regression Tests - Header
 *
 * Comprehensive benchmarking tests to prevent performance degradation
 * across icon rendering, memory usage, search operations, and application lifecycle.
 */

#ifndef TEST_PERFORMANCE_REGRESSION_H
#define TEST_PERFORMANCE_REGRESSION_H

#include <QObject>
#include <QtTest/QtTest>
#include <QElapsedTimer>
#include <QTimer>
#include <QEventLoop>
#include <QApplication>
#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QProcess>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

namespace lucide {
class QtLucide;
}

class IconMetadataManager;
class SearchWidget;
class IconGridWidget;

/**
 * @brief Performance metrics collection structure
 */
struct PerformanceMetrics {
    qint64 averageTime = 0;
    qint64 minTime = LLONG_MAX;
    qint64 maxTime = 0;
    qint64 totalTime = 0;
    int iterations = 0;
    double standardDeviation = 0.0;

    void addMeasurement(qint64 time) {
        totalTime += time;
        iterations++;
        if (time < minTime) minTime = time;
        if (time > maxTime) maxTime = time;
        averageTime = totalTime / iterations;
    }

    void calculateStandardDeviation(const QList<qint64>& measurements) {
        if (measurements.size() < 2) return;

        double sum = 0.0;
        for (qint64 time : measurements) {
            double diff = static_cast<double>(time) - static_cast<double>(averageTime);
            sum += diff * diff;
        }
        standardDeviation = sqrt(sum / (static_cast<double>(measurements.size()) - 1));
    }
};

/**
 * @brief Memory usage tracking structure
 */
struct MemoryMetrics {
    qint64 initialMemory = 0;
    qint64 peakMemory = 0;
    qint64 finalMemory = 0;
    qint64 memoryGrowth = 0;
    int allocations = 0;
    int deallocations = 0;
};

class TestPerformanceRegression : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Icon rendering performance tests
    void testIconRenderingPerformance();
    void testIconRenderingUnderLoad();
    void testIconRenderingScalability();
    void testIconRenderingMemoryEfficiency();
    void testIconRenderingConcurrency();

    // Memory usage pattern tests
    void testMemoryUsageBaseline();
    void testMemoryUsageUnderLoad();
    void testMemoryLeakDetection();
    void testMemoryFragmentation();
    void testMemoryRecoveryPatterns();

    // Search and filtering performance tests
    void testSearchPerformanceBaseline();
    void testSearchPerformanceWithLargeDataset();
    void testFilteringPerformanceBaseline();
    void testCombinedSearchFilterPerformance();
    void testSearchResponseTime();

    // Application lifecycle performance tests
    void testApplicationStartupPerformance();
    void testApplicationShutdownPerformance();
    void testResourceInitializationPerformance();
    void testResourceCleanupPerformance();
    void testApplicationRecoveryPerformance();

    // UI responsiveness tests
    void testUIResponsivenessUnderLoad();
    void testScrollingPerformance();
    void testResizePerformance();
    void testAnimationPerformance();

    // Regression detection tests
    void testPerformanceRegression();
    void testMemoryRegression();
    void testResponsivenessRegression();

private:
    // Performance measurement helpers
    PerformanceMetrics measureIconRendering(int iconCount, int iterations);
    MemoryMetrics measureMemoryUsage(std::function<void()> operation);
    qint64 measureSearchTime(const QString& searchTerm, int iterations);
    qint64 measureApplicationStartup();
    qint64 measureApplicationShutdown();

    // Benchmark helpers
    void warmupSystem();
    void clearSystemCaches();
    QList<qint64> runBenchmark(std::function<qint64()> benchmark, int iterations);
    bool isPerformanceAcceptable(const PerformanceMetrics& metrics, qint64 threshold);

    // Memory tracking helpers
    qint64 getCurrentMemoryUsage();
    void trackMemoryAllocations(bool enable);

    // Test data and objects
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    SearchWidget* m_searchWidget;
    IconGridWidget* m_iconGrid;
    QWidget* m_testWidget;

    // Performance tracking
    QElapsedTimer m_timer;
    QList<PerformanceMetrics> m_performanceHistory;
    QList<MemoryMetrics> m_memoryHistory;

    // Baseline performance thresholds (in milliseconds)
    static constexpr qint64 ICON_RENDER_THRESHOLD_MS = 5;
    static constexpr qint64 SEARCH_RESPONSE_THRESHOLD_MS = 100;
    static constexpr qint64 STARTUP_THRESHOLD_MS = 2000;
    static constexpr qint64 SHUTDOWN_THRESHOLD_MS = 1000;
    static constexpr qint64 MEMORY_LEAK_THRESHOLD_KB = 1024;

    // Test configuration
    static constexpr int PERFORMANCE_ITERATIONS = 100;
    static constexpr int STRESS_TEST_ITERATIONS = 1000;
    static constexpr int WARMUP_ITERATIONS = 10;
};

#endif // TEST_PERFORMANCE_REGRESSION_H
