/**
 * QtLucide Integration Tests - Header
 *
 * Comprehensive end-to-end tests that verify the complete workflow
 * from icon loading to rendering, including cross-component interactions.
 */

#ifndef TEST_INTEGRATION_H
#define TEST_INTEGRATION_H

#include <QObject>
#include <QtTest/QtTest>
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QEventLoop>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>

namespace lucide {
class QtLucide;
}

class IconMetadataManager;
class IconItem;

class TestIntegration : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // End-to-end workflow tests
    void testCompleteIconCreationPipeline();
    void testIconLoadingToDisplayWorkflow();
    void testIconRenderingPipeline();
    void testIconCachingWorkflow();
    void testIconCustomizationPipeline();

    // Resource loading integration tests
    void testResourceSystemIntegration();
    void testQrcResourceLoading();
    void testResourceInitializationSequence();
    void testResourceCleanupSequence();
    void testResourceReloadingCapability();

    // Cross-component interaction tests
    void testLibraryGalleryIntegration();
    void testMetadataManagerIntegration();
    void testIconItemIntegration();
    void testMultiComponentWorkflow();
    void testComponentStateSync();

    // Application lifecycle integration
    void testApplicationLifecycleIntegration();
    void testApplicationStartupIntegration();
    void testApplicationShutdownIntegration();
    void testApplicationStateTransitions();
    void testApplicationRecoveryScenarios();

    // Data flow integration tests
    void testIconDataFlowIntegrity();
    void testMetadataDataFlowIntegrity();
    void testSettingsDataFlowIntegrity();
    void testEventPropagationIntegrity();

    // Performance integration tests
    void testIntegratedPerformanceMetrics();
    void testMemoryIntegrationPatterns();
    void testResourceUsageIntegration();
    void testScalabilityIntegration();

private:
    // Helper methods
    void createTestWidget();
    void destroyTestWidget();
    bool verifyIconRendering(const QString& iconName, const QSize& size);
    bool verifyResourceAvailability(const QString& resourcePath);
    void simulateUserInteraction();
    void measureIntegrationPerformance();

    // Test data and objects
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    QWidget* m_testWidget;
    QLabel* m_testLabel;
    QTemporaryDir* m_tempDir;

    // Performance tracking
    QElapsedTimer m_performanceTimer;
    QList<qint64> m_performanceMetrics;

    // Test configuration
    static constexpr int INTEGRATION_TIMEOUT_MS = 5000;
    static constexpr int PERFORMANCE_ITERATIONS = 100;
    static constexpr int MAX_ACCEPTABLE_DELAY_MS = 100;
};

#endif // TEST_INTEGRATION_H
