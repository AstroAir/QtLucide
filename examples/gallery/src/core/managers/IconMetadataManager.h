/**
 * QtLucide Gallery Application - Enhanced Icon Metadata Manager
 *
 * A high-performance, feature-rich metadata management system for icons with:
 * - Advanced caching and indexing
 * - Real-time search capabilities
 * - Favorites and usage tracking
 * - Category and tag management
 * - Asynchronous loading and processing
 * - Memory-efficient storage
 */

#ifndef ICONMETADATAMANAGER_H
#define ICONMETADATAMANAGER_H

#include <QCache>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QRegularExpression>
#include <QResource>
#include <QRunnable>
#include <QSettings>
#include <QSize>
#include <QStringList>
#include <QThread>
#include <QThreadPool>
#include <QTimer>

#include <memory>
#include <unordered_map>
#include <unordered_set>

// Include for SearchCriteria
#include "../GalleryTypes.h"

// Forward declarations

/**
 * @brief Enhanced structure representing comprehensive metadata for a single icon
 */
struct IconMetadata {
    QString name;
    QString svgFile;
    QString svgContent; // Cached SVG content for performance
    QStringList tags;
    QStringList categories;
    QStringList contributors;
    QStringList aliases; // Alternative names for the icon

    // Enhanced computed fields
    QString displayName;
    QString description;
    QString searchText;     // Pre-computed search string for performance
    QString normalizedName; // Normalized name for sorting

    // User data
    bool isFavorite = false;
    int usageCount = 0;
    QDateTime lastUsed;
    QDateTime dateAdded;

    // Metadata
    QString version;
    QString license;
    QSize originalSize;
    qint64 fileSize = 0;

    // Performance data
    mutable QElapsedTimer loadTimer;
    mutable int renderCount = 0;

    IconMetadata() = default;
    explicit IconMetadata(const QString& iconName) : name(iconName) {
        dateAdded = QDateTime::currentDateTime();
        updateSearchText();
    }

    bool isValid() const { return !name.isEmpty(); }

    // Helper methods
    QString getDisplayName() const {
        return displayName.isEmpty() ? formatName(name) : displayName;
    }

    bool matchesSearch(const QString& searchTerm) const {
        return searchText.contains(searchTerm, Qt::CaseInsensitive);
    }

    bool matchesAdvancedSearch(const QString& searchTerm,
                               const QStringList& categoryFilter = QStringList(),
                               const QStringList& tagFilter = QStringList()) const;

    bool hasCategory(const QString& category) const {
        return categories.contains(category, Qt::CaseInsensitive);
    }

    bool hasTag(const QString& tag) const { return tags.contains(tag, Qt::CaseInsensitive); }

    void updateSearchText() {
        QStringList searchComponents;
        searchComponents << name << displayName << description << tags << categories << aliases;
        searchText = searchComponents.join(' ').toLower();
        normalizedName = name.toLower().replace('-', ' ').replace('_', ' ');
    }

    void incrementUsage() {
        usageCount++;
        lastUsed = QDateTime::currentDateTime();
    }

    // Serialization
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

private:
    static QString formatName(const QString& name) {
        QString formatted = name;
        formatted.replace('-', ' ').replace('_', ' ');
        if (!formatted.isEmpty()) {
            formatted[0] = formatted[0].toUpper();
        }
        return formatted;
    }
};

/**
 * @brief Enhanced filter criteria for advanced icon searching and filtering
 */
struct IconFilterCriteria {
    QString searchText;
    QStringList categories;
    QStringList tags;
    QStringList contributors;
    bool favoritesOnly = false;
    bool recentlyUsedOnly = false;
    bool exactMatch = false;
    bool useRegex = false;

    // Date range filtering
    QDateTime fromDate;
    QDateTime toDate;

    // Usage filtering
    int minUsageCount = 0;
    int maxUsageCount = -1; // -1 means no limit

    enum SortOrder {
        SortByName,
        SortByDisplayName,
        SortByCategory,
        SortByUsage,
        SortByRecent,
        SortByDateAdded,
        SortByFileSize,
        SortByRelevance // For search results
    };
    SortOrder sortOrder = SortByName;
    bool sortAscending = true;

    // Performance options
    int maxResults = -1; // -1 means no limit
    bool enableFuzzySearch = true;

    // Default constructor
    IconFilterCriteria() = default;

    // Conversion constructor from SearchCriteria
    IconFilterCriteria(const SearchCriteria& criteria) {
        searchText = criteria.searchText;
        categories = criteria.categories;
        tags = criteria.tags;
        contributors = criteria.contributors;
        exactMatch = criteria.exactMatch;
        useRegex = criteria.useRegex;
        enableFuzzySearch = criteria.fuzzySearch;
        // Set defaults for fields not in SearchCriteria
        favoritesOnly = false;
        recentlyUsedOnly = false;
        fromDate = QDateTime();
        toDate = QDateTime();
        minUsageCount = 0;
        maxUsageCount = -1;
        sortOrder = SortByName;
        sortAscending = true;
        maxResults = -1;
    }

    // Assignment operator from SearchCriteria
    IconFilterCriteria& operator=(const SearchCriteria& criteria) {
        searchText = criteria.searchText;
        categories = criteria.categories;
        tags = criteria.tags;
        contributors = criteria.contributors;
        exactMatch = criteria.exactMatch;
        useRegex = criteria.useRegex;
        enableFuzzySearch = criteria.fuzzySearch;
        // Keep existing values for fields not in SearchCriteria
        return *this;
    }

    bool isEmpty() const {
        return searchText.isEmpty() && categories.isEmpty() && tags.isEmpty() &&
               contributors.isEmpty() && !favoritesOnly && !recentlyUsedOnly &&
               !fromDate.isValid() && !toDate.isValid() && minUsageCount == 0 &&
               maxUsageCount == -1;
    }

    void clear() {
        searchText.clear();
        categories.clear();
        tags.clear();
        contributors.clear();
        favoritesOnly = false;
        recentlyUsedOnly = false;
        exactMatch = false;
        useRegex = false;
        fromDate = QDateTime();
        toDate = QDateTime();
        minUsageCount = 0;
        maxUsageCount = -1;
        maxResults = -1;
        enableFuzzySearch = true;
    }
};

/**
 * @brief Search result with relevance scoring
 */
struct IconSearchResult {
    QString iconName;
    double relevanceScore = 0.0;
    QStringList matchedFields; // Which fields matched the search

    IconSearchResult() = default;
    IconSearchResult(const QString& name, double score = 0.0)
        : iconName(name), relevanceScore(score) {}

    bool operator<(const IconSearchResult& other) const {
        return relevanceScore > other.relevanceScore; // Higher score first
    }
};

/**
 * @brief Enhanced main class for managing icon metadata with advanced features
 */
class IconMetadataManager : public QObject {
    Q_OBJECT

public:
    explicit IconMetadataManager(QObject* parent = nullptr);
    ~IconMetadataManager();

    // Initialization and lifecycle
    bool initialize();
    bool loadMetadata();
    bool loadMetadataAsync();
    void saveMetadata();
    bool isLoaded() const { return m_isLoaded; }
    bool isLoading() const { return m_isLoading; }

    // Basic metadata access
    QStringList getAllIconNames() const;
    QStringList getAllCategories() const;
    QStringList getAllTags() const;
    QStringList getAllContributors() const;
    IconMetadata getIconMetadata(const QString& iconName) const;
    bool hasIcon(const QString& iconName) const;

    // Enhanced search and filtering
    QStringList searchIcons(const IconFilterCriteria& criteria) const;
    QList<IconSearchResult> searchIconsWithRelevance(const IconFilterCriteria& criteria) const;
    QStringList getIconsByCategory(const QString& category) const;
    QStringList getIconsByTag(const QString& tag) const;
    QStringList getIconsByContributor(const QString& contributor) const;
    QStringList getFavoriteIcons() const;
    QStringList getRecentlyUsedIcons(int limit = 50) const;
    QStringList getMostUsedIcons(int limit = 50) const;

    // Advanced search features
    QStringList findSimilarIcons(const QString& iconName, int maxResults = 10) const;
    QStringList suggestIcons(const QString& partialName) const;
    QStringList getRandomIcons(int count = 10) const;

    // Statistics and analytics
    int getTotalIconCount() const;
    int getCategoryCount() const;
    int getTagCount() const;
    int getContributorCount() const;
    QHash<QString, int> getCategoryStatistics() const;
    QHash<QString, int> getTagStatistics() const;
    QHash<QString, int> getUsageStatistics() const;

    // Favorites management
    void addToFavorites(const QString& iconName);
    void removeFromFavorites(const QString& iconName);
    bool isFavorite(const QString& iconName) const;
    void toggleFavorite(const QString& iconName);
    void clearFavorites();
    int getFavoriteCount() const;

    // Usage tracking and analytics
    void recordIconUsage(const QString& iconName);
    int getIconUsageCount(const QString& iconName) const;
    QDateTime getLastUsedDate(const QString& iconName) const;
    void clearUsageHistory();
    void clearUsageHistory(const QString& iconName);

    // Metadata management
    void updateIconMetadata(const QString& iconName, const IconMetadata& metadata);
    void addCustomTag(const QString& iconName, const QString& tag);
    void removeCustomTag(const QString& iconName, const QString& tag);
    void addCustomCategory(const QString& iconName, const QString& category);

    // Additional methods for gallery functionality
    QStringList getIconTags(const QString& iconName) const;
    void setIconTags(const QString& iconName, const QStringList& tags);
    QString getIconDescription(const QString& iconName) const;
    void setIconDescription(const QString& iconName, const QString& description);
    QStringList getIconCategories(const QString& iconName) const;
    QStringList getIconContributors(const QString& iconName) const;
    void removeCustomCategory(const QString& iconName, const QString& category);

    // Cache management
    void clearCache();
    void preloadIcons(const QStringList& iconNames);
    qint64 getCacheSize() const;
    void setCacheLimit(qint64 limit);

    // Import/Export
    bool exportFavorites(const QString& filePath) const;
    bool importFavorites(const QString& filePath);
    bool exportUsageData(const QString& filePath) const;
    bool importUsageData(const QString& filePath);
    bool exportMetadata(const QString& filePath) const;

    // Data persistence
    bool saveFavorites(const QString& filePath = QString()) const;
    bool loadFavorites(const QString& filePath = QString());
    bool saveUsageData(const QString& filePath = QString()) const;
    bool loadUsageData(const QString& filePath = QString());

    // Search suggestions and auto-completion
    QStringList getSearchSuggestions(const QString& partialText, int maxSuggestions = 10) const;
    QStringList getCategorySuggestions(const QString& partialText) const;
    QStringList getTagSuggestions(const QString& partialText) const;

    // Performance monitoring
    void logPerformanceMetrics() const;
    QVariantMap getPerformanceMetrics() const;

public slots:
    void refreshMetadata();
    void applyFilter(const IconFilterCriteria& criteria);
    void clearFilter();

signals:
    void metadataLoaded(int totalIcons);
    void metadataLoadProgress(int current, int total);
    void metadataLoadFailed(const QString& error);
    void filteredIconsChanged(const QStringList& iconNames);
    void favoritesChanged();
    void usageDataChanged();
    void cacheUpdated();
    void performanceMetricsUpdated(const QVariantMap& metrics);

private slots:
    void onMetadataLoadFinished();
    void onAsyncLoadProgress(int current, int total);

private:
    // Enhanced loading methods
    bool loadIconsMetadata();
    bool loadCategoriesMetadata();
    bool loadTagsMetadata();
    bool loadContributorsMetadata();
    void buildSearchIndex();
    void buildFuzzySearchIndex();
    void computeDisplayNames();
    void validateMetadata();

    // Advanced search helpers
    QList<IconSearchResult> performFuzzySearch(const QString& searchTerm,
                                               int maxResults = -1) const;
    QList<IconSearchResult> performExactSearch(const QString& searchTerm,
                                               int maxResults = -1) const;
    QList<IconSearchResult> performRegexSearch(const QString& pattern, int maxResults = -1) const;
    double calculateRelevanceScore(const IconMetadata& metadata, const QString& searchTerm) const;
    bool matchesFilter(const IconMetadata& metadata, const IconFilterCriteria& criteria) const;

    // Sorting and filtering helpers
    QString createSearchText(const IconMetadata& metadata) const;
    QString createDisplayName(const QString& iconName) const;
    QStringList sortIcons(const QStringList& iconNames, IconFilterCriteria::SortOrder order,
                          bool ascending) const;
    QStringList applyUsageFilter(const QStringList& iconNames,
                                 const IconFilterCriteria& criteria) const;
    QStringList applyDateFilter(const QStringList& iconNames,
                                const IconFilterCriteria& criteria) const;

    // File I/O helpers
    QJsonDocument loadJsonFile(const QString& resourcePath) const;
    bool saveJsonFile(const QString& filePath, const QJsonDocument& document) const;
    QString getFavoritesFilePath() const;
    QString getUsageDataFilePath() const;
    QString getMetadataCacheFilePath() const;

    // Fallback methods for missing resources
    void createFallbackIconMetadata();
    void createFallbackCategoryMetadata();
    void createFallbackTagMetadata();

    // Cache management helpers
    void updateCache();
    void invalidateCache();
    bool loadFromCache();
    bool saveToCache() const;

    // Performance optimization
    void optimizeDataStructures();
    void precomputeSearchData();
    void updateStatistics();

    // Enhanced data members
    QHash<QString, IconMetadata> m_iconMetadata;
    QHash<QString, QStringList> m_categories;         // category -> icon names
    QHash<QString, QStringList> m_tagToIcons;         // tag -> icon names
    QHash<QString, QStringList> m_contributorToIcons; // contributor -> icon names

    // Cached lists for performance
    QStringList m_allIconNames;
    QStringList m_allCategories;
    QStringList m_allTags;
    QStringList m_allContributors;

    // User data
    QStringList m_favoriteIcons;
    QHash<QString, int> m_usageCount;
    QHash<QString, QDateTime> m_lastUsed;
    QStringList m_recentlyUsed;

    // Search and filtering
    IconFilterCriteria m_currentFilter;
    QStringList m_filteredIcons;

    // State management
    bool m_isLoaded;
    bool m_isLoading;
    bool m_cacheValid;
    QDateTime m_lastMetadataUpdate;

    // Threading and async operations
    QFutureWatcher<bool>* m_loadWatcher;
    QThreadPool* m_threadPool;
    mutable QReadWriteLock m_dataLock; // For thread-safe access
    mutable QMutex m_cacheMutex;

    // Performance monitoring
    mutable QElapsedTimer m_performanceTimer;
    mutable QHash<QString, qint64> m_operationTimes;
    mutable int m_searchCount;
    mutable int m_cacheHits;
    mutable int m_cacheMisses;

    // Configuration
    QSettings* m_settings;
    qint64 m_cacheLimit;
    bool m_enableFuzzySearch;
    bool m_enablePerformanceMonitoring;

    // Constants
    static constexpr int MAX_RECENT_ITEMS = 100;
    static constexpr int MAX_SEARCH_RESULTS = 1000;
    static constexpr int DEFAULT_CACHE_LIMIT = 50 * 1024 * 1024; // 50MB
    static constexpr const char* FAVORITES_FILE = "favorites.json";
    static constexpr const char* USAGE_FILE = "usage.json";
    static constexpr const char* METADATA_CACHE_FILE = "metadata_cache.json";
    static constexpr const char* SETTINGS_GROUP = "IconMetadataManager";
};

#endif // ICONMETADATAMANAGER_H
