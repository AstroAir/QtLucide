/**
 * @file CategoryFilterWidget.h
 * @brief Category filter widget with favorites toggle
 * @details Provides filtering controls for category selection and
 *          favorites-only mode, independent of search functionality.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef CATEGORY_FILTER_WIDGET_H
#define CATEGORY_FILTER_WIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>

class QComboBox;
class QCheckBox;
class QListWidget;
class QListWidgetItem;
class QVBoxLayout;
class QHBoxLayout;

namespace gallery {

/**
 * @enum FilterMode
 * @brief Enumeration of available filter modes
 */
enum class FilterMode {
    Dropdown,   ///< Use a dropdown/combo box for category selection
    List        ///< Use a list widget for category selection
};

/**
 * @struct CategoryFilterSettings
 * @brief Current category filter settings
 */
struct CategoryFilterSettings {
    /**
     * @brief Selected category name
     * @default Empty string (all categories)
     */
    QString selectedCategory;

    /**
     * @brief Whether to show only favorites
     * @default false
     */
    bool favoritesOnly{false};

    /**
     * @brief Equality comparison operator
     * @param other The other settings to compare with
     * @return true if settings are equal
     */
    bool operator==(const CategoryFilterSettings& other) const noexcept {
        return selectedCategory == other.selectedCategory &&
               favoritesOnly == other.favoritesOnly;
    }

    /**
     * @brief Inequality comparison operator
     * @param other The other settings to compare with
     * @return true if settings differ
     */
    bool operator!=(const CategoryFilterSettings& other) const noexcept {
        return !(*this == other);
    }
};

/**
 * @class CategoryFilterWidget
 * @brief Widget for filtering icons by category and favorites
 * @details Provides independent filtering controls with two layout modes:
 *          - Dropdown mode: Compact dropdown for category selection
 *          - List mode: Expanded list of categories for selection
 *          Both modes include a favorites-only checkbox.
 *
 * @par Usage:
 * @code
 * CategoryFilterWidget filter;
 * filter.setCategories({"All", "Accessibility", "Animals"});
 * filter.setMode(FilterMode::Dropdown);
 * connect(&filter, &CategoryFilterWidget::categoryFilterChanged,
 *         this, &MyWidget::onFilterChanged);
 * @endcode
 *
 * @see IconSearchWidget
 */
class CategoryFilterWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct a CategoryFilterWidget
     * @param mode The filter mode (Dropdown or List)
     * @param parent The parent widget
     */
    explicit CategoryFilterWidget(FilterMode mode = FilterMode::Dropdown, QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~CategoryFilterWidget() override;

    /**
     * @brief Set the filter mode
     * @param mode The mode to use
     * @details Switches between dropdown and list views
     */
    void setMode(FilterMode mode);

    /**
     * @brief Get the current filter mode
     * @return The current FilterMode
     */
    [[nodiscard]] FilterMode mode() const;

    /**
     * @brief Set available categories
     * @param categories List of category names
     */
    void setCategories(const QStringList& categories);

    /**
     * @brief Get available categories
     * @return List of all categories
     */
    [[nodiscard]] QStringList categories() const;

    /**
     * @brief Set the selected category
     * @param category The category to select (empty for all)
     */
    void setSelectedCategory(const QString& category);

    /**
     * @brief Get the selected category
     * @return The selected category name
     */
    [[nodiscard]] QString selectedCategory() const;

    /**
     * @brief Set the favorites-only filter state
     * @param favoritesOnly true to show only favorites
     */
    void setFavoritesOnly(bool favoritesOnly);

    /**
     * @brief Get the favorites-only filter state
     * @return true if showing only favorites
     */
    [[nodiscard]] bool isFavoritesOnly() const;

    /**
     * @brief Get current filter settings
     * @return The current CategoryFilterSettings
     */
    [[nodiscard]] CategoryFilterSettings filterSettings() const;

    /**
     * @brief Set filter settings
     * @param settings The settings to apply
     */
    void setFilterSettings(const CategoryFilterSettings& settings);

    /**
     * @brief Clear all filters
     */
    void clear();

Q_SIGNALS:
    /**
     * @brief Emitted when category filter changes
     * @param category The selected category name
     */
    void categoryFilterChanged(const QString& category);

    /**
     * @brief Emitted when favorites-only filter changes
     * @param favoritesOnly true if showing only favorites
     */
    void favoritesOnlyChanged(bool favoritesOnly);

    /**
     * @brief Emitted when any filter setting changes
     * @param settings The new CategoryFilterSettings
     */
    void filterChanged(const CategoryFilterSettings& settings);

private slots:
    /**
     * @brief Called when category dropdown selection changes
     * @param index The new index
     */
    void onCategoryChanged(int index);

    /**
     * @brief Called when category list item is selected
     * @param item The selected list item
     */
    void onCategoryListItemSelected(QListWidgetItem* item);

    /**
     * @brief Called when favorites checkbox is toggled
     * @param checked The new state
     */
    void onFavoritesToggled(bool checked);

private:
    /**
     * @brief Initialize the UI with the current mode
     */
    void setupUI();

    /**
     * @brief Create the dropdown-based layout
     * @return The layout widget
     */
    QWidget* createDropdownLayout();

    /**
     * @brief Create the list-based layout
     * @return The layout widget
     */
    QWidget* createListLayout();

    // State
    FilterMode m_mode;                           ///< Current filter mode
    QStringList m_categories;                    ///< Available categories
    QString m_selectedCategory;                  ///< Currently selected category
    bool m_favoritesOnly;                        ///< Favorites-only flag

    // UI Components - Shared
    QCheckBox* m_favoritesCheckBox;              ///< Favorites-only checkbox

    // UI Components - Dropdown mode
    QComboBox* m_categoryComboBox;               ///< Category selection dropdown

    // UI Components - List mode
    QListWidget* m_categoryListWidget;           ///< Category selection list

    // Main container
    QVBoxLayout* m_mainLayout;                   ///< Main vertical layout
};

} // namespace gallery

#endif // CATEGORY_FILTER_WIDGET_H
