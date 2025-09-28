/**
 * QtLucide Gallery Application - Unified Content Manager
 *
 * Manages both icons and images in a unified interface, providing
 * a single point of access for all gallery content.
 */

#ifndef CONTENTMANAGER_H
#define CONTENTMANAGER_H

#include <QHash>
#include <QIcon>
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "IconMetadataManager.h"
#include "ImageMetadataManager.h"
#include <QtLucide/QtLucide.h>

/**
 * @brief Content type enumeration
 */
enum class ContentType { Icon, Image };

/**
 * @brief Unified content item structure
 */
struct ContentItem {
    QString identifier; // Icon name or image file path
    ContentType type;
    QString displayName;
    QSize dimensions;
    QString format;
    qint64 fileSize;
    QDateTime dateModified;
    QHash<QString, QVariant> metadata;

    bool isValid() const { return !identifier.isEmpty(); }
    QString getDisplayName() const { return displayName.isEmpty() ? identifier : displayName; }
};

/**
 * @brief Unified content manager for icons and images
 */
class ContentManager : public QObject {
    Q_OBJECT

public:
    explicit ContentManager(QObject* parent = nullptr);
    ~ContentManager();

    // Initialization
    void setLucide(lucide::QtLucide* lucide);
    void setIconMetadataManager(IconMetadataManager* iconManager);
    void setImageMetadataManager(ImageMetadataManager* imageManager);

    // Content access
    QStringList getAllContent() const;
    QStringList getIcons() const;
    QStringList getImages() const;
    ContentItem getContentItem(const QString& identifier) const;
    ContentType getContentType(const QString& identifier) const;

    // Visual representation
    QPixmap getPixmap(const QString& identifier, const QSize& size = QSize(48, 48)) const;
    QIcon getIcon(const QString& identifier) const;
    QPixmap getThumbnail(const QString& identifier, const QSize& size = QSize(128, 128)) const;

    // Search and filtering
    QStringList searchContent(const QString& searchTerm) const;
    QStringList filterByType(ContentType type) const;
    QStringList filterByFormat(const QString& format) const;
    QStringList filterBySize(const QSize& minSize, const QSize& maxSize) const;

    // Content operations
    bool isContentAvailable(const QString& identifier) const;
    QString getContentPath(const QString& identifier) const;
    QHash<QString, QVariant> getContentMetadata(const QString& identifier) const;

    // Directory operations
    void loadImageDirectory(const QString& directoryPath);
    void refreshContent();

    // Statistics
    int getTotalContentCount() const;
    int getIconCount() const;
    int getImageCount() const;
    QHash<QString, int> getFormatStatistics() const;

public slots:
    void onImageDirectoryLoaded(const QString& directoryPath, int imageCount);
    void onImageMetadataLoaded(const QString& filePath);
    void onImageThumbnailReady(const QString& filePath, const QPixmap& thumbnail);

signals:
    void contentLoaded(const QString& identifier, ContentType type);
    void contentUpdated(const QString& identifier);
    void thumbnailReady(const QString& identifier, const QPixmap& thumbnail);
    void loadingProgress(int current, int total);
    void loadingFinished();
    void contentCountChanged(int totalCount, int iconCount, int imageCount);

private:
    // Helper methods
    bool isIconIdentifier(const QString& identifier) const;
    bool isImageIdentifier(const QString& identifier) const;
    ContentItem createIconContentItem(const QString& iconName) const;
    ContentItem createImageContentItem(const QString& filePath) const;

    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_iconManager;
    ImageMetadataManager* m_imageManager;

    // Content tracking
    QStringList m_allContent;
    QHash<QString, ContentType> m_contentTypes;

    // Caching
    mutable QHash<QString, ContentItem> m_contentCache;
    mutable QHash<QString, QPixmap> m_thumbnailCache;

    // Configuration
    bool m_autoRefresh;
    int m_maxCacheSize;
};

/**
 * @brief Content transformation utilities
 */
class ContentTransform {
public:
    // Image transformations
    static QPixmap rotateImage(const QPixmap& pixmap, int degrees);
    static QPixmap flipImage(const QPixmap& pixmap, bool horizontal, bool vertical);
    static QPixmap scaleImage(const QPixmap& pixmap, const QSize& size,
                              Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
    static QPixmap cropImage(const QPixmap& pixmap, const QRect& cropRect);

    // Color adjustments
    static QPixmap adjustBrightness(const QPixmap& pixmap, int brightness);
    static QPixmap adjustContrast(const QPixmap& pixmap, double contrast);
    static QPixmap adjustSaturation(const QPixmap& pixmap, double saturation);

    // Effects
    static QPixmap applyGrayscale(const QPixmap& pixmap);
    static QPixmap applySepia(const QPixmap& pixmap);
    static QPixmap applyBlur(const QPixmap& pixmap, int radius);
};

/**
 * @brief Content export utilities
 */
class ContentExporter {
public:
    enum ExportFormat { PNG, JPEG, BMP, SVG, ICO, WEBP };

    struct ExportOptions {
        ExportFormat format = PNG;
        QSize size = QSize(256, 256);
        int quality = 90;
        bool maintainAspectRatio = true;
        QColor backgroundColor = Qt::transparent;
    };

    static bool exportContent(const QString& identifier, const QString& outputPath,
                              const ExportOptions& options, ContentManager* manager);
    static QStringList getSupportedExportFormats();
    static QString getFormatExtension(ExportFormat format);
};

#endif // CONTENTMANAGER_H
