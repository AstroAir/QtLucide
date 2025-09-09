/**
 * QtLucide Gallery Application - File Browser Widget Implementation
 */

#include "FileBrowserWidget.h"
#include "GalleryLogger.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeView>
#include <QListView>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QSettings>
#include <QApplication>
#include <QDebug>

// ImageFileSystemModel Implementation
ImageFileSystemModel::ImageFileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
    , m_metadataManager(nullptr)
    , m_showOnlyImages(false)
{
    // Initialize supported image extensions
    m_imageExtensions << "png" << "jpg" << "jpeg" << "gif" << "bmp" 
                      << "svg" << "webp" << "tiff" << "tif" << "ico" 
                      << "xpm" << "pbm" << "pgm" << "ppm";
    
    setNameFilterDisables(false);
}

void ImageFileSystemModel::setImageMetadataManager(ImageMetadataManager* manager)
{
    m_metadataManager = manager;
}

void ImageFileSystemModel::setShowOnlyImages(bool showOnly)
{
    m_showOnlyImages = showOnly;
    
    if (m_showOnlyImages) {
        QStringList nameFilters;
        for (const QString& ext : m_imageExtensions) {
            nameFilters << QString("*.%1").arg(ext);
        }
        setNameFilters(nameFilters);
    } else {
        setNameFilters(QStringList());
    }
}

QVariant ImageFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole && m_metadataManager) {
        QString filePath = this->filePath(index);
        if (isDir(index)) {
            return QFileSystemModel::data(index, role);
        }
        
        // Check if it's an image file
        QFileInfo fileInfo(filePath);
        QString suffix = fileInfo.suffix().toLower();
        if (m_imageExtensions.contains(suffix)) {
            // Return thumbnail if available
            QPixmap thumbnail = m_metadataManager->getThumbnail(filePath, QSize(16, 16));
            if (!thumbnail.isNull()) {
                return QIcon(thumbnail);
            }
        }
    }
    
    return QFileSystemModel::data(index, role);
}

// ThumbnailDelegate Implementation
ThumbnailDelegate::ThumbnailDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_thumbnailSize(64, 64)
    , m_contentManager(nullptr)
{
}

void ThumbnailDelegate::setThumbnailSize(const QSize& size)
{
    m_thumbnailSize = size;
}

void ThumbnailDelegate::setContentManager(ContentManager* manager)
{
    m_contentManager = manager;
}

void ThumbnailDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, 
                             const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }
    
    // Get file path
    QString filePath = index.data(QFileSystemModel::FilePathRole).toString();
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isFile() && m_contentManager) {
        // Draw thumbnail for image files
        QPixmap thumbnail = m_contentManager->getThumbnail(filePath, m_thumbnailSize);
        if (!thumbnail.isNull()) {
            QRect rect = option.rect;
            QRect thumbnailRect = QRect(rect.left() + 4, rect.top() + 4, 
                                      m_thumbnailSize.width(), m_thumbnailSize.height());
            
            painter->drawPixmap(thumbnailRect, thumbnail);
            
            // Draw file name below thumbnail
            QRect textRect = QRect(rect.left(), rect.top() + m_thumbnailSize.height() + 8,
                                 rect.width(), rect.height() - m_thumbnailSize.height() - 8);
            painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, fileInfo.baseName());
            return;
        }
    }
    
    // Fall back to default painting
    QStyledItemDelegate::paint(painter, option, index);
}

QSize ThumbnailDelegate::sizeHint(const QStyleOptionViewItem &option, 
                                 const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    
    return QSize(m_thumbnailSize.width() + 8, m_thumbnailSize.height() + 24);
}

// FileBrowserWidget Implementation
FileBrowserWidget::FileBrowserWidget(QWidget *parent)
    : QWidget(parent)
    , m_contentManager(nullptr)
    , m_metadataManager(nullptr)
    , m_fileSystemModel(new ImageFileSystemModel(this))
    , m_proxyModel(new QSortFilterProxyModel(this))
    , m_directoryTree(new QTreeView(this))
    , m_fileList(new QListView(this))
    , m_thumbnailDelegate(new ThumbnailDelegate(this))
    , m_viewMode(ListView)
    , m_thumbnailSize(128, 128)
    , m_showOnlyImages(true)
    , m_showHiddenFiles(false)
    , m_historyIndex(-1)
    , m_thumbnailWatcher(new QFutureWatcher<void>(this))
    , m_updateTimer(new QTimer(this))
{
    GALLERY_LOG_INFO(galleryInit, "FileBrowserWidget constructor started");
    
    setupUI();
    
    // Set initial directory to Pictures
    QString picturesPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (QDir(picturesPath).exists()) {
        setCurrentDirectory(picturesPath);
    } else {
        setCurrentDirectory(QDir::homePath());
    }
    
    // Load bookmarks
    loadBookmarks();
    
    // Setup update timer
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(500);
    connect(m_updateTimer, &QTimer::timeout, this, &FileBrowserWidget::updateThumbnails);
    
    GALLERY_LOG_INFO(galleryInit, "FileBrowserWidget initialized");
}

FileBrowserWidget::~FileBrowserWidget()
{
    saveBookmarks();
}

void FileBrowserWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    
    setupToolbar();
    
    // Create main splitter
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    setupDirectoryTree();
    setupFileList();
    setupBookmarks();
    setupStatusBar();
    
    // Add widgets to splitter
    QWidget* leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(m_directoryTree);
    leftLayout->addWidget(m_bookmarkPanel);
    
    m_mainSplitter->addWidget(leftPanel);
    m_mainSplitter->addWidget(m_fileList);
    m_mainSplitter->setSizes({200, 400});
    
    mainLayout->addWidget(m_toolbar);
    mainLayout->addWidget(m_mainSplitter, 1);
    mainLayout->addWidget(m_statusLabel);
}

void FileBrowserWidget::setupToolbar()
{
    m_toolbar = new QWidget(this);
    QHBoxLayout* toolbarLayout = new QHBoxLayout(m_toolbar);
    toolbarLayout->setContentsMargins(0, 0, 0, 0);
    
    // Navigation buttons
    m_upButton = new QPushButton("Up", this);
    m_homeButton = new QPushButton("Home", this);
    m_refreshButton = new QPushButton("Refresh", this);
    
    // Path edit
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setReadOnly(true);
    
    // View mode combo
    m_viewModeCombo = new QComboBox(this);
    m_viewModeCombo->addItems({"Tree View", "List View", "Thumbnail View"});
    m_viewModeCombo->setCurrentIndex(static_cast<int>(m_viewMode));
    
    // Filter combo
    m_filterCombo = new QComboBox(this);
    m_filterCombo->addItems({"All Files", "Images Only"});
    m_filterCombo->setCurrentIndex(m_showOnlyImages ? 1 : 0);
    
    // Name filter
    m_nameFilterEdit = new QLineEdit(this);
    m_nameFilterEdit->setPlaceholderText("Filter by name...");
    
    toolbarLayout->addWidget(m_upButton);
    toolbarLayout->addWidget(m_homeButton);
    toolbarLayout->addWidget(m_refreshButton);
    toolbarLayout->addWidget(m_pathEdit, 1);
    toolbarLayout->addWidget(m_viewModeCombo);
    toolbarLayout->addWidget(m_filterCombo);
    toolbarLayout->addWidget(m_nameFilterEdit);
    
    // Connect signals
    connect(m_upButton, &QPushButton::clicked, this, &FileBrowserWidget::navigateUp);
    connect(m_homeButton, &QPushButton::clicked, this, &FileBrowserWidget::navigateHome);
    connect(m_refreshButton, &QPushButton::clicked, this, &FileBrowserWidget::refresh);
    connect(m_viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FileBrowserWidget::onViewModeChanged);
    connect(m_filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                setShowOnlyImages(index == 1);
            });
    connect(m_nameFilterEdit, &QLineEdit::textChanged, this, &FileBrowserWidget::onNameFilterChanged);
}

void FileBrowserWidget::setupDirectoryTree()
{
    m_directoryTree->setModel(m_fileSystemModel);
    m_directoryTree->setRootIndex(m_fileSystemModel->index(QDir::rootPath()));
    
    // Hide all columns except name
    for (int i = 1; i < m_fileSystemModel->columnCount(); ++i) {
        m_directoryTree->hideColumn(i);
    }
    
    m_directoryTree->setHeaderHidden(true);
    m_directoryTree->setAnimated(true);
    
    connect(m_directoryTree, &QTreeView::clicked, this, &FileBrowserWidget::onDirectoryTreeClicked);
}

void FileBrowserWidget::setupFileList()
{
    m_proxyModel->setSourceModel(m_fileSystemModel);
    m_fileList->setModel(m_proxyModel);
    
    m_thumbnailDelegate->setThumbnailSize(m_thumbnailSize);
    m_fileList->setItemDelegate(m_thumbnailDelegate);
    
    m_fileList->setViewMode(QListView::IconMode);
    m_fileList->setResizeMode(QListView::Adjust);
    m_fileList->setUniformItemSizes(true);
    
    connect(m_fileList, &QListView::clicked, this, &FileBrowserWidget::onFileListClicked);
    connect(m_fileList, &QListView::doubleClicked, this, &FileBrowserWidget::onFileListDoubleClicked);
}

void FileBrowserWidget::setupBookmarks()
{
    m_bookmarkPanel = new QWidget(this);
    m_bookmarkLayout = new QVBoxLayout(m_bookmarkPanel);
    m_bookmarkLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel* bookmarkLabel = new QLabel("Bookmarks", this);
    bookmarkLabel->setStyleSheet("font-weight: bold;");
    m_bookmarkLayout->addWidget(bookmarkLabel);
    
    m_bookmarkPanel->setMaximumHeight(200);
}

void FileBrowserWidget::setupStatusBar()
{
    m_statusLabel = new QLabel(this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
}

void FileBrowserWidget::setCurrentDirectory(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    
    m_currentDirectory = dir.absolutePath();
    
    // Update models
    QModelIndex index = m_fileSystemModel->index(m_currentDirectory);
    m_fileList->setRootIndex(index);
    m_directoryTree->setCurrentIndex(index);
    m_directoryTree->expand(index);
    
    // Update UI
    m_pathEdit->setText(m_currentDirectory);
    updateNavigationButtons();
    updateStatusBar();
    
    // Add to history
    addToHistory(m_currentDirectory);
    
    // Start thumbnail loading
    m_updateTimer->start();
    
    emit directoryChanged(m_currentDirectory);
}

void FileBrowserWidget::updateStatusBar()
{
    QDir dir(m_currentDirectory);
    QStringList nameFilters;
    if (m_showOnlyImages && m_metadataManager) {
        QStringList formats = m_metadataManager->getSupportedFormats();
        for (const QString& format : formats) {
            nameFilters << QString("*.%1").arg(format);
        }
    }
    
    QFileInfoList files = dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable);
    m_statusLabel->setText(QString("%1 items").arg(files.size()));
}

void FileBrowserWidget::addToHistory(const QString& path)
{
    // Remove any entries after current position
    while (m_navigationHistory.size() > m_historyIndex + 1) {
        m_navigationHistory.removeLast();
    }
    
    // Add new path
    m_navigationHistory.append(path);
    m_historyIndex = m_navigationHistory.size() - 1;
    
    // Limit history size
    const int maxHistorySize = 50;
    if (m_navigationHistory.size() > maxHistorySize) {
        m_navigationHistory.removeFirst();
        m_historyIndex--;
    }
}

void FileBrowserWidget::updateNavigationButtons()
{
    QDir dir(m_currentDirectory);
    m_upButton->setEnabled(dir.cdUp());
}

void FileBrowserWidget::loadBookmarks()
{
    QSettings settings;
    settings.beginGroup("FileBrowser");
    
    QStringList bookmarkPaths = settings.value("bookmarks").toStringList();
    for (const QString& path : bookmarkPaths) {
        QDir dir(path);
        if (dir.exists()) {
            addBookmark(path, dir.dirName());
        }
    }
    
    settings.endGroup();
}

void FileBrowserWidget::saveBookmarks()
{
    QSettings settings;
    settings.beginGroup("FileBrowser");
    
    QStringList bookmarkPaths = m_bookmarks.keys();
    settings.setValue("bookmarks", bookmarkPaths);
    
    settings.endGroup();
}

void FileBrowserWidget::onDirectoryTreeClicked(const QModelIndex& index)
{
    if (m_fileSystemModel->isDir(index)) {
        QString path = m_fileSystemModel->filePath(index);
        setCurrentDirectory(path);
    }
}

void FileBrowserWidget::onFileListClicked(const QModelIndex& index)
{
    QString filePath = m_fileSystemModel->filePath(m_proxyModel->mapToSource(index));
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isFile()) {
        emit fileSelected(filePath);
        
        // Check if it's an image
        if (m_metadataManager && m_metadataManager->getSupportedFormats().contains(fileInfo.suffix().toLower())) {
            emit imageSelected(filePath);
        }
    }
}

void FileBrowserWidget::onFileListDoubleClicked(const QModelIndex& index)
{
    QString filePath = m_fileSystemModel->filePath(m_proxyModel->mapToSource(index));
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isDir()) {
        setCurrentDirectory(filePath);
        emit directoryDoubleClicked(filePath);
    }
}
