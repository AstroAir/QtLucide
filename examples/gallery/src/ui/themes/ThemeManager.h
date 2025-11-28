/**
 * @file ThemeManager.h
 * @brief Theme management system for the gallery application
 * @details Provides dark/light theme switching with QSS stylesheet loading
 *          and theme preference management.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QString>

namespace gallery {

/**
 * @enum ThemeMode
 * @brief Enumeration of available theme modes
 */
enum class ThemeMode {
    Dark,   ///< Dark theme
    Light,  ///< Light theme
    System  ///< Follow system theme preference
};

/**
 * @class ThemeManager
 * @brief Manages application theming with support for dark/light themes
 * @details ThemeManager handles loading and applying QSS stylesheets for different
 *          theme modes. It supports dynamic theme switching and emits signals when
 *          the theme changes, allowing the application to update its UI accordingly.
 *
 * @par Usage:
 * @code
 * ThemeManager themeManager;
 * themeManager.setTheme(ThemeMode::Dark);
 *
 * // Connect to theme changes
 * connect(&themeManager, &ThemeManager::themeChanged,
 *         this, &MyWindow::onThemeChanged);
 *
 * // Get current theme
 * ThemeMode current = themeManager.currentTheme();
 * @endcode
 *
 * @see ThemeMode
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Construct a ThemeManager instance
     * @param parent Parent QObject for memory management
     */
    explicit ThemeManager(QObject* parent = nullptr);

    /**
     * @brief Destroy the ThemeManager instance
     */
    ~ThemeManager() override;

    /**
     * @brief Set the application theme
     * @param mode The theme mode to apply
     * @details Changes the current theme and applies the corresponding stylesheet.
     *          Emits the themeChanged() signal when the theme is successfully changed.
     *
     * @note For System mode, the actual theme applied depends on the system settings
     * @see currentTheme(), themeChanged()
     */
    void setTheme(ThemeMode mode);

    /**
     * @brief Get the current theme mode
     * @return The currently active theme mode
     * @see setTheme()
     */
    [[nodiscard]] ThemeMode currentTheme() const;

Q_SIGNALS:
    /**
     * @brief Emitted when the theme changes
     * @param mode The newly applied theme mode
     * @details This signal is emitted whenever setTheme() is called and the theme
     *          is successfully changed. Connected widgets should update their
     *          appearance in response to this signal.
     *
     * @see setTheme()
     */
    void themeChanged(ThemeMode mode);

private:
    /**
     * @brief Load and apply a stylesheet for the given theme
     * @param mode The theme mode whose stylesheet to load
     * @return true if stylesheet was successfully applied, false otherwise
     */
    bool applyStylesheet(ThemeMode mode);

    /**
     * @brief Load stylesheet content from resources
     * @param resourcePath Path to the stylesheet resource
     * @return Stylesheet content as QString, or empty string if not found
     */
    [[nodiscard]] QString loadStylesheet(const QString& resourcePath) const;

    /**
     * @brief Detect the system theme preference
     * @return Dark or Light depending on system settings
     */
    [[nodiscard]] ThemeMode detectSystemTheme() const;

    ThemeMode m_currentTheme{ThemeMode::Dark}; ///< Currently active theme
};

} // namespace gallery

#endif // THEMEMANAGER_H
