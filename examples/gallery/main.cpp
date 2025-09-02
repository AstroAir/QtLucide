/**
 * QtLucide Gallery Application - Main Entry Point
 * 
 * A comprehensive icon gallery application showcasing all QtLucide icons
 * with search, filtering, and preview capabilities similar to FontAwesome.
 */

#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDebug>

#include "GalleryMainWindow.h"

int qMain(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("QtLucide Gallery");
    app.setApplicationVersion("1.0.0");
    app.setApplicationDisplayName("QtLucide Icon Gallery");
    app.setOrganizationName("QtLucide");
    app.setOrganizationDomain("qtlucide.org");
    
    // Set application icon (using a QtLucide icon)
    // This will be set from within the main window once QtLucide is initialized
    
    // Apply modern styling
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Set up application palette for a modern look
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
    
    // Allow users to override with system theme
    if (app.arguments().contains("--system-theme")) {
        qDebug() << "Using system theme";
    } else {
        app.setPalette(darkPalette);
    }
    
    try {
        // Create and show main window
        GalleryMainWindow window;
        window.show();
        
        return app.exec();
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Application Error", 
                            QString("Failed to start QtLucide Gallery: %1").arg(e.what()));
        return 1;
    } catch (...) {
        QMessageBox::critical(nullptr, "Application Error", 
                            "An unknown error occurred while starting QtLucide Gallery.");
        return 1;
    }
}
