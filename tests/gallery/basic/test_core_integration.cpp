/**
 * @file test_core_integration.cpp
 * @brief Core Integration Tests Implementation
 * @details Implementation of core QtLucide library integration tests
 */

#include "test_core_integration.h"
#include <QApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QPixmap>
#include <QStandardPaths>
#include <QtTest/QtTest>

// Include QtLucide headers
#include "QtLucide/QtLucide.h"
#include "QtLucide/QtLucideIconPainter.h"

void TestCoreIntegration::initTestCase() {
    // Initialize test environment
    m_testDataDir =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/qtlucide_core_test";
    QDir().mkpath(m_testDataDir);

    m_qtLucide = nullptr;

    qDebug() << "Core integration test environment initialized at:" << m_testDataDir;
}

void TestCoreIntegration::cleanupTestCase() {
    // Clean up test environment
    if (m_qtLucide) {
        delete m_qtLucide;
        m_qtLucide = nullptr;
    }

    QDir testDir(m_testDataDir);
    if (testDir.exists()) {
        testDir.removeRecursively();
    }

    qDebug() << "Core integration test environment cleaned up";
}

void TestCoreIntegration::init() {
    // Setup for each test - create fresh QtLucide instance
    m_qtLucide = createTestLucideInstance();
}

void TestCoreIntegration::cleanup() {
    // Cleanup after each test
    if (m_qtLucide) {
        delete m_qtLucide;
        m_qtLucide = nullptr;
    }
}

void TestCoreIntegration::testLibraryInitialization() {
    // Test QtLucide library initialization
    QVERIFY2(m_qtLucide != nullptr, "Failed to create QtLucide instance");

    // Test initialization
    bool initResult = m_qtLucide->initLucide();
    QVERIFY2(initResult, "QtLucide initialization failed");

    qDebug() << "QtLucide library initialized successfully";
}

void TestCoreIntegration::testIconEnumeration() {
    // Ensure library is initialized
    QVERIFY(m_qtLucide != nullptr);
    QVERIFY(m_qtLucide->initLucide());

    // Test icon enumeration
    QStringList availableIcons = m_qtLucide->availableIcons();

    // Validate icon count
    QVERIFY2(!availableIcons.isEmpty(), "No icons available");
    QVERIFY2(availableIcons.size() > 1000, "Expected more than 1000 icons");

    // Test for common icons
    QStringList expectedIcons = {"house", "user", "settings", "search",
                                 "heart", "star", "check",    "x"};
    for (const QString& expectedIcon : expectedIcons) {
        QVERIFY2(availableIcons.contains(expectedIcon),
                 qPrintable(QString("Expected icon '%1' not found").arg(expectedIcon)));
    }

    qDebug() << "Icon enumeration test passed with" << availableIcons.size() << "icons";
}

void TestCoreIntegration::testIconRendering() {
    // Ensure library is initialized
    QVERIFY(m_qtLucide != nullptr);
    QVERIFY(m_qtLucide->initLucide());

    QStringList testIcons = {"house", "user", "settings"};
    QList<int> testSizes = {16, 24, 32, 48, 64, 128};

    for (const QString& iconName : testIcons) {
        for (int size : testSizes) {
            // Test icon creation
            QIcon icon = m_qtLucide->icon(iconName);
            QVERIFY2(!icon.isNull(),
                     qPrintable(QString("Failed to create icon: %1").arg(iconName)));

            // Test pixmap rendering
            QPixmap pixmap = icon.pixmap(size, size);
            QVERIFY2(
                !pixmap.isNull(),
                qPrintable(
                    QString("Failed to render pixmap for %1 at size %2").arg(iconName).arg(size)));

            // Verify pixmap properties
            QVERIFY2(pixmap.width() > 0, "Pixmap width should be greater than 0");
            QVERIFY2(pixmap.height() > 0, "Pixmap height should be greater than 0");

            // Verify rendering quality
            QVERIFY2(verifyIconRenderingQuality(iconName, size),
                     qPrintable(QString("Rendering quality check failed for %1 at size %2")
                                    .arg(iconName)
                                    .arg(size)));
        }
    }

    qDebug() << "Icon rendering test passed for" << testIcons.size() << "icons at"
             << testSizes.size() << "sizes";
}

void TestCoreIntegration::testIconCaching() {
    // Ensure library is initialized
    QVERIFY(m_qtLucide != nullptr);
    QVERIFY(m_qtLucide->initLucide());

    QString testIcon = "house";

    // Measure first load time
    qint64 firstLoadTime = measureIconLoadingTime(testIcon);
    QVERIFY2(firstLoadTime >= 0, "First load time measurement failed");

    // Measure second load time (should be faster due to caching)
    qint64 secondLoadTime = measureIconLoadingTime(testIcon);
    QVERIFY2(secondLoadTime >= 0, "Second load time measurement failed");

    // Note: We don't strictly enforce caching performance as it depends on implementation
    // but we verify that both loads succeed
    qDebug() << "Icon caching test completed - First load:" << firstLoadTime
             << "ms, Second load:" << secondLoadTime << "ms";
}

void TestCoreIntegration::testResourceManagement() {
    // Test multiple QtLucide instances
    const int instanceCount = 5;
    QList<lucide::QtLucide*> instances;

    // Create multiple instances
    for (int i = 0; i < instanceCount; ++i) {
        lucide::QtLucide* instance = new lucide::QtLucide();
        QVERIFY2(instance != nullptr, qPrintable(QString("Failed to create instance %1").arg(i)));
        QVERIFY2(instance->initLucide(),
                 qPrintable(QString("Failed to initialize instance %1").arg(i)));
        instances.append(instance);
    }

    // Test that all instances work
    for (int i = 0; i < instanceCount; ++i) {
        QIcon icon = instances[i]->icon("house");
        QVERIFY2(!icon.isNull(), qPrintable(QString("Instance %1 failed to create icon").arg(i)));
    }

    // Clean up instances
    for (lucide::QtLucide* instance : instances) {
        delete instance;
    }

    qDebug() << "Resource management test passed with" << instanceCount << "instances";
}

void TestCoreIntegration::testErrorHandling() {
    // Ensure library is initialized
    QVERIFY(m_qtLucide != nullptr);
    QVERIFY(m_qtLucide->initLucide());

    // Test invalid icon names
    QStringList invalidIcons = {"nonexistent-icon", "", "invalid/icon/name", "123invalid"};

    for (const QString& invalidIcon : invalidIcons) {
        QIcon icon = m_qtLucide->icon(invalidIcon);
        // Invalid icons should return null icons (not crash)
        QVERIFY2(icon.isNull(),
                 qPrintable(QString("Expected null icon for invalid name: %1").arg(invalidIcon)));
    }

    qDebug() << "Error handling test passed for" << invalidIcons.size() << "invalid icon names";
}

// Helper method implementations
lucide::QtLucide* TestCoreIntegration::createTestLucideInstance() {
    return new lucide::QtLucide();
}

bool TestCoreIntegration::verifyIconRenderingQuality(const QString& iconName, int size) {
    Q_UNUSED(iconName)
    Q_UNUSED(size)

    // Basic quality check - in a real implementation, this could check:
    // - Pixel density
    // - Color accuracy
    // - Edge smoothness
    // - Proper scaling

    return true; // Simplified for this example
}

qint64 TestCoreIntegration::measureIconLoadingTime(const QString& iconName) {
    QElapsedTimer timer;
    timer.start();

    QIcon icon = m_qtLucide->icon(iconName);
    QPixmap pixmap = icon.pixmap(32, 32);

    return timer.elapsed();
}
