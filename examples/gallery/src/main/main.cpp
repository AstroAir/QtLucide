/**
 * @file main.cpp
 * @brief Application entry point for QtLucide Gallery
 * @details Main function sets up QApplication, configures Qt settings,
 *          and launches the main gallery window.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <iostream>

// Include the main window
#include "../ui/windows/GalleryMainWindow.h"

using namespace gallery;

/**
 * @brief Parse command-line arguments
 * @param argc Argument count
 * @param argv Argument values
 * @return true if smoke test flag is present, false otherwise
 */
bool parseSmokeTestFlag(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (QString(argv[i]) == QStringLiteral("--smoke")) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Configure Qt application settings
 * @param app The QApplication instance
 */
void configureApplication(QApplication& app) {
    // Set application metadata
    app.setApplicationName(QStringLiteral("QtLucide Gallery"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setApplicationDisplayName(QStringLiteral("QtLucide Icon Gallery"));

    // Set organization name for QSettings
    app.setOrganizationName(QStringLiteral("QtLucide"));
    app.setOrganizationDomain(QStringLiteral("qtlucide.example.com"));

    // Note: High DPI support is always enabled in Qt6, no need to set attributes

    // Configure graphics backend
    QSurfaceFormat format;
    format.setVersion(4, 3); // OpenGL 4.3 if available
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(format);

    // Use Fusion style for consistent look across platforms
    app.setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    // Set color palette for dark theme by default
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
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));

    app.setPalette(darkPalette);

    // Initialize settings paths
    QSettings settings(QStringLiteral("QtLucide"), QStringLiteral("Gallery"));

    // Ensure configuration directory exists
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir configDir(configPath);
    if (!configDir.exists()) {
        configDir.mkpath(QStringLiteral("."));
    }
}

/**
 * @brief Main entry point
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code
 */
int main(int argc, char* argv[]) {
    // Create QApplication
    QApplication app(argc, argv);

    // Configure Qt application
    configureApplication(app);

    // Check for smoke test flag
    bool isSmokeTest = parseSmokeTestFlag(argc, argv);

    if (isSmokeTest) {
        // Smoke test mode: initialize and exit immediately for CI testing
        std::cout << "Running in smoke test mode..." << std::endl;

        // Create and show main window for a brief moment
        GalleryMainWindow window;
        window.show();

        // Process events to ensure window is painted
        QApplication::processEvents();

        std::cout << "Smoke test passed: window initialized successfully" << std::endl;
        return 0; // Exit immediately after smoke test
    }

    // Normal mode: create and show main window
    GalleryMainWindow window;
    window.show();

    // Run the application event loop
    return app.exec();
}
