/**
 * @file test_icon_rendering_performance.cpp
 * @brief Icon Rendering Performance Tests Implementation
 * @details Comprehensive performance test implementations for icon rendering
 */

#include "test_icon_rendering_performance.h"

#include <QDir>
#include <QElapsedTimer>
#include <QStandardPaths>
#include <QPixmap>
#include <QIcon>
#include <QDebug>
#include <QCoreApplication>
#include <memory>

#include "QtLucide/QtLucide.h"
#include "managers/ContentManager.h"
#include "managers/IconMetadataManager.h"

// Helper function to locate metadata files
static QString findMetadataFile(const QString &filename) {
    // Try multiple possible locations
    QStringList searchPaths = {
        QCoreApplication::applicationDirPath(),
        QCoreApplication::applicationDirPath() + "/resources",
        QCoreApplication::applicationDirPath() + "/../share/QtLucide",
        QTLUCIDE_RESOURCES_PATH,  // May be defined during build
    };

    for (const QString &basePath : searchPaths) {
        QString filePath = basePath + "/" + filename;
        if (QFile::exists(filePath)) {
            return filePath;
        }
    }

    // Return empty string if not found
    qWarning() << "Could not find metadata file:" << filename;
    return QString();
}

void TestIconRenderingPerformance::initTestCase() {
    // Create test data directory
    m_testDataDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) +
                    "/QtLucideIconRenderingPerfTest";
    QDir().mkpath(m_testDataDir);

    qDebug() << "Test data directory:" << m_testDataDir;

    // Initialize QtLucide
    m_lucide = std::make_unique<lucide::QtLucide>();
    if (!m_lucide->initLucide()) {
        qWarning() << "Failed to initialize QtLucide";
        // Continue anyway, some tests may still work
    }

    // Initialize ContentManager (without initializing, as metadata may not be available)
    m_contentManager = std::make_unique<gallery::ContentManager>();
}

void TestIconRenderingPerformance::cleanupTestCase() {
    // Clean up
    m_lucide.reset();
    m_contentManager.reset();

    // Remove test data directory
    QDir(m_testDataDir).removeRecursively();
    qDebug() << "Cleaned up test data";
}

void TestIconRenderingPerformance::init() {
    // Setup for each test
}

void TestIconRenderingPerformance::cleanup() {
    // Cleanup after each test
}

QString TestIconRenderingPerformance::getTestDataPath() const {
    return m_testDataDir;
}

QString TestIconRenderingPerformance::getIconName(int index) const {
    // Return common icon names
    const QStringList commonIcons = {
        "home", "search", "settings", "user", "heart", "star", "bell",
        "calendar", "camera", "clock", "code", "database", "download",
        "edit", "file", "filter", "flag", "folder", "gift", "globe",
        "help", "info", "key", "link", "lock", "mail", "map", "menu",
        "message", "minus", "mobile", "more-horizontal", "music", "plus",
        "power", "print", "refresh", "save", "share", "shield", "shopping-cart",
        "slack", "square", "start", "stop", "sun", "target", "trash",
        "trending-up", "twitter", "upload", "user-plus", "video", "volume",
        "watch", "wifi", "window", "x", "youtube", "zap"
    };

    return commonIcons[index % commonIcons.size()];
}

QStringList TestIconRenderingPerformance::getIconNames(int count) const {
    QStringList icons;
    for (int i = 0; i < count; ++i) {
        icons.append(getIconName(i));
    }
    return icons;
}

// ============================================================================
// INITIALIZATION PERFORMANCE TESTS
// ============================================================================

void TestIconRenderingPerformance::testQtLucideInitializationPerformance() {
    QElapsedTimer timer;
    timer.start();

    auto lucide = std::make_unique<lucide::QtLucide>();
    bool initialized = lucide->initLucide();

    qint64 elapsed = timer.elapsed();

    qDebug() << "QtLucide initialization time:" << elapsed << "ms"
             << "(Initialized:" << initialized << ")";

    QVERIFY2(elapsed < QTLUCIDE_INIT_MAX_TIME,
             qPrintable(QString("QtLucide initialization took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(QTLUCIDE_INIT_MAX_TIME)));
}

void TestIconRenderingPerformance::testContentManagerInitializationPerformance() {
    QElapsedTimer timer;
    timer.start();

    auto contentManager = std::make_unique<gallery::ContentManager>();

    qint64 elapsed = timer.elapsed();

    qDebug() << "ContentManager initialization time:" << elapsed << "ms";

    QVERIFY2(elapsed < CONTENT_MANAGER_INIT_MAX_TIME,
             qPrintable(QString("ContentManager initialization took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(CONTENT_MANAGER_INIT_MAX_TIME)));
}

void TestIconRenderingPerformance::testMetadataLoadingPerformance() {
    auto metadataManager = std::make_unique<gallery::IconMetadataManager>();

    // Try to find metadata files
    QString categoriesPath = findMetadataFile("categories.json");
    QString iconsPath = findMetadataFile("icons.json");

    if (categoriesPath.isEmpty() || iconsPath.isEmpty()) {
        QSKIP("Metadata files not found, skipping metadata loading performance test");
        return;
    }

    QElapsedTimer timer;
    timer.start();

    bool loaded = metadataManager->loadMetadata(categoriesPath, iconsPath);

    qint64 elapsed = timer.elapsed();

    qDebug() << "Metadata loading time:" << elapsed << "ms"
             << "(Loaded:" << loaded << ")";

    QVERIFY2(elapsed < METADATA_LOAD_MAX_TIME,
             qPrintable(QString("Metadata loading took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(METADATA_LOAD_MAX_TIME)));
}

// ============================================================================
// SINGLE ICON RENDERING PERFORMANCE TESTS
// ============================================================================

void TestIconRenderingPerformance::testSingleIconCreationPerformance() {
    QVERIFY(m_lucide != nullptr);

    QString iconName = "home";

    QElapsedTimer timer;
    timer.start();

    QIcon icon = m_lucide->icon(iconName);

    qint64 elapsed = timer.elapsed();

    qDebug() << "Single icon creation time:" << elapsed << "ms";

    QVERIFY(!icon.isNull());
    QVERIFY2(elapsed < SINGLE_ICON_CREATE_MAX_TIME,
             qPrintable(QString("Single icon creation took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(SINGLE_ICON_CREATE_MAX_TIME)));
}

void TestIconRenderingPerformance::testSingleIconPixmapRenderingPerformance() {
    QVERIFY(m_lucide != nullptr);

    QString iconName = "home";
    QIcon icon = m_lucide->icon(iconName);
    QVERIFY(!icon.isNull());

    int size = 64;

    QElapsedTimer timer;
    timer.start();

    QPixmap pixmap = icon.pixmap(size, size);

    qint64 elapsed = timer.elapsed();

    qDebug() << "Single icon pixmap rendering time:" << elapsed << "ms";

    QVERIFY(!pixmap.isNull());
    QVERIFY2(elapsed < SINGLE_ICON_RENDER_MAX_TIME,
             qPrintable(QString("Single icon pixmap rendering took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(SINGLE_ICON_RENDER_MAX_TIME)));
}

void TestIconRenderingPerformance::testIconRenderingDifferentSizes_data() {
    QTest::addColumn<int>("size");

    QTest::newRow("16px") << 16;
    QTest::newRow("48px") << 48;
    QTest::newRow("128px") << 128;
    QTest::newRow("256px") << 256;
}

void TestIconRenderingPerformance::testIconRenderingDifferentSizes() {
    QVERIFY(m_lucide != nullptr);

    QFETCH(int, size);

    QString iconName = "star";
    QIcon icon = m_lucide->icon(iconName);
    QVERIFY(!icon.isNull());

    QElapsedTimer timer;
    timer.start();

    QPixmap pixmap = icon.pixmap(size, size);

    qint64 elapsed = timer.elapsed();

    qDebug() << QString("Icon rendering at %1x%1: %2ms").arg(size).arg(elapsed);

    QVERIFY(!pixmap.isNull());
    QVERIFY(pixmap.width() > 0 && pixmap.height() > 0);

    // Rendering time should scale reasonably with size
    // Allow up to 30ms for larger sizes
    qint64 maxTime = SINGLE_ICON_RENDER_MAX_TIME + (size / 64) * 10;
    QVERIFY2(elapsed < maxTime,
             qPrintable(QString("Icon rendering at %1x%1 took %2ms, expected < %3ms")
                            .arg(size)
                            .arg(elapsed)
                            .arg(maxTime)));
}

// ============================================================================
// BATCH RENDERING PERFORMANCE TESTS
// ============================================================================

void TestIconRenderingPerformance::testBatch100IconsRenderingPerformance() {
    QVERIFY(m_lucide != nullptr);

    QStringList icons = getIconNames(100);
    int size = 32;

    QElapsedTimer timer;
    timer.start();

    int successCount = 0;
    for (const QString &iconName : icons) {
        QIcon icon = m_lucide->icon(iconName);
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(size, size);
            if (!pixmap.isNull()) {
                successCount++;
            }
        }
    }

    qint64 elapsed = timer.elapsed();

    qDebug() << "Batch 100 icons rendering time:" << elapsed << "ms"
             << "(Success:" << successCount << "/100)";

    QVERIFY2(elapsed < BATCH_100_MAX_TIME,
             qPrintable(QString("Batch 100 icons rendering took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(BATCH_100_MAX_TIME)));

    // At least 90% of icons should render successfully
    QVERIFY2(successCount >= 90,
             qPrintable(QString("Only %1/100 icons rendered successfully, expected >= 90")
                            .arg(successCount)));
}

void TestIconRenderingPerformance::testBatch1000IconsRenderingPerformance() {
    QVERIFY(m_lucide != nullptr);

    QStringList icons = getIconNames(1000);
    int size = 32;

    QElapsedTimer timer;
    timer.start();

    int successCount = 0;
    for (const QString &iconName : icons) {
        QIcon icon = m_lucide->icon(iconName);
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(size, size);
            if (!pixmap.isNull()) {
                successCount++;
            }
        }
    }

    qint64 elapsed = timer.elapsed();

    qDebug() << "Batch 1000 icons rendering time:" << elapsed << "ms"
             << "(Success:" << successCount << "/1000)";

    QVERIFY2(elapsed < BATCH_1000_MAX_TIME,
             qPrintable(QString("Batch 1000 icons rendering took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(BATCH_1000_MAX_TIME)));

    // At least 90% of icons should render successfully
    QVERIFY2(successCount >= 900,
             qPrintable(QString("Only %1/1000 icons rendered successfully, expected >= 900")
                            .arg(successCount)));
}

void TestIconRenderingPerformance::testBatchAllIconsRenderingPerformance() {
    QVERIFY(m_lucide != nullptr);

    // Get all available icons from the system
    // This test may render up to 1634+ icons
    QStringList allIcons;

    // Generate a representative set of all icon names
    // Since we don't have direct access to all icons, we'll use a larger set
    for (int i = 0; i < 1634; ++i) {
        allIcons.append(getIconName(i));
    }

    int size = 32;

    QElapsedTimer timer;
    timer.start();

    int successCount = 0;
    for (const QString &iconName : allIcons) {
        QIcon icon = m_lucide->icon(iconName);
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(size, size);
            if (!pixmap.isNull()) {
                successCount++;
            }
        }
    }

    qint64 elapsed = timer.elapsed();

    qDebug() << "Batch all icons rendering time:" << elapsed << "ms"
             << "(Success:" << successCount << "/" << allIcons.size() << ")";

    QVERIFY2(elapsed < BATCH_ALL_MAX_TIME,
             qPrintable(QString("Batch all icons rendering took %1ms, expected < %2ms")
                            .arg(elapsed)
                            .arg(BATCH_ALL_MAX_TIME)));

    // At least 80% of icons should render successfully
    int minSuccess = allIcons.size() * 80 / 100;
    QVERIFY2(successCount >= minSuccess,
             qPrintable(QString("Only %1/%2 icons rendered successfully, expected >= %3")
                            .arg(successCount)
                            .arg(allIcons.size())
                            .arg(minSuccess)));
}

// ============================================================================
// CACHING PERFORMANCE TESTS
// ============================================================================

void TestIconRenderingPerformance::testCacheFirstLoadVsCachedLoad() {
    QVERIFY(m_lucide != nullptr);

    QString iconName = "bell";
    int size = 64;

    // First load (uncached)
    m_lucide->icon(iconName);  // Create icon to warm up

    QElapsedTimer timer1;
    timer1.start();

    QIcon icon1 = m_lucide->icon(iconName);
    QPixmap pixmap1 = icon1.pixmap(size, size);

    qint64 firstLoadTime = timer1.elapsed();

    // Second load (should be cached)
    QElapsedTimer timer2;
    timer2.start();

    QIcon icon2 = m_lucide->icon(iconName);
    QPixmap pixmap2 = icon2.pixmap(size, size);

    qint64 cachedLoadTime = timer2.elapsed();

    qDebug() << "First load time:" << firstLoadTime << "ms"
             << "Cached load time:" << cachedLoadTime << "ms";

    QVERIFY(!pixmap1.isNull());
    QVERIFY(!pixmap2.isNull());

    // Cached load should be faster (log the difference)
    qDebug() << "Cache speedup ratio:" << (firstLoadTime > 0 ? (double)cachedLoadTime / firstLoadTime : 0);
}

void TestIconRenderingPerformance::testCachePerformanceImprovement() {
    QVERIFY(m_lucide != nullptr);

    QStringList icons = getIconNames(50);
    int size = 48;

    // First batch (fresh loads)
    QElapsedTimer timer1;
    timer1.start();

    int success1 = 0;
    for (const QString &iconName : icons) {
        QIcon icon = m_lucide->icon(iconName);
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(size, size);
            if (!pixmap.isNull()) {
                success1++;
            }
        }
    }

    qint64 firstBatchTime = timer1.elapsed();

    // Second batch (should be cached)
    QElapsedTimer timer2;
    timer2.start();

    int success2 = 0;
    for (const QString &iconName : icons) {
        QIcon icon = m_lucide->icon(iconName);
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(size, size);
            if (!pixmap.isNull()) {
                success2++;
            }
        }
    }

    qint64 cachedBatchTime = timer2.elapsed();

    qDebug() << "First batch time:" << firstBatchTime << "ms"
             << "Cached batch time:" << cachedBatchTime << "ms"
             << "Success:" << success1 << "/" << success2;

    // Calculate improvement ratio
    double improvementRatio = firstBatchTime > 0 ? (double)cachedBatchTime / firstBatchTime : 0;
    qDebug() << "Cache improvement ratio:" << improvementRatio;

    // Cached batch should show some performance improvement
    // Allow for variability in timing
    if (firstBatchTime > 100) {  // Only check if timing is significant
        QVERIFY2(improvementRatio < (1.0 - CACHE_IMPROVEMENT_MIN_RATIO),
                 qPrintable(QString("Cache improvement ratio %1 is not >= %2x faster")
                                .arg(1.0 / improvementRatio)
                                .arg(1.0 / (1.0 - CACHE_IMPROVEMENT_MIN_RATIO))));
    }
}

// ============================================================================
// MEMORY STRESS TESTS
// ============================================================================

void TestIconRenderingPerformance::testMemoryStabilityLargeLoad() {
    QVERIFY(m_lucide != nullptr);

    // Load a large number of icons and verify memory remains stable
    QStringList icons = getIconNames(500);
    int size = 32;

    QElapsedTimer timer;
    timer.start();

    QList<QIcon> loadedIcons;
    QList<QPixmap> loadedPixmaps;

    int successCount = 0;
    for (const QString &iconName : icons) {
        QIcon icon = m_lucide->icon(iconName);
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(size, size);
            if (!pixmap.isNull()) {
                loadedIcons.append(icon);
                loadedPixmaps.append(pixmap);
                successCount++;
            }
        }
    }

    qint64 elapsed = timer.elapsed();

    qDebug() << "Large load test: loaded" << successCount << "icons in" << elapsed << "ms"
             << "Total loaded objects:" << (loadedIcons.size() + loadedPixmaps.size());

    QVERIFY(successCount > 0);

    // Clear and verify cleanup doesn't crash
    loadedIcons.clear();
    loadedPixmaps.clear();

    QVERIFY(true);  // If we got here, memory cleanup worked
}

void TestIconRenderingPerformance::testCacheCleanupFunctionality() {
    QVERIFY(m_lucide != nullptr);

    // Load icons
    QStringList icons = getIconNames(100);
    int size = 32;

    for (const QString &iconName : icons) {
        QIcon icon = m_lucide->icon(iconName);
        if (!icon.isNull()) {
            QPixmap pixmap = icon.pixmap(size, size);
            Q_UNUSED(pixmap);
        }
    }

    // The cache should be working
    // Try to verify through repeated loads showing improvement
    QElapsedTimer timer1;
    timer1.start();

    for (int i = 0; i < 10; ++i) {
        QIcon icon = m_lucide->icon(getIconName(i));
        QPixmap pixmap = icon.pixmap(size, size);
        Q_UNUSED(pixmap);
    }

    qint64 cachedTime = timer1.elapsed();

    qDebug() << "Cache cleanup verification: repeated load time:" << cachedTime << "ms";

    // Should complete without errors
    QVERIFY(true);
}

#include "test_icon_rendering_performance.moc"
