/**
 * QtLucide Test Runner
 */

#include <QtTest/QtTest>
#include <QApplication>

// Include test classes
#include "test_qtlucide.cpp"
#include "test_icon_loading.cpp"
#include "test_svg_rendering.cpp"

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
    
    return result;
}
