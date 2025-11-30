/**
 * @file FavoritesManager.cpp
 * @brief Implementation of FavoritesManager
 */

#include "FavoritesManager.h"

#include <QDebug>
#include <QMetaType>
#include <QStandardPaths>
#include <algorithm>

namespace gallery {

FavoritesManager::FavoritesManager(QObject* parent) : QObject(parent) {
    initializeSettings();
    loadFavorites();
}

FavoritesManager::~FavoritesManager() = default;

void FavoritesManager::initializeSettings() {
    // Create settings with app data location
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString settingsPath = appDataPath + "/QtLucideGallery";
    m_settings = std::make_unique<QSettings>(settingsPath, QSettings::IniFormat, this);
}

void FavoritesManager::loadFavorites() {
    m_settings->beginGroup(SETTINGS_GROUP);
    QVariant favVar = m_settings->value(FAVORITES_KEY, QStringList());
    m_settings->endGroup();

    const int favTypeId = favVar.metaType().id();

    if (favTypeId == QMetaType::QStringList) {
        m_favorites = favVar.toStringList();
    } else if (favTypeId == QMetaType::QVariantList) {
        // Handle legacy format if needed
        QVariantList varList = favVar.toList();
        for (const QVariant& var : varList) {
            if (var.metaType().id() == QMetaType::QString) {
                m_favorites.append(var.toString());
            }
        }
    }

    qInfo() << "Loaded" << m_favorites.size() << "favorite icons";
}

void FavoritesManager::saveFavorites() const {
    m_settings->beginGroup(SETTINGS_GROUP);
    m_settings->setValue(FAVORITES_KEY, m_favorites);
    m_settings->endGroup();
    m_settings->sync();
}

bool FavoritesManager::isFavorite(const QString& iconName) const {
    if (iconName.isEmpty()) {
        return false;
    }
    return m_favorites.contains(iconName);
}

bool FavoritesManager::addFavorite(const QString& iconName) {
    if (iconName.isEmpty()) {
        qWarning() << "Cannot add empty icon name to favorites";
        return false;
    }

    if (isFavorite(iconName)) {
        return true; // Already in favorites
    }

    m_favorites.append(iconName);
    saveFavorites();
    emit favoriteAdded(iconName);

    qDebug() << "Added icon to favorites:" << iconName;
    return true;
}

bool FavoritesManager::removeFavorite(const QString& iconName) {
    if (iconName.isEmpty()) {
        qWarning() << "Cannot remove empty icon name from favorites";
        return false;
    }

    if (!isFavorite(iconName)) {
        return true; // Already not in favorites
    }

    m_favorites.removeAll(iconName);
    saveFavorites();
    emit favoriteRemoved(iconName);

    qDebug() << "Removed icon from favorites:" << iconName;
    return true;
}

QStringList FavoritesManager::favorites() const {
    return m_favorites;
}

int FavoritesManager::favoritesCount() const {
    return static_cast<int>(m_favorites.size());
}

void FavoritesManager::clearFavorites() {
    m_favorites.clear();
    saveFavorites();
    emit favoritesCleared();

    qDebug() << "Cleared all favorites";
}

bool FavoritesManager::toggleFavorite(const QString& iconName) {
    if (isFavorite(iconName)) {
        removeFavorite(iconName);
        return false;
    } else {
        addFavorite(iconName);
        return true;
    }
}

} // namespace gallery
