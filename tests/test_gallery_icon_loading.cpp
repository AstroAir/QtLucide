/**
 * Gallery Icon Loading Tests
 * Tests specific to the Gallery application's icon loading functionality
 */

#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPixmap>
#include <QtTest/QtTest>

#include "../examples/gallery/IconItem.h"
#include "../examples/gallery/IconMetadataManager.h"
#include "test_gallery_icon_loading.h"
#include <QtLucide/QtLucide.h>

void TestGalleryIconLoading::initTestCase() {
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());

    // Initialize metadata manager
    m_metadataManager = new IconMetadataManager(this);
    QVERIFY(m_metadataManager != nullptr);

    qDebug() << "Gallery icon loading test initialized with" << m_lucide->availableIcons().size()
             << "icons";
}

void TestGalleryIconLoading::cleanupTestCase() {
    delete m_metadataManager;
    delete m_lucide;
}

void TestGalleryIconLoading::testMetadataManagerInitialization() {
    // Test that metadata manager can be created and initialized
    QVERIFY(m_metadataManager != nullptr);

    // Test metadata loading
    bool loaded = m_metadataManager->loadMetadata();
    QVERIFY(loaded);

    // Verify we have icons
    QStringList allIcons = m_metadataManager->getAllIconNames();
    QVERIFY(allIcons.size() > 1000);
    qDebug() << "Metadata manager loaded" << allIcons.size() << "icons";
}

void TestGalleryIconLoading::testMetadataLoading() {
    // Test that metadata files can be loaded
    QFile iconsFile(":/lucide/metadata/icons.json");
    QVERIFY(iconsFile.exists());
    QVERIFY(iconsFile.open(QIODevice::ReadOnly));

    QJsonDocument doc = QJsonDocument::fromJson(iconsFile.readAll());
    QVERIFY(!doc.isNull());

    QJsonObject root = doc.object();
    QVERIFY(root.contains("icons"));
    QVERIFY(root.contains("count"));

    int iconCount = root["count"].toInt();
    QVERIFY(iconCount > 1000);
    qDebug() << "Metadata contains" << iconCount << "icons";
}

void TestGalleryIconLoading::testIconItemCreation() {
    // Test creating IconItem widgets
    QString testIconName = "heart";

    IconItem* iconItem = new IconItem(testIconName, m_lucide, m_metadataManager, nullptr);
    QVERIFY(iconItem != nullptr);

    // Test that the icon item has the correct name
    QCOMPARE(iconItem->iconName(), testIconName);

    // Test that the icon can be rendered
    QIcon icon = m_lucide->icon(testIconName);
    QVERIFY(!icon.isNull());

    delete iconItem;
}

void TestGalleryIconLoading::testIconItemRendering() {
    // Test that icons can be rendered to pixmaps
    QStringList testIcons = {"heart", "star", "house", "user", "settings"};

    for (const QString& iconName : testIcons) {
        QIcon icon = m_lucide->icon(iconName);
        QVERIFY2(!icon.isNull(), qPrintable(QString("Icon '%1' should not be null").arg(iconName)));

        QPixmap pixmap = icon.pixmap(QSize(32, 32));
        QVERIFY2(!pixmap.isNull(),
                 qPrintable(QString("Pixmap for icon '%1' should not be null").arg(iconName)));
        QVERIFY2(!pixmap.size().isEmpty(),
                 qPrintable(QString("Pixmap for icon '%1' should have valid size").arg(iconName)));
    }
}

void TestGalleryIconLoading::testGalleryIconAccess() {
    // Test that the Gallery can access all required icons
    QStringList requiredIcons = {
        "image",    // Application icon
        "search",   // Search functionality
        "heart",    // Favorites
        "grid-3x3", // Grid view
        "list",     // List view
        "settings", // Settings
        "download", // Export functionality
        "copy",     // Copy functionality
        "star",     // Rating/favorites
        "folder"    // Categories
    };

    for (const QString& iconName : requiredIcons) {
        // Test that the icon exists in available icons
        QStringList availableIcons = m_lucide->availableIcons();
        QVERIFY2(availableIcons.contains(iconName),
                 qPrintable(QString("Required icon '%1' should be available").arg(iconName)));

        // Test that the icon can be loaded
        QIcon icon = m_lucide->icon(iconName);
        QVERIFY2(!icon.isNull(),
                 qPrintable(QString("Required icon '%1' should load successfully").arg(iconName)));

        // Test that SVG data exists
        QByteArray svgData = m_lucide->svgData(iconName);
        QVERIFY2(!svgData.isEmpty(),
                 qPrintable(QString("SVG data for icon '%1' should not be empty").arg(iconName)));
    }
}

void TestGalleryIconLoading::testResourceIntegrity() {
    // Test that all icons in metadata actually exist as resources
    QStringList allIcons = m_metadataManager->getAllIconNames();
    int checkedCount = 0;
    int maxToCheck = 100; // Check first 100 icons for performance

    for (const QString& iconName : allIcons) {
        if (checkedCount >= maxToCheck)
            break;

        // Test that the icon can be loaded
        QIcon icon = m_lucide->icon(iconName);
        QVERIFY2(
            !icon.isNull(),
            qPrintable(QString("Icon '%1' from metadata should load successfully").arg(iconName)));

        // Test that SVG data exists
        QByteArray svgData = m_lucide->svgData(iconName);
        QVERIFY2(!svgData.isEmpty(),
                 qPrintable(QString("SVG data for icon '%1' should not be empty").arg(iconName)));

        checkedCount++;
    }

    qDebug() << "Verified resource integrity for" << checkedCount << "icons";
}
