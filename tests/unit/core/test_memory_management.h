/**
 * QtLucide Memory Management Tests - Header
 * Tests for memory leaks, resource cleanup, and large-scale operations
 */

#ifndef TEST_MEMORY_MANAGEMENT_H
#define TEST_MEMORY_MANAGEMENT_H

#include <QObject>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
}

class TestMemoryManagement : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Basic memory management tests
    void testBasicResourceCleanup();
    void testMultipleInstanceCleanup();
    void testIconLifetimeAfterLucideDestruction();
    void testCustomPainterCleanup();

    // Large-scale memory tests
    void testMassiveIconCreation();
    void testMassiveIconCreationWithOptions();
    void testRepeatedIconCreation();
    void testLargeIconSizes();

    // Cache management tests
    void testIconCacheGrowth();
    void testCacheMemoryUsage();
    void testCacheEviction();
    void testCacheConsistency();

    // Resource leak tests
    void testResourceLeakPrevention();
    void testSvgDataMemoryUsage();
    void testPixmapMemoryUsage();
    void testOptionMapMemoryUsage();

    // Stress tests
    void testMemoryStressTest();
    void testLongRunningMemoryTest();
    void testConcurrentMemoryUsage();
    void testMemoryFragmentation();

    // Custom painter memory tests
    void testCustomPainterMemoryManagement();
    void testCustomPainterReplacement();
    void testCustomPainterLifetime();

    // Edge case memory tests
    void testMemoryWithInvalidOperations();
    void testMemoryWithExtremeParameters();
    void testMemoryRecoveryAfterErrors();

private:
    lucide::QtLucide* m_lucide;

    // Helper methods
    void createManyIcons(int count, bool withOptions = false);
    void measureMemoryUsage(const QString& testName, std::function<void()> testFunction);
    size_t getCurrentMemoryUsage();
};

#endif // TEST_MEMORY_MANAGEMENT_H
