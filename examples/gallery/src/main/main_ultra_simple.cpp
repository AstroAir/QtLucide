/**
 * QtLucide Gallery Application - Ultra Simple Image Viewer Test
 *
 * This is the most minimal version that demonstrates just the core image viewing
 * functionality without any complex dependencies.
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

#include "ImageViewerWidget.h"
#include "ContentManager.h"
#include "IconMetadataManager.h"
#include "GalleryLogger.h"
#include "QtLucide/QtLucide.h"

class UltraSimpleImageViewerWindow : public QMainWindow
{
    Q_OBJECT

public:
    UltraSimpleImageViewerWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_lucide(new lucide::QtLucide(this))
        , m_iconMetadataManager(new IconMetadataManager(this))
        , m_contentManager(new ContentManager(this))
        , m_imageViewer(new ImageViewerWidget(this))
    {
        setWindowTitle("QtLucide Icon & Image Gallery");
        setMinimumSize(800, 600);
        resize(1000, 700);

        setupComponents();
        setupUI();
        setupMenus();
        loadAllIcons();

        statusBar()->showMessage("QtLucide Gallery initialized - Viewing all available icons");
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
        QMessageBox::about(this, "About QtLucide Icon & Image Gallery",
            "<h3>QtLucide Icon & Image Gallery</h3>"
            "<p>A unified gallery application for viewing both Lucide icons and regular images. "
            "Browse through all available Lucide icons or load your own image files.</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>Browse all 1600+ Lucide icons with metadata</li>"
            "<li>View images in multiple formats (PNG, JPEG, GIF, BMP, SVG, WebP, TIFF, ICO)</li>"
            "<li>Advanced zoom, rotate, and flip transformations</li>"
            "<li>Multiple view modes (fit to window, actual size, etc.)</li>"
            "<li>Slideshow mode with configurable timing</li>"
            "<li>Fullscreen viewing and keyboard shortcuts</li>"
            "<li>Unified navigation for both icons and images</li>"
            "</ul>"
            "<p>Built with Qt " QT_VERSION_STR " and QtLucide</p>");
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
        connect(openButton, &QPushButton::clicked, this, &UltraSimpleImageViewerWindow::onOpenImage);
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
        connect(openAction, &QAction::triggered, this, &UltraSimpleImageViewerWindow::onOpenImage);

        QAction* loadIconsAction = fileMenu->addAction("&Reload All Icons");
        loadIconsAction->setShortcut(QKeySequence::Refresh);
        connect(loadIconsAction, &QAction::triggered, this, &UltraSimpleImageViewerWindow::loadAllIcons);

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
        connect(aboutAction, &QAction::triggered, this, &UltraSimpleImageViewerWindow::onAbout);

        QAction* aboutQtAction = helpMenu->addAction("About &Qt");
        connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    }

    void setupComponents()
    {
        // Initialize QtLucide
        if (!m_lucide->initLucide()) {
            qWarning() << "Failed to initialize QtLucide";
            return;
        }

        // Setup content manager with QtLucide
        m_contentManager->setLucide(m_lucide);
        m_contentManager->setIconMetadataManager(m_iconMetadataManager);

        // Setup image viewer with content manager
        m_imageViewer->setContentManager(m_contentManager);

        qDebug() << "Components initialized successfully";
    }

    void loadAllIcons()
    {
        // Get all available icons
        QStringList allIcons = m_lucide->availableIcons();
        qDebug() << "Loading" << allIcons.size() << "icons";

        if (!allIcons.isEmpty()) {
            // Set the icon list in the image viewer
            m_imageViewer->setImageList(allIcons);

            // Start with the first icon
            m_imageViewer->setCurrentImage(allIcons.first());

            statusBar()->showMessage(QString("Loaded %1 icons - Use navigation controls to browse").arg(allIcons.size()));
        } else {
            statusBar()->showMessage("No icons found");
        }
    }

private:
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_iconMetadataManager;
    ContentManager* m_contentManager;
    ImageViewerWidget* m_imageViewer;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Icon & Image Gallery");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide Project");
    app.setApplicationDisplayName("QtLucide Icon & Image Gallery");

    // Initialize logging
    GALLERY_LOG_INFO(galleryInit, "Starting Ultra Simple Image Viewer Application");

    UltraSimpleImageViewerWindow window;
    window.show();

    GALLERY_LOG_INFO(galleryInit, "Application window shown, entering event loop");

    int result = app.exec();

    GALLERY_LOG_INFO(galleryInit, QString("Application exiting with code %1").arg(result));

    return result;
}

#include "main_ultra_simple.moc"
