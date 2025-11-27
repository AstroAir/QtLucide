/**
 * QtLucide Test Runner
 */

#include <QApplication>
#include <QtTest/QtTest>

// Include test class headers
#include "unit/core/test_boundary_conditions.h"
#include "unit/core/test_error_handling.h"
#include "unit/core/test_icon_engine.h"
#include "unit/core/test_icon_loading.h"
#include "unit/core/test_icon_painter.h"
#include "unit/core/test_memory_management.h"
#include "unit/core/test_qtlucide.h"
#include "unit/core/test_svg_rendering.h"
#include "unit/core/test_thread_safety.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    int result = 0;

    // Run all test classes
    {
        TestQtLucide test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestIconEngine test;
        result |= QTest::qExec(&test, argc, argv);
    }

    {
        TestIconPainter test;
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

    return result;
}
