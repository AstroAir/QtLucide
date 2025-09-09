/**
 * QtLucide Gallery Application - Minimal Image Gallery Test
 *
 * This is a simplified version that focuses on testing the new image gallery
 * functionality without the complex existing components that have compilation issues.
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
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
#include "FileBrowserWidget.h"
#include "GalleryLogger.h"
#include <QtLucide/QtLucide.h>

class MinimalImageGalleryWindow : public QMainWindow
{
    Q_OBJECT

public:
    MinimalImageGalleryWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_lucide(new lucide::QtLucide(this))
        , m_imageMetadataManager(new ImageMetadataManager(this))
        , m_contentManager(new ContentManager(this))
        , m_imageViewer(new ImageViewerWidget(this))
        , m_fileBrowser(new FileBrowserWidget(this))
    {
        setWindowTitle("QtLucide Image Gallery - Test Application");
        setMinimumSize(1200, 800);
        resize(1400, 900);

        setupUI();
        setupMenus();
        setupConnections();
        initializeComponents();

        // Set initial directory to Pictures
        QString picturesPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (QDir(picturesPath).exists()) {
            m_fileBrowser->setCurrentDirectory(picturesPath);
        } else {
            m_fileBrowser->setCurrentDirectory(QDir::homePath());
        }

        statusBar()->showMessage("Image Gallery initialized - Ready to browse images");
    }

private slots:
    void onOpenDirectory()
    {
        QString dir = QFileDialog::getExistingDirectory(
            this,
            "Select Image Directory",
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
        );

        if (!dir.isEmpty()) {
            m_fileBrowser->setCurrentDirectory(dir);
            m_contentManager->loadImageDirectory(dir);
            statusBar()->showMessage(QString("Loaded directory: %1").arg(dir));
        }
    }

    void onImageSelected(const QString& imagePath)
    {
        QFileInfo fileInfo(imagePath);
        m_imageViewer->setCurrentImage(imagePath);
        statusBar()->showMessage(QString("Selected: %1").arg(fileInfo.fileName()));
    }

    void onDirectoryChanged(const QString& path)
    {
        statusBar()->showMessage(QString("Directory: %1").arg(path));
    }

    void onContentCountChanged(int total, int icons, int images)
    {
        QString message = QString("Content: %1 total (%2 icons, %3 images)")
                         .arg(total).arg(icons).arg(images);
        statusBar()->showMessage(message);
    }

    void onAbout()
    {
        QMessageBox::about(this, "About QtLucide Image Gallery",
            "<h3>QtLucide Image Gallery Test</h3>"
            "<p>This application demonstrates the enhanced image gallery functionality "
            "added to the QtLucide Gallery application.</p>"
            "<p><b>Features:</b></p>"
            "<ul>"
            "<li>Browse and view images in multiple formats</li>"
            "<li>Zoom, rotate, and flip images</li>"
            "<li>Slideshow mode with configurable timing</li>"
            "<li>Directory navigation with thumbnails</li>"
            "<li>Integration with QtLucide icons</li>"
            "</ul>"
            "<p>Built with Qt " QT_VERSION_STR "</p>");
    }

    void onTestLucideIcons()
    {
        if (m_lucide->initLucide()) {
            QStringList icons = m_lucide->availableIcons();
            QMessageBox::information(this, "QtLucide Test",
                QString("QtLucide successfully initialized with %1 icons available.")
                .arg(icons.size()));
            statusBar()->showMessage(QString("QtLucide: %1 icons available").arg(icons.size()));
        } else {
            QMessageBox::warning(this, "QtLucide Test",
                "Failed to initialize QtLucide. Icon functionality may not work.");
            statusBar()->showMessage("QtLucide initialization failed");
        }
    }

private:
    void setupUI()
    {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(4, 4, 4, 4);
        mainLayout->setSpacing(4);

        // Create main splitter
        QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);

        // Left panel - File browser
        m_fileBrowser->setMaximumWidth(350);
        m_fileBrowser->setMinimumWidth(250);

        // Right panel - Image viewer
        mainSplitter->addWidget(m_fileBrowser);
        mainSplitter->addWidget(m_imageViewer);
        mainSplitter->setSizes({300, 900});

        mainLayout->addWidget(mainSplitter);
    }

    void setupMenus()
    {
        // File menu
        QMenu* fileMenu = menuBar()->addMenu("&File");

        QAction* openDirAction = fileMenu->addAction("&Open Directory...");
        openDirAction->setShortcut(QKeySequence::Open);
        connect(openDirAction, &QAction::triggered, this, &MinimalImageGalleryWindow::onOpenDirectory);

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

        QAction* fullscreenAction = viewMenu->addAction("&Fullscreen");
        fullscreenAction->setShortcut(Qt::Key_F11);
        connect(fullscreenAction, &QAction::triggered, m_imageViewer, &ImageViewerWidget::onToggleFullscreen);

        // Tools menu
        QMenu* toolsMenu = menuBar()->addMenu("&Tools");

        QAction* testLucideAction = toolsMenu->addAction("Test &QtLucide Icons");
        connect(testLucideAction, &QAction::triggered, this, &MinimalImageGalleryWindow::onTestLucideIcons);

        // Help menu
        QMenu* helpMenu = menuBar()->addMenu("&Help");

        QAction* aboutAction = helpMenu->addAction("&About");
        connect(aboutAction, &QAction::triggered, this, &MinimalImageGalleryWindow::onAbout);

        QAction* aboutQtAction = helpMenu->addAction("About &Qt");
        connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    }

    void setupConnections()
    {
        // Connect file browser to image viewer
        connect(m_fileBrowser, &FileBrowserWidget::imageSelected,
                this, &MinimalImageGalleryWindow::onImageSelected);
        connect(m_fileBrowser, &FileBrowserWidget::directoryChanged,
                this, &MinimalImageGalleryWindow::onDirectoryChanged);

        // Connect content manager signals
        connect(m_contentManager, &ContentManager::contentLoaded,
                this, [this](const QString& identifier, ContentType type) {
                    QString typeStr = (type == ContentType::Icon) ? "icon" : "image";
                    qDebug() << "Content loaded:" << identifier << "(" << typeStr << ")";
                });

        connect(m_contentManager, &ContentManager::contentCountChanged,
                this, &MinimalImageGalleryWindow::onContentCountChanged);

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
        // Initialize QtLucide (optional - will work without it)
        if (!m_lucide->initLucide()) {
            qWarning() << "Failed to initialize QtLucide - icon functionality will be limited";
        }

        // Setup content manager
        m_contentManager->setLucide(m_lucide);
        m_contentManager->setImageMetadataManager(m_imageMetadataManager);

        // Setup image viewer
        m_imageViewer->setContentManager(m_contentManager);

        // Setup file browser
        m_fileBrowser->setContentManager(m_contentManager);
        m_fileBrowser->setImageMetadataManager(m_imageMetadataManager);
        m_fileBrowser->setShowOnlyImages(true); // Focus on images for this test

        qDebug() << "Minimal Image Gallery initialized successfully";
    }

private:
    lucide::QtLucide* m_lucide;
    ImageMetadataManager* m_imageMetadataManager;
    ContentManager* m_contentManager;
    ImageViewerWidget* m_imageViewer;
    FileBrowserWidget* m_fileBrowser;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Image Gallery Test");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide Project");
    app.setApplicationDisplayName("QtLucide Image Gallery");

    // Initialize logging
    GALLERY_LOG_INFO(galleryInit, "Starting QtLucide Image Gallery Test Application");

    MinimalImageGalleryWindow window;
    window.show();

    GALLERY_LOG_INFO(galleryInit, "Application window shown, entering event loop");

    int result = app.exec();

    GALLERY_LOG_INFO(galleryInit, QString("Application exiting with code %1").arg(result));

    return result;
}

#include "main.moc"
