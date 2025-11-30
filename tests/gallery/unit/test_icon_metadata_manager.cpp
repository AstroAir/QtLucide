/**
 * @file test_icon_metadata_manager.cpp
 * @brief Unit tests for IconMetadataManager class
 * @details Comprehensive unit tests covering all functionality of IconMetadataManager including:
 *          - Loading metadata from JSON files (categories.json and icons.json)
 *          - Retrieving categories and icons
 *          - Searching icons by name and tags
 *          - Icon metadata retrieval
 *          - Icon counting and existence checks
 *          - Error handling for invalid paths and corrupted JSON
 * @author Test Suite
 * @date 2025
 * @version 1.0
 */

#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <QtTest/QSignalSpy>
#include <QtTest/QtTest>

// Include Gallery components
#include "core/managers/IconMetadataManager.h"

class TestIconMetadataManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Metadata Loading Tests
    void testLoadMetadata_ValidFiles();
    void testLoadMetadata_InvalidCategoriesPath();
    void testLoadMetadata_InvalidIconsPath();
    void testLoadMetadata_BothPathsInvalid();
    void testLoadMetadata_CorruptedCategoriesJSON();
    void testLoadMetadata_CorruptedIconsJSON();
    void testLoadMetadata_EmptyCategoriesFile();
    void testLoadMetadata_EmptyIconsFile();

    // Categories Tests
    void testGetCategories_AfterSuccessfulLoad();
    void testGetCategories_Empty();
    void testGetCategories_MultipleCategories();

    // Get Icons by Category Tests
    void testGetIconsByCategory_ValidCategory();
    void testGetIconsByCategory_InvalidCategory();
    void testGetIconsByCategory_EmptyCategory();
    void testGetIconsByCategory_CaseSensitivity();

    // Search Icons Tests
    void testSearchIcons_EmptySearch();
    void testSearchIcons_ByName_ExactMatch();
    void testSearchIcons_ByName_PartialMatch();
    void testSearchIcons_ByName_CaseInsensitive();
    void testSearchIcons_ByTag_ExactMatch();
    void testSearchIcons_ByTag_PartialMatch();
    void testSearchIcons_NoResults();
    void testSearchIcons_MultipleMatches();

    // Icon Metadata Tests
    void testGetIconMetadata_ValidIcon();
    void testGetIconMetadata_InvalidIcon();
    void testGetIconMetadata_Completeness();
    void testGetIconMetadata_Tags();
    void testGetIconMetadata_Categories();
    void testGetIconMetadata_Contributors();

    // Icon Tags Tests
    void testGetIconTags_ValidIcon();
    void testGetIconTags_InvalidIcon();
    void testGetIconTags_EmptyTags();

    // Icon Existence Tests
    void testIconExists_ValidIcon();
    void testIconExists_InvalidIcon();
    void testIconExists_CaseSensitivity();

    // Icon Counting Tests
    void testGetTotalIconCount_AfterLoad();
    void testGetTotalIconCount_Empty();
    void testGetCategoryIconCount_ValidCategory();
    void testGetCategoryIconCount_InvalidCategory();
    void testGetCategoryIconCount_EmptyCategory();

    // Performance Tests
    void testPerformance_LoadMetadata();
    void testPerformance_SearchLargeDataset();
    void testPerformance_GetMetadata();

private:
    // Test fixtures
    QApplication* m_app;
    gallery::IconMetadataManager* m_manager;
    QTemporaryDir* m_tempDir;

    // Test data
    QString m_testCategoriesPath;
    QString m_testIconsPath;
    QString m_validCategoriesJson;
    QString m_validIconsJson;

    // Helper methods
    void createTestMetadataFiles();
    QString getResourcePath(const QString& resourceName) const;
    bool createJsonFile(const QString& filePath, const QString& content);
    QString createValidCategoriesJson() const;
    QString createValidIconsJson() const;
    QString createMalformedJson() const;
    QString createEmptyCategoriesJson() const;
    QString createEmptyIconsJson() const;
};

void TestIconMetadataManager::initTestCase() {
    // Create temporary directory for test files
    m_tempDir = new QTemporaryDir();
    QVERIFY2(m_tempDir->isValid(), "Failed to create temporary directory");

    qDebug() << "Test environment initialized with temp dir:" << m_tempDir->path();

    // Create manager instance
    m_manager = new gallery::IconMetadataManager();
    QVERIFY2(m_manager != nullptr, "Failed to create IconMetadataManager");

    // Create test metadata files
    createTestMetadataFiles();
}

void TestIconMetadataManager::cleanupTestCase() {
    if (m_manager != nullptr) {
        delete m_manager;
        m_manager = nullptr;
    }

    if (m_tempDir != nullptr) {
        delete m_tempDir;
        m_tempDir = nullptr;
    }

    qDebug() << "Test environment cleaned up";
}

void TestIconMetadataManager::init() {
    // Reset state before each test
    m_manager = new gallery::IconMetadataManager();
}

void TestIconMetadataManager::cleanup() {
    // Clean up after each test
    if (m_manager != nullptr) {
        delete m_manager;
        m_manager = nullptr;
    }
}

void TestIconMetadataManager::createTestMetadataFiles() {
    // Create valid test files
    m_validCategoriesJson = createValidCategoriesJson();
    m_validIconsJson = createValidIconsJson();

    m_testCategoriesPath = m_tempDir->path() + "/categories.json";
    m_testIconsPath = m_tempDir->path() + "/icons.json";

    createJsonFile(m_testCategoriesPath, m_validCategoriesJson);
    createJsonFile(m_testIconsPath, m_validIconsJson);

    qDebug() << "Test metadata files created:";
    qDebug() << "  Categories:" << m_testCategoriesPath;
    qDebug() << "  Icons:" << m_testIconsPath;
}

bool TestIconMetadataManager::createJsonFile(const QString& filePath, const QString& content) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to create file:" << filePath;
        return false;
    }

    file.write(content.toUtf8());
    file.close();

    return true;
}

QString TestIconMetadataManager::createValidCategoriesJson() const {
    return R"({
        "navigation": ["home", "back", "forward", "menu", "settings"],
        "media": ["play", "pause", "stop", "volume"],
        "ui": ["check", "close", "alert", "info"],
        "editing": ["edit", "delete", "save", "undo"],
        "social": ["share", "like", "comment", "message"]
    })";
}

QString TestIconMetadataManager::createValidIconsJson() const {
    return R"({
        "icons": {
            "home": {
                "name": "home",
                "svg_file": "home.svg",
                "tags": ["house", "homepage", "start", "index"],
                "categories": ["navigation"],
                "contributors": ["author1", "author2"]
            },
            "back": {
                "name": "back",
                "svg_file": "back.svg",
                "tags": ["arrow", "previous", "return"],
                "categories": ["navigation"],
                "contributors": ["author1"]
            },
            "forward": {
                "name": "forward",
                "svg_file": "forward.svg",
                "tags": ["arrow", "next", "proceed"],
                "categories": ["navigation"],
                "contributors": []
            },
            "play": {
                "name": "play",
                "svg_file": "play.svg",
                "tags": ["media", "start", "music", "video"],
                "categories": ["media"],
                "contributors": ["author2"]
            },
            "pause": {
                "name": "pause",
                "svg_file": "pause.svg",
                "tags": ["media", "stop", "music"],
                "categories": ["media"],
                "contributors": []
            },
            "check": {
                "name": "check",
                "svg_file": "check.svg",
                "tags": ["mark", "validate", "confirm"],
                "categories": ["ui"],
                "contributors": ["author1"]
            },
            "edit": {
                "name": "edit",
                "svg_file": "edit.svg",
                "tags": ["pencil", "modify", "change"],
                "categories": ["editing"],
                "contributors": ["author3"]
            }
        }
    })";
}

QString TestIconMetadataManager::createMalformedJson() const {
    return R"({
        "invalid": [
            "incomplete", "structure"
        ]
    })";
}

QString TestIconMetadataManager::createEmptyCategoriesJson() const {
    return "{}";
}

QString TestIconMetadataManager::createEmptyIconsJson() const {
    return R"({"icons": {}})";
}

// ============================================================================
// Metadata Loading Tests
// ============================================================================

void TestIconMetadataManager::testLoadMetadata_ValidFiles() {
    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata(m_testCategoriesPath, m_testIconsPath);

    QVERIFY2(result, "Failed to load valid metadata files");
    QVERIFY2(!manager.getCategories().isEmpty(), "No categories loaded");
    QVERIFY2(manager.getTotalIconCount() > 0, "No icons loaded");

    qDebug() << "Loaded" << manager.getTotalIconCount() << "icons from"
             << manager.getCategories().size() << "categories";
}

void TestIconMetadataManager::testLoadMetadata_InvalidCategoriesPath() {
    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata("/invalid/path/categories.json", m_testIconsPath);

    QVERIFY2(!result, "Should fail with invalid categories path");
    QVERIFY2(manager.getTotalIconCount() == 0, "Should not load any icons when categories fail");
}

void TestIconMetadataManager::testLoadMetadata_InvalidIconsPath() {
    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata(m_testCategoriesPath, "/invalid/path/icons.json");

    QVERIFY2(!result, "Should fail with invalid icons path");
    QVERIFY2(manager.getTotalIconCount() == 0, "Should not load icons when icons file is invalid");
}

void TestIconMetadataManager::testLoadMetadata_BothPathsInvalid() {
    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata("/invalid/categories.json", "/invalid/icons.json");

    QVERIFY2(!result, "Should fail with both paths invalid");
}

void TestIconMetadataManager::testLoadMetadata_CorruptedCategoriesJSON() {
    QString corruptPath = m_tempDir->path() + "/corrupt_categories.json";
    createJsonFile(corruptPath, createMalformedJson());

    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata(corruptPath, m_testIconsPath);

    QVERIFY2(!result, "Should fail with corrupted categories JSON");
}

void TestIconMetadataManager::testLoadMetadata_CorruptedIconsJSON() {
    QString corruptPath = m_tempDir->path() + "/corrupt_icons.json";
    QString corruptJson = R"({
        "invalid_structure": "missing icons key"
    })";
    createJsonFile(corruptPath, corruptJson);

    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata(m_testCategoriesPath, corruptPath);

    QVERIFY2(!result, "Should fail with corrupted icons JSON");
}

void TestIconMetadataManager::testLoadMetadata_EmptyCategoriesFile() {
    QString emptyPath = m_tempDir->path() + "/empty_categories.json";
    createJsonFile(emptyPath, createEmptyCategoriesJson());

    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata(emptyPath, m_testIconsPath);

    QVERIFY2(result, "Should succeed with empty categories");
    QVERIFY2(manager.getCategories().isEmpty(), "Categories should be empty");
}

void TestIconMetadataManager::testLoadMetadata_EmptyIconsFile() {
    QString emptyPath = m_tempDir->path() + "/empty_icons.json";
    createJsonFile(emptyPath, createEmptyIconsJson());

    gallery::IconMetadataManager manager;
    bool result = manager.loadMetadata(m_testCategoriesPath, emptyPath);

    QVERIFY2(result, "Should succeed with empty icons");
    QVERIFY2(manager.getTotalIconCount() == 0, "Icon count should be 0");
}

// ============================================================================
// Categories Tests
// ============================================================================

void TestIconMetadataManager::testGetCategories_AfterSuccessfulLoad() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList categories = manager.getCategories();
    QVERIFY2(!categories.isEmpty(), "Categories should not be empty");
    QCOMPARE(categories.size(), 5);

    qDebug() << "Categories:" << categories;
}

void TestIconMetadataManager::testGetCategories_Empty() {
    QString emptyPath = m_tempDir->path() + "/empty_cat.json";
    createJsonFile(emptyPath, createEmptyCategoriesJson());

    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(emptyPath, m_testIconsPath));

    QStringList categories = manager.getCategories();
    QCOMPARE(categories.size(), 0);
}

void TestIconMetadataManager::testGetCategories_MultipleCategories() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList categories = manager.getCategories();
    QVERIFY(categories.contains("navigation"));
    QVERIFY(categories.contains("media"));
    QVERIFY(categories.contains("ui"));
    QVERIFY(categories.contains("editing"));
    QVERIFY(categories.contains("social"));
}

// ============================================================================
// Get Icons by Category Tests
// ============================================================================

void TestIconMetadataManager::testGetIconsByCategory_ValidCategory() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList navigationIcons = manager.getIconsByCategory("navigation");
    QVERIFY2(!navigationIcons.isEmpty(), "Should have navigation icons");
    QVERIFY2(navigationIcons.contains("home"), "Should contain home icon");
    QVERIFY2(navigationIcons.contains("back"), "Should contain back icon");
    QVERIFY2(navigationIcons.contains("forward"), "Should contain forward icon");

    qDebug() << "Navigation icons:" << navigationIcons;
}

void TestIconMetadataManager::testGetIconsByCategory_InvalidCategory() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList icons = manager.getIconsByCategory("nonexistent");
    QVERIFY2(icons.isEmpty(), "Should return empty list for invalid category");
}

void TestIconMetadataManager::testGetIconsByCategory_EmptyCategory() {
    // Create categories with an empty category
    QString emptyCatJson = R"({
        "navigation": ["home"],
        "empty": []
    })";
    QString emptyCatPath = m_tempDir->path() + "/empty_cat.json";
    createJsonFile(emptyCatPath, emptyCatJson);

    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(emptyCatPath, m_testIconsPath));

    QStringList icons = manager.getIconsByCategory("empty");
    QCOMPARE(icons.size(), 0);
}

void TestIconMetadataManager::testGetIconsByCategory_CaseSensitivity() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList lowerCase = manager.getIconsByCategory("navigation");
    QStringList upperCase = manager.getIconsByCategory("Navigation");

    QVERIFY2(!lowerCase.isEmpty(), "Lowercase should match");
    QVERIFY2(upperCase.isEmpty(), "Uppercase should not match (case-sensitive)");
}

// ============================================================================
// Search Icons Tests
// ============================================================================

void TestIconMetadataManager::testSearchIcons_EmptySearch() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList results = manager.searchIcons("");
    QVERIFY2(!results.isEmpty(), "Empty search should return all icons");
    QCOMPARE(results.size(), manager.getTotalIconCount());
}

void TestIconMetadataManager::testSearchIcons_ByName_ExactMatch() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList results = manager.searchIcons("home");
    QVERIFY2(results.contains("home"), "Should find exact name match");
    QVERIFY2(results.size() >= 1, "Should have at least one result");

    qDebug() << "Search results for 'home':" << results;
}

void TestIconMetadataManager::testSearchIcons_ByName_PartialMatch() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList results = manager.searchIcons("hom");
    QVERIFY2(results.contains("home"), "Should find partial name match");
}

void TestIconMetadataManager::testSearchIcons_ByName_CaseInsensitive() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList resultsLower = manager.searchIcons("home");
    QStringList resultsUpper = manager.searchIcons("HOME");
    QStringList resultsMixed = manager.searchIcons("HoMe");

    QCOMPARE(resultsLower.size(), resultsUpper.size());
    QCOMPARE(resultsUpper.size(), resultsMixed.size());
    QVERIFY(resultsUpper.contains("home"));
}

void TestIconMetadataManager::testSearchIcons_ByTag_ExactMatch() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList results = manager.searchIcons("arrow");
    QVERIFY2(!results.isEmpty(), "Should find icons by tag");
    QVERIFY2(results.contains("back") || results.contains("forward"),
             "Should find back or forward icon with 'arrow' tag");

    qDebug() << "Search results for 'arrow' tag:" << results;
}

void TestIconMetadataManager::testSearchIcons_ByTag_PartialMatch() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList results = manager.searchIcons("arr");
    QVERIFY2(!results.isEmpty(), "Should find partial tag match");
}

void TestIconMetadataManager::testSearchIcons_NoResults() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList results = manager.searchIcons("nonexistentterm");
    QCOMPARE(results.size(), 0);
}

void TestIconMetadataManager::testSearchIcons_MultipleMatches() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList results = manager.searchIcons("media");
    QVERIFY2(results.size() >= 2, "Should find multiple matches for 'media'");
    QVERIFY(results.contains("play") || results.contains("pause"));
}

// ============================================================================
// Icon Metadata Tests
// ============================================================================

void TestIconMetadataManager::testGetIconMetadata_ValidIcon() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    gallery::IconMetadata metadata = manager.getIconMetadata("home");
    QVERIFY2(!metadata.name.isEmpty(), "Metadata should not be empty");
    QCOMPARE(metadata.name, QString("home"));
    QCOMPARE(metadata.svgFile, QString("home.svg"));

    qDebug() << "Icon metadata - Name:" << metadata.name << "SVG:" << metadata.svgFile
             << "Tags count:" << metadata.tags.size();
}

void TestIconMetadataManager::testGetIconMetadata_InvalidIcon() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    gallery::IconMetadata metadata = manager.getIconMetadata("nonexistent");
    QVERIFY2(metadata.name.isEmpty(), "Invalid icon should return empty metadata");
}

void TestIconMetadataManager::testGetIconMetadata_Completeness() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    gallery::IconMetadata metadata = manager.getIconMetadata("home");
    QVERIFY2(!metadata.name.isEmpty(), "Name should be present");
    QVERIFY2(!metadata.svgFile.isEmpty(), "SVG file should be present");
    QVERIFY2(!metadata.tags.isEmpty(), "Tags should be present");
    QVERIFY2(!metadata.categories.isEmpty(), "Categories should be present");
}

void TestIconMetadataManager::testGetIconMetadata_Tags() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    gallery::IconMetadata metadata = manager.getIconMetadata("home");
    QVERIFY(metadata.tags.contains("house"));
    QVERIFY(metadata.tags.contains("homepage"));
    QVERIFY(metadata.tags.contains("start"));
    QCOMPARE(metadata.tags.size(), 4);
}

void TestIconMetadataManager::testGetIconMetadata_Categories() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    gallery::IconMetadata metadata = manager.getIconMetadata("home");
    QVERIFY(metadata.categories.contains("navigation"));
    QCOMPARE(metadata.categories.size(), 1);
}

void TestIconMetadataManager::testGetIconMetadata_Contributors() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    gallery::IconMetadata homeMetadata = manager.getIconMetadata("home");
    QVERIFY(!homeMetadata.contributors.isEmpty());
    QVERIFY(homeMetadata.contributors.contains("author1"));
    QVERIFY(homeMetadata.contributors.contains("author2"));

    // Test icon with empty contributors
    gallery::IconMetadata forwardMetadata = manager.getIconMetadata("forward");
    QCOMPARE(forwardMetadata.contributors.size(), 0);
}

// ============================================================================
// Icon Tags Tests
// ============================================================================

void TestIconMetadataManager::testGetIconTags_ValidIcon() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList tags = manager.getIconTags("home");
    QVERIFY2(!tags.isEmpty(), "Should have tags");
    QVERIFY(tags.contains("house"));
    QCOMPARE(tags.size(), 4);
}

void TestIconMetadataManager::testGetIconTags_InvalidIcon() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QStringList tags = manager.getIconTags("nonexistent");
    QCOMPARE(tags.size(), 0);
}

void TestIconMetadataManager::testGetIconTags_EmptyTags() {
    QString noTagsJson = R"({
        "icons": {
            "notags": {
                "name": "notags",
                "svg_file": "notags.svg",
                "tags": [],
                "categories": ["test"],
                "contributors": []
            }
        }
    })";
    QString noTagsPath = m_tempDir->path() + "/notags.json";
    createJsonFile(noTagsPath, noTagsJson);

    QString catJson = R"({"test": ["notags"]})";
    QString catPath = m_tempDir->path() + "/notags_cat.json";
    createJsonFile(catPath, catJson);

    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(catPath, noTagsPath));

    QStringList tags = manager.getIconTags("notags");
    QCOMPARE(tags.size(), 0);
}

// ============================================================================
// Icon Existence Tests
// ============================================================================

void TestIconMetadataManager::testIconExists_ValidIcon() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QVERIFY2(manager.iconExists("home"), "home icon should exist");
    QVERIFY2(manager.iconExists("play"), "play icon should exist");
    QVERIFY2(manager.iconExists("edit"), "edit icon should exist");
}

void TestIconMetadataManager::testIconExists_InvalidIcon() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QVERIFY2(!manager.iconExists("nonexistent"), "nonexistent icon should not exist");
}

void TestIconMetadataManager::testIconExists_CaseSensitivity() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QVERIFY(manager.iconExists("home"));
    QVERIFY(!manager.iconExists("Home"));
    QVERIFY(!manager.iconExists("HOME"));
}

// ============================================================================
// Icon Counting Tests
// ============================================================================

void TestIconMetadataManager::testGetTotalIconCount_AfterLoad() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    int count = manager.getTotalIconCount();
    QVERIFY2(count > 0, "Should have loaded some icons");
    QCOMPARE(count, 7); // Based on createValidIconsJson()
}

void TestIconMetadataManager::testGetTotalIconCount_Empty() {
    QString emptyPath = m_tempDir->path() + "/empty_count.json";
    createJsonFile(emptyPath, createEmptyIconsJson());

    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, emptyPath));

    int count = manager.getTotalIconCount();
    QCOMPARE(count, 0);
}

void TestIconMetadataManager::testGetCategoryIconCount_ValidCategory() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    int navCount = manager.getCategoryIconCount("navigation");
    QVERIFY2(navCount > 0, "Navigation category should have icons");
    QCOMPARE(navCount, 3);

    int mediaCount = manager.getCategoryIconCount("media");
    QCOMPARE(mediaCount, 2);
}

void TestIconMetadataManager::testGetCategoryIconCount_InvalidCategory() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    int count = manager.getCategoryIconCount("nonexistent");
    QCOMPARE(count, 0);
}

void TestIconMetadataManager::testGetCategoryIconCount_EmptyCategory() {
    QString emptyCatJson = R"({
        "navigation": ["home"],
        "empty": []
    })";
    QString emptyCatPath = m_tempDir->path() + "/empty_count_cat.json";
    createJsonFile(emptyCatPath, emptyCatJson);

    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(emptyCatPath, m_testIconsPath));

    int count = manager.getCategoryIconCount("empty");
    QCOMPARE(count, 0);
}

// ============================================================================
// Performance Tests
// ============================================================================

void TestIconMetadataManager::testPerformance_LoadMetadata() {
    gallery::IconMetadataManager manager;

    QElapsedTimer timer;
    timer.start();

    bool result = manager.loadMetadata(m_testCategoriesPath, m_testIconsPath);

    qint64 elapsed = timer.elapsed();

    QVERIFY(result);
    qDebug() << "Metadata loading took" << elapsed << "ms";
    QVERIFY2(elapsed < 1000, "Loading should complete in less than 1 second");
}

void TestIconMetadataManager::testPerformance_SearchLargeDataset() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QElapsedTimer timer;
    timer.start();

    // Perform multiple searches
    for (int i = 0; i < 100; ++i) {
        manager.searchIcons("home");
        manager.searchIcons("arrow");
        manager.searchIcons("media");
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "300 searches took" << elapsed << "ms";
    QVERIFY2(elapsed < 500, "Searching should be fast");
}

void TestIconMetadataManager::testPerformance_GetMetadata() {
    gallery::IconMetadataManager manager;
    QVERIFY(manager.loadMetadata(m_testCategoriesPath, m_testIconsPath));

    QElapsedTimer timer;
    timer.start();

    // Get metadata for multiple icons
    for (int i = 0; i < 1000; ++i) {
        manager.getIconMetadata("home");
        manager.getIconMetadata("play");
        manager.getIconMetadata("edit");
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "3000 metadata lookups took" << elapsed << "ms";
    QVERIFY2(elapsed < 100, "Metadata lookup should be very fast");
}

QTEST_MAIN(TestIconMetadataManager)
#include "test_icon_metadata_manager.moc"
