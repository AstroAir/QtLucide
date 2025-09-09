/**
 * QtLucide Gallery Application - Enhanced Main Entry Point
 *
 * This is the enhanced version of the QtLucide Gallery with comprehensive features:
 * - Thumbnail grid view with virtual scrolling
 * - Category filtering sidebar (9 categories)
 * - Real-time search with 916+ tags
 * - Favorites/bookmarks system
 * - Export and code generation
 * - Advanced navigation and keyboard shortcuts
 * - Customizable settings and themes
 */

#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QDebug>

#include "../ui/windows/GalleryMainWindow.h"
#include "GalleryLogger.h"

void setupApplicationProperties(QApplication& app)
{
    app.setApplicationName("QtLucide Enhanced Gallery");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("QtLucide Project");
    app.setOrganizationDomain("qtlucide.org");
    app.setApplicationDisplayName("QtLucide Enhanced Icon & Image Gallery");

    // Set application icon (if available)
    // app.setWindowIcon(QIcon(":/icons/app-icon.png"));

    // High DPI support is enabled by default in Qt 6
}

void setupApplicationStyle(QApplication& app)
{
    // Try to use a modern style
    QStringList availableStyles = QStyleFactory::keys();

    // Prefer Fusion style for consistent cross-platform appearance
    if (availableStyles.contains("Fusion", Qt::CaseInsensitive)) {
        app.setStyle("Fusion");
    }

    // Apply a modern dark palette (optional)
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    // Uncomment to apply dark theme by default
    // app.setPalette(darkPalette);
}

void createApplicationDirectories()
{
    // Ensure application data directories exist
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataDir);

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);

    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(cacheDir);

    qDebug() << "Application directories:";
    qDebug() << "  Data:" << appDataDir;
    qDebug() << "  Config:" << configDir;
    qDebug() << "  Cache:" << cacheDir;
}

QSplashScreen* createSplashScreen()
{
    // Create a simple splash screen
    QPixmap splashPixmap(400, 300);
    splashPixmap.fill(QColor(42, 130, 218));

    QPainter painter(&splashPixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));

    // Draw application name
    painter.drawText(splashPixmap.rect(), Qt::AlignCenter,
                    "QtLucide Enhanced Gallery\n\nLoading...");

    QSplashScreen* splash = new QSplashScreen(splashPixmap);
    splash->show();

    return splash;
}

void showSplashMessage(QSplashScreen* splash, const QString& message)
{
    if (splash) {
        splash->showMessage(message, Qt::AlignBottom | Qt::AlignCenter, Qt::white);
        QApplication::processEvents();
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Setup application properties
    setupApplicationProperties(app);

    // Initialize logging
    GALLERY_LOG_INFO(galleryInit, "Starting QtLucide Enhanced Gallery Application");

    // Create splash screen
    QSplashScreen* splash = createSplashScreen();
    showSplashMessage(splash, "Initializing application...");

    // Setup application style and directories
    setupApplicationStyle(app);
    createApplicationDirectories();

    showSplashMessage(splash, "Loading QtLucide icons...");

    // Create and show main window
    GalleryMainWindow* mainWindow = nullptr;

    try {
        mainWindow = new GalleryMainWindow();

        showSplashMessage(splash, "Setting up user interface...");
        QTimer::singleShot(1000, [splash, mainWindow]() {
            if (splash) {
                splash->finish(mainWindow);
                splash->deleteLater();
            }
            mainWindow->show();
            mainWindow->raise();
            mainWindow->activateWindow();
        });

        GALLERY_LOG_INFO(galleryInit, "Enhanced Gallery Main Window created successfully");

    } catch (const std::exception& e) {
        GALLERY_LOG_CRITICAL(galleryInit, QString("Failed to create main window: %1").arg(e.what()));

        if (splash) {
            splash->close();
            splash->deleteLater();
        }

        QMessageBox::critical(nullptr, "Initialization Error",
                             QString("Failed to initialize the application:\n%1").arg(e.what()));
        return 1;
    }

    GALLERY_LOG_INFO(galleryInit, "Application window shown, entering event loop");

    // Run the application
    int result = app.exec();

    GALLERY_LOG_INFO(galleryInit, QString("Application exiting with code %1").arg(result));

    // Cleanup
    if (mainWindow) {
        delete mainWindow;
    }

    return result;
}
