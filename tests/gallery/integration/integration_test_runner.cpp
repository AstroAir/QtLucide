/**
 * @file integration_test_runner.cpp
 * @brief Integration Test Runner for QtLucide Gallery
 * @details Main test runner that executes integration test categories
 * @author QtLucide Test Suite
 * @date 2025
 */

#include <QApplication>
#include <QDebug>
#include <QtTest/QtTest>

// Include all integration test classes
#include "test_application_startup.h"
#include "test_gallery_integration.h"

/**
 * @brief Main function for integration tests
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code (0 for success, non-zero for failure)
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties for testing
    app.setApplicationName("QtLucide Gallery Integration Tests");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide");
    app.setOrganizationDomain("qtlucide.org");

    qDebug() << "=== QtLucide Gallery Integration Test Suite ===";

    int result = 0;

    // Run integration tests
    {
        TestGalleryIntegration integrationTest;
        result += QTest::qExec(&integrationTest, argc, argv);
    }

    // Run application startup tests
    {
        TestApplicationStartup startupTest;
        result += QTest::qExec(&startupTest, argc, argv);
    }

    qDebug() << "=== End of Integration Tests ===";

    return result;
}
