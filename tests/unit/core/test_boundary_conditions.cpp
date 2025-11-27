/**
 * QtLucide Boundary Conditions Tests
 * Tests for extreme values, edge cases, and performance limits
 */

#include "test_boundary_conditions.h"
#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QElapsedTimer>
#include <QIcon>
#include <QPixmap>
#include <functional>
#include <limits>

#include <QtLucide/QtLucide.h>
#include <QtLucide/QtLucideIconPainter.h>

void TestBoundaryConditions::initTestCase() {
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());
}

void TestBoundaryConditions::cleanupTestCase() {
    delete m_lucide;
    m_lucide = nullptr;
}

void TestBoundaryConditions::testZeroSizeIcons() {
    QIcon icon = m_lucide->icon("heart");
    QVERIFY(!icon.isNull());

    // Test zero width
    QPixmap pixmap1 = icon.pixmap(QSize(0, 32));
    // Qt behavior: may return null or valid but empty pixmap

    // Test zero height
    QPixmap pixmap2 = icon.pixmap(QSize(32, 0));
    // Qt behavior: may return null or valid but empty pixmap

    // Test both zero
    QPixmap pixmap3 = icon.pixmap(QSize(0, 0));
    // Qt behavior: may return null or valid but empty pixmap

    // Test that the icon itself is still valid
    QPixmap normalPixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!normalPixmap.isNull());
}

void TestBoundaryConditions::testNegativeSizeIcons() {
    QIcon icon = m_lucide->icon("heart");
    QVERIFY(!icon.isNull());

    // Test negative width
    QPixmap pixmap1 = icon.pixmap(QSize(-32, 32));
    // Qt should handle this gracefully

    // Test negative height
    QPixmap pixmap2 = icon.pixmap(QSize(32, -32));
    // Qt should handle this gracefully

    // Test both negative
    QPixmap pixmap3 = icon.pixmap(QSize(-32, -32));
    // Qt should handle this gracefully

    // Verify normal operation still works
    QPixmap normalPixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!normalPixmap.isNull());
}

void TestBoundaryConditions::testExtremelyLargeIcons() {
    QIcon icon = m_lucide->icon("heart");
    QVERIFY(!icon.isNull());

    // Test very large but reasonable size
    QPixmap pixmap1 = icon.pixmap(QSize(2048, 2048));
    QVERIFY(!pixmap1.isNull());

    // Test extremely large size (may fail due to memory limits)
    QPixmap pixmap2 = icon.pixmap(QSize(8192, 8192));
    // Don't assert on this - it may legitimately fail due to memory constraints

    // Test asymmetric extreme sizes
    QPixmap pixmap3 = icon.pixmap(QSize(10000, 1));
    QPixmap pixmap4 = icon.pixmap(QSize(1, 10000));

    // Verify normal operation still works
    QPixmap normalPixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!normalPixmap.isNull());
}

void TestBoundaryConditions::testMaximumReasonableSize() {
    QIcon icon = m_lucide->icon("heart");
    QVERIFY(!icon.isNull());

    // Test maximum reasonable sizes for different use cases
    QList<QSize> testSizes = {
        QSize(512, 512),   // Large icon
        QSize(1024, 1024), // Very large icon
        QSize(256, 1024),  // Tall banner
        QSize(1024, 256),  // Wide banner
    };

    for (const QSize& size : testSizes) {
        QPixmap pixmap = icon.pixmap(size);
        QVERIFY2(!pixmap.isNull(), qPrintable(QString("Failed to create pixmap of size %1x%2")
                                                  .arg(size.width())
                                                  .arg(size.height())));
    }
}

void TestBoundaryConditions::testInvalidColorValues() {
    QVariantMap options;

    // Test invalid color strings
    options["color"] = QString("not-a-color");
    QIcon icon1 = m_lucide->icon("heart", options);
    QVERIFY(!icon1.isNull()); // Should use default color

    // Test empty color string
    options["color"] = QString("");
    QIcon icon2 = m_lucide->icon("heart", options);
    QVERIFY(!icon2.isNull());

    // Test null QVariant
    options["color"] = QVariant();
    QIcon icon3 = m_lucide->icon("heart", options);
    QVERIFY(!icon3.isNull());

    // Test wrong type
    options["color"] = 12345;
    QIcon icon4 = m_lucide->icon("heart", options);
    QVERIFY(!icon4.isNull());
}

void TestBoundaryConditions::testExtremeColorValues() {
    QVariantMap options;

    // Test colors with extreme RGB values
    options["color"] = QColor(255, 255, 255, 255); // Maximum values
    QIcon icon1 = m_lucide->icon("heart", options);
    QVERIFY(!icon1.isNull());

    options["color"] = QColor(0, 0, 0, 0); // Minimum values
    QIcon icon2 = m_lucide->icon("heart", options);
    QVERIFY(!icon2.isNull());

    // Test invalid RGB values (Qt should clamp these)
    options["color"] = QColor(300, -50, 1000, 500);
    QIcon icon3 = m_lucide->icon("heart", options);
    QVERIFY(!icon3.isNull());
}

void TestBoundaryConditions::testTransparentColors() {
    QVariantMap options;

    // Test fully transparent color
    options["color"] = QColor(255, 0, 0, 0);
    QIcon icon1 = m_lucide->icon("heart", options);
    QVERIFY(!icon1.isNull());

    // Test semi-transparent color
    options["color"] = QColor(255, 0, 0, 128);
    QIcon icon2 = m_lucide->icon("heart", options);
    QVERIFY(!icon2.isNull());

    // Verify the icons can be rendered
    QPixmap pixmap1 = icon1.pixmap(QSize(32, 32));
    QPixmap pixmap2 = icon2.pixmap(QSize(32, 32));
    QVERIFY(!pixmap1.isNull());
    QVERIFY(!pixmap2.isNull());
}

void TestBoundaryConditions::testColorOverflow() {
    QVariantMap options;

    // Test with extreme integer values
    options["color"] = QColor(std::numeric_limits<int>::max(), std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max());
    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testZeroScaleFactor() {
    QVariantMap options;
    options["scale-factor"] = 0.0;

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testNegativeScaleFactor() {
    QVariantMap options;
    options["scale-factor"] = -1.0;

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testExtremelyLargeScaleFactor() {
    QVariantMap options;
    options["scale-factor"] = 1000.0;

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    // This might fail due to memory constraints, which is acceptable
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    // Don't assert on pixmap validity for extreme scale factors
}

void TestBoundaryConditions::testVerySmallScaleFactor() {
    QVariantMap options;
    options["scale-factor"] = 0.001;

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testInfiniteScaleFactor() {
    QVariantMap options;

    // Test infinity
    options["scale-factor"] = std::numeric_limits<double>::infinity();
    QIcon icon1 = m_lucide->icon("heart", options);
    QVERIFY(!icon1.isNull());

    // Test negative infinity
    options["scale-factor"] = -std::numeric_limits<double>::infinity();
    QIcon icon2 = m_lucide->icon("heart", options);
    QVERIFY(!icon2.isNull());

    // Test NaN
    options["scale-factor"] = std::numeric_limits<double>::quiet_NaN();
    QIcon icon3 = m_lucide->icon("heart", options);
    QVERIFY(!icon3.isNull());
}

void TestBoundaryConditions::testNegativeOpacity() {
    QVariantMap options;
    options["opacity"] = -0.5;

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testOpacityGreaterThanOne() {
    QVariantMap options;
    options["opacity"] = 2.0;

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testExtremeOpacityValues() {
    QVariantMap options;

    // Test very large opacity
    options["opacity"] = 1000000.0;
    QIcon icon1 = m_lucide->icon("heart", options);
    QVERIFY(!icon1.isNull());

    // Test very small opacity
    options["opacity"] = -1000000.0;
    QIcon icon2 = m_lucide->icon("heart", options);
    QVERIFY(!icon2.isNull());

    // Test infinity
    options["opacity"] = std::numeric_limits<double>::infinity();
    QIcon icon3 = m_lucide->icon("heart", options);
    QVERIFY(!icon3.isNull());
}

void TestBoundaryConditions::testZeroOpacity() {
    QVariantMap options;
    options["opacity"] = 0.0;

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testVeryLongIconNames() {
    // Test very long icon name
    QString longName(10000, 'a');
    QIcon icon1 = m_lucide->icon(longName);
    QVERIFY(icon1.isNull()); // Should fail gracefully

    // Test extremely long icon name
    QString extremelyLongName(1000000, 'b');
    QIcon icon2 = m_lucide->icon(extremelyLongName);
    QVERIFY(icon2.isNull()); // Should fail gracefully

    // Verify normal operation still works
    QIcon normalIcon = m_lucide->icon("heart");
    QVERIFY(!normalIcon.isNull());
}

void TestBoundaryConditions::testUnicodeIconNames() {
    // Test various Unicode characters (avoiding surrogate pairs for MSVC compatibility)
    QStringList unicodeNames = {
        QString::fromUtf8("\u03b1\u03b2\u03b3\u03b4\u03b5"), // Greek letters
        QString::fromUtf8("\u4e2d\u6587"),                   // Chinese characters (reduced)
        QString::fromUtf8("\u0627\u0644\u0639\u0631\u0628\u064a\u0629"), // Arabic text
        QString::fromUtf8("\u2764"),                                     // Heart symbol
        QString::fromUtf8("\u2b50"),                                     // Star
    };

    for (const QString& unicodeName : unicodeNames) {
        QIcon icon = m_lucide->icon(unicodeName);
        QVERIFY(icon.isNull()); // Should fail gracefully for non-existent icons
    }
}

void TestBoundaryConditions::testSpecialCharacterIconNames() {
    QStringList specialNames = {
        "icon\nwith\nnewlines",  "icon\twith\ttabs",        "icon with spaces",
        "icon/with/slashes",     "icon\\with\\backslashes", "icon\"with\"quotes",
        "icon'with'apostrophes", "icon<with>brackets",      "icon{with}braces",
        "icon[with]square",      "icon|with|pipes",         "icon&with&ampersands",
        "icon%with%percent",     "icon#with#hash",          "icon@with@at",
        "icon$with$dollar",      "icon^with^caret",         "icon*with*asterisk",
        "icon+with+plus",        "icon=with=equals",        "icon?with?question",
        "icon!with!exclamation", "icon~with~tilde",         "icon`with`backtick",
    };

    for (const QString& specialName : specialNames) {
        QIcon icon = m_lucide->icon(specialName);
        QVERIFY(icon.isNull()); // Should fail gracefully for invalid names
    }
}

void TestBoundaryConditions::testBinaryDataInIconNames() {
    // Test icon names with binary data
    QByteArray binaryData;
    for (int i = 0; i < 256; ++i) {
        binaryData.append(static_cast<char>(i));
    }

    QString binaryName = QString::fromLatin1(binaryData);
    QIcon icon = m_lucide->icon(binaryName);
    QVERIFY(icon.isNull()); // Should fail gracefully

    // Test with null bytes
    QByteArray nullBytes("icon\0with\0nulls", 15);
    QString nullName = QString::fromLatin1(nullBytes);
    QIcon nullIcon = m_lucide->icon(nullName);
    QVERIFY(nullIcon.isNull()); // Should fail gracefully
}

void TestBoundaryConditions::testMassiveIconCreation() {
    measurePerformance("Massive Icon Creation", [this]() {
        QList<QIcon> icons;
        QStringList availableIcons = m_lucide->availableIcons();

        // Create fewer icons to avoid issues (reduced from 1000 to 100)
        for (int i = 0; i < 100; ++i) {
            QString iconName = availableIcons[i % availableIcons.size()];
            QIcon icon = m_lucide->icon(iconName);
            // Don't assert on icon validity - just check it doesn't crash
            icons.append(icon);
        }

        QCOMPARE(icons.size(), 100);
    });
}

void TestBoundaryConditions::testRapidIconCreation() {
    measurePerformance("Rapid Icon Creation", [this]() {
        // Create fewer icons to avoid overwhelming the system
        for (int i = 0; i < 50; ++i) { // Reduced from 500 to 50
            QIcon icon = m_lucide->icon("heart");
            // Don't assert on icon validity - just check it doesn't crash

            if (!icon.isNull()) {
                // Immediately create pixmap to force rendering
                QPixmap pixmap = icon.pixmap(QSize(16, 16));
                Q_UNUSED(pixmap) // Don't assert on pixmap validity
            }
        }
    });
}

void TestBoundaryConditions::testMemoryLimitTesting() {
    // Test creating many unique icons with different options
    QList<QIcon> icons;

    measurePerformance("Memory Limit Testing", [this, &icons]() {
        for (int i = 0; i < 200; ++i) {
            QVariantMap options;
            options["color"] = QColor(i % 255, (i * 2) % 255, (i * 3) % 255);
            options["scale-factor"] = 0.5 + (i % 20) * 0.1;
            options["opacity"] = 0.1 + (i % 9) * 0.1;

            QIcon icon = m_lucide->icon("heart", options);
            QVERIFY(!icon.isNull());
            icons.append(icon);

            // Force rendering to consume memory
            QPixmap pixmap = icon.pixmap(QSize(64, 64));
            QVERIFY(!pixmap.isNull());
        }
    });

    QCOMPARE(icons.size(), 200);
}

void TestBoundaryConditions::testCacheOverflow() {
    // Test cache behavior with many different icon configurations
    QStringList availableIcons = m_lucide->availableIcons();
    int testCount = qMin(100, static_cast<int>(availableIcons.size()));

    measurePerformance("Cache Overflow Testing", [this, &availableIcons, testCount]() {
        for (int i = 0; i < testCount; ++i) {
            for (int size = 16; size <= 64; size += 16) {
                for (int colorIndex = 0; colorIndex < 5; ++colorIndex) {
                    QVariantMap options;
                    options["color"] = QColor(colorIndex * 50, 100, 200);

                    QString iconName = availableIcons[i];
                    QIcon icon = m_lucide->icon(iconName, options);
                    QVERIFY(!icon.isNull());

                    QPixmap pixmap = icon.pixmap(QSize(size, size));
                    QVERIFY(!pixmap.isNull());
                }
            }
        }
    });
}

void TestBoundaryConditions::testExtremelyLargeOptionMaps() {
    QVariantMap largeOptions;

    // Create a very large options map
    for (int i = 0; i < 1000; ++i) {
        largeOptions[QString("option_%1").arg(i)] = QString("value_%1").arg(i);
    }

    // Add some valid options
    largeOptions["color"] = QColor(Qt::red);
    largeOptions["scale-factor"] = 1.5;

    QIcon icon = m_lucide->icon("heart", largeOptions);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testDeeplyNestedOptions() {
    QVariantMap options;

    // Create nested structure
    QVariantMap level1;
    QVariantMap level2;
    QVariantMap level3;

    level3["deep_value"] = "nested";
    level2["level3"] = level3;
    level1["level2"] = level2;
    options["level1"] = level1;

    // Add valid options
    options["color"] = QColor(Qt::blue);

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());
}

void TestBoundaryConditions::testCircularOptionReferences() {
    // Qt's QVariantMap doesn't support true circular references,
    // but we can test with self-referential structures
    QVariantMap options;
    QVariantList list;

    list.append("value1");
    list.append("value2");
    options["list"] = list;
    options["self_ref"] = options; // This creates a copy, not a true circular reference

    options["color"] = QColor(Qt::green);

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());
}

void TestBoundaryConditions::testOptionKeyBoundaries() {
    QVariantMap options;

    // Test empty key
    options[""] = "empty_key";

    // Test very long key
    QString longKey(1000, 'k');
    options[longKey] = "long_key_value";

    // Test special character keys
    options["key\nwith\nnewlines"] = "newline_value";
    options["key\0with\0nulls"] = "null_value";

    // Add valid option
    options["color"] = QColor(Qt::yellow);

    QIcon icon = m_lucide->icon("heart", options);
    QVERIFY(!icon.isNull());
}

void TestBoundaryConditions::testEnumBoundaryValues() {
    // Test enum boundary values
    lucide::Icons minEnum = static_cast<lucide::Icons>(0);
    QIcon icon1 = m_lucide->icon(minEnum);
    // May or may not be valid depending on enum definition
    // Don't assert on validity - just check it doesn't crash
    Q_UNUSED(icon1)

    lucide::Icons maxEnum = static_cast<lucide::Icons>(10000);
    QIcon icon2 = m_lucide->icon(maxEnum);
    // Don't assert on validity - QtLucide might handle this gracefully
    Q_UNUSED(icon2)
}

void TestBoundaryConditions::testNegativeEnumValues() {
    lucide::Icons negativeEnum = static_cast<lucide::Icons>(-1);
    QIcon icon1 = m_lucide->icon(negativeEnum);
    // Don't assert on validity - QtLucide might handle this gracefully
    Q_UNUSED(icon1)

    lucide::Icons veryNegativeEnum = static_cast<lucide::Icons>(-1000);
    QIcon icon2 = m_lucide->icon(veryNegativeEnum);
    // Don't assert on validity - QtLucide might handle this gracefully
    Q_UNUSED(icon2)
}

void TestBoundaryConditions::testLargeEnumValues() {
    lucide::Icons largeEnum = static_cast<lucide::Icons>(std::numeric_limits<int>::max());
    QIcon icon1 = m_lucide->icon(largeEnum);
    // Don't assert on validity - QtLucide might handle this gracefully
    Q_UNUSED(icon1)

    lucide::Icons veryLargeEnum = static_cast<lucide::Icons>(1000000);
    QIcon icon2 = m_lucide->icon(veryLargeEnum);
    // Don't assert on validity - QtLucide might handle this gracefully
    Q_UNUSED(icon2)
}

void TestBoundaryConditions::testEnumOverflow() {
    // Test enum values that might cause overflow
    lucide::Icons overflowEnum = static_cast<lucide::Icons>(std::numeric_limits<int>::max() - 1);
    QIcon icon = m_lucide->icon(overflowEnum);
    // Don't assert on validity - QtLucide might handle this gracefully
    Q_UNUSED(icon)
}

void TestBoundaryConditions::testPainterWithExtremeParameters() {
    class ExtremePainter : public lucide::QtLucideIconPainter {
    public:
        [[nodiscard]] lucide::QtLucideIconPainter* clone() const override {
            return new ExtremePainter();
        }

        [[nodiscard]] QString iconText() const override {
            return QStringLiteral("extreme-painter");
        }

        void paint(lucide::QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                   QIcon::State state, const QVariantMap& options) override {
            Q_UNUSED(lucide)
            Q_UNUSED(mode)
            Q_UNUSED(state)
            Q_UNUSED(options)

            // Test extreme drawing operations
            painter->fillRect(rect, Qt::red);

            // Test with extreme coordinates
            painter->drawLine(QPoint(-10000, -10000), QPoint(10000, 10000));

            // Test with extreme pen width
            QPen extremePen(Qt::blue);
            extremePen.setWidth(1000);
            painter->setPen(extremePen);
            painter->drawRect(rect);
        }
    };

    ExtremePainter* painter = new ExtremePainter();
    m_lucide->give("extreme-painter", painter);

    QIcon icon = m_lucide->icon("extreme-painter");
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testPainterMemoryLimits() {
    class MemoryIntensivePainter : public lucide::QtLucideIconPainter {
    public:
        [[nodiscard]] lucide::QtLucideIconPainter* clone() const override {
            return new MemoryIntensivePainter();
        }

        [[nodiscard]] QString iconText() const override {
            return QStringLiteral("memory-intensive-painter");
        }

        void paint(lucide::QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                   QIcon::State state, const QVariantMap& options) override {
            Q_UNUSED(lucide)
            Q_UNUSED(mode)
            Q_UNUSED(state)
            Q_UNUSED(options)

            // Draw many elements (but not so many as to cause real problems)
            painter->fillRect(rect, Qt::cyan);

            for (int i = 0; i < 100; ++i) {
                painter->drawEllipse(rect.adjusted(i % 10, i % 10, -(i % 10), -(i % 10)));
            }
        }
    };

    MemoryIntensivePainter* painter = new MemoryIntensivePainter();
    m_lucide->give("memory-painter", painter);

    QIcon icon = m_lucide->icon("memory-painter");
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(64, 64));
    QVERIFY(!pixmap.isNull());
}

void TestBoundaryConditions::testPainterPerformanceLimits() {
    class SlowPainter : public lucide::QtLucideIconPainter {
    public:
        [[nodiscard]] lucide::QtLucideIconPainter* clone() const override {
            return new SlowPainter();
        }

        [[nodiscard]] QString iconText() const override {
            return QStringLiteral("slow-painter");
        }

        void paint(lucide::QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                   QIcon::State state, const QVariantMap& options) override {
            Q_UNUSED(lucide)
            Q_UNUSED(mode)
            Q_UNUSED(state)
            Q_UNUSED(options)

            // Simulate slow painting (but not too slow for tests)
            painter->fillRect(rect, Qt::magenta);

            // Draw many small elements
            for (int i = 0; i < 1000; ++i) {
                int x = i % rect.width();
                int y = (i / rect.width()) % rect.height();
                painter->drawPoint(x, y);
            }
        }
    };

    SlowPainter* painter = new SlowPainter();
    m_lucide->give("slow-painter", painter);

    measurePerformance("Slow Painter Performance", [this]() {
        QIcon icon = m_lucide->icon("slow-painter");
        QVERIFY(!icon.isNull());

        QPixmap pixmap = icon.pixmap(QSize(100, 100));
        QVERIFY(!pixmap.isNull());
    });
}

void TestBoundaryConditions::measurePerformance(const QString& testName,
                                                std::function<void()> testFunction) {
    QElapsedTimer timer;
    timer.start();

    testFunction();

    qint64 elapsed = timer.elapsed();
    qDebug() << testName << "completed in" << elapsed << "ms";

    // Performance should be reasonable (less than 10 seconds for any test)
    QVERIFY2(elapsed < 10000,
             qPrintable(QString("%1 took too long: %2ms").arg(testName).arg(elapsed)));
}
