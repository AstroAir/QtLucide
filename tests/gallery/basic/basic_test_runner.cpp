/**
 * @file basic_test_runner.cpp
 * @brief Basic Test Runner for QtLucide Gallery
 * @details Main test runner for basic functionality tests including core library integration,
 *          library availability, and resource loading tests
 * @author QtLucide Test Suite
 * @date 2025
 */

#include <QApplication>
#include <QDebug>
#include <QtTest/QtTest>

// Include test classes
#include "test_core_integration.h"
#include "test_library_availability.h"
#include "test_resource_loading.h"

/**
 * @brief Main function for basic gallery tests
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code (0 for success, non-zero for failure)
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties for testing
    app.setApplicationName("QtLucide Gallery Basic Tests");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide");
    app.setOrganizationDomain("qtlucide.org");

    qDebug() << "=== QtLucide Gallery Basic Tests ===";
    qDebug() << "Starting basic functionality test suite...";

    int totalFailures = 0;
    int totalTests = 0;

    // Test 1: Core Integration Tests
    {
        qDebug() << "\n--- Running Core Integration Tests ---";
        TestCoreIntegration coreIntegrationTest;
        int result = QTest::qExec(&coreIntegrationTest, argc, argv);
        totalFailures += result;
        totalTests++;
        qDebug() << "Core Integration Tests result:" << (result == 0 ? "PASSED" : "FAILED");
    }

    // Test 2: Library Availability Tests
    {
        qDebug() << "\n--- Running Library Availability Tests ---";
        TestLibraryAvailability libraryAvailabilityTest;
        int result = QTest::qExec(&libraryAvailabilityTest, argc, argv);
        totalFailures += result;
        totalTests++;
        qDebug() << "Library Availability Tests result:" << (result == 0 ? "PASSED" : "FAILED");
    }

    // Test 3: Resource Loading Tests
    {
        qDebug() << "\n--- Running Resource Loading Tests ---";
        TestResourceLoading resourceLoadingTest;
        int result = QTest::qExec(&resourceLoadingTest, argc, argv);
        totalFailures += result;
        totalTests++;
        qDebug() << "Resource Loading Tests result:" << (result == 0 ? "PASSED" : "FAILED");
    }

    // Print summary
    qDebug() << "\n=== Basic Tests Summary ===";
    qDebug() << "Total test suites run:" << totalTests;
    qDebug() << "Test suites passed:" << (totalTests - totalFailures);
    qDebug() << "Test suites failed:" << totalFailures;

    if (totalFailures == 0) {
        qDebug() << "🎉 ALL BASIC TESTS PASSED! 🎉";
        qDebug() << "Basic gallery functionality is working correctly.";
    } else {
        qDebug() << "❌ Some basic tests failed.";
        qDebug() << "Please review the test output above for details.";
    }

    qDebug() << "=== End of Basic Tests ===\n";

    return totalFailures;
}
