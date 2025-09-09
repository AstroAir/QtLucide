/**
 * QtLucide Gallery Application - File Browser Widget
 *
 * A comprehensive file browser for selecting images and directories,
 * with thumbnail previews and filtering capabilities.
 */

#ifndef FILEBROWSERWIDGET_H
#define FILEBROWSERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTreeView>
#include <QListView>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QDir>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QFutureWatcher>
#ifdef QTLUCIDE_HAS_CONCURRENT
#include <QtConcurrent>
#endif

#include "ContentManager.h"
#include "ImageMetadataManager.h"

/**
 * @brief Custom file system model with image filtering
 */
class ImageFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit ImageFileSystemModel(QObject *parent = nullptr);

    void setImageMetadataManager(ImageMetadataManager* manager);
    void setShowOnlyImages(bool showOnly);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    ImageMetadataManager* m_metadataManager;
    bool m_showOnlyImages;
    QStringList m_imageExtensions;
};

/**
 * @brief Thumbnail delegate for file list view
 */
class ThumbnailDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ThumbnailDelegate(QObject *parent = nullptr);

    void setThumbnailSize(const QSize& size);
    void setContentManager(ContentManager* manager);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    QSize m_thumbnailSize;
    ContentManager* m_contentManager;
    mutable QHash<QString, QPixmap> m_thumbnailCache;
};

/**
 * @brief Main file browser widget
 */
class FileBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    enum ViewMode {
        TreeView,
        ListView,
        ThumbnailView
    };
    Q_ENUM(ViewMode)

    explicit FileBrowserWidget(QWidget *parent = nullptr);
    ~FileBrowserWidget();

    // Setup
    void setContentManager(ContentManager* manager);
    void setImageMetadataManager(ImageMetadataManager* manager);

    // Navigation
    void setCurrentDirectory(const QString& path);
    QString getCurrentDirectory() const;
    void navigateUp();
    void navigateHome();
    void navigateToPath(const QString& path);

    // View configuration
    void setViewMode(ViewMode mode);
    void setThumbnailSize(const QSize& size);
    void setShowOnlyImages(bool showOnly);
    void setShowHiddenFiles(bool show);

    // Selection
    QStringList getSelectedFiles() const;
    QStringList getSelectedImages() const;
    QString getCurrentFile() const;

    // Filtering
    void setNameFilter(const QString& filter);
    void setFormatFilter(const QStringList& formats);

    // Bookmarks
    void addBookmark(const QString& path, const QString& name = QString());
    void removeBookmark(const QString& path);
    QStringList getBookmarks() const;

public slots:
    void refresh();
    void selectAll();
    void clearSelection();
    void onThumbnailReady(const QString& filePath, const QPixmap& thumbnail);

signals:
    void directoryChanged(const QString& path);
    void fileSelected(const QString& filePath);
    void filesSelected(const QStringList& filePaths);
    void imageSelected(const QString& imagePath);
    void imagesSelected(const QStringList& imagePaths);
    void directoryDoubleClicked(const QString& path);
    void loadingStarted();
    void loadingFinished();

private slots:
    void onDirectoryTreeClicked(const QModelIndex& index);
    void onFileListClicked(const QModelIndex& index);
    void onFileListDoubleClicked(const QModelIndex& index);
    void onSelectionChanged();
    void onNameFilterChanged();
    void onViewModeChanged();
    void onNavigationButtonClicked();
    void onBookmarkClicked();

private:
    // UI setup
    void setupUI();
    void setupToolbar();
    void setupDirectoryTree();
    void setupFileList();
    void setupBookmarks();
    void setupStatusBar();

    // Navigation helpers
    void updateNavigationButtons();
    void updatePathBar();
    void updateStatusBar();
    void addToHistory(const QString& path);

    // View helpers
    void updateFileList();
    void updateThumbnails();
    void applyFilters();

    // Bookmark helpers
    void loadBookmarks();
    void saveBookmarks();

    // Core components
    ContentManager* m_contentManager;
    ImageMetadataManager* m_metadataManager;

    // Models and views
    ImageFileSystemModel* m_fileSystemModel;
    QSortFilterProxyModel* m_proxyModel;
    QTreeView* m_directoryTree;
    QListView* m_fileList;
    ThumbnailDelegate* m_thumbnailDelegate;

    // UI components
    QSplitter* m_mainSplitter;
    QWidget* m_toolbar;
    QPushButton* m_upButton;
    QPushButton* m_homeButton;
    QPushButton* m_refreshButton;
    QLineEdit* m_pathEdit;
    QComboBox* m_viewModeCombo;
    QComboBox* m_filterCombo;
    QLineEdit* m_nameFilterEdit;

    // Bookmarks
    QWidget* m_bookmarkPanel;
    QVBoxLayout* m_bookmarkLayout;
    QList<QPushButton*> m_bookmarkButtons;

    // Status
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;

    // Configuration
    ViewMode m_viewMode;
    QSize m_thumbnailSize;
    bool m_showOnlyImages;
    bool m_showHiddenFiles;
    QString m_currentDirectory;
    QStringList m_navigationHistory;
    int m_historyIndex;

    // Filtering
    QString m_nameFilter;
    QStringList m_formatFilter;

    // Bookmarks data
    QHash<QString, QString> m_bookmarks;
    QString m_bookmarksFile;

    // Threading
    QFutureWatcher<void>* m_thumbnailWatcher;
    QTimer* m_updateTimer;
};

/**
 * @brief Directory scanner for background loading
 */
class DirectoryScanner : public QObject
{
    Q_OBJECT

public:
    explicit DirectoryScanner(QObject *parent = nullptr);

    void scanDirectory(const QString& path, const QStringList& filters);
    void cancel();

signals:
    void fileFound(const QString& filePath);
    void scanProgress(int current, int total);
    void scanFinished(int totalFiles);

private slots:
    void doScan();

private:
    QString m_scanPath;
    QStringList m_filters;
    bool m_cancelled;
    QMutex m_mutex;
};

#endif // FILEBROWSERWIDGET_H
