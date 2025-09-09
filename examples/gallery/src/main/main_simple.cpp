/**
 * QtLucide Gallery Application - Simple Image Viewer Test
 *
 * This is a minimal version that demonstrates the core image viewing
 * functionality without complex file browser components.
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDebug>

#include "ImageMetadataManager.h"
#include "ContentManager.h"
#include "ImageViewerWidget.h"
#include "GalleryLogger.h"

class SimpleImageViewerWindow : public QMainWindow
{
    Q_OBJECT

public:
    SimpleImageViewerWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_imageMetadataManager(new ImageMetadataManager(this))
        , m_contentManager(new ContentManager(this))
        , m_imageViewer(new ImageViewerWidget(this))
    {
        setWindowTitle("QtLucide Simple Image Viewer");
        setMinimumSize(800, 600);
        resize(1000, 700);

        setupUI();
        setupMenus();
        setupConnections();
        initializeComponents();

        statusBar()->showMessage("Simple Image Viewer initialized - Ready to view images");
    }

private slots:
    void onOpenImage()
    {
        QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open Image",
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            "Image Files (*.png *.jpg *.jpeg *.gif *.bmp *.svg *.webp *.tiff *.ico)"
        );

        if (!fileName.isEmpty()) {
            m_imageViewer->setCurrentImage(fileName);
            QFileInfo fileInfo(fileName);
            statusBar()->showMessage(QString("Loaded: %1").arg(fileInfo.fileName()));
        }
    }

    void onAbout()
    {
        QMessageBox::about(this, "About Simple Image Viewer",
            "<h3>QtLucide Simple Image Viewer</h3>"
            "<p>This application demonstrates the core image viewing functionality "
            "of the QtLucide Gallery application.</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>View images in multiple formats</li>"
            "<li>Zoom, rotate, and flip images</li>"
            "<li>Multiple view modes (fit to window, actual size, etc.)</li>"
            "<li>Keyboard and mouse controls</li>"
            "</ul>"
            "<p>Built with Qt " QT_VERSION_STR "</p>");
    }

private:
    void setupUI()
    {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->setContentsMargins(4, 4, 4, 4);
        mainLayout->setSpacing(4);

        // Add toolbar with basic controls
        QHBoxLayout* toolbarLayout = new QHBoxLayout();

        QPushButton* openButton = new QPushButton("Open Image", this);
        connect(openButton, &QPushButton::clicked, this, &SimpleImageViewerWindow::onOpenImage);
        toolbarLayout->addWidget(openButton);

        QPushButton* fitButton = new QPushButton("Fit to Window", this);
        connect(fitButton, &QPushButton::clicked, m_imageViewer, &ImageViewerWidget::onFitToWindow);
        toolbarLayout->addWidget(fitButton);

        QPushButton* actualSizeButton = new QPushButton("Actual Size", this);
        connect(actualSizeButton, &QPushButton::clicked, m_imageViewer, &ImageViewerWidget::onActualSize);
        toolbarLayout->addWidget(actualSizeButton);

        QPushButton* rotateButton = new QPushButton("Rotate", this);
        connect(rotateButton, &QPushButton::clicked, m_imageViewer, &ImageViewerWidget::rotateRight);
        toolbarLayout->addWidget(rotateButton);

        toolbarLayout->addStretch();

        mainLayout->addLayout(toolbarLayout);
        mainLayout->addWidget(m_imageViewer, 1);
    }

    void setupMenus()
    {
        // File menu
        QMenu* fileMenu = menuBar()->addMenu("&File");

        QAction* openAction = fileMenu->addAction("&Open Image...");
        openAction->setShortcut(QKeySequence::Open);
        connect(openAction, &QAction::triggered, this, &SimpleImageViewerWindow::onOpenImage);

        fileMenu->addSeparator();

        QAction* exitAction = fileMenu->addAction("E&xit");
        exitAction->setShortcut(QKeySequence::Quit);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);

        // View menu
        QMenu* viewMenu = menuBar()->addMenu("&View");

        QAction* fitToWindowAction = viewMenu->addAction("Fit to &Window");
        fitToWindowAction->setShortcut(Qt::Key_F);
        connect(fitToWindowAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::onFitToWindow);

        QAction* actualSizeAction = viewMenu->addAction("&Actual Size");
        actualSizeAction->setShortcut(Qt::Key_0);
        connect(actualSizeAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::onActualSize);

        QAction* zoomInAction = viewMenu->addAction("Zoom &In");
        zoomInAction->setShortcut(QKeySequence::ZoomIn);
        connect(zoomInAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::onZoomIn);

        QAction* zoomOutAction = viewMenu->addAction("Zoom &Out");
        zoomOutAction->setShortcut(QKeySequence::ZoomOut);
        connect(zoomOutAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::onZoomOut);

        viewMenu->addSeparator();

        QAction* rotateLeftAction = viewMenu->addAction("Rotate &Left");
        rotateLeftAction->setShortcut(Qt::Key_L);
        connect(rotateLeftAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::rotateLeft);

        QAction* rotateRightAction = viewMenu->addAction("Rotate &Right");
        rotateRightAction->setShortcut(Qt::Key_R);
        connect(rotateRightAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::rotateRight);

        QAction* flipHorizontalAction = viewMenu->addAction("Flip &Horizontal");
        flipHorizontalAction->setShortcut(Qt::Key_H);
        connect(flipHorizontalAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::flipHorizontal);

        QAction* flipVerticalAction = viewMenu->addAction("Flip &Vertical");
        flipVerticalAction->setShortcut(Qt::Key_V);
        connect(flipVerticalAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::flipVertical);

        // Help menu
        QMenu* helpMenu = menuBar()->addMenu("&Help");

        QAction* aboutAction = helpMenu->addAction("&About");
        connect(aboutAction, &QAction::triggered, this, &SimpleImageViewerWindow::onAbout);

        QAction* aboutQtAction = helpMenu->addAction("About &Qt");
        connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    }

    void setupConnections()
    {
        // Connect image viewer signals
        connect(m_imageViewer, &ImageViewerWidget::imageChanged,
                this, [this](const QString& identifier, int index) {
                    Q_UNUSED(index)
                    QFileInfo fileInfo(identifier);
                    statusBar()->showMessage(QString("Viewing: %1").arg(fileInfo.fileName()));
                });
    }

    void initializeComponents()
    {
        // Setup content manager
        m_contentManager->setImageMetadataManager(m_imageMetadataManager);

        // Setup image viewer
        m_imageViewer->setContentManager(m_contentManager);

        qDebug() << "Simple Image Viewer initialized successfully";
    }

private:
    ImageMetadataManager* m_imageMetadataManager;
    ContentManager* m_contentManager;
    ImageViewerWidget* m_imageViewer;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Simple Image Viewer");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide Project");
    app.setApplicationDisplayName("Simple Image Viewer");

    // Initialize logging
    GALLERY_LOG_INFO(galleryInit, "Starting Simple Image Viewer Application");

    SimpleImageViewerWindow window;
    window.show();

    GALLERY_LOG_INFO(galleryInit, "Application window shown, entering event loop");

    int result = app.exec();

    GALLERY_LOG_INFO(galleryInit, QString("Application exiting with code %1").arg(result));

    return result;
}

#include "main_simple.moc"
