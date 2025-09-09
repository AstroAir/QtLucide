/**
 * QtLucide Gallery Application - Image Gallery Test
 * 
 * Simple test to verify the image gallery functionality works correctly.
 */

#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>

#include "ImageMetadataManager.h"
#include "ContentManager.h"
#include "ImageViewerWidget.h"
#include "FileBrowserWidget.h"
#include <QtLucide/QtLucide.h>

class ImageGalleryTestWindow : public QMainWindow
{
    Q_OBJECT

public:
    ImageGalleryTestWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
        , m_lucide(new lucide::QtLucide(this))
        , m_imageMetadataManager(new ImageMetadataManager(this))
        , m_contentManager(new ContentManager(this))
        , m_imageViewer(new ImageViewerWidget(this))
        , m_fileBrowser(new FileBrowserWidget(this))
    {
        setWindowTitle("QtLucide Image Gallery Test");
        setMinimumSize(1200, 800);
        
        setupUI();
        setupConnections();
        initializeComponents();
    }

private slots:
    void onOpenDirectory()
    {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Image Directory",
                                                       QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
        if (!dir.isEmpty()) {
            m_fileBrowser->setCurrentDirectory(dir);
            m_contentManager->loadImageDirectory(dir);
            m_statusLabel->setText(QString("Loaded directory: %1").arg(dir));
        }
    }
    
    void onImageSelected(const QString& imagePath)
    {
        m_imageViewer->setCurrentImage(imagePath);
        m_statusLabel->setText(QString("Selected: %1").arg(QFileInfo(imagePath).fileName()));
    }
    
    void onDirectoryChanged(const QString& path)
    {
        m_statusLabel->setText(QString("Directory: %1").arg(path));
    }

private:
    void setupUI()
    {
        QWidget* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        
        // Toolbar
        QHBoxLayout* toolbarLayout = new QHBoxLayout();
        QPushButton* openDirButton = new QPushButton("Open Directory", this);
        QPushButton* testIconsButton = new QPushButton("Test Icons", this);
        
        toolbarLayout->addWidget(openDirButton);
        toolbarLayout->addWidget(testIconsButton);
        toolbarLayout->addStretch();
        
        // Main content area
        QHBoxLayout* contentLayout = new QHBoxLayout();
        
        // Left panel - File browser
        m_fileBrowser->setMaximumWidth(300);
        contentLayout->addWidget(m_fileBrowser);
        
        // Right panel - Image viewer
        contentLayout->addWidget(m_imageViewer, 1);
        
        // Status bar
        m_statusLabel = new QLabel("Ready", this);
        
        mainLayout->addLayout(toolbarLayout);
        mainLayout->addLayout(contentLayout, 1);
        mainLayout->addWidget(m_statusLabel);
        
        // Connect toolbar buttons
        connect(openDirButton, &QPushButton::clicked, this, &ImageGalleryTestWindow::onOpenDirectory);
        connect(testIconsButton, &QPushButton::clicked, this, [this]() {
            // Test icon functionality
            if (m_lucide->initLucide()) {
                QStringList icons = m_lucide->availableIcons();
                m_statusLabel->setText(QString("QtLucide initialized with %1 icons").arg(icons.size()));
            } else {
                m_statusLabel->setText("Failed to initialize QtLucide");
            }
        });
    }
    
    void setupConnections()
    {
        // Connect file browser to image viewer
        connect(m_fileBrowser, &FileBrowserWidget::imageSelected,
                this, &ImageGalleryTestWindow::onImageSelected);
        connect(m_fileBrowser, &FileBrowserWidget::directoryChanged,
                this, &ImageGalleryTestWindow::onDirectoryChanged);
        
        // Connect content manager signals
        connect(m_contentManager, &ContentManager::contentLoaded,
                this, [this](const QString& identifier, ContentType type) {
                    QString typeStr = (type == ContentType::Icon) ? "icon" : "image";
                    qDebug() << "Content loaded:" << identifier << "(" << typeStr << ")";
                });
        
        connect(m_contentManager, &ContentManager::contentCountChanged,
                this, [this](int total, int icons, int images) {
                    m_statusLabel->setText(QString("Content: %1 total (%2 icons, %3 images)")
                                         .arg(total).arg(icons).arg(images));
                });
    }
    
    void initializeComponents()
    {
        // Initialize QtLucide
        if (!m_lucide->initLucide()) {
            qWarning() << "Failed to initialize QtLucide";
        }
        
        // Setup content manager
        m_contentManager->setLucide(m_lucide);
        m_contentManager->setImageMetadataManager(m_imageMetadataManager);
        
        // Setup image viewer
        m_imageViewer->setContentManager(m_contentManager);
        
        // Setup file browser
        m_fileBrowser->setContentManager(m_contentManager);
        m_fileBrowser->setImageMetadataManager(m_imageMetadataManager);
        
        // Set initial directory to Pictures
        QString picturesPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        if (QDir(picturesPath).exists()) {
            m_fileBrowser->setCurrentDirectory(picturesPath);
        }
        
        m_statusLabel->setText("Image Gallery Test initialized");
    }

private:
    lucide::QtLucide* m_lucide;
    ImageMetadataManager* m_imageMetadataManager;
    ContentManager* m_contentManager;
    ImageViewerWidget* m_imageViewer;
    FileBrowserWidget* m_fileBrowser;
    QLabel* m_statusLabel;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("QtLucide Image Gallery Test");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide Project");
    
    ImageGalleryTestWindow window;
    window.show();
    
    return app.exec();
}

#include "test_image_gallery.moc"
