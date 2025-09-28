/**
 * QtLucide Gallery Application - Image Metadata Manager Implementation
 */

#include "ImageMetadataManager.h"
#include "../utils/GalleryLogger.h"

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QPixmap>
#include <QStandardPaths>
#include <QThreadPool>
#ifdef QTLUCIDE_HAS_CONCURRENT
    #include <QtConcurrent>
#endif
#include <QDebug>

ImageMetadataManager::ImageMetadataManager(QObject* parent)
    : QObject(parent), m_metadataWatcher(new QFutureWatcher<ImageMetadata>(this)),
      m_thumbnailWatcher(new QFutureWatcher<QPixmap>(this)), m_maxThumbnailCacheSize(1000),
      m_maxMetadataCacheSize(5000), m_totalImageCount(0), m_totalSize(0), m_statsNeedUpdate(true) {
    GALLERY_LOG_INFO(galleryInit, "ImageMetadataManager constructor started");

    // Initialize supported formats
    m_supportedFormats = QStringList()
                         << "png" << "jpg" << "jpeg" << "gif" << "bmp" << "svg" << "webp"
                         << "tiff" << "tif" << "ico" << "xpm" << "pbm" << "pgm" << "ppm";

    // Connect async watchers
    connect(m_metadataWatcher, &QFutureWatcher<ImageMetadata>::finished, this,
            &ImageMetadataManager::onMetadataLoadFinished);
    connect(m_thumbnailWatcher, &QFutureWatcher<QPixmap>::finished, this,
            &ImageMetadataManager::onThumbnailLoadFinished);

    GALLERY_LOG_INFO(galleryInit,
                     QString("ImageMetadataManager initialized with support for %1 formats")
                         .arg(static_cast<int>(m_supportedFormats.size())));
}

ImageMetadataManager::~ImageMetadataManager() {
    clearCache();
}

void ImageMetadataManager::loadDirectory(const QString& directoryPath) {
    GALLERY_LOG_INFO(galleryMain, QString("Loading directory: %1").arg(directoryPath));
    GALLERY_START_TIMER("Directory loading");

    QDir dir(directoryPath);
    if (!dir.exists()) {
        GALLERY_LOG_WARNING(galleryMain,
                            QString("Directory does not exist: %1").arg(directoryPath));
        return;
    }

    // Get all image files
    QStringList nameFilters;
    for (const QString& format : m_supportedFormats) {
        nameFilters << QString("*.%1").arg(format);
    }

    QStringList imageFiles = dir.entryList(nameFilters, QDir::Files | QDir::Readable);

    GALLERY_LOG_INFO(galleryMain,
                     QString("Found %1 image files").arg(static_cast<int>(imageFiles.size())));

    // Load metadata for each file asynchronously
    int totalFiles = static_cast<int>(imageFiles.size());
    int currentFile = 0;

    for (const QString& fileName : imageFiles) {
        QString fullPath = dir.absoluteFilePath(fileName);
        loadImageFile(fullPath);

        emit loadingProgress(++currentFile, totalFiles);
    }

    emit directoryLoaded(directoryPath, static_cast<int>(imageFiles.size()));
    emit loadingFinished();

    GALLERY_END_TIMER("Directory loading");
}

void ImageMetadataManager::loadImageFile(const QString& filePath) {
    if (!isImageFile(filePath)) {
        return;
    }

    // Check if already cached
    QMutexLocker locker(&m_metadataMutex);
    if (m_metadataCache.contains(filePath)) {
        return;
    }
    locker.unlock();

    // Load asynchronously
    loadMetadataAsync(filePath);
}

void ImageMetadataManager::loadMetadataAsync(const QString& filePath) {
#ifdef QTLUCIDE_HAS_CONCURRENT
    QFuture<ImageMetadata> future =
        QtConcurrent::run([this, filePath]() { return extractMetadata(filePath); });

    m_metadataWatcher->setFuture(future);
#else
    // Fallback to synchronous loading
    ImageMetadata metadata = extractMetadata(filePath);
    if (metadata.isValid()) {
        QMutexLocker locker(&m_metadataMutex);
        m_metadataCache.insert(metadata.filePath, metadata);
        m_statsNeedUpdate = true;
        locker.unlock();

        emit metadataLoaded(metadata.filePath);
    }
#endif
}

ImageMetadata ImageMetadataManager::extractMetadata(const QString& filePath) const {
    ImageMetadata metadata;

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        return metadata;
    }

    metadata.filePath = filePath;
    metadata.fileName = fileInfo.fileName();
    metadata.displayName = fileInfo.baseName();
    metadata.fileSize = fileInfo.size();
    metadata.dateCreated = fileInfo.birthTime();
    metadata.dateModified = fileInfo.lastModified();

    // Get MIME type
    QMimeType mimeType = m_mimeDatabase.mimeTypeForFile(filePath);
    metadata.mimeType = mimeType.name();
    metadata.format = fileInfo.suffix().toLower();

    // Read image properties
    QImageReader reader(filePath);
    if (reader.canRead()) {
        metadata.dimensions = reader.size();
        metadata.colorDepth = reader.imageFormat() != QImage::Format_Invalid ? 32 : 0; // Simplified
        metadata.hasAlpha = reader.supportsOption(QImageIOHandler::ImageFormat);
        metadata.isAnimated = reader.supportsAnimation();
        metadata.frameCount = reader.imageCount();
    }

    // Format computed properties
    metadata.aspectRatioString = formatAspectRatio(metadata.dimensions);
    metadata.fileSizeString = formatFileSize(metadata.fileSize);

    // Extract EXIF data (simplified - would need external library for full EXIF)
    metadata.exifData = extractExifData(filePath);

    return metadata;
}

ImageMetadata ImageMetadataManager::getImageMetadata(const QString& filePath) const {
    QMutexLocker locker(&m_metadataMutex);
    return m_metadataCache.value(filePath, ImageMetadata());
}

QStringList ImageMetadataManager::getSupportedFormats() const {
    return m_supportedFormats;
}

QPixmap ImageMetadataManager::getThumbnail(const QString& filePath, const QSize& size) {
    QString cacheKey = QString("%1_%2x%3").arg(filePath).arg(size.width()).arg(size.height());

    QMutexLocker locker(&m_metadataMutex);
    if (m_thumbnailCache.contains(cacheKey)) {
        return m_thumbnailCache.value(cacheKey);
    }
    locker.unlock();

    // Generate thumbnail asynchronously
    loadThumbnailAsync(filePath, size);

    // Return empty pixmap for now - will emit signal when ready
    return QPixmap();
}

void ImageMetadataManager::loadThumbnailAsync(const QString& filePath, const QSize& size) {
#ifdef QTLUCIDE_HAS_CONCURRENT
    QFuture<QPixmap> future =
        QtConcurrent::run([this, filePath, size]() { return createThumbnail(filePath, size); });

    m_thumbnailWatcher->setFuture(future);
#else
    // Fallback to synchronous loading
    QPixmap thumbnail = createThumbnail(filePath, size);
    if (!thumbnail.isNull()) {
        QString cacheKey = QString("%1_%2x%3").arg(filePath).arg(size.width()).arg(size.height());
        QMutexLocker locker(&m_metadataMutex);
        m_thumbnailCache.insert(cacheKey, thumbnail);
        locker.unlock();

        emit thumbnailReady(filePath, thumbnail);
    }
#endif
}

QPixmap ImageMetadataManager::createThumbnail(const QString& filePath, const QSize& size) const {
    QPixmap pixmap(filePath);
    if (pixmap.isNull()) {
        return QPixmap();
    }

    return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

bool ImageMetadataManager::isImageFile(const QString& filePath) const {
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    return m_supportedFormats.contains(suffix);
}

QString ImageMetadataManager::formatFileSize(qint64 bytes) const {
    const qint64 KB = 1024;
    const qint64 MB = KB * 1024;
    const qint64 GB = MB * 1024;

    if (bytes >= GB) {
        return QString("%1 GB").arg(static_cast<double>(bytes) / static_cast<double>(GB), 0, 'f',
                                    2);
    } else if (bytes >= MB) {
        return QString("%1 MB").arg(static_cast<double>(bytes) / static_cast<double>(MB), 0, 'f',
                                    2);
    } else if (bytes >= KB) {
        return QString("%1 KB").arg(static_cast<double>(bytes) / static_cast<double>(KB), 0, 'f',
                                    1);
    } else {
        return QString("%1 bytes").arg(bytes);
    }
}

QString ImageMetadataManager::formatAspectRatio(const QSize& size) const {
    if (size.isEmpty()) {
        return "Unknown";
    }

    int gcd = [](int a, int b) -> int {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }(size.width(), size.height());

    int ratioW = size.width() / gcd;
    int ratioH = size.height() / gcd;

    return QString("%1:%2").arg(ratioW).arg(ratioH);
}

QHash<QString, QString> ImageMetadataManager::extractExifData(const QString& filePath) const {
    Q_UNUSED(filePath)
    // Placeholder - would need external EXIF library like libexif or exiv2
    QHash<QString, QString> exifData;
    return exifData;
}

void ImageMetadataManager::onMetadataLoadFinished() {
    ImageMetadata metadata = m_metadataWatcher->result();
    if (metadata.isValid()) {
        QMutexLocker locker(&m_metadataMutex);
        m_metadataCache.insert(metadata.filePath, metadata);
        m_statsNeedUpdate = true;
        locker.unlock();

        emit metadataLoaded(metadata.filePath);
    }
}

void ImageMetadataManager::onThumbnailLoadFinished() {
    QPixmap thumbnail = m_thumbnailWatcher->result();
    if (!thumbnail.isNull()) {
        // Store in cache and emit signal
        // Implementation would include cache key management
        emit thumbnailReady(QString(), thumbnail); // Placeholder
    }
}

void ImageMetadataManager::clearCache() {
    QMutexLocker locker(&m_metadataMutex);
    m_metadataCache.clear();
    m_thumbnailCache.clear();
    m_statsNeedUpdate = true;
}

int ImageMetadataManager::getTotalImageCount() const {
    QMutexLocker locker(&m_metadataMutex);
    return static_cast<int>(m_metadataCache.size());
}

void ImageMetadataManager::refreshMetadata() {
    clearCache();
    // Could reload current directory if we tracked it
    GALLERY_LOG_INFO(galleryMain, "Metadata cache refreshed");
}

void ImageMetadataManager::updateImageMetadata(const QString& filePath) {
    if (!isImageFile(filePath)) {
        return;
    }

    // Remove from cache to force reload
    QMutexLocker locker(&m_metadataMutex);
    m_metadataCache.remove(filePath);
    locker.unlock();

    // Reload metadata
    loadImageFile(filePath);
}
