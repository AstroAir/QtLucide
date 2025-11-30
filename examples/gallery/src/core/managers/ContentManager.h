/**
 * @file ContentManager.h
 * @brief Central manager for content coordination
 * @details This file contains the ContentManager class which coordinates icon
 *          metadata, favorites, and provides filtered icon lists.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef CONTENT_MANAGER_H
#define CONTENT_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

#include "FavoritesManager.h"
#include "IconMetadataManager.h"

namespace gallery {

/**
 * @class ContentManager
 * @brief Central coordinator for icon metadata and favorites management
 * @details This class serves as a central hub that owns and coordinates
 *          IconMetadataManager and FavoritesManager, providing high-level
 *          filtering and search capabilities.
 */
class ContentManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct ContentManager
     * @param parent The parent QObject
     */
    explicit ContentManager(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ContentManager() override;

    /**
     * @brief Initialize the content manager with metadata files
     * @param categoriesPath Path to categories.json file
     * @param iconsPath Path to icons.json file
     * @return true if initialization was successful, false otherwise
     */
    bool initialize(const QString& categoriesPath, const QString& iconsPath);

    /**
     * @brief Get the icon metadata manager
     * @return Pointer to IconMetadataManager instance
     */
    IconMetadataManager* iconMetadata() const;

    /**
     * @brief Get the favorites manager
     * @return Pointer to FavoritesManager instance
     */
    FavoritesManager* favorites() const;

    /**
     * @brief Get filtered icons based on current search and category
     * @return List of filtered icon names
     */
    QStringList getFilteredIcons() const;

    /**
     * @brief Set the current search text for filtering
     * @param searchText The search text to filter by
     */
    void setSearchText(const QString& searchText);

    /**
     * @brief Get the current search text
     * @return The current search text
     */
    QString getSearchText() const;

    /**
     * @brief Set the current category filter
     * @param category The category to filter by, empty string for all categories
     */
    void setCategory(const QString& category);

    /**
     * @brief Get the current category filter
     * @return The current category filter, or empty string if showing all
     */
    QString getCategory() const;

    /**
     * @brief Set whether to show only favorite icons
     * @param showFavoritesOnly true to show only favorites, false to show all
     */
    void setShowFavoritesOnly(bool showFavoritesOnly);

    /**
     * @brief Get whether only favorites are being shown
     * @return true if showing only favorites, false otherwise
     */
    bool getShowFavoritesOnly() const;

    /**
     * @brief Set the selected icon
     * @param iconName The name of the selected icon
     */
    void selectIcon(const QString& iconName);

    /**
     * @brief Get the currently selected icon
     * @return The name of the selected icon, or empty string if none
     */
    QString getSelectedIcon() const;

    /**
     * @brief Apply all current filters and return filtered icons
     * @return List of icons matching current filters
     */
    QStringList applyFilters() const;

    /**
     * @brief Reset all filters to default state
     */
    void resetFilters();

signals:
    /**
     * @brief Signal emitted when icon filtering changes
     * @param icons The new list of filtered icons
     */
    void iconFilterChanged(const QStringList& icons);

    /**
     * @brief Signal emitted when an icon is selected
     * @param iconName The name of the selected icon
     */
    void iconSelected(const QString& iconName);

    /**
     * @brief Signal emitted when search text changes
     * @param searchText The new search text
     */
    void searchTextChanged(const QString& searchText);

    /**
     * @brief Signal emitted when category filter changes
     * @param category The new category filter
     */
    void categoryChanged(const QString& category);

    /**
     * @brief Signal emitted when favorites-only filter changes
     * @param showFavoritesOnly The new state
     */
    void favoritesOnlyChanged(bool showFavoritesOnly);

private:
    /**
     * @brief Update filters and emit iconFilterChanged signal
     */
    void updateFilters();

    // Member variables
    std::unique_ptr<IconMetadataManager> m_iconMetadata;
    std::unique_ptr<FavoritesManager> m_favorites;

    QString m_searchText;
    QString m_currentCategory;
    bool m_showFavoritesOnly;
    QString m_selectedIcon;
};

} // namespace gallery

#endif // CONTENT_MANAGER_H
