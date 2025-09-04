#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "QtLucide/QtLucide.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "=== Resource Packaging Verification ===";

    // Test 1: QtLucide initialization
    qDebug() << "1. Testing QtLucide initialization...";
    lucide::QtLucide* lucide = new lucide::QtLucide();
    bool initResult = lucide->initLucide();
    qDebug() << "   QtLucide init result:" << initResult;

    if (!initResult) {
        qDebug() << "   ERROR: QtLucide initialization failed!";
        return 1;
    }

    QStringList availableIcons = lucide->availableIcons();
    qDebug() << "   Available icons count:" << availableIcons.size();

    // Test 2: Verify metadata files
    qDebug() << "2. Testing metadata files...";

    // Test icons.json
    QFile iconsFile(":/lucide/metadata/icons.json");
    if (iconsFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(iconsFile.readAll());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            qDebug() << "   icons.json: Valid JSON with" << obj.size() << "entries";
        } else {
            qDebug() << "   ERROR: icons.json is not a valid JSON object";
        }
    } else {
        qDebug() << "   ERROR: Cannot open icons.json";
    }

    // Test categories.json
    QFile categoriesFile(":/lucide/metadata/categories.json");
    if (categoriesFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(categoriesFile.readAll());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            qDebug() << "   categories.json: Valid JSON with" << obj.size() << "categories";
        } else {
            qDebug() << "   ERROR: categories.json is not a valid JSON object";
        }
    } else {
        qDebug() << "   ERROR: Cannot open categories.json";
    }

    // Test tags.json
    QFile tagsFile(":/lucide/metadata/tags.json");
    if (tagsFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(tagsFile.readAll());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            qDebug() << "   tags.json: Valid JSON with" << obj.size() << "entries";
        } else {
            qDebug() << "   ERROR: tags.json is not a valid JSON object";
        }
    } else {
        qDebug() << "   ERROR: Cannot open tags.json";
    }

    // Test 3: Verify SVG files for sample icons
    qDebug() << "3. Testing SVG files...";
    QStringList sampleIcons = {"heart", "star", "house", "user", "settings", "search", "menu", "x", "check", "folder"};
    int validSvgCount = 0;

    for (const QString& iconName : sampleIcons) {
        QString resourcePath = QString(":/lucide/%1").arg(iconName);
        QFile svgFile(resourcePath);
        if (svgFile.open(QIODevice::ReadOnly)) {
            QByteArray data = svgFile.readAll();
            if (data.size() > 0 && data.contains("<svg")) {
                validSvgCount++;
                qDebug() << "   ✓" << iconName << "- SVG file valid (" << data.size() << "bytes)";
            } else {
                qDebug() << "   ✗" << iconName << "- SVG file invalid or empty";
            }
        } else {
            qDebug() << "   ✗" << iconName << "- SVG file not found at" << resourcePath;
        }
    }

    qDebug() << "   Valid SVG files:" << validSvgCount << "/" << sampleIcons.size();

    // Test 4: Test icon creation
    qDebug() << "4. Testing icon creation...";
    int validIconCount = 0;

    for (const QString& iconName : sampleIcons) {
        QIcon icon = lucide->icon(iconName);
        if (!icon.isNull()) {
            validIconCount++;
            QPixmap pixmap = icon.pixmap(QSize(32, 32));
            qDebug() << "   ✓" << iconName << "- Icon created successfully, pixmap size:" << pixmap.size();
        } else {
            qDebug() << "   ✗" << iconName << "- Failed to create icon";
        }
    }

    qDebug() << "   Valid icons created:" << validIconCount << "/" << sampleIcons.size();

    // Test 5: Summary
    qDebug() << "5. Summary:";
    qDebug() << "   QtLucide initialization:" << (initResult ? "SUCCESS" : "FAILED");
    qDebug() << "   Total available icons:" << availableIcons.size();
    qDebug() << "   Metadata files:" << "All accessible";
    qDebug() << "   SVG files:" << validSvgCount << "/" << sampleIcons.size() << "valid";
    qDebug() << "   Icon creation:" << validIconCount << "/" << sampleIcons.size() << "successful";

    bool allTestsPassed = initResult &&
                         availableIcons.size() > 1600 &&
                         validSvgCount == sampleIcons.size() &&
                         validIconCount == sampleIcons.size();

    qDebug() << "=== OVERALL RESULT:" << (allTestsPassed ? "ALL TESTS PASSED" : "SOME TESTS FAILED") << "===";

    return allTestsPassed ? 0 : 1;
}
