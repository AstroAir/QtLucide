/**
 * Gallery Performance Tests
 */

#ifndef TEST_GALLERY_PERFORMANCE_H
#define TEST_GALLERY_PERFORMANCE_H

#include <QObject>
#include <QtTest/QtTest>

class TestGalleryPerformance : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testStartupPerformance();
    void testSearchPerformance();
    void testRenderingPerformance();
    void testMemoryUsage();

private:
    QString m_testDataDir;
};

#endif // TEST_GALLERY_PERFORMANCE_H
