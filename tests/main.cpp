/**
 * QtLucide Test Runner
 */

#include <QApplication>
#include <QtTest/QtTest>

// Include test class headers
#include "test_boundary_conditions.h"
#include "test_error_handling.h"
#include "test_gallery_icon_loading.h"
#include "test_icon_loading.h"
#include "test_memory_management.h"
#include "test_qtlucide.h"
#include "test_svg_rendering.h"
#include "test_thread_safety.h"
#include "test_integration.h"
#include "test_ui_components.h"
#include "test_performance_regression.h"
#include "test_platform_specific.h"
#include "test_export_functionality.h"
#include "test_accessibility.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    int result = 0;

    // Run all test classes
    {
        TestQtLucide test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestIconLoading test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestSvgRendering test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestGalleryIconLoading test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestErrorHandling test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestThreadSafety test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestBoundaryConditions test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestMemoryManagement test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestIntegration test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestUIComponents test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestPerformanceRegression test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestPlatformSpecific test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestExportFunctionality test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestAccessibility test;
        result |= QTest::qExec(&test, argc, argv);
    }

    return result;
}
