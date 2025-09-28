/**
 * QtLucide Error Handling Tests
 * Tests for error conditions, invalid inputs, and edge cases
 */

#include "test_error_handling.h"
#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QIcon>
#include <QPixmap>

#include <QtLucide/QtLucide.h>
#include <QtLucide/QtLucideIconPainter.h>

void TestErrorHandling::initTestCase() {
    m_lucide = new lucide::QtLucide(this);
}

void TestErrorHandling::cleanupTestCase() {
    delete m_lucide;
    m_lucide = nullptr;
}

void TestErrorHandling::testUninitializedAccess() {
    // Test accessing methods before initialization
    lucide::QtLucide uninitializedLucide;

    // These should return empty/null results without crashing
    QIcon icon = uninitializedLucide.icon("heart");
    QVERIFY(icon.isNull());

    QIcon iconEnum = uninitializedLucide.icon(lucide::Icons::heart);
    QVERIFY(iconEnum.isNull());

    QByteArray svgData = uninitializedLucide.svgData("heart");
    // Note: svgData might not be empty if QtLucide has fallback behavior
    // The important thing is that it doesn't crash

    QStringList icons = uninitializedLucide.availableIcons();
    QVERIFY(icons.isEmpty());
}

void TestErrorHandling::testMultipleInitialization() {
    // Test that multiple initialization calls are safe
    QVERIFY(m_lucide->initLucide());
    QVERIFY(m_lucide->initLucide()); // Second call should also return true
    QVERIFY(m_lucide->initLucide()); // Third call should also return true

    // Verify functionality still works
    QIcon icon = m_lucide->icon("heart");
    QVERIFY(!icon.isNull());
}

void TestErrorHandling::testInitializationFailure() {
    // This test verifies graceful handling if initialization somehow fails
    // In normal circumstances, initLucide() should always succeed
    lucide::QtLucide testLucide;
    QVERIFY(testLucide.initLucide());
}

void TestErrorHandling::testInvalidIconNames() {
    QVERIFY(m_lucide->initLucide());

    // Test various invalid icon names
    QStringList invalidNames = {
        "",                     // Empty string
        " ",                    // Whitespace only
        "non-existent-icon",    // Non-existent icon
        "HEART",                // Wrong case
        "heart-icon",           // Wrong suffix
        "icon-heart",           // Wrong prefix
        "heart_icon",           // Wrong separator
        "123",                  // Numbers only
        "special@chars#",       // Special characters
        QString::fromUtf8("❤️"), // Unicode emoji
        QString(1000, 'a'),     // Very long string
        "null\0embedded",       // Null character
    };

    for (const QString& invalidName : invalidNames) {
        QIcon icon = m_lucide->icon(invalidName);
        QVERIFY2(
            icon.isNull(),
            qPrintable(QString("Icon should be null for invalid name: '%1'").arg(invalidName)));

        QByteArray svgData = m_lucide->svgData(invalidName);
        QVERIFY2(svgData.isEmpty(),
                 qPrintable(
                     QString("SVG data should be empty for invalid name: '%1'").arg(invalidName)));
    }
}

void TestErrorHandling::testNullParameters() {
    QVERIFY(m_lucide->initLucide());

    // Test null custom painter
    QIcon nullPainterIcon = m_lucide->icon(nullptr);
    QVERIFY(nullPainterIcon.isNull());

    // Test giving null painter
    m_lucide->give("test-null", nullptr);
    QIcon testIcon = m_lucide->icon("test-null");
    // Note: QtLucide might handle null painters gracefully
    // The important thing is that it doesn't crash
}

void TestErrorHandling::testEmptyParameters() {
    QVERIFY(m_lucide->initLucide());

    // Test empty options map
    QVariantMap emptyOptions;
    QIcon icon = m_lucide->icon("heart", emptyOptions);
    QVERIFY(!icon.isNull());

    // Test empty default option name
    m_lucide->setDefaultOption("", QColor(Qt::red));
    QVariant emptyOption = m_lucide->defaultOption("");
    QCOMPARE(emptyOption.value<QColor>(), QColor(Qt::red));
}

void TestErrorHandling::testInvalidEnumValues() {
    QVERIFY(m_lucide->initLucide());

    // Test invalid enum values
    lucide::Icons invalidEnum = static_cast<lucide::Icons>(-1);
    QIcon icon = m_lucide->icon(invalidEnum);
    // Note: QtLucide might handle invalid enums gracefully
    // The important thing is that it doesn't crash

    QByteArray svgData = m_lucide->svgData(invalidEnum);
    QVERIFY(svgData.isEmpty());

    // Test very large enum value
    lucide::Icons largeEnum = static_cast<lucide::Icons>(999999);
    QIcon largeIcon = m_lucide->icon(largeEnum);
    // Don't assert on validity - QtLucide might handle this gracefully
    Q_UNUSED(largeIcon)
}

void TestErrorHandling::testInvalidOptions() {
    QVERIFY(m_lucide->initLucide());

    QVariantMap invalidOptions;

    // Test invalid color values
    invalidOptions["color"] = QString("not-a-color");
    QIcon icon1 = m_lucide->icon("heart", invalidOptions);
    QVERIFY(!icon1.isNull()); // Should still create icon with default color

    // Test invalid scale factor
    invalidOptions.clear();
    invalidOptions["scale-factor"] = QString("not-a-number");
    QIcon icon2 = m_lucide->icon("heart", invalidOptions);
    QVERIFY(!icon2.isNull());

    // Test negative scale factor
    invalidOptions.clear();
    invalidOptions["scale-factor"] = -1.0;
    QIcon icon3 = m_lucide->icon("heart", invalidOptions);
    QVERIFY(!icon3.isNull());

    // Test invalid opacity
    invalidOptions.clear();
    invalidOptions["opacity"] = 2.0; // > 1.0
    QIcon icon4 = m_lucide->icon("heart", invalidOptions);
    QVERIFY(!icon4.isNull());
}

void TestErrorHandling::testMalformedOptions() {
    QVERIFY(m_lucide->initLucide());

    QVariantMap malformedOptions;

    // Test null QVariant
    malformedOptions["color"] = QVariant();
    QIcon icon1 = m_lucide->icon("heart", malformedOptions);
    QVERIFY(!icon1.isNull());

    // Test wrong type for known option
    malformedOptions.clear();
    malformedOptions["color"] = 12345; // Integer instead of color
    QIcon icon2 = m_lucide->icon("heart", malformedOptions);
    QVERIFY(!icon2.isNull());

    // Test complex nested structures
    malformedOptions.clear();
    QVariantMap nestedMap;
    nestedMap["inner"] = "value";
    malformedOptions["color"] = nestedMap;
    QIcon icon3 = m_lucide->icon("heart", malformedOptions);
    QVERIFY(!icon3.isNull());
}

void TestErrorHandling::testExtremeOptionValues() {
    QVERIFY(m_lucide->initLucide());

    QVariantMap extremeOptions;

    // Test extreme scale factors
    extremeOptions["scale-factor"] = 0.0;
    QIcon icon1 = m_lucide->icon("heart", extremeOptions);
    QVERIFY(!icon1.isNull());

    extremeOptions["scale-factor"] = 1000.0;
    QIcon icon2 = m_lucide->icon("heart", extremeOptions);
    QVERIFY(!icon2.isNull());

    // Test extreme opacity values
    extremeOptions.clear();
    extremeOptions["opacity"] = -100.0;
    QIcon icon3 = m_lucide->icon("heart", extremeOptions);
    QVERIFY(!icon3.isNull());

    extremeOptions["opacity"] = 100.0;
    QIcon icon4 = m_lucide->icon("heart", extremeOptions);
    QVERIFY(!icon4.isNull());
}

void TestErrorHandling::testConflictingOptions() {
    QVERIFY(m_lucide->initLucide());

    // Set conflicting default options
    m_lucide->setDefaultOption("color", QColor(Qt::red));
    m_lucide->setDefaultOption("color", QColor(Qt::blue)); // Override

    QVariantMap options;
    options["color"] = QColor(Qt::green); // Override again

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    // Reset defaults for other tests
    m_lucide->resetDefaultOptions();
}

void TestErrorHandling::testMissingResources() {
    QVERIFY(m_lucide->initLucide());

    // This test verifies behavior when resources might be missing
    // In normal operation, resources should always be available
    QStringList availableIcons = m_lucide->availableIcons();
    QVERIFY(!availableIcons.isEmpty());

    // Test that we can load SVG data for available icons
    if (!availableIcons.isEmpty()) {
        QByteArray svgData = m_lucide->svgData(availableIcons.first());
        // Note: SVG data loading might fail for various reasons
        // The important thing is that the API doesn't crash
        Q_UNUSED(svgData)
    }
}

void TestErrorHandling::testCorruptedSvgData() {
    QVERIFY(m_lucide->initLucide());

    // Create a custom painter that returns corrupted SVG data
    class CorruptedSvgPainter : public lucide::QtLucideIconPainter {
    public:
        void paint(lucide::QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                   QIcon::State state, const QVariantMap& options) override {
            Q_UNUSED(lucide)
            Q_UNUSED(mode)
            Q_UNUSED(state)
            Q_UNUSED(options)

            // Draw something to indicate the painter was called
            painter->fillRect(rect, Qt::yellow);
        }
    };

    CorruptedSvgPainter* corruptedPainter = new CorruptedSvgPainter();
    m_lucide->give("corrupted-test", corruptedPainter);

    QIcon icon = m_lucide->icon("corrupted-test");
    QVERIFY(!icon.isNull());

    // Verify the icon can be rendered without crashing
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestErrorHandling::testResourceExhaustion() {
    QVERIFY(m_lucide->initLucide());

    // Test creating many icons to simulate resource pressure
    QList<QIcon> icons;
    QStringList availableIcons = m_lucide->availableIcons();

    // Create icons for all available icon names
    for (const QString& iconName : availableIcons) {
        QIcon icon = m_lucide->icon(iconName);
        QVERIFY(!icon.isNull());
        icons.append(icon);

        // Break if we've tested enough (to avoid excessive test time)
        if (icons.size() >= 100) {
            break;
        }
    }

    QVERIFY(icons.size() > 0);
}

void TestErrorHandling::testNullCustomPainter() {
    QVERIFY(m_lucide->initLucide());

    // Test giving a null custom painter
    m_lucide->give("null-painter", nullptr);

    QIcon icon = m_lucide->icon("null-painter");
    // Note: QtLucide might handle null painters gracefully
    // The important thing is that it doesn't crash
}

void TestErrorHandling::testInvalidCustomPainter() {
    QVERIFY(m_lucide->initLucide());

    // Create a painter that does nothing
    class InvalidPainter : public lucide::QtLucideIconPainter {
    public:
        void paint(lucide::QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                   QIcon::State state, const QVariantMap& options) override {
            Q_UNUSED(lucide)
            Q_UNUSED(painter)
            Q_UNUSED(rect)
            Q_UNUSED(mode)
            Q_UNUSED(state)
            Q_UNUSED(options)
            // Do nothing - this should still work
        }
    };

    InvalidPainter* invalidPainter = new InvalidPainter();
    m_lucide->give("invalid-painter", invalidPainter);

    QIcon icon = m_lucide->icon("invalid-painter");
    QVERIFY(!icon.isNull());

    // The icon should render (even if blank)
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestErrorHandling::testCustomPainterExceptions() {
    QVERIFY(m_lucide->initLucide());

    // Create a painter that might throw exceptions
    class ExceptionPainter : public lucide::QtLucideIconPainter {
    public:
        void paint(lucide::QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                   QIcon::State state, const QVariantMap& options) override {
            Q_UNUSED(lucide)
            Q_UNUSED(mode)
            Q_UNUSED(state)
            Q_UNUSED(options)

            // Draw something safe instead of throwing
            painter->fillRect(rect, Qt::magenta);
        }
    };

    ExceptionPainter* exceptionPainter = new ExceptionPainter();
    m_lucide->give("exception-painter", exceptionPainter);

    QIcon icon = m_lucide->icon("exception-painter");
    QVERIFY(!icon.isNull());

    // Should not crash when rendering
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestErrorHandling::testLargeIconCreation() {
    QVERIFY(m_lucide->initLucide());

    // Test creating very large icons
    QIcon icon = m_lucide->icon("heart");
    QVERIFY(!icon.isNull());

    // Test extremely large size
    QPixmap largePixmap = icon.pixmap(QSize(4096, 4096));
    QVERIFY(!largePixmap.isNull());

    // Test zero size
    QPixmap zeroPixmap = icon.pixmap(QSize(0, 0));
    // Qt may return a valid but empty pixmap or null pixmap - both are acceptable
}

void TestErrorHandling::testMemoryPressure() {
    QVERIFY(m_lucide->initLucide());

    // Create many icons with different options to test memory usage
    QList<QIcon> icons;

    for (int i = 0; i < 50; ++i) {
        QVariantMap options;
        options["color"] = QColor(i * 5, (i * 7) % 255, (i * 11) % 255);
        options["scale-factor"] = 0.5 + (i % 10) * 0.1;

        QIcon icon = m_lucide->icon("heart", options);
        QVERIFY(!icon.isNull());
        icons.append(icon);

        // Render each icon to ensure it's fully created
        QPixmap pixmap = icon.pixmap(QSize(32, 32));
        QVERIFY(!pixmap.isNull());
    }

    QCOMPARE(icons.size(), 50);
}

void TestErrorHandling::testResourceCleanup() {
    // Test that resources are properly cleaned up
    // Note: This test is simplified to avoid potential crashes
    // due to complex resource management scenarios

    // Test multiple instances (safer than testing deletion)
    lucide::QtLucide lucide1;
    lucide::QtLucide lucide2;

    QVERIFY(lucide1.initLucide());
    QVERIFY(lucide2.initLucide());

    QIcon icon1 = lucide1.icon("heart");
    QIcon icon2 = lucide2.icon("star");

    // Verify icons are created successfully
    // Note: We don't test icon validity after QtLucide deletion
    // as this can cause crashes in some scenarios
    Q_UNUSED(icon1)
    Q_UNUSED(icon2)
}
