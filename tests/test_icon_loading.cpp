/**
 * QtLucide Icon Loading Tests
 */

#include "test_icon_loading.h"
#include <QApplication>
#include <QIcon>
#include <QPixmap>
#include <QSvgRenderer>

#include <QtLucide/QtLucide.h>



void TestIconLoading::initTestCase()
{
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());
}

void TestIconLoading::cleanupTestCase()
{
    delete m_lucide;
    m_lucide = nullptr;
}

void TestIconLoading::testSvgDataLoading()
{
    // Test loading SVG data by name
    QByteArray svgData = m_lucide->svgData("activity");
    QVERIFY(!svgData.isEmpty());
    QVERIFY(svgData.contains("<svg"));
    QVERIFY(svgData.contains("</svg>"));
    
    // Test loading SVG data by enum
    QByteArray svgData2 = m_lucide->svgData(lucide::Icons::activity);
    QVERIFY(!svgData2.isEmpty());
    QCOMPARE(svgData, svgData2);
}

void TestIconLoading::testSvgDataValidity()
{
    QStringList testIcons = {"activity", "alert-circle", "home", "settings"};
    
    for (const QString& iconName : testIcons) {
        QByteArray svgData = m_lucide->svgData(iconName);
        QVERIFY(!svgData.isEmpty());
        
        // Test that SVG data is valid
        QSvgRenderer renderer(svgData);
        QVERIFY(renderer.isValid());
        
        // Test SVG attributes
        QVERIFY(svgData.contains("xmlns=\"http://www.w3.org/2000/svg\""));
        QVERIFY(svgData.contains("viewBox=\"0 0 24 24\""));
        QVERIFY(svgData.contains("stroke=\"currentColor\""));
    }
}

void TestIconLoading::testIconPixmapGeneration()
{
    QIcon icon = m_lucide->icon("activity");
    QVERIFY(!icon.isNull());
    
    // Test different sizes
    QList<QSize> sizes = {QSize(16, 16), QSize(32, 32), QSize(64, 64), QSize(128, 128)};
    
    for (const QSize& size : sizes) {
        QPixmap pixmap = icon.pixmap(size);
        QVERIFY(!pixmap.isNull());
        QCOMPARE(pixmap.size(), size);
        
        // Test that pixmap is not completely transparent
        QImage image = pixmap.toImage();
        bool hasNonTransparentPixels = false;
        for (int y = 0; y < image.height() && !hasNonTransparentPixels; ++y) {
            for (int x = 0; x < image.width() && !hasNonTransparentPixels; ++x) {
                if (qAlpha(image.pixel(x, y)) > 0) {
                    hasNonTransparentPixels = true;
                }
            }
        }
        QVERIFY(hasNonTransparentPixels);
    }
}

void TestIconLoading::testIconScaling()
{
    // Test with different scale factors
    QVariantMap options;
    options["scale-factor"] = 0.5;
    
    QIcon icon = m_lucide->icon("activity", options);
    QVERIFY(!icon.isNull());
    
    QPixmap pixmap = icon.pixmap(QSize(64, 64));
    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.size(), QSize(64, 64));
}

void TestIconLoading::testIconModes()
{
    QIcon icon = m_lucide->icon("activity");
    QVERIFY(!icon.isNull());
    
    // Test different icon modes
    QPixmap normalPixmap = icon.pixmap(QSize(32, 32), QIcon::Normal);
    QPixmap disabledPixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);
    QPixmap activePixmap = icon.pixmap(QSize(32, 32), QIcon::Active);
    QPixmap selectedPixmap = icon.pixmap(QSize(32, 32), QIcon::Selected);
    
    QVERIFY(!normalPixmap.isNull());
    QVERIFY(!disabledPixmap.isNull());
    QVERIFY(!activePixmap.isNull());
    QVERIFY(!selectedPixmap.isNull());
}

void TestIconLoading::testResourceAccess()
{
    // Test that we can access resources directly
    QStringList availableIcons = m_lucide->availableIcons();
    QVERIFY(availableIcons.size() > 100);
    
    // Test a sample of icons to ensure they all load
    int testCount = qMin(50, availableIcons.size());
    for (int i = 0; i < testCount; ++i) {
        const QString& iconName = availableIcons[i];
        QByteArray svgData = m_lucide->svgData(iconName);
        QVERIFY2(!svgData.isEmpty(), qPrintable(QString("Failed to load icon: %1").arg(iconName)));
        
        QIcon icon = m_lucide->icon(iconName);
        QVERIFY2(!icon.isNull(), qPrintable(QString("Failed to create icon: %1").arg(iconName)));
    }
}


