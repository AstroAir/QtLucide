/**
 * QtLucide Gallery Application - Main Entry Point
 *
 * Comprehensive QtLucide Gallery application featuring:
 * - Icon browsing and search functionality
 * - Image gallery with metadata support
 * - Theme switching and customization
 * - Export/import capabilities
 * - Responsive layout system
 */

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QTimer>

#include "../core/utils/GalleryLogger.h"
#include "../ui/windows/GalleryMainWindow.h"
#include <QtLucide/QtLucide.h>

void setupApplicationProperties(QApplication& app) {
    app.setApplicationName("QtLucide Gallery");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("QtLucide");
    app.setOrganizationDomain("qtlucide.org");
    app.setApplicationDisplayName("QtLucide Gallery");

    // Enable high DPI support
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
}

QSplashScreen* createSplashScreen() {
    QPixmap splashPixmap(400, 300);
    splashPixmap.fill(QColor(45, 52, 54));

    QPainter painter(&splashPixmap);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(splashPixmap.rect(), Qt::AlignCenter, "QtLucide Gallery\nLoading...");

    auto* splash = new QSplashScreen(splashPixmap);
    splash->show();
    return splash;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Setup application properties
    setupApplicationProperties(app);

    // Initialize logging
    GalleryLogger::initialize();
    GALLERY_LOG_INFO(galleryMain, "Starting QtLucide Gallery application");

    // Create and show splash screen
    QSplashScreen* splash = createSplashScreen();
    splash->showMessage("Initializing QtLucide...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    // Initialize QtLucide
    lucide::QtLucide lucide;
    if (!lucide.initLucide()) {
        QMessageBox::critical(nullptr, "Error",
                              "Failed to initialize QtLucide library.\n"
                              "The application will continue with limited functionality.");
        GALLERY_LOG_ERROR(galleryMain, "Failed to initialize QtLucide library");
    }

    splash->showMessage("Creating main window...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    // Create main window
    GalleryMainWindow mainWindow;

    splash->showMessage("Loading complete!", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    // Show main window and hide splash
    QTimer::singleShot(1000, [&]() {
        mainWindow.show();
        splash->finish(&mainWindow);
        splash->deleteLater();
    });

    GALLERY_LOG_INFO(galleryMain, "QtLucide Gallery started successfully");

    return app.exec();
