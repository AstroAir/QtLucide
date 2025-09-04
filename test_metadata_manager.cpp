#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "QtLucide/QtLucide.h"
#include "examples/gallery/IconMetadataManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "Testing IconMetadataManager";

    // Initialize QtLucide first to ensure resources are available
    lucide::QtLucide* lucide = new lucide::QtLucide();
    if (!lucide->initLucide()) {
        qDebug() << "Failed to initialize QtLucide";
        return 1;
    }
    qDebug() << "QtLucide initialized successfully";

    IconMetadataManager* manager = new IconMetadataManager();

    // Connect signals
    QObject::connect(manager, &IconMetadataManager::metadataLoaded,
                    [](int count) {
                        qDebug() << "metadataLoaded signal received with count:" << count;
                    });

    QObject::connect(manager, &IconMetadataManager::metadataLoadFailed,
                    [](const QString& error) {
                        qDebug() << "metadataLoadFailed signal received:" << error;
                    });

    qDebug() << "About to call loadMetadata()";
    bool result = manager->loadMetadata();
    qDebug() << "loadMetadata() returned:" << result;

    if (result) {
        QStringList icons = manager->getAllIconNames();
        qDebug() << "Total icons:" << icons.size();

        QStringList categories = manager->getAllCategories();
        qDebug() << "Total categories:" << categories.size();

        QStringList tags = manager->getAllTags();
        qDebug() << "Total tags:" << tags.size();
    }

    qDebug() << "Test completed";

    // Exit after a short delay
    QTimer::singleShot(1000, &app, &QApplication::quit);

    return app.exec();
}
