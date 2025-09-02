/**
 * QtLucide Core Functionality Tests
 */

#include <QtTest/QtTest>
#include <QApplication>
#include <QIcon>
#include <QPixmap>

#include <QtLucide/QtLucide.h>

class TestQtLucide : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testInitialization();
    void testIconCreation();
    void testIconByName();
    void testIconByEnum();
    void testDefaultOptions();
    void testCustomOptions();
    void testAvailableIcons();
    void testInvalidIcon();

private:
    lucide::QtLucide* m_lucide;
};

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
    QVERIFY(icons.contains("alert-circle"));
}

void TestQtLucide::testIconCreation()
{
    QVERIFY(m_lucide->initLucide());
    
    // Test creating icon by enum
    QIcon icon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!icon.isNull());
    
    // Test that icon has proper sizes
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.size(), QSize(32, 32));
}

void TestQtLucide::testIconByName()
{
    QVERIFY(m_lucide->initLucide());
    
    // Test creating icon by string name
    QIcon icon = m_lucide->icon("activity");
    QVERIFY(!icon.isNull());
    
    // Test with hyphenated name
    QIcon icon2 = m_lucide->icon("alert-circle");
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
    QVERIFY(icons.contains("home"));
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

#include "test_qtlucide.moc"
