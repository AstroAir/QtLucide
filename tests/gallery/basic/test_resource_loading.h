/**
 * @file test_resource_loading.h
 * @brief Resource Loading Tests for QtLucide Gallery
 * @details Tests to verify that all required resources are properly loaded
 *          and accessible for the gallery application
 * @author QtLucide Test Suite
 * @date 2025
 */

#ifndef TEST_RESOURCE_LOADING_H
#define TEST_RESOURCE_LOADING_H

#include <QObject>
#include <QtTest/QtTest>

/**
 * @brief Test class for resource loading validation
 * @details Validates that all required resources including icons, themes,
 *          translations, and other assets are properly loaded and accessible.
 */
class TestResourceLoading : public QObject
{
    Q_OBJECT

private slots:
    /**
     * @brief Initialize test case environment
     */
    void initTestCase();
    
    /**
     * @brief Clean up test case environment
     */
    void cleanupTestCase();

    // Icon resource loading tests
    /**
     * @brief Test icon resource loading
     * @details Verifies that icon resources are properly loaded
     */
    void testIconResourceLoading();
    
    /**
     * @brief Test icon resource accessibility
     * @details Verifies that loaded icon resources are accessible
     */
    void testIconResourceAccessibility();
    
    /**
     * @brief Test icon resource completeness
     * @details Verifies that all expected icon resources are present
     */
    void testIconResourceCompleteness();

    // Theme resource loading tests
    /**
     * @brief Test theme resource loading
     * @details Verifies that theme resources are properly loaded
     */
    void testThemeResourceLoading();
    
    /**
     * @brief Test theme resource validation
     * @details Verifies that loaded theme resources are valid
     */
    void testThemeResourceValidation();

    // Translation resource loading tests
    /**
     * @brief Test translation resource loading
     * @details Verifies that translation resources are properly loaded
     */
    void testTranslationResourceLoading();
    
    /**
     * @brief Test translation resource completeness
     * @details Verifies that all expected translations are present
     */
    void testTranslationResourceCompleteness();

    // Asset resource loading tests
    /**
     * @brief Test asset resource loading
     * @details Verifies that asset resources are properly loaded
     */
    void testAssetResourceLoading();
    
    /**
     * @brief Test asset resource integrity
     * @details Verifies that loaded asset resources have integrity
     */
    void testAssetResourceIntegrity();

    // Resource performance tests
    /**
     * @brief Test resource loading performance
     * @details Tests the performance of resource loading operations
     */
    void testResourceLoadingPerformance();
    
    /**
     * @brief Test resource memory usage
     * @details Tests memory usage during resource loading
     */
    void testResourceMemoryUsage();

private:
    // Helper methods
    /**
     * @brief Verify resource exists
     * @param resourcePath Path to the resource
     * @return true if resource exists
     */
    bool verifyResourceExists(const QString& resourcePath);
    
    /**
     * @brief Verify resource is valid
     * @param resourcePath Path to the resource
     * @return true if resource is valid
     */
    bool verifyResourceIsValid(const QString& resourcePath);
    
    /**
     * @brief Get resource size
     * @param resourcePath Path to the resource
     * @return Size of resource in bytes
     */
    qint64 getResourceSize(const QString& resourcePath);
    
    /**
     * @brief Measure resource loading time
     * @param resourcePath Path to the resource
     * @return Loading time in milliseconds
     */
    qint64 measureResourceLoadingTime(const QString& resourcePath);
};

#endif // TEST_RESOURCE_LOADING_H
