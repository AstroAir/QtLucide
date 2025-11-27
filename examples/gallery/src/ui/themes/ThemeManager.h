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

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QGraphicsOpacityEffect>
#include <QHash>
#include <QObject>
#include <QPalette>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSettings>
#include <QStyleFactory>
#include <QTimer>
#include <QVariant>
#include <QWidget>

// Forward declarations
namespace lucide {
class QtLucide;
}

/**
 * @brief Comprehensive theme management for the Gallery application
 */
class ThemeManager : public QObject {
    Q_OBJECT

public:
    enum Theme {
        SystemTheme = 0,
        LightTheme = 1,
        DarkTheme = 2,
        CustomTheme = 3,
        HighContrastTheme = 4
    };

    // Modern Spacing System (8px base unit)
    enum SpacingSize {
        SpacingXS = 4,   // 0.5 units - Very tight spacing
        SpacingSM = 8,   // 1 unit - Base spacing unit
        SpacingMD = 16,  // 2 units - Standard spacing
        SpacingLG = 24,  // 3 units - Large spacing
        SpacingXL = 32,  // 4 units - Extra large spacing
        SpacingXXL = 48, // 6 units - Section spacing
        SpacingXXXL = 64 // 8 units - Page-level spacing
    };

    // Typography System
    enum TypographyRole {
        // Display typography (large, attention-grabbing)
        DisplayLarge = 0,  // 57px - Hero headings
        DisplayMedium = 1, // 45px - Major section headings
        DisplaySmall = 2,  // 36px - Page titles

        // Headline typography (structured content)
        HeadlineLarge = 3,  // 32px - Main headings
        HeadlineMedium = 4, // 28px - Section headings
        HeadlineSmall = 5,  // 24px - Subsection headings

        // Title typography (UI elements)
        TitleLarge = 6,  // 22px - Dialog titles
        TitleMedium = 7, // 16px - Card titles
        TitleSmall = 8,  // 14px - List item titles

        // Label typography (UI labels)
        LabelLarge = 9,   // 14px - Button labels
        LabelMedium = 10, // 12px - Form labels
        LabelSmall = 11,  // 11px - Caption labels

        // Body typography (content)
        BodyLarge = 12,  // 16px - Main content
        BodyMedium = 13, // 14px - Secondary content
        BodySmall = 14,  // 12px - Supporting text

        // Code typography (monospace)
        CodeLarge = 15,  // 14px - Code blocks
        CodeMedium = 16, // 13px - Inline code
        CodeSmall = 17   // 12px - Code annotations
    };
    Q_ENUM(TypographyRole)

    enum FontWeight {
        FontWeightThin = 100,
        FontWeightExtraLight = 200,
        FontWeightLight = 300,
        FontWeightRegular = 400,
        FontWeightMedium = 500,
        FontWeightSemiBold = 600,
        FontWeightBold = 700,
        FontWeightExtraBold = 800,
        FontWeightBlack = 900
    };
    Q_ENUM(FontWeight)
    Q_ENUM(Theme)

    enum ColorRole {
        // Background colors
        WindowBackground,
        PanelBackground,
        SidebarBackground,
        ContentBackground,
        CardBackground,
        ToolbarBackground,
        StatusBarBackground,
        MenuBackground,
        PopupBackground,
        TooltipBackground,

        // Text colors
        PrimaryText,
        SecondaryText,
        TertiaryText,
        DisabledText,
        LinkText,
        LinkTextHover,
        PlaceholderText,

        // Accent colors
        AccentColor,
        AccentColorHover,
        AccentColorPressed,
        AccentColorDisabled,
        AccentColorLight,
        AccentColorDark,

        // Border colors
        BorderColor,
        BorderColorHover,
        BorderColorFocus,
        BorderColorDisabled,
        SeparatorColor,
        OutlineColor,

        // State colors
        SuccessColor,
        SuccessColorHover,
        SuccessColorPressed,
        WarningColor,
        WarningColorHover,
        WarningColorPressed,
        ErrorColor,
        ErrorColorHover,
        ErrorColorPressed,
        InfoColor,
        InfoColorHover,
        InfoColorPressed,

        // Selection colors
        SelectionBackground,
        SelectionBackgroundInactive,
        SelectionText,
        SelectionTextInactive,

        // Hover colors
        HoverBackground,
        HoverBackgroundLight,
        HoverBackgroundStrong,
        HoverText,

        // Button colors
        ButtonBackground,
        ButtonBackgroundHover,
        ButtonBackgroundPressed,
        ButtonBackgroundDisabled,
        ButtonText,
        ButtonTextDisabled,

        // Input colors
        InputBackground,
        InputBackgroundHover,
        InputBackgroundFocus,
        InputBackgroundDisabled,
        InputBorder,
        InputBorderHover,
        InputBorderFocus,
        InputBorderError,
        InputText,
        InputTextDisabled,

        // Surface colors
        SurfaceColor,
        SurfaceColorHover,
        SurfaceColorPressed,
        SurfaceVariant,
        OnSurface,
        OnSurfaceVariant,

        // Shadow colors
        ShadowColor,
        ShadowColorLight,
        ShadowColorStrong,

        // Icon colors
        IconColor,
        IconColorHover,
        IconColorPressed,
        IconColorDisabled,
        IconColorAccent,

        // Favorite colors
        FavoriteColor,
        FavoriteColorHover,
        FavoriteColorPressed,

        // Grid colors
        GridBackground,
        GridItemBackground,
        GridItemBackgroundHover,
        GridItemBackgroundSelected,
        GridItemBorder,
        GridItemBorderHover,
        GridItemBorderSelected
    };
    Q_ENUM(ColorRole)

    struct ThemeColors {
        QHash<ColorRole, QColor> colors;
        QString name;
        QString description;
        bool isDark;
    };

    explicit ThemeManager(QObject* parent = nullptr);
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

    // Enhanced color utilities
    QColor getColorWithOpacity(ColorRole role, qreal opacity) const;
    QColor blendColors(const QColor& base, const QColor& overlay, qreal factor) const;
    QColor adjustColorBrightness(const QColor& color, qreal factor) const;
    QColor adjustColorSaturation(const QColor& color, qreal factor) const;
    QColor getContrastingColor(const QColor& background) const;
    bool isColorDark(const QColor& color) const;

    // Semantic color getters
    QColor getPrimaryColor() const { return getColor(AccentColor); }
    QColor getSecondaryColor() const { return getColor(SurfaceVariant); }
    QColor getBackgroundColor() const { return getColor(WindowBackground); }
    QColor getSurfaceColor() const { return getColor(SurfaceColor); }
    QColor getOnSurfaceColor() const { return getColor(OnSurface); }
    QColor getErrorColor() const { return getColor(ErrorColor); }
    QColor getWarningColor() const { return getColor(WarningColor); }
    QColor getSuccessColor() const { return getColor(SuccessColor); }
    QColor getInfoColor() const { return getColor(InfoColor); }

    // Color palette generation
    QStringList generateColorPalette(const QColor& baseColor, int count = 10) const;
    QHash<QString, QColor> getSemanticColorMap() const;

    // Font management (legacy)
    QFont getFont(const QString& role = "default") const;
    void setFont(const QString& role, const QFont& font);

    // Modern Typography System
    QFont getTypographyFont(TypographyRole role) const;
    void setTypographyFont(TypographyRole role, const QFont& font);
    QString getTypographyStyleSheet(TypographyRole role) const;
    void applyTypography(QWidget* widget, TypographyRole role);
    void initializeTypographySystem();

    // Typography utilities
    static QFont createFont(const QString& family, int pixelSize,
                            FontWeight weight = FontWeightRegular);
    static QString getFontFamilyStack(); // Returns system font stack
    static int getOptimalLineHeight(int fontSize);
    static QString getTypographyClassName(TypographyRole role);

    // Button styling utilities
    enum ButtonVariant {
        DefaultButton = 0,
        PrimaryButton = 1,
        SecondaryButton = 2,
        OutlineButton = 3,
        GhostButton = 4,
        IconButton = 5,
        SuccessButton = 6,
        WarningButton = 7,
        ErrorButton = 8
    };

    enum ButtonSize { SmallButton = 0, MediumButton = 1, LargeButton = 2 };

    void applyButtonStyle(QPushButton* button, ButtonVariant variant = DefaultButton,
                          ButtonSize size = MediumButton);
    void applyButtonAnimation(QPushButton* button);
    QString getButtonClassName(ButtonVariant variant, ButtonSize size) const;

    // Stylesheet management
    QString getStyleSheet() const;
    QString getWidgetStyleSheet(const QString& widgetClass) const;
    void applyThemeToWidget(QWidget* widget);
    void applyThemeToApplication();

    // Animation and transitions
    void setAnimationsEnabled(bool enabled) { m_animationsEnabled = enabled; }
    bool animationsEnabled() const { return m_animationsEnabled; }
    void animateThemeChange(QWidget* widget, int duration = DEFAULT_ANIMATION_DURATION);
    void scaleWidget(QWidget* widget, qreal startScale, qreal endScale,
                     int duration = DEFAULT_ANIMATION_DURATION);
    void applyThemeTransitions();
    void applySmoothThemeTransition(QWidget* widget);
    QString getEnhancedAnimationStyles() const;

    // Comprehensive theme application
    void applyThemeToAllComponents(QWidget* rootWidget);
    void applyThemeToSpecificComponent(QWidget* widget);

    // Spacing system utilities
    static int getSpacing(SpacingSize size) { return static_cast<int>(size); }
    static QMargins getMargins(SpacingSize size) { return QMargins(size, size, size, size); }
    static QMargins getMargins(SpacingSize horizontal, SpacingSize vertical) {
        return QMargins(horizontal, vertical, horizontal, vertical);
    }
    static QMargins getMargins(SpacingSize left, SpacingSize top, SpacingSize right,
                               SpacingSize bottom) {
        return QMargins(left, top, right, bottom);
    }
    static QString getSpacingStyleSheet(SpacingSize padding, SpacingSize margin = SpacingSM);
    static QString getPaddingStyleSheet(SpacingSize size);
    static QString getMarginStyleSheet(SpacingSize size);

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
    void themeManagerInitialized(Theme theme);
    void themeTransitionCompleted();
    void themeApplied(Theme theme);

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

    // Component-specific theme application
    void applyThemeToGalleryComponents(QWidget* rootWidget);
    void applyIconGridTheme(QWidget* widget);
    void applyIconDetailsPanelTheme(QWidget* widget);
    void applySearchWidgetTheme(QWidget* widget);
    void applyCategoryFilterTheme(QWidget* widget);
    void applyCategorySidebarTheme(QWidget* widget);
    void applyIconPreviewTheme(QWidget* widget);
    void applyCodeExampleTheme(QWidget* widget);
    void applyStandardWidgetTheme(QWidget* widget);

    // Enhanced initialization methods
    void setupThemeTransitions();
    void setupPerformanceMonitoring();
    void initializeColorSystem();
    void initializeAnimationSystem();
    void applyInitialThemeWithTransition();
    void setThemeInternal(Theme theme);

    // Core state
    Theme m_currentTheme;
    ThemeColors m_currentColors;
    QHash<QString, QFont> m_fonts;                  // Legacy font system
    QHash<TypographyRole, QFont> m_typographyFonts; // Modern typography system
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

    // Enhanced animation and transition members
    QTimer* m_themeTransitionTimer;
    QTimer* m_performanceTimer;
    int m_animationDuration;
    double m_transitionOpacity;

    // Enhanced theme mode flags
    bool m_highContrastMode;
    bool m_accessibilityMode;
    bool m_previewMode;
    bool m_customThemeMode;
    bool m_dynamicColorGeneration;
    bool m_performanceMonitoringEnabled;
    bool m_debugMode;

    // Enhanced paths
    QString m_backupThemesPath;

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
class ThemeAwareWidget : public QObject {
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
