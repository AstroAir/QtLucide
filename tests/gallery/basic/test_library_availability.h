/**
 * @file test_library_availability.h
 * @brief Library Availability Tests for QtLucide Gallery
 * @details Tests to verify that all required libraries and dependencies are available
 *          and properly configured for the gallery application
 * @author QtLucide Test Suite
 * @date 2025
 */

#ifndef TEST_LIBRARY_AVAILABILITY_H
#define TEST_LIBRARY_AVAILABILITY_H

#include <QObject>
#include <QtTest/QtTest>

/**
 * @brief Test class for library availability validation
 * @details Validates that all required libraries, Qt modules, and dependencies
 *          are available and properly configured for the gallery application.
 */
class TestLibraryAvailability : public QObject
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

    // Qt module availability tests
    /**
     * @brief Test Qt Core module availability
     * @details Verifies that Qt Core module is available and functional
     */
    void testQtCoreAvailability();
    
    /**
     * @brief Test Qt Widgets module availability
     * @details Verifies that Qt Widgets module is available and functional
     */
    void testQtWidgetsAvailability();
    
    /**
     * @brief Test Qt SVG module availability
     * @details Verifies that Qt SVG module is available and functional
     */
    void testQtSvgAvailability();
    
    /**
     * @brief Test Qt Concurrent module availability
     * @details Verifies that Qt Concurrent module is available and functional
     */
    void testQtConcurrentAvailability();
    
    /**
     * @brief Test Qt Network module availability
     * @details Verifies that Qt Network module is available and functional
     */
    void testQtNetworkAvailability();
    
    /**
     * @brief Test Qt Test module availability
     * @details Verifies that Qt Test module is available and functional
     */
    void testQtTestAvailability();

    // QtLucide library tests
    /**
     * @brief Test QtLucide library availability
     * @details Verifies that QtLucide library is available and can be loaded
     */
    void testQtLucideLibraryAvailability();
    
    /**
     * @brief Test QtLucide headers availability
     * @details Verifies that QtLucide headers are available and can be included
     */
    void testQtLucideHeadersAvailability();
    
    /**
     * @brief Test QtLucide icon resources
     * @details Verifies that QtLucide icon resources are available
     */
    void testQtLucideIconResources();

    // Gallery component availability tests
    /**
     * @brief Test gallery core components availability
     * @details Verifies that gallery core components can be instantiated
     */
    void testGalleryCoreComponentsAvailability();
    
    /**
     * @brief Test gallery UI components availability
     * @details Verifies that gallery UI components can be instantiated
     */
    void testGalleryUIComponentsAvailability();
    
    /**
     * @brief Test gallery manager components availability
     * @details Verifies that gallery manager components can be instantiated
     */
    void testGalleryManagerComponentsAvailability();

    // System requirements tests
    /**
     * @brief Test minimum Qt version requirement
     * @details Verifies that Qt version meets minimum requirements
     */
    void testMinimumQtVersionRequirement();
    
    /**
     * @brief Test C++ standard support
     * @details Verifies that C++17 standard is supported
     */
    void testCppStandardSupport();
    
    /**
     * @brief Test platform compatibility
     * @details Verifies platform-specific requirements are met
     */
    void testPlatformCompatibility();

    // Resource availability tests
    /**
     * @brief Test icon resource files
     * @details Verifies that icon resource files are available
     */
    void testIconResourceFiles();
    
    /**
     * @brief Test theme resource files
     * @details Verifies that theme resource files are available
     */
    void testThemeResourceFiles();
    
    /**
     * @brief Test translation resource files
     * @details Verifies that translation resource files are available
     */
    void testTranslationResourceFiles();

private:
    // Helper methods
    /**
     * @brief Check if Qt module is available
     * @param moduleName Name of the Qt module to check
     * @return true if module is available
     */
    bool isQtModuleAvailable(const QString& moduleName);
    
    /**
     * @brief Check if library file exists
     * @param libraryName Name of the library to check
     * @return true if library file exists
     */
    bool isLibraryFileAvailable(const QString& libraryName);
    
    /**
     * @brief Check if header file can be included
     * @param headerName Name of the header to check
     * @return true if header can be included
     */
    bool isHeaderAvailable(const QString& headerName);
    
    /**
     * @brief Check if resource file exists
     * @param resourcePath Path to the resource file
     * @return true if resource file exists
     */
    bool isResourceFileAvailable(const QString& resourcePath);
    
    /**
     * @brief Get Qt version information
     * @return Qt version string
     */
    QString getQtVersionInfo();
    
    /**
     * @brief Get compiler information
     * @return Compiler information string
     */
    QString getCompilerInfo();
    
    /**
     * @brief Get platform information
     * @return Platform information string
     */
    QString getPlatformInfo();
};

#endif // TEST_LIBRARY_AVAILABILITY_H
