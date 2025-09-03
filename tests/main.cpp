/**
 * QtLucide Test Runner
 */

#include <QtTest/QtTest>
#include <QApplication>

// Include test class headers
#include "test_qtlucide.h"
#include "test_icon_loading.h"
#include "test_svg_rendering.h"
#include "test_gallery_icon_loading.h"

int main(int argc, char *argv[])
{
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

    return result;
}
