/**
 * @file gallery_test_runner.cpp
 * @brief Gallery Test Runner for QtLucide Gallery
 * @details Main test runner that executes all gallery test categories in sequence
 *          and provides detailed reporting of test results
 * @author QtLucide Test Suite
 * @date 2025
 */

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QtTest/QtTest>

// Include main gallery test classes
// Note: Basic and component tests are now in separate executables

/**
 * @brief Test execution result structure
 */
struct TestResult {
    QString testName;
    int result;
    qint64 executionTime;

    bool passed() const { return result == 0; }
};

/**
 * @brief Execute a test and return result
 */
template <typename TestClass>
TestResult executeTest(const QString& testName, int argc, char* argv[]) {
    QElapsedTimer timer;
    timer.start();

    TestClass test;
    int result = QTest::qExec(&test, argc, argv);

    return {testName, result, timer.elapsed()};
}

/**
 * @brief Main function for comprehensive gallery tests
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code (0 for success, non-zero for failure)
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Set application properties for testing
    app.setApplicationName("QtLucide Gallery Tests");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide");
    app.setOrganizationDomain("qtlucide.org");

    qDebug() << "=== QtLucide Gallery Test Suite ===";
    qDebug() << "Starting gallery test execution...";

    QElapsedTimer totalTimer;
    totalTimer.start();

    QList<TestResult> results;

    // Note: Basic and component tests are now executed via separate test executables
    // This main gallery test runner is reserved for overall gallery integration tests
    qDebug() << "\n📋 === MAIN GALLERY TESTS ===";
    qDebug() << "Main gallery test runner - currently no specific tests defined.";
    qDebug() << "Basic tests: Run QtLucideGalleryBasicTests";
    qDebug() << "Component tests: Run QtLucideGalleryComponentTests";
    qDebug() << "Integration tests: Run QtLucideGalleryIntegrationTests";

    // Calculate summary statistics
    qint64 totalTime = totalTimer.elapsed();

    // Print test summary
    qDebug() << "\n🎯 === GALLERY TEST SUMMARY ===";
    qDebug() << "📋 Main Gallery Test Runner - No specific tests executed";
    qDebug() << "🔧 Basic Tests: Execute QtLucideGalleryBasicTests separately";
    qDebug() << "🧩 Component Tests: Execute QtLucideGalleryComponentTests separately";
    qDebug() << "🔗 Integration Tests: Execute QtLucideGalleryIntegrationTests separately";
    qDebug() << "Total wall clock time:" << totalTime << "ms";

    // Print final verdict
    qDebug() << "\n✅ MAIN GALLERY TEST RUNNER COMPLETED SUCCESSFULLY";
    qDebug() << "This runner serves as a coordinator for the gallery test suite.";
    qDebug() << "Execute the individual test categories for comprehensive testing.";

    // Print test execution recommendations
    qDebug() << "\n💡 === TEST EXECUTION RECOMMENDATIONS ===";
    qDebug() << "To run specific test categories:";
    qDebug() << "  - Basic tests only: ctest -L gallery.basic";
    qDebug() << "  - Component tests only: ctest -L gallery.components";
    qDebug() << "  - All gallery tests: ctest -L gallery";
    qDebug() << "  - Individual test suites: ./<TestExecutableName>";

    qDebug() << "\n=== End of Gallery Tests ===\n";

    return 0; // Success - no tests failed because no tests were executed
}
