#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Submodule Example");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide");

    MainWindow window;
    window.show();

    return app.exec();
}
