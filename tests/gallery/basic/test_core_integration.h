/**
 * @file test_core_integration.h
 * @brief Core Integration Tests for QtLucide Gallery
 * @details Tests basic integration between QtLucide core library and gallery components
 * @author QtLucide Test Suite
 * @date 2025
 */

#ifndef TEST_CORE_INTEGRATION_H
#define TEST_CORE_INTEGRATION_H

#include <QObject>
#include <QtTest/QtTest>

// Forward declarations
namespace lucide {
    class QtLucide;
}

/**
 * @brief Test class for core QtLucide library integration
 * @details Validates that the gallery application can properly integrate with
 *          the core QtLucide library functionality including icon loading,
 *          rendering, and resource management.
 */
class TestCoreIntegration : public QObject
{
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

    // Core library integration tests
    /**
     * @brief Test QtLucide library initialization
     * @details Verifies that the QtLucide library can be properly initialized
     *          and is ready for use by gallery components
     */
    void testLibraryInitialization();
    
    /**
     * @brief Test icon enumeration functionality
     * @details Validates that all expected icons are available and can be enumerated
     */
    void testIconEnumeration();
    
    /**
     * @brief Test icon rendering capabilities
     * @details Tests basic icon rendering functionality with various sizes and formats
     */
    void testIconRendering();
    
    /**
     * @brief Test icon caching system
     * @details Validates that icon caching works correctly for performance optimization
     */
    void testIconCaching();
    
    /**
     * @brief Test resource management
     * @details Ensures proper resource allocation and deallocation
     */
    void testResourceManagement();
    
    /**
     * @brief Test error handling in core integration
     * @details Validates proper error handling for invalid operations
     */
    void testErrorHandling();

private:
    lucide::QtLucide* m_qtLucide;           ///< QtLucide instance for testing
    QString m_testDataDir;                   ///< Test data directory path
    
    // Helper methods
    /**
     * @brief Create test QtLucide instance
     * @return Initialized QtLucide instance or nullptr on failure
     */
    lucide::QtLucide* createTestLucideInstance();
    
    /**
     * @brief Verify icon rendering quality
     * @param iconName Name of icon to test
     * @param size Size to render
     * @return true if rendering meets quality standards
     */
    bool verifyIconRenderingQuality(const QString& iconName, int size);
    
    /**
     * @brief Measure icon loading performance
     * @param iconName Name of icon to test
     * @return Loading time in milliseconds
     */
    qint64 measureIconLoadingTime(const QString& iconName);
};

#endif // TEST_CORE_INTEGRATION_H
