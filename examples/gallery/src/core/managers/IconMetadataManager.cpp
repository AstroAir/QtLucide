/**
 * QtLucide Gallery Application - Icon Metadata Manager Implementation
 */

#include "IconMetadataManager.h"
#include "../utils/GalleryLogger.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QReadLocker>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QWriteLocker>
// #include <QtConcurrent/QtConcurrent> // Removed for simplicity
#include <QApplication>

IconMetadataManager::IconMetadataManager(QObject* parent)
    : QObject(parent), m_isLoaded(false), m_loadWatcher(new QFutureWatcher<bool>(this)) {
    connect(m_loadWatcher, &QFutureWatcher<bool>::finished, this,
            &IconMetadataManager::onMetadataLoadFinished);
}

IconMetadataManager::~IconMetadataManager() {
    if (m_loadWatcher->isRunning()) {
        m_loadWatcher->cancel();
        m_loadWatcher->waitForFinished();
    }
}

bool IconMetadataManager::loadMetadata() {
    if (m_isLoaded) {
        GALLERY_LOG_DEBUG(galleryMetadata, "Metadata already loaded, skipping");
        return true;
    }

    GALLERY_LOG_INFO(galleryMetadata, "Starting metadata loading");
    GALLERY_START_TIMER("Total metadata loading");

    // Load synchronously for simplicity
    QWriteLocker locker(&m_dataLock);

    bool success = true;

    GALLERY_LOG_DEBUG(galleryMetadata, "Loading icons metadata");
    success &= loadIconsMetadata();

    GALLERY_LOG_DEBUG(galleryMetadata, "Loading categories metadata");
    success &= loadCategoriesMetadata();

    GALLERY_LOG_DEBUG(galleryMetadata, "Loading tags metadata");
    success &= loadTagsMetadata();

    if (success) {
        GALLERY_LOG_INFO(galleryMetadata, "Building search index");
        buildSearchIndex();

        GALLERY_LOG_DEBUG(galleryMetadata, "Computing display names");
        computeDisplayNames();

        GALLERY_LOG_DEBUG(galleryMetadata, "Loading favorites");
        loadFavorites();

        GALLERY_LOG_DEBUG(galleryMetadata, "Loading usage data");
        loadUsageData();

        m_isLoaded = true;

        GALLERY_END_TIMER("Total metadata loading");
        GALLERY_LOG_INFO(
            galleryMetadata,
            QString("Metadata loading completed successfully - %1 icons, %2 categories, %3 tags")
                .arg(static_cast<int>(m_iconMetadata.size()))
                .arg(static_cast<int>(m_allCategories.size()))
                .arg(static_cast<int>(m_allTags.size())));

        emit metadataLoaded(static_cast<int>(m_iconMetadata.size()));
    } else {
        GALLERY_LOG_CRITICAL(galleryMetadata, "Failed to load icon metadata");
        emit metadataLoadFailed("Failed to load icon metadata");
    }

    return success;
}

bool IconMetadataManager::loadIconsMetadata() {
    QJsonDocument doc = loadJsonFile(":/lucide/metadata/icons.json");
    if (doc.isNull()) {
        qWarning() << "Failed to load icons metadata, creating fallback data";
        // Create minimal fallback metadata
        createFallbackIconMetadata();
        return true; // Return true to allow application to continue with limited functionality
    }

    QJsonObject root = doc.object();
    QJsonObject icons = root["icons"].toObject();

    m_iconMetadata.clear();
    m_iconMetadata.reserve(icons.size());

    for (auto it = icons.begin(); it != icons.end(); ++it) {
        const QString iconName = it.key();
        const QJsonObject iconData = it.value().toObject();

        IconMetadata metadata(iconName);
        metadata.svgFile = iconData["svg_file"].toString();

        // Load tags
        QJsonArray tagsArray = iconData["tags"].toArray();
        for (const QJsonValue& tag : tagsArray) {
            metadata.tags.append(tag.toString());
        }

        // Load categories
        QJsonArray categoriesArray = iconData["categories"].toArray();
        for (const QJsonValue& category : categoriesArray) {
            metadata.categories.append(category.toString());
        }

        // Load contributors
        QJsonArray contributorsArray = iconData["contributors"].toArray();
        for (const QJsonValue& contributor : contributorsArray) {
            metadata.contributors.append(contributor.toString());
        }

        m_iconMetadata[iconName] = metadata;
    }

    qDebug() << "Loaded metadata for" << static_cast<int>(m_iconMetadata.size()) << "icons";
    return true;
}

bool IconMetadataManager::loadCategoriesMetadata() {
    QJsonDocument doc = loadJsonFile(":/lucide/metadata/categories.json");
    if (doc.isNull()) {
        qWarning() << "Failed to load categories metadata, creating fallback data";
        // Create minimal fallback categories
        createFallbackCategoryMetadata();
        return true; // Return true to allow application to continue
    }

    QJsonObject root = doc.object();
    m_categories.clear();
    m_allCategories.clear();

    for (auto it = root.begin(); it != root.end(); ++it) {
        const QString category = it.key();
        const QJsonArray iconsArray = it.value().toArray();

        QStringList iconNames;
        for (const QJsonValue& iconValue : iconsArray) {
            iconNames.append(iconValue.toString());
        }

        m_categories[category] = iconNames;
        m_allCategories.append(category);
    }

    m_allCategories.sort();
    qDebug() << "Loaded" << static_cast<int>(m_categories.size()) << "categories";
    return true;
}

bool IconMetadataManager::loadTagsMetadata() {
    QJsonDocument doc = loadJsonFile(":/lucide/metadata/tags.json");
    if (doc.isNull()) {
        qWarning() << "Failed to load tags metadata";
        return false;
    }

    QJsonObject root = doc.object();
    m_tagToIcons.clear();
    m_allTags.clear();

    for (auto it = root.begin(); it != root.end(); ++it) {
        const QString tag = it.key();
        const QJsonArray iconsArray = it.value().toArray();

        QStringList iconNames;
        for (const QJsonValue& iconValue : iconsArray) {
            iconNames.append(iconValue.toString());
        }

        m_tagToIcons[tag] = iconNames;
        m_allTags.append(tag);
    }

    m_allTags.sort();
    qDebug() << "Loaded" << static_cast<int>(m_tagToIcons.size()) << "tags";
    return true;
}

void IconMetadataManager::buildSearchIndex() {
    // Build search text for each icon
    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        IconMetadata& metadata = it.value();
        metadata.searchText = createSearchText(metadata);
    }
}

void IconMetadataManager::computeDisplayNames() {
    // Create user-friendly display names
    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        IconMetadata& metadata = it.value();
        metadata.displayName = createDisplayName(metadata.name);
    }
}

QString IconMetadataManager::createSearchText(const IconMetadata& metadata) const {
    QStringList searchTerms;
    searchTerms << metadata.name;
    searchTerms << metadata.displayName;
    searchTerms << metadata.tags;
    searchTerms << metadata.categories;

    return searchTerms.join(" ").toLower();
}

QString IconMetadataManager::createDisplayName(const QString& iconName) const {
    QString displayName = iconName;

    // Replace hyphens and underscores with spaces
    displayName.replace(QRegularExpression("[-_]"), " ");

    // Capitalize first letter of each word
    QStringList words = displayName.split(" ", Qt::SkipEmptyParts);
    for (QString& word : words) {
        if (!word.isEmpty()) {
            word[0] = word[0].toUpper();
        }
    }

    return words.join(" ");
}

QJsonDocument IconMetadataManager::loadJsonFile(const QString& resourcePath) const {
    // First check if the resource exists
    QResource resource(resourcePath);
    if (!resource.isValid()) {
        qWarning() << "Resource does not exist:" << resourcePath;
        return QJsonDocument();
    }

    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open resource file:" << resourcePath << "Error:" << file.errorString();
        return QJsonDocument();
    }

    QByteArray data = file.readAll();
    if (data.isEmpty()) {
        qWarning() << "Resource file is empty:" << resourcePath;
        return QJsonDocument();
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error in" << resourcePath << "at offset" << error.offset << ":" << error.errorString();
        return QJsonDocument();
    }

    qDebug() << "Successfully loaded JSON resource:" << resourcePath << "Size:" << data.size() << "bytes";
    return doc;
}

void IconMetadataManager::createFallbackIconMetadata() {
    qWarning() << "Creating fallback icon metadata - limited functionality will be available";

    // Create basic metadata for common icons that should exist
    QStringList commonIcons = {
        "home", "user", "settings", "search", "heart", "star", "plus", "minus",
        "edit", "delete", "save", "download", "upload", "refresh", "info", "warning"
    };

    m_iconMetadata.clear();
    m_allIconNames.clear();

    for (const QString& iconName : commonIcons) {
        IconMetadata metadata;
        metadata.name = iconName;
        metadata.displayName = iconName.left(1).toUpper() + iconName.mid(1);
        metadata.description = QString("Fallback icon: %1").arg(iconName);
        metadata.categories = QStringList{"general"};
        metadata.tags = QStringList{iconName, "fallback"};
        metadata.contributors = QStringList{"QtLucide"};
        metadata.usageCount = 0;
        metadata.lastUsed = QDateTime::currentDateTime();

        m_iconMetadata[iconName] = metadata;
        m_allIconNames.append(iconName);
    }

    qDebug() << "Created fallback metadata for" << commonIcons.size() << "icons";
}

void IconMetadataManager::createFallbackCategoryMetadata() {
    qWarning() << "Creating fallback category metadata";

    m_categories.clear();
    m_allCategories.clear();

    // Create basic categories
    m_categories["general"] = m_allIconNames;
    m_categories["fallback"] = m_allIconNames;

    m_allCategories = QStringList{"general", "fallback"};

    qDebug() << "Created fallback categories:" << m_allCategories;
}

void IconMetadataManager::createFallbackTagMetadata() {
    qWarning() << "Creating fallback tag metadata";

    m_tagToIcons.clear();
    m_allTags.clear();

    // Create basic tags
    m_tagToIcons["fallback"] = m_allIconNames;
    m_tagToIcons["general"] = m_allIconNames;

    m_allTags = QStringList{"fallback", "general"};

    qDebug() << "Created fallback tags:" << m_allTags;
}

void IconMetadataManager::onMetadataLoadFinished() {
    bool success = m_loadWatcher->result();

    if (success) {
        m_isLoaded = true;
        emit metadataLoaded(static_cast<int>(m_iconMetadata.size()));
        qDebug() << "Metadata loading completed successfully";
    } else {
        emit metadataLoadFailed("Failed to load icon metadata");
        qWarning() << "Metadata loading failed";
    }
}

QStringList IconMetadataManager::getAllIconNames() const {
    QReadLocker locker(&m_dataLock);
    return m_iconMetadata.keys();
}

QStringList IconMetadataManager::getAllCategories() const {
    QReadLocker locker(&m_dataLock);
    return m_allCategories;
}

QStringList IconMetadataManager::getAllTags() const {
    QReadLocker locker(&m_dataLock);
    return m_allTags;
}

IconMetadata IconMetadataManager::getIconMetadata(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    return m_iconMetadata.value(iconName);
}

QStringList IconMetadataManager::searchIcons(const IconFilterCriteria& criteria) const {
    QReadLocker locker(&m_dataLock);

    QStringList results;

    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        const QString& iconName = it.key();
        const IconMetadata& metadata = it.value();

        // Apply search text filter
        if (!criteria.searchText.isEmpty()) {
            if (!metadata.matchesSearch(criteria.searchText)) {
                continue;
            }
        }

        // Apply category filter
        if (!criteria.categories.isEmpty()) {
            bool hasMatchingCategory = false;
            for (const QString& category : criteria.categories) {
                if (metadata.hasCategory(category)) {
                    hasMatchingCategory = true;
                    break;
                }
            }
            if (!hasMatchingCategory) {
                continue;
            }
        }

        // Apply tag filter
        if (!criteria.tags.isEmpty()) {
            bool hasMatchingTag = false;
            for (const QString& tag : criteria.tags) {
                if (metadata.hasTag(tag)) {
                    hasMatchingTag = true;
                    break;
                }
            }
            if (!hasMatchingTag) {
                continue;
            }
        }

        // Apply favorites filter
        if (criteria.favoritesOnly && !metadata.isFavorite) {
            continue;
        }

        // Apply recently used filter
        if (criteria.recentlyUsedOnly && !m_recentlyUsed.contains(iconName)) {
            continue;
        }

        results.append(iconName);
    }

    // Sort results
    results = sortIcons(results, criteria.sortOrder, criteria.sortAscending);

    return results;
}

QStringList IconMetadataManager::getIconsByCategory(const QString& category) const {
    QReadLocker locker(&m_dataLock);
    return m_categories.value(category);
}

QStringList IconMetadataManager::getIconsByTag(const QString& tag) const {
    QReadLocker locker(&m_dataLock);
    return m_tagToIcons.value(tag);
}

QStringList IconMetadataManager::getFavoriteIcons() const {
    QReadLocker locker(&m_dataLock);
    return m_favoriteIcons;
}

QStringList IconMetadataManager::getRecentlyUsedIcons(int limit) const {
    QReadLocker locker(&m_dataLock);
    if (limit <= 0 || limit >= static_cast<int>(m_recentlyUsed.size())) {
        return m_recentlyUsed;
    }
    return m_recentlyUsed.mid(0, limit);
}

void IconMetadataManager::addToFavorites(const QString& iconName) {
    QWriteLocker locker(&m_dataLock);
    if (!m_favoriteIcons.contains(iconName)) {
        m_favoriteIcons.append(iconName);
        if (m_iconMetadata.contains(iconName)) {
            m_iconMetadata[iconName].isFavorite = true;
        }
        emit favoritesChanged();
    }
}

void IconMetadataManager::removeFromFavorites(const QString& iconName) {
    QWriteLocker locker(&m_dataLock);
    if (m_favoriteIcons.removeOne(iconName)) {
        if (m_iconMetadata.contains(iconName)) {
            m_iconMetadata[iconName].isFavorite = false;
        }
        emit favoritesChanged();
    }
}

bool IconMetadataManager::isFavorite(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    return m_favoriteIcons.contains(iconName);
}

void IconMetadataManager::clearFavorites() {
    QWriteLocker locker(&m_dataLock);
    for (const QString& iconName : m_favoriteIcons) {
        if (m_iconMetadata.contains(iconName)) {
            m_iconMetadata[iconName].isFavorite = false;
        }
    }
    m_favoriteIcons.clear();
    emit favoritesChanged();
}

void IconMetadataManager::recordIconUsage(const QString& iconName) {
    QWriteLocker locker(&m_dataLock);

    // Update usage count
    m_usageCount[iconName]++;
    if (m_iconMetadata.contains(iconName)) {
        m_iconMetadata[iconName].usageCount = m_usageCount[iconName];
    }

    // Update recently used list
    m_recentlyUsed.removeOne(iconName);
    m_recentlyUsed.prepend(iconName);

    // Limit recently used list size
    while (static_cast<int>(m_recentlyUsed.size()) > MAX_RECENT_ITEMS) {
        m_recentlyUsed.removeLast();
    }

    emit usageDataChanged();
}

int IconMetadataManager::getIconUsageCount(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    return m_usageCount.value(iconName, 0);
}

void IconMetadataManager::clearUsageHistory() {
    QWriteLocker locker(&m_dataLock);
    m_usageCount.clear();
    m_recentlyUsed.clear();

    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        it.value().usageCount = 0;
    }

    emit usageDataChanged();
}

QString IconMetadataManager::getFavoritesFilePath() const {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return QDir(dataDir).filePath(FAVORITES_FILE);
}

QString IconMetadataManager::getUsageDataFilePath() const {
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return QDir(dataDir).filePath(USAGE_FILE);
}

bool IconMetadataManager::saveFavorites(const QString& filePath) const {
    QString path = filePath.isEmpty() ? getFavoritesFilePath() : filePath;

    QJsonObject root;
    QJsonArray favoritesArray;

    QReadLocker locker(&m_dataLock);
    for (const QString& iconName : m_favoriteIcons) {
        favoritesArray.append(iconName);
    }

    root["favorites"] = favoritesArray;
    root["version"] = "1.0";
    root["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(root);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save favorites to" << path;
        return false;
    }

    file.write(doc.toJson());
    return true;
}

bool IconMetadataManager::loadFavorites(const QString& filePath) {
    QString path = filePath.isEmpty() ? getFavoritesFilePath() : filePath;

    QFile file(path);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return true; // Not an error if file doesn't exist
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse favorites file:" << error.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    QJsonArray favoritesArray = root["favorites"].toArray();

    QWriteLocker locker(&m_dataLock);
    m_favoriteIcons.clear();

    for (const QJsonValue& value : favoritesArray) {
        QString iconName = value.toString();
        if (m_iconMetadata.contains(iconName)) {
            m_favoriteIcons.append(iconName);
            m_iconMetadata[iconName].isFavorite = true;
        }
    }

    return true;
}

bool IconMetadataManager::saveUsageData(const QString& filePath) const {
    QString path = filePath.isEmpty() ? getUsageDataFilePath() : filePath;

    QJsonObject root;
    QJsonObject usageObject;
    QJsonArray recentArray;

    QReadLocker locker(&m_dataLock);

    // Save usage counts
    for (auto it = m_usageCount.begin(); it != m_usageCount.end(); ++it) {
        usageObject[it.key()] = it.value();
    }

    // Save recently used list
    for (const QString& iconName : m_recentlyUsed) {
        recentArray.append(iconName);
    }

    root["usage"] = usageObject;
    root["recent"] = recentArray;
    root["version"] = "1.0";
    root["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonDocument doc(root);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save usage data to" << path;
        return false;
    }

    file.write(doc.toJson());
    return true;
}

bool IconMetadataManager::loadUsageData(const QString& filePath) {
    QString path = filePath.isEmpty() ? getUsageDataFilePath() : filePath;

    QFile file(path);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return true; // Not an error if file doesn't exist
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse usage data file:" << error.errorString();
        return false;
    }

    QJsonObject root = doc.object();
    QJsonObject usageObject = root["usage"].toObject();
    QJsonArray recentArray = root["recent"].toArray();

    QWriteLocker locker(&m_dataLock);

    // Load usage counts
    m_usageCount.clear();
    for (auto it = usageObject.begin(); it != usageObject.end(); ++it) {
        QString iconName = it.key();
        int count = it.value().toInt();
        if (m_iconMetadata.contains(iconName)) {
            m_usageCount[iconName] = count;
            m_iconMetadata[iconName].usageCount = count;
        }
    }

    // Load recently used list
    m_recentlyUsed.clear();
    for (const QJsonValue& value : recentArray) {
        QString iconName = value.toString();
        if (m_iconMetadata.contains(iconName)) {
            m_recentlyUsed.append(iconName);
        }
    }

    return true;
}

QStringList IconMetadataManager::getSearchSuggestions(const QString& partialText,
                                                      int maxSuggestions) const {
    if (partialText.length() < 2) {
        return QStringList();
    }

    QReadLocker locker(&m_dataLock);
    QStringList suggestions;
    QString lowerPartial = partialText.toLower();

    // Search in icon names
    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        const QString& iconName = it.key();
        const IconMetadata& metadata = it.value();

        if (iconName.startsWith(lowerPartial, Qt::CaseInsensitive)) {
            suggestions.append(iconName);
        } else if (metadata.displayName.contains(lowerPartial, Qt::CaseInsensitive)) {
            suggestions.append(iconName);
        } else if (metadata.searchText.contains(lowerPartial)) {
            suggestions.append(iconName);
        }

        if (static_cast<int>(suggestions.size()) >= maxSuggestions) {
            break;
        }
    }

    // Remove duplicates and sort
    suggestions.removeDuplicates();
    suggestions.sort();

    return suggestions.mid(0, maxSuggestions);
}

QStringList IconMetadataManager::getCategorySuggestions(const QString& partialText) const {
    if (partialText.length() < 2) {
        return QStringList();
    }

    QReadLocker locker(&m_dataLock);
    QStringList suggestions;
    QString lowerPartial = partialText.toLower();

    for (const QString& category : m_allCategories) {
        if (category.contains(lowerPartial, Qt::CaseInsensitive)) {
            suggestions.append(category);
        }
    }

    return suggestions;
}

QStringList IconMetadataManager::getTagSuggestions(const QString& partialText) const {
    if (partialText.length() < 2) {
        return QStringList();
    }

    QReadLocker locker(&m_dataLock);
    QStringList suggestions;
    QString lowerPartial = partialText.toLower();

    for (const QString& tag : m_allTags) {
        if (tag.contains(lowerPartial, Qt::CaseInsensitive)) {
            suggestions.append(tag);
        }
    }

    return suggestions;
}

void IconMetadataManager::refreshMetadata() {
    m_isLoaded = false;
    loadMetadata();
}

void IconMetadataManager::applyFilter(const IconFilterCriteria& criteria) {
    m_currentFilter = criteria;
    QStringList filteredIcons = searchIcons(criteria);
    m_filteredIcons = filteredIcons;
    emit filteredIconsChanged(filteredIcons);
}

QStringList IconMetadataManager::sortIcons(const QStringList& iconNames,
                                           IconFilterCriteria::SortOrder order,
                                           bool ascending) const {
    QStringList sorted = iconNames;

    switch (order) {
        case IconFilterCriteria::SortByName:
            std::sort(sorted.begin(), sorted.end(),
                      [ascending](const QString& a, const QString& b) {
                          return ascending ? a < b : a > b;
                      });
            break;

        case IconFilterCriteria::SortByCategory:
            std::sort(sorted.begin(), sorted.end(),
                      [this, ascending](const QString& a, const QString& b) {
                          const IconMetadata& metaA = m_iconMetadata.value(a);
                          const IconMetadata& metaB = m_iconMetadata.value(b);
                          QString catA = metaA.categories.isEmpty() ? "" : metaA.categories.first();
                          QString catB = metaB.categories.isEmpty() ? "" : metaB.categories.first();
                          return ascending ? catA < catB : catA > catB;
                      });
            break;

        case IconFilterCriteria::SortByUsage:
            std::sort(sorted.begin(), sorted.end(),
                      [this, ascending](const QString& a, const QString& b) {
                          int usageA = m_usageCount.value(a, 0);
                          int usageB = m_usageCount.value(b, 0);
                          return ascending ? usageA < usageB : usageA > usageB;
                      });
            break;

        case IconFilterCriteria::SortByRecent:
            std::sort(sorted.begin(), sorted.end(),
                      [this, ascending](const QString& a, const QString& b) {
                          int indexA = static_cast<int>(m_recentlyUsed.indexOf(a));
                          int indexB = static_cast<int>(m_recentlyUsed.indexOf(b));
                          if (indexA == -1)
                              indexA = INT_MAX;
                          if (indexB == -1)
                              indexB = INT_MAX;
                          return ascending ? indexA > indexB : indexA < indexB;
                      });
            break;

        case IconFilterCriteria::SortByDisplayName:
            // For now, treat display name same as name
            std::sort(sorted.begin(), sorted.end(),
                      [ascending](const QString& a, const QString& b) {
                          return ascending ? a < b : a > b;
                      });
            break;

        case IconFilterCriteria::SortByDateAdded:
            // For now, sort by name as we don't have date added metadata
            std::sort(sorted.begin(), sorted.end(),
                      [ascending](const QString& a, const QString& b) {
                          return ascending ? a < b : a > b;
                      });
            break;

        case IconFilterCriteria::SortByFileSize:
            // For now, sort by name as we don't have file size metadata
            std::sort(sorted.begin(), sorted.end(),
                      [ascending](const QString& a, const QString& b) {
                          return ascending ? a < b : a > b;
                      });
            break;

        case IconFilterCriteria::SortByRelevance:
            // For now, sort by name as we don't have relevance scoring
            std::sort(sorted.begin(), sorted.end(),
                      [ascending](const QString& a, const QString& b) {
                          return ascending ? a < b : a > b;
                      });
            break;
    }

    return sorted;
}

void IconMetadataManager::clearFilter() {
    m_currentFilter = IconFilterCriteria();
    m_filteredIcons.clear();
    emit filteredIconsChanged(getAllIconNames());
}

void IconMetadataManager::onAsyncLoadProgress(int current, int total) {
    emit metadataLoadProgress(current, total);
}

// Missing methods that are called from other code
int IconMetadataManager::getTotalIconCount() const {
    QReadLocker locker(&m_dataLock);
    return static_cast<int>(m_iconMetadata.size());
}

int IconMetadataManager::getCategoryCount() const {
    QReadLocker locker(&m_dataLock);
    return static_cast<int>(m_categories.size());
}

int IconMetadataManager::getTagCount() const {
    QReadLocker locker(&m_dataLock);
    QSet<QString> allTags;
    for (const auto& metadata : m_iconMetadata) {
        for (const QString& tag : metadata.tags) {
            allTags.insert(tag);
        }
    }
    return static_cast<int>(allTags.size());
}

// Missing IconMetadataManager methods
void IconMetadataManager::setIconTags(const QString& iconName, const QList<QString>& tags) {
    QWriteLocker locker(&m_dataLock);
    if (m_iconMetadata.contains(iconName)) {
        m_iconMetadata[iconName].tags = tags;
        // Metadata updated - could emit filteredIconsChanged if needed
    }
}

void IconMetadataManager::setIconDescription(const QString& iconName, const QString& description) {
    QWriteLocker locker(&m_dataLock);
    if (m_iconMetadata.contains(iconName)) {
        m_iconMetadata[iconName].description = description;
        // Metadata updated - could emit filteredIconsChanged if needed
    }
}

QList<QString> IconMetadataManager::getIconTags(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    if (m_iconMetadata.contains(iconName)) {
        return m_iconMetadata[iconName].tags;
    }
    return QList<QString>();
}

QString IconMetadataManager::getIconDescription(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    if (m_iconMetadata.contains(iconName)) {
        return m_iconMetadata[iconName].description;
    }
    return QString();
}

QList<QString> IconMetadataManager::getIconCategories(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    if (m_iconMetadata.contains(iconName)) {
        return m_iconMetadata[iconName].categories;
    }
    return QList<QString>();
}

QList<QString> IconMetadataManager::getIconContributors(const QString& iconName) const {
    QReadLocker locker(&m_dataLock);
    if (m_iconMetadata.contains(iconName)) {
        return m_iconMetadata[iconName].contributors;
    }
    return QList<QString>();
}
