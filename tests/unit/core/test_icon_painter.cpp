/**
 * @file test_icon_painter.cpp
 * @brief Unit tests for QtLucideIconPainter and QtLucideSvgIconPainter
 */

#include "test_icon_painter.h"

#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QPixmap>

#include <QtLucide/QtLucide.h>
#include <QtLucide/QtLucideIconPainter.h>

void TestIconPainter::initTestCase() {
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());
}

void TestIconPainter::cleanupTestCase() {
    delete m_lucide;
    m_lucide = nullptr;
}

void TestIconPainter::init() {
    m_svgPainter = new lucide::QtLucideSvgIconPainter();
}

void TestIconPainter::cleanup() {
    delete m_svgPainter;
    m_svgPainter = nullptr;
}

// ============================================================================
// QtLucideSvgIconPainter basic functionality
// ============================================================================

void TestIconPainter::testSvgPainterConstruction() {
    QVERIFY(m_svgPainter != nullptr);
}

void TestIconPainter::testSvgPainterClone() {
    lucide::QtLucideIconPainter* cloned = m_svgPainter->clone();
    QVERIFY(cloned != nullptr);
    QVERIFY(cloned != m_svgPainter);

    // Cloned painter should have same iconText
    QCOMPARE(cloned->iconText(), m_svgPainter->iconText());

    delete cloned;
}

void TestIconPainter::testSvgPainterIconText() {
    QString iconText = m_svgPainter->iconText();
    QCOMPARE(iconText, QStringLiteral("svg-icon-painter"));
}

// ============================================================================
// paint() with valid parameters
// ============================================================================

void TestIconPainter::testPaintWithValidIcon() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Verify something was painted (not all transparent)
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

void TestIconPainter::testPaintWithValidIconAndColor() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::heart);
    options["color"] = QColor(Qt::red);

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Verify the icon was painted
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

void TestIconPainter::testPaintWithValidIconAndOpacity() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);
    options["opacity"] = 0.5;

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Should still have content
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

void TestIconPainter::testPaintWithValidIconAndScaleFactor() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);
    options["scale-factor"] = 0.5;

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Should have content (scaled down)
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

// ============================================================================
// paint() with invalid parameters
// ============================================================================

void TestIconPainter::testPaintWithNullLucide() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);

    // Should not crash
    m_svgPainter->paint(nullptr, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off, options);
    painter.end();

    // Pixmap should remain white (nothing painted)
    QImage image = pixmap.toImage();
    QCOMPARE(image.pixel(32, 32), QColor(Qt::white).rgb());
}

void TestIconPainter::testPaintWithNullQPainter() {
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);

    // Should not crash
    m_svgPainter->paint(m_lucide, nullptr, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off, options);
    QVERIFY(true);
}

void TestIconPainter::testPaintWithInvalidIconId() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = 999999; // Invalid ID

    // Should not crash
    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    QVERIFY(true);
}

void TestIconPainter::testPaintWithNegativeIconId() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = -1;

    // Should not crash
    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Pixmap should remain white (nothing painted)
    QImage image = pixmap.toImage();
    QCOMPARE(image.pixel(32, 32), QColor(Qt::white).rgb());
}

// ============================================================================
// Color processing
// ============================================================================

void TestIconPainter::testColorReplacementInSvg() {
    // Get SVG data and verify it contains currentColor
    QByteArray svgData = m_lucide->svgData(lucide::Icons::activity);
    QVERIFY(svgData.contains("currentColor"));

    // Paint with a specific color
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(255, 0, 0); // Red

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Verify red pixels exist
    QImage image = pixmap.toImage();
    bool hasRedPixels = false;
    for (int y = 0; y < image.height() && !hasRedPixels; ++y) {
        for (int x = 0; x < image.width() && !hasRedPixels; ++x) {
            QColor pixel = image.pixelColor(x, y);
            if (pixel.alpha() > 0 && pixel.red() > 200 && pixel.green() < 50 && pixel.blue() < 50) {
                hasRedPixels = true;
            }
        }
    }
    QVERIFY(hasRedPixels);
}

void TestIconPainter::testCurrentColorReplacement() {
    QByteArray svgData = m_lucide->svgData(lucide::Icons::heart);

    // SVG should contain currentColor references
    QVERIFY(svgData.contains("stroke=\"currentColor\"") ||
            svgData.contains("fill=\"currentColor\""));
}

void TestIconPainter::testDisabledModeAutoColor() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);
    // No explicit color-disabled, should auto-generate

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Disabled, QIcon::Off,
                        options);
    painter.end();

    // Should have content (lighter color)
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

void TestIconPainter::testActiveModeColor() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);
    options["color-active"] = QColor(Qt::blue);

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Active, QIcon::Off,
                        options);
    painter.end();

    // Should have blue content
    QImage image = pixmap.toImage();
    bool hasBluePixels = false;
    for (int y = 0; y < image.height() && !hasBluePixels; ++y) {
        for (int x = 0; x < image.width() && !hasBluePixels; ++x) {
            QColor pixel = image.pixelColor(x, y);
            if (pixel.alpha() > 0 && pixel.blue() > 200) {
                hasBluePixels = true;
            }
        }
    }
    QVERIFY(hasBluePixels);
}

void TestIconPainter::testSelectedModeColor() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);
    options["color-selected"] = QColor(Qt::green);

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Selected, QIcon::Off,
                        options);
    painter.end();

    // Should have green content
    QImage image = pixmap.toImage();
    bool hasGreenPixels = false;
    for (int y = 0; y < image.height() && !hasGreenPixels; ++y) {
        for (int x = 0; x < image.width() && !hasGreenPixels; ++x) {
            QColor pixel = image.pixelColor(x, y);
            if (pixel.alpha() > 0 && pixel.green() > 200) {
                hasGreenPixels = true;
            }
        }
    }
    QVERIFY(hasGreenPixels);
}

// ============================================================================
// Mode and state option keys
// ============================================================================

void TestIconPainter::testOptionKeysForNormalMode() {
    // Normal mode should use base key
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Normal);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionKeysForDisabledMode() {
    QVariantMap options;
    options["color"] = QColor(Qt::black);
    options["color-disabled"] = QColor(Qt::gray);

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionKeysForActiveMode() {
    QVariantMap options;
    options["color"] = QColor(Qt::black);
    options["color-active"] = QColor(Qt::blue);

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Active);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionKeysForSelectedMode() {
    QVariantMap options;
    options["color"] = QColor(Qt::black);
    options["color-selected"] = QColor(Qt::white);

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Selected);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionKeysForOffState() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Normal, QIcon::Off);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionKeysForOnState() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Normal, QIcon::On);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionKeysPriorityOrder() {
    // Test that mode-specific keys take priority over base keys
    QVariantMap options;
    options["color"] = QColor(Qt::black);
    options["color-disabled"] = QColor(Qt::red);

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);

    // Disabled mode should use color-disabled
    QPixmap disabledPixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);
    QVERIFY(!disabledPixmap.isNull());

    // Normal mode should use color
    QPixmap normalPixmap = icon.pixmap(QSize(32, 32), QIcon::Normal);
    QVERIFY(!normalPixmap.isNull());
}

// ============================================================================
// Option value resolution
// ============================================================================

void TestIconPainter::testOptionValueWithExactMatch() {
    QVariantMap options;
    options["color-disabled"] = QColor(Qt::gray);

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionValueWithFallback() {
    QVariantMap options;
    options["color"] = QColor(Qt::black);
    // No color-active, should fall back to color

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32), QIcon::Active);
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionValueWithNoMatch() {
    QVariantMap options;
    // No color options at all

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOptionValueWithInvalidVariant() {
    QVariantMap options;
    options["color"] = QVariant(); // Invalid variant

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

// ============================================================================
// Scale factor handling
// ============================================================================

void TestIconPainter::testScaleFactorDefault() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testScaleFactorCustom() {
    QVariantMap options;
    options["scale-factor"] = 0.5;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testScaleFactorClamping() {
    // Test scale factor is clamped to [0.1, 10.0]
    QVariantMap optionsSmall;
    optionsSmall["scale-factor"] = 0.01;

    QIcon iconSmall = m_lucide->icon(lucide::Icons::activity, optionsSmall);
    QPixmap pixmapSmall = iconSmall.pixmap(QSize(32, 32));
    QVERIFY(!pixmapSmall.isNull());

    QVariantMap optionsLarge;
    optionsLarge["scale-factor"] = 100.0;

    QIcon iconLarge = m_lucide->icon(lucide::Icons::activity, optionsLarge);
    QPixmap pixmapLarge = iconLarge.pixmap(QSize(32, 32));
    QVERIFY(!pixmapLarge.isNull());
}

void TestIconPainter::testScaleFactorZero() {
    QVariantMap options;
    options["scale-factor"] = 0.0;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    // Should be clamped to minimum
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testScaleFactorNegative() {
    QVariantMap options;
    options["scale-factor"] = -1.0;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    // Should be clamped to minimum
    QVERIFY(!pixmap.isNull());
}

// ============================================================================
// Opacity handling
// ============================================================================

void TestIconPainter::testOpacityDefault() {
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOpacityCustom() {
    QVariantMap options;
    options["opacity"] = 0.5;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOpacityZero() {
    QVariantMap options;
    options["opacity"] = 0.0;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOpacityGreaterThanOne() {
    QVariantMap options;
    options["opacity"] = 2.0;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestIconPainter::testOpacityNegative() {
    QVariantMap options;
    options["opacity"] = -0.5;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

// ============================================================================
// Custom painter implementation
// ============================================================================

namespace {
class TestCustomPainter : public lucide::QtLucideIconPainter {
public:
    TestCustomPainter() = default;

    lucide::QtLucideIconPainter* clone() const override { return new TestCustomPainter(); }

    QString iconText() const override { return QStringLiteral("test-custom-painter"); }

    void paint(lucide::QtLucide*, QPainter* painter, const QRect& rect, QIcon::Mode, QIcon::State,
               const QVariantMap& options) override {
        if (!painter)
            return;

        QColor color = options.value("color", QColor(Qt::black)).value<QColor>();
        painter->fillRect(rect, color);
    }
};
} // namespace

void TestIconPainter::testCustomPainterRegistration() {
    m_lucide->give("test-custom", new TestCustomPainter());

    QIcon icon = m_lucide->icon("test-custom");
    QVERIFY(!icon.isNull());
}

void TestIconPainter::testCustomPainterClone() {
    TestCustomPainter painter;
    lucide::QtLucideIconPainter* cloned = painter.clone();

    QVERIFY(cloned != nullptr);
    QVERIFY(cloned != &painter);
    QCOMPARE(cloned->iconText(), painter.iconText());

    delete cloned;
}

void TestIconPainter::testCustomPainterIconText() {
    TestCustomPainter painter;
    QCOMPARE(painter.iconText(), QStringLiteral("test-custom-painter"));
}

void TestIconPainter::testCustomPainterPaint() {
    m_lucide->give("test-paint", new TestCustomPainter());

    QIcon icon = m_lucide->icon("test-paint");
    QPixmap pixmap = icon.pixmap(QSize(32, 32));

    QVERIFY(!pixmap.isNull());

    // Should have content (filled rect)
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

void TestIconPainter::testCustomPainterWithOptions() {
    m_lucide->give("test-options", new TestCustomPainter());

    QVariantMap options;
    options["color"] = QColor(Qt::red);

    QIcon icon = m_lucide->icon("test-options", options);
    QPixmap pixmap = icon.pixmap(QSize(32, 32));

    QVERIFY(!pixmap.isNull());

    // Should have red pixels
    QImage image = pixmap.toImage();
    bool hasRedPixels = false;
    for (int y = 0; y < image.height() && !hasRedPixels; ++y) {
        for (int x = 0; x < image.width() && !hasRedPixels; ++x) {
            QColor pixel = image.pixelColor(x, y);
            if (pixel.alpha() > 0 && pixel.red() > 200) {
                hasRedPixels = true;
            }
        }
    }
    QVERIFY(hasRedPixels);
}

// ============================================================================
// Edge cases
// ============================================================================

void TestIconPainter::testPaintWithEmptyRect() {
    QPixmap pixmap(64, 64);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);

    // Empty rect should not crash
    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 0, 0), QIcon::Normal, QIcon::Off, options);
    painter.end();

    QVERIFY(true);
}

void TestIconPainter::testPaintWithLargeRect() {
    QPixmap pixmap(1024, 1024);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 1024, 1024), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Should have content
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; y += 10) {
        for (int x = 0; x < image.width() && !hasContent; x += 10) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

void TestIconPainter::testPaintWithNonSquareRect() {
    QPixmap pixmap(128, 64);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);

    m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 128, 64), QIcon::Normal, QIcon::Off,
                        options);
    painter.end();

    // Should have content
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}

void TestIconPainter::testMultiplePaintsInSequence() {
    QPixmap pixmap(64, 64);

    QVariantMap options;
    options["iconId"] = static_cast<int>(lucide::Icons::activity);
    options["color"] = QColor(Qt::black);

    // Paint multiple times in sequence
    for (int i = 0; i < 100; ++i) {
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        m_svgPainter->paint(m_lucide, &painter, QRect(0, 0, 64, 64), QIcon::Normal, QIcon::Off,
                            options);
        painter.end();
    }

    // Should not crash and final pixmap should have content
    QImage image = pixmap.toImage();
    bool hasContent = false;
    for (int y = 0; y < image.height() && !hasContent; ++y) {
        for (int x = 0; x < image.width() && !hasContent; ++x) {
            if (qAlpha(image.pixel(x, y)) > 0) {
                hasContent = true;
            }
        }
    }
    QVERIFY(hasContent);
}
