/**
 * QtLucide Gallery Application - Image Metadata Manager
 *
 * Manages metadata for regular image files (PNG, JPEG, GIF, BMP, SVG)
 * extending the functionality of the icon gallery to support general images.
 */

#ifndef IMAGEMETADATAMANAGER_H
#define IMAGEMETADATAMANAGER_H

#include <QDateTime>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QImageReader>
#include <QMimeDatabase>
#include <QMimeType>
#include <QMutex>
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QThread>
#ifdef QTLUCIDE_HAS_CONCURRENT
    #include <QtConcurrent>
#endif

/**
 * @brief Metadata structure for image files
 */
struct ImageMetadata {
    QString filePath;
    QString fileName;
    QString displayName;
    QSize dimensions;
    qint64 fileSize;
    QString format;
    QString mimeType;
    QDateTime dateCreated;
    QDateTime dateModified;
    int colorDepth;
    bool hasAlpha;
    QString compression;
    QHash<QString, QString> exifData;

    // Computed properties
    QString aspectRatioString;
    QString fileSizeString;
    bool isAnimated;
    int frameCount;

    bool isValid() const { return !filePath.isEmpty(); }
    QString getDisplayName() const { return displayName.isEmpty() ? fileName : displayName; }
};

/**
 * @brief Enhanced metadata manager for image files
 */
class ImageMetadataManager : public QObject {
    Q_OBJECT

public:
    explicit ImageMetadataManager(QObject* parent = nullptr);
    ~ImageMetadataManager();

    // Core functionality
    void loadDirectory(const QString& directoryPath);
    void loadImageFile(const QString& filePath);
    void clearCache();

    // Metadata access
    ImageMetadata getImageMetadata(const QString& filePath) const;
    QStringList getSupportedFormats() const;
    QStringList getImageFiles(const QString& directoryPath) const;

    // Search and filtering
    QStringList searchImages(const QString& searchTerm) const;
    QStringList filterByFormat(const QString& format) const;
    QStringList filterBySize(const QSize& minSize, const QSize& maxSize) const;
    QStringList filterByDate(const QDateTime& startDate, const QDateTime& endDate) const;

    // Statistics
    int getTotalImageCount() const;
    QHash<QString, int> getFormatStatistics() const;
    qint64 getTotalSize() const;

    // Thumbnail management
    QPixmap getThumbnail(const QString& filePath, const QSize& size = QSize(128, 128));
    void preloadThumbnails(const QStringList& filePaths);
    void clearThumbnailCache();

public slots:
    void refreshMetadata();
    void updateImageMetadata(const QString& filePath);

signals:
    void metadataLoaded(const QString& filePath);
    void directoryLoaded(const QString& directoryPath, int imageCount);
    void thumbnailReady(const QString& filePath, const QPixmap& thumbnail);
    void loadingProgress(int current, int total);
    void loadingFinished();

private slots:
    void onMetadataLoadFinished();
    void onThumbnailLoadFinished();

private:
    // Core methods
    void loadMetadataAsync(const QString& filePath);
    void loadThumbnailAsync(const QString& filePath, const QSize& size);
    ImageMetadata extractMetadata(const QString& filePath) const;
    QPixmap createThumbnail(const QString& filePath, const QSize& size) const;

    // Utility methods
    QString formatFileSize(qint64 bytes) const;
    QString formatAspectRatio(const QSize& size) const;
    bool isImageFile(const QString& filePath) const;
    QHash<QString, QString> extractExifData(const QString& filePath) const;

    // Data storage
    mutable QMutex m_metadataMutex;
    QHash<QString, ImageMetadata> m_metadataCache;
    QHash<QString, QPixmap> m_thumbnailCache;

    // Async loading
    QFutureWatcher<ImageMetadata>* m_metadataWatcher;
    QFutureWatcher<QPixmap>* m_thumbnailWatcher;

    // Configuration
    QStringList m_supportedFormats;
    QMimeDatabase m_mimeDatabase;
    int m_maxThumbnailCacheSize;
    int m_maxMetadataCacheSize;

    // Statistics
    mutable int m_totalImageCount;
    mutable QHash<QString, int> m_formatStats;
    mutable qint64 m_totalSize;
    mutable bool m_statsNeedUpdate;
};

/**
 * @brief Async task for loading image metadata
 */
class ImageMetadataLoadTask : public QRunnable {
public:
    ImageMetadataLoadTask(const QString& filePath, ImageMetadataManager* manager);
    void run() override;

private:
    QString m_filePath;
    ImageMetadataManager* m_manager;
};

/**
 * @brief Async task for generating thumbnails
 */
class ThumbnailGenerateTask : public QRunnable {
public:
    ThumbnailGenerateTask(const QString& filePath, const QSize& size,
                          ImageMetadataManager* manager);
    void run() override;

private:
    QString m_filePath;
    QSize m_size;
    ImageMetadataManager* m_manager;
};

#endif // IMAGEMETADATAMANAGER_H
