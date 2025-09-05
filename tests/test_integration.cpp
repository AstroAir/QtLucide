/**
 * QtLucide Integration Tests - Implementation
 */

#include "test_integration.h"
#include <QtLucide/QtLucide.h>
#include "../examples/gallery/IconMetadataManager.h"
#include "../examples/gallery/IconItem.h"

#include <QPixmap>
#include <QIcon>
#include <QResource>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QThread>

void TestIntegration::initTestCase() {
    qDebug() << "Initializing Integration Test Suite";
    
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());
    
    // Initialize metadata manager
    m_metadataManager = new IconMetadataManager(this);
    QVERIFY(m_metadataManager != nullptr);
    
    // Create temporary directory for test files
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    
    // Initialize test widget
    createTestWidget();
    
    qDebug() << "Integration test environment initialized successfully";
}

void TestIntegration::cleanupTestCase() {
    destroyTestWidget();
    delete m_tempDir;
    delete m_metadataManager;
    delete m_lucide;
    qDebug() << "Integration test cleanup completed";
}

void TestIntegration::init() {
    m_performanceMetrics.clear();
    m_performanceTimer.start();
}

void TestIntegration::cleanup() {
    // Reset any test state
    if (m_testLabel) {
        m_testLabel->clear();
    }
}

void TestIntegration::testCompleteIconCreationPipeline() {
    qDebug() << "Testing complete icon creation pipeline";
    
    // Test the full pipeline: name -> enum -> icon -> pixmap -> display
    QString iconName = "heart";
    
    // Step 1: Verify icon name exists
    QStringList availableIcons = m_lucide->availableIcons();
    QVERIFY(availableIcons.contains(iconName));
    
    // Step 2: Convert name to enum
    lucide::Icons iconEnum = m_lucide->stringToIconId(iconName);
    QVERIFY(iconEnum != lucide::Icons::unknown);
    
    // Step 3: Create QIcon from enum
    QIcon icon = m_lucide->icon(iconEnum);
    QVERIFY(!icon.isNull());
    
    // Step 4: Generate pixmap
    QPixmap pixmap = icon.pixmap(64, 64);
    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.size(), QSize(64, 64));
    
    // Step 5: Display in widget
    m_testLabel->setPixmap(pixmap);
    QVERIFY(!m_testLabel->pixmap().isNull());
    
    qDebug() << "Complete icon creation pipeline test passed";
}

void TestIntegration::testIconLoadingToDisplayWorkflow() {
    qDebug() << "Testing icon loading to display workflow";
    
    QStringList testIcons = {"star", "home", "user", "settings", "search"};
    
    for (const QString& iconName : testIcons) {
        // Load icon
        QIcon icon = m_lucide->icon(iconName);
        QVERIFY2(!icon.isNull(), qPrintable(QString("Failed to load icon: %1").arg(iconName)));
        
        // Render at different sizes
        QList<int> sizes = {16, 24, 32, 48, 64, 128};
        for (int size : sizes) {
            QPixmap pixmap = icon.pixmap(size, size);
            QVERIFY2(!pixmap.isNull(), qPrintable(QString("Failed to render %1 at size %2").arg(iconName).arg(size)));
            QCOMPARE(pixmap.size(), QSize(size, size));
        }
    }
    
    qDebug() << "Icon loading to display workflow test passed";
}

void TestIntegration::testResourceSystemIntegration() {
    qDebug() << "Testing resource system integration";
    
    // Verify QRC resource system is properly integrated
    QVERIFY(QResource::registerResource(":/lucide"));
    
    // Test resource directory access
    QDir resourceDir(":/lucide");
    QVERIFY(resourceDir.exists());
    
    QStringList entries = resourceDir.entryList();
    QVERIFY(entries.size() > 1000); // Should have 1634+ icons
    
    // Test specific resource access
    QFile heartIcon(":/lucide/heart");
    QVERIFY(heartIcon.exists());
    QVERIFY(heartIcon.open(QIODevice::ReadOnly));
    
    QByteArray iconData = heartIcon.readAll();
    QVERIFY(!iconData.isEmpty());
    QVERIFY(iconData.contains("<svg")); // Should be SVG content
    
    heartIcon.close();
    qDebug() << "Resource system integration test passed";
}

void TestIntegration::testLibraryGalleryIntegration() {
    qDebug() << "Testing library-gallery integration";
    
    // Test metadata manager integration with QtLucide
    QVERIFY(m_metadataManager->initialize());
    
    // Verify icon count consistency
    int lucideIconCount = m_lucide->availableIcons().size();
    int metadataIconCount = m_metadataManager->getTotalIconCount();
    
    QCOMPARE(lucideIconCount, metadataIconCount);
    
    // Test icon item creation
    QString testIconName = "calendar";
    IconItem* iconItem = new IconItem(testIconName, m_testWidget);
    QVERIFY(iconItem != nullptr);
    
    // Verify icon item has proper icon
    QIcon itemIcon = iconItem->getIcon();
    QVERIFY(!itemIcon.isNull());
    
    delete iconItem;
    qDebug() << "Library-gallery integration test passed";
}

void TestIntegration::testApplicationLifecycleIntegration() {
    qDebug() << "Testing application lifecycle integration";
    
    // Test initialization sequence
    lucide::QtLucide* tempLucide = new lucide::QtLucide();
    QVERIFY(tempLucide->initLucide());
    
    // Verify resources are available after initialization
    QVERIFY(!tempLucide->availableIcons().isEmpty());
    
    // Test icon creation after initialization
    QIcon testIcon = tempLucide->icon("home");
    QVERIFY(!testIcon.isNull());
    
    // Test cleanup sequence
    delete tempLucide;
    
    // Verify our main instance still works after cleanup of another instance
    QIcon mainIcon = m_lucide->icon("star");
    QVERIFY(!mainIcon.isNull());
    
    qDebug() << "Application lifecycle integration test passed";
}

void TestIntegration::testIntegratedPerformanceMetrics() {
    qDebug() << "Testing integrated performance metrics";
    
    QElapsedTimer timer;
    timer.start();
    
    // Test batch icon creation performance
    QStringList testIcons = m_lucide->availableIcons().mid(0, 50); // Test first 50 icons
    
    for (const QString& iconName : testIcons) {
        QIcon icon = m_lucide->icon(iconName);
        QVERIFY(!icon.isNull());
        
        QPixmap pixmap = icon.pixmap(32, 32);
        QVERIFY(!pixmap.isNull());
    }
    
    qint64 elapsed = timer.elapsed();
    qDebug() << "Created and rendered 50 icons in" << elapsed << "ms";
    
    // Performance should be reasonable (less than 2 seconds for 50 icons)
    QVERIFY(elapsed < 2000);
    
    // Average should be less than 40ms per icon
    double averagePerIcon = static_cast<double>(elapsed) / testIcons.size();
    QVERIFY(averagePerIcon < 40.0);
    
    qDebug() << "Integrated performance metrics test passed";
}

void TestIntegration::createTestWidget() {
    m_testWidget = new QWidget();
    m_testWidget->resize(200, 200);
    
    m_testLabel = new QLabel(m_testWidget);
    m_testLabel->setGeometry(10, 10, 64, 64);
    m_testLabel->setStyleSheet("border: 1px solid gray;");
}

void TestIntegration::destroyTestWidget() {
    delete m_testWidget;
    m_testWidget = nullptr;
    m_testLabel = nullptr;
}

bool TestIntegration::verifyIconRendering(const QString& iconName, const QSize& size) {
    QIcon icon = m_lucide->icon(iconName);
    if (icon.isNull()) return false;
    
    QPixmap pixmap = icon.pixmap(size);
    return !pixmap.isNull() && pixmap.size() == size;
}

#include "test_integration.moc"
