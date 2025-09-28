/**
 * QtLucide Gallery Application - Favorites Manager
 *
 * Comprehensive favorites/bookmarks system with:
 * - Persistent storage of favorite icons
 * - Favorites collections/groups
 * - Import/export of favorites
 * - Recently viewed icons tracking
 * - Usage statistics and analytics
 * - Favorites synchronization
 */

#ifndef FAVORITESMANAGER_H
#define FAVORITESMANAGER_H

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QSettings>
#include <QStandardPaths>
#include <QStringList>
#include <QTimer>

// Forward declarations
class IconMetadataManager;

/**
 * @brief Individual favorite item with metadata
 */
struct FavoriteItem {
    QString iconName;
    QString displayName;
    QString description;
    QDateTime dateAdded;
    QDateTime lastAccessed;
    int accessCount = 0;
    QStringList tags;
    QString collection;
    QHash<QString, QVariant> customData;

    FavoriteItem() = default;
    explicit FavoriteItem(const QString& name)
        : iconName(name), dateAdded(QDateTime::currentDateTime()) {}

    bool isValid() const { return !iconName.isEmpty(); }

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

/**
 * @brief Favorites collection/group
 */
struct FavoriteCollection {
    QString name;
    QString displayName;
    QString description;
    QStringList iconNames;
    QDateTime dateCreated;
    QDateTime dateModified;
    QString color;
    QString icon;
    bool isDefault = false;

    FavoriteCollection() = default;
    explicit FavoriteCollection(const QString& collectionName)
        : name(collectionName), displayName(collectionName),
          dateCreated(QDateTime::currentDateTime()) {}

    bool isValid() const { return !name.isEmpty(); }

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

/**
 * @brief Recently viewed item with access tracking
 */
struct RecentItem {
    QString iconName;
    QDateTime lastAccessed;
    int accessCount = 1;
    qint64 totalViewTime = 0; // milliseconds

    RecentItem() = default;
    explicit RecentItem(const QString& name)
        : iconName(name), lastAccessed(QDateTime::currentDateTime()) {}

    bool isValid() const { return !iconName.isEmpty(); }

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
};

/**
 * @brief Comprehensive favorites and usage tracking manager
 */
class FavoritesManager : public QObject {
    Q_OBJECT

public:
    explicit FavoritesManager(QObject* parent = nullptr);
    ~FavoritesManager();

    // Manager setup
    void setIconMetadataManager(IconMetadataManager* manager);

    // Initialization and persistence
    bool loadFavorites();
    bool saveFavorites();
    void setAutoSave(bool enabled);
    bool autoSave() const { return m_autoSave; }

    // Favorites management
    void addFavorite(const QString& iconName, const QString& collection = QString());
    void removeFavorite(const QString& iconName);
    bool isFavorite(const QString& iconName) const;
    void toggleFavorite(const QString& iconName);

    QStringList getFavorites() const;
    QStringList getFavorites(const QString& collection) const;
    FavoriteItem getFavoriteItem(const QString& iconName) const;

    void clearFavorites();
    void clearFavorites(const QString& collection);

    // Collections management
    void createCollection(const QString& name, const QString& displayName = QString());
    void removeCollection(const QString& name);
    void renameCollection(const QString& oldName, const QString& newName);

    QStringList getCollections() const;
    FavoriteCollection getCollection(const QString& name) const;
    void setCollectionProperty(const QString& name, const QString& property, const QVariant& value);

    void moveToCollection(const QString& iconName, const QString& collection);
    void copyToCollection(const QString& iconName, const QString& collection);

    // Recently viewed tracking
    void recordAccess(const QString& iconName, qint64 viewTime = 0);
    QStringList getRecentlyViewed(int limit = 50) const;
    RecentItem getRecentItem(const QString& iconName) const;
    void clearRecentHistory();

    // Usage statistics
    int getTotalFavorites() const;
    int getFavoritesCount(const QString& collection) const;
    int getAccessCount(const QString& iconName) const;
    qint64 getTotalViewTime(const QString& iconName) const;

    QStringList getMostUsedIcons(int limit = 10) const;
    QStringList getMostRecentIcons(int limit = 10) const;
    QHash<QString, int> getUsageStatistics() const;

    // Import/Export
    bool exportFavorites(const QString& filePath) const;
    bool importFavorites(const QString& filePath);
    bool exportCollection(const QString& collection, const QString& filePath) const;
    bool importCollection(const QString& filePath);

    // Search and filtering
    QStringList searchFavorites(const QString& searchText) const;
    QStringList filterFavorites(const QStringList& categories, const QStringList& tags) const;

    // Backup and sync
    bool createBackup(const QString& backupPath = QString()) const;
    bool restoreBackup(const QString& backupPath);
    QStringList getAvailableBackups() const;

signals:
    void favoriteAdded(const QString& iconName, const QString& collection);
    void favoriteRemoved(const QString& iconName);
    void favoriteToggled(const QString& iconName, bool isFavorite);
    void favoritesChanged();

    void collectionCreated(const QString& name);
    void collectionRemoved(const QString& name);
    void collectionRenamed(const QString& oldName, const QString& newName);
    void collectionsChanged();

    void recentItemAdded(const QString& iconName);
    void recentHistoryChanged();

    void dataChanged();
    void saveCompleted(bool success);
    void loadCompleted(bool success);

private slots:
    void onAutoSaveTimer();

private:
    void setupDefaultCollections();
    void updateFavoriteItem(const QString& iconName);
    void updateRecentItem(const QString& iconName, qint64 viewTime);
    void cleanupOldRecents();

    QString getFavoritesFilePath() const;
    QString getBackupDirectory() const;
    QString generateBackupFileName() const;

    bool saveToFile(const QString& filePath) const;
    bool loadFromFile(const QString& filePath);

    QJsonObject serializeData() const;
    bool deserializeData(const QJsonObject& data);

    // Core data
    QHash<QString, FavoriteItem> m_favorites;
    QHash<QString, FavoriteCollection> m_collections;
    QHash<QString, RecentItem> m_recentItems;
    QStringList m_recentOrder; // Ordered list for quick access

    // Managers
    IconMetadataManager* m_iconMetadataManager;

    // Settings and persistence
    QSettings* m_settings;
    QTimer* m_autoSaveTimer;
    bool m_autoSave;
    bool m_dataChanged;

    // Thread safety
    mutable QReadWriteLock m_dataLock;

    // Configuration
    int m_maxRecentItems;
    int m_autoSaveInterval; // seconds
    int m_maxBackups;

    // File paths
    QString m_dataDirectory;
    QString m_favoritesFile;
    QString m_backupDirectory;

    static const QString DEFAULT_COLLECTION;
    static const QString RECENT_COLLECTION;
    static const QString MOST_USED_COLLECTION;
    static const int DEFAULT_MAX_RECENT_ITEMS = 100;
    static const int DEFAULT_AUTO_SAVE_INTERVAL = 30; // seconds
    static const int DEFAULT_MAX_BACKUPS = 10;
};

#endif // FAVORITESMANAGER_H
