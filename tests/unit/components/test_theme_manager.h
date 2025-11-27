/**
 * Theme Manager Tests
 * Tests for theme management system functionality
 */

#ifndef TEST_THEME_MANAGER_H
#define TEST_THEME_MANAGER_H

#include <QObject>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QtTest/QtTest>

// Gallery includes
#include "ui/themes/ThemeManager.h"

class TestThemeManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic theme management tests
    void testThemeManagerCreation();
    void testDefaultTheme();
    void testThemeSwitching();
    void testThemeSignals();

    // Built-in theme tests
    void testLightTheme();
    void testDarkTheme();
    void testSystemTheme();

    // Custom theme tests
    void testCustomThemeLoading();
    void testCustomThemeSaving();
    void testInvalidCustomTheme();
    void testCustomThemeValidation();

    // Theme application tests
    void testThemeApplication();
    void testStyleSheetGeneration();
    void testColorRetrieval();

    // Error handling tests
    void testInvalidThemeFile();
    void testMissingThemeFile();
    void testCorruptedThemeFile();

    // Performance tests
    void testThemeSwitchingPerformance();
    void testMultipleThemeLoads();

private:
    ThemeManager* m_themeManager;
    QString m_testDataDir;

    // Helper methods
    void createTestThemeFile(const QString& filePath, bool valid = true);
    bool verifyThemeColors(const ThemeManager::ThemeColors& colors);
};

#endif // TEST_THEME_MANAGER_H
