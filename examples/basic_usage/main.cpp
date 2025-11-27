/**
 * QtLucide Basic Usage Example
 *
 * This example demonstrates how to use QtLucide icons in a Qt application,
 * showing API compatibility with QtAwesome and various customization options.
 */

#include <QApplication>
#include <QDebug>
#include <QtLucide/QtLucide.h>

#include "MainWindow.h"

static bool hasArg(int argc, char* argv[], const char* flag) {
    for (int i = 1; i < argc; ++i) {
        if (QString::fromLatin1(argv[i]) == QLatin1String(flag))
            return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    // Support smoke testing mode for automated testing
    const bool smokeMode = hasArg(argc, argv, "--smoke");

    if (smokeMode) {
        // Headless smoke test: verify QtLucide initialization without showing UI
        // Use QApplication even in smoke mode since QtLucide requires it for icon rendering
        QApplication app(argc, argv);
        app.setApplicationName("QtLucide Example");
        app.setApplicationVersion("1.0.0");
        app.setOrganizationName("QtLucide");

        lucide::QtLucide lucide;
        const bool ok = lucide.initLucide();
        if (!ok) {
            qCritical() << "[SMOKE] QtLucide initialization failed";
            return 1;
        }
        const auto icons = lucide.availableIcons();
        qInfo() << "[SMOKE] QtLucide Example initialized with" << icons.size() << "icons";
        return 0;
    }

    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Example");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide");

    MainWindow window;
    window.show();

    return app.exec();
}
