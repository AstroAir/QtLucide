/**
 * QtLucide Error Handling Tests - Header
 * Tests for error conditions, invalid inputs, and edge cases
 */

#ifndef TEST_ERROR_HANDLING_H
#define TEST_ERROR_HANDLING_H

#include <QObject>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
}

class TestErrorHandling : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Initialization error tests
    void testUninitializedAccess();
    void testMultipleInitialization();
    void testInitializationFailure();

    // Invalid input tests
    void testInvalidIconNames();
    void testNullParameters();
    void testEmptyParameters();
    void testInvalidEnumValues();

    // Option validation tests
    void testInvalidOptions();
    void testMalformedOptions();
    void testExtremeOptionValues();
    void testConflictingOptions();

    // Resource error tests
    void testMissingResources();
    void testCorruptedSvgData();
    void testResourceExhaustion();

    // Custom painter error tests
    void testNullCustomPainter();
    void testInvalidCustomPainter();
    void testCustomPainterExceptions();

    // Memory error tests
    void testLargeIconCreation();
    void testMemoryPressure();
    void testResourceCleanup();

private:
    lucide::QtLucide* m_lucide;
};

#endif // TEST_ERROR_HANDLING_H
