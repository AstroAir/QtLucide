/**
 * QtLucide Gallery Application - Favorites Manager Implementation
 */

#include "FavoritesManager.h"
#include "GalleryLogger.h"
#include "IconMetadataManager.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QReadLocker>
#include <QStandardPaths>
#include <QWriteLocker>

// Static constants
const QString FavoritesManager::DEFAULT_COLLECTION = "default";
const QString FavoritesManager::RECENT_COLLECTION = "recent";
const QString FavoritesManager::MOST_USED_COLLECTION = "most_used";

// FavoriteItem implementation
QJsonObject FavoriteItem::toJson() const {
    QJsonObject obj;
    obj["iconName"] = iconName;
    obj["displayName"] = displayName;
    obj["description"] = description;
    obj["dateAdded"] = dateAdded.toString(Qt::ISODate);
    obj["lastAccessed"] = lastAccessed.toString(Qt::ISODate);
    obj["accessCount"] = accessCount;
    obj["collection"] = collection;

    QJsonArray tagsArray;
    for (const QString& tag : tags) {
        tagsArray.append(tag);
    }
    obj["tags"] = tagsArray;

    return obj;
}

void FavoriteItem::fromJson(const QJsonObject& json) {
    iconName = json["iconName"].toString();
    displayName = json["displayName"].toString();
    description = json["description"].toString();
    dateAdded = QDateTime::fromString(json["dateAdded"].toString(), Qt::ISODate);
    lastAccessed = QDateTime::fromString(json["lastAccessed"].toString(), Qt::ISODate);
    accessCount = json["accessCount"].toInt();
    collection = json["collection"].toString();

    tags.clear();
    QJsonArray tagsArray = json["tags"].toArray();
    for (const QJsonValue& value : tagsArray) {
        tags.append(value.toString());
    }
}

// FavoriteCollection implementation
QJsonObject FavoriteCollection::toJson() const {
    QJsonObject obj;
    obj["name"] = name;
    obj["displayName"] = displayName;
    obj["description"] = description;
    obj["dateCreated"] = dateCreated.toString(Qt::ISODate);
    obj["dateModified"] = dateModified.toString(Qt::ISODate);
    obj["color"] = color;
    obj["icon"] = icon;
    obj["isDefault"] = isDefault;

    QJsonArray iconsArray;
    for (const QString& iconName : iconNames) {
        iconsArray.append(iconName);
    }
    obj["iconNames"] = iconsArray;

    return obj;
}

void FavoriteCollection::fromJson(const QJsonObject& json) {
    name = json["name"].toString();
    displayName = json["displayName"].toString();
    description = json["description"].toString();
    dateCreated = QDateTime::fromString(json["dateCreated"].toString(), Qt::ISODate);
    dateModified = QDateTime::fromString(json["dateModified"].toString(), Qt::ISODate);
    color = json["color"].toString();
    icon = json["icon"].toString();
    isDefault = json["isDefault"].toBool();

    iconNames.clear();
    QJsonArray iconsArray = json["iconNames"].toArray();
    for (const QJsonValue& value : iconsArray) {
        iconNames.append(value.toString());
    }
}

// RecentItem implementation
QJsonObject RecentItem::toJson() const {
    QJsonObject obj;
    obj["iconName"] = iconName;
    obj["lastAccessed"] = lastAccessed.toString(Qt::ISODate);
    obj["accessCount"] = accessCount;
    obj["totalViewTime"] = static_cast<double>(totalViewTime);
    return obj;
}

void RecentItem::fromJson(const QJsonObject& json) {
    iconName = json["iconName"].toString();
    lastAccessed = QDateTime::fromString(json["lastAccessed"].toString(), Qt::ISODate);
    accessCount = json["accessCount"].toInt();
    totalViewTime = static_cast<qint64>(json["totalViewTime"].toDouble());
}

// FavoritesManager implementation
FavoritesManager::FavoritesManager(QObject* parent)
    : QObject(parent), m_iconMetadataManager(nullptr), m_settings(new QSettings(this)),
      m_autoSaveTimer(new QTimer(this)), m_autoSave(true), m_dataChanged(false),
      m_maxRecentItems(DEFAULT_MAX_RECENT_ITEMS), m_autoSaveInterval(DEFAULT_AUTO_SAVE_INTERVAL),
      m_maxBackups(DEFAULT_MAX_BACKUPS) {
    GALLERY_LOG_INFO(galleryInit, "FavoritesManager constructor started");

    // Setup directories
    m_dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(m_dataDirectory);

    m_favoritesFile = QDir(m_dataDirectory).filePath("favorites.json");
    m_backupDirectory = QDir(m_dataDirectory).filePath("backups");
    QDir().mkpath(m_backupDirectory);

    // Setup auto-save timer
    m_autoSaveTimer->setSingleShot(false);
    m_autoSaveTimer->setInterval(m_autoSaveInterval * 1000);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &FavoritesManager::onAutoSaveTimer);

    // Setup default collections
    setupDefaultCollections();

    GALLERY_LOG_INFO(galleryInit, "FavoritesManager initialized successfully");
}

FavoritesManager::~FavoritesManager() {
    if (m_autoSave && m_dataChanged) {
        saveFavorites();
    }
}

void FavoritesManager::setIconMetadataManager(IconMetadataManager* manager) {
    m_iconMetadataManager = manager;
}

bool FavoritesManager::loadFavorites() {
    return loadFromFile(m_favoritesFile);
}

bool FavoritesManager::saveFavorites() {
    return saveToFile(m_favoritesFile);
}

void FavoritesManager::setAutoSave(bool enabled) {
    m_autoSave = enabled;
    if (enabled && !m_autoSaveTimer->isActive()) {
        m_autoSaveTimer->start();
    } else if (!enabled && m_autoSaveTimer->isActive()) {
        m_autoSaveTimer->stop();
    }
}

void FavoritesManager::addFavorite(const QString& iconName, const QString& collection) {
    QWriteLocker locker(&m_dataLock);

    if (m_favorites.contains(iconName)) {
        return; // Already a favorite
    }

    FavoriteItem item(iconName);
    item.collection = collection.isEmpty() ? DEFAULT_COLLECTION : collection;

    // Get metadata if available
    if (m_iconMetadataManager) {
        IconMetadata metadata = m_iconMetadataManager->getIconMetadata(iconName);
        if (metadata.isValid()) {
            item.displayName = metadata.getDisplayName();
            item.tags = metadata.tags;
        }
    }

    m_favorites[iconName] = item;

    // Add to collection
    if (!m_collections.contains(item.collection)) {
        createCollection(item.collection);
    }
    m_collections[item.collection].iconNames.append(iconName);
    m_collections[item.collection].dateModified = QDateTime::currentDateTime();

    m_dataChanged = true;

    locker.unlock();

    emit favoriteAdded(iconName, item.collection);
    emit favoritesChanged();
    emit dataChanged();
}

void FavoritesManager::removeFavorite(const QString& iconName) {
    QWriteLocker locker(&m_dataLock);

    if (!m_favorites.contains(iconName)) {
        return;
    }

    FavoriteItem item = m_favorites.take(iconName);

    // Remove from collection
    if (m_collections.contains(item.collection)) {
        m_collections[item.collection].iconNames.removeOne(iconName);
        m_collections[item.collection].dateModified = QDateTime::currentDateTime();
    }

    m_dataChanged = true;

    locker.unlock();

    emit favoriteRemoved(iconName);
    emit favoritesChanged();
    emit dataChanged();
}

bool FavoritesManager::isFavorite(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    return m_favorites.contains(iconName);
}

void FavoritesManager::toggleFavorite(const QString& iconName) {
    if (isFavorite(iconName)) {
        removeFavorite(iconName);
    } else {
        addFavorite(iconName);
    }
}

QStringList FavoritesManager::getFavorites() const {
    QReadLocker locker(&m_dataLock);
    return m_favorites.keys();
}

QStringList FavoritesManager::getFavorites(const QString& collection) const {
    QReadLocker locker(&m_dataLock);
    if (m_collections.contains(collection)) {
        return m_collections[collection].iconNames;
    }
    return QStringList();
}

FavoriteItem FavoritesManager::getFavoriteItem(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    return m_favorites.value(iconName);
}

void FavoritesManager::clearFavorites() {
    QWriteLocker locker(&m_dataLock);

    m_favorites.clear();

    // Clear all collections except default ones
    for (auto it = m_collections.begin(); it != m_collections.end(); ++it) {
        it.value().iconNames.clear();
        it.value().dateModified = QDateTime::currentDateTime();
    }

    m_dataChanged = true;

    locker.unlock();

    emit favoritesChanged();
    emit dataChanged();
}

void FavoritesManager::recordAccess(const QString& iconName, qint64 viewTime) {
    updateRecentItem(iconName, viewTime);
    updateFavoriteItem(iconName);
}

QStringList FavoritesManager::getRecentlyViewed(int limit) const {
    QReadLocker locker(&m_dataLock);

    if (limit <= 0 || limit >= m_recentOrder.size()) {
        return m_recentOrder;
    }

    return m_recentOrder.mid(0, limit);
}

int FavoritesManager::getTotalFavorites() const {
    QReadLocker locker(&m_dataLock);
    return static_cast<int>(m_favorites.size());
}

// Private methods
void FavoritesManager::setupDefaultCollections() {
    createCollection(DEFAULT_COLLECTION, "Default");
    createCollection(RECENT_COLLECTION, "Recent");
    createCollection(MOST_USED_COLLECTION, "Most Used");

    m_collections[DEFAULT_COLLECTION].isDefault = true;
    m_collections[DEFAULT_COLLECTION].description = "Default favorites collection";
    m_collections[RECENT_COLLECTION].description = "Recently viewed icons";
    m_collections[MOST_USED_COLLECTION].description = "Most frequently used icons";
}

void FavoritesManager::updateFavoriteItem(const QString& iconName) {
    QWriteLocker locker(&m_dataLock);

    if (m_favorites.contains(iconName)) {
        m_favorites[iconName].lastAccessed = QDateTime::currentDateTime();
        m_favorites[iconName].accessCount++;
        m_dataChanged = true;
    }
}

void FavoritesManager::updateRecentItem(const QString& iconName, qint64 viewTime) {
    QWriteLocker locker(&m_dataLock);

    // Update or create recent item
    if (m_recentItems.contains(iconName)) {
        m_recentItems[iconName].lastAccessed = QDateTime::currentDateTime();
        m_recentItems[iconName].accessCount++;
        m_recentItems[iconName].totalViewTime += viewTime;
    } else {
        RecentItem item(iconName);
        item.totalViewTime = viewTime;
        m_recentItems[iconName] = item;
    }

    // Update recent order
    m_recentOrder.removeOne(iconName);
    m_recentOrder.prepend(iconName);

    // Limit recent items
    while (m_recentOrder.size() > m_maxRecentItems) {
        QString removedIcon = m_recentOrder.takeLast();
        m_recentItems.remove(removedIcon);
    }

    m_dataChanged = true;

    locker.unlock();

    emit recentItemAdded(iconName);
    emit recentHistoryChanged();
}

void FavoritesManager::createCollection(const QString& name, const QString& displayName) {
    QWriteLocker locker(&m_dataLock);

    if (m_collections.contains(name)) {
        return;
    }

    FavoriteCollection collection(name);
    collection.displayName = displayName.isEmpty() ? name : displayName;
    m_collections[name] = collection;

    m_dataChanged = true;

    locker.unlock();

    emit collectionCreated(name);
    emit collectionsChanged();
}

QString FavoritesManager::getFavoritesFilePath() const {
    return m_favoritesFile;
}

bool FavoritesManager::saveToFile(const QString& filePath) const {
    QJsonObject data = serializeData();
    QJsonDocument doc(data);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        GALLERY_LOG_WARNING(galleryMain,
                            QString("Failed to open favorites file for writing: %1").arg(filePath));
        return false;
    }

    file.write(doc.toJson());
    return true;
}

bool FavoritesManager::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return true; // Not an error if file doesn't exist
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        GALLERY_LOG_WARNING(galleryMain,
                            QString("Failed to parse favorites file: %1").arg(error.errorString()));
        return false;
    }

    return deserializeData(doc.object());
}

QJsonObject FavoritesManager::serializeData() const {
    QReadLocker locker(&m_dataLock);

    QJsonObject root;
    root["version"] = "1.0";
    root["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    // Serialize favorites
    QJsonObject favoritesObj;
    for (auto it = m_favorites.begin(); it != m_favorites.end(); ++it) {
        favoritesObj[it.key()] = it.value().toJson();
    }
    root["favorites"] = favoritesObj;

    // Serialize collections
    QJsonObject collectionsObj;
    for (auto it = m_collections.begin(); it != m_collections.end(); ++it) {
        collectionsObj[it.key()] = it.value().toJson();
    }
    root["collections"] = collectionsObj;

    // Serialize recent items
    QJsonObject recentObj;
    for (auto it = m_recentItems.begin(); it != m_recentItems.end(); ++it) {
        recentObj[it.key()] = it.value().toJson();
    }
    root["recent"] = recentObj;

    // Serialize recent order
    QJsonArray recentOrderArray;
    for (const QString& iconName : m_recentOrder) {
        recentOrderArray.append(iconName);
    }
    root["recentOrder"] = recentOrderArray;

    return root;
}

bool FavoritesManager::deserializeData(const QJsonObject& data) {
    QWriteLocker locker(&m_dataLock);

    // Clear existing data
    m_favorites.clear();
    m_collections.clear();
    m_recentItems.clear();
    m_recentOrder.clear();

    // Setup default collections
    setupDefaultCollections();

    // Deserialize favorites
    QJsonObject favoritesObj = data["favorites"].toObject();
    for (auto it = favoritesObj.begin(); it != favoritesObj.end(); ++it) {
        FavoriteItem item;
        item.fromJson(it.value().toObject());
        m_favorites[it.key()] = item;
    }

    // Deserialize collections
    QJsonObject collectionsObj = data["collections"].toObject();
    for (auto it = collectionsObj.begin(); it != collectionsObj.end(); ++it) {
        FavoriteCollection collection;
        collection.fromJson(it.value().toObject());
        m_collections[it.key()] = collection;
    }

    // Deserialize recent items
    QJsonObject recentObj = data["recent"].toObject();
    for (auto it = recentObj.begin(); it != recentObj.end(); ++it) {
        RecentItem item;
        item.fromJson(it.value().toObject());
        m_recentItems[it.key()] = item;
    }

    // Deserialize recent order
    QJsonArray recentOrderArray = data["recentOrder"].toArray();
    for (const QJsonValue& value : recentOrderArray) {
        m_recentOrder.append(value.toString());
    }

    m_dataChanged = false;

    locker.unlock();

    emit loadCompleted(true);
    return true;
}

void FavoritesManager::onAutoSaveTimer() {
    if (m_dataChanged) {
        bool success = saveFavorites();
        if (success) {
            m_dataChanged = false;
        }
        emit saveCompleted(success);
    }
}
