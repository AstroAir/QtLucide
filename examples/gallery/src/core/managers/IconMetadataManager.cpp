/**
 * @file IconMetadataManager.cpp
 * @brief Implementation of IconMetadataManager
 */

#include "IconMetadataManager.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <algorithm>

namespace gallery {

IconMetadataManager::IconMetadataManager(QObject* parent) : QObject(parent) {}

bool IconMetadataManager::loadMetadata(const QString& categoriesPath, const QString& iconsPath) {
    // Load categories
    QJsonDocument categoriesDoc = loadJsonFile(categoriesPath);
    if (categoriesDoc.isEmpty()) {
        qWarning() << "Failed to load categories from:" << categoriesPath;
        return false;
    }

    if (!parseCategories(categoriesDoc)) {
        qWarning() << "Failed to parse categories";
        return false;
    }

    // Load icons
    QJsonDocument iconsDoc = loadJsonFile(iconsPath);
    if (iconsDoc.isEmpty()) {
        qWarning() << "Failed to load icons from:" << iconsPath;
        return false;
    }

    if (!parseIcons(iconsDoc)) {
        qWarning() << "Failed to parse icons";
        return false;
    }

    qInfo() << "Successfully loaded" << m_icons.size() << "icons from" << m_categories.size()
            << "categories";
    return true;
}

QJsonDocument IconMetadataManager::loadJsonFile(const QString& path) const {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << path;
        return QJsonDocument();
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error in" << path << ":" << parseError.errorString();
        return QJsonDocument();
    }

    return doc;
}

bool IconMetadataManager::parseCategories(const QJsonDocument& doc) {
    if (!doc.isObject()) {
        qWarning() << "Categories document is not a JSON object";
        return false;
    }

    QJsonObject root = doc.object();

    for (auto it = root.begin(); it != root.end(); ++it) {
        QString categoryName = it.key();
        QJsonValue value = it.value();

        if (!value.isArray()) {
            qWarning() << "Category" << categoryName << "is not an array";
            continue;
        }

        QStringList icons;
        QJsonArray iconArray = value.toArray();
        for (const QJsonValue& iconValue : iconArray) {
            if (iconValue.isString()) {
                icons.append(iconValue.toString());
            }
        }

        m_categories[categoryName] = icons;
    }

    return true;
}

bool IconMetadataManager::parseIcons(const QJsonDocument& doc) {
    if (!doc.isObject()) {
        qWarning() << "Icons document is not a JSON object";
        return false;
    }

    QJsonObject root = doc.object();

    // The structure is { "icons": { "icon-name": { ... }, ... } }
    if (!root.contains("icons")) {
        qWarning() << "Icons document missing 'icons' key";
        return false;
    }

    QJsonValue iconsValue = root.value("icons");
    if (!iconsValue.isObject()) {
        qWarning() << "Icons value is not a JSON object";
        return false;
    }

    QJsonObject iconsObj = iconsValue.toObject();

    for (auto it = iconsObj.begin(); it != iconsObj.end(); ++it) {
        QString iconName = it.key();
        QJsonValue value = it.value();

        if (!value.isObject()) {
            qWarning() << "Icon" << iconName << "is not a JSON object";
            continue;
        }

        QJsonObject iconObj = value.toObject();

        IconMetadata metadata;
        metadata.name = iconObj.value("name").toString(iconName);
        metadata.svgFile = iconObj.value("svg_file").toString();

        // Parse tags
        QJsonValue tagsValue = iconObj.value("tags");
        if (tagsValue.isArray()) {
            QJsonArray tagsArray = tagsValue.toArray();
            for (const QJsonValue& tagValue : tagsArray) {
                if (tagValue.isString()) {
                    metadata.tags.append(tagValue.toString());
                }
            }
        }

        // Parse categories
        QJsonValue categoriesValue = iconObj.value("categories");
        if (categoriesValue.isArray()) {
            QJsonArray categoriesArray = categoriesValue.toArray();
            for (const QJsonValue& catValue : categoriesArray) {
                if (catValue.isString()) {
                    metadata.categories.append(catValue.toString());
                }
            }
        }

        // Parse contributors
        QJsonValue contributorsValue = iconObj.value("contributors");
        if (contributorsValue.isArray()) {
            QJsonArray contributorsArray = contributorsValue.toArray();
            for (const QJsonValue& contribValue : contributorsArray) {
                if (contribValue.isString()) {
                    metadata.contributors.append(contribValue.toString());
                }
            }
        }

        m_icons[iconName] = metadata;
    }

    return true;
}

QStringList IconMetadataManager::getCategories() const {
    return m_categories.keys();
}

QStringList IconMetadataManager::getIconsByCategory(const QString& category) const {
    auto it = m_categories.find(category);
    if (it != m_categories.end()) {
        return it.value();
    }
    return QStringList();
}

QStringList IconMetadataManager::getIconTags(const QString& iconName) const {
    auto it = m_icons.find(iconName);
    if (it != m_icons.end()) {
        return it.value().tags;
    }
    return QStringList();
}

int IconMetadataManager::getCategoryIconCount(const QString& category) const {
    auto it = m_categories.find(category);
    if (it != m_categories.end()) {
        return static_cast<int>(it.value().size());
    }
    return 0;
}

IconMetadata IconMetadataManager::getIconMetadata(const QString& iconName) const {
    auto it = m_icons.find(iconName);
    if (it != m_icons.end()) {
        return it.value();
    }
    return IconMetadata();
}

bool IconMetadataManager::iconExists(const QString& iconName) const {
    return m_icons.contains(iconName);
}

int IconMetadataManager::getTotalIconCount() const {
    return static_cast<int>(m_icons.size());
}

QStringList IconMetadataManager::searchIcons(const QString& searchText) const {
    QStringList results;

    if (searchText.isEmpty()) {
        return m_icons.keys();
    }

    QString lowerSearchText = searchText.toLower();

    for (auto it = m_icons.begin(); it != m_icons.end(); ++it) {
        const IconMetadata& metadata = it.value();

        // Check if search text matches icon name
        if (metadata.name.contains(lowerSearchText, Qt::CaseInsensitive)) {
            results.append(metadata.name);
            continue;
        }

        // Check if search text matches any tag
        bool foundInTags = false;
        for (const QString& tag : metadata.tags) {
            if (tag.contains(lowerSearchText, Qt::CaseInsensitive)) {
                foundInTags = true;
                break;
            }
        }

        if (foundInTags) {
            results.append(metadata.name);
        }
    }

    return results;
}

} // namespace gallery
