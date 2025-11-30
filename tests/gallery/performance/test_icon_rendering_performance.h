/**
 * @file test_icon_rendering_performance.h
 * @brief Icon Rendering Performance Tests
 * @details Comprehensive performance tests for icon rendering including:
 *          - Initialization performance (QtLucide, ContentManager, metadata)
 *          - Single icon rendering performance
 *          - Batch icon rendering performance
 *          - Caching performance
 *          - Memory stress tests
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef TEST_ICON_RENDERING_PERFORMANCE_H
#define TEST_ICON_RENDERING_PERFORMANCE_H

#include <QObject>
#include <QtTest/QtTest>
#include <memory>

// Forward declarations
namespace lucide {
class QtLucide;
}

namespace gallery {
class ContentManager;
class IconMetadataManager;
} // namespace gallery

/**
 * @class TestIconRenderingPerformance
 * @brief Comprehensive performance test suite for icon rendering
 * @details Tests cover:
 *          1. Initialization performance
 *             - QtLucide initialization (<100ms)
 *             - ContentManager initialization (<500ms)
 *             - Metadata loading (<200ms)
 *          2. Single icon rendering
 *             - Icon creation (<5ms)
 *             - Pixmap rendering (<10ms)
 *             - Various sizes (16px, 48px, 128px, 256px)
 *          3. Batch rendering
 *             - 100 icons (<500ms)
 *             - 1000 icons (<3000ms)
 *             - All 1634+ icons (<5000ms)
 *          4. Caching performance
 *             - First load vs cached load
 *             - Cache hit performance improvement (>=50%)
 *          5. Memory stress
 *             - Large icon loads with memory stability
 *             - Cache cleanup functionality
 */
class TestIconRenderingPerformance : public QObject {
    Q_OBJECT

private slots:
    // Test lifecycle methods
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Initialization performance tests
    void testQtLucideInitializationPerformance();
    void testContentManagerInitializationPerformance();
    void testMetadataLoadingPerformance();

    // Single icon rendering tests
    void testSingleIconCreationPerformance();
    void testSingleIconPixmapRenderingPerformance();
    void testIconRenderingDifferentSizes();
    void testIconRenderingDifferentSizes_data();

    // Batch rendering tests
    void testBatch100IconsRenderingPerformance();
    void testBatch1000IconsRenderingPerformance();
    void testBatchAllIconsRenderingPerformance();

    // Caching performance tests
    void testCacheFirstLoadVsCachedLoad();
    void testCachePerformanceImprovement();

    // Memory stress tests
    void testMemoryStabilityLargeLoad();
    void testCacheCleanupFunctionality();

private:
    // Helper methods
    QString getTestDataPath() const;
    QString getIconName(int index) const;
    QStringList getIconNames(int count) const;

    // Member variables
    QString m_testDataDir;
    std::unique_ptr<lucide::QtLucide> m_lucide;
    std::unique_ptr<gallery::ContentManager> m_contentManager;

    // Performance threshold constants
    static constexpr qint64 QTLUCIDE_INIT_MAX_TIME = 100;        // ms
    static constexpr qint64 CONTENT_MANAGER_INIT_MAX_TIME = 500; // ms
    static constexpr qint64 METADATA_LOAD_MAX_TIME = 200;        // ms
    static constexpr qint64 SINGLE_ICON_CREATE_MAX_TIME = 5;     // ms
    static constexpr qint64 SINGLE_ICON_RENDER_MAX_TIME = 10;    // ms
    static constexpr qint64 BATCH_100_MAX_TIME = 500;            // ms
    static constexpr qint64 BATCH_1000_MAX_TIME = 3000;          // ms
    static constexpr qint64 BATCH_ALL_MAX_TIME = 5000;           // ms
    static constexpr double CACHE_IMPROVEMENT_MIN_RATIO = 0.5;   // 50% improvement
};

#endif // TEST_ICON_RENDERING_PERFORMANCE_H
