/**
 * QtLucide Icon Loading Tests - Header
 */

#ifndef TEST_ICON_LOADING_H
#define TEST_ICON_LOADING_H

#include <QtTest/QtTest>
#include <QObject>

namespace lucide {
class QtLucide;
}

class TestIconLoading : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testSvgDataLoading();
    void testSvgDataValidity();
    void testIconPixmapGeneration();
    void testIconScaling();
    void testIconModes();
    void testResourceAccess();

private:
    lucide::QtLucide* m_lucide;
};

#endif // TEST_ICON_LOADING_H
