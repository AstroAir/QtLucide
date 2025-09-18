/**
 * QtLucide Gallery Application - Theme Manager
 *
 * A comprehensive theme management system that provides:
 * - Modern light and dark themes
 * - System theme detection and auto-switching
 * - Custom theme support
 * - Theme persistence and settings
 * - Real-time theme switching with smooth transitions
 * - Color palette management
 * - Icon theme integration
 */

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QSettings>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QColor>
#include <QFont>
#include <QPalette>
#include <QStyleFactory>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QVariant>

// Forward declarations
namespace lucide {
    class QtLucide;
}

/**
 * @brief Comprehensive theme management for the Gallery application
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        SystemTheme = 0,
        LightTheme = 1,
        DarkTheme = 2,
        CustomTheme = 3
    };
    Q_ENUM(Theme)

    enum ColorRole {
        // Background colors
        WindowBackground,
        PanelBackground,
        SidebarBackground,
        ContentBackground,

        // Text colors
        PrimaryText,
        SecondaryText,
        DisabledText,

        // Accent colors
        AccentColor,
        AccentColorHover,
        AccentColorPressed,

        // Border colors
        BorderColor,
        BorderColorHover,
        BorderColorFocus,

        // State colors
        SuccessColor,
        WarningColor,
        ErrorColor,
        InfoColor,

        // Selection colors
        SelectionBackground,
        SelectionText,

        // Hover colors
        HoverBackground,
        HoverText
    };
    Q_ENUM(ColorRole)

    struct ThemeColors {
        QHash<ColorRole, QColor> colors;
        QString name;
        QString description;
        bool isDark;
    };

    explicit ThemeManager(QObject *parent = nullptr);
    ~ThemeManager();

    // Theme management
    void setTheme(Theme theme);
    Theme currentTheme() const { return m_currentTheme; }
    QString currentThemeName() const;
    bool isDarkTheme() const;
    bool isSystemDarkMode() const;

    // Custom themes
    bool loadCustomTheme(const QString& filePath);
    bool saveCustomTheme(const QString& filePath, const ThemeColors& colors);
    QStringList availableCustomThemes() const;

    // Color management
    QColor getColor(ColorRole role) const;
    void setColor(ColorRole role, const QColor& color);
    ThemeColors getCurrentColors() const;
    void setCustomColors(const ThemeColors& colors);

    // Font management
    QFont getFont(const QString& role = "default") const;
    void setFont(const QString& role, const QFont& font);

    // Stylesheet management
    QString getStyleSheet() const;
    QString getWidgetStyleSheet(const QString& widgetClass) const;
    void applyThemeToWidget(QWidget* widget);
    void applyThemeToApplication();

    // Animation and transitions
    void setAnimationsEnabled(bool enabled) { m_animationsEnabled = enabled; }
    bool animationsEnabled() const { return m_animationsEnabled; }
    void animateThemeChange(QWidget* widget, int duration = 300);

    // Settings persistence
    void loadSettings();
    void saveSettings();

    // QtLucide integration
    void setLucide(lucide::QtLucide* lucide) { m_lucide = lucide; }
    lucide::QtLucide* lucide() const { return m_lucide; }

    // Utility methods
    static ThemeManager* instance();
    static QString getThemeResourcePath(Theme theme);
    static bool isValidThemeFile(const QString& filePath);

public slots:
    void onSystemThemeChanged();
    void refreshTheme();
    void resetToDefaults();

signals:
    void themeChanged(Theme theme);
    void colorsChanged();
    void fontsChanged();
    void styleSheetChanged();
    void systemThemeDetected(bool isDark);
    void iconColorChanged(const QColor& color);

private slots:
    void onAnimationFinished();
    void checkSystemTheme();

private:
    void initializeThemes();
    void setupSystemThemeDetection();
    void loadThemeColors(Theme theme);
    void loadThemeStyleSheet(Theme theme);
    void updateApplicationPalette();
    void updateIconTheme();

    // Theme loading helpers
    ThemeColors loadLightTheme();
    ThemeColors loadDarkTheme();
    ThemeColors loadSystemTheme();
    ThemeColors loadCustomThemeFromFile(const QString& filePath);

    // Stylesheet helpers
    QString loadStyleSheetFromFile(const QString& filePath);
    QString processStyleSheetVariables(const QString& styleSheet);
    QString replaceColorVariables(const QString& styleSheet, const ThemeColors& colors);

    // Animation helpers
    void fadeWidget(QWidget* widget, qreal startOpacity, qreal endOpacity, int duration);
    void slideWidget(QWidget* widget, const QPoint& startPos, const QPoint& endPos, int duration);

    // Core state
    Theme m_currentTheme;
    ThemeColors m_currentColors;
    QHash<QString, QFont> m_fonts;
    QString m_currentStyleSheet;

    // Theme data
    QHash<Theme, ThemeColors> m_themeColors;
    QHash<Theme, QString> m_themeStyleSheets;
    QHash<QString, ThemeColors> m_customThemes;

    // Settings and persistence
    QSettings* m_settings;
    QString m_customThemesPath;

    // System theme detection
    QTimer* m_systemThemeTimer;
    bool m_lastSystemDarkMode;

    // Animation system
    bool m_animationsEnabled;
    QHash<QWidget*, QPropertyAnimation*> m_activeAnimations;
    QHash<QWidget*, QGraphicsOpacityEffect*> m_opacityEffects;

    // QtLucide integration
    lucide::QtLucide* m_lucide;

    // Singleton instance
    static ThemeManager* s_instance;

    // Constants
    static constexpr int SYSTEM_THEME_CHECK_INTERVAL = 5000; // 5 seconds
    static constexpr int DEFAULT_ANIMATION_DURATION = 300;   // milliseconds
    static constexpr const char* SETTINGS_GROUP = "Theme";
    static constexpr const char* THEME_KEY = "currentTheme";
    static constexpr const char* CUSTOM_THEME_KEY = "customThemePath";
    static constexpr const char* ANIMATIONS_KEY = "animationsEnabled";
};

/**
 * @brief Helper class for theme-aware widgets
 */
class ThemeAwareWidget : public QObject
{
    Q_OBJECT

public:
    explicit ThemeAwareWidget(QWidget* widget, QObject* parent = nullptr);
    virtual ~ThemeAwareWidget();

    void setThemeClass(const QString& themeClass);
    QString themeClass() const { return m_themeClass; }

    void refreshTheme();
    void setCustomColors(const QHash<ThemeManager::ColorRole, QColor>& colors);

protected:
    virtual void onThemeChanged() {}
    virtual void applyThemeColors() {}

private slots:
    void onThemeManagerChanged();
    void onColorsChanged();

private:
    QWidget* m_widget;
    QString m_themeClass;
    QHash<ThemeManager::ColorRole, QColor> m_customColors;
};

// Convenience macros for theme-aware styling
#define THEME_COLOR(role) ThemeManager::instance()->getColor(ThemeManager::role)
#define THEME_FONT(role) ThemeManager::instance()->getFont(role)
#define APPLY_THEME(widget) ThemeManager::instance()->applyThemeToWidget(widget)

#endif // THEMEMANAGER_H
