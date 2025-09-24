/**
 * Batch Export Manager Tests
 * Tests for batch export functionality and progress tracking
 */

#ifndef TEST_BATCH_EXPORT_MANAGER_H
#define TEST_BATCH_EXPORT_MANAGER_H

#include <QObject>
#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTemporaryDir>

// Gallery includes
#include "core/BatchExportManager.h"

class TestBatchExportManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Basic functionality tests
    void testManagerCreation();
    void testManagerInitialization();
    void testManagerDestruction();
    
    // Export task management tests
    void testAddExportTask();
    void testRemoveExportTask();
    void testClearExportTasks();
    void testExportTaskQueue();
    
    // Export execution tests
    void testStartExport();
    void testCancelExport();
    void testPauseResumeExport();
    void testExportProgress();
    
    // Worker management tests
    void testWorkerSetup();
    void testWorkerCleanup();
    void testConcurrentTasks();
    void testMaxConcurrentTasks();
    
    // Signal tests
    void testExportSignals();
    void testProgressSignals();
    void testStatisticsSignals();
    void testErrorSignals();
    
    // Export settings tests
    void testExportFormat();
    void testExportSize();
    void testExportQuality();
    void testOutputDirectory();
    
    // Error handling tests
    void testInvalidExportPath();
    void testInsufficientPermissions();
    void testDiskSpaceHandling();
    void testCorruptedSourceFiles();
    
    // Performance tests
    void testLargeBatchExport();
    void testExportPerformance();
    void testMemoryUsage();

private:
    BatchExportManager* m_exportManager;
    QTemporaryDir* m_tempDir;
    QString m_testDataDir;
    
    // Helper methods
    void createTestExportTasks(int count);
    ExportTask createTestTask(const QString& iconName, const QString& outputPath);
    bool verifyExportedFile(const QString& filePath);
    void waitForExportCompletion(int timeoutMs = 5000);
};

#endif // TEST_BATCH_EXPORT_MANAGER_H
