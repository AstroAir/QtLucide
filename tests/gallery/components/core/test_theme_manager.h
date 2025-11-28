/**
 * @file test_theme_manager.h
 * @brief Theme Manager Component Tests
 * @details Comprehensive tests for ThemeManager functionality including theme switching,
 *          color management, custom themes, and integration capabilities
 * @author QtLucide Test Suite
 * @date 2025
 */

#ifndef TEST_THEME_MANAGER_H
#define TEST_THEME_MANAGER_H

#include <QColor>
#include <QObject>
#include <QTemporaryDir>
#include <QtTest/QtTest>

// Include ThemeManager for full type definition
#include "ui/themes/ThemeManager.h"

namespace gallery {
class ThemeManager;
}

/**
 * @brief Test class for ThemeManager component
 * @details Validates all ThemeManager functionality including theme switching,
 *          color management, custom theme loading/saving, and integration with
 *          other gallery components.
 */
class TestThemeManager : public QObject {
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

    /**
     * @brief Initialize individual test
     */
    void init();

    /**
     * @brief Clean up individual test
     */
    void cleanup();

    // Basic functionality tests
    /**
     * @brief Test ThemeManager creation and initialization
     * @details Verifies that ThemeManager can be created and properly initialized
     */
    void testManagerCreation();

    /**
     * @brief Test singleton pattern implementation
     * @details Validates that ThemeManager follows singleton pattern correctly
     */
    void testSingletonPattern();

    /**
     * @brief Test default theme initialization
     * @details Ensures default theme is properly set on initialization
     */
    void testDefaultThemeInitialization();

    // Theme switching tests
    /**
     * @brief Test basic theme switching functionality
     * @details Tests switching between System, Light, and Dark themes
     */
    void testThemeSwitching();

    /**
     * @brief Test theme switching signals
     * @details Validates that proper signals are emitted during theme changes
     */
    void testThemeSwitchingSignals();

    /**
     * @brief Test system theme detection
     * @details Tests automatic system theme detection and switching
     */
    void testSystemThemeDetection();

    // Color management tests
    /**
     * @brief Test color retrieval functionality
     * @details Tests getting colors for all defined color roles
     */
    void testColorRetrieval();

    /**
     * @brief Test color setting functionality
     * @details Tests setting custom colors for specific roles
     */
    void testColorSetting();

    /**
     * @brief Test color validation
     * @details Ensures invalid colors are handled properly
     */
    void testColorValidation();

    /**
     * @brief Test theme color consistency
     * @details Validates that theme colors are consistent across switches
     */
    void testThemeColorConsistency();

    // Custom theme tests
    /**
     * @brief Test custom theme creation
     * @details Tests creating and applying custom themes
     */
    void testCustomThemeCreation();

    /**
     * @brief Test custom theme loading from file
     * @details Tests loading custom themes from JSON files
     */
    void testCustomThemeLoading();

    /**
     * @brief Test custom theme saving to file
     * @details Tests saving custom themes to JSON files
     */
    void testCustomThemeSaving();

    /**
     * @brief Test custom theme validation
     * @details Ensures invalid custom theme files are handled properly
     */
    void testCustomThemeValidation();

    // Integration tests
    /**
     * @brief Test widget theme application
     * @details Tests applying themes to Qt widgets
     */
    void testWidgetThemeApplication();

    /**
     * @brief Test application-wide theme application
     * @details Tests applying themes to the entire application
     */
    void testApplicationThemeApplication();

    /**
     * @brief Test theme animations
     * @details Tests animated theme transitions
     */
    void testThemeAnimations();

    // Settings persistence tests
    /**
     * @brief Test settings loading
     * @details Tests loading theme settings from persistent storage
     */
    void testSettingsLoading();

    /**
     * @brief Test settings saving
     * @details Tests saving theme settings to persistent storage
     */
    void testSettingsSaving();

    // Error handling tests
    /**
     * @brief Test invalid theme handling
     * @details Tests handling of invalid theme values
     */
    void testInvalidThemeHandling();

    /**
     * @brief Test corrupted theme file handling
     * @details Tests handling of corrupted custom theme files
     */
    void testCorruptedThemeFileHandling();

private:
    gallery::ThemeManager* m_themeManager; ///< ThemeManager instance for testing
    QTemporaryDir* m_tempDir;     ///< Temporary directory for test files
    QString m_testThemeDir;       ///< Test theme directory path

    // Helper methods
    /**
     * @brief Create test theme manager instance
     * @return Initialized ThemeManager instance
     */
    gallery::ThemeManager* createTestThemeManager();

    /**
     * @brief Create test custom theme file
     * @param filePath Path where to create the theme file
     * @param isValid Whether to create a valid or invalid theme file
     * @return true if file was created successfully
     */
    bool createTestCustomThemeFile(const QString& filePath, bool isValid = true);

    /**
     * @brief Verify theme colors are valid
     * @param themeManager ThemeManager instance to test
     * @return true if all theme colors are valid
     */
    bool verifyThemeColorsValid(gallery::ThemeManager* themeManager);

    /**
     * @brief Wait for theme change signal
     * @param timeoutMs Timeout in milliseconds
     * @return true if signal was received within timeout
     */
    bool waitForThemeChangeSignal(int timeoutMs = 1000);
};

#endif // TEST_THEME_MANAGER_H
