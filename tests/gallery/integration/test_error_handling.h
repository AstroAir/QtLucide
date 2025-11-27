/**
 * Gallery Error Handling Tests
 */

#ifndef TEST_ERROR_HANDLING_H
#define TEST_ERROR_HANDLING_H

#include <QObject>
#include <QtTest/QtTest>

class TestErrorHandling : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInvalidFileHandling();
    void testNetworkErrorHandling();
    void testMemoryErrorHandling();
    void testGracefulDegradation();

private:
    QString m_testDataDir;
};

#endif // TEST_ERROR_HANDLING_H
