/**
 * @file IconSearchWidget.h
 * @brief Icon-specific search widget with category and favorites filters
 * @details Extends SearchWidget with additional filtering capabilities
 *          for icon-specific searches, including category dropdown and
 *          favorites-only option.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef ICON_SEARCH_WIDGET_H
#define ICON_SEARCH_WIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <memory>

#include "SearchWidget.h"

class QComboBox;
class QCheckBox;
class QHBoxLayout;

namespace gallery {

/**
 * @class IconSearchWidget
 * @brief Extended search widget with icon-specific filtering
 * @details Combines the SearchWidget with additional filters:
 *          - Category dropdown
 *          - Favorites-only checkbox
 *          - Compact horizontal layout suitable for tool bars
 *
 * @par Usage:
 * @code
 * IconSearchWidget search;
 * search.setCategories({"All", "Accessibility", "Animals", "Arrows"});
 * connect(&search, &IconSearchWidget::searchTextChanged,
 *         this, &MyWidget::onSearch);
 * connect(&search, &IconSearchWidget::categoryFilterChanged,
 *         this, &MyWidget::onCategoryFilterChanged);
 * connect(&search, &IconSearchWidget::favoritesOnlyChanged,
 *         this, &MyWidget::onFavoritesOnlyChanged);
 * @endcode
 *
 * @see SearchWidget
 * @see CategoryFilterWidget
 */
class IconSearchWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct an IconSearchWidget
     * @param parent The parent widget
     */
    explicit IconSearchWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~IconSearchWidget() override;

    /**
     * @brief Set available categories for the filter dropdown
     * @param categories List of category names
     * @details The first category should typically be "All"
     */
    void setCategories(const QStringList& categories);

    /**
     * @brief Get the current search text
     * @return The search query
     */
    [[nodiscard]] QString searchText() const;

    /**
     * @brief Set the search text
     * @param text The search query
     */
    void setSearchText(const QString& text);

    /**
     * @brief Get the selected category filter
     * @return The selected category name
     */
    [[nodiscard]] QString selectedCategory() const;

    /**
     * @brief Set the selected category filter
     * @param category The category to select
     */
    void setSelectedCategory(const QString& category);

    /**
     * @brief Get whether favorites-only filter is active
     * @return true if only favorites are shown
     */
    [[nodiscard]] bool isFavoritesOnly() const;

    /**
     * @brief Set the favorites-only filter state
     * @param favoritesOnly true to show only favorites
     */
    void setFavoritesOnly(bool favoritesOnly);

    /**
     * @brief Clear all search fields and filters
     */
    void clear();

    /**
     * @brief Focus the search input
     */
    void focusSearchInput();

Q_SIGNALS:
    /**
     * @brief Emitted when search text changes
     * @param text The new search query
     * @see SearchWidget::searchTextChanged
     */
    void searchTextChanged(const QString& text);

    /**
     * @brief Emitted when category filter changes
     * @param category The selected category
     */
    void categoryFilterChanged(const QString& category);

    /**
     * @brief Emitted when favorites-only filter changes
     * @param favoritesOnly true if showing only favorites
     */
    void favoritesOnlyChanged(bool favoritesOnly);

private slots:
    /**
     * @brief Called when category selection changes
     * @param index The new combo box index
     */
    void onCategoryChanged(int index);

    /**
     * @brief Called when favorites checkbox state changes
     * @param checked The new checkbox state
     */
    void onFavoritesToggled(bool checked);

    /**
     * @brief Forward search text changes from SearchWidget
     * @param text The search text
     */
    void onSearchTextChanged(const QString& text);

private:
    /**
     * @brief Initialize the UI layout
     */
    void setupUI();

    // UI Components
    std::unique_ptr<SearchWidget> m_searchWidget;   ///< Main search input
    QComboBox* m_categoryComboBox;                  ///< Category filter dropdown
    QCheckBox* m_favoritesCheckBox;                 ///< Favorites-only checkbox
};

} // namespace gallery

#endif // ICON_SEARCH_WIDGET_H
