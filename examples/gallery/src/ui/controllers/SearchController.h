/**
 * QtLucide Gallery Application - Search Controller
 *
 * Manages all search and filtering functionality for the gallery application.
 * Provides centralized search logic, filter management, and search history.
 *
 * Features:
 * - Text-based icon search with fuzzy matching
 * - Category and tag filtering
 * - Favorites and recent icons filtering
 * - Search history management
 * - Saved filter profiles
 * - Advanced search options
 */

#ifndef SEARCHCONTROLLER_H
#define SEARCHCONTROLLER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QVariantMap>

// Forward declarations
class SearchWidget;
class IconSearchWidget;
class CategoryFilterWidget;
class IconMetadataManager;
class FavoritesManager;

/**
 * @brief Controller for search and filtering operations
 *
 * This controller manages all search-related functionality, including:
 * - Search query processing
 * - Filter application and management
 * - Search history tracking
 * - Saved filter profiles
 */
class SearchController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a new Search Controller
     * @param metadataManager Icon metadata manager for search operations
     * @param favoritesManager Favorites manager for favorites filtering
     * @param settings Application settings for persistence
     * @param parent Parent QObject for memory management
     */
    explicit SearchController(IconMetadataManager* metadataManager,
                              FavoritesManager* favoritesManager, QSettings* settings,
                              QObject* parent = nullptr);
    ~SearchController();

    // Widget management
    void setSearchWidget(SearchWidget* widget);
    void setIconSearchWidget(IconSearchWidget* widget);
    void setCategoryFilterWidget(CategoryFilterWidget* widget);

    // Search operations
    void performSearch(const QString& query);
    void clearSearch();
    QStringList getFilteredIcons() const;
    QString getCurrentSearchText() const;

    // Filter management
    void applyCategoryFilter(const QStringList& categories);
    void applyTagFilter(const QStringList& tags);
    void applyContributorFilter(const QStringList& contributors);
    void setFavoritesFilterEnabled(bool enabled);
    void setRecentFilterEnabled(bool enabled);
    void clearAllFilters();

    // Filter profiles
    bool saveCurrentFilter(const QString& name);
    bool loadSavedFilter(const QString& name);
    QStringList getSavedFilterNames() const;
    bool deleteSavedFilter(const QString& name);

    // Search history
    void addToSearchHistory(const QString& query);
    QStringList getSearchHistory() const;
    void clearSearchHistory();

    // Advanced search
    void showAdvancedSearch();
    void setFuzzySearchEnabled(bool enabled);
    void setSearchInDescriptions(bool enabled);

signals:
    /**
     * @brief Emitted when search results change
     * @param filteredIcons List of icon names matching current filters
     * @param totalIcons Total number of available icons
     */
    void searchResultsChanged(const QStringList& filteredIcons, int totalIcons);

    /**
     * @brief Emitted when filter state changes
     * @param activeFilters Map of active filter types and their values
     */
    void filtersChanged(const QVariantMap& activeFilters);

    /**
     * @brief Emitted when search history is updated
     * @param history List of recent search queries
     */
    void searchHistoryUpdated(const QStringList& history);

    /**
     * @brief Emitted to request status bar update
     * @param message Status message to display
     */
    void statusMessageRequested(const QString& message);

public slots:
    // Slots for widget signals
    void onSearchTextChanged(const QString& text);
    void onSearchClicked();
    void onClearSearchClicked();
    void onCategoryFilterChanged(const QStringList& categories);
    void onTagFilterChanged(const QStringList& tags);
    void onContributorFilterChanged(const QStringList& contributors);
    void onFavoritesFilterToggled(bool enabled);
    void onRecentFilterToggled(bool enabled);
    void onClearAllFiltersClicked();
    void onSaveCurrentFilterClicked();
    void onLoadSavedFilterClicked();
    void onSearchHistorySelected(const QString& query);
    void onAdvancedSearchRequested();

private:
    // Search implementation
    void executeSearch();
    QStringList fuzzySearch(const QString& query, const QStringList& items);
    bool matchesSearchQuery(const QString& iconName, const QString& query);
    void updateSearchResults();

    // Filter implementation
    void applyActiveFilters();
    QStringList applyFilters(const QStringList& icons);
    bool passesFilters(const QString& iconName);

    // Persistence
    void loadSearchHistory();
    void saveSearchHistory();
    void loadSavedFilters();
    void saveSavedFilters();

    // Helper methods
    void connectWidgetSignals();
    void updateWidgetStates();
    QVariantMap getActiveFiltersMap() const;

    // Dependencies
    IconMetadataManager* m_metadataManager;
    FavoritesManager* m_favoritesManager;
    QSettings* m_settings;

    // Widgets (not owned)
    SearchWidget* m_searchWidget;
    IconSearchWidget* m_iconSearchWidget;
    CategoryFilterWidget* m_categoryFilterWidget;

    // Search state
    QString m_currentSearchText;
    QStringList m_filteredIcons;
    QStringList m_allIcons;

    // Filter state
    QStringList m_activeCategories;
    QStringList m_activeTags;
    QStringList m_activeContributors;
    bool m_favoritesFilterEnabled;
    bool m_recentFilterEnabled;

    // Search options
    bool m_fuzzySearchEnabled;
    bool m_searchInDescriptions;

    // History and saved filters
    QStringList m_searchHistory;
    QMap<QString, QVariantMap> m_savedFilters;

    // Constants
    static constexpr int MAX_SEARCH_HISTORY = 20;
    static constexpr const char* SEARCH_HISTORY_KEY = "SearchHistory";
    static constexpr const char* SAVED_FILTERS_KEY = "SavedFilters";
};

#endif // SEARCHCONTROLLER_H
