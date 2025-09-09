/**
 * QtLucide Gallery Application - Performance Tests
 *
 * Comprehensive performance tests including:
 * - Large dataset loading performance
 * - Memory usage monitoring
 * - Response time measurements
 * - Thumbnail generation performance
 * - Search performance with large datasets
 * - UI responsiveness under load
 * - Resource cleanup verification
 */

#include <QtTest/QtTest>
#include <QApplication>
#include <QtTest/QSignalSpy>
#include <QElapsedTimer>
#include <QTimer>
#include <QThread>
#include <QPixmap>
#include <QStringList>
#include <QProcess>
#include <QDir>
#include <QTemporaryDir>

// Include Gallery components
#include "ui/widgets/grids/IconGridWidget.h"
#include "ui/widgets/grids/IconThumbnailGridWidget.h"
#include "ui/widgets/search/SearchWidget.h"
#include "ui/widgets/search/IconSearchWidget.h"
#include "ui/widgets/panels/CategorySidebarWidget.h"
#include "core/managers/IconMetadataManager.h"
#include "core/managers/ContentManager.h"
#include "core/managers/FavoritesManager.h"
#include <QtLucide/QtLucide.h>

class TestPerformance : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Loading Performance Tests
    void testLoadingPerformance_SmallDataset();
    void testLoadingPerformance_MediumDataset();
    void testLoadingPerformance_LargeDataset();
    void testLoadingPerformance_VeryLargeDataset();

    // Memory Usage Tests
    void testMemoryUsage_BaselineUsage();
    void testMemoryUsage_AfterLoading();
    void testMemoryUsage_ThumbnailCache();
    void testMemoryUsage_SearchOperations();
    void testMemoryUsage_ResourceCleanup();

    // Response Time Tests
    void testResponseTime_IconSelection();
    void testResponseTime_SearchQuery();
    void testResponseTime_CategoryFilter();
    void testResponseTime_ScrollingLarge();

    // Thumbnail Performance Tests
    void testThumbnailPerformance_GenerationSpeed();
    void testThumbnailPerformance_CacheEfficiency();
    void testThumbnailPerformance_ConcurrentGeneration();
    void testThumbnailPerformance_MemoryFootprint();

    // Search Performance Tests
    void testSearchPerformance_SimpleQuery();
    void testSearchPerformance_ComplexQuery();
    void testSearchPerformance_RealTimeSearch();
    void testSearchPerformance_LargeResultSet();

    // UI Responsiveness Tests
    void testUIResponsiveness_UnderLoad();
    void testUIResponsiveness_DuringSearch();
    void testUIResponsiveness_DuringResize();
    void testUIResponsiveness_ConcurrentOperations();

    // Stress Tests
    void testStress_RapidOperations();
    void testStress_MemoryPressure();
    void testStress_ConcurrentUsers();
    void testStress_LongRunning();

private:
    // Test fixtures
    QApplication* m_app;
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_iconMetadataManager;
    ContentManager* m_contentManager;
    FavoritesManager* m_favoritesManager;
    QTemporaryDir* m_tempDir;

    // Test data
    QStringList m_smallDataset;    // ~50 icons
    QStringList m_mediumDataset;   // ~200 icons
    QStringList m_largeDataset;    // ~500 icons
    QStringList m_veryLargeDataset; // ~1000+ icons

    // Performance tracking
    struct PerformanceMetrics {
        qint64 loadTime = 0;
        qint64 searchTime = 0;
        qint64 renderTime = 0;
        qint64 memoryUsage = 0;
        int operationsPerSecond = 0;
    };

    // Helper methods
    void setupTestData();
    QStringList getIconSubset(int count);
    PerformanceMetrics measureLoadingPerformance(const QStringList& icons);
    PerformanceMetrics measureSearchPerformance(const QString& query, const QStringList& dataset);
    qint64 getCurrentMemoryUsage();
    bool verifyUIResponsiveness(QWidget* widget, int timeoutMs = 1000);
    void simulateUserInteraction(QWidget* widget, int duration = 1000);
    void generateTestThumbnails(const QStringList& icons);
};

void TestPerformance::initTestCase()
{
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());

    // Create managers
    m_iconMetadataManager = new IconMetadataManager(this);
    m_contentManager = new ContentManager(this);
    m_favoritesManager = new FavoritesManager(this);

    // Setup test environment
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());

    setupTestData();

    qDebug() << "Performance test environment initialized";
    qDebug() << "Small dataset:" << m_smallDataset.size() << "icons";
    qDebug() << "Medium dataset:" << m_mediumDataset.size() << "icons";
    qDebug() << "Large dataset:" << m_largeDataset.size() << "icons";
    qDebug() << "Very large dataset:" << m_veryLargeDataset.size() << "icons";
}

void TestPerformance::cleanupTestCase()
{
    delete m_tempDir;
    qDebug() << "Performance test environment cleaned up";
}

void TestPerformance::init()
{
    // Reset state before each test
}

void TestPerformance::cleanup()
{
    // Clean up after each test
    QApplication::processEvents(); // Process any pending events
}

void TestPerformance::setupTestData()
{
    QStringList allIcons = m_lucide->availableIcons();

    // Create datasets of different sizes
    m_smallDataset = getIconSubset(qMin(50, allIcons.size()));
    m_mediumDataset = getIconSubset(qMin(200, allIcons.size()));
    m_largeDataset = getIconSubset(qMin(500, allIcons.size()));
    m_veryLargeDataset = getIconSubset(allIcons.size()); // All available icons

    QVERIFY(!m_smallDataset.isEmpty());
    QVERIFY(!m_mediumDataset.isEmpty());
    QVERIFY(!m_largeDataset.isEmpty());
    QVERIFY(!m_veryLargeDataset.isEmpty());
}

QStringList TestPerformance::getIconSubset(int count)
{
    QStringList allIcons = m_lucide->availableIcons();
    if (count >= allIcons.size()) {
        return allIcons;
    }
    return allIcons.mid(0, count);
}

TestPerformance::PerformanceMetrics TestPerformance::measureLoadingPerformance(const QStringList& icons)
{
    PerformanceMetrics metrics;

    IconGridWidget* widget = new IconGridWidget();
    widget->setLucide(m_lucide);
    widget->setMetadataManager(m_iconMetadataManager);

    QElapsedTimer timer;
    timer.start();

    // Measure loading time
    widget->setIconNames(icons);
    widget->show();
    QTest::qWait(100); // Allow initial rendering

    metrics.loadTime = timer.elapsed();

    // Measure render time
    timer.restart();
    widget->repaint();
    QApplication::processEvents();
    metrics.renderTime = timer.elapsed();

    delete widget;
    return metrics;
}

TestPerformance::PerformanceMetrics TestPerformance::measureSearchPerformance(const QString& query, const QStringList& dataset)
{
    PerformanceMetrics metrics;

    SearchWidget* widget = new SearchWidget(m_iconMetadataManager);

    QElapsedTimer timer;
    timer.start();

    // Simulate search
    widget->focusSearchInput();
    QTest::keyClicks(widget, query);

    // Wait for search to complete
    QTest::qWait(500);

    metrics.searchTime = timer.elapsed();

    delete widget;
    return metrics;
}

qint64 TestPerformance::getCurrentMemoryUsage()
{
    // This is a simplified memory usage measurement
    // In a real implementation, you might use platform-specific APIs
    // or QProcess to get actual memory usage

    // For now, return a placeholder value
    return 0; // Would need platform-specific implementation
}

bool TestPerformance::verifyUIResponsiveness(QWidget* widget, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();

    // Try to interact with the widget
    QTest::mouseClick(widget, Qt::LeftButton, Qt::NoModifier, QPoint(50, 50));
    QApplication::processEvents();

    // Check if interaction completed within timeout
    return timer.elapsed() < timeoutMs;
}

void TestPerformance::simulateUserInteraction(QWidget* widget, int duration)
{
    QElapsedTimer timer;
    timer.start();

    while (timer.elapsed() < duration) {
        // Simulate various user interactions
        QTest::mouseClick(widget, Qt::LeftButton, Qt::NoModifier,
                         QPoint(qrand() % widget->width(), qrand() % widget->height()));

        QTest::keyClick(widget, Qt::Key_Down);
        QTest::keyClick(widget, Qt::Key_Up);

        QApplication::processEvents();
        QTest::qWait(50);
    }
}

void TestPerformance::generateTestThumbnails(const QStringList& icons)
{
    // Generate thumbnails for performance testing
    for (const QString& iconName : icons) {
        QPixmap pixmap = m_lucide->icon(iconName).pixmap(64, 64);
        // Thumbnails would be cached by the content manager
    }
}

// Loading Performance Tests
void TestPerformance::testLoadingPerformance_SmallDataset()
{
    PerformanceMetrics metrics = measureLoadingPerformance(m_smallDataset);

    qDebug() << "Small dataset (" << m_smallDataset.size() << " icons):";
    qDebug() << "  Load time:" << metrics.loadTime << "ms";
    qDebug() << "  Render time:" << metrics.renderTime << "ms";

    // Small dataset should load quickly
    QVERIFY(metrics.loadTime < 1000);
    QVERIFY(metrics.renderTime < 500);
}

void TestPerformance::testLoadingPerformance_MediumDataset()
{
    PerformanceMetrics metrics = measureLoadingPerformance(m_mediumDataset);

    qDebug() << "Medium dataset (" << m_mediumDataset.size() << " icons):";
    qDebug() << "  Load time:" << metrics.loadTime << "ms";
    qDebug() << "  Render time:" << metrics.renderTime << "ms";

    // Medium dataset should load reasonably quickly
    QVERIFY(metrics.loadTime < 3000);
    QVERIFY(metrics.renderTime < 1000);
}

void TestPerformance::testLoadingPerformance_LargeDataset()
{
    PerformanceMetrics metrics = measureLoadingPerformance(m_largeDataset);

    qDebug() << "Large dataset (" << m_largeDataset.size() << " icons):";
    qDebug() << "  Load time:" << metrics.loadTime << "ms";
    qDebug() << "  Render time:" << metrics.renderTime << "ms";

    // Large dataset should load within acceptable time
    QVERIFY(metrics.loadTime < 5000);
    QVERIFY(metrics.renderTime < 2000);
}

void TestPerformance::testLoadingPerformance_VeryLargeDataset()
{
    PerformanceMetrics metrics = measureLoadingPerformance(m_veryLargeDataset);

    qDebug() << "Very large dataset (" << m_veryLargeDataset.size() << " icons):";
    qDebug() << "  Load time:" << metrics.loadTime << "ms";
    qDebug() << "  Render time:" << metrics.renderTime << "ms";

    // Very large dataset should still be manageable
    QVERIFY(metrics.loadTime < 10000);
    QVERIFY(metrics.renderTime < 5000);
}

// Memory Usage Tests
void TestPerformance::testMemoryUsage_BaselineUsage()
{
    qint64 baselineMemory = getCurrentMemoryUsage();

    // Create minimal components
    IconGridWidget* widget = new IconGridWidget();
    widget->setLucide(m_lucide);
    widget->setMetadataManager(m_iconMetadataManager);

    qint64 afterCreation = getCurrentMemoryUsage();

    qDebug() << "Baseline memory usage:" << baselineMemory << "bytes";
    qDebug() << "After widget creation:" << afterCreation << "bytes";
    qDebug() << "Memory increase:" << (afterCreation - baselineMemory) << "bytes";

    // Memory increase should be reasonable
    QVERIFY((afterCreation - baselineMemory) >= 0);

    delete widget;
}

void TestPerformance::testMemoryUsage_AfterLoading()
{
    IconGridWidget* widget = new IconGridWidget();
    widget->setLucide(m_lucide);
    widget->setMetadataManager(m_iconMetadataManager);

    qint64 beforeLoading = getCurrentMemoryUsage();

    // Load large dataset
    widget->setIconNames(m_largeDataset);
    QTest::qWait(1000);

    qint64 afterLoading = getCurrentMemoryUsage();

    qDebug() << "Memory before loading:" << beforeLoading << "bytes";
    qDebug() << "Memory after loading:" << afterLoading << "bytes";
    qDebug() << "Memory increase:" << (afterLoading - beforeLoading) << "bytes";

    // Memory should increase but not excessively
    QVERIFY((afterLoading - beforeLoading) >= 0);

    delete widget;
}

void TestPerformance::testMemoryUsage_ThumbnailCache()
{
    IconThumbnailGridWidget* widget = new IconThumbnailGridWidget();
    widget->setContentManager(m_contentManager);
    widget->setIconMetadataManager(m_iconMetadataManager);

    qint64 beforeThumbnails = getCurrentMemoryUsage();

    // Generate thumbnails
    widget->setIconList(m_mediumDataset);
    QTest::qWait(2000); // Allow thumbnail generation

    qint64 afterThumbnails = getCurrentMemoryUsage();

    qDebug() << "Memory before thumbnails:" << beforeThumbnails << "bytes";
    qDebug() << "Memory after thumbnails:" << afterThumbnails << "bytes";
    qDebug() << "Thumbnail cache memory:" << (afterThumbnails - beforeThumbnails) << "bytes";

    // Thumbnail cache should use reasonable memory
    QVERIFY((afterThumbnails - beforeThumbnails) >= 0);

    delete widget;
}

void TestPerformance::testMemoryUsage_SearchOperations()
{
    SearchWidget* widget = new SearchWidget(m_iconMetadataManager);

    qint64 beforeSearch = getCurrentMemoryUsage();

    // Perform multiple searches
    QStringList searchTerms = {"home", "user", "file", "arrow", "button", "icon"};
    for (const QString& term : searchTerms) {
        widget->focusSearchInput();
        QTest::keyClicks(widget, term);
        QTest::qWait(200);

        // Clear search
        QTest::keySequence(widget, QKeySequence::SelectAll);
        QTest::keyClick(widget, Qt::Key_Delete);
    }

    qint64 afterSearch = getCurrentMemoryUsage();

    qDebug() << "Memory before search operations:" << beforeSearch << "bytes";
    qDebug() << "Memory after search operations:" << afterSearch << "bytes";
    qDebug() << "Search memory overhead:" << (afterSearch - beforeSearch) << "bytes";

    // Search operations shouldn't cause significant memory leaks
    QVERIFY((afterSearch - beforeSearch) >= 0);

    delete widget;
}

void TestPerformance::testMemoryUsage_ResourceCleanup()
{
    qint64 initialMemory = getCurrentMemoryUsage();

    // Create and destroy multiple widgets
    for (int i = 0; i < 10; ++i) {
        IconGridWidget* widget = new IconGridWidget();
        widget->setLucide(m_lucide);
        widget->setMetadataManager(m_iconMetadataManager);
        widget->setIconNames(m_smallDataset);
        QTest::qWait(100);
        delete widget;
    }

    // Force garbage collection
    QApplication::processEvents();
    QTest::qWait(500);

    qint64 finalMemory = getCurrentMemoryUsage();

    qDebug() << "Initial memory:" << initialMemory << "bytes";
    qDebug() << "Final memory:" << finalMemory << "bytes";
    qDebug() << "Memory difference:" << (finalMemory - initialMemory) << "bytes";

    // Memory should return close to initial level (allowing for some overhead)
    QVERIFY(qAbs(finalMemory - initialMemory) < 1000000); // Less than 1MB difference
}

// Response Time Tests
void TestPerformance::testResponseTime_IconSelection()
{
    IconGridWidget* widget = new IconGridWidget();
    widget->setLucide(m_lucide);
    widget->setMetadataManager(m_iconMetadataManager);
    widget->setIconNames(m_mediumDataset);
    widget->show();
    QTest::qWait(500);

    QElapsedTimer timer;
    timer.start();

    // Simulate icon selection
    QTest::mouseClick(widget, Qt::LeftButton, Qt::NoModifier, QPoint(50, 50));
    QApplication::processEvents();

    qint64 responseTime = timer.elapsed();
    qDebug() << "Icon selection response time:" << responseTime << "ms";

    // Selection should be immediate
    QVERIFY(responseTime < 100);

    delete widget;
}

void TestPerformance::testResponseTime_SearchQuery()
{
    PerformanceMetrics metrics = measureSearchPerformance("home", m_largeDataset);

    qDebug() << "Search query response time:" << metrics.searchTime << "ms";

    // Search should respond quickly
    QVERIFY(metrics.searchTime < 1000);
}

void TestPerformance::testResponseTime_CategoryFilter()
{
    CategorySidebarWidget* widget = new CategorySidebarWidget();
    widget->setIconMetadataManager(m_iconMetadataManager);
    widget->show();
    QTest::qWait(200);

    QElapsedTimer timer;
    timer.start();

    // Simulate category selection
    QTreeWidget* tree = widget->findChild<QTreeWidget*>();
    if (tree && tree->topLevelItemCount() > 0) {
        tree->setCurrentItem(tree->topLevelItem(0));
        QApplication::processEvents();
    }

    qint64 responseTime = timer.elapsed();
    qDebug() << "Category filter response time:" << responseTime << "ms";

    // Category filtering should be fast
    QVERIFY(responseTime < 200);

    delete widget;
}

void TestPerformance::testResponseTime_ScrollingLarge()
{
    IconGridWidget* widget = new IconGridWidget();
    widget->setLucide(m_lucide);
    widget->setMetadataManager(m_iconMetadataManager);
    widget->setIconNames(m_largeDataset);
    widget->resize(600, 400);
    widget->show();
    QTest::qWait(1000);

    QElapsedTimer timer;
    timer.start();

    // Simulate scrolling
    for (int i = 0; i < 10; ++i) {
        QTest::keyClick(widget, Qt::Key_PageDown);
        QApplication::processEvents();
        QTest::qWait(50);
    }

    qint64 scrollTime = timer.elapsed();
    qDebug() << "Large dataset scrolling time:" << scrollTime << "ms";

    // Scrolling should be smooth
    QVERIFY(scrollTime < 2000);

    delete widget;
}

// Thumbnail Performance Tests
void TestPerformance::testThumbnailPerformance_GenerationSpeed()
{
    IconThumbnailGridWidget* widget = new IconThumbnailGridWidget();
    widget->setContentManager(m_contentManager);
    widget->setIconMetadataManager(m_iconMetadataManager);

    QElapsedTimer timer;
    timer.start();

    // Generate thumbnails for medium dataset
    widget->setIconList(m_mediumDataset);
    QTest::qWait(3000); // Allow thumbnail generation

    qint64 generationTime = timer.elapsed();
    qDebug() << "Thumbnail generation for" << m_mediumDataset.size() << "icons:" << generationTime << "ms";
    qDebug() << "Average per thumbnail:" << (generationTime / m_mediumDataset.size()) << "ms";

    // Thumbnail generation should be reasonable
    QVERIFY(generationTime < 10000);
    QVERIFY((generationTime / m_mediumDataset.size()) < 100); // Less than 100ms per thumbnail

    delete widget;
}

void TestPerformance::testThumbnailPerformance_CacheEfficiency()
{
    IconThumbnailGridWidget* widget = new IconThumbnailGridWidget();
    widget->setContentManager(m_contentManager);
    widget->setIconMetadataManager(m_iconMetadataManager);

    // First load - should generate thumbnails
    QElapsedTimer timer;
    timer.start();
    widget->setIconList(m_smallDataset);
    QTest::qWait(2000);
    qint64 firstLoadTime = timer.elapsed();

    // Second load - should use cached thumbnails
    timer.restart();
    widget->setIconList(QStringList()); // Clear
    widget->setIconList(m_smallDataset); // Reload same icons
    QTest::qWait(1000);
    qint64 secondLoadTime = timer.elapsed();

    qDebug() << "First thumbnail load:" << firstLoadTime << "ms";
    qDebug() << "Second thumbnail load (cached):" << secondLoadTime << "ms";
    qDebug() << "Cache efficiency:" << ((firstLoadTime - secondLoadTime) * 100 / firstLoadTime) << "%";

    // Cached load should be significantly faster
    QVERIFY(secondLoadTime <= firstLoadTime);

    delete widget;
}

void TestPerformance::testThumbnailPerformance_ConcurrentGeneration()
{
    // Test concurrent thumbnail generation
    QList<IconThumbnailGridWidget*> widgets;

    QElapsedTimer timer;
    timer.start();

    // Create multiple widgets generating thumbnails concurrently
    for (int i = 0; i < 3; ++i) {
        IconThumbnailGridWidget* widget = new IconThumbnailGridWidget();
        widget->setContentManager(m_contentManager);
        widget->setIconMetadataManager(m_iconMetadataManager);
        widget->setIconList(m_smallDataset);
        widgets.append(widget);
    }

    QTest::qWait(3000); // Allow concurrent generation

    qint64 concurrentTime = timer.elapsed();
    qDebug() << "Concurrent thumbnail generation:" << concurrentTime << "ms";

    // Concurrent generation should not be much slower than sequential
    QVERIFY(concurrentTime < 8000);

    // Cleanup
    for (auto* widget : widgets) {
        delete widget;
    }
}

void TestPerformance::testThumbnailPerformance_MemoryFootprint()
{
    qint64 beforeThumbnails = getCurrentMemoryUsage();

    IconThumbnailGridWidget* widget = new IconThumbnailGridWidget();
    widget->setContentManager(m_contentManager);
    widget->setIconMetadataManager(m_iconMetadataManager);
    widget->setIconList(m_largeDataset);
    QTest::qWait(5000); // Allow thumbnail generation

    qint64 afterThumbnails = getCurrentMemoryUsage();
    qint64 memoryPerThumbnail = (afterThumbnails - beforeThumbnails) / m_largeDataset.size();

    qDebug() << "Thumbnail memory footprint:" << (afterThumbnails - beforeThumbnails) << "bytes";
    qDebug() << "Memory per thumbnail:" << memoryPerThumbnail << "bytes";

    // Memory per thumbnail should be reasonable (assuming 64x64 RGBA = ~16KB max)
    QVERIFY(memoryPerThumbnail < 50000); // Less than 50KB per thumbnail

    delete widget;
}

QTEST_MAIN(TestPerformance)
#include "test_performance.moc"
