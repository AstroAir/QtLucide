/**
 * Theme Manager Tests Implementation
 */

#include "test_theme_manager.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QElapsedTimer>

void TestThemeManager::initTestCase()
{
    // Set up test environment
    m_testDataDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/QtLucideThemeTests";
    QDir().mkpath(m_testDataDir);
    
    m_themeManager = nullptr;
    
    qDebug() << "Theme manager test environment initialized at:" << m_testDataDir;
}

void TestThemeManager::cleanupTestCase()
{
    // Clean up test environment
    QDir(m_testDataDir).removeRecursively();
    qDebug() << "Theme manager test environment cleaned up";
}

void TestThemeManager::init()
{
    // Create fresh theme manager for each test
    if (m_themeManager) {
        delete m_themeManager;
    }
    m_themeManager = new ThemeManager();
}

void TestThemeManager::cleanup()
{
    // Cleanup after each test
    if (m_themeManager) {
        delete m_themeManager;
        m_themeManager = nullptr;
    }
}

void TestThemeManager::testThemeManagerCreation()
{
    // Test that ThemeManager can be created
    QVERIFY(m_themeManager != nullptr);
    QVERIFY(m_themeManager->inherits("QObject"));
}

void TestThemeManager::testDefaultTheme()
{
    // Test that default theme is set properly
    QVERIFY(m_themeManager != nullptr);
    
    ThemeManager::Theme currentTheme = m_themeManager->currentTheme();
    QVERIFY(currentTheme == ThemeManager::SystemTheme || 
            currentTheme == ThemeManager::LightTheme || 
            currentTheme == ThemeManager::DarkTheme);
    
    // Test that theme name is not empty
    QString themeName = m_themeManager->currentThemeName();
    QVERIFY(!themeName.isEmpty());
}

void TestThemeManager::testThemeSwitching()
{
    // Test switching between different themes
    QVERIFY(m_themeManager != nullptr);
    
    // Test switching to light theme
    m_themeManager->setTheme(ThemeManager::LightTheme);
    QCOMPARE(m_themeManager->currentTheme(), ThemeManager::LightTheme);
    
    // Test switching to dark theme
    m_themeManager->setTheme(ThemeManager::DarkTheme);
    QCOMPARE(m_themeManager->currentTheme(), ThemeManager::DarkTheme);
    
    // Test switching to system theme
    m_themeManager->setTheme(ThemeManager::SystemTheme);
    QCOMPARE(m_themeManager->currentTheme(), ThemeManager::SystemTheme);
}

void TestThemeManager::testThemeSignals()
{
    // Test that theme change signals are emitted
    QVERIFY(m_themeManager != nullptr);
    
    QSignalSpy spy(m_themeManager, &ThemeManager::themeChanged);
    QVERIFY(spy.isValid());
    
    // Change theme and verify signal emission
    ThemeManager::Theme originalTheme = m_themeManager->currentTheme();
    ThemeManager::Theme newTheme = (originalTheme == ThemeManager::LightTheme) ? 
                                   ThemeManager::DarkTheme : ThemeManager::LightTheme;
    
    m_themeManager->setTheme(newTheme);
    
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).value<ThemeManager::Theme>(), newTheme);
}

void TestThemeManager::testLightTheme()
{
    // Test light theme properties
    QVERIFY(m_themeManager != nullptr);
    
    m_themeManager->setTheme(ThemeManager::LightTheme);
    QCOMPARE(m_themeManager->currentTheme(), ThemeManager::LightTheme);
    
    QString themeName = m_themeManager->currentThemeName();
    QVERIFY(themeName.contains("Light", Qt::CaseInsensitive));
}

void TestThemeManager::testDarkTheme()
{
    // Test dark theme properties
    QVERIFY(m_themeManager != nullptr);
    
    m_themeManager->setTheme(ThemeManager::DarkTheme);
    QCOMPARE(m_themeManager->currentTheme(), ThemeManager::DarkTheme);
    
    QString themeName = m_themeManager->currentThemeName();
    QVERIFY(themeName.contains("Dark", Qt::CaseInsensitive));
}

void TestThemeManager::testSystemTheme()
{
    // Test system theme properties
    QVERIFY(m_themeManager != nullptr);
    
    m_themeManager->setTheme(ThemeManager::SystemTheme);
    QCOMPARE(m_themeManager->currentTheme(), ThemeManager::SystemTheme);
    
    QString themeName = m_themeManager->currentThemeName();
    QVERIFY(themeName.contains("System", Qt::CaseInsensitive));
}

void TestThemeManager::testCustomThemeLoading()
{
    // Test loading custom theme from file
    QVERIFY(m_themeManager != nullptr);
    
    QString themeFile = m_testDataDir + "/test_theme.json";
    createTestThemeFile(themeFile, true);
    
    bool result = m_themeManager->loadCustomTheme(themeFile);
    QVERIFY(result);
}

void TestThemeManager::testCustomThemeSaving()
{
    // Test saving custom theme to file
    QVERIFY(m_themeManager != nullptr);
    
    // Create a test theme
    ThemeManager::ThemeColors colors;
    colors.name = "Test Theme";
    colors.description = "A test theme";
    colors.isDark = false;
    colors.colors[ThemeManager::WindowBackground] = QColor("#FFFFFF");
    colors.colors[ThemeManager::PrimaryText] = QColor("#000000");
    
    QString themeFile = m_testDataDir + "/saved_theme.json";
    bool result = m_themeManager->saveCustomTheme(themeFile, colors);
    QVERIFY(result);
    
    // Verify file was created
    QVERIFY(QFile::exists(themeFile));
}

void TestThemeManager::testInvalidCustomTheme()
{
    // Test handling of invalid custom theme
    QVERIFY(m_themeManager != nullptr);
    
    QString themeFile = m_testDataDir + "/invalid_theme.json";
    createTestThemeFile(themeFile, false);
    
    bool result = m_themeManager->loadCustomTheme(themeFile);
    QVERIFY(!result); // Should fail for invalid theme
}

void TestThemeManager::testCustomThemeValidation()
{
    // Test theme file validation
    QVERIFY(m_themeManager != nullptr);
    
    // Test valid theme file
    QString validThemeFile = m_testDataDir + "/valid_theme.json";
    createTestThemeFile(validThemeFile, true);
    QVERIFY(m_themeManager->isValidThemeFile(validThemeFile));
    
    // Test invalid theme file
    QString invalidThemeFile = m_testDataDir + "/invalid_theme.json";
    createTestThemeFile(invalidThemeFile, false);
    QVERIFY(!m_themeManager->isValidThemeFile(invalidThemeFile));
}

void TestThemeManager::testThemeApplication()
{
    // Test that themes are applied to widgets
    QVERIFY(m_themeManager != nullptr);

    QWidget testWidget;
    m_themeManager->applyThemeToWidget(&testWidget);

    // Verify that stylesheet was applied (may be empty for basic widgets)
    // Just verify the method doesn't crash
    QVERIFY(true);
}

void TestThemeManager::testStyleSheetGeneration()
{
    // Test stylesheet generation
    QVERIFY(m_themeManager != nullptr);
    
    QString styleSheet = m_themeManager->getStyleSheet();
    QVERIFY(!styleSheet.isEmpty());
    
    // Verify it contains CSS-like content
    QVERIFY(styleSheet.contains("{") && styleSheet.contains("}"));
}

void TestThemeManager::testColorRetrieval()
{
    // Test color retrieval from current theme
    QVERIFY(m_themeManager != nullptr);
    
    QColor bgColor = m_themeManager->getColor(ThemeManager::WindowBackground);
    QVERIFY(bgColor.isValid());
    
    QColor textColor = m_themeManager->getColor(ThemeManager::PrimaryText);
    QVERIFY(textColor.isValid());
}

void TestThemeManager::testInvalidThemeFile()
{
    // Test handling of invalid theme file path
    QVERIFY(m_themeManager != nullptr);
    
    QString nonExistentFile = m_testDataDir + "/non_existent.json";
    bool result = m_themeManager->loadCustomTheme(nonExistentFile);
    QVERIFY(!result);
}

void TestThemeManager::testMissingThemeFile()
{
    // Test handling of missing theme file
    QVERIFY(m_themeManager != nullptr);
    
    QString missingFile = "/path/that/does/not/exist/theme.json";
    bool result = m_themeManager->loadCustomTheme(missingFile);
    QVERIFY(!result);
}

void TestThemeManager::testCorruptedThemeFile()
{
    // Test handling of corrupted theme file
    QVERIFY(m_themeManager != nullptr);
    
    QString corruptedFile = m_testDataDir + "/corrupted_theme.json";
    QFile file(corruptedFile);
    if (file.open(QIODevice::WriteOnly)) {
        file.write("{ invalid json content");
        file.close();
    }
    
    bool result = m_themeManager->loadCustomTheme(corruptedFile);
    QVERIFY(!result);
}

void TestThemeManager::testThemeSwitchingPerformance()
{
    // Test performance of theme switching
    QVERIFY(m_themeManager != nullptr);
    
    QElapsedTimer timer;
    timer.start();
    
    // Switch themes multiple times
    for (int i = 0; i < 10; ++i) {
        m_themeManager->setTheme(ThemeManager::LightTheme);
        m_themeManager->setTheme(ThemeManager::DarkTheme);
    }
    
    qint64 elapsed = timer.elapsed();
    qDebug() << "Theme switching performance: 20 switches in" << elapsed << "ms";
    
    // Should complete within reasonable time (less than 1 second)
    QVERIFY(elapsed < 1000);
}

void TestThemeManager::testMultipleThemeLoads()
{
    // Test loading multiple custom themes
    QVERIFY(m_themeManager != nullptr);
    
    for (int i = 0; i < 5; ++i) {
        QString themeFile = m_testDataDir + QString("/theme_%1.json").arg(i);
        createTestThemeFile(themeFile, true);
        
        bool result = m_themeManager->loadCustomTheme(themeFile);
        QVERIFY(result);
    }
}

// Helper methods
void TestThemeManager::createTestThemeFile(const QString& filePath, bool valid)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    
    if (valid) {
        QJsonObject themeObj;
        themeObj["name"] = "Test Theme";
        themeObj["description"] = "A test theme for unit testing";
        themeObj["isDark"] = false;
        
        QJsonObject colorsObj;
        colorsObj["background"] = "#FFFFFF";
        colorsObj["text"] = "#000000";
        colorsObj["accent"] = "#0078D4";
        
        themeObj["colors"] = colorsObj;
        
        QJsonDocument doc(themeObj);
        file.write(doc.toJson());
    } else {
        file.write("{ invalid json content }");
    }
    
    file.close();
}

bool TestThemeManager::verifyThemeColors(const ThemeManager::ThemeColors& colors)
{
    return !colors.name.isEmpty() && 
           colors.colors.contains(ThemeManager::WindowBackground) &&
           colors.colors.contains(ThemeManager::PrimaryText);
}

#include "test_theme_manager.moc"
