/**
 * @file FavoritesManager.h
 * @brief Manager for handling user favorite icons
 * @details This file contains the FavoritesManager class which manages favorite icons
 *          with persistent storage using QSettings.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef FAVORITES_MANAGER_H
#define FAVORITES_MANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <memory>

namespace gallery {

/**
 * @class FavoritesManager
 * @brief Manager for user favorite icons with persistent storage
 * @details This class manages a list of favorite icons and persists them using
 *          QSettings in the application data location.
 */
class FavoritesManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct FavoritesManager
     * @param parent The parent QObject
     */
    explicit FavoritesManager(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~FavoritesManager() override;

    /**
     * @brief Check if an icon is in favorites
     * @param iconName The icon name to check
     * @return true if icon is in favorites, false otherwise
     */
    bool isFavorite(const QString& iconName) const;

    /**
     * @brief Add an icon to favorites
     * @param iconName The icon name to add
     * @return true if icon was added or already in favorites, false if invalid
     */
    bool addFavorite(const QString& iconName);

    /**
     * @brief Remove an icon from favorites
     * @param iconName The icon name to remove
     * @return true if icon was removed or not in favorites, false if invalid
     */
    bool removeFavorite(const QString& iconName);

    /**
     * @brief Get all favorite icons
     * @return List of all favorite icon names
     */
    QStringList favorites() const;

    /**
     * @brief Get the count of favorite icons
     * @return Number of favorite icons
     */
    int favoritesCount() const;

    /**
     * @brief Clear all favorites
     */
    void clearFavorites();

    /**
     * @brief Toggle favorite status of an icon
     * @param iconName The icon name
     * @return true if icon is now a favorite, false if removed from favorites
     */
    bool toggleFavorite(const QString& iconName);

signals:
    /**
     * @brief Signal emitted when an icon is added to favorites
     * @param iconName The name of the icon added
     */
    void favoriteAdded(const QString& iconName);

    /**
     * @brief Signal emitted when an icon is removed from favorites
     * @param iconName The name of the icon removed
     */
    void favoriteRemoved(const QString& iconName);

    /**
     * @brief Signal emitted when all favorites are cleared
     */
    void favoritesCleared();

private:
    /**
     * @brief Save favorites to persistent storage
     */
    void saveFavorites() const;

    /**
     * @brief Load favorites from persistent storage
     */
    void loadFavorites();

    /**
     * @brief Initialize settings
     */
    void initializeSettings();

    // Member variables
    std::unique_ptr<QSettings> m_settings;
    QStringList m_favorites;
    static constexpr const char* SETTINGS_GROUP = "Favorites";
    static constexpr const char* FAVORITES_KEY = "FavoriteIcons";
};

} // namespace gallery

#endif // FAVORITES_MANAGER_H
