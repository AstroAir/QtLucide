/**
 * QtLucide Thread Safety Tests - Header
 * Tests for concurrent access and thread safety
 */

#ifndef TEST_THREAD_SAFETY_H
#define TEST_THREAD_SAFETY_H

#include <QAtomicInt>
#include <QMutex>
#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QtTest/QtTest>

namespace lucide {
class QtLucide;
}

class IconCreationWorker : public QObject {
    Q_OBJECT

public:
    IconCreationWorker(lucide::QtLucide* lucide, const QStringList& iconNames, int iterations);

public slots:
    void createIcons();

signals:
    void finished();
    void iconCreated(const QString& iconName, bool success);

private:
    lucide::QtLucide* m_lucide;
    QStringList m_iconNames;
    int m_iterations;
};

class TestThreadSafety : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // Basic thread safety tests
    void testConcurrentIconCreation();
    void testConcurrentIconCreationByEnum();
    void testConcurrentSvgDataAccess();
    void testConcurrentAvailableIconsAccess();

    // Configuration thread safety tests
    void testConcurrentDefaultOptionAccess();
    void testConfigurationDuringIconCreation();
    void testCustomPainterRegistrationSafety();

    // Stress tests
    void testHighConcurrencyIconCreation();
    void testMixedOperationsConcurrency();
    void testLongRunningConcurrentAccess();

    // Race condition tests
    void testInitializationRaceCondition();
    void testCustomPainterRaceCondition();
    void testMemoryManagementRaceCondition();

private:
    lucide::QtLucide* m_lucide;
    QStringList m_testIcons;

    void runConcurrentTest(std::function<void()> testFunction, int threadCount = 4);
};

#endif // TEST_THREAD_SAFETY_H
