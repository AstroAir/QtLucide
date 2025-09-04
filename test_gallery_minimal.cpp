#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <QTimer>

#include "QtLucide/QtLucide.h"
#include "examples/gallery/IconMetadataManager.h"

class MinimalGalleryWindow : public QMainWindow
{
    Q_OBJECT

public:
    MinimalGalleryWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        qDebug() << "MinimalGalleryWindow constructor started";
        
        setWindowTitle("Minimal Gallery Test");
        setMinimumSize(800, 600);
        
        // Test QtLucide
        qDebug() << "Creating QtLucide instance";
        m_lucide = new lucide::QtLucide(this);
        
        qDebug() << "Initializing QtLucide";
        if (!m_lucide->initLucide()) {
            qDebug() << "QtLucide initialization failed!";
            return;
        }
        qDebug() << "QtLucide initialized successfully";
        
        // Test IconMetadataManager
        qDebug() << "Creating IconMetadataManager";
        m_metadataManager = new IconMetadataManager(this);
        
        qDebug() << "Connecting signals";
        connect(m_metadataManager, &IconMetadataManager::metadataLoaded,
                this, [this](int count) {
                    qDebug() << "metadataLoaded signal received with count:" << count;
                });
        
        connect(m_metadataManager, &IconMetadataManager::metadataLoadFailed,
                this, [this](const QString& error) {
                    qDebug() << "metadataLoadFailed signal received:" << error;
                });
        
        qDebug() << "About to call loadMetadata()";
        bool result = m_metadataManager->loadMetadata();
        qDebug() << "loadMetadata() returned:" << result;
        
        if (result) {
            QStringList icons = m_metadataManager->getAllIconNames();
            qDebug() << "Total icons from metadata manager:" << icons.size();
        }
        
        qDebug() << "MinimalGalleryWindow constructor completed";
    }

private:
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
};

#include "test_gallery_minimal.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Application started";
    
    try {
        qDebug() << "Creating MinimalGalleryWindow";
        MinimalGalleryWindow window;
        qDebug() << "Window created, about to show";
        window.show();
        qDebug() << "Window shown";
        
        // Exit after a short delay
        QTimer::singleShot(2000, &app, &QApplication::quit);
        
        return app.exec();
    } catch (const std::exception& e) {
        qDebug() << "Exception caught:" << e.what();
        return 1;
    } catch (...) {
        qDebug() << "Unknown exception caught";
        return 1;
    }
}
