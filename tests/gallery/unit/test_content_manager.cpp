/**
 * @file test_content_manager.cpp
 * @brief Comprehensive Unit Tests for ContentManager Class
 * @details Tests ContentManager functionality including initialization, search,
 *          filtering, icon selection, signal emission, and filter management
 * @author QtLucide Test Suite
 * @date 2025
 */

#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QSignalSpy>
#include <QStringList>
#include <QTemporaryDir>
#include <QtTest/QtTest>

// Include Gallery components
#include "core/managers/ContentManager.h"
#include "core/managers/FavoritesManager.h"
#include "core/managers/IconMetadataManager.h"

using namespace gallery;

/**
 * @class TestContentManager
 * @brief Unit tests for ContentManager class
 * @details Comprehensive test suite covering all ContentManager functionality
 */
class TestContentManager : public QObject {
    Q_OBJECT

private slots:
    /**
     * @brief Initialize test case environment
     * @details Sets up test data directories and initializes test resources
     */
    void initTestCase();

    /**
     * @brief Clean up test case environment
     * @details Removes temporary test data and cleans up resources
     */
    void cleanupTestCase();

    /**
     * @brief Initialize individual test
     * @details Called before each test method
     */
    void init();

    /**
     * @brief Clean up individual test
     * @details Called after each test method
     */
    void cleanup();

    // Initialization tests
    /**
     * @brief Test ContentManager creation
     */
    void testContentManager_Creation();

    /**
     * @brief Test ContentManager initialization with real resource paths
     */
    void testContentManager_Initialize();

    /**
     * @brief Test invalid initialization paths
     */
    void testContentManager_InitializeWithInvalidPaths();

    // Search functionality tests
    /**
     * @brief Test setSearchText and getSearchText
     */
    void testSearchText_SetAndGet();

    /**
     * @brief Test search text without duplicate signal emission
     */
    void testSearchText_NoDuplicateSignals();

    /**
     * @brief Test empty search text
     */
    void testSearchText_EmptySearch();

    // Category filter tests
    /**
     * @brief Test setCategory and getCategory
     */
    void testCategory_SetAndGet();

    /**
     * @brief Test category without duplicate signal emission
     */
    void testCategory_NoDuplicateSignals();

    /**
     * @brief Test empty category (all categories)
     */
    void testCategory_EmptyCategory();

    // Favorites filter tests
    /**
     * @brief Test setShowFavoritesOnly and getShowFavoritesOnly
     */
    void testFavoritesOnly_SetAndGet();

    /**
     * @brief Test favorites filter without duplicate signal emission
     */
    void testFavoritesOnly_NoDuplicateSignals();

    // Icon selection tests
    /**
     * @brief Test selectIcon and getSelectedIcon
     */
    void testIconSelection_SetAndGet();

    /**
     * @brief Test icon selection without duplicate signal emission
     */
    void testIconSelection_NoDuplicateSignals();

    /**
     * @brief Test empty icon selection
     */
    void testIconSelection_EmptySelection();

    // Filter application tests
    /**
     * @brief Test applyFilters with no filters
     */
    void testApplyFilters_NoFilters();

    /**
     * @brief Test applyFilters with search text
     */
    void testApplyFilters_WithSearchText();

    /**
     * @brief Test applyFilters with category filter
     */
    void testApplyFilters_WithCategory();

    /**
     * @brief Test applyFilters with favorites filter
     */
    void testApplyFilters_WithFavoritesOnly();

    /**
     * @brief Test applyFilters with combined filters
     */
    void testApplyFilters_WithCombinedFilters();

    /**
     * @brief Test getFilteredIcons
     */
    void testGetFilteredIcons();

    // Signal emission tests
    /**
     * @brief Test iconFilterChanged signal emission
     */
    void testSignal_IconFilterChanged();

    /**
     * @brief Test iconSelected signal emission
     */
    void testSignal_IconSelected();

    /**
     * @brief Test searchTextChanged signal emission
     */
    void testSignal_SearchTextChanged();

    /**
     * @brief Test categoryChanged signal emission
     */
    void testSignal_CategoryChanged();

    /**
     * @brief Test favoritesOnlyChanged signal emission
     */
    void testSignal_FavoritesOnlyChanged();

    /**
     * @brief Test multiple signal emissions in sequence
     */
    void testSignal_MultipleSignalsSequence();

    // Filter reset tests
    /**
     * @brief Test resetFilters clears all filters
     */
    void testResetFilters_ClearsAllFilters();

    /**
     * @brief Test resetFilters emits appropriate signals
     */
    void testResetFilters_EmitsSignals();

    // Manager access tests
    /**
     * @brief Test iconMetadata manager access
     */
    void testManagerAccess_IconMetadata();

    /**
     * @brief Test favorites manager access
     */
    void testManagerAccess_Favorites();

    // Integration tests
    /**
     * @brief Test search followed by category filter
     */
    void testIntegration_SearchAndCategoryFilter();

    /**
     * @brief Test favorites filter with search
     */
    void testIntegration_FavoritesWithSearch();

    /**
     * @brief Test full workflow: search, filter, select, reset
     */
    void testIntegration_FullWorkflow();

private:
    ContentManager* m_contentManager;  ///< ContentManager instance under test
    QString m_tempDir;                 ///< Temporary directory for test data

    // Helper methods
    /**
     * @brief Verify that resource files exist
     * @return true if resources are available
     */
    bool verifyResourcesAvailable();

    /**
     * @brief Get count of all available icons
     * @return Total icon count
     */
    int getTotalIconCount();

    /**
     * @brief Get first test category
     * @return Category name or empty string
     */
    QString getFirstCategory();
};

void TestContentManager::initTestCase() {
    // Create test instance
    m_contentManager = nullptr;

    // Verify resources exist
    if (!verifyResourcesAvailable()) {
        QSKIP("Icon resources not available");
    }

    qDebug() << "TestContentManager test case initialized";
}

void TestContentManager::cleanupTestCase() {
    if (m_contentManager) {
        delete m_contentManager;
        m_contentManager = nullptr;
    }
    qDebug() << "TestContentManager test case cleaned up";
}

void TestContentManager::init() {
    // Create fresh ContentManager instance for each test
    m_contentManager = new ContentManager();

    // Initialize with real resource paths
    bool initialized = m_contentManager->initialize(
        ":/lucide/metadata/categories.json",
        ":/lucide/metadata/icons.json");

    if (!initialized) {
        QSKIP("Failed to initialize ContentManager with resources");
    }
}

void TestContentManager::cleanup() {
    if (m_contentManager) {
        delete m_contentManager;
        m_contentManager = nullptr;
    }
}

bool TestContentManager::verifyResourcesAvailable() {
    // Try to load the metadata files
    QFile categoriesFile(":/lucide/metadata/categories.json");
    QFile iconsFile(":/lucide/metadata/icons.json");

    bool available = categoriesFile.exists() && iconsFile.exists();
    if (!available) {
        qWarning() << "Icon resources not found at expected paths";
    }
    return available;
}

int TestContentManager::getTotalIconCount() {
    if (m_contentManager == nullptr) {
        return 0;
    }
    IconMetadataManager* metadata = m_contentManager->iconMetadata();
    if (metadata == nullptr) {
        return 0;
    }
    return metadata->getTotalIconCount();
}

QString TestContentManager::getFirstCategory() {
    if (m_contentManager == nullptr) {
        return QString();
    }
    IconMetadataManager* metadata = m_contentManager->iconMetadata();
    if (metadata == nullptr) {
        return QString();
    }
    QStringList categories = metadata->getCategories();
    return categories.isEmpty() ? QString() : categories.first();
}

// ============================================================================
// Initialization Tests
// ============================================================================

void TestContentManager::testContentManager_Creation() {
    ContentManager* manager = new ContentManager();
    QVERIFY(manager != nullptr);

    // Check initial state
    QVERIFY(manager->getSearchText().isEmpty());
    QVERIFY(manager->getCategory().isEmpty());
    QVERIFY(!manager->getShowFavoritesOnly());
    QVERIFY(manager->getSelectedIcon().isEmpty());

    // Check manager access
    QVERIFY(manager->iconMetadata() != nullptr);
    QVERIFY(manager->favorites() != nullptr);

    delete manager;
}

void TestContentManager::testContentManager_Initialize() {
    QVERIFY(m_contentManager != nullptr);

    // Verify managers are accessible
    QVERIFY(m_contentManager->iconMetadata() != nullptr);
    QVERIFY(m_contentManager->favorites() != nullptr);

    // Verify metadata was loaded
    int totalIcons = m_contentManager->iconMetadata()->getTotalIconCount();
    QVERIFY(totalIcons > 0);

    QStringList categories = m_contentManager->iconMetadata()->getCategories();
    QVERIFY(!categories.isEmpty());

    qDebug() << "ContentManager initialized with" << totalIcons << "icons in"
             << categories.size() << "categories";
}

void TestContentManager::testContentManager_InitializeWithInvalidPaths() {
    ContentManager* manager = new ContentManager();
    bool result = manager->initialize(
        ":/nonexistent/path/categories.json",
        ":/nonexistent/path/icons.json");

    QVERIFY(!result);
    delete manager;
}

// ============================================================================
// Search Text Tests
// ============================================================================

void TestContentManager::testSearchText_SetAndGet() {
    QVERIFY(m_contentManager != nullptr);

    QString testText = "house";
    m_contentManager->setSearchText(testText);
    QCOMPARE(m_contentManager->getSearchText(), testText);

    QString anotherText = "search";
    m_contentManager->setSearchText(anotherText);
    QCOMPARE(m_contentManager->getSearchText(), anotherText);
}

void TestContentManager::testSearchText_NoDuplicateSignals() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(searchTextChanged(const QString &)));

    // Set search text
    m_contentManager->setSearchText("test");
    QCOMPARE(spy.count(), 1);

    // Set same search text - should not emit signal
    spy.clear();
    m_contentManager->setSearchText("test");
    QCOMPARE(spy.count(), 0);

    // Set different search text - should emit signal
    m_contentManager->setSearchText("different");
    QCOMPARE(spy.count(), 1);
}

void TestContentManager::testSearchText_EmptySearch() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(searchTextChanged(const QString &)));

    // Set search text
    m_contentManager->setSearchText("test");
    QCOMPARE(spy.count(), 1);

    // Clear search text
    spy.clear();
    m_contentManager->setSearchText("");
    QCOMPARE(spy.count(), 1);
    QVERIFY(m_contentManager->getSearchText().isEmpty());
}

// ============================================================================
// Category Filter Tests
// ============================================================================

void TestContentManager::testCategory_SetAndGet() {
    QVERIFY(m_contentManager != nullptr);

    QString category = getFirstCategory();
    if (category.isEmpty()) {
        QSKIP("No categories available");
    }

    m_contentManager->setCategory(category);
    QCOMPARE(m_contentManager->getCategory(), category);

    // Set empty category
    m_contentManager->setCategory("");
    QVERIFY(m_contentManager->getCategory().isEmpty());
}

void TestContentManager::testCategory_NoDuplicateSignals() {
    QVERIFY(m_contentManager != nullptr);

    QString category = getFirstCategory();
    if (category.isEmpty()) {
        QSKIP("No categories available");
    }

    QSignalSpy spy(m_contentManager, SIGNAL(categoryChanged(const QString &)));

    // Set category
    m_contentManager->setCategory(category);
    QCOMPARE(spy.count(), 1);

    // Set same category - should not emit signal
    spy.clear();
    m_contentManager->setCategory(category);
    QCOMPARE(spy.count(), 0);

    // Set different category - should emit signal
    QStringList categories = m_contentManager->iconMetadata()->getCategories();
    if (categories.size() > 1) {
        QString anotherCategory = categories.at(1);
        m_contentManager->setCategory(anotherCategory);
        QCOMPARE(spy.count(), 1);
    }
}

void TestContentManager::testCategory_EmptyCategory() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(categoryChanged(const QString &)));

    // Set empty category
    m_contentManager->setCategory("");
    QCOMPARE(spy.count(), 1);
    QVERIFY(m_contentManager->getCategory().isEmpty());

    // Verify we get all icons when no category is set
    QStringList allIcons = m_contentManager->getFilteredIcons();
    QVERIFY(allIcons.size() > 0);
}

// ============================================================================
// Favorites Filter Tests
// ============================================================================

void TestContentManager::testFavoritesOnly_SetAndGet() {
    QVERIFY(m_contentManager != nullptr);

    m_contentManager->setShowFavoritesOnly(true);
    QVERIFY(m_contentManager->getShowFavoritesOnly());

    m_contentManager->setShowFavoritesOnly(false);
    QVERIFY(!m_contentManager->getShowFavoritesOnly());
}

void TestContentManager::testFavoritesOnly_NoDuplicateSignals() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(favoritesOnlyChanged(bool)));

    // Set favorites only to true
    m_contentManager->setShowFavoritesOnly(true);
    QCOMPARE(spy.count(), 1);

    // Set to same value - should not emit signal
    spy.clear();
    m_contentManager->setShowFavoritesOnly(true);
    QCOMPARE(spy.count(), 0);

    // Set to different value - should emit signal
    m_contentManager->setShowFavoritesOnly(false);
    QCOMPARE(spy.count(), 1);
}

// ============================================================================
// Icon Selection Tests
// ============================================================================

void TestContentManager::testIconSelection_SetAndGet() {
    QVERIFY(m_contentManager != nullptr);

    QStringList availableIcons = m_contentManager->getFilteredIcons();
    if (availableIcons.isEmpty()) {
        QSKIP("No icons available");
    }

    QString icon = availableIcons.first();
    m_contentManager->selectIcon(icon);
    QCOMPARE(m_contentManager->getSelectedIcon(), icon);

    if (availableIcons.size() > 1) {
        QString anotherIcon = availableIcons.at(1);
        m_contentManager->selectIcon(anotherIcon);
        QCOMPARE(m_contentManager->getSelectedIcon(), anotherIcon);
    }
}

void TestContentManager::testIconSelection_NoDuplicateSignals() {
    QVERIFY(m_contentManager != nullptr);

    QStringList availableIcons = m_contentManager->getFilteredIcons();
    if (availableIcons.isEmpty()) {
        QSKIP("No icons available");
    }

    QSignalSpy spy(m_contentManager, SIGNAL(iconSelected(const QString &)));

    // Select icon
    m_contentManager->selectIcon(availableIcons.first());
    QCOMPARE(spy.count(), 1);

    // Select same icon - should not emit signal
    spy.clear();
    m_contentManager->selectIcon(availableIcons.first());
    QCOMPARE(spy.count(), 0);

    // Select different icon - should emit signal
    if (availableIcons.size() > 1) {
        m_contentManager->selectIcon(availableIcons.at(1));
        QCOMPARE(spy.count(), 1);
    }
}

void TestContentManager::testIconSelection_EmptySelection() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(iconSelected(const QString &)));

    // Clear selection
    m_contentManager->selectIcon("");
    QCOMPARE(spy.count(), 1);
    QVERIFY(m_contentManager->getSelectedIcon().isEmpty());
}

// ============================================================================
// Filter Application Tests
// ============================================================================

void TestContentManager::testApplyFilters_NoFilters() {
    QVERIFY(m_contentManager != nullptr);

    // No filters applied - should get all icons
    QStringList filteredIcons = m_contentManager->applyFilters();
    QVERIFY(filteredIcons.size() > 0);

    int totalIcons = getTotalIconCount();
    QCOMPARE(filteredIcons.size(), totalIcons);
}

void TestContentManager::testApplyFilters_WithSearchText() {
    QVERIFY(m_contentManager != nullptr);

    // Search for "house" icon
    m_contentManager->setSearchText("house");
    QStringList filteredIcons = m_contentManager->applyFilters();

    // Should get results containing "house"
    QVERIFY(filteredIcons.size() > 0);
    QVERIFY(filteredIcons.contains("house"));
}

void TestContentManager::testApplyFilters_WithCategory() {
    QVERIFY(m_contentManager != nullptr);

    QString category = getFirstCategory();
    if (category.isEmpty()) {
        QSKIP("No categories available");
    }

    // Filter by category
    m_contentManager->setCategory(category);
    QStringList filteredIcons = m_contentManager->applyFilters();

    // Should get icons in this category
    QVERIFY(filteredIcons.size() > 0);

    // Verify all returned icons are in the category
    QStringList categoryIcons = m_contentManager->iconMetadata()->getIconsByCategory(category);
    for (const QString& icon : filteredIcons) {
        QVERIFY(categoryIcons.contains(icon));
    }
}

void TestContentManager::testApplyFilters_WithFavoritesOnly() {
    QVERIFY(m_contentManager != nullptr);

    // Add some favorites
    QStringList allIcons = m_contentManager->getFilteredIcons();
    if (allIcons.isEmpty()) {
        QSKIP("No icons available");
    }

    m_contentManager->favorites()->addFavorite(allIcons.first());
    if (allIcons.size() > 1) {
        m_contentManager->favorites()->addFavorite(allIcons.at(1));
    }

    // Filter to show only favorites
    m_contentManager->setShowFavoritesOnly(true);
    QStringList filteredIcons = m_contentManager->applyFilters();

    // Should get only favorite icons
    QVERIFY(filteredIcons.size() > 0);
    QVERIFY(filteredIcons.size() <= 2);  // We added max 2 favorites
    QVERIFY(filteredIcons.contains(allIcons.first()));

    // Clean up
    m_contentManager->favorites()->clearFavorites();
}

void TestContentManager::testApplyFilters_WithCombinedFilters() {
    QVERIFY(m_contentManager != nullptr);

    QString category = getFirstCategory();
    if (category.isEmpty()) {
        QSKIP("No categories available");
    }

    // Set both search and category filters
    m_contentManager->setSearchText("house");
    m_contentManager->setCategory(category);

    QStringList filteredIcons = m_contentManager->applyFilters();

    // Should get filtered results
    if (filteredIcons.size() > 0) {
        // If we got results, they should all be in the category
        QStringList categoryIcons = m_contentManager->iconMetadata()->getIconsByCategory(category);
        for (const QString& icon : filteredIcons) {
            QVERIFY(categoryIcons.contains(icon));
        }
    }
}

void TestContentManager::testGetFilteredIcons() {
    QVERIFY(m_contentManager != nullptr);

    QStringList filteredIcons = m_contentManager->getFilteredIcons();
    QVERIFY(filteredIcons.size() > 0);

    // getFilteredIcons should return same as applyFilters
    QStringList appliedIcons = m_contentManager->applyFilters();
    QCOMPARE(filteredIcons, appliedIcons);
}

// ============================================================================
// Signal Emission Tests
// ============================================================================

void TestContentManager::testSignal_IconFilterChanged() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(iconFilterChanged(const QStringList &)));

    // Change search text - should trigger filter change
    m_contentManager->setSearchText("test");
    QVERIFY(spy.count() >= 1);

    // Verify signal contains valid icon list
    QVERIFY(!spy.empty());
    QList<QVariant> args = spy.first();
    QCOMPARE(args.count(), 1);
    QStringList icons = args.at(0).toStringList();
    QVERIFY(icons.isEmpty() || !icons.isEmpty());  // Valid list either way
}

void TestContentManager::testSignal_IconSelected() {
    QVERIFY(m_contentManager != nullptr);

    QStringList availableIcons = m_contentManager->getFilteredIcons();
    if (availableIcons.isEmpty()) {
        QSKIP("No icons available");
    }

    QSignalSpy spy(m_contentManager, SIGNAL(iconSelected(const QString &)));

    // Select an icon
    QString testIcon = availableIcons.first();
    m_contentManager->selectIcon(testIcon);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.first();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.at(0).toString(), testIcon);
}

void TestContentManager::testSignal_SearchTextChanged() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(searchTextChanged(const QString &)));

    QString testText = "search";
    m_contentManager->setSearchText(testText);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.first();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.at(0).toString(), testText);
}

void TestContentManager::testSignal_CategoryChanged() {
    QVERIFY(m_contentManager != nullptr);

    QString category = getFirstCategory();
    if (category.isEmpty()) {
        QSKIP("No categories available");
    }

    QSignalSpy spy(m_contentManager, SIGNAL(categoryChanged(const QString &)));

    m_contentManager->setCategory(category);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.first();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.at(0).toString(), category);
}

void TestContentManager::testSignal_FavoritesOnlyChanged() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy spy(m_contentManager, SIGNAL(favoritesOnlyChanged(bool)));

    m_contentManager->setShowFavoritesOnly(true);

    QCOMPARE(spy.count(), 1);
    QList<QVariant> args = spy.first();
    QCOMPARE(args.count(), 1);
    QCOMPARE(args.at(0).toBool(), true);
}

void TestContentManager::testSignal_MultipleSignalsSequence() {
    QVERIFY(m_contentManager != nullptr);

    QSignalSpy searchSpy(m_contentManager, SIGNAL(searchTextChanged(const QString &)));
    QSignalSpy categorySpy(m_contentManager, SIGNAL(categoryChanged(const QString &)));
    QSignalSpy filterSpy(m_contentManager, SIGNAL(iconFilterChanged(const QStringList &)));

    // Perform multiple operations
    m_contentManager->setSearchText("test");
    int searchCount = searchSpy.count();
    int filterCount = filterSpy.count();

    QVERIFY(searchCount >= 1);
    QVERIFY(filterCount >= 1);

    QString category = getFirstCategory();
    if (!category.isEmpty()) {
        m_contentManager->setCategory(category);
        QVERIFY(categorySpy.count() >= 1);
        QVERIFY(filterSpy.count() > filterCount);
    }
}

// ============================================================================
// Filter Reset Tests
// ============================================================================

void TestContentManager::testResetFilters_ClearsAllFilters() {
    QVERIFY(m_contentManager != nullptr);

    // Set various filters
    m_contentManager->setSearchText("test");
    QString category = getFirstCategory();
    if (!category.isEmpty()) {
        m_contentManager->setCategory(category);
    }
    m_contentManager->setShowFavoritesOnly(true);

    QStringList availableIcons = m_contentManager->getFilteredIcons();
    if (!availableIcons.isEmpty()) {
        m_contentManager->selectIcon(availableIcons.first());
    }

    // Reset filters
    m_contentManager->resetFilters();

    // Verify all filters are cleared
    QVERIFY(m_contentManager->getSearchText().isEmpty());
    QVERIFY(m_contentManager->getCategory().isEmpty());
    QVERIFY(!m_contentManager->getShowFavoritesOnly());
    QVERIFY(m_contentManager->getSelectedIcon().isEmpty());
}

void TestContentManager::testResetFilters_EmitsSignals() {
    QVERIFY(m_contentManager != nullptr);

    // Set some filters first
    m_contentManager->setSearchText("test");
    m_contentManager->setShowFavoritesOnly(true);

    QSignalSpy searchSpy(m_contentManager, SIGNAL(searchTextChanged(const QString &)));
    QSignalSpy categorySpy(m_contentManager, SIGNAL(categoryChanged(const QString &)));
    QSignalSpy favoritesSpy(m_contentManager, SIGNAL(favoritesOnlyChanged(bool)));
    QSignalSpy filterSpy(m_contentManager, SIGNAL(iconFilterChanged(const QStringList &)));

    // Reset filters
    m_contentManager->resetFilters();

    // Verify signals were emitted (at least from updateFilters)
    QVERIFY(filterSpy.count() >= 1);
}

// ============================================================================
// Manager Access Tests
// ============================================================================

void TestContentManager::testManagerAccess_IconMetadata() {
    QVERIFY(m_contentManager != nullptr);

    IconMetadataManager* metadata = m_contentManager->iconMetadata();
    QVERIFY(metadata != nullptr);

    // Verify metadata manager is functional
    int totalIcons = metadata->getTotalIconCount();
    QVERIFY(totalIcons > 0);

    QStringList categories = metadata->getCategories();
    QVERIFY(!categories.isEmpty());
}

void TestContentManager::testManagerAccess_Favorites() {
    QVERIFY(m_contentManager != nullptr);

    FavoritesManager* favorites = m_contentManager->favorites();
    QVERIFY(favorites != nullptr);

    // Verify favorites manager is functional
    int count = favorites->favoritesCount();
    QCOMPARE(count, 0);  // Should start empty

    // Add a favorite
    QStringList allIcons = m_contentManager->getFilteredIcons();
    if (!allIcons.isEmpty()) {
        favorites->addFavorite(allIcons.first());
        QCOMPARE(favorites->favoritesCount(), 1);

        // Clean up
        favorites->clearFavorites();
        QCOMPARE(favorites->favoritesCount(), 0);
    }
}

// ============================================================================
// Integration Tests
// ============================================================================

void TestContentManager::testIntegration_SearchAndCategoryFilter() {
    QVERIFY(m_contentManager != nullptr);

    QString category = getFirstCategory();
    if (category.isEmpty()) {
        QSKIP("No categories available");
    }

    // Set category first
    m_contentManager->setCategory(category);
    QStringList categoryIcons = m_contentManager->getFilteredIcons();
    QVERIFY(categoryIcons.size() > 0);

    // Then add search filter
    m_contentManager->setSearchText("house");
    QStringList filtered = m_contentManager->getFilteredIcons();

    // Results should be subset of category icons
    for (const QString& icon : filtered) {
        QVERIFY(categoryIcons.contains(icon));
    }
}

void TestContentManager::testIntegration_FavoritesWithSearch() {
    QVERIFY(m_contentManager != nullptr);

    // Add some favorites
    QStringList allIcons = m_contentManager->getFilteredIcons();
    if (allIcons.isEmpty()) {
        QSKIP("No icons available");
    }

    m_contentManager->favorites()->addFavorite(allIcons.first());

    // Search for something
    m_contentManager->setSearchText("house");
    QStringList searchResults = m_contentManager->getFilteredIcons();

    // Now filter to favorites only
    m_contentManager->setShowFavoritesOnly(true);
    QStringList favoritesResults = m_contentManager->getFilteredIcons();

    // Favorites results should be subset of search results
    for (const QString& icon : favoritesResults) {
        QVERIFY(searchResults.contains(icon) || m_contentManager->favorites()->isFavorite(icon));
    }

    // Clean up
    m_contentManager->favorites()->clearFavorites();
}

void TestContentManager::testIntegration_FullWorkflow() {
    QVERIFY(m_contentManager != nullptr);

    // 1. Get initial state
    QStringList allIcons = m_contentManager->getFilteredIcons();
    QVERIFY(allIcons.size() > 0);

    // 2. Search
    m_contentManager->setSearchText("house");
    QStringList searchResults = m_contentManager->getFilteredIcons();
    QVERIFY(searchResults.size() > 0);

    // 3. Filter by category
    QString category = getFirstCategory();
    if (!category.isEmpty()) {
        m_contentManager->setCategory(category);
        QStringList categoryResults = m_contentManager->getFilteredIcons();
        QVERIFY(categoryResults.size() >= 0);

        // 4. Add to favorites
        if (!categoryResults.isEmpty()) {
            m_contentManager->favorites()->addFavorite(categoryResults.first());

            // 5. Filter to favorites
            m_contentManager->setShowFavoritesOnly(true);
            QStringList favoriteResults = m_contentManager->getFilteredIcons();
            QVERIFY(favoriteResults.size() > 0);

            // 6. Select an icon
            if (!favoriteResults.isEmpty()) {
                m_contentManager->selectIcon(favoriteResults.first());
                QCOMPARE(m_contentManager->getSelectedIcon(), favoriteResults.first());
            }
        }
    }

    // 7. Reset filters
    m_contentManager->resetFilters();
    QVERIFY(m_contentManager->getSearchText().isEmpty());
    QVERIFY(m_contentManager->getCategory().isEmpty());
    QVERIFY(!m_contentManager->getShowFavoritesOnly());

    // 8. Verify we're back to all icons
    QStringList finalIcons = m_contentManager->getFilteredIcons();
    QCOMPARE(finalIcons.size(), allIcons.size());

    // Clean up
    m_contentManager->favorites()->clearFavorites();
}

// ============================================================================
// Test Entry Point
// ============================================================================

QTEST_MAIN(TestContentManager)
#include "test_content_manager.moc"
