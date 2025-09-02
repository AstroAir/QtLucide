/**
 * QtLucide Gallery Application - Icon Metadata Manager Implementation
 */

#include "IconMetadataManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QRegularExpression>
// #include <QtConcurrent/QtConcurrent> // Removed for simplicity
#include <QApplication>

IconMetadataManager::IconMetadataManager(QObject *parent)
    : QObject(parent)
    , m_isLoaded(false)
    , m_loadWatcher(new QFutureWatcher<bool>(this))
{
    connect(m_loadWatcher, &QFutureWatcher<bool>::finished,
            this, &IconMetadataManager::onMetadataLoadFinished);
}

IconMetadataManager::~IconMetadataManager()
{
    if (m_loadWatcher->isRunning()) {
        m_loadWatcher->cancel();
        m_loadWatcher->waitForFinished();
    }
}

bool IconMetadataManager::loadMetadata()
{
    if (m_isLoaded) {
        return true;
    }

    // Load synchronously for simplicity
    QMutexLocker locker(&m_dataMutex);

    bool success = true;
    success &= loadIconsMetadata();
    success &= loadCategoriesMetadata();
    success &= loadTagsMetadata();

    if (success) {
        buildSearchIndex();
        computeDisplayNames();
        loadFavorites();
        loadUsageData();
        m_isLoaded = true;
        emit metadataLoaded(m_iconMetadata.size());
    } else {
        emit metadataLoadFailed("Failed to load icon metadata");
    }

    return success;
}

bool IconMetadataManager::loadIconsMetadata()
{
    QJsonDocument doc = loadJsonFile(":/lucide/metadata/icons.json");
    if (doc.isNull()) {
        qWarning() << "Failed to load icons metadata";
        return false;
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
    
    qDebug() << "Loaded metadata for" << m_iconMetadata.size() << "icons";
    return true;
}

bool IconMetadataManager::loadCategoriesMetadata()
{
    QJsonDocument doc = loadJsonFile(":/lucide/metadata/categories.json");
    if (doc.isNull()) {
        qWarning() << "Failed to load categories metadata";
        return false;
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
    qDebug() << "Loaded" << m_categories.size() << "categories";
    return true;
}

bool IconMetadataManager::loadTagsMetadata()
{
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
    qDebug() << "Loaded" << m_tagToIcons.size() << "tags";
    return true;
}

void IconMetadataManager::buildSearchIndex()
{
    // Build search text for each icon
    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        IconMetadata& metadata = it.value();
        metadata.searchText = createSearchText(metadata);
    }
}

void IconMetadataManager::computeDisplayNames()
{
    // Create user-friendly display names
    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        IconMetadata& metadata = it.value();
        metadata.displayName = createDisplayName(metadata.name);
    }
}

QString IconMetadataManager::createSearchText(const IconMetadata& metadata) const
{
    QStringList searchTerms;
    searchTerms << metadata.name;
    searchTerms << metadata.displayName;
    searchTerms << metadata.tags;
    searchTerms << metadata.categories;
    
    return searchTerms.join(" ").toLower();
}

QString IconMetadataManager::createDisplayName(const QString& iconName) const
{
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

QJsonDocument IconMetadataManager::loadJsonFile(const QString& resourcePath) const
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open" << resourcePath;
        return QJsonDocument();
    }
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error in" << resourcePath << ":" << error.errorString();
        return QJsonDocument();
    }
    
    return doc;
}

void IconMetadataManager::onMetadataLoadFinished()
{
    bool success = m_loadWatcher->result();
    
    if (success) {
        m_isLoaded = true;
        emit metadataLoaded(m_iconMetadata.size());
        qDebug() << "Metadata loading completed successfully";
    } else {
        emit metadataLoadFailed("Failed to load icon metadata");
        qWarning() << "Metadata loading failed";
    }
}

QStringList IconMetadataManager::getAllIconNames() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_iconMetadata.keys();
}

QStringList IconMetadataManager::getAllCategories() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_allCategories;
}

QStringList IconMetadataManager::getAllTags() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_allTags;
}

IconMetadata IconMetadataManager::getIconMetadata(const QString& iconName) const
{
    QMutexLocker locker(&m_dataMutex);
    return m_iconMetadata.value(iconName);
}

QStringList IconMetadataManager::searchIcons(const IconFilterCriteria& criteria) const
{
    QMutexLocker locker(&m_dataMutex);
    
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

QStringList IconMetadataManager::getIconsByCategory(const QString& category) const
{
    QMutexLocker locker(&m_dataMutex);
    return m_categories.value(category);
}

QStringList IconMetadataManager::getIconsByTag(const QString& tag) const
{
    QMutexLocker locker(&m_dataMutex);
    return m_tagToIcons.value(tag);
}

QStringList IconMetadataManager::getFavoriteIcons() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_favoriteIcons;
}

QStringList IconMetadataManager::getRecentlyUsedIcons() const
{
    QMutexLocker locker(&m_dataMutex);
    return m_recentlyUsed;
}

void IconMetadataManager::addToFavorites(const QString& iconName)
{
    QMutexLocker locker(&m_dataMutex);
    if (!m_favoriteIcons.contains(iconName)) {
        m_favoriteIcons.append(iconName);
        if (m_iconMetadata.contains(iconName)) {
            m_iconMetadata[iconName].isFavorite = true;
        }
        emit favoritesChanged();
    }
}

void IconMetadataManager::removeFromFavorites(const QString& iconName)
{
    QMutexLocker locker(&m_dataMutex);
    if (m_favoriteIcons.removeOne(iconName)) {
        if (m_iconMetadata.contains(iconName)) {
            m_iconMetadata[iconName].isFavorite = false;
        }
        emit favoritesChanged();
    }
}

bool IconMetadataManager::isFavorite(const QString& iconName) const
{
    QMutexLocker locker(&m_dataMutex);
    return m_favoriteIcons.contains(iconName);
}

void IconMetadataManager::clearFavorites()
{
    QMutexLocker locker(&m_dataMutex);
    for (const QString& iconName : m_favoriteIcons) {
        if (m_iconMetadata.contains(iconName)) {
            m_iconMetadata[iconName].isFavorite = false;
        }
    }
    m_favoriteIcons.clear();
    emit favoritesChanged();
}

void IconMetadataManager::recordIconUsage(const QString& iconName)
{
    QMutexLocker locker(&m_dataMutex);

    // Update usage count
    m_usageCount[iconName]++;
    if (m_iconMetadata.contains(iconName)) {
        m_iconMetadata[iconName].usageCount = m_usageCount[iconName];
    }

    // Update recently used list
    m_recentlyUsed.removeOne(iconName);
    m_recentlyUsed.prepend(iconName);

    // Limit recently used list size
    while (m_recentlyUsed.size() > MAX_RECENT_ITEMS) {
        m_recentlyUsed.removeLast();
    }

    emit usageDataChanged();
}

int IconMetadataManager::getIconUsageCount(const QString& iconName) const
{
    QMutexLocker locker(&m_dataMutex);
    return m_usageCount.value(iconName, 0);
}

void IconMetadataManager::clearUsageHistory()
{
    QMutexLocker locker(&m_dataMutex);
    m_usageCount.clear();
    m_recentlyUsed.clear();

    for (auto it = m_iconMetadata.begin(); it != m_iconMetadata.end(); ++it) {
        it.value().usageCount = 0;
    }

    emit usageDataChanged();
}

QString IconMetadataManager::getFavoritesFilePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return QDir(dataDir).filePath(FAVORITES_FILE);
}

QString IconMetadataManager::getUsageDataFilePath() const
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    return QDir(dataDir).filePath(USAGE_FILE);
}

bool IconMetadataManager::saveFavorites(const QString& filePath) const
{
    QString path = filePath.isEmpty() ? getFavoritesFilePath() : filePath;

    QJsonObject root;
    QJsonArray favoritesArray;

    QMutexLocker locker(&m_dataMutex);
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

bool IconMetadataManager::loadFavorites(const QString& filePath)
{
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

    QMutexLocker locker(&m_dataMutex);
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

bool IconMetadataManager::saveUsageData(const QString& filePath) const
{
    QString path = filePath.isEmpty() ? getUsageDataFilePath() : filePath;

    QJsonObject root;
    QJsonObject usageObject;
    QJsonArray recentArray;

    QMutexLocker locker(&m_dataMutex);

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

bool IconMetadataManager::loadUsageData(const QString& filePath)
{
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

    QMutexLocker locker(&m_dataMutex);

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

QStringList IconMetadataManager::getSearchSuggestions(const QString& partialText, int maxSuggestions) const
{
    if (partialText.length() < 2) {
        return QStringList();
    }

    QMutexLocker locker(&m_dataMutex);
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

        if (suggestions.size() >= maxSuggestions) {
            break;
        }
    }

    // Remove duplicates and sort
    suggestions.removeDuplicates();
    suggestions.sort();

    return suggestions.mid(0, maxSuggestions);
}

QStringList IconMetadataManager::getCategorySuggestions(const QString& partialText) const
{
    if (partialText.length() < 2) {
        return QStringList();
    }

    QMutexLocker locker(&m_dataMutex);
    QStringList suggestions;
    QString lowerPartial = partialText.toLower();

    for (const QString& category : m_allCategories) {
        if (category.contains(lowerPartial, Qt::CaseInsensitive)) {
            suggestions.append(category);
        }
    }

    return suggestions;
}

QStringList IconMetadataManager::getTagSuggestions(const QString& partialText) const
{
    if (partialText.length() < 2) {
        return QStringList();
    }

    QMutexLocker locker(&m_dataMutex);
    QStringList suggestions;
    QString lowerPartial = partialText.toLower();

    for (const QString& tag : m_allTags) {
        if (tag.contains(lowerPartial, Qt::CaseInsensitive)) {
            suggestions.append(tag);
        }
    }

    return suggestions;
}

void IconMetadataManager::refreshMetadata()
{
    m_isLoaded = false;
    loadMetadata();
}

void IconMetadataManager::applyFilter(const IconFilterCriteria& criteria)
{
    m_currentFilter = criteria;
    QStringList filteredIcons = searchIcons(criteria);
    m_filteredIcons = filteredIcons;
    emit filteredIconsChanged(filteredIcons);
}

QStringList IconMetadataManager::sortIcons(const QStringList& iconNames,
                                         IconFilterCriteria::SortOrder order,
                                         bool ascending) const
{
    QStringList sorted = iconNames;

    switch (order) {
    case IconFilterCriteria::SortByName:
        std::sort(sorted.begin(), sorted.end(), [ascending](const QString& a, const QString& b) {
            return ascending ? a < b : a > b;
        });
        break;

    case IconFilterCriteria::SortByCategory:
        std::sort(sorted.begin(), sorted.end(), [this, ascending](const QString& a, const QString& b) {
            const IconMetadata& metaA = m_iconMetadata.value(a);
            const IconMetadata& metaB = m_iconMetadata.value(b);
            QString catA = metaA.categories.isEmpty() ? "" : metaA.categories.first();
            QString catB = metaB.categories.isEmpty() ? "" : metaB.categories.first();
            return ascending ? catA < catB : catA > catB;
        });
        break;

    case IconFilterCriteria::SortByUsage:
        std::sort(sorted.begin(), sorted.end(), [this, ascending](const QString& a, const QString& b) {
            int usageA = m_usageCount.value(a, 0);
            int usageB = m_usageCount.value(b, 0);
            return ascending ? usageA < usageB : usageA > usageB;
        });
        break;

    case IconFilterCriteria::SortByRecent:
        std::sort(sorted.begin(), sorted.end(), [this, ascending](const QString& a, const QString& b) {
            int indexA = m_recentlyUsed.indexOf(a);
            int indexB = m_recentlyUsed.indexOf(b);
            if (indexA == -1) indexA = INT_MAX;
            if (indexB == -1) indexB = INT_MAX;
            return ascending ? indexA > indexB : indexA < indexB;
        });
        break;
    }

    return sorted;
}
