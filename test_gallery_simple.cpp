#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>

#include "QtLucide/QtLucide.h"

class SimpleTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimpleTestWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        qDebug() << "SimpleTestWindow constructor started";
        
        setWindowTitle("Simple Gallery Test");
        setMinimumSize(800, 600);
        
        // Create central widget
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);
        
        // Add a simple label
        QLabel* label = new QLabel("Gallery Test Window", this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        
        // Test QtLucide initialization
        lucide::QtLucide* lucide = new lucide::QtLucide(this);
        if (lucide->initLucide()) {
            QLabel* statusLabel = new QLabel(QString("QtLucide initialized successfully with %1 icons").arg(lucide->availableIcons().size()), this);
            layout->addWidget(statusLabel);
            
            // Test creating an icon
            QIcon testIcon = lucide->icon("heart");
            if (!testIcon.isNull()) {
                QLabel* iconLabel = new QLabel(this);
                iconLabel->setPixmap(testIcon.pixmap(QSize(64, 64)));
                iconLabel->setAlignment(Qt::AlignCenter);
                layout->addWidget(iconLabel);
                
                QLabel* iconStatusLabel = new QLabel("Heart icon loaded successfully", this);
                iconStatusLabel->setAlignment(Qt::AlignCenter);
                layout->addWidget(iconStatusLabel);
            } else {
                QLabel* errorLabel = new QLabel("Failed to create heart icon", this);
                layout->addWidget(errorLabel);
            }
        } else {
            QLabel* errorLabel = new QLabel("Failed to initialize QtLucide", this);
            layout->addWidget(errorLabel);
        }
        
        qDebug() << "SimpleTestWindow constructor completed";
    }
};

#include "test_gallery_simple.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    qDebug() << "Application started";
    
    try {
        SimpleTestWindow window;
        qDebug() << "Window created, about to show";
        window.show();
        qDebug() << "Window shown, starting event loop";
        
        return app.exec();
    } catch (const std::exception& e) {
        qDebug() << "Exception caught:" << e.what();
        QMessageBox::critical(nullptr, "Error", QString("Exception: %1").arg(e.what()));
        return 1;
    } catch (...) {
        qDebug() << "Unknown exception caught";
        QMessageBox::critical(nullptr, "Error", "Unknown exception occurred");
        return 1;
    }
}
