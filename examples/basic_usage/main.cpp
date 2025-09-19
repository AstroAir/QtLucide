/**
 * QtLucide Basic Usage Example
 *
 * This example demonstrates how to use QtLucide icons in a Qt application,
 * showing API compatibility with QtAwesome and various customization options.
 */

#include <QApplication>
#include <QDebug>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Example");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide");

    MainWindow window;
    window.show();

    return app.exec();
}
