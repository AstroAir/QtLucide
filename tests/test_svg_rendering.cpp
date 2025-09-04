/**
 * QtLucide SVG Rendering Tests
 */

#include "test_svg_rendering.h"
#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>

#include <QtLucide/QtLucide.h>
#include <QtLucide/QtLucideIconPainter.h>

void TestSvgRendering::initTestCase() {
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());
}

void TestSvgRendering::cleanupTestCase() {
    delete m_lucide;
    m_lucide = nullptr;
}

void TestSvgRendering::testColorCustomization() {
    // Test rendering with different colors
    QVariantMap redOptions;
    redOptions["color"] = QColor(Qt::red);

    QVariantMap blueOptions;
    blueOptions["color"] = QColor(Qt::blue);

    QIcon redIcon = m_lucide->icon("activity", redOptions);
    QIcon blueIcon = m_lucide->icon("activity", blueOptions);

    QVERIFY(!redIcon.isNull());
    QVERIFY(!blueIcon.isNull());

    QPixmap redPixmap = redIcon.pixmap(QSize(32, 32));
    QPixmap bluePixmap = blueIcon.pixmap(QSize(32, 32));

    QVERIFY(!redPixmap.isNull());
    QVERIFY(!bluePixmap.isNull());

    // Pixmaps should be different due to different colors
    QVERIFY(redPixmap.toImage() != bluePixmap.toImage());
}

void TestSvgRendering::testScaleFactorRendering() {
    // Test rendering with different scale factors
    QVariantMap smallOptions;
    smallOptions["scale-factor"] = 0.5;

    QVariantMap largeOptions;
    largeOptions["scale-factor"] = 1.0;

    QIcon smallIcon = m_lucide->icon("activity", smallOptions);
    QIcon largeIcon = m_lucide->icon("activity", largeOptions);

    QVERIFY(!smallIcon.isNull());
    QVERIFY(!largeIcon.isNull());

    QPixmap smallPixmap = smallIcon.pixmap(QSize(64, 64));
    QPixmap largePixmap = largeIcon.pixmap(QSize(64, 64));

    QVERIFY(!smallPixmap.isNull());
    QVERIFY(!largePixmap.isNull());

    // Both should be same size but different content due to scaling
    QCOMPARE(smallPixmap.size(), largePixmap.size());
    QVERIFY(smallPixmap.toImage() != largePixmap.toImage());
}

void TestSvgRendering::testIconStateRendering() {
    // Test rendering different icon states
    QVariantMap disabledOptions;
    disabledOptions["color-disabled"] = QColor(Qt::gray);

    QIcon icon = m_lucide->icon("activity", disabledOptions);
    QVERIFY(!icon.isNull());

    QPixmap normalPixmap = icon.pixmap(QSize(32, 32), QIcon::Normal);
    QPixmap disabledPixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);

    QVERIFY(!normalPixmap.isNull());
    QVERIFY(!disabledPixmap.isNull());

    // Should render differently for different states
    QVERIFY(normalPixmap.toImage() != disabledPixmap.toImage());
}

void TestSvgRendering::testCustomPainter() {
    // Create a custom painter for testing
    class TestIconPainter : public lucide::QtLucideIconPainter {
    public:
        void paint(lucide::QtLucide* lucide, QPainter* painter, const QRect& rect, QIcon::Mode mode,
                   QIcon::State state, const QVariantMap& options) override {
            Q_UNUSED(lucide)
            Q_UNUSED(mode)
            Q_UNUSED(state)
            Q_UNUSED(options)

            // Draw a simple test pattern
            painter->fillRect(rect, Qt::red);
            painter->setPen(Qt::white);
            painter->drawText(rect, Qt::AlignCenter, "TEST");
        }
    };

    TestIconPainter* customPainter = new TestIconPainter();
    m_lucide->give("test-icon", customPainter);

    QIcon customIcon = m_lucide->icon("test-icon");
    QVERIFY(!customIcon.isNull());

    QPixmap pixmap = customIcon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());

    // Verify the custom painter was used
    QImage image = pixmap.toImage();
    bool hasRedPixels = false;
    for (int y = 0; y < image.height() && !hasRedPixels; ++y) {
        for (int x = 0; x < image.width() && !hasRedPixels; ++x) {
            QRgb pixel = image.pixel(x, y);
            if (qRed(pixel) > 200 && qGreen(pixel) < 50 && qBlue(pixel) < 50) {
                hasRedPixels = true;
            }
        }
    }
    QVERIFY(hasRedPixels);
}

void TestSvgRendering::testRenderingPerformance() {
    // Test rendering performance
    QStringList testIcons = {"activity", "circle-alert", "house", "settings", "user"};

    QElapsedTimer timer;
    timer.start();

    // Render multiple icons multiple times
    for (int i = 0; i < 100; ++i) {
        for (const QString& iconName : testIcons) {
            QIcon icon = m_lucide->icon(iconName);
            QPixmap pixmap = icon.pixmap(QSize(32, 32));
            Q_UNUSED(pixmap)
        }
    }

    qint64 elapsed = timer.elapsed();

    // Should complete within reasonable time (adjust threshold as needed)
    QVERIFY2(elapsed < 5000, qPrintable(QString("Rendering took too long: %1ms").arg(elapsed)));

    qDebug() << "Rendered" << (testIcons.size() * 100) << "icons in" << elapsed << "ms";
    qDebug() << "Average time per icon:" << (double(elapsed) / (testIcons.size() * 100)) << "ms";
}
