/**
 * @file IconMetadataManager.h
 * @brief Manager for loading and managing icon metadata
 * @details This file contains the IconMetadataManager class which loads and manages
 *          icon metadata from JSON files (categories.json and icons.json).
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef ICON_METADATA_MANAGER_H
#define ICON_METADATA_MANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace gallery {

/**
 * @struct IconMetadata
 * @brief Structure holding metadata for a single icon
 */
struct IconMetadata {
    QString name;
    QString svgFile;
    QStringList tags;
    QStringList categories;
    QStringList contributors;
};

/**
 * @class IconMetadataManager
 * @brief Manager for loading and accessing icon metadata
 * @details This class loads icon metadata from JSON files and provides convenient
 *          methods to query icon information by category, tags, and names.
 */
class IconMetadataManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct IconMetadataManager
     * @param parent The parent QObject
     */
    explicit IconMetadataManager(QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~IconMetadataManager() override = default;

    /**
     * @brief Load metadata from JSON files
     * @param categoriesPath Path to categories.json file
     * @param iconsPath Path to icons.json file
     * @return true if loading was successful, false otherwise
     */
    bool loadMetadata(const QString &categoriesPath, const QString &iconsPath);

    /**
     * @brief Get all categories
     * @return List of all available categories
     */
    QStringList getCategories() const;

    /**
     * @brief Get icons for a specific category
     * @param category The category name
     * @return List of icon names in the category, empty list if category not found
     */
    QStringList getIconsByCategory(const QString &category) const;

    /**
     * @brief Get tags for a specific icon
     * @param iconName The icon name
     * @return List of tags for the icon, empty list if icon not found
     */
    QStringList getIconTags(const QString &iconName) const;

    /**
     * @brief Get the count of icons in a category
     * @param category The category name
     * @return Number of icons in the category, 0 if category not found
     */
    int getCategoryIconCount(const QString &category) const;

    /**
     * @brief Get metadata for a specific icon
     * @param iconName The icon name
     * @return IconMetadata structure, empty if icon not found
     */
    IconMetadata getIconMetadata(const QString &iconName) const;

    /**
     * @brief Check if an icon exists
     * @param iconName The icon name
     * @return true if icon exists, false otherwise
     */
    bool iconExists(const QString &iconName) const;

    /**
     * @brief Get total number of icons
     * @return Total count of all loaded icons
     */
    int getTotalIconCount() const;

    /**
     * @brief Search icons by name and tags
     * @param searchText The search text to match against icon names and tags
     * @return List of matching icon names
     */
    QStringList searchIcons(const QString &searchText) const;

private:
    /**
     * @brief Parse categories from JSON
     * @param doc The JSON document containing categories
     * @return true if parsing was successful, false otherwise
     */
    bool parseCategories(const QJsonDocument &doc);

    /**
     * @brief Parse icons from JSON
     * @param doc The JSON document containing icons
     * @return true if parsing was successful, false otherwise
     */
    bool parseIcons(const QJsonDocument &doc);

    /**
     * @brief Load JSON file from path
     * @param path The file path
     * @return Parsed QJsonDocument, empty if loading failed
     */
    QJsonDocument loadJsonFile(const QString &path) const;

    // Member variables
    QMap<QString, QStringList> m_categories;  // categoryName -> [iconNames]
    QMap<QString, IconMetadata> m_icons;      // iconName -> metadata
};

}  // namespace gallery

#endif  // ICON_METADATA_MANAGER_H
