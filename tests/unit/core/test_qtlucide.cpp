/**
 * QtLucide Core Functionality Tests
 */

#include "test_qtlucide.h"
#include <QApplication>
#include <QIcon>
#include <QPixmap>

#include <QtLucide/QtLucide.h>

void TestQtLucide::initTestCase() {
    m_lucide = new lucide::QtLucide(this);
}

void TestQtLucide::cleanupTestCase() {
    delete m_lucide;
    m_lucide = nullptr;
}

void TestQtLucide::testInitialization() {
    QVERIFY(m_lucide != nullptr);
    QVERIFY(m_lucide->initLucide());

    // Test that we have the expected number of icons
    QStringList icons = m_lucide->availableIcons();
    QVERIFY(icons.size() > 1000); // Should have 1634 icons
    QVERIFY(icons.contains("activity"));
    QVERIFY(icons.contains("circle-alert"));
}

void TestQtLucide::testIconCreation() {
    QVERIFY(m_lucide->initLucide());

    // Test creating icon by enum
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());

    // Test that icon has proper sizes (account for device pixel ratio)
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
    // The actual size may be scaled by device pixel ratio, so just verify it's reasonable
    QVERIFY(pixmap.width() >= 16 && pixmap.width() <= 128);
    QVERIFY(pixmap.height() >= 16 && pixmap.height() <= 128);
    
    // Test multiple sizes
    QPixmap pixmap64 = icon.pixmap(QSize(64, 64));
    QVERIFY(!pixmap64.isNull());
    
    QPixmap pixmap128 = icon.pixmap(QSize(128, 128));
    QVERIFY(!pixmap128.isNull());
}

void TestQtLucide::testIconByName() {
    QVERIFY(m_lucide->initLucide());

    // Test creating icon by string name
    QIcon icon = m_lucide->icon("activity");
    QVERIFY(!icon.isNull());

    // Test with hyphenated name
    QIcon icon2 = m_lucide->icon("circle-alert");
    QVERIFY(!icon2.isNull());
}

void TestQtLucide::testIconByEnum() {
    QVERIFY(m_lucide->initLucide());

    // Test creating icon by enum
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());

    QIcon icon2 = m_lucide->icon(lucide::Icons::circle_alert);
    QVERIFY(!icon2.isNull());
}

void TestQtLucide::testDefaultOptions() {
    QVERIFY(m_lucide->initLucide());

    // Test default options
    QVariant colorOption = m_lucide->defaultOption("color");
    QVERIFY(colorOption.isValid());

    QVariant scaleOption = m_lucide->defaultOption("scale-factor");
    QVERIFY(scaleOption.isValid());
    QCOMPARE(scaleOption.toDouble(), 0.9);
}

void TestQtLucide::testCustomOptions() {
    QVERIFY(m_lucide->initLucide());

    // Test setting custom options
    m_lucide->setDefaultOption("color", QColor(Qt::red));
    QVariant colorOption = m_lucide->defaultOption("color");
    QCOMPARE(colorOption.value<QColor>(), QColor(Qt::red));

    // Test icon with custom options
    QVariantMap options;
    options["color"] = QColor(Qt::blue);
    options["scale-factor"] = 1.0;

    QIcon icon = m_lucide->icon("activity", options);
    QVERIFY(!icon.isNull());
}

void TestQtLucide::testAvailableIcons() {
    QVERIFY(m_lucide->initLucide());

    QStringList icons = m_lucide->availableIcons();
    QVERIFY(!icons.isEmpty());
    QVERIFY(icons.contains("activity"));
    QVERIFY(icons.contains("house"));
    QVERIFY(icons.contains("settings"));

    // Test that icons are sorted
    QStringList sortedIcons = icons;
    sortedIcons.sort();
    QCOMPARE(icons, sortedIcons);
}

void TestQtLucide::testInvalidIcon() {
    QVERIFY(m_lucide->initLucide());

    // Test with invalid icon name
    QIcon icon = m_lucide->icon("non-existent-icon");
    QVERIFY(icon.isNull());

    // Test SVG data for invalid icon
    QByteArray svgData = m_lucide->svgData("non-existent-icon");
    QVERIFY(svgData.isEmpty());
    
    // Test with empty name
    QByteArray emptySvg = m_lucide->svgData("");
    QVERIFY(emptySvg.isEmpty());
}

void TestQtLucide::testSvgData() {
    QVERIFY(m_lucide->initLucide());

    // Test getting SVG data by name
    QByteArray svgData = m_lucide->svgData("activity");
    QVERIFY(!svgData.isEmpty());
    QVERIFY(svgData.contains("<svg"));
    QVERIFY(svgData.contains("</svg>"));

    // Test getting SVG data by enum
    QByteArray svgDataEnum = m_lucide->svgData(lucide::Icons::activity);
    QVERIFY(!svgDataEnum.isEmpty());
    QCOMPARE(svgData, svgDataEnum);

    // Test SVG data contains expected attributes
    QVERIFY(svgData.contains("viewBox"));
}

void TestQtLucide::testIconModes() {
    QVERIFY(m_lucide->initLucide());

    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());

    // Test different icon modes
    QPixmap normalPixmap = icon.pixmap(QSize(32, 32), QIcon::Normal);
    QVERIFY(!normalPixmap.isNull());

    QPixmap disabledPixmap = icon.pixmap(QSize(32, 32), QIcon::Disabled);
    QVERIFY(!disabledPixmap.isNull());

    QPixmap activePixmap = icon.pixmap(QSize(32, 32), QIcon::Active);
    QVERIFY(!activePixmap.isNull());

    QPixmap selectedPixmap = icon.pixmap(QSize(32, 32), QIcon::Selected);
    QVERIFY(!selectedPixmap.isNull());
}

void TestQtLucide::testOpacity() {
    QVERIFY(m_lucide->initLucide());

    // Test icon with custom opacity
    QVariantMap options;
    options["opacity"] = 0.5;

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
}

void TestQtLucide::testScaleFactor() {
    QVERIFY(m_lucide->initLucide());

    // Test icon with custom scale factor
    QVariantMap options;
    options["scale-factor"] = 1.0; // Full size

    QIcon icon = m_lucide->icon(lucide::Icons::activity, options);
    QVERIFY(!icon.isNull());

    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());

    // Test with smaller scale factor
    QVariantMap smallOptions;
    smallOptions["scale-factor"] = 0.5;

    QIcon smallIcon = m_lucide->icon(lucide::Icons::activity, smallOptions);
    QVERIFY(!smallIcon.isNull());
}
