/**
 * QtLucide Gallery Application - Search Controller Implementation
 */

#include "SearchController.h"
#include "../../core/managers/FavoritesManager.h"
#include "../../core/managers/IconMetadataManager.h"
#include "../widgets/search/CategoryFilterWidget.h"
#include "../widgets/search/IconSearchWidget.h"
#include "../widgets/search/SearchWidget.h"
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

SearchController::SearchController(IconMetadataManager* metadataManager,
                                   FavoritesManager* favoritesManager, QSettings* settings,
                                   QObject* parent)
    : QObject(parent), m_metadataManager(metadataManager), m_favoritesManager(favoritesManager),
      m_settings(settings), m_searchWidget(nullptr), m_iconSearchWidget(nullptr),
      m_categoryFilterWidget(nullptr), m_favoritesFilterEnabled(false),
      m_recentFilterEnabled(false), m_fuzzySearchEnabled(true), m_searchInDescriptions(false) {
    // Load persisted data
    loadSearchHistory();
    loadSavedFilters();

    // Initialize all icons list from metadata manager
    if (m_metadataManager) {
        m_allIcons = m_metadataManager->getAllIconNames();
        m_filteredIcons = m_allIcons;
    }
}

SearchController::~SearchController() {
    // Save state before destruction
    saveSearchHistory();
    saveSavedFilters();
}

// Widget management
void SearchController::setSearchWidget(SearchWidget* widget) {
    m_searchWidget = widget;
    connectWidgetSignals();
}

void SearchController::setIconSearchWidget(IconSearchWidget* widget) {
    m_iconSearchWidget = widget;
    connectWidgetSignals();
}

void SearchController::setCategoryFilterWidget(CategoryFilterWidget* widget) {
    m_categoryFilterWidget = widget;
    connectWidgetSignals();
}

// Search operations
void SearchController::performSearch(const QString& query) {
    m_currentSearchText = query;

    if (!query.isEmpty()) {
        addToSearchHistory(query);
    }

    executeSearch();
}

void SearchController::clearSearch() {
    m_currentSearchText.clear();
    m_filteredIcons = m_allIcons;

    updateSearchResults();
    emit statusMessageRequested(tr("Search cleared"));
}

QStringList SearchController::getFilteredIcons() const {
    return m_filteredIcons;
}

QString SearchController::getCurrentSearchText() const {
    return m_currentSearchText;
}

// Filter management
void SearchController::applyCategoryFilter(const QStringList& categories) {
    m_activeCategories = categories;
    applyActiveFilters();
}

void SearchController::applyTagFilter(const QStringList& tags) {
    m_activeTags = tags;
    applyActiveFilters();
}

void SearchController::applyContributorFilter(const QStringList& contributors) {
    m_activeContributors = contributors;
    applyActiveFilters();
}

void SearchController::setFavoritesFilterEnabled(bool enabled) {
    m_favoritesFilterEnabled = enabled;
    applyActiveFilters();

    emit statusMessageRequested(enabled ? tr("Showing favorites only") : tr("Showing all icons"));
}

void SearchController::setRecentFilterEnabled(bool enabled) {
    m_recentFilterEnabled = enabled;
    applyActiveFilters();

    emit statusMessageRequested(enabled ? tr("Showing recent icons") : tr("Showing all icons"));
}

void SearchController::clearAllFilters() {
    m_currentSearchText.clear();
    m_activeCategories.clear();
    m_activeTags.clear();
    m_activeContributors.clear();
    m_favoritesFilterEnabled = false;
    m_recentFilterEnabled = false;

    m_filteredIcons = m_allIcons;

    updateWidgetStates();
    updateSearchResults();

    emit statusMessageRequested(tr("All filters cleared"));
}

// Filter profiles
bool SearchController::saveCurrentFilter(const QString& name) {
    if (name.isEmpty()) {
        return false;
    }

    QVariantMap filterData;
    filterData["searchText"] = m_currentSearchText;
    filterData["categories"] = m_activeCategories;
    filterData["tags"] = m_activeTags;
    filterData["contributors"] = m_activeContributors;
    filterData["favoritesOnly"] = m_favoritesFilterEnabled;
    filterData["recentOnly"] = m_recentFilterEnabled;

    m_savedFilters[name] = filterData;
    saveSavedFilters();

    emit statusMessageRequested(tr("Filter '%1' saved").arg(name));
    return true;
}

bool SearchController::loadSavedFilter(const QString& name) {
    if (!m_savedFilters.contains(name)) {
        return false;
    }

    QVariantMap filterData = m_savedFilters[name];

    m_currentSearchText = filterData.value("searchText").toString();
    m_activeCategories = filterData.value("categories").toStringList();
    m_activeTags = filterData.value("tags").toStringList();
    m_activeContributors = filterData.value("contributors").toStringList();
    m_favoritesFilterEnabled = filterData.value("favoritesOnly").toBool();
    m_recentFilterEnabled = filterData.value("recentOnly").toBool();

    updateWidgetStates();
    applyActiveFilters();

    emit statusMessageRequested(tr("Filter '%1' loaded").arg(name));
    return true;
}

QStringList SearchController::getSavedFilterNames() const {
    return m_savedFilters.keys();
}

bool SearchController::deleteSavedFilter(const QString& name) {
    if (m_savedFilters.remove(name) > 0) {
        saveSavedFilters();
        emit statusMessageRequested(tr("Filter '%1' deleted").arg(name));
        return true;
    }
    return false;
}

// Search history
void SearchController::addToSearchHistory(const QString& query) {
    if (query.isEmpty()) {
        return;
    }

    // Remove if already exists (to move to front)
    m_searchHistory.removeAll(query);

    // Add to front
    m_searchHistory.prepend(query);

    // Limit history size
    while (m_searchHistory.size() > MAX_SEARCH_HISTORY) {
        m_searchHistory.removeLast();
    }

    saveSearchHistory();
    emit searchHistoryUpdated(m_searchHistory);
}

QStringList SearchController::getSearchHistory() const {
    return m_searchHistory;
}

void SearchController::clearSearchHistory() {
    m_searchHistory.clear();
    saveSearchHistory();
    emit searchHistoryUpdated(m_searchHistory);
}

// Advanced search
void SearchController::showAdvancedSearch() {
    if (m_searchWidget) {
        m_searchWidget->showAdvancedOptions();
    }
}

void SearchController::setFuzzySearchEnabled(bool enabled) {
    m_fuzzySearchEnabled = enabled;
    if (!m_currentSearchText.isEmpty()) {
        executeSearch();
    }
}

void SearchController::setSearchInDescriptions(bool enabled) {
    m_searchInDescriptions = enabled;
    if (!m_currentSearchText.isEmpty()) {
        executeSearch();
    }
}

// Slots for widget signals
void SearchController::onSearchTextChanged(const QString& text) {
    m_currentSearchText = text;
    executeSearch();
}

void SearchController::onSearchClicked() {
    if (!m_currentSearchText.isEmpty()) {
        addToSearchHistory(m_currentSearchText);
    }
    executeSearch();
}

void SearchController::onClearSearchClicked() {
    clearSearch();
}

void SearchController::onCategoryFilterChanged(const QStringList& categories) {
    applyCategoryFilter(categories);
}

void SearchController::onTagFilterChanged(const QStringList& tags) {
    applyTagFilter(tags);
}

void SearchController::onContributorFilterChanged(const QStringList& contributors) {
    applyContributorFilter(contributors);
}

void SearchController::onFavoritesFilterToggled(bool enabled) {
    setFavoritesFilterEnabled(enabled);
}

void SearchController::onRecentFilterToggled(bool enabled) {
    setRecentFilterEnabled(enabled);
}

void SearchController::onClearAllFiltersClicked() {
    clearAllFilters();
}

void SearchController::onSaveCurrentFilterClicked() {
    bool ok;
    QString name = QInputDialog::getText(nullptr, tr("Save Filter"), tr("Enter filter name:"),
                                         QLineEdit::Normal, QString(), &ok);
    if (ok && !name.isEmpty()) {
        saveCurrentFilter(name);
    }
}

void SearchController::onLoadSavedFilterClicked() {
    QStringList filterNames = getSavedFilterNames();
    if (filterNames.isEmpty()) {
        QMessageBox::information(nullptr, tr("No Saved Filters"),
                                 tr("No saved filters available."));
        return;
    }

    bool ok;
    QString name = QInputDialog::getItem(nullptr, tr("Load Filter"), tr("Select filter to load:"),
                                         filterNames, 0, false, &ok);
    if (ok && !name.isEmpty()) {
        loadSavedFilter(name);
    }
}

void SearchController::onSearchHistorySelected(const QString& query) {
    if (!query.isEmpty()) {
        m_currentSearchText = query;
        updateWidgetStates();
        executeSearch();
    }
}

void SearchController::onAdvancedSearchRequested() {
    showAdvancedSearch();
}

// Private implementation methods

void SearchController::executeSearch() {
    if (m_currentSearchText.isEmpty()) {
        m_filteredIcons = m_allIcons;
    } else {
        if (m_fuzzySearchEnabled) {
            m_filteredIcons = fuzzySearch(m_currentSearchText, m_allIcons);
        } else {
            m_filteredIcons.clear();
            for (const QString& icon : m_allIcons) {
                if (matchesSearchQuery(icon, m_currentSearchText)) {
                    m_filteredIcons.append(icon);
                }
            }
        }
    }

    // Apply additional filters
    m_filteredIcons = applyFilters(m_filteredIcons);

    updateSearchResults();
}

QStringList SearchController::fuzzySearch(const QString& query, const QStringList& items) {
    QStringList results;
    QString lowerQuery = query.toLower();

    for (const QString& item : items) {
        QString lowerItem = item.toLower();

        // Exact match gets highest priority
        if (lowerItem == lowerQuery) {
            results.prepend(item);
            continue;
        }

        // Starts with query
        if (lowerItem.startsWith(lowerQuery)) {
            results.append(item);
            continue;
        }

        // Contains query
        if (lowerItem.contains(lowerQuery)) {
            results.append(item);
            continue;
        }

        // Fuzzy match - check if all characters in query appear in order
        int queryPos = 0;
        for (int i = 0; i < lowerItem.length() && queryPos < lowerQuery.length(); ++i) {
            if (lowerItem[i] == lowerQuery[queryPos]) {
                queryPos++;
            }
        }
        if (queryPos == lowerQuery.length()) {
            results.append(item);
        }
    }

    return results;
}

bool SearchController::matchesSearchQuery(const QString& iconName, const QString& query) {
    QString lowerIcon = iconName.toLower();
    QString lowerQuery = query.toLower();

    // Check icon name
    if (lowerIcon.contains(lowerQuery)) {
        return true;
    }

    // Check description if enabled
    if (m_searchInDescriptions && m_metadataManager) {
        IconMetadata metadata = m_metadataManager->getIconMetadata(iconName);
        if (metadata.description.toLower().contains(lowerQuery)) {
            return true;
        }
    }

    return false;
}

void SearchController::updateSearchResults() {
    emit searchResultsChanged(m_filteredIcons, static_cast<int>(m_allIcons.size()));
    emit filtersChanged(getActiveFiltersMap());

    QString message = tr("Found %1 of %2 icons").arg(m_filteredIcons.size()).arg(m_allIcons.size());
    emit statusMessageRequested(message);
}

void SearchController::applyActiveFilters() {
    executeSearch();
}

QStringList SearchController::applyFilters(const QStringList& icons) {
    QStringList filtered = icons;

    // Apply category filter
    if (!m_activeCategories.isEmpty() && m_metadataManager) {
        QStringList categoryFiltered;
        for (const QString& icon : filtered) {
            IconMetadata metadata = m_metadataManager->getIconMetadata(icon);
            // Check if any of the icon's categories match the active categories
            for (const QString& category : metadata.categories) {
                if (m_activeCategories.contains(category)) {
                    categoryFiltered.append(icon);
                    break;
                }
            }
        }
        filtered = categoryFiltered;
    }

    // Apply tag filter
    if (!m_activeTags.isEmpty() && m_metadataManager) {
        QStringList tagFiltered;
        for (const QString& icon : filtered) {
            IconMetadata metadata = m_metadataManager->getIconMetadata(icon);
            for (const QString& tag : m_activeTags) {
                if (metadata.tags.contains(tag)) {
                    tagFiltered.append(icon);
                    break;
                }
            }
        }
        filtered = tagFiltered;
    }

    // Apply favorites filter
    if (m_favoritesFilterEnabled && m_favoritesManager) {
        QStringList favorites = m_favoritesManager->getFavorites();
        QStringList favoritesFiltered;
        for (const QString& icon : filtered) {
            if (favorites.contains(icon)) {
                favoritesFiltered.append(icon);
            }
        }
        filtered = favoritesFiltered;
    }

    // Apply recent filter
    if (m_recentFilterEnabled && m_metadataManager) {
        QStringList recent = m_metadataManager->getRecentlyUsedIcons(50);
        QStringList recentFiltered;
        for (const QString& icon : filtered) {
            if (recent.contains(icon)) {
                recentFiltered.append(icon);
            }
        }
        filtered = recentFiltered;
    }

    return filtered;
}

bool SearchController::passesFilters(const QString& iconName) {
    // Check category filter
    if (!m_activeCategories.isEmpty() && m_metadataManager) {
        IconMetadata metadata = m_metadataManager->getIconMetadata(iconName);
        bool hasMatchingCategory = false;
        for (const QString& category : metadata.categories) {
            if (m_activeCategories.contains(category)) {
                hasMatchingCategory = true;
                break;
            }
        }
        if (!hasMatchingCategory) {
            return false;
        }
    }

    // Check tag filter
    if (!m_activeTags.isEmpty() && m_metadataManager) {
        IconMetadata metadata = m_metadataManager->getIconMetadata(iconName);
        bool hasMatchingTag = false;
        for (const QString& tag : m_activeTags) {
            if (metadata.tags.contains(tag)) {
                hasMatchingTag = true;
                break;
            }
        }
        if (!hasMatchingTag) {
            return false;
        }
    }

    // Check favorites filter
    if (m_favoritesFilterEnabled && m_favoritesManager) {
        if (!m_favoritesManager->isFavorite(iconName)) {
            return false;
        }
    }

    return true;
}

// Persistence methods

void SearchController::loadSearchHistory() {
    if (!m_settings)
        return;

    m_searchHistory = m_settings->value(SEARCH_HISTORY_KEY).toStringList();
}

void SearchController::saveSearchHistory() {
    if (!m_settings)
        return;

    m_settings->setValue(SEARCH_HISTORY_KEY, m_searchHistory);
}

void SearchController::loadSavedFilters() {
    if (!m_settings)
        return;

    QVariantMap filtersMap = m_settings->value(SAVED_FILTERS_KEY).toMap();
    for (auto it = filtersMap.begin(); it != filtersMap.end(); ++it) {
        m_savedFilters[it.key()] = it.value().toMap();
    }
}

void SearchController::saveSavedFilters() {
    if (!m_settings)
        return;

    QVariantMap filtersMap;
    for (auto it = m_savedFilters.begin(); it != m_savedFilters.end(); ++it) {
        filtersMap[it.key()] = it.value();
    }
    m_settings->setValue(SAVED_FILTERS_KEY, filtersMap);
}

// Helper methods

void SearchController::connectWidgetSignals() {
    // TODO: Connect widget signals when widgets are integrated
    // The exact signal names need to be verified from the widget implementations

    // Example connections (commented out until widget APIs are verified):
    /*
    if (m_searchWidget) {
        connect(m_searchWidget, &SearchWidget::textChanged,
                this, &SearchController::onSearchTextChanged);
    }

    if (m_iconSearchWidget) {
        connect(m_iconSearchWidget, &IconSearchWidget::textChanged,
                this, &SearchController::onSearchTextChanged);
    }

    if (m_categoryFilterWidget) {
        connect(m_categoryFilterWidget, &CategoryFilterWidget::selectionChanged,
                this, &SearchController::onCategoryFilterChanged);
    }
    */
}

void SearchController::updateWidgetStates() {
    // TODO: Update widget states when widgets are integrated
    // The exact method names need to be verified from the widget implementations

    // Example updates (commented out until widget APIs are verified):
    /*
    if (m_searchWidget) {
        m_searchWidget->setText(m_currentSearchText);
    }

    if (m_iconSearchWidget) {
        m_iconSearchWidget->setText(m_currentSearchText);
    }

    if (m_categoryFilterWidget) {
        m_categoryFilterWidget->setCategories(m_activeCategories);
    }
    */
}

QVariantMap SearchController::getActiveFiltersMap() const {
    QVariantMap filters;

    if (!m_currentSearchText.isEmpty()) {
        filters["searchText"] = m_currentSearchText;
    }

    if (!m_activeCategories.isEmpty()) {
        filters["categories"] = m_activeCategories;
    }

    if (!m_activeTags.isEmpty()) {
        filters["tags"] = m_activeTags;
    }

    if (!m_activeContributors.isEmpty()) {
        filters["contributors"] = m_activeContributors;
    }

    if (m_favoritesFilterEnabled) {
        filters["favoritesOnly"] = true;
    }

    if (m_recentFilterEnabled) {
        filters["recentOnly"] = true;
    }

    return filters;
}
