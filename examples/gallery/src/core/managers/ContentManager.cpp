/**
 * @file ContentManager.cpp
 * @brief Implementation of ContentManager
 */

#include "ContentManager.h"

#include <QDebug>
#include <algorithm>

namespace gallery {

ContentManager::ContentManager(QObject *parent)
    : QObject(parent),
      m_iconMetadata(std::make_unique<IconMetadataManager>(this)),
      m_favorites(std::make_unique<FavoritesManager>(this)),
      m_showFavoritesOnly(false) {
}

ContentManager::~ContentManager() = default;

bool ContentManager::initialize(const QString &categoriesPath, const QString &iconsPath) {
    if (!m_iconMetadata->loadMetadata(categoriesPath, iconsPath)) {
        qWarning() << "Failed to initialize ContentManager: unable to load metadata";
        return false;
    }

    qInfo() << "ContentManager initialized successfully with"
            << m_iconMetadata->getTotalIconCount() << "icons";
    return true;
}

IconMetadataManager *ContentManager::iconMetadata() const {
    return m_iconMetadata.get();
}

FavoritesManager *ContentManager::favorites() const {
    return m_favorites.get();
}

void ContentManager::setSearchText(const QString &searchText) {
    if (m_searchText != searchText) {
        m_searchText = searchText;
        emit searchTextChanged(searchText);
        updateFilters();
    }
}

QString ContentManager::getSearchText() const {
    return m_searchText;
}

void ContentManager::setCategory(const QString &category) {
    if (m_currentCategory != category) {
        m_currentCategory = category;
        emit categoryChanged(category);
        updateFilters();
    }
}

QString ContentManager::getCategory() const {
    return m_currentCategory;
}

void ContentManager::setShowFavoritesOnly(bool showFavoritesOnly) {
    if (m_showFavoritesOnly != showFavoritesOnly) {
        m_showFavoritesOnly = showFavoritesOnly;
        emit favoritesOnlyChanged(showFavoritesOnly);
        updateFilters();
    }
}

bool ContentManager::getShowFavoritesOnly() const {
    return m_showFavoritesOnly;
}

void ContentManager::selectIcon(const QString &iconName) {
    if (m_selectedIcon != iconName) {
        m_selectedIcon = iconName;
        emit iconSelected(iconName);
    }
}

QString ContentManager::getSelectedIcon() const {
    return m_selectedIcon;
}

QStringList ContentManager::applyFilters() const {
    QStringList result;

    // Start with all icons or search results
    if (!m_searchText.isEmpty()) {
        result = m_iconMetadata->searchIcons(m_searchText);
    } else if (!m_currentCategory.isEmpty()) {
        result = m_iconMetadata->getIconsByCategory(m_currentCategory);
    } else {
        // Get all icons
        for (const QString &category : m_iconMetadata->getCategories()) {
            result.append(m_iconMetadata->getIconsByCategory(category));
        }
    }

    // Filter by favorites if enabled
    if (m_showFavoritesOnly) {
        QStringList favorites = m_favorites->favorites();
        result.erase(std::remove_if(result.begin(), result.end(),
                                     [&favorites](const QString &icon) {
                                         return !favorites.contains(icon);
                                     }),
                     result.end());
    }

    return result;
}

QStringList ContentManager::getFilteredIcons() const {
    return applyFilters();
}

void ContentManager::updateFilters() {
    QStringList filteredIcons = applyFilters();
    emit iconFilterChanged(filteredIcons);
}

void ContentManager::resetFilters() {
    m_searchText.clear();
    m_currentCategory.clear();
    m_showFavoritesOnly = false;
    m_selectedIcon.clear();
    updateFilters();
}

}  // namespace gallery
