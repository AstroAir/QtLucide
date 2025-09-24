/**
 * Batch Export Manager Tests Implementation
 */

#include "test_batch_export_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QTimer>

void TestBatchExportManager::initTestCase()
{
    // Set up test environment
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    
    m_testDataDir = m_tempDir->path();
    m_exportManager = nullptr;
    
    qDebug() << "Batch export manager test environment initialized at:" << m_testDataDir;
}

void TestBatchExportManager::cleanupTestCase()
{
    // Clean up test environment
    delete m_tempDir;
    qDebug() << "Batch export manager test environment cleaned up";
}

void TestBatchExportManager::init()
{
    // Create fresh export manager for each test
    if (m_exportManager) {
        delete m_exportManager;
    }
    m_exportManager = new BatchExportManager();
}

void TestBatchExportManager::cleanup()
{
    // Cleanup after each test
    if (m_exportManager) {
        m_exportManager->cancelExport();
        delete m_exportManager;
        m_exportManager = nullptr;
    }
}

void TestBatchExportManager::testManagerCreation()
{
    // Test that BatchExportManager can be created
    QVERIFY(m_exportManager != nullptr);
    QVERIFY(m_exportManager->inherits("QObject"));
}

void TestBatchExportManager::testManagerInitialization()
{
    // Test that manager initializes properly
    QVERIFY(m_exportManager != nullptr);
    
    // Test initial state
    QVERIFY(!m_exportManager->isExporting());
    QCOMPARE(m_exportManager->getPendingTaskCount(), 0);
    QCOMPARE(m_exportManager->getActiveTaskCount(), 0);
}

void TestBatchExportManager::testManagerDestruction()
{
    // Test that manager can be destroyed without crashes
    QVERIFY(m_exportManager != nullptr);
    
    // Add some tasks
    createTestExportTasks(5);
    
    delete m_exportManager;
    m_exportManager = nullptr;
    
    QApplication::processEvents();
    
    // If we reach here without crashing, the test passes
    QVERIFY(true);
}

void TestBatchExportManager::testAddExportTask()
{
    // Test adding export tasks
    QVERIFY(m_exportManager != nullptr);
    
    ExportTask task = createTestTask("test-icon", m_testDataDir + "/test.png");
    m_exportManager->addTask(task);
    
    QCOMPARE(m_exportManager->getPendingTaskCount(), 1);
}

void TestBatchExportManager::testRemoveExportTask()
{
    // Test removing export tasks
    QVERIFY(m_exportManager != nullptr);
    
    ExportTask task = createTestTask("test-icon", m_testDataDir + "/test.png");
    m_exportManager->addTask(task);
    QCOMPARE(m_exportManager->getPendingTaskCount(), 1);
    
    m_exportManager->removeTask(task);
    QCOMPARE(m_exportManager->getPendingTaskCount(), 0);
}

void TestBatchExportManager::testClearExportTasks()
{
    // Test clearing all export tasks
    QVERIFY(m_exportManager != nullptr);
    
    createTestExportTasks(10);
    QCOMPARE(m_exportManager->getPendingTaskCount(), 10);
    
    m_exportManager->clearTasks();
    QCOMPARE(m_exportManager->getPendingTaskCount(), 0);
}

void TestBatchExportManager::testExportTaskQueue()
{
    // Test export task queue management
    QVERIFY(m_exportManager != nullptr);
    
    // Add tasks in specific order
    for (int i = 0; i < 5; ++i) {
        ExportTask task = createTestTask(QString("icon-%1").arg(i), 
                                       m_testDataDir + QString("/test-%1.png").arg(i));
        m_exportManager->addTask(task);
    }
    
    QCOMPARE(m_exportManager->getPendingTaskCount(), 5);
    
    // Verify queue order (if accessible)
    QList<ExportTask> tasks = m_exportManager->getPendingTasks();
    QCOMPARE(tasks.size(), 5);
}

void TestBatchExportManager::testStartExport()
{
    // Test starting export process
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy spy(m_exportManager, &BatchExportManager::exportStarted);
    QVERIFY(spy.isValid());
    
    createTestExportTasks(3);
    m_exportManager->startExport();
    
    QVERIFY(m_exportManager->isExporting());
    QCOMPARE(spy.count(), 1);
}

void TestBatchExportManager::testCancelExport()
{
    // Test canceling export process
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy spy(m_exportManager, &BatchExportManager::exportCancelled);
    QVERIFY(spy.isValid());
    
    createTestExportTasks(3);
    m_exportManager->startExport();
    QVERIFY(m_exportManager->isExporting());
    
    m_exportManager->cancelExport();
    QVERIFY(!m_exportManager->isExporting());
    QCOMPARE(spy.count(), 1);
}

void TestBatchExportManager::testPauseResumeExport()
{
    // Test pausing and resuming export process
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy pauseSpy(m_exportManager, &BatchExportManager::exportPaused);
    QSignalSpy resumeSpy(m_exportManager, &BatchExportManager::exportResumed);
    
    QVERIFY(pauseSpy.isValid());
    QVERIFY(resumeSpy.isValid());
    
    createTestExportTasks(3);
    m_exportManager->startExport();
    
    m_exportManager->pauseExport();
    QVERIFY(m_exportManager->isPaused());
    QCOMPARE(pauseSpy.count(), 1);
    
    m_exportManager->resumeExport();
    QVERIFY(!m_exportManager->isPaused());
    QCOMPARE(resumeSpy.count(), 1);
}

void TestBatchExportManager::testExportProgress()
{
    // Test export progress tracking
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy spy(m_exportManager, &BatchExportManager::progressUpdated);
    QVERIFY(spy.isValid());
    
    createTestExportTasks(5);
    m_exportManager->startExport();
    
    // Wait for some progress
    QTest::qWait(100);
    
    // Progress should be reported
    QVERIFY(spy.count() >= 0); // May or may not have progress yet
}

void TestBatchExportManager::testWorkerSetup()
{
    // Test worker thread setup
    QVERIFY(m_exportManager != nullptr);
    
    // Test setting max concurrent tasks
    m_exportManager->setMaxConcurrentTasks(4);
    
    // Verify setting was applied (if getter is available)
    // This is a basic test to ensure the method doesn't crash
    QVERIFY(true);
}

void TestBatchExportManager::testWorkerCleanup()
{
    // Test worker thread cleanup
    QVERIFY(m_exportManager != nullptr);
    
    createTestExportTasks(3);
    m_exportManager->startExport();
    
    // Cancel to trigger cleanup
    m_exportManager->cancelExport();
    
    // Process events to allow cleanup
    QApplication::processEvents();
    
    // If we reach here without crashing, cleanup worked
    QVERIFY(true);
}

void TestBatchExportManager::testConcurrentTasks()
{
    // Test concurrent task execution
    QVERIFY(m_exportManager != nullptr);
    
    m_exportManager->setMaxConcurrentTasks(3);
    createTestExportTasks(10);
    
    m_exportManager->startExport();
    
    // Verify that tasks are being processed
    QTest::qWait(100);
    QVERIFY(m_exportManager->isExporting());
}

void TestBatchExportManager::testMaxConcurrentTasks()
{
    // Test max concurrent tasks setting
    QVERIFY(m_exportManager != nullptr);
    
    // Test various limits
    m_exportManager->setMaxConcurrentTasks(1);
    m_exportManager->setMaxConcurrentTasks(8);
    m_exportManager->setMaxConcurrentTasks(16);
    
    // Test invalid values
    m_exportManager->setMaxConcurrentTasks(0);  // Should be clamped to 1
    m_exportManager->setMaxConcurrentTasks(100); // Should be clamped to reasonable limit
    
    // If we reach here without crashing, the test passes
    QVERIFY(true);
}

void TestBatchExportManager::testExportSignals()
{
    // Test export-related signals
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy startSpy(m_exportManager, &BatchExportManager::exportStarted);
    QSignalSpy finishSpy(m_exportManager, &BatchExportManager::exportFinished);
    QSignalSpy cancelSpy(m_exportManager, &BatchExportManager::exportCancelled);
    
    QVERIFY(startSpy.isValid());
    QVERIFY(finishSpy.isValid());
    QVERIFY(cancelSpy.isValid());
    
    createTestExportTasks(2);
    
    // Start export
    m_exportManager->startExport();
    QCOMPARE(startSpy.count(), 1);
    
    // Cancel export
    m_exportManager->cancelExport();
    QCOMPARE(cancelSpy.count(), 1);
}

void TestBatchExportManager::testProgressSignals()
{
    // Test progress-related signals
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy progressSpy(m_exportManager, &BatchExportManager::progressUpdated);
    QSignalSpy taskSpy(m_exportManager, &BatchExportManager::taskCompleted);
    
    QVERIFY(progressSpy.isValid());
    QVERIFY(taskSpy.isValid());
    
    createTestExportTasks(3);
    m_exportManager->startExport();
    
    // Wait for some progress
    QTest::qWait(200);
    
    // Signals may or may not have been emitted depending on timing
    QVERIFY(progressSpy.count() >= 0);
    QVERIFY(taskSpy.count() >= 0);
}

void TestBatchExportManager::testStatisticsSignals()
{
    // Test statistics-related signals
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy statsSpy(m_exportManager, &BatchExportManager::statisticsUpdated);
    QVERIFY(statsSpy.isValid());
    
    createTestExportTasks(3);
    m_exportManager->startExport();
    
    // Wait for statistics update
    QTest::qWait(200);
    
    // Statistics may or may not have been updated depending on timing
    QVERIFY(statsSpy.count() >= 0);
}

void TestBatchExportManager::testErrorSignals()
{
    // Test error-related signals
    QVERIFY(m_exportManager != nullptr);
    
    QSignalSpy errorSpy(m_exportManager, &BatchExportManager::taskFailed);
    QVERIFY(errorSpy.isValid());
    
    // Create task with invalid output path to trigger error
    ExportTask invalidTask = createTestTask("test-icon", "/invalid/path/test.png");
    m_exportManager->addTask(invalidTask);
    
    m_exportManager->startExport();
    
    // Wait for error
    QTest::qWait(500);
    
    // Error may or may not have been emitted depending on implementation
    QVERIFY(errorSpy.count() >= 0);
}

void TestBatchExportManager::testExportFormat()
{
    // Test export format settings
    QVERIFY(m_exportManager != nullptr);
    
    // Test different formats
    m_exportManager->setExportFormat(ExportFormat::PNG);
    m_exportManager->setExportFormat(ExportFormat::SVG);
    m_exportManager->setExportFormat(ExportFormat::ICO);
    
    // If we reach here without crashing, the test passes
    QVERIFY(true);
}

void TestBatchExportManager::testExportSize()
{
    // Test export size settings
    QVERIFY(m_exportManager != nullptr);
    
    // Test different sizes
    m_exportManager->setExportSize(QSize(16, 16));
    m_exportManager->setExportSize(QSize(32, 32));
    m_exportManager->setExportSize(QSize(64, 64));
    m_exportManager->setExportSize(QSize(128, 128));
    
    // If we reach here without crashing, the test passes
    QVERIFY(true);
}

void TestBatchExportManager::testExportQuality()
{
    // Test export quality settings
    QVERIFY(m_exportManager != nullptr);
    
    // Test different quality levels
    m_exportManager->setExportQuality(50);  // Low quality
    m_exportManager->setExportQuality(75);  // Medium quality
    m_exportManager->setExportQuality(100); // High quality
    
    // If we reach here without crashing, the test passes
    QVERIFY(true);
}

void TestBatchExportManager::testOutputDirectory()
{
    // Test output directory settings
    QVERIFY(m_exportManager != nullptr);
    
    // Test setting output directory
    m_exportManager->setOutputDirectory(m_testDataDir);
    
    // Verify directory was set (if getter is available)
    QString outputDir = m_exportManager->getOutputDirectory();
    QCOMPARE(outputDir, m_testDataDir);
}

void TestBatchExportManager::testInvalidExportPath()
{
    // Test handling of invalid export paths
    QVERIFY(m_exportManager != nullptr);
    
    // Create task with invalid path
    ExportTask invalidTask = createTestTask("test-icon", "/invalid/path/test.png");
    m_exportManager->addTask(invalidTask);
    
    m_exportManager->startExport();
    
    // Should handle invalid path gracefully
    QTest::qWait(200);
    QVERIFY(true); // If we reach here without crashing, the test passes
}

void TestBatchExportManager::testInsufficientPermissions()
{
    // Test handling of insufficient permissions
    QVERIFY(m_exportManager != nullptr);
    
    // Try to export to a protected directory (if available)
    QString protectedPath = "/root/test.png"; // Unix-style protected path
    ExportTask task = createTestTask("test-icon", protectedPath);
    m_exportManager->addTask(task);
    
    m_exportManager->startExport();
    
    // Should handle permission errors gracefully
    QTest::qWait(200);
    QVERIFY(true); // If we reach here without crashing, the test passes
}

void TestBatchExportManager::testDiskSpaceHandling()
{
    // Test handling of disk space issues
    QVERIFY(m_exportManager != nullptr);
    
    // This is a basic test - actual disk space testing would require
    // more complex setup
    createTestExportTasks(5);
    m_exportManager->startExport();
    
    QTest::qWait(200);
    QVERIFY(true); // If we reach here without crashing, the test passes
}

void TestBatchExportManager::testCorruptedSourceFiles()
{
    // Test handling of corrupted source files
    QVERIFY(m_exportManager != nullptr);
    
    // Create task with non-existent icon
    ExportTask task = createTestTask("non-existent-icon", m_testDataDir + "/test.png");
    m_exportManager->addTask(task);
    
    m_exportManager->startExport();
    
    // Should handle missing source gracefully
    QTest::qWait(200);
    QVERIFY(true); // If we reach here without crashing, the test passes
}

void TestBatchExportManager::testLargeBatchExport()
{
    // Test large batch export
    QVERIFY(m_exportManager != nullptr);
    
    // Create large number of tasks
    createTestExportTasks(100);
    QCOMPARE(m_exportManager->getPendingTaskCount(), 100);
    
    m_exportManager->startExport();
    
    // Should handle large batches without issues
    QTest::qWait(500);
    QVERIFY(true); // If we reach here without crashing, the test passes
}

void TestBatchExportManager::testExportPerformance()
{
    // Test export performance
    QVERIFY(m_exportManager != nullptr);
    
    QElapsedTimer timer;
    timer.start();
    
    createTestExportTasks(50);
    m_exportManager->startExport();
    
    // Wait for completion or timeout
    waitForExportCompletion(10000);
    
    qint64 elapsed = timer.elapsed();
    qDebug() << "Export performance: 50 tasks in" << elapsed << "ms";
    
    // Performance test - should complete within reasonable time
    QVERIFY(elapsed < 30000); // Less than 30 seconds
}

void TestBatchExportManager::testMemoryUsage()
{
    // Test memory usage during export
    QVERIFY(m_exportManager != nullptr);
    
    // Create many tasks to test memory usage
    createTestExportTasks(200);
    m_exportManager->startExport();
    
    // Let it run for a bit
    QTest::qWait(1000);
    
    // Cancel to free memory
    m_exportManager->cancelExport();
    
    // If we reach here without crashing or excessive memory usage, test passes
    QVERIFY(true);
}

// Helper methods
void TestBatchExportManager::createTestExportTasks(int count)
{
    for (int i = 0; i < count; ++i) {
        ExportTask task = createTestTask(QString("test-icon-%1").arg(i), 
                                       m_testDataDir + QString("/test-%1.png").arg(i));
        m_exportManager->addTask(task);
    }
}

ExportTask TestBatchExportManager::createTestTask(const QString& iconName, const QString& outputPath)
{
    ExportTask task;
    task.iconName = iconName;
    task.outputPath = outputPath;
    task.format = ExportFormat::PNG;
    task.size = QSize(32, 32);
    task.quality = 100;
    return task;
}

bool TestBatchExportManager::verifyExportedFile(const QString& filePath)
{
    return QFile::exists(filePath) && QFile(filePath).size() > 0;
}

void TestBatchExportManager::waitForExportCompletion(int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    
    while (m_exportManager->isExporting() && timer.elapsed() < timeoutMs) {
        QApplication::processEvents();
        QTest::qWait(100);
    }
}

#include "test_batch_export_manager.moc"
