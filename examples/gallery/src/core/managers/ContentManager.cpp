/**
 * QtLucide Gallery Application - Unified Content Manager Implementation
 */

#include "ContentManager.h"
#include "GalleryLogger.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <QImageReader>
#include <QPixmap>

ContentManager::ContentManager(QObject* parent)
    : QObject(parent), m_lucide(nullptr), m_iconManager(nullptr), m_imageManager(nullptr),
      m_autoRefresh(true), m_maxCacheSize(1000) {
    GALLERY_LOG_INFO(galleryInit, "ContentManager constructor started");
}

ContentManager::~ContentManager() {
    m_contentCache.clear();
    m_thumbnailCache.clear();
}

void ContentManager::setLucide(lucide::QtLucide* lucide) {
    m_lucide = lucide;
    refreshContent();
}

void ContentManager::setIconMetadataManager(IconMetadataManager* iconManager) {
    m_iconManager = iconManager;
    if (m_iconManager) {
        connect(m_iconManager, &IconMetadataManager::metadataLoaded, this, [this](int totalIcons) {
            Q_UNUSED(totalIcons)
            // Icon metadata loaded - could emit a general signal here
            qDebug() << "Icon metadata loaded for" << totalIcons << "icons";
        });
    }
}

void ContentManager::setImageMetadataManager(ImageMetadataManager* imageManager) {
    m_imageManager = imageManager;
    if (m_imageManager) {
        connect(
            m_imageManager, &ImageMetadataManager::metadataLoaded, this,
            [this](const QString& filePath) { emit contentLoaded(filePath, ContentType::Image); });
        connect(m_imageManager, &ImageMetadataManager::thumbnailReady, this,
                &ContentManager::onImageThumbnailReady);
        connect(m_imageManager, &ImageMetadataManager::directoryLoaded, this,
                &ContentManager::onImageDirectoryLoaded);
    }
}

QStringList ContentManager::getAllContent() const {
    QStringList allContent;

    // Add icons
    if (m_lucide) {
        allContent.append(m_lucide->availableIcons());
    }

    // Add images
    allContent.append(getImages());

    return allContent;
}

QStringList ContentManager::getIcons() const {
    if (m_lucide) {
        return m_lucide->availableIcons();
    }
    return QStringList();
}

QStringList ContentManager::getImages() const {
    QStringList images;
    if (m_imageManager) {
        // Get all cached image paths
        // This would need to be implemented in ImageMetadataManager
        // For now, return empty list
    }
    return images;
}

ContentItem ContentManager::getContentItem(const QString& identifier) const {
    // Check cache first
    if (m_contentCache.contains(identifier)) {
        return m_contentCache.value(identifier);
    }

    ContentItem item;

    if (isIconIdentifier(identifier)) {
        item = createIconContentItem(identifier);
    } else if (isImageIdentifier(identifier)) {
        item = createImageContentItem(identifier);
    }

    // Cache the result
    if (item.isValid()) {
        m_contentCache.insert(identifier, item);
    }

    return item;
}

ContentType ContentManager::getContentType(const QString& identifier) const {
    if (isIconIdentifier(identifier)) {
        return ContentType::Icon;
    } else if (isImageIdentifier(identifier)) {
        return ContentType::Image;
    }

    // Default to icon for unknown identifiers
    return ContentType::Icon;
}

QPixmap ContentManager::getPixmap(const QString& identifier, const QSize& size) const {
    ContentType type = getContentType(identifier);

    if (type == ContentType::Icon && m_lucide) {
        QIcon icon = m_lucide->icon(identifier);
        if (!icon.isNull()) {
            return icon.pixmap(size);
        }
    } else if (type == ContentType::Image) {
        // Load image directly
        QPixmap pixmap(identifier);
        if (!pixmap.isNull() && pixmap.size() != size) {
            return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        return pixmap;
    }

    return QPixmap();
}

QIcon ContentManager::getIcon(const QString& identifier) const {
    ContentType type = getContentType(identifier);

    if (type == ContentType::Icon && m_lucide) {
        return m_lucide->icon(identifier);
    } else if (type == ContentType::Image) {
        // Create icon from image
        QPixmap pixmap(identifier);
        if (!pixmap.isNull()) {
            return QIcon(pixmap);
        }
    }

    return QIcon();
}

QPixmap ContentManager::getThumbnail(const QString& identifier, const QSize& size) const {
    QString cacheKey = QString("%1_%2x%3").arg(identifier).arg(size.width()).arg(size.height());

    if (m_thumbnailCache.contains(cacheKey)) {
        return m_thumbnailCache.value(cacheKey);
    }

    ContentType type = getContentType(identifier);
    QPixmap thumbnail;

    if (type == ContentType::Icon && m_lucide) {
        QIcon icon = m_lucide->icon(identifier);
        if (!icon.isNull()) {
            thumbnail = icon.pixmap(size);
        }
    } else if (type == ContentType::Image && m_imageManager) {
        thumbnail = m_imageManager->getThumbnail(identifier, size);
    }

    if (!thumbnail.isNull()) {
        m_thumbnailCache.insert(cacheKey, thumbnail);
    }

    return thumbnail;
}

bool ContentManager::isIconIdentifier(const QString& identifier) const {
    if (!m_lucide)
        return false;

    // Check if it's in the available icons list
    QStringList availableIcons = m_lucide->availableIcons();
    return availableIcons.contains(identifier);
}

bool ContentManager::isImageIdentifier(const QString& identifier) const {
    // Check if it's a file path that exists and is an image
    QFileInfo fileInfo(identifier);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        return false;
    }

    // Check if it's a supported image format
    if (m_imageManager) {
        QStringList supportedFormats = m_imageManager->getSupportedFormats();
        QString suffix = fileInfo.suffix().toLower();
        return supportedFormats.contains(suffix);
    }

    return false;
}

ContentItem ContentManager::createIconContentItem(const QString& iconName) const {
    ContentItem item;
    item.identifier = iconName;
    item.type = ContentType::Icon;
    item.displayName = iconName;
    item.format = "SVG";

    if (m_iconManager) {
        IconMetadata metadata = m_iconManager->getIconMetadata(iconName);
        if (metadata.isValid()) {
            item.displayName = metadata.getDisplayName();
            // Add more metadata as needed
        }
    }

    return item;
}

ContentItem ContentManager::createImageContentItem(const QString& filePath) const {
    ContentItem item;
    item.identifier = filePath;
    item.type = ContentType::Image;

    QFileInfo fileInfo(filePath);
    item.displayName = fileInfo.baseName();
    item.format = fileInfo.suffix().toUpper();
    item.fileSize = fileInfo.size();
    item.dateModified = fileInfo.lastModified();

    if (m_imageManager) {
        ImageMetadata metadata = m_imageManager->getImageMetadata(filePath);
        if (metadata.isValid()) {
            item.dimensions = metadata.dimensions;
            item.displayName = metadata.getDisplayName();
            // Add more metadata as needed
        }
    }

    return item;
}

void ContentManager::loadImageDirectory(const QString& directoryPath) {
    if (m_imageManager) {
        m_imageManager->loadDirectory(directoryPath);
    }
}

void ContentManager::refreshContent() {
    m_contentCache.clear();
    m_allContent = getAllContent();

    // Update content types mapping
    m_contentTypes.clear();
    for (const QString& identifier : m_allContent) {
        m_contentTypes.insert(identifier, getContentType(identifier));
    }

    int iconCount = getIconCount();
    int imageCount = getImageCount();
    emit contentCountChanged(static_cast<int>(m_allContent.size()), iconCount, imageCount);
}

int ContentManager::getTotalContentCount() const {
    return static_cast<int>(m_allContent.size());
}

int ContentManager::getIconCount() const {
    return static_cast<int>(getIcons().size());
}

int ContentManager::getImageCount() const {
    return static_cast<int>(getImages().size());
}

void ContentManager::onImageDirectoryLoaded(const QString& directoryPath, int imageCount) {
    Q_UNUSED(directoryPath)
    Q_UNUSED(imageCount)
    refreshContent();
}

void ContentManager::onImageMetadataLoaded(const QString& filePath) {
    emit contentLoaded(filePath, ContentType::Image);
}

void ContentManager::onImageThumbnailReady(const QString& filePath, const QPixmap& thumbnail) {
    QString cacheKey =
        QString("%1_%2x%3").arg(filePath).arg(thumbnail.width()).arg(thumbnail.height());
    m_thumbnailCache.insert(cacheKey, thumbnail);
    emit thumbnailReady(filePath, thumbnail);
}
