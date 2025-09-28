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
#include <QCoreApplication>
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

static bool hasArg(int argc, char* argv[], const char* flag) {
    for (int i = 1; i < argc; ++i) {
        if (QString::fromLatin1(argv[i]) == QLatin1String(flag)) return true;
    }
    return false;
}

void setupApplicationProperties(QApplication& app) {
    app.setApplicationName("QtLucide Gallery");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("QtLucide");
    app.setOrganizationDomain("qtlucide.org");
    app.setApplicationDisplayName("QtLucide Gallery");

    // High DPI support is automatically enabled in Qt6
    // Qt::AA_EnableHighDpiScaling and Qt::AA_UseHighDpiPixmaps are deprecated and no longer needed
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
    const bool smokeMode = hasArg(argc, argv, "--smoke");

    if (smokeMode) {
        // Headless-friendly smoke run: use QApplication but don't create any windows
        QApplication app(argc, argv);
        lucide::QtLucide lucide;
        const bool ok = lucide.initLucide();
        if (!ok) {
            qCritical() << "[SMOKE] QtLucide initialization failed";
            return 1;
        }
        const auto icons = lucide.availableIcons();
        qInfo() << "[SMOKE] QtLucide initialized with" << icons.size() << "icons";
        return 0;
    }

    QApplication app(argc, argv);

    // Setup application properties
    setupApplicationProperties(app);

    // Initialize logging
    GalleryLogger::instance()->initialize();
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
    qDebug() << "Creating GalleryMainWindow...";
    try {
        GalleryMainWindow mainWindow;
        qDebug() << "GalleryMainWindow created successfully";

        splash->showMessage("Loading complete!", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
        app.processEvents();

        // Show main window and hide splash
        qDebug() << "Showing main window...";
        mainWindow.show();
        qDebug() << "Main window shown";
        splash->finish(&mainWindow);
        splash->deleteLater();
        qDebug() << "Splash screen finished";

        GALLERY_LOG_INFO(galleryMain, "QtLucide Gallery started successfully");

        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "Exception caught:" << e.what();
        QMessageBox::critical(nullptr, "Error", QString("Application failed to start: %1").arg(e.what()));
        return 1;
    } catch (...) {
        qCritical() << "Unknown exception caught";
        QMessageBox::critical(nullptr, "Error", "Application failed to start due to an unknown error");
        return 1;
    }
}
