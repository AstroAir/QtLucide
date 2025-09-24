/**
 * Application Startup Tests
 * Tests for gallery application initialization and basic functionality
 */

#ifndef TEST_APPLICATION_STARTUP_H
#define TEST_APPLICATION_STARTUP_H

#include <QObject>
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QTimer>

// Gallery concrete implementation includes
#include "../../../examples/gallery/src/ui/themes/ThemeManager.h"
#include "../../../examples/gallery/src/core/managers/IconMetadataManager.h"
#include "../../../examples/gallery/src/core/BatchExportManager.h"
#include "../../../examples/gallery/src/ui/windows/GalleryMainWindow.h"

class TestApplicationStartup : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic startup tests
    void testGalleryMainWindowCreation();
    void testGalleryMainWindowInitialization();
    void testGalleryMainWindowShow();
    void testGalleryMainWindowClose();
    
    // Component initialization tests
    void testThemeManagerInitialization();
    void testIconMetadataManagerInitialization();
    void testDefaultThemeLoading();
    void testResourcesAvailable();
    
    // Memory management tests
    void testGalleryMainWindowDestruction();
    void testComponentCleanup();
    
    // Configuration tests
    void testDefaultSettings();
    void testSettingsLoad();
    void testSettingsSave();

private:
    GalleryMainWindow* m_mainWindow;
    ThemeManager* m_themeManager;
    IconMetadataManager* m_metadataManager;
    QString m_testDataDir;
};

#endif // TEST_APPLICATION_STARTUP_H
