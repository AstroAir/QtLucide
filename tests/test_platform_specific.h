/**
 * QtLucide Platform-Specific Tests - Header
 * 
 * Tests for different operating system behaviors including Windows-specific
 * file path handling, theme integration, and high DPI display support.
 */

#ifndef TEST_PLATFORM_SPECIFIC_H
#define TEST_PLATFORM_SPECIFIC_H

#include <QObject>
#include <QtTest/QtTest>
#include <QApplication>
#include <QScreen>
#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QStyleFactory>
#include <QPalette>

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>
#endif

namespace lucide {
class QtLucide;
}

class IconMetadataManager;

/**
 * @brief Platform-specific configuration structure
 */
struct PlatformConfig {
    QString platformName;
    QStringList supportedThemes;
    QList<qreal> supportedDpiScales;
    QString defaultFilePathSeparator;
    bool supportsSystemTheme;
    bool supportsHighDpi;
    bool supportsFileAssociations;
};

/**
 * @brief High DPI test configuration
 */
struct HighDpiConfig {
    qreal dpiScale;
    QSize expectedIconSize;
    QSize actualIconSize;
    bool scalingCorrect;
    QString scalingMethod;
};

class TestPlatformSpecific : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Windows-specific file path tests
    void testWindowsFilePathHandling();
    void testWindowsResourcePathResolution();
    void testWindowsLongPathSupport();
    void testWindowsUnicodePathSupport();
    void testWindowsNetworkPathHandling();
    void testWindowsFilePermissions();

    // Theme integration tests
    void testSystemThemeDetection();
    void testSystemThemeChanges();
    void testDarkModeIntegration();
    void testLightModeIntegration();
    void testHighContrastModeSupport();
    void testCustomThemeSupport();

    // High DPI display tests
    void testHighDpiIconRendering();
    void testHighDpiScalingFactors();
    void testHighDpiPixmapGeneration();
    void testHighDpiLayoutScaling();
    void testMultiMonitorDpiHandling();
    void testDpiAwarenessModes();

    // Windows-specific API integration
    void testWindowsRegistryIntegration();
    void testWindowsShellIntegration();
    void testWindowsFileAssociations();
    void testWindowsJumpListIntegration();
    void testWindowsTaskbarIntegration();

    // Platform performance tests
    void testPlatformSpecificPerformance();
    void testFileSystemPerformance();
    void testGraphicsPerformance();
    void testMemoryManagementPlatformSpecific();

    // Cross-platform compatibility tests
    void testCrossPlatformPathHandling();
    void testCrossPlatformResourceLoading();
    void testCrossPlatformThemeConsistency();
    void testCrossPlatformIconConsistency();

private:
    // Platform detection helpers
    PlatformConfig detectPlatformCapabilities();
    bool isWindowsPlatform();
    bool isHighDpiDisplay();
    qreal getCurrentDpiScale();
    
    // Windows-specific helpers
#ifdef Q_OS_WIN
    bool isWindowsDarkModeEnabled();
    bool isWindowsHighContrastEnabled();
    QString getWindowsThemeName();
    void setWindowsDpiAwareness();
#endif
    
    // High DPI testing helpers
    HighDpiConfig testIconAtDpiScale(qreal scale, const QString& iconName);
    QPixmap generateHighDpiPixmap(const QString& iconName, const QSize& size, qreal dpiScale);
    bool verifyDpiScaling(const QPixmap& pixmap, qreal expectedScale);
    
    // Theme testing helpers
    void applySystemTheme();
    void applyDarkTheme();
    void applyLightTheme();
    void applyHighContrastTheme();
    bool verifyThemeConsistency();
    
    // File path testing helpers
    void testPathWithSpaces();
    void testPathWithUnicodeCharacters();
    void testPathWithSpecialCharacters();
    void testLongFilePath();
    void testNetworkPath();
    
    // Performance measurement helpers
    qint64 measureFileSystemAccess(const QString& path);
    qint64 measureIconRenderingOnPlatform(const QString& iconName, const QSize& size);
    qint64 measureThemeSwitchingTime();
    
    // Test data and objects
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    QWidget* m_testWidget;
    PlatformConfig m_platformConfig;
    
    // Test paths for different scenarios
    QStringList m_testPaths;
    QStringList m_unicodeTestPaths;
    QStringList m_specialCharPaths;
    
    // DPI test configurations
    QList<qreal> m_testDpiScales;
    QList<QSize> m_testIconSizes;
    
    // Performance tracking
    QElapsedTimer m_performanceTimer;
    
    // Platform-specific thresholds
    static constexpr qint64 WINDOWS_FILE_ACCESS_THRESHOLD_MS = 50;
    static constexpr qint64 THEME_SWITCH_THRESHOLD_MS = 500;
    static constexpr qreal HIGH_DPI_THRESHOLD = 1.5;
    static constexpr qreal DPI_TOLERANCE = 0.1;
};

#endif // TEST_PLATFORM_SPECIFIC_H
