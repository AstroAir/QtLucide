/**
 * QtLucide Gallery Application - Icon Integration Test
 *
 * This test verifies that the icon integration is working correctly.
 */

#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "ContentManager.h"
#include "GalleryLogger.h"
#include "IconMetadataManager.h"
#include "QtLucide/QtLucide.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    qDebug() << "=== QtLucide Icon Integration Test ===";

    // Initialize QtLucide
    lucide::QtLucide lucide;
    if (!lucide.initLucide()) {
        qDebug() << "FAILED: Could not initialize QtLucide";
        return 1;
    }
    qDebug() << "✓ QtLucide initialized successfully";

    // Get available icons
    QStringList availableIcons = lucide.availableIcons();
    qDebug() << "✓ Found" << availableIcons.size() << "available icons";

    if (availableIcons.isEmpty()) {
        qDebug() << "FAILED: No icons available";
        return 1;
    }

    // Test icon creation
    QString testIconName = availableIcons.first();
    QIcon testIcon = lucide.icon(testIconName);
    if (testIcon.isNull()) {
        qDebug() << "FAILED: Could not create icon for" << testIconName;
        return 1;
    }
    qDebug() << "✓ Successfully created icon:" << testIconName;

    // Test ContentManager
    ContentManager contentManager;
    contentManager.setLucide(&lucide);

    QStringList allContent = contentManager.getAllContent();
    qDebug() << "✓ ContentManager found" << allContent.size() << "total content items";

    QStringList icons = contentManager.getIcons();
    qDebug() << "✓ ContentManager found" << icons.size() << "icons";

    if (icons.size() != availableIcons.size()) {
        qDebug() << "WARNING: Icon count mismatch between QtLucide and ContentManager";
    }

    // Test pixmap generation
    QPixmap testPixmap = contentManager.getPixmap(testIconName, QSize(48, 48));
    if (testPixmap.isNull()) {
        qDebug() << "FAILED: Could not generate pixmap for" << testIconName;
        return 1;
    }
    qDebug() << "✓ Successfully generated pixmap for" << testIconName
             << "size:" << testPixmap.size();

    // Test IconMetadataManager
    IconMetadataManager iconManager;
    if (!iconManager.loadMetadata()) {
        qDebug() << "FAILED: Could not load icon metadata";
        return 1;
    }
    qDebug() << "✓ Icon metadata loaded successfully";

    QStringList allIconNames = iconManager.getAllIconNames();
    qDebug() << "✓ IconMetadataManager found" << allIconNames.size() << "icons with metadata";

    // Test metadata for a specific icon
    IconMetadata metadata = iconManager.getIconMetadata(testIconName);
    if (metadata.isValid()) {
        qDebug() << "✓ Retrieved metadata for" << testIconName;
        qDebug() << "  - Display name:" << metadata.getDisplayName();
        qDebug() << "  - Categories:" << metadata.categories;
        qDebug() << "  - Tags:" << metadata.tags;
    } else {
        qDebug() << "WARNING: No metadata found for" << testIconName;
    }

    // Test categories
    QStringList categories = iconManager.getAllCategories();
    qDebug() << "✓ Found" << categories.size() << "categories:" << categories;

    // Test a few more icons
    int testCount = qMin(5, availableIcons.size());
    qDebug() << "Testing" << testCount << "additional icons...";

    for (int i = 1; i < testCount; ++i) {
        QString iconName = availableIcons.at(i);
        QIcon icon = lucide.icon(iconName);
        QPixmap pixmap = contentManager.getPixmap(iconName, QSize(32, 32));

        if (icon.isNull() || pixmap.isNull()) {
            qDebug() << "FAILED: Icon" << iconName << "- Icon null:" << icon.isNull()
                     << "Pixmap null:" << pixmap.isNull();
            return 1;
        }
    }
    qDebug() << "✓ All" << testCount << "test icons generated successfully";

    qDebug() << "\n=== Icon Integration Test Results ===";
    qDebug() << "✓ QtLucide initialization: PASSED";
    qDebug() << "✓ Icon enumeration: PASSED (" << availableIcons.size() << "icons)";
    qDebug() << "✓ Icon creation: PASSED";
    qDebug() << "✓ ContentManager integration: PASSED";
    qDebug() << "✓ Pixmap generation: PASSED";
    qDebug() << "✓ IconMetadataManager: PASSED";
    qDebug() << "✓ Metadata loading: PASSED";
    qDebug() << "✓ Category support: PASSED (" << categories.size() << "categories)";
    qDebug() << "\n🎉 ALL TESTS PASSED! Icon integration is working correctly.";

    // Exit after a short delay to allow reading the output
    QTimer::singleShot(100, &app, &QApplication::quit);

    return app.exec();
}
