/**
 * @file SearchController.cpp
 * @brief Implementation of SearchController
 * @details Manages debounced search with highlighting support.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "SearchController.h"
#include "ContentManager.h"
#include "IconMetadataManager.h"

namespace gallery {

SearchController::SearchController(ContentManager *contentManager, QObject *parent)
    : QObject(parent)
    , m_contentManager(contentManager)
    , m_searchTimer(std::make_unique<QTimer>(this))
{
    m_searchTimer->setInterval(m_searchDelay);
    m_searchTimer->setSingleShot(true);

    connect(m_searchTimer.get(), &QTimer::timeout, this, &SearchController::performSearch);
}

SearchController::~SearchController() = default;

void SearchController::setSearchDelay(int delayMs)
{
    m_searchDelay = std::max(0, delayMs);
    m_searchTimer->setInterval(m_searchDelay);
}

void SearchController::setCaseSensitive(bool caseSensitive)
{
    m_caseSensitive = caseSensitive;
}

void SearchController::search(const QString &searchText)
{
    m_pendingSearchText = searchText;
    emit searchTextChanged(searchText);

    // Stop any pending search
    m_searchTimer->stop();

    // Schedule new search with debounce delay
    if (!searchText.isEmpty()) {
        m_searchTimer->start();
    } else {
        // Clear search immediately if empty
        clear();
    }
}

QStringList SearchController::searchImmediate(const QString &searchText)
{
    if (!m_contentManager) {
        return QStringList();
    }

    m_currentSearchText = searchText;
    m_lastResults.clear();

    if (searchText.isEmpty()) {
        return QStringList();
    }

    // Search by name
    QStringList nameMatches = matchIconNames(searchText,
        m_contentManager->iconMetadata()->getCategories());

    // Search by tags
    QStringList tagMatches = matchIconTags(searchText);

    // Combine results and remove duplicates
    m_lastResults = nameMatches + tagMatches;
    m_lastResults.removeDuplicates();

    return m_lastResults;
}

void SearchController::clear()
{
    m_currentSearchText.clear();
    m_pendingSearchText.clear();
    m_lastResults.clear();
    m_searchTimer->stop();
    emit searchCleared();
}

QPair<int, QList<int>> SearchController::getHighlightInfo(const QString &iconName) const
{
    if (m_currentSearchText.isEmpty()) {
        return {0, QList<int>()};
    }

    QList<int> positions;

    // Check if match is in icon name
    QString searchText = m_caseSensitive ? m_currentSearchText : m_currentSearchText.toLower();
    QString checkName = m_caseSensitive ? iconName : iconName.toLower();

    int pos = checkName.indexOf(searchText);
    if (pos >= 0) {
        positions.append(pos);
        return {0, positions};  // 0 = name match
    }

    // Check if match is in tags
    if (m_contentManager) {
        auto metadata = m_contentManager->iconMetadata()->getIconMetadata(iconName);
        for (const QString &tag : metadata.tags) {
            QString checkTag = m_caseSensitive ? tag : tag.toLower();
            if (checkTag.contains(searchText)) {
                return {1, positions};  // 1 = tag match
            }
        }
    }

    return {-1, positions};  // No match
}

void SearchController::performSearch()
{
    if (!m_contentManager) {
        return;
    }

    m_currentSearchText = m_pendingSearchText;
    m_lastResults = searchImmediate(m_pendingSearchText);

    emit searchResultsReady(m_lastResults, m_currentSearchText);
}

QStringList SearchController::matchIconNames(const QString &searchText,
                                           const QStringList &iconNames) const
{
    QStringList results;

    QString pattern = m_caseSensitive ? searchText : searchText.toLower();

    for (const QString &iconName : iconNames) {
        QString checkName = m_caseSensitive ? iconName : iconName.toLower();
        if (checkName.contains(pattern)) {
            results.append(iconName);
        }
    }

    return results;
}

QStringList SearchController::matchIconTags(const QString &searchText) const
{
    if (!m_contentManager) {
        return QStringList();
    }

    QStringList results;
    QString pattern = m_caseSensitive ? searchText : searchText.toLower();

    // Get all icons from the metadata manager
    auto categories = m_contentManager->iconMetadata()->getCategories();

    for (const QString &category : categories) {
        auto categoryIcons = m_contentManager->iconMetadata()->getIconsByCategory(category);

        for (const QString &iconName : categoryIcons) {
            auto tags = m_contentManager->iconMetadata()->getIconTags(iconName);

            for (const QString &tag : tags) {
                QString checkTag = m_caseSensitive ? tag : tag.toLower();
                if (checkTag.contains(pattern)) {
                    if (!results.contains(iconName)) {
                        results.append(iconName);
                    }
                    break;  // Only add icon once
                }
            }
        }
    }

    return results;
}

}  // namespace gallery
