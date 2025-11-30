/**
 * @file test_gallery_icon_loading.h
 * @brief Gallery icon loading integration test suite header
 * @details Comprehensive test class for validating the complete icon loading
 *          workflow in the QtLucide Gallery example, including metadata loading,
 *          icon rendering, filtering, and performance benchmarks.
 * @author Generated with Claude Code
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef TEST_GALLERY_ICON_LOADING_H
#define TEST_GALLERY_ICON_LOADING_H

#include <QObject>
#include <QtTest/QtTest>
#include <memory>

// Forward declarations
namespace lucide {
class QtLucide;
}

namespace gallery {
class IconMetadataManager;
class ContentManager;
} // namespace gallery

/**
 * @class TestGalleryIconLoading
 * @brief Integration tests for Gallery icon loading workflow
 *
 * This test class validates the complete end-to-end icon loading pipeline:
 *
 * ## Test Coverage
 * 1. **Metadata Loading** - JSON resources load successfully
 * 2. **QtLucide Rendering** - Icon creation and pixmap generation works
 * 3. **Content Flow** - Icons propagate correctly through ContentManager
 * 4. **Category Filtering** - Category filters work with correct icon counts
 * 5. **Search Filtering** - Text search filters return relevant results
 * 6. **Icon Availability** - All 1634+ icons are loadable
 * 7. **Performance** - All icons load in < 5 seconds
 * 8. **Pixel Verification** - Rendered pixmaps contain actual content
 * 9. **Resource Validation** - Metadata files are valid JSON
 * 10. **Signals** - ContentManager emits proper filter change signals
 *
 * ## Test Organization
 * - **initTestCase()** - One-time setup for all tests
 * - **init()** - Per-test setup with fresh managers
 * - **cleanup()** - Per-test teardown
 * - **cleanupTestCase()** - One-time cleanup
 *
 * ## Usage Notes
 * - Tests require QtLucide resources to be built
 * - Tests require gallery example components to be available
 * - Some tests are performance-sensitive; run on consistent hardware
 * - Tests are independent and can run in any order
 *
 * @see IconMetadataManager
 * @see ContentManager
 * @see lucide::QtLucide
 */
class TestGalleryIconLoading : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    TestGalleryIconLoading() = default;

    /**
     * @brief Destructor
     */
    ~TestGalleryIconLoading() override = default;

private slots:
    // ========================================================================
    // Lifecycle Methods
    // ========================================================================

    /**
     * @brief Initialize test fixtures (called once before all tests)
     * @details Creates and initializes shared QtLucide instance
     */
    void initTestCase();

    /**
     * @brief Cleanup after all tests (called once after all tests)
     * @details Destroys all test fixtures and resources
     */
    void cleanupTestCase();

    /**
     * @brief Setup for each test (called before each test method)
     * @details Creates fresh ContentManager and IconMetadataManager instances
     */
    void init();

    /**
     * @brief Cleanup after each test (called after each test method)
     * @details Destroys per-test fixtures to ensure test isolation
     */
    void cleanup();

    // ========================================================================
    // Test Methods - Core Functionality
    // ========================================================================

    /**
     * @brief Test 1: Metadata loading from embedded resources succeeds
     * @details Validates:
     *          - JSON files are accessible from resources
     *          - Metadata loads without errors
     *          - Categories are populated correctly
     *          - Total icon count meets expectations (>= 1634)
     */
    void testMetadataLoadingSuccess();

    /**
     * @brief Test 2: QtLucide can render icons successfully
     * @details Validates:
     *          - Essential icons are available
     *          - Icons can be created without null results
     *          - Pixmaps can be generated at various sizes
     *          - Rendered content is valid
     */
    void testQtLucideIconRendering();

    /**
     * @brief Test 3: Icons flow correctly through ContentManager
     * @details Validates:
     *          - ContentManager initializes successfully
     *          - Provides access to available icons
     *          - Icons have associated metadata
     *          - Icons are renderable through QtLucide
     */
    void testIconContentFlow();

    /**
     * @brief Test 4: Category filtering works with correct icon counts
     * @details Validates:
     *          - All categories are loaded
     *          - Each category has icons
     *          - Icon counts are consistent
     *          - Icon counts are within reasonable ranges
     */
    void testCategoryFilteringAccuracy();

    /**
     * @brief Test 5: Text search filtering works correctly
     * @details Validates:
     *          - Search returns results for common icon names
     *          - Results include relevant icons
     *          - Search is not case-sensitive
     *          - Search finds both exact and partial matches
     */
    void testSearchFilteringAccuracy();

    /**
     * @brief Test 6: All 1634+ icons are loadable
     * @details Validates:
     *          - Sample of all unique icons can be created
     *          - First icons load successfully
     *          - Last icons load successfully
     *          - Random samples load successfully
     *          - At least 95% success rate
     */
    void testAllIconsLoadable();

    /**
     * @brief Test 7: All icons load in acceptable time (< 5 seconds)
     * @details Validates:
     *          - Performance timing for full icon set load
     *          - All 1634+ icons load in < 5000ms
     *          - Average throughput is calculated
     *          - No significant performance degradation
     */
    void testIconRenderingPerformance();

    /**
     * @brief Test 8: Rendered pixmaps contain actual pixel content
     * @details Validates:
     *          - Pixmaps are not null
     *          - Pixmaps contain visible content (non-blank)
     *          - Content is present at various sizes
     *          - Pixel sampling confirms content
     */
    void testPixelContentVerification();

    /**
     * @brief Test 9: Metadata resource files are valid JSON
     * @details Validates:
     *          - categories.json exists in resources
     *          - icons.json exists in resources
     *          - Both files contain valid JSON
     *          - Files are readable and parseable
     */
    void testMetadataResourceValidation();

    /**
     * @brief Test 10: ContentManager signals are emitted correctly
     * @details Validates:
     *          - iconFilterChanged signal is emitted
     *          - categoryChanged signal is emitted
     *          - Signals contain correct data
     *          - Signal emission counts are appropriate
     */
    void testContentManagerSignals();

private:
    // ========================================================================
    // Member Variables
    // ========================================================================

    /** @brief Shared QtLucide instance (persists across tests) */
    std::unique_ptr<lucide::QtLucide> m_lucide;

    /** @brief Per-test IconMetadataManager instance */
    std::unique_ptr<gallery::IconMetadataManager> m_metadataManager;

    /** @brief Per-test ContentManager instance */
    std::unique_ptr<gallery::ContentManager> m_contentManager;

    // ========================================================================
    // Helper Methods
    // ========================================================================

    /**
     * @brief Helper: Verify metadata structure is valid
     * @return true if metadata structure is valid, false otherwise
     * @details Checks for required fields and reasonable values
     */
    bool validateMetadataStructure() const;

    /**
     * @brief Helper: Sample and test icons for loading
     * @param iconNames List of icon names to test
     * @param sampleSize Maximum number of icons to test
     * @return Number of successfully loaded icons
     */
    int sampleIconLoading(const QStringList& iconNames, int sampleSize) const;
};

#endif // TEST_GALLERY_ICON_LOADING_H
