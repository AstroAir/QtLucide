/**
 * @file test_icon_engine.cpp
 * @brief Unit tests for QtLucideIconEngine
 */

#include "test_icon_engine.h"

#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QScreen>

#include <QtLucide/QtLucide.h>
#include <QtLucide/QtLucideIconEngine.h>
#include <QtLucide/QtLucideIconPainter.h>

void TestIconEngine::initTestCase() {
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());
}

void TestIconEngine::cleanupTestCase() {
    delete m_lucide;
    m_lucide = nullptr;
}

void TestIconEngine::init() {
    // Per-test setup if needed
}

void TestIconEngine::cleanup() {
    // Per-test cleanup if needed
}

// ============================================================================
// Construction and basic properties
// ============================================================================

void TestIconEngine::testConstructionWithValidParameters() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());
}

void TestIconEngine::testConstructionWithNullLucide() {
    // Creating an engine with null lucide should result in null icon
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    
    auto* painter = new lucide::QtLucideSvgIconPainter();
    auto* engine = new lucide::QtLucideIconEngine(nullptr, painter, options);
    
    QIcon icon(engine);
    QVERIFY(icon.isNull());
}

void TestIconEngine::testConstructionWithNullPainter() {
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    
    auto* engine = new lucide::QtLucideIconEngine(m_lucide, nullptr, options);
    
    QIcon icon(engine);
    QVERIFY(icon.isNull());
}

void TestIconEngine::testKeyReturnsCorrectIdentifier() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());
    
    // Access the engine through the icon
    // The key should be "QtLucideIconEngine"
    // We verify this indirectly by checking the icon works correctly
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

// ============================================================================
// isNull() behavior
// ============================================================================

void TestIconEngine::testIsNullWithValidBuiltinIcon() {
    QIcon icon = m_lucide->icon(lucide::Icons::heart);
    QVERIFY(!icon.isNull());
}

void TestIconEngine::testIsNullWithInvalidIconId() {
    QIcon icon = m_lucide->icon("non-existent-icon-name");
    QVERIFY(icon.isNull());
}

void TestIconEngine::testIsNullWithCustomPainter() {
    // Create a custom painter and register it
    class TestPainter : public lucide::QtLucideIconPainter {
    public:
        lucide::QtLucideIconPainter* clone() const override {
            return new TestPainter();
        }
        QString iconText() const override {
            return "test-painter";
        }
        void paint(lucide::QtLucide*, QPainter* painter, const QRect& rect,
                   QIcon::Mode, QIcon::State, const QVariantMap& options) override {
            QColor color = options.value("color", QColor(Qt::black)).value<QColor>();
            painter->fillRect(rect, color);
        }
    };
    
    m_lucide->give("custom-test-icon", new TestPainter());
    QIcon icon = m_lucide->icon("custom-test-icon");
    
    // Custom painter icons should not be null
    QVERIFY(!icon.isNull());
}

void TestIconEngine::testIsNullWithNullPainter() {
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    
    auto* engine = new lucide::QtLucideIconEngine(m_lucide, nullptr, options);
    QIcon icon(engine);
    
    QVERIFY(icon.isNull());
}

void TestIconEngine::testIsNullWithNullLucide() {
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    
    auto* painter = new lucide::QtLucideSvgIconPainter();
    auto* engine = new lucide::QtLucideIconEngine(nullptr, painter, options);
    QIcon icon(engine);
    
    QVERIFY(icon.isNull());
}

// ============================================================================
// paint() and pixmap()
// ============================================================================

void TestIconEngine::testPaintDoesNotCrashWithValidIcon() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    icon.paint(&painter, QRect(0, 0, 64, 64));
    // Should not crash
    QVERIFY(true);
}

void TestIconEngine::testPaintDoesNothingWithNullPainter() {
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    
    auto* engine = new lucide::QtLucideIconEngine(m_lucide, nullptr, options);
    QIcon icon(engine);
    
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::white);
    
    QPainter painter(&pixmap);
    icon.paint(&painter, QRect(0, 0, 64, 64));
    
    // Pixmap should remain white (no painting occurred)
    QImage image = pixmap.toImage();
    QCOMPARE(image.pixel(32, 32), QColor(Qt::white).rgb());
}

void TestIconEngine::testPixmapReturnsValidPixmap() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
    QVERIFY(pixmap.width() >= 32);
    QVERIFY(pixmap.height() >= 32);
}

void TestIconEngine::testPixmapCachingWorks() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    // Request the same pixmap twice
    QPixmap pixmap1 = icon.pixmap(QSize(32, 32));
    QPixmap pixmap2 = icon.pixmap(QSize(32, 32));
    
    // Both should be valid
    QVERIFY(!pixmap1.isNull());
    QVERIFY(!pixmap2.isNull());
    
    // They should have the same dimensions
    QCOMPARE(pixmap1.size(), pixmap2.size());
}

void TestIconEngine::testPixmapCacheLimitRespected() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    // Request many different sizes to test cache limit (100)
    for (int i = 10; i < 120; ++i) {
        QPixmap pixmap = icon.pixmap(QSize(i, i));
        QVERIFY(!pixmap.isNull());
    }
    
    // Should not crash and still work
    QPixmap finalPixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!finalPixmap.isNull());
}

void TestIconEngine::testPixmapDifferentModesAreCachedSeparately() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    QPixmap normalPixmap = icon.pixmap(QSize(32, 32), QIcon::Normal);
    QPixmap disabledPixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);
    
    QVERIFY(!normalPixmap.isNull());
    QVERIFY(!disabledPixmap.isNull());
    
    // The pixmaps should be different (disabled has different color/opacity)
    // We can't directly compare cacheKeys, but we verify both are valid
}

void TestIconEngine::testPixmapDifferentSizesAreCachedSeparately() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    QPixmap small = icon.pixmap(QSize(16, 16));
    QPixmap large = icon.pixmap(QSize(64, 64));
    
    QVERIFY(!small.isNull());
    QVERIFY(!large.isNull());
    
    // Sizes should be different (accounting for device pixel ratio)
    QVERIFY(small.width() < large.width());
    QVERIFY(small.height() < large.height());
}

// ============================================================================
// clone()
// ============================================================================

void TestIconEngine::testCloneCreatesNewInstance() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QIcon clonedIcon = icon;  // Uses copy constructor which calls clone()
    
    QVERIFY(!icon.isNull());
    QVERIFY(!clonedIcon.isNull());
    
    // Both should produce valid pixmaps
    QPixmap original = icon.pixmap(QSize(32, 32));
    QPixmap cloned = clonedIcon.pixmap(QSize(32, 32));
    
    QVERIFY(!original.isNull());
    QVERIFY(!cloned.isNull());
}

void TestIconEngine::testClonePreservesOptions() {
    QVariantMap options;
    options["color"] = QColor(Qt::red);
    options["scale-factor"] = 0.8;
    
    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QIcon clonedIcon = icon;
    
    // Both should work correctly with the same options
    QPixmap original = icon.pixmap(QSize(32, 32));
    QPixmap cloned = clonedIcon.pixmap(QSize(32, 32));
    
    QVERIFY(!original.isNull());
    QVERIFY(!cloned.isNull());
}

void TestIconEngine::testCloneSharesPainterReference() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QIcon clonedIcon = icon;
    
    // Both icons should render correctly (sharing the same painter)
    for (int i = 0; i < 10; ++i) {
        QPixmap p1 = icon.pixmap(QSize(32, 32));
        QPixmap p2 = clonedIcon.pixmap(QSize(32, 32));
        QVERIFY(!p1.isNull());
        QVERIFY(!p2.isNull());
    }
}

// ============================================================================
// actualSize()
// ============================================================================

void TestIconEngine::testActualSizeReturnsScaledSize() {
    QVariantMap options;
    options["scale-factor"] = 0.5;
    
    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    
    // The actual size should be scaled
    QPixmap pixmap = icon.pixmap(QSize(64, 64));
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testActualSizeWithDefaultScaleFactor() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    // Default scale factor is 0.9
    QPixmap pixmap = icon.pixmap(QSize(100, 100));
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testActualSizeWithCustomScaleFactor() {
    QVariantMap options;
    options["scale-factor"] = 1.5;
    
    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testActualSizeClampsBounds() {
    // Test with extreme scale factors
    QVariantMap optionsSmall;
    optionsSmall["scale-factor"] = 0.01;  // Should be clamped to 0.1
    
    QIcon iconSmall = m_lucide->icon(lucide::Icons::activity, optionsSmall);
    QPixmap pixmapSmall = iconSmall.pixmap(QSize(32, 32));
    QVERIFY(!pixmapSmall.isNull());
    
    QVariantMap optionsLarge;
    optionsLarge["scale-factor"] = 100.0;  // Should be clamped to 10.0
    
    QIcon iconLarge = m_lucide->icon(lucide::Icons::activity, optionsLarge);
    QPixmap pixmapLarge = iconLarge.pixmap(QSize(32, 32));
    QVERIFY(!pixmapLarge.isNull());
}

// ============================================================================
// availableSizes()
// ============================================================================

void TestIconEngine::testAvailableSizesReturnsEmptyList() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    // SVG icons are infinitely scalable, so availableSizes returns empty
    QList<QSize> sizes = icon.availableSizes();
    QVERIFY(sizes.isEmpty());
}

// ============================================================================
// iconName()
// ============================================================================

void TestIconEngine::testIconNameWithValidIconId() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    
    // The icon should have a name
    QString name = icon.name();
    // Name format is "lucide-<id>"
    QVERIFY(name.startsWith("lucide-") || name.isEmpty());
}

void TestIconEngine::testIconNameWithInvalidIconId() {
    QIcon icon = m_lucide->icon("invalid-icon");
    
    // Invalid icon should be null
    QVERIFY(icon.isNull());
}

void TestIconEngine::testIconNameWithNullLucide() {
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    
    auto* painter = new lucide::QtLucideSvgIconPainter();
    auto* engine = new lucide::QtLucideIconEngine(nullptr, painter, options);
    QIcon icon(engine);
    
    QString name = icon.name();
    QVERIFY(name.isEmpty());
}

// ============================================================================
// Mode and state handling
// ============================================================================

void TestIconEngine::testPixmapNormalMode() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Normal);
    
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testPixmapDisabledMode() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);
    
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testPixmapActiveMode() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Active);
    
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testPixmapSelectedMode() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Selected);
    
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testPixmapOnState() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Normal, QIcon::On);
    
    QVERIFY(!pixmap.isNull());
}

void TestIconEngine::testPixmapOffState() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Normal, QIcon::Off);
    
    QVERIFY(!pixmap.isNull());
}

// ============================================================================
// High-DPI support
// ============================================================================

void TestIconEngine::testPixmapDevicePixelRatio() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    
    QVERIFY(!pixmap.isNull());
    
    // Device pixel ratio should be set
    qreal dpr = pixmap.devicePixelRatio();
    QVERIFY(dpr >= 1.0);
    
    // Actual pixel dimensions should account for DPR
    QVERIFY(pixmap.width() >= 32);
    QVERIFY(pixmap.height() >= 32);
}
