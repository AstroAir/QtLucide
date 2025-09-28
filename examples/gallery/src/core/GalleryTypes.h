/**
 * @file GalleryTypes.h
 * @brief Common type definitions for QtLucide Gallery
 * @details This file contains common data structures and type definitions
 *          used throughout the QtLucide Gallery application.
 */

#ifndef GALLERYTYPES_H
#define GALLERYTYPES_H

#include <QDateTime>
#include <QHash>
#include <QString>
#include <QStringList>
#include <QVariant>

// Forward declarations to avoid circular dependencies
struct IconMetadata;
enum class ExportFormat;

/**
 * @brief Search criteria structure for icon filtering
 */
struct SearchCriteria {
    QString searchText;
    QStringList categories;
    QStringList tags;
    QStringList contributors;
    bool exactMatch = false;
    bool caseSensitive = false;
    bool useRegex = false;
    bool fuzzySearch = true;

    // Default constructor
    SearchCriteria() = default;

    // Copy constructor
    SearchCriteria(const SearchCriteria& other) = default;

    // Assignment operator
    SearchCriteria& operator=(const SearchCriteria& other) = default;

    // Equality operator
    bool operator==(const SearchCriteria& other) const {
        return searchText == other.searchText && categories == other.categories &&
               tags == other.tags && contributors == other.contributors &&
               exactMatch == other.exactMatch && caseSensitive == other.caseSensitive &&
               useRegex == other.useRegex && fuzzySearch == other.fuzzySearch;
    }

    // Clear all criteria
    void clear() {
        searchText.clear();
        categories.clear();
        tags.clear();
        contributors.clear();
        exactMatch = false;
        caseSensitive = false;
        useRegex = false;
        fuzzySearch = true;
    }

    // Check if criteria is empty
    bool isEmpty() const {
        return searchText.isEmpty() && categories.isEmpty() && tags.isEmpty() &&
               contributors.isEmpty();
    }
};

// IconFilterCriteria is defined in IconMetadataManager.h

// Note: IconMetadata is defined in IconMetadataManager.h
// Note: ExportFormat is defined in ExportDialog.h

/**
 * @brief Theme enumeration
 */
enum class GalleryTheme { Light, Dark, Auto };

#endif // GALLERYTYPES_H
