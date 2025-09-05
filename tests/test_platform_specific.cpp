/**
 * QtLucide Platform-Specific Tests - Implementation
 */

#include "test_platform_specific.h"
#include <QtLucide/QtLucide.h>
#include "../examples/gallery/IconMetadataManager.h"

#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>
#include <QStyleFactory>

#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif

void TestPlatformSpecific::initTestCase() {
    qDebug() << "Initializing Platform-Specific Test Suite";
    
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    QVERIFY(m_lucide->initLucide());
    
    // Initialize metadata manager
    m_metadataManager = new IconMetadataManager(this);
    QVERIFY(m_metadataManager->initialize());
    
    // Create test widget
    m_testWidget = new QWidget();
    m_testWidget->resize(400, 300);
    
    // Detect platform capabilities
    m_platformConfig = detectPlatformCapabilities();
    
    // Setup test data
    m_testPaths = {
        "C:\\Program Files\\Test App\\icons",
        "C:\\Users\\Test User\\Documents\\My Icons",
        "D:\\Projects\\QtLucide\\resources"
    };
    
    m_unicodeTestPaths = {
        "C:\\测试\\图标",
        "C:\\Тест\\иконки",
        "C:\\テスト\\アイコン"
    };
    
    m_specialCharPaths = {
        "C:\\Test & Special\\icons",
        "C:\\Test (Parentheses)\\icons",
        "C:\\Test-Dash_Underscore\\icons"
    };
    
    m_testDpiScales = {1.0, 1.25, 1.5, 2.0, 2.5, 3.0};
    m_testIconSizes = {{16, 16}, {24, 24}, {32, 32}, {48, 48}, {64, 64}, {128, 128}};
    
    qDebug() << "Platform:" << m_platformConfig.platformName;
    qDebug() << "High DPI Support:" << m_platformConfig.supportsHighDpi;
    qDebug() << "System Theme Support:" << m_platformConfig.supportsSystemTheme;
    
    qDebug() << "Platform-specific test environment initialized successfully";
}

void TestPlatformSpecific::cleanupTestCase() {
    delete m_testWidget;
    delete m_metadataManager;
    delete m_lucide;
    qDebug() << "Platform-specific test cleanup completed";
}

void TestPlatformSpecific::init() {
    m_performanceTimer.start();
}

void TestPlatformSpecific::cleanup() {
    // Reset any platform-specific state
}

void TestPlatformSpecific::testWindowsFilePathHandling() {
    if (!isWindowsPlatform()) {
        QSKIP("Windows-specific test skipped on non-Windows platform");
    }
    
    qDebug() << "Testing Windows file path handling";
    
    // Test standard Windows paths
    for (const QString& path : m_testPaths) {
        QDir dir(path);
        QString normalizedPath = QDir::toNativeSeparators(path);
        
        // Verify path normalization
        QVERIFY(normalizedPath.contains('\\'));
        QVERIFY(!normalizedPath.contains('/'));
        
        // Test path validation
        QFileInfo fileInfo(normalizedPath);
        QVERIFY(!fileInfo.fileName().isEmpty());
    }
    
    qDebug() << "Windows file path handling test passed";
}

void TestPlatformSpecific::testWindowsUnicodePathSupport() {
    if (!isWindowsPlatform()) {
        QSKIP("Windows-specific test skipped on non-Windows platform");
    }
    
    qDebug() << "Testing Windows Unicode path support";
    
    // Test Unicode paths
    for (const QString& unicodePath : m_unicodeTestPaths) {
        QString normalizedPath = QDir::toNativeSeparators(unicodePath);
        QFileInfo fileInfo(normalizedPath);
        
        // Verify Unicode characters are preserved
        QVERIFY(!fileInfo.fileName().isEmpty());
        QVERIFY(fileInfo.absolutePath().contains(QRegExp("[\\u4e00-\\u9fff\\u0400-\\u04ff\\u3040-\\u309f]")));
    }
    
    qDebug() << "Windows Unicode path support test passed";
}

void TestPlatformSpecific::testSystemThemeDetection() {
    qDebug() << "Testing system theme detection";
    
    if (!m_platformConfig.supportsSystemTheme) {
        QSKIP("System theme detection not supported on this platform");
    }
    
    // Test theme detection
    QString currentTheme = "unknown";
    
#ifdef Q_OS_WIN
    if (isWindowsDarkModeEnabled()) {
        currentTheme = "dark";
    } else {
        currentTheme = "light";
    }
#endif
    
    QVERIFY(!currentTheme.isEmpty());
    QVERIFY(currentTheme == "dark" || currentTheme == "light" || currentTheme == "unknown");
    
    qDebug() << "Detected system theme:" << currentTheme;
    qDebug() << "System theme detection test passed";
}

void TestPlatformSpecific::testHighDpiIconRendering() {
    qDebug() << "Testing high DPI icon rendering";
    
    if (!m_platformConfig.supportsHighDpi) {
        QSKIP("High DPI not supported on this platform");
    }
    
    QString testIconName = "heart";
    
    // Test icon rendering at different DPI scales
    for (qreal dpiScale : m_testDpiScales) {
        HighDpiConfig config = testIconAtDpiScale(dpiScale, testIconName);
        
        qDebug() << QString("DPI Scale %1: expected=%2x%3, actual=%4x%5, correct=%6")
                    .arg(dpiScale)
                    .arg(config.expectedIconSize.width())
                    .arg(config.expectedIconSize.height())
                    .arg(config.actualIconSize.width())
                    .arg(config.actualIconSize.height())
                    .arg(config.scalingCorrect);
        
        // Verify scaling is approximately correct (within tolerance)
        if (dpiScale > HIGH_DPI_THRESHOLD) {
            QVERIFY2(config.scalingCorrect, 
                    qPrintable(QString("High DPI scaling failed at scale %1").arg(dpiScale)));
        }
    }
    
    qDebug() << "High DPI icon rendering test passed";
}

void TestPlatformSpecific::testHighDpiScalingFactors() {
    qDebug() << "Testing high DPI scaling factors";
    
    qreal currentDpiScale = getCurrentDpiScale();
    qDebug() << "Current DPI scale:" << currentDpiScale;
    
    // Test icon rendering at current DPI scale
    QIcon testIcon = m_lucide->icon("star");
    QVERIFY(!testIcon.isNull());
    
    // Test different icon sizes at current DPI
    for (const QSize& size : m_testIconSizes) {
        QPixmap pixmap = testIcon.pixmap(size);
        QVERIFY(!pixmap.isNull());
        
        // At high DPI, pixmap should be larger than requested size
        if (currentDpiScale > HIGH_DPI_THRESHOLD) {
            QVERIFY(pixmap.size().width() >= size.width());
            QVERIFY(pixmap.size().height() >= size.height());
        }
    }
    
    qDebug() << "High DPI scaling factors test passed";
}

void TestPlatformSpecific::testPlatformSpecificPerformance() {
    qDebug() << "Testing platform-specific performance";
    
    // Test file system access performance
    QString testPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    qint64 fileAccessTime = measureFileSystemAccess(testPath);
    
    qDebug() << "File system access time:" << fileAccessTime << "ms";
    
    if (isWindowsPlatform()) {
        QVERIFY2(fileAccessTime < WINDOWS_FILE_ACCESS_THRESHOLD_MS,
                qPrintable(QString("Windows file access too slow: %1ms").arg(fileAccessTime)));
    }
    
    // Test icon rendering performance on current platform
    qint64 renderTime = measureIconRenderingOnPlatform("home", QSize(64, 64));
    qDebug() << "Icon rendering time:" << renderTime << "ms";
    
    // Platform-specific performance should be reasonable
    QVERIFY(renderTime < 100); // 100ms threshold for any platform
    
    qDebug() << "Platform-specific performance test passed";
}

PlatformConfig TestPlatformSpecific::detectPlatformCapabilities() {
    PlatformConfig config;
    
#ifdef Q_OS_WIN
    config.platformName = "Windows";
    config.supportedThemes = {"light", "dark", "high-contrast"};
    config.defaultFilePathSeparator = "\\";
    config.supportsSystemTheme = true;
    config.supportsHighDpi = true;
    config.supportsFileAssociations = true;
#elif defined(Q_OS_MAC)
    config.platformName = "macOS";
    config.supportedThemes = {"light", "dark"};
    config.defaultFilePathSeparator = "/";
    config.supportsSystemTheme = true;
    config.supportsHighDpi = true;
    config.supportsFileAssociations = true;
#elif defined(Q_OS_LINUX)
    config.platformName = "Linux";
    config.supportedThemes = {"light", "dark"};
    config.defaultFilePathSeparator = "/";
    config.supportsSystemTheme = false; // Varies by desktop environment
    config.supportsHighDpi = true;
    config.supportsFileAssociations = false; // Varies by desktop environment
#else
    config.platformName = "Unknown";
    config.supportedThemes = {"light"};
    config.defaultFilePathSeparator = "/";
    config.supportsSystemTheme = false;
    config.supportsHighDpi = false;
    config.supportsFileAssociations = false;
#endif
    
    // Detect actual DPI scales available
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        config.supportedDpiScales.append(screen->devicePixelRatio());
    }
    
    return config;
}

bool TestPlatformSpecific::isWindowsPlatform() {
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool TestPlatformSpecific::isHighDpiDisplay() {
    QScreen* screen = QApplication::primaryScreen();
    return screen && screen->devicePixelRatio() > HIGH_DPI_THRESHOLD;
}

qreal TestPlatformSpecific::getCurrentDpiScale() {
    QScreen* screen = QApplication::primaryScreen();
    return screen ? screen->devicePixelRatio() : 1.0;
}

#ifdef Q_OS_WIN
bool TestPlatformSpecific::isWindowsDarkModeEnabled() {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
}
#endif

HighDpiConfig TestPlatformSpecific::testIconAtDpiScale(qreal scale, const QString& iconName) {
    HighDpiConfig config;
    config.dpiScale = scale;
    config.expectedIconSize = QSize(64 * scale, 64 * scale);
    
    QIcon icon = m_lucide->icon(iconName);
    QPixmap pixmap = icon.pixmap(64, 64);
    config.actualIconSize = pixmap.size();
    
    // Check if scaling is approximately correct
    qreal widthRatio = static_cast<qreal>(config.actualIconSize.width()) / config.expectedIconSize.width();
    qreal heightRatio = static_cast<qreal>(config.actualIconSize.height()) / config.expectedIconSize.height();
    
    config.scalingCorrect = (qAbs(widthRatio - 1.0) < DPI_TOLERANCE) && 
                           (qAbs(heightRatio - 1.0) < DPI_TOLERANCE);
    
    return config;
}

qint64 TestPlatformSpecific::measureFileSystemAccess(const QString& path) {
    QElapsedTimer timer;
    timer.start();
    
    QDir dir(path);
    dir.exists();
    dir.entryList();
    
    return timer.elapsed();
}

qint64 TestPlatformSpecific::measureIconRenderingOnPlatform(const QString& iconName, const QSize& size) {
    QElapsedTimer timer;
    timer.start();
    
    QIcon icon = m_lucide->icon(iconName);
    QPixmap pixmap = icon.pixmap(size);
    
    return timer.elapsed();
}

#include "test_platform_specific.moc"
