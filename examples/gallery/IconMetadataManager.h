/**
 * QtLucide Gallery Application - Icon Metadata Manager Header
 * 
 * Manages loading, parsing, and providing access to icon metadata including
 * categories, tags, and search functionality.
 */

#ifndef ICONMETADATAMANAGER_H
#define ICONMETADATAMANAGER_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QMutex>
#include <QFuture>
#include <QFutureWatcher>

/**
 * @brief Structure representing metadata for a single icon
 */
struct IconMetadata {
    QString name;
    QString svgFile;
    QStringList tags;
    QStringList categories;
    QStringList contributors;
    
    // Additional computed fields
    QString displayName;
    QString searchText; // Pre-computed search string for performance
    bool isFavorite = false;
    int usageCount = 0;
    
    IconMetadata() = default;
    IconMetadata(const QString& iconName) : name(iconName) {}
    
    bool isValid() const { return !name.isEmpty(); }
    
    // Helper methods
    QString getDisplayName() const {
        return displayName.isEmpty() ? name : displayName;
    }
    
    bool matchesSearch(const QString& searchTerm) const {
        return searchText.contains(searchTerm, Qt::CaseInsensitive);
    }
    
    bool hasCategory(const QString& category) const {
        return categories.contains(category, Qt::CaseInsensitive);
    }
    
    bool hasTag(const QString& tag) const {
        return tags.contains(tag, Qt::CaseInsensitive);
    }
};

/**
 * @brief Filter criteria for icon searching and filtering
 */
struct IconFilterCriteria {
    QString searchText;
    QStringList categories;
    QStringList tags;
    bool favoritesOnly = false;
    bool recentlyUsedOnly = false;
    
    enum SortOrder {
        SortByName,
        SortByCategory,
        SortByUsage,
        SortByRecent
    };
    SortOrder sortOrder = SortByName;
    bool sortAscending = true;
    
    bool isEmpty() const {
        return searchText.isEmpty() && categories.isEmpty() && 
               tags.isEmpty() && !favoritesOnly && !recentlyUsedOnly;
    }
    
    void clear() {
        searchText.clear();
        categories.clear();
        tags.clear();
        favoritesOnly = false;
        recentlyUsedOnly = false;
    }
};

/**
 * @brief Main class for managing icon metadata and search functionality
 */
class IconMetadataManager : public QObject
{
    Q_OBJECT

public:
    explicit IconMetadataManager(QObject *parent = nullptr);
    ~IconMetadataManager();
    
    // Initialization
    bool loadMetadata();
    bool isLoaded() const { return m_isLoaded; }
    
    // Basic metadata access
    QStringList getAllIconNames() const;
    QStringList getAllCategories() const;
    QStringList getAllTags() const;
    IconMetadata getIconMetadata(const QString& iconName) const;
    
    // Search and filtering
    QStringList searchIcons(const IconFilterCriteria& criteria) const;
    QStringList getIconsByCategory(const QString& category) const;
    QStringList getIconsByTag(const QString& tag) const;
    QStringList getFavoriteIcons() const;
    QStringList getRecentlyUsedIcons() const;
    
    // Statistics
    int getTotalIconCount() const { return m_iconMetadata.size(); }
    int getCategoryCount() const { return m_categories.size(); }
    int getTagCount() const { return m_allTags.size(); }
    
    // Favorites management
    void addToFavorites(const QString& iconName);
    void removeFromFavorites(const QString& iconName);
    bool isFavorite(const QString& iconName) const;
    void clearFavorites();
    
    // Usage tracking
    void recordIconUsage(const QString& iconName);
    int getIconUsageCount(const QString& iconName) const;
    void clearUsageHistory();
    
    // Data persistence
    bool saveFavorites(const QString& filePath = QString()) const;
    bool loadFavorites(const QString& filePath = QString());
    bool saveUsageData(const QString& filePath = QString()) const;
    bool loadUsageData(const QString& filePath = QString());
    
    // Search suggestions
    QStringList getSearchSuggestions(const QString& partialText, int maxSuggestions = 10) const;
    QStringList getCategorySuggestions(const QString& partialText) const;
    QStringList getTagSuggestions(const QString& partialText) const;

public slots:
    void refreshMetadata();
    void applyFilter(const IconFilterCriteria& criteria);

signals:
    void metadataLoaded(int totalIcons);
    void metadataLoadFailed(const QString& error);
    void filteredIconsChanged(const QStringList& iconNames);
    void favoritesChanged();
    void usageDataChanged();

private slots:
    void onMetadataLoadFinished();

private:
    // Loading methods
    bool loadIconsMetadata();
    bool loadCategoriesMetadata();
    bool loadTagsMetadata();
    void buildSearchIndex();
    void computeDisplayNames();
    
    // Helper methods
    QString createSearchText(const IconMetadata& metadata) const;
    QString createDisplayName(const QString& iconName) const;
    QStringList sortIcons(const QStringList& iconNames, 
                         IconFilterCriteria::SortOrder order, 
                         bool ascending) const;
    
    // File I/O helpers
    QJsonDocument loadJsonFile(const QString& resourcePath) const;
    QString getFavoritesFilePath() const;
    QString getUsageDataFilePath() const;
    
    // Data members
    QHash<QString, IconMetadata> m_iconMetadata;
    QHash<QString, QStringList> m_categories; // category -> icon names
    QHash<QString, QStringList> m_tagToIcons; // tag -> icon names
    QStringList m_allCategories;
    QStringList m_allTags;
    QStringList m_favoriteIcons;
    QHash<QString, int> m_usageCount;
    QStringList m_recentlyUsed;
    
    // State
    bool m_isLoaded;
    IconFilterCriteria m_currentFilter;
    QStringList m_filteredIcons;
    
    // Threading
    QFutureWatcher<bool>* m_loadWatcher;
    mutable QMutex m_dataMutex;
    
    // Configuration
    static constexpr int MAX_RECENT_ITEMS = 50;
    static constexpr const char* FAVORITES_FILE = "favorites.json";
    static constexpr const char* USAGE_FILE = "usage.json";
};

#endif // ICONMETADATAMANAGER_H
