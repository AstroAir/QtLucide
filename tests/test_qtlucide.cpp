/**
 * QtLucide Core Functionality Tests
 */

#include "test_qtlucide.h"
#include <QApplication>
#include <QIcon>
#include <QPixmap>

#include <QtLucide/QtLucide.h>



void TestQtLucide::initTestCase()
{
    m_lucide = new lucide::QtLucide(this);
}

void TestQtLucide::cleanupTestCase()
{
    delete m_lucide;
    m_lucide = nullptr;
}

void TestQtLucide::testInitialization()
{
    QVERIFY(m_lucide != nullptr);
    QVERIFY(m_lucide->initLucide());

    // Test that we have the expected number of icons
    QStringList icons = m_lucide->availableIcons();
    QVERIFY(icons.size() > 1000);  // Should have 1634 icons
    QVERIFY(icons.contains("activity"));
    QVERIFY(icons.contains("circle-alert"));
}

void TestQtLucide::testIconCreation()
{
    QVERIFY(m_lucide->initLucide());

    // Debug: Check if resource exists
    QFile resourceFile(":/lucide/activity");
    qDebug() << "Resource exists:" << resourceFile.exists();

    // Try with .svg extension
    QFile resourceFileSvg(":/lucide/activity.svg");
    qDebug() << "Resource with .svg exists:" << resourceFileSvg.exists();

    // List all resources in /lucide
    QDir resourceDir(":/lucide");
    if (resourceDir.exists()) {
        qDebug() << "Resource directory exists, entries:" << resourceDir.entryList();

        // Check resources subdirectory
        QDir resourcesSubDir(":/lucide/resources");
        if (resourcesSubDir.exists()) {
            qDebug() << "Resources subdirectory exists, entries:" << resourcesSubDir.entryList();

            // Check icons subdirectory
            QDir iconsSubDir(":/lucide/resources/icons");
            if (iconsSubDir.exists()) {
                qDebug() << "Icons subdirectory exists, entries:" << iconsSubDir.entryList();

                // Check svg subdirectory
                QDir svgSubDir(":/lucide/resources/icons/svg");
                if (svgSubDir.exists()) {
                    qDebug() << "SVG subdirectory exists, first 10 entries:" << svgSubDir.entryList().mid(0, 10);
                }
            }
        }
    } else {
        qDebug() << "Resource directory does not exist";
    }

    if (resourceFile.exists()) {
        qDebug() << "Resource size:" << resourceFile.size();
    } else if (resourceFileSvg.exists()) {
        qDebug() << "SVG Resource size:" << resourceFileSvg.size();
    }

    // Test creating icon by enum
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());

    // Test that icon has proper sizes (account for device pixel ratio)
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
    // The actual size may be scaled by device pixel ratio, so just verify it's reasonable
    QVERIFY(pixmap.width() >= 32 && pixmap.width() <= 128);
    QVERIFY(pixmap.height() >= 32 && pixmap.height() <= 128);
}

void TestQtLucide::testIconByName()
{
    QVERIFY(m_lucide->initLucide());

    // Test creating icon by string name
    QIcon icon = m_lucide->icon("activity");
    QVERIFY(!icon.isNull());

    // Test with hyphenated name
    QIcon icon2 = m_lucide->icon("circle-alert");
    QVERIFY(!icon2.isNull());
}

void TestQtLucide::testIconByEnum()
{
    QVERIFY(m_lucide->initLucide());

    // Test creating icon by enum
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());

    QIcon icon2 = m_lucide->icon(lucide::Icons::circle_alert);
    QVERIFY(!icon2.isNull());
}

void TestQtLucide::testDefaultOptions()
{
    QVERIFY(m_lucide->initLucide());

    // Test default options
    QVariant colorOption = m_lucide->defaultOption("color");
    QVERIFY(colorOption.isValid());

    QVariant scaleOption = m_lucide->defaultOption("scale-factor");
    QVERIFY(scaleOption.isValid());
    QCOMPARE(scaleOption.toDouble(), 0.9);
}

void TestQtLucide::testCustomOptions()
{
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

void TestQtLucide::testAvailableIcons()
{
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

void TestQtLucide::testInvalidIcon()
{
    QVERIFY(m_lucide->initLucide());

    // Test with invalid icon name
    QIcon icon = m_lucide->icon("non-existent-icon");
    QVERIFY(icon.isNull());

    // Test SVG data for invalid icon
    QByteArray svgData = m_lucide->svgData("non-existent-icon");
    QVERIFY(svgData.isEmpty());
}


