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
#include <fstream>

#include "../core/utils/GalleryLogger.h"
#include "../ui/windows/GalleryMainWindow.h"
#include <QtLucide/QtLucide.h>

static bool hasArg(int argc, char* argv[], const char* flag) {
    for (int i = 1; i < argc; ++i) {
        if (QString::fromLatin1(argv[i]) == QLatin1String(flag))
            return true;
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
    // Write to a debug file to confirm main() is called
    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "=== NEW RUN ===" << std::endl;
        debugFile << "main() called with " << argc << " arguments" << std::endl;
        for (int i = 0; i < argc; ++i) {
            debugFile << "argv[" << i << "] = " << argv[i] << std::endl;
        }
        debugFile.flush();
    }

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to check smoke mode" << std::endl;
        debugFile.flush();
    }

    const bool smokeMode = hasArg(argc, argv, "--smoke");

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Smoke mode: " << (smokeMode ? "true" : "false") << std::endl;
        debugFile.flush();
    }

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

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Not in smoke mode, continuing with normal execution" << std::endl;
        debugFile.flush();
    }

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to create QApplication" << std::endl;
        debugFile.flush();
    }

    QApplication app(argc, argv);

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "QApplication created successfully" << std::endl;
        debugFile.flush();
    }

    // Setup application properties
    try {
        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "About to call setupApplicationProperties" << std::endl;
            debugFile.flush();
        }

        setupApplicationProperties(app);

        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "Application properties set" << std::endl;
            debugFile.flush();
        }
    } catch (const std::exception& e) {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Exception in setupApplicationProperties: " << e.what() << std::endl;
        debugFile.flush();
        return 1;
    } catch (...) {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Unknown exception in setupApplicationProperties" << std::endl;
        debugFile.flush();
        return 1;
    }

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Passed setupApplicationProperties try-catch block" << std::endl;
        debugFile.flush();
    }

    // Initialize logging (with our threading fixes)
    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to initialize GalleryLogger" << std::endl;
        debugFile.flush();
    }

    GalleryLogger::instance()->initialize();

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "GalleryLogger initialized successfully" << std::endl;
        debugFile.flush();
    }

    // Skip GALLERY_LOG_INFO for now to avoid potential issues
    // GALLERY_LOG_INFO(galleryMain, "Starting QtLucide Gallery application");

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Skipping Gallery log message for now" << std::endl;
        debugFile.flush();
    }

    // Create and show splash screen
    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to create splash screen" << std::endl;
        debugFile.flush();
    }

    QSplashScreen* splash = createSplashScreen();

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Splash screen created successfully" << std::endl;
        debugFile.flush();
    }

    splash->showMessage("Initializing QtLucide...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Splash screen message set and events processed" << std::endl;
        debugFile.flush();
    }

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to proceed to QtLucide initialization section" << std::endl;
        debugFile.flush();
    }

    // Initialize QtLucide
    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to initialize QtLucide" << std::endl;
        debugFile.flush();
    }

    lucide::QtLucide lucide;

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "QtLucide instance created" << std::endl;
        debugFile.flush();
    }

    if (!lucide.initLucide()) {
        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "QtLucide initialization failed" << std::endl;
            debugFile.flush();
        }
        QMessageBox::critical(nullptr, "Error",
                              "Failed to initialize QtLucide library.\n"
                              "The application will continue with limited functionality.");
    } else {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "QtLucide initialized successfully" << std::endl;
        debugFile.flush();

        debugFile << "Exiting QtLucide success block" << std::endl;
        debugFile.flush();
    }

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to update splash message" << std::endl;
        debugFile.flush();
    }

    splash->showMessage("Creating main window...", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
    app.processEvents();

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "Splash message updated and events processed" << std::endl;
        debugFile.flush();
    }

    {
        std::ofstream debugFile("debug_main.txt", std::ios::app);
        debugFile << "About to create main window" << std::endl;
        debugFile.flush();
    }

    // Create main window
    try {
        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "About to call GalleryMainWindow constructor" << std::endl;
            debugFile.flush();
        }

        GalleryMainWindow mainWindow(&lucide);

        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "GalleryMainWindow created successfully" << std::endl;
            debugFile.flush();
        }

        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "About to show splash loading complete message" << std::endl;
            debugFile.flush();
        }

        splash->showMessage("Loading complete!", Qt::AlignBottom | Qt::AlignCenter, Qt::white);
        app.processEvents();

        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "Splash message updated and events processed" << std::endl;
            debugFile.flush();
        }

        // Show main window and hide splash
        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "About to show main window" << std::endl;
            debugFile.flush();
        }

        mainWindow.show();

        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "Main window shown successfully" << std::endl;
            debugFile.flush();
        }

        splash->finish(&mainWindow);
        splash->deleteLater();

        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "About to call GALLERY_LOG_INFO" << std::endl;
            debugFile.flush();
        }

        // GALLERY_LOG_INFO(galleryMain, "QtLucide Gallery started successfully");
        // Temporarily comment out the logging call to see if it's causing issues

        {
            std::ofstream debugFile("debug_main.txt", std::ios::app);
            debugFile << "About to enter app.exec()" << std::endl;
            debugFile.flush();
        }

        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "Exception caught:" << e.what();
        QMessageBox::critical(nullptr, "Error",
                              QString("Application failed to start: %1").arg(e.what()));
        return 1;
    } catch (...) {
        qCritical() << "Unknown exception caught";
        QMessageBox::critical(nullptr, "Error",
                              "Application failed to start due to an unknown error");
        return 1;
    }
}
