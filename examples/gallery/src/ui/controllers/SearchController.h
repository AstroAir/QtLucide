/**
 * @file SearchController.h
 * @brief Search functionality controller with debouncing
 * @details This file contains the SearchController class which handles debounced
 *          search input, performs icon name and tag matching, and manages search results.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef SEARCH_CONTROLLER_H
#define SEARCH_CONTROLLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <memory>

namespace gallery {

// Forward declaration
class ContentManager;

/**
 * @class SearchController
 * @brief Manages search functionality with debouncing
 * @details This class provides debounced search capabilities across icon names and tags.
 *          It uses a timer to delay search execution (300ms by default) to avoid
 *          excessive filtering while the user is still typing.
 *
 * @par Features:
 * - Debounced search with configurable delay
 * - Search across icon names and tags
 * - Case-sensitive and case-insensitive modes
 * - Search highlighting information
 * - Performance optimized with minimal CPU usage during typing
 *
 * @par Usage:
 * @code
 * SearchController searchCtrl(contentManager);
 * searchCtrl.setSearchDelay(300);
 *
 * connect(&searchCtrl, &SearchController::searchResultsReady,
 *         this, &MyWidget::updateSearchResults);
 *
 * // Called as user types
 * searchCtrl.search("lucide");
 *
 * // Get results
 * QStringList results = searchCtrl.getLastResults();
 * @endcode
 */
class SearchController : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct SearchController
     * @param contentManager The content manager to search within
     * @param parent The parent QObject
     */
    explicit SearchController(ContentManager* contentManager, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~SearchController() override;

    /**
     * @brief Set the debounce delay in milliseconds
     * @param delayMs The delay duration (default: 300ms)
     */
    void setSearchDelay(int delayMs);

    /**
     * @brief Get the current debounce delay
     * @return Delay in milliseconds
     */
    [[nodiscard]] int getSearchDelay() const { return m_searchDelay; }

    /**
     * @brief Set case sensitivity for search
     * @param caseSensitive true for case-sensitive, false for case-insensitive
     */
    void setCaseSensitive(bool caseSensitive);

    /**
     * @brief Get case sensitivity setting
     * @return true if case-sensitive, false otherwise
     */
    [[nodiscard]] bool isCaseSensitive() const { return m_caseSensitive; }

    /**
     * @brief Perform a search with debouncing
     * @param searchText The text to search for
     *
     * This method is debounced - the actual search will be delayed by the
     * configured delay period. Calling this multiple times will reset the timer.
     */
    void search(const QString& searchText);

    /**
     * @brief Perform immediate search without debouncing
     * @param searchText The text to search for
     * @return List of matching icon names
     *
     * This method performs the search immediately without any delay.
     */
    QStringList searchImmediate(const QString& searchText);

    /**
     * @brief Clear the search and reset to all icons
     */
    void clear();

    /**
     * @brief Get the last search results
     * @return List of icon names matching the last search
     */
    [[nodiscard]] QStringList getLastResults() const { return m_lastResults; }

    /**
     * @brief Get the current search text
     * @return Current search query
     */
    [[nodiscard]] QString getCurrentSearchText() const { return m_currentSearchText; }

    /**
     * @brief Check if search results contain matches
     * @return true if there are matching results, false if no matches
     */
    [[nodiscard]] bool hasResults() const { return !m_lastResults.isEmpty(); }

    /**
     * @brief Get the count of search results
     * @return Number of icons in the last results
     */
    [[nodiscard]] int getResultCount() const { return static_cast<int>(m_lastResults.size()); }

    /**
     * @brief Get highlight information for a search result
     * @param iconName The icon name to get highlight info for
     * @return Pair of (matchType, highlightPositions) where matchType is
     *         0 for name match, 1 for tag match
     *
     * This can be used by UI to highlight matching portions of icon names/tags.
     */
    [[nodiscard]] QPair<int, QList<int>> getHighlightInfo(const QString& iconName) const;

signals:
    /**
     * @brief Emitted when search results are ready
     * @param results List of icon names matching the search criteria
     * @param searchText The search text that produced these results
     *
     * This signal is emitted after the debounce delay when a new search is performed.
     */
    void searchResultsReady(const QStringList& results, const QString& searchText);

    /**
     * @brief Emitted when search text changes
     * @param searchText The new search text
     */
    void searchTextChanged(const QString& searchText);

    /**
     * @brief Emitted when search is cleared
     */
    void searchCleared();

private:
    /**
     * @brief Perform the actual search after debounce delay
     */
    Q_SLOT void performSearch();

    /**
     * @brief Perform case-sensitive icon name matching
     * @param searchText The search text
     * @param iconNames The icon names to search through
     * @return Matching icon names
     */
    [[nodiscard]] QStringList matchIconNames(const QString& searchText,
                                             const QStringList& iconNames) const;

    /**
     * @brief Perform case-sensitive tag matching
     * @param searchText The search text
     * @return Icon names with matching tags
     */
    [[nodiscard]] QStringList matchIconTags(const QString& searchText) const;

    // Member variables
    ContentManager* m_contentManager = nullptr; ///< Content manager reference
    std::unique_ptr<QTimer> m_searchTimer;      ///< Debounce timer
    QString m_currentSearchText;                ///< Current search query
    QString m_pendingSearchText;                ///< Pending search query
    QStringList m_lastResults;                  ///< Last search results
    int m_searchDelay = 300;                    ///< Debounce delay in ms
    bool m_caseSensitive = false;               ///< Case sensitivity flag
};

} // namespace gallery

#endif // SEARCH_CONTROLLER_H
