/**
 * QtLucide SVG Rendering Tests - Header
 */

#ifndef TEST_SVG_RENDERING_H
#define TEST_SVG_RENDERING_H

#include <QtTest/QtTest>
#include <QObject>

namespace lucide {
class QtLucide;
}

class TestSvgRendering : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testColorCustomization();
    void testScaleFactorRendering();
    void testIconStateRendering();
    void testCustomPainter();
    void testRenderingPerformance();

private:
    lucide::QtLucide* m_lucide;
};

#endif // TEST_SVG_RENDERING_H
