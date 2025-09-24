/**
 * Gallery Integration Tests
 * Tests for end-to-end gallery functionality and component integration
 */

#ifndef TEST_GALLERY_INTEGRATION_H
#define TEST_GALLERY_INTEGRATION_H

#include <QObject>
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTemporaryDir>

// Gallery concrete implementation includes
#include "../../../examples/gallery/src/ui/themes/ThemeManager.h"
#include "../../../examples/gallery/src/core/managers/IconMetadataManager.h"
#include "../../../examples/gallery/src/core/BatchExportManager.h"
#include "../../../examples/gallery/src/ui/windows/GalleryMainWindow.h"
#include "../../../examples/gallery/src/ui/dialogs/ExportDialog.h"

class TestGalleryIntegration : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Application integration tests
    void testFullApplicationStartup();
    void testApplicationShutdown();
    void testComponentInitialization();
    void testComponentCommunication();
    
    // Icon loading and display tests
    void testIconLoadingWorkflow();
    void testIconDisplayInGrid();
    void testIconSelectionWorkflow();
    void testIconDetailsDisplay();
    
    // Search integration tests
    void testSearchToDisplayWorkflow();
    void testFilterIntegration();
    void testSearchResultsDisplay();
    void testSearchHistoryIntegration();
    
    // Theme integration tests
    void testThemeApplicationWorkflow();
    void testThemeSwitchingIntegration();
    void testCustomThemeWorkflow();
    void testThemeUIUpdates();
    
    // Export integration tests
    void testExportWorkflowFromSelection();
    void testBatchExportIntegration();
    void testExportProgressIntegration();
    void testExportCompletionWorkflow();
    
    // Import integration tests
    void testImportWorkflowComplete();
    void testImportPreviewIntegration();
    void testImportValidationWorkflow();
    
    // File browser integration tests
    void testFileBrowserNavigation();
    void testFileBrowserToGalleryIntegration();
    void testImageFilteringIntegration();
    
    // Settings integration tests
    void testSettingsLoadSaveIntegration();
    void testSettingsUIIntegration();
    void testSettingsPersistence();
    
    // Error handling integration tests
    void testErrorDisplayIntegration();
    void testErrorRecoveryWorkflow();
    void testGracefulDegradation();
    
    // Performance integration tests
    void testLargeIconSetHandling();
    void testMemoryManagementIntegration();
    void testResponsiveUIPerformance();

private:
    GalleryMainWindow* m_mainWindow;
    ThemeManager* m_themeManager;
    IconMetadataManager* m_metadataManager;
    BatchExportManager* m_exportManager;
    QTemporaryDir* m_tempDir;
    QString m_testDataDir;
    
    // Helper methods
    void setupTestEnvironment();
    void createTestIconSet(int count);
    void simulateUserWorkflow();
    bool verifyComponentIntegration();
    void waitForUIUpdate(int timeoutMs = 1000);
};

#endif // TEST_GALLERY_INTEGRATION_H
