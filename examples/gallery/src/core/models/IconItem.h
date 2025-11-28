/**
 * @file IconItem.h
 * @brief Icon data model class for the QtLucide gallery
 * @details This file defines the IconItem class which represents a single icon
 *          with all its metadata, properties, and sorting capabilities.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef GALLERY_ICONITEM_H
#define GALLERY_ICONITEM_H

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QMetaType>

namespace gallery {

/**
 * @brief Icon data model class
 * @details Represents a single icon with all its metadata, including name,
 *          categories, tags, favorites status, and other properties. This class
 *          is used throughout the gallery for icon management and display.
 * @ingroup Gallery
 *
 * @par Features:
 * - Icon name and description
 * - Multiple categories and tags
 * - Favorites tracking
 * - Usage statistics
 * - Creation and modification timestamps
 * - Comparison operators for sorting
 *
 * @par Usage:
 * @code
 * IconItem item;
 * item.setName("house");
 * item.setCategory("building");
 * item.setTags({"home", "building", "real-estate"});
 * item.setFavorite(true);
 *
 * // Sort items
 * if (item1 < item2) {
 *     qDebug() << "item1 comes before item2";
 * }
 * @endcode
 */
class IconItem {
public:
    /**
     * @brief Default constructor
     * @details Creates an empty IconItem with default values
     */
    IconItem();

    /**
     * @brief Constructor with icon name
     * @param name The icon name (e.g., "house", "alert-circle")
     * @details Creates an IconItem with the specified name
     */
    explicit IconItem(const QString& name);

    /**
     * @brief Constructor with full initialization
     * @param name The icon name
     * @param category The primary category
     * @param tags Associated tags
     * @details Creates and initializes an IconItem with all basic properties
     */
    IconItem(const QString& name, const QString& category, const QStringList& tags);

    /**
     * @brief Destructor
     */
    ~IconItem() = default;

    /**
     * @brief Copy constructor
     * @param other The other IconItem to copy from
     */
    IconItem(const IconItem& other) = default;

    /**
     * @brief Move constructor
     * @param other The other IconItem to move from
     */
    IconItem(IconItem&& other) noexcept = default;

    /**
     * @brief Copy assignment operator
     * @param other The other IconItem to copy from
     * @return Reference to this IconItem
     */
    IconItem& operator=(const IconItem& other) = default;

    /**
     * @brief Move assignment operator
     * @param other The other IconItem to move from
     * @return Reference to this IconItem
     */
    IconItem& operator=(IconItem&& other) noexcept = default;

    // Getters

    /**
     * @brief Get the icon name
     * @return The icon name (e.g., "house", "alert-circle")
     *
     * @example
     * @code
     * QString name = item.name();  // "house"
     * @endcode
     */
    [[nodiscard]] QString name() const;

    /**
     * @brief Get the icon description
     * @return Human-readable description of the icon
     */
    [[nodiscard]] QString description() const;

    /**
     * @brief Get the primary category
     * @return The category name (e.g., "building", "status", "ui")
     *
     * @example
     * @code
     * QString category = item.category();  // "building"
     * @endcode
     */
    [[nodiscard]] QString category() const;

    /**
     * @brief Get all categories this icon belongs to
     * @return List of category names
     *
     * @example
     * @code
     * QStringList categories = item.categories();
     * // ["building", "real-estate"]
     * @endcode
     */
    [[nodiscard]] QStringList categories() const;

    /**
     * @brief Get all tags associated with this icon
     * @return List of tags (e.g., ["home", "building", "real-estate"])
     *
     * @example
     * @code
     * QStringList tags = item.tags();  // ["home", "building", "real-estate"]
     * @endcode
     */
    [[nodiscard]] QStringList tags() const;

    /**
     * @brief Check if this icon is in the favorites list
     * @return true if marked as favorite, false otherwise
     *
     * @example
     * @code
     * if (item.isFavorite()) {
     *     // Show star icon or favorite marker
     * }
     * @endcode
     */
    [[nodiscard]] bool isFavorite() const;

    /**
     * @brief Get the creation timestamp
     * @return QDateTime when the icon was created/added
     */
    [[nodiscard]] QDateTime createdAt() const;

    /**
     * @brief Get the last modification timestamp
     * @return QDateTime when the icon metadata was last modified
     */
    [[nodiscard]] QDateTime modifiedAt() const;

    /**
     * @brief Get the usage count
     * @return Number of times this icon has been used/viewed
     */
    [[nodiscard]] int usageCount() const;

    /**
     * @brief Get custom metadata
     * @param key The metadata key
     * @return The metadata value, or empty string if not found
     *
     * @example
     * @code
     * QString author = item.metadata("author");
     * @endcode
     */
    [[nodiscard]] QString metadata(const QString& key) const;

    // Setters

    /**
     * @brief Set the icon name
     * @param name The new icon name
     *
     * @example
     * @code
     * item.setName("house");
     * @endcode
     */
    void setName(const QString& name);

    /**
     * @brief Set the icon description
     * @param description Human-readable description
     */
    void setDescription(const QString& description);

    /**
     * @brief Set the primary category
     * @param category The category name
     *
     * @example
     * @code
     * item.setCategory("building");
     * @endcode
     */
    void setCategory(const QString& category);

    /**
     * @brief Set all categories this icon belongs to
     * @param categories List of category names
     *
     * @example
     * @code
     * item.setCategories({"building", "real-estate"});
     * @endcode
     */
    void setCategories(const QStringList& categories);

    /**
     * @brief Set tags associated with this icon
     * @param tags List of tags
     *
     * @example
     * @code
     * item.setTags({"home", "building", "structure"});
     * @endcode
     */
    void setTags(const QStringList& tags);

    /**
     * @brief Add a single tag
     * @param tag The tag to add
     * @details Does nothing if the tag already exists
     */
    void addTag(const QString& tag);

    /**
     * @brief Remove a single tag
     * @param tag The tag to remove
     * @details Does nothing if the tag doesn't exist
     */
    void removeTag(const QString& tag);

    /**
     * @brief Check if this icon has a specific tag
     * @param tag The tag to check for
     * @return true if the tag exists, false otherwise
     */
    [[nodiscard]] bool hasTag(const QString& tag) const;

    /**
     * @brief Set the favorite status
     * @param isFavorite true to mark as favorite, false otherwise
     *
     * @example
     * @code
     * item.setFavorite(true);  // Mark as favorite
     * @endcode
     */
    void setFavorite(bool isFavorite);

    /**
     * @brief Increment the usage count
     * @param count The number to increment by (default 1)
     *
     * @example
     * @code
     * item.incrementUsageCount();  // Increment by 1
     * item.incrementUsageCount(5); // Increment by 5
     * @endcode
     */
    void incrementUsageCount(int count = 1);

    /**
     * @brief Reset the usage count to zero
     */
    void resetUsageCount();

    /**
     * @brief Set custom metadata
     * @param key The metadata key
     * @param value The metadata value
     *
     * @example
     * @code
     * item.setMetadata("author", "John Doe");
     * item.setMetadata("license", "MIT");
     * @endcode
     */
    void setMetadata(const QString& key, const QString& value);

    /**
     * @brief Remove custom metadata
     * @param key The metadata key to remove
     */
    void removeMetadata(const QString& key);

    // Comparison operators

    /**
     * @brief Equality comparison operator
     * @param other The other IconItem to compare with
     * @return true if names are equal, false otherwise
     *
     * @note Only compares icon names, not other properties
     *
     * @example
     * @code
     * if (item1 == item2) {
     *     // Same icon
     * }
     * @endcode
     */
    bool operator==(const IconItem& other) const noexcept;

    /**
     * @brief Inequality comparison operator
     * @param other The other IconItem to compare with
     * @return true if names differ, false otherwise
     */
    bool operator!=(const IconItem& other) const noexcept;

    /**
     * @brief Less-than comparison operator for sorting
     * @param other The other IconItem to compare with
     * @return true if this item's name is less than the other's (alphabetically)
     *
     * @note Used for alphabetical sorting
     *
     * @example
     * @code
     * QList<IconItem> items = ...;
     * std::sort(items.begin(), items.end());  // Sorts alphabetically by name
     * @endcode
     */
    bool operator<(const IconItem& other) const noexcept;

    /**
     * @brief Greater-than comparison operator for sorting
     * @param other The other IconItem to compare with
     * @return true if this item's name is greater than the other's (alphabetically)
     */
    bool operator>(const IconItem& other) const noexcept;

    /**
     * @brief Less-than-or-equal comparison operator
     * @param other The other IconItem to compare with
     * @return true if this item's name is less than or equal to the other's
     */
    bool operator<=(const IconItem& other) const noexcept;

    /**
     * @brief Greater-than-or-equal comparison operator
     * @param other The other IconItem to compare with
     * @return true if this item's name is greater than or equal to the other's
     */
    bool operator>=(const IconItem& other) const noexcept;

    /**
     * @brief Check if this icon is valid
     * @return true if the icon has a non-empty name, false otherwise
     *
     * @example
     * @code
     * if (item.isValid()) {
     *     // Icon has required properties
     * }
     * @endcode
     */
    [[nodiscard]] bool isValid() const;

    /**
     * @brief Get a string representation of this icon item
     * @return A formatted string with icon details
     *
     * @example
     * @code
     * qDebug() << item.toString();
     * // "IconItem(name=house, category=building, favorite=true)"
     * @endcode
     */
    [[nodiscard]] QString toString() const;

private:
    QString m_name;                         ///< Icon name
    QString m_description;                  ///< Icon description
    QString m_category;                     ///< Primary category
    QStringList m_categories;               ///< All categories
    QStringList m_tags;                     ///< Associated tags
    bool m_isFavorite{false};               ///< Favorite status
    QDateTime m_createdAt;                  ///< Creation timestamp
    QDateTime m_modifiedAt;                 ///< Modification timestamp
    int m_usageCount{0};                    ///< Usage count
    QHash<QString, QString> m_metadata;     ///< Custom metadata
};

} // namespace gallery

// Register with Qt's meta type system for use in signals/slots and QVariant
Q_DECLARE_METATYPE(gallery::IconItem)

#endif // GALLERY_ICONITEM_H
