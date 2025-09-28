/**
 * QtLucide Boundary Conditions Tests - Header
 * Tests for extreme values, edge cases, and performance limits
 */

#ifndef TEST_BOUNDARY_CONDITIONS_H
#define TEST_BOUNDARY_CONDITIONS_H

#include <QObject>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
}

class TestBoundaryConditions : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Size boundary tests
    void testZeroSizeIcons();
    void testNegativeSizeIcons();
    void testExtremelyLargeIcons();
    void testMaximumReasonableSize();

    // Color boundary tests
    void testInvalidColorValues();
    void testExtremeColorValues();
    void testTransparentColors();
    void testColorOverflow();

    // Scale factor boundary tests
    void testZeroScaleFactor();
    void testNegativeScaleFactor();
    void testExtremelyLargeScaleFactor();
    void testVerySmallScaleFactor();
    void testInfiniteScaleFactor();

    // Opacity boundary tests
    void testNegativeOpacity();
    void testOpacityGreaterThanOne();
    void testExtremeOpacityValues();
    void testZeroOpacity();

    // String boundary tests
    void testVeryLongIconNames();
    void testUnicodeIconNames();
    void testSpecialCharacterIconNames();
    void testBinaryDataInIconNames();

    // Performance boundary tests
    void testMassiveIconCreation();
    void testRapidIconCreation();
    void testMemoryLimitTesting();
    void testCacheOverflow();

    // Option boundary tests
    void testExtremelyLargeOptionMaps();
    void testDeeplyNestedOptions();
    void testCircularOptionReferences();
    void testOptionKeyBoundaries();

    // Enum boundary tests
    void testEnumBoundaryValues();
    void testNegativeEnumValues();
    void testLargeEnumValues();
    void testEnumOverflow();

    // Painter boundary tests
    void testPainterWithExtremeParameters();
    void testPainterMemoryLimits();
    void testPainterPerformanceLimits();

private:
    lucide::QtLucide* m_lucide;

    void measurePerformance(const QString& testName, std::function<void()> testFunction);
};

#endif // TEST_BOUNDARY_CONDITIONS_H
