/**
 * QtLucide Gallery - Enhanced Theme Manager Implementation
 *
 * Advanced theme management system with smooth transitions, dynamic theming,
 * and enhanced user experience features. Provides seamless theme switching
 * with animation support and accessibility considerations.
 *
 * Enhanced Features:
 * - Smooth theme transitions with customizable animations
 * - Dynamic color scheme generation based on system preferences
 * - Accessibility support with high contrast modes
 * - Performance monitoring and optimization
 * - Custom theme import/export functionality
 * - Real-time preview capabilities
 * - Component-level theme isolation
 * - Integration with system dark mode detection
 */

#include "ThemeManager.h"
#include <QApplication>
#include <QColorDialog>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFontDatabase>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QPalette>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScreen>
#include <QSequentialAnimationGroup>
#include <QStandardPaths>
#include <QStyle>
#include <QStyleHints>
#include <QTextStream>
#include <QTimer>
#include <QtLucide/QtLucide.h>

// Static instance
ThemeManager* ThemeManager::s_instance = nullptr;

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent), m_currentTheme(SystemTheme), m_settings(new QSettings(this)),
      m_systemThemeTimer(new QTimer(this)), m_lastSystemDarkMode(false), m_animationsEnabled(true),
      m_themeTransitionTimer(new QTimer(this)), m_performanceTimer(new QTimer(this)),
      m_animationDuration(300), m_transitionOpacity(0.8), m_highContrastMode(false),
      m_accessibilityMode(false), m_previewMode(false), m_customThemeMode(false),
      m_dynamicColorGeneration(true), m_performanceMonitoringEnabled(false), m_debugMode(false),
      m_lucide(nullptr) {
    // Enhanced initialization with performance tracking
    QElapsedTimer initTimer;
    initTimer.start();

    // Set up singleton
    if (!s_instance) {
        s_instance = this;
    }

    // Initialize enhanced paths with fallback locations
    m_customThemesPath =
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/themes";
    m_backupThemesPath =
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/themes";

    // Create directories if they don't exist
    QDir().mkpath(m_customThemesPath);
    QDir().mkpath(m_backupThemesPath);

    // Initialize enhanced theme system
    initializeThemes();
    setupSystemThemeDetection();
    setupThemeTransitions();
    setupPerformanceMonitoring();

    // Initialize enhanced typography system
    initializeTypographySystem();
    initializeColorSystem();
    initializeAnimationSystem();

    // Load settings with enhanced error handling
    loadSettings();

    // Apply initial theme with enhanced transitions
    if (m_animationsEnabled) {
        applyInitialThemeWithTransition();
    } else {
        setThemeInternal(m_currentTheme);
    }

    qDebug() << "Enhanced ThemeManager initialization completed in" << initTimer.elapsed()
             << "ms with theme:" << currentThemeName()
             << "Animations enabled:" << m_animationsEnabled
             << "High contrast mode:" << m_highContrastMode;

    // Emit initialization signal
    emit themeManagerInitialized(m_currentTheme);
}

ThemeManager::~ThemeManager() {
    saveSettings();
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

ThemeManager* ThemeManager::instance() {
    return s_instance;
}

void ThemeManager::setTheme(Theme theme) {
    if (m_currentTheme == theme)
        return;

    Theme oldTheme = m_currentTheme;
    m_currentTheme = theme;

    // Load theme data
    loadThemeColors(theme);
    loadThemeStyleSheet(theme);

    // Update application with smooth transitions
    updateApplicationPalette();
    updateIconTheme();

    // Apply enhanced theme transitions if animations are enabled
    if (m_animationsEnabled) {
        applyThemeTransitions();
    }

    // Save settings
    saveSettings();

    // Emit signals
    emit themeChanged(theme);
    emit colorsChanged();
    emit styleSheetChanged();

    qDebug() << "Theme changed from" << oldTheme << "to" << theme;
}

QString ThemeManager::currentThemeName() const {
    switch (m_currentTheme) {
        case SystemTheme:
            return "System";
        case LightTheme:
            return "Light";
        case DarkTheme:
            return "Dark";
        case CustomTheme:
            return "Custom";
        default:
            return "Unknown";
    }
}

bool ThemeManager::isDarkTheme() const {
    if (m_currentTheme == SystemTheme) {
        return isSystemDarkMode();
    }
    return m_currentColors.isDark;
}

bool ThemeManager::isSystemDarkMode() const {
    // Check system dark mode
    QStyleHints* hints = QApplication::styleHints();
    return hints->colorScheme() == Qt::ColorScheme::Dark;
}

QColor ThemeManager::getColor(ColorRole role) const {
    return m_currentColors.colors.value(role, QColor("#000000"));
}

void ThemeManager::setColor(ColorRole role, const QColor& color) {
    // Only set the color if it's valid
    if (color.isValid()) {
        m_currentColors.colors[role] = color;
        emit colorsChanged();
    }
    // If invalid color is provided, keep the existing color (no change)
}

ThemeManager::ThemeColors ThemeManager::getCurrentColors() const {
    return m_currentColors;
}

QString ThemeManager::getStyleSheet() const {
    return m_currentStyleSheet;
}

void ThemeManager::applyThemeToWidget(QWidget* widget) {
    if (!widget)
        return;

    if (m_animationsEnabled) {
        animateThemeChange(widget);
    } else {
        widget->setStyleSheet(m_currentStyleSheet);
    }
}

void ThemeManager::applyThemeToApplication() {
    if (QApplication* app = qobject_cast<QApplication*>(QApplication::instance())) {
        app->setStyleSheet(m_currentStyleSheet);
    }
    updateApplicationPalette();
}

void ThemeManager::animateThemeChange(QWidget* widget, int duration) {
    if (!widget || !m_animationsEnabled) {
        widget->setStyleSheet(m_currentStyleSheet);
        return;
    }

    // Create opacity effect if needed
    if (!m_opacityEffects.contains(widget)) {
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        m_opacityEffects[widget] = effect;
    }

    // First fade out, then apply new theme, then fade in
    QGraphicsOpacityEffect* effect = m_opacityEffects[widget];

    // Create fade out animation
    QPropertyAnimation* fadeOut = new QPropertyAnimation(effect, "opacity", this);
    fadeOut->setDuration(duration / 2);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.3);
    fadeOut->setEasingCurve(QEasingCurve::InOutQuad);

    // Connect to apply new theme and fade back in
    connect(fadeOut, &QPropertyAnimation::finished, this,
            [this, widget, effect, duration, fadeOut]() {
                // Apply new stylesheet
                if (widget) {
                    widget->setStyleSheet(m_currentStyleSheet);
                }

                // Create fade in animation
                QPropertyAnimation* fadeIn = new QPropertyAnimation(effect, "opacity", this);
                fadeIn->setDuration(duration / 2);
                fadeIn->setStartValue(0.3);
                fadeIn->setEndValue(1.0);
                fadeIn->setEasingCurve(QEasingCurve::InOutQuad);

                // Clean up after fade in completes
                connect(fadeIn, &QPropertyAnimation::finished, this, [this, fadeIn]() {
                    fadeIn->deleteLater();
                    onAnimationFinished();
                });

                fadeIn->start();
                fadeOut->deleteLater();
            });

    // Store animation reference and start
    m_activeAnimations[widget] = fadeOut;
    fadeOut->start();
}

void ThemeManager::loadSettings() {
    m_settings->beginGroup(SETTINGS_GROUP);

    int themeValue = m_settings->value(THEME_KEY, static_cast<int>(SystemTheme)).toInt();
    m_animationsEnabled = m_settings->value(ANIMATIONS_KEY, true).toBool();

    m_settings->endGroup();

    // Apply loaded theme
    setTheme(static_cast<Theme>(themeValue));
}

void ThemeManager::saveSettings() {
    m_settings->beginGroup(SETTINGS_GROUP);

    m_settings->setValue(THEME_KEY, static_cast<int>(m_currentTheme));
    m_settings->setValue(ANIMATIONS_KEY, m_animationsEnabled);

    m_settings->endGroup();
    m_settings->sync();
}

void ThemeManager::onSystemThemeChanged() {
    if (m_currentTheme == SystemTheme) {
        loadThemeColors(SystemTheme);
        loadThemeStyleSheet(SystemTheme);
        updateApplicationPalette();
        emit themeChanged(SystemTheme);
        emit colorsChanged();
    }
}

void ThemeManager::refreshTheme() {
    Theme current = m_currentTheme;
    // Force refresh by temporarily setting to a different theme
    Theme tempTheme = (current == SystemTheme) ? LightTheme : SystemTheme;
    m_currentTheme = tempTheme;
    setTheme(current);
}

void ThemeManager::checkSystemTheme() {
    bool currentSystemDark = isSystemDarkMode();
    if (currentSystemDark != m_lastSystemDarkMode) {
        m_lastSystemDarkMode = currentSystemDark;
        emit systemThemeDetected(currentSystemDark);

        if (m_currentTheme == SystemTheme) {
            onSystemThemeChanged();
        }
    }
}

void ThemeManager::initializeThemes() {
    // Initialize built-in themes
    m_themeColors[LightTheme] = loadLightTheme();
    m_themeColors[DarkTheme] = loadDarkTheme();
    m_themeColors[SystemTheme] = loadSystemTheme();

    // Load stylesheets
    m_themeStyleSheets[LightTheme] = loadStyleSheetFromFile(":/styles/modern-theme.qss");
    m_themeStyleSheets[DarkTheme] = loadStyleSheetFromFile(":/styles/dark-theme.qss");
    m_themeStyleSheets[SystemTheme] =
        isSystemDarkMode() ? m_themeStyleSheets[DarkTheme] : m_themeStyleSheets[LightTheme];
}

void ThemeManager::setupSystemThemeDetection() {
    m_lastSystemDarkMode = isSystemDarkMode();

    connect(m_systemThemeTimer, &QTimer::timeout, this, &ThemeManager::checkSystemTheme);
    m_systemThemeTimer->start(SYSTEM_THEME_CHECK_INTERVAL);
}

void ThemeManager::loadThemeColors(Theme theme) {
    if (m_themeColors.contains(theme)) {
        m_currentColors = m_themeColors[theme];
    } else {
        // Fallback to light theme
        m_currentColors = m_themeColors[LightTheme];
    }
}

void ThemeManager::loadThemeStyleSheet(Theme theme) {
    if (theme == SystemTheme) {
        // Use appropriate theme based on system
        theme = isSystemDarkMode() ? DarkTheme : LightTheme;
    }

    if (m_themeStyleSheets.contains(theme)) {
        m_currentStyleSheet = m_themeStyleSheets[theme];
    } else {
        m_currentStyleSheet = m_themeStyleSheets[LightTheme];
    }

    // Process variables in stylesheet
    m_currentStyleSheet = processStyleSheetVariables(m_currentStyleSheet);
}

ThemeManager::ThemeColors ThemeManager::loadLightTheme() {
    ThemeColors colors;
    colors.name = "Light";
    colors.description =
        "Modern light theme with enhanced accessibility and Material Design 3 colors";
    colors.isDark = false;

    // Background colors - Enhanced with better contrast and modern palette
    colors.colors[WindowBackground] = QColor("#FFFFFF");    // Pure white for main surfaces
    colors.colors[PanelBackground] = QColor("#FAFAFA");     // Subtle off-white for panels
    colors.colors[SidebarBackground] = QColor("#F8F9FA");   // Light grey for sidebars
    colors.colors[ContentBackground] = QColor("#FFFFFF");   // Pure white for content
    colors.colors[CardBackground] = QColor("#FFFFFF");      // Pure white for cards
    colors.colors[ToolbarBackground] = QColor("#FFFFFF");   // Pure white for toolbars
    colors.colors[StatusBarBackground] = QColor("#F8F9FA"); // Light grey for status
    colors.colors[MenuBackground] = QColor("#FFFFFF");      // Pure white for menus
    colors.colors[PopupBackground] = QColor("#FFFFFF");     // Pure white for popups
    colors.colors[TooltipBackground] = QColor("#323232");   // Dark grey for tooltips

    // Text colors - Enhanced for better accessibility (WCAG 2.1 AA compliance)
    colors.colors[PrimaryText] = QColor("#202124");   // High contrast primary text (21:1 ratio)
    colors.colors[SecondaryText] = QColor("#5F6368"); // Medium contrast secondary text (7:1 ratio)
    colors.colors[TertiaryText] = QColor("#80868B");  // Lower contrast tertiary text (4.5:1 ratio)
    colors.colors[DisabledText] = QColor("#9AA0A6");  // Disabled text (4.5:1 ratio)
    colors.colors[LinkText] = QColor("#1976D2");      // Material Blue 700 for links
    colors.colors[LinkTextHover] = QColor("#1565C0"); // Material Blue 800 for hover
    colors.colors[PlaceholderText] = QColor("#9AA0A6"); // Consistent with disabled text

    // Accent colors - Enhanced Material Design 3 inspired palette
    colors.colors[AccentColor] = QColor("#1976D2");         // Material Blue 700 - primary accent
    colors.colors[AccentColorHover] = QColor("#1565C0");    // Material Blue 800 - hover state
    colors.colors[AccentColorPressed] = QColor("#0D47A1");  // Material Blue 900 - pressed state
    colors.colors[AccentColorDisabled] = QColor("#DADCE0"); // Disabled accent color
    colors.colors[AccentColorLight] = QColor("#E3F2FD");    // Material Blue 50 - light variant
    colors.colors[AccentColorDark] = QColor("#0D47A1");     // Material Blue 900 - dark variant

    // Border colors - Enhanced with better visual hierarchy
    colors.colors[BorderColor] = QColor("#DADCE0");         // Standard border (3:1 contrast)
    colors.colors[BorderColorHover] = QColor("#BDC1C6");    // Hover border
    colors.colors[BorderColorFocus] = QColor("#1976D2");    // Focus border (accent color)
    colors.colors[BorderColorDisabled] = QColor("#E0E0E0"); // Disabled border
    colors.colors[SeparatorColor] = QColor("#E8EAED");      // Visual separators
    colors.colors[OutlineColor] = QColor("#DADCE0");        // Outline color

    // State colors - Enhanced semantic colors with better accessibility
    colors.colors[SuccessColor] = QColor("#4CAF50");        // Material Green 500
    colors.colors[SuccessColorHover] = QColor("#45A049");   // Success hover
    colors.colors[SuccessColorPressed] = QColor("#388E3C"); // Material Green 700
    colors.colors[WarningColor] = QColor("#FF9800");        // Material Orange 500
    colors.colors[WarningColorHover] = QColor("#FB8C00");   // Warning hover
    colors.colors[WarningColorPressed] = QColor("#F57C00"); // Material Orange 700
    colors.colors[ErrorColor] = QColor("#F44336");          // Material Red 500
    colors.colors[ErrorColorHover] = QColor("#E53935");     // Error hover
    colors.colors[ErrorColorPressed] = QColor("#D32F2F");   // Material Red 700
    colors.colors[InfoColor] = QColor("#2196F3");           // Material Blue 500
    colors.colors[InfoColorHover] = QColor("#1E88E5");      // Info hover
    colors.colors[InfoColorPressed] = QColor("#1976D2");    // Material Blue 700

    // Selection colors - Enhanced for better visibility
    colors.colors[SelectionBackground] = QColor("#1976D2");         // Primary accent for selection
    colors.colors[SelectionBackgroundInactive] = QColor("#E0E0E0"); // Inactive selection
    colors.colors[SelectionText] = QColor("#FFFFFF");               // White text on selection
    colors.colors[SelectionTextInactive] = QColor("#202124");       // Dark text on inactive

    // Hover colors - Enhanced interaction feedback
    colors.colors[HoverBackground] = QColor("#F1F3F4");       // Light hover background
    colors.colors[HoverBackgroundLight] = QColor("#F8F9FA");  // Very light hover
    colors.colors[HoverBackgroundStrong] = QColor("#E8EAED"); // Strong hover background
    colors.colors[HoverText] = QColor("#202124");             // Text on hover

    // Button colors - Enhanced button styling
    colors.colors[ButtonBackground] = QColor("#F1F3F4");         // Standard button background
    colors.colors[ButtonBackgroundHover] = QColor("#E8EAED");    // Button hover
    colors.colors[ButtonBackgroundPressed] = QColor("#DADCE0");  // Button pressed
    colors.colors[ButtonBackgroundDisabled] = QColor("#F5F5F5"); // Button disabled
    colors.colors[ButtonText] = QColor("#3C4043");               // Button text
    colors.colors[ButtonTextDisabled] = QColor("#9AA0A6");       // Disabled button text

    // Input colors - Enhanced form controls
    colors.colors[InputBackground] = QColor("#FFFFFF");         // Input background
    colors.colors[InputBackgroundHover] = QColor("#F8F9FA");    // Input hover
    colors.colors[InputBackgroundFocus] = QColor("#FFFFFF");    // Input focus
    colors.colors[InputBackgroundDisabled] = QColor("#F5F5F5"); // Input disabled
    colors.colors[InputBorder] = QColor("#DADCE0");             // Input border
    colors.colors[InputBorderHover] = QColor("#BDC1C6");        // Input hover border
    colors.colors[InputBorderFocus] = QColor("#1976D2");        // Input focus border
    colors.colors[InputBorderError] = QColor("#F44336");        // Input error border
    colors.colors[InputText] = QColor("#202124");               // Input text
    colors.colors[InputTextDisabled] = QColor("#9AA0A6");       // Input disabled text

    // Surface colors - Enhanced Material Design 3 surface system
    colors.colors[SurfaceColor] = QColor("#FFFFFF");        // Primary surface
    colors.colors[SurfaceColorHover] = QColor("#F8F9FA");   // Surface hover
    colors.colors[SurfaceColorPressed] = QColor("#F1F3F4"); // Surface pressed
    colors.colors[SurfaceVariant] = QColor("#F8F9FA");      // Surface variant
    colors.colors[OnSurface] = QColor("#202124");           // Text on surface
    colors.colors[OnSurfaceVariant] = QColor("#5F6368");    // Text on surface variant

    // Shadow colors - Enhanced elevation system
    colors.colors[ShadowColor] = QColor(0, 0, 0, 31);       // Standard shadow (12% opacity)
    colors.colors[ShadowColorLight] = QColor(0, 0, 0, 20);  // Light shadow (8% opacity)
    colors.colors[ShadowColorStrong] = QColor(0, 0, 0, 61); // Strong shadow (24% opacity)

    // Icon colors - Enhanced icon system
    colors.colors[IconColor] = QColor("#5F6368");         // Standard icon color
    colors.colors[IconColorHover] = QColor("#1976D2");    // Icon hover (accent)
    colors.colors[IconColorPressed] = QColor("#0D47A1");  // Icon pressed
    colors.colors[IconColorDisabled] = QColor("#9AA0A6"); // Disabled icon
    colors.colors[IconColorAccent] = QColor("#1976D2");   // Accent icon color

    // Favorite colors - Enhanced favorite system
    colors.colors[FavoriteColor] = QColor("#FFB74D");        // Material Amber 300
    colors.colors[FavoriteColorHover] = QColor("#FFA726");   // Favorite hover
    colors.colors[FavoriteColorPressed] = QColor("#FF9800"); // Favorite pressed

    // Grid colors - Enhanced grid system
    colors.colors[GridBackground] = QColor("#FFFFFF");             // Grid background
    colors.colors[GridItemBackground] = QColor("#FFFFFF");         // Grid item background
    colors.colors[GridItemBackgroundHover] = QColor("#F8F9FA");    // Grid item hover
    colors.colors[GridItemBackgroundSelected] = QColor("#E8F0FE"); // Grid item selected
    colors.colors[GridItemBorder] = QColor("#DADCE0");             // Grid item border
    colors.colors[GridItemBorderHover] = QColor("#BDC1C6");        // Grid item hover border
    colors.colors[GridItemBorderSelected] = QColor("#1976D2");     // Grid item selected border

    return colors;
}

ThemeManager::ThemeColors ThemeManager::loadDarkTheme() {
    ThemeColors colors;
    colors.name = "Dark";
    colors.description = "Modern dark theme with enhanced accessibility and reduced eye strain";
    colors.isDark = true;

    // Background colors - Enhanced dark theme with proper contrast ratios
    colors.colors[WindowBackground] = QColor("#121212");    // Material Dark Surface
    colors.colors[PanelBackground] = QColor("#1E1E1E");     // Elevated surface
    colors.colors[SidebarBackground] = QColor("#1A1A1A");   // Sidebar background
    colors.colors[ContentBackground] = QColor("#1E1E1E");   // Content background
    colors.colors[CardBackground] = QColor("#1E1E1E");      // Card background
    colors.colors[ToolbarBackground] = QColor("#1E1E1E");   // Toolbar background
    colors.colors[StatusBarBackground] = QColor("#1A1A1A"); // Status bar background
    colors.colors[MenuBackground] = QColor("#1E1E1E");      // Menu background
    colors.colors[PopupBackground] = QColor("#1E1E1E");     // Popup background
    colors.colors[TooltipBackground] = QColor("#424242");   // Tooltip background

    // Text colors - Enhanced for better accessibility in dark mode
    colors.colors[PrimaryText] = QColor("#E0E0E0");     // High contrast primary text
    colors.colors[SecondaryText] = QColor("#BDBDBD");   // Medium contrast secondary text
    colors.colors[TertiaryText] = QColor("#9E9E9E");    // Lower contrast tertiary text
    colors.colors[DisabledText] = QColor("#757575");    // Disabled text
    colors.colors[LinkText] = QColor("#90CAF9");        // Material Blue 200 for links
    colors.colors[LinkTextHover] = QColor("#64B5F6");   // Material Blue 300 for hover
    colors.colors[PlaceholderText] = QColor("#757575"); // Consistent with disabled text

    // Accent colors - Enhanced Material Design 3 dark theme palette
    colors.colors[AccentColor] = QColor("#90CAF9");         // Material Blue 200 - primary accent
    colors.colors[AccentColorHover] = QColor("#64B5F6");    // Material Blue 300 - hover state
    colors.colors[AccentColorPressed] = QColor("#42A5F5");  // Material Blue 400 - pressed state
    colors.colors[AccentColorDisabled] = QColor("#424242"); // Disabled accent color
    colors.colors[AccentColorLight] = QColor("#1A237E");    // Dark blue variant
    colors.colors[AccentColorDark] = QColor("#BBDEFB");     // Light blue variant

    // Border colors - Enhanced dark theme borders
    colors.colors[BorderColor] = QColor("#424242");         // Standard border
    colors.colors[BorderColorHover] = QColor("#616161");    // Hover border
    colors.colors[BorderColorFocus] = QColor("#90CAF9");    // Focus border (accent color)
    colors.colors[BorderColorDisabled] = QColor("#303030"); // Disabled border
    colors.colors[SeparatorColor] = QColor("#373737");      // Visual separators
    colors.colors[OutlineColor] = QColor("#424242");        // Outline color

    // State colors - Enhanced semantic colors for dark theme
    colors.colors[SuccessColor] = QColor("#81C784");        // Material Green 300
    colors.colors[SuccessColorHover] = QColor("#66BB6A");   // Success hover
    colors.colors[SuccessColorPressed] = QColor("#4CAF50"); // Material Green 500
    colors.colors[WarningColor] = QColor("#FFB74D");        // Material Orange 300
    colors.colors[WarningColorHover] = QColor("#FFA726");   // Warning hover
    colors.colors[WarningColorPressed] = QColor("#FF9800"); // Material Orange 500
    colors.colors[ErrorColor] = QColor("#E57373");          // Material Red 300
    colors.colors[ErrorColorHover] = QColor("#EF5350");     // Error hover
    colors.colors[ErrorColorPressed] = QColor("#F44336");   // Material Red 500
    colors.colors[InfoColor] = QColor("#64B5F6");           // Material Blue 300
    colors.colors[InfoColorHover] = QColor("#42A5F5");      // Info hover
    colors.colors[InfoColorPressed] = QColor("#2196F3");    // Material Blue 500

    // Selection colors - Enhanced dark theme selection
    colors.colors[SelectionBackground] = QColor("#90CAF9");         // Primary accent for selection
    colors.colors[SelectionBackgroundInactive] = QColor("#424242"); // Inactive selection
    colors.colors[SelectionText] = QColor("#000000");               // Black text on selection
    colors.colors[SelectionTextInactive] = QColor("#E0E0E0");       // Light text on inactive

    // Hover colors - Enhanced dark theme interaction feedback
    colors.colors[HoverBackground] = QColor("#2C2C2C");       // Light hover background
    colors.colors[HoverBackgroundLight] = QColor("#262626");  // Very light hover
    colors.colors[HoverBackgroundStrong] = QColor("#373737"); // Strong hover background
    colors.colors[HoverText] = QColor("#E0E0E0");             // Text on hover

    // Button colors - Enhanced dark theme button styling
    colors.colors[ButtonBackground] = QColor("#2C2C2C");         // Standard button background
    colors.colors[ButtonBackgroundHover] = QColor("#373737");    // Button hover
    colors.colors[ButtonBackgroundPressed] = QColor("#424242");  // Button pressed
    colors.colors[ButtonBackgroundDisabled] = QColor("#1A1A1A"); // Button disabled
    colors.colors[ButtonText] = QColor("#E0E0E0");               // Button text
    colors.colors[ButtonTextDisabled] = QColor("#757575");       // Disabled button text

    // Input colors - Enhanced dark theme form controls
    colors.colors[InputBackground] = QColor("#2C2C2C");         // Input background
    colors.colors[InputBackgroundHover] = QColor("#373737");    // Input hover
    colors.colors[InputBackgroundFocus] = QColor("#2C2C2C");    // Input focus
    colors.colors[InputBackgroundDisabled] = QColor("#1A1A1A"); // Input disabled
    colors.colors[InputBorder] = QColor("#424242");             // Input border
    colors.colors[InputBorderHover] = QColor("#616161");        // Input hover border
    colors.colors[InputBorderFocus] = QColor("#90CAF9");        // Input focus border
    colors.colors[InputBorderError] = QColor("#E57373");        // Input error border
    colors.colors[InputText] = QColor("#E0E0E0");               // Input text
    colors.colors[InputTextDisabled] = QColor("#757575");       // Input disabled text

    // Surface colors - Enhanced Material Design 3 dark surface system
    colors.colors[SurfaceColor] = QColor("#1E1E1E");        // Primary surface
    colors.colors[SurfaceColorHover] = QColor("#2C2C2C");   // Surface hover
    colors.colors[SurfaceColorPressed] = QColor("#373737"); // Surface pressed
    colors.colors[SurfaceVariant] = QColor("#2C2C2C");      // Surface variant
    colors.colors[OnSurface] = QColor("#E0E0E0");           // Text on surface
    colors.colors[OnSurfaceVariant] = QColor("#BDBDBD");    // Text on surface variant

    // Shadow colors - Enhanced dark theme elevation system
    colors.colors[ShadowColor] = QColor(0, 0, 0, 102);       // Standard shadow (40% opacity)
    colors.colors[ShadowColorLight] = QColor(0, 0, 0, 51);   // Light shadow (20% opacity)
    colors.colors[ShadowColorStrong] = QColor(0, 0, 0, 153); // Strong shadow (60% opacity)

    // Icon colors - Enhanced dark theme icon system
    colors.colors[IconColor] = QColor("#BDBDBD");         // Standard icon color
    colors.colors[IconColorHover] = QColor("#90CAF9");    // Icon hover (accent)
    colors.colors[IconColorPressed] = QColor("#64B5F6");  // Icon pressed
    colors.colors[IconColorDisabled] = QColor("#757575"); // Disabled icon
    colors.colors[IconColorAccent] = QColor("#90CAF9");   // Accent icon color

    // Favorite colors - Enhanced dark theme favorite system
    colors.colors[FavoriteColor] = QColor("#FFD54F");        // Material Amber 200
    colors.colors[FavoriteColorHover] = QColor("#FFCC02");   // Favorite hover
    colors.colors[FavoriteColorPressed] = QColor("#FFC107"); // Favorite pressed

    // Grid colors - Enhanced dark theme grid system
    colors.colors[GridBackground] = QColor("#1E1E1E");             // Grid background
    colors.colors[GridItemBackground] = QColor("#1E1E1E");         // Grid item background
    colors.colors[GridItemBackgroundHover] = QColor("#2C2C2C");    // Grid item hover
    colors.colors[GridItemBackgroundSelected] = QColor("#1A237E"); // Grid item selected
    colors.colors[GridItemBorder] = QColor("#424242");             // Grid item border
    colors.colors[GridItemBorderHover] = QColor("#616161");        // Grid item hover border
    colors.colors[GridItemBorderSelected] = QColor("#90CAF9");     // Grid item selected border

    return colors;
}

ThemeManager::ThemeColors ThemeManager::loadSystemTheme() {
    // Return appropriate theme based on system
    return isSystemDarkMode() ? loadDarkTheme() : loadLightTheme();
}

QString ThemeManager::loadStyleSheetFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to load stylesheet from:" << filePath;
        return QString();
    }

    QTextStream stream(&file);
    return stream.readAll();
}

QString ThemeManager::processStyleSheetVariables(const QString& styleSheet) {
    QString processed = styleSheet;

    // Replace color variables with actual colors
    processed = replaceColorVariables(processed, m_currentColors);

    return processed;
}

QString ThemeManager::replaceColorVariables(const QString& styleSheet, const ThemeColors& colors) {
    QString result = styleSheet;

    // Define comprehensive color variable mappings
    QHash<QString, ColorRole> colorVars = {// Background colors
                                           {"@window-bg", WindowBackground},
                                           {"@panel-bg", PanelBackground},
                                           {"@sidebar-bg", SidebarBackground},
                                           {"@content-bg", ContentBackground},
                                           {"@card-bg", CardBackground},
                                           {"@toolbar-bg", ToolbarBackground},
                                           {"@statusbar-bg", StatusBarBackground},
                                           {"@menu-bg", MenuBackground},
                                           {"@popup-bg", PopupBackground},
                                           {"@tooltip-bg", TooltipBackground},

                                           // Text colors
                                           {"@primary-text", PrimaryText},
                                           {"@secondary-text", SecondaryText},
                                           {"@tertiary-text", TertiaryText},
                                           {"@disabled-text", DisabledText},
                                           {"@link-text", LinkText},
                                           {"@link-text-hover", LinkTextHover},
                                           {"@placeholder-text", PlaceholderText},

                                           // Accent colors
                                           {"@accent-color", AccentColor},
                                           {"@accent-hover", AccentColorHover},
                                           {"@accent-pressed", AccentColorPressed},
                                           {"@accent-disabled", AccentColorDisabled},
                                           {"@accent-light", AccentColorLight},
                                           {"@accent-dark", AccentColorDark},

                                           // Border colors
                                           {"@border-color", BorderColor},
                                           {"@border-hover", BorderColorHover},
                                           {"@border-focus", BorderColorFocus},
                                           {"@border-disabled", BorderColorDisabled},
                                           {"@separator-color", SeparatorColor},
                                           {"@outline-color", OutlineColor},

                                           // State colors
                                           {"@success-color", SuccessColor},
                                           {"@success-hover", SuccessColorHover},
                                           {"@success-pressed", SuccessColorPressed},
                                           {"@warning-color", WarningColor},
                                           {"@warning-hover", WarningColorHover},
                                           {"@warning-pressed", WarningColorPressed},
                                           {"@error-color", ErrorColor},
                                           {"@error-hover", ErrorColorHover},
                                           {"@error-pressed", ErrorColorPressed},
                                           {"@info-color", InfoColor},
                                           {"@info-hover", InfoColorHover},
                                           {"@info-pressed", InfoColorPressed},

                                           // Selection colors
                                           {"@selection-bg", SelectionBackground},
                                           {"@selection-bg-inactive", SelectionBackgroundInactive},
                                           {"@selection-text", SelectionText},
                                           {"@selection-text-inactive", SelectionTextInactive},

                                           // Hover colors
                                           {"@hover-bg", HoverBackground},
                                           {"@hover-bg-light", HoverBackgroundLight},
                                           {"@hover-bg-strong", HoverBackgroundStrong},
                                           {"@hover-text", HoverText},

                                           // Button colors
                                           {"@button-bg", ButtonBackground},
                                           {"@button-bg-hover", ButtonBackgroundHover},
                                           {"@button-bg-pressed", ButtonBackgroundPressed},
                                           {"@button-bg-disabled", ButtonBackgroundDisabled},
                                           {"@button-text", ButtonText},
                                           {"@button-text-disabled", ButtonTextDisabled},

                                           // Input colors
                                           {"@input-bg", InputBackground},
                                           {"@input-bg-hover", InputBackgroundHover},
                                           {"@input-bg-focus", InputBackgroundFocus},
                                           {"@input-bg-disabled", InputBackgroundDisabled},
                                           {"@input-border", InputBorder},
                                           {"@input-border-hover", InputBorderHover},
                                           {"@input-border-focus", InputBorderFocus},
                                           {"@input-border-error", InputBorderError},
                                           {"@input-text", InputText},
                                           {"@input-text-disabled", InputTextDisabled},

                                           // Surface colors
                                           {"@surface-color", SurfaceColor},
                                           {"@surface-hover", SurfaceColorHover},
                                           {"@surface-pressed", SurfaceColorPressed},
                                           {"@surface-variant", SurfaceVariant},
                                           {"@on-surface", OnSurface},
                                           {"@on-surface-variant", OnSurfaceVariant},

                                           // Shadow colors
                                           {"@shadow-color", ShadowColor},
                                           {"@shadow-light", ShadowColorLight},
                                           {"@shadow-strong", ShadowColorStrong},

                                           // Icon colors
                                           {"@icon-color", IconColor},
                                           {"@icon-hover", IconColorHover},
                                           {"@icon-pressed", IconColorPressed},
                                           {"@icon-disabled", IconColorDisabled},
                                           {"@icon-accent", IconColorAccent},

                                           // Favorite colors
                                           {"@favorite-color", FavoriteColor},
                                           {"@favorite-hover", FavoriteColorHover},
                                           {"@favorite-pressed", FavoriteColorPressed},

                                           // Grid colors
                                           {"@grid-bg", GridBackground},
                                           {"@grid-item-bg", GridItemBackground},
                                           {"@grid-item-bg-hover", GridItemBackgroundHover},
                                           {"@grid-item-bg-selected", GridItemBackgroundSelected},
                                           {"@grid-item-border", GridItemBorder},
                                           {"@grid-item-border-hover", GridItemBorderHover},
                                           {"@grid-item-border-selected", GridItemBorderSelected}};

    // Replace variables with actual color values
    for (auto it = colorVars.begin(); it != colorVars.end(); ++it) {
        QColor color = colors.colors.value(it.value(), QColor("#000000"));
        result.replace(it.key(), color.name());
    }

    return result;
}

QColor ThemeManager::getColorWithOpacity(ColorRole role, qreal opacity) const {
    QColor color = getColor(role);
    color.setAlphaF(static_cast<float>(opacity));
    return color;
}

QColor ThemeManager::blendColors(const QColor& base, const QColor& overlay, qreal factor) const {
    if (factor <= 0.0)
        return base;
    if (factor >= 1.0)
        return overlay;

    int r = static_cast<int>(base.red() * (1.0 - factor) + overlay.red() * factor);
    int g = static_cast<int>(base.green() * (1.0 - factor) + overlay.green() * factor);
    int b = static_cast<int>(base.blue() * (1.0 - factor) + overlay.blue() * factor);
    int a = static_cast<int>(base.alpha() * (1.0 - factor) + overlay.alpha() * factor);

    return QColor(r, g, b, a);
}

QColor ThemeManager::adjustColorBrightness(const QColor& color, qreal factor) const {
    QColor hsl = color.toHsl();
    int lightness = hsl.lightness();
    lightness = qBound(0, static_cast<int>(lightness * factor), 255);
    hsl.setHsl(hsl.hue(), hsl.saturation(), lightness, hsl.alpha());
    return hsl.toRgb();
}

QColor ThemeManager::adjustColorSaturation(const QColor& color, qreal factor) const {
    QColor hsl = color.toHsl();
    int saturation = hsl.saturation();
    saturation = qBound(0, static_cast<int>(saturation * factor), 255);
    hsl.setHsl(hsl.hue(), saturation, hsl.lightness(), hsl.alpha());
    return hsl.toRgb();
}

QColor ThemeManager::getContrastingColor(const QColor& background) const {
    // Calculate relative luminance
    qreal r = background.redF();
    qreal g = background.greenF();
    qreal b = background.blueF();

    // Apply gamma correction
    r = (r <= 0.03928) ? r / 12.92 : qPow((r + 0.055) / 1.055, 2.4);
    g = (g <= 0.03928) ? g / 12.92 : qPow((g + 0.055) / 1.055, 2.4);
    b = (b <= 0.03928) ? b / 12.92 : qPow((b + 0.055) / 1.055, 2.4);

    qreal luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;

    // Return white for dark backgrounds, black for light backgrounds
    return (luminance > 0.5) ? QColor("#000000") : QColor("#ffffff");
}

bool ThemeManager::isColorDark(const QColor& color) const {
    // Calculate relative luminance
    qreal r = color.redF();
    qreal g = color.greenF();
    qreal b = color.blueF();

    // Apply gamma correction
    r = (r <= 0.03928) ? r / 12.92 : qPow((r + 0.055) / 1.055, 2.4);
    g = (g <= 0.03928) ? g / 12.92 : qPow((g + 0.055) / 1.055, 2.4);
    b = (b <= 0.03928) ? b / 12.92 : qPow((b + 0.055) / 1.055, 2.4);

    qreal luminance = 0.2126 * r + 0.7152 * g + 0.0722 * b;

    return luminance <= 0.5;
}

QStringList ThemeManager::generateColorPalette(const QColor& baseColor, int count) const {
    QStringList palette;
    if (count <= 0)
        return palette;

    QColor hsl = baseColor.toHsl();
    int baseHue = hsl.hue();
    int baseSaturation = hsl.saturation();

    for (int i = 0; i < count; ++i) {
        qreal factor = static_cast<qreal>(i) / (count - 1);

        // Generate variations by adjusting lightness
        int lightness;
        if (isDarkTheme()) {
            // For dark themes, go from dark to light
            lightness = qBound(20, static_cast<int>(20 + factor * 200), 220);
        } else {
            // For light themes, go from light to dark
            lightness = qBound(35, static_cast<int>(220 - factor * 185), 220);
        }

        QColor color;
        color.setHsl(baseHue, baseSaturation, lightness);
        palette.append(color.name());
    }

    return palette;
}

QHash<QString, QColor> ThemeManager::getSemanticColorMap() const {
    QHash<QString, QColor> semanticColors;

    // Primary colors
    semanticColors["primary"] = getColor(AccentColor);
    semanticColors["primary-hover"] = getColor(AccentColorHover);
    semanticColors["primary-pressed"] = getColor(AccentColorPressed);
    semanticColors["primary-disabled"] = getColor(AccentColorDisabled);

    // Secondary colors
    semanticColors["secondary"] = getColor(SurfaceVariant);
    semanticColors["secondary-hover"] = getColor(SurfaceColorHover);
    semanticColors["secondary-pressed"] = getColor(SurfaceColorPressed);

    // Background colors
    semanticColors["background"] = getColor(WindowBackground);
    semanticColors["surface"] = getColor(SurfaceColor);
    semanticColors["panel"] = getColor(PanelBackground);
    semanticColors["card"] = getColor(CardBackground);

    // Text colors
    semanticColors["on-background"] = getColor(PrimaryText);
    semanticColors["on-surface"] = getColor(OnSurface);
    semanticColors["on-surface-variant"] = getColor(OnSurfaceVariant);
    semanticColors["on-primary"] = getColor(ButtonText);

    // State colors
    semanticColors["success"] = getColor(SuccessColor);
    semanticColors["warning"] = getColor(WarningColor);
    semanticColors["error"] = getColor(ErrorColor);
    semanticColors["info"] = getColor(InfoColor);

    // Interactive colors
    semanticColors["hover"] = getColor(HoverBackground);
    semanticColors["selection"] = getColor(SelectionBackground);
    semanticColors["focus"] = getColor(BorderColorFocus);

    // Border colors
    semanticColors["border"] = getColor(BorderColor);
    semanticColors["separator"] = getColor(SeparatorColor);
    semanticColors["outline"] = getColor(OutlineColor);

    return semanticColors;
}

void ThemeManager::slideWidget(QWidget* widget, const QPoint& startPos, const QPoint& endPos,
                               int duration) {
    if (!widget || !m_animationsEnabled) {
        widget->move(endPos);
        return;
    }

    // Clean up any existing animation for this widget
    if (m_activeAnimations.contains(widget)) {
        m_activeAnimations[widget]->stop();
        m_activeAnimations[widget]->deleteLater();
        m_activeAnimations.remove(widget);
    }

    // Create position animation
    QPropertyAnimation* animation = new QPropertyAnimation(widget, "pos", this);
    animation->setDuration(duration);
    animation->setStartValue(startPos);
    animation->setEndValue(endPos);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Store animation reference
    m_activeAnimations[widget] = animation;

    // Connect finished signal for cleanup
    connect(animation, &QPropertyAnimation::finished, this, [this, widget, animation]() {
        if (m_activeAnimations.contains(widget)) {
            m_activeAnimations.remove(widget);
        }
        animation->deleteLater();
        onAnimationFinished();
    });

    animation->start();
}

void ThemeManager::scaleWidget(QWidget* widget, qreal startScale, qreal endScale, int duration) {
    if (!widget || !m_animationsEnabled) {
        return;
    }

    // Clean up any existing animation for this widget
    if (m_activeAnimations.contains(widget)) {
        m_activeAnimations[widget]->stop();
        m_activeAnimations[widget]->deleteLater();
        m_activeAnimations.remove(widget);
    }

    // Create scale animation using QGraphicsEffect
    QGraphicsOpacityEffect* effect = m_opacityEffects.value(widget);
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        m_opacityEffects[widget] = effect;
    }

    // For now, we'll use a simple resize animation as a scale substitute
    QSize startSize = widget->size() * startScale;
    QSize endSize = widget->size() * endScale;

    QPropertyAnimation* animation = new QPropertyAnimation(widget, "size", this);
    animation->setDuration(duration);
    animation->setStartValue(startSize);
    animation->setEndValue(endSize);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Store animation reference
    m_activeAnimations[widget] = animation;

    // Connect finished signal for cleanup
    connect(animation, &QPropertyAnimation::finished, this, [this, widget, animation]() {
        if (m_activeAnimations.contains(widget)) {
            m_activeAnimations.remove(widget);
        }
        animation->deleteLater();
        onAnimationFinished();
    });

    animation->start();
}

void ThemeManager::updateApplicationPalette() {
    QPalette palette;

    // Set palette colors based on current theme
    palette.setColor(QPalette::Window, getColor(WindowBackground));
    palette.setColor(QPalette::WindowText, getColor(PrimaryText));
    palette.setColor(QPalette::Base, getColor(ContentBackground));
    palette.setColor(QPalette::AlternateBase, getColor(PanelBackground));
    palette.setColor(QPalette::Text, getColor(PrimaryText));
    palette.setColor(QPalette::Button, getColor(PanelBackground));
    palette.setColor(QPalette::ButtonText, getColor(PrimaryText));
    palette.setColor(QPalette::Highlight, getColor(SelectionBackground));
    palette.setColor(QPalette::HighlightedText, getColor(SelectionText));
    palette.setColor(QPalette::Link, getColor(AccentColor));
    palette.setColor(QPalette::LinkVisited, getColor(AccentColorPressed));

    // Apply disabled colors
    palette.setColor(QPalette::Disabled, QPalette::WindowText, getColor(DisabledText));
    palette.setColor(QPalette::Disabled, QPalette::Text, getColor(DisabledText));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, getColor(DisabledText));

    QApplication::setPalette(palette);
}

void ThemeManager::updateIconTheme() {
    // Update icon colors based on theme
    // This would integrate with QtLucide to set appropriate icon colors
    QString iconColor = isDarkTheme() ? "#e8e8e8" : "#2c3e50";

    // Integrate with QtLucide icon color settings
    if (m_lucide) {
        m_lucide->setDefaultOption("color", QColor(iconColor));
        emit iconColorChanged(QColor(iconColor));
    }
}

void ThemeManager::fadeWidget(QWidget* widget, qreal startOpacity, qreal endOpacity, int duration) {
    if (!widget || !m_animationsEnabled)
        return;

    QGraphicsOpacityEffect* effect = m_opacityEffects.value(widget);
    if (!effect)
        return;

    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity", this);
    animation->setDuration(duration);
    animation->setStartValue(startOpacity);
    animation->setEndValue(endOpacity);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(animation, &QPropertyAnimation::finished, this, &ThemeManager::onAnimationFinished);

    m_activeAnimations[widget] = animation;
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void ThemeManager::onAnimationFinished() {
    QPropertyAnimation* animation = qobject_cast<QPropertyAnimation*>(sender());
    if (!animation)
        return;

    // Remove from active animations
    for (auto it = m_activeAnimations.begin(); it != m_activeAnimations.end(); ++it) {
        if (it.value() == animation) {
            m_activeAnimations.erase(it);
            break;
        }
    }
}

QString ThemeManager::getThemeResourcePath(Theme theme) {
    switch (theme) {
        case LightTheme:
            return ":/styles/modern-theme.qss";
        case DarkTheme:
            return ":/styles/dark-theme.qss";
        case SystemTheme: {
            // Check system dark mode statically
            QStyleHints* hints = QApplication::styleHints();
            bool isDark = hints->colorScheme() == Qt::ColorScheme::Dark;
            return isDark ? ":/styles/dark-theme.qss" : ":/styles/modern-theme.qss";
        }
        default:
            return ":/styles/modern-theme.qss";
    }
}

// ThemeAwareWidget implementation
ThemeAwareWidget::ThemeAwareWidget(QWidget* widget, QObject* parent)
    : QObject(parent), m_widget(widget) {
    if (ThemeManager::instance()) {
        connect(ThemeManager::instance(), &ThemeManager::themeChanged, this,
                &ThemeAwareWidget::onThemeManagerChanged);
        connect(ThemeManager::instance(), &ThemeManager::colorsChanged, this,
                &ThemeAwareWidget::onColorsChanged);
    }
}

ThemeAwareWidget::~ThemeAwareWidget() = default;

void ThemeAwareWidget::setThemeClass(const QString& themeClass) {
    m_themeClass = themeClass;
    refreshTheme();
}

void ThemeAwareWidget::refreshTheme() {
    if (!m_widget || !ThemeManager::instance())
        return;

    ThemeManager::instance()->applyThemeToWidget(m_widget);
    onThemeChanged();
    applyThemeColors();
}

void ThemeAwareWidget::onThemeManagerChanged() {
    refreshTheme();
}

void ThemeAwareWidget::onColorsChanged() {
    applyThemeColors();
}

void ThemeAwareWidget::setCustomColors(const QHash<ThemeManager::ColorRole, QColor>& colors) {
    m_customColors = colors;
    applyThemeColors();
}

void ThemeManager::resetToDefaults() {
    // Reset to system theme and clear customizations
    m_fonts.clear();
    m_customThemes.clear();
    setAnimationsEnabled(true);
    setTheme(SystemTheme);
}

// Missing method implementations
bool ThemeManager::loadCustomTheme(const QString& filePath) {
    if (!isValidThemeFile(filePath)) {
        return false;
    }

    ThemeColors colors = loadCustomThemeFromFile(filePath);
    if (colors.name.isEmpty()) {
        return false;
    }

    m_customThemes[colors.name] = colors;
    return true;
}

bool ThemeManager::saveCustomTheme(const QString& filePath, const ThemeColors& colors) {
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject themeObject;
    themeObject["name"] = colors.name;
    themeObject["description"] = colors.description;
    themeObject["isDark"] = colors.isDark;

    QJsonObject colorsObject;
    for (auto it = colors.colors.begin(); it != colors.colors.end(); ++it) {
        colorsObject[QString::number(static_cast<int>(it.key()))] = it.value().name();
    }
    themeObject["colors"] = colorsObject;

    QJsonDocument doc(themeObject);
    file.write(doc.toJson());
    return true;
}

QStringList ThemeManager::availableCustomThemes() const {
    return m_customThemes.keys();
}

void ThemeManager::setCustomColors(const ThemeColors& colors) {
    m_currentColors = colors;
    emit colorsChanged();
}

QFont ThemeManager::getFont(const QString& role) const {
    if (m_fonts.contains(role)) {
        return m_fonts[role];
    }
    return QApplication::font(); // Default font
}

void ThemeManager::setFont(const QString& role, const QFont& font) {
    m_fonts[role] = font;
    emit fontsChanged();
}

QString ThemeManager::getWidgetStyleSheet(const QString& widgetClass) const {
    // Return class-specific stylesheet
    QString classStyleSheet = m_currentStyleSheet;

    // Filter for specific widget class if needed
    if (!widgetClass.isEmpty()) {
        // This would parse the stylesheet and return only rules for the specified class
        // For now, return the full stylesheet
    }

    return classStyleSheet;
}

bool ThemeManager::isValidThemeFile(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        return false;
    }

    // Check if it's a JSON file
    if (fileInfo.suffix().toLower() != "json") {
        return false;
    }

    // Try to parse as JSON
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    return (error.error == QJsonParseError::NoError && doc.isObject());
}

// Missing ThemeManager method
ThemeManager::ThemeColors ThemeManager::loadCustomThemeFromFile(const QString& filePath) {
    ThemeColors themeColors;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return themeColors; // Return empty colors on failure
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);

    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return themeColors; // Return empty colors on failure
    }

    QJsonObject themeObj = doc.object();

    // Load basic theme info
    themeColors.name = themeObj["name"].toString();
    themeColors.description = themeObj["description"].toString();
    themeColors.isDark = themeObj["isDark"].toBool();

    // Load color values (simplified implementation)
    QJsonObject colorsObj = themeObj["colors"].toObject();

    // Map basic colors to ColorRole enum values
    themeColors.colors[WindowBackground] = QColor(colorsObj["background"].toString());
    themeColors.colors[PrimaryText] = QColor(colorsObj["text"].toString());
    themeColors.colors[AccentColor] = QColor(colorsObj["accent"].toString());

    return themeColors;
}

void ThemeManager::applyThemeToAllComponents(QWidget* rootWidget) {
    if (!rootWidget) {
        return;
    }

    // Apply theme to the root widget
    applyThemeToWidget(rootWidget);

    // Find and apply theme to all child widgets recursively
    QList<QWidget*> allWidgets = rootWidget->findChildren<QWidget*>();

    for (QWidget* widget : allWidgets) {
        applyThemeToSpecificComponent(widget);
    }

    // Apply theme to specific Gallery components
    applyThemeToGalleryComponents(rootWidget);
}

void ThemeManager::applyThemeToSpecificComponent(QWidget* widget) {
    if (!widget) {
        return;
    }

    QString className = widget->metaObject()->className();

    // Apply specific styling based on widget type
    if (className == "IconGridWidget") {
        applyIconGridTheme(widget);
    } else if (className == "IconDetailsPanel") {
        applyIconDetailsPanelTheme(widget);
    } else if (className == "SearchWidget") {
        applySearchWidgetTheme(widget);
    } else if (className == "CategoryFilterWidget") {
        applyCategoryFilterTheme(widget);
    } else if (className == "CategorySidebarWidget") {
        applyCategorySidebarTheme(widget);
    } else if (className == "IconPreviewWidget") {
        applyIconPreviewTheme(widget);
    } else if (className == "CodeExampleWidget") {
        applyCodeExampleTheme(widget);
    } else {
        // Apply general theme to standard Qt widgets
        applyStandardWidgetTheme(widget);
    }
}

void ThemeManager::applyIconGridTheme(QWidget* widget) {
    if (!widget)
        return;

    QString gridStyle =
        QString("QWidget {"
                "    background-color: %1;"
                "    border: 1px solid %2;"
                "    border-radius: 8px;"
                "}"
                "QWidget:hover {"
                "    border-color: %3;"
                "    background-color: %4;"
                "}"
                "QWidget[selected=\"true\"] {"
                "    background-color: %5;"
                "    border-color: %6;"
                "}")
            .arg(getColor(GridItemBackground).name(), getColor(GridItemBorder).name(),
                 getColor(GridItemBorderHover).name(), getColor(GridItemBackgroundHover).name(),
                 getColor(GridItemBackgroundSelected).name(),
                 getColor(GridItemBorderSelected).name());

    widget->setStyleSheet(gridStyle);
}

void ThemeManager::applyIconDetailsPanelTheme(QWidget* widget) {
    if (!widget)
        return;

    QString panelStyle =
        QString("QFrame {"
                "    background-color: %1;"
                "    border: 1px solid %2;"
                "    border-radius: 12px;"
                "    padding: 16px;"
                "}"
                "QLabel {"
                "    color: %3;"
                "    background-color: transparent;"
                "}"
                "QTextEdit {"
                "    background-color: %4;"
                "    border: 1px solid %5;"
                "    border-radius: 8px;"
                "    color: %6;"
                "    padding: 8px;"
                "}"
                "QToolButton {"
                "    background-color: %7;"
                "    border: 1px solid %8;"
                "    border-radius: 6px;"
                "    padding: 6px 12px;"
                "    color: %9;"
                "}"
                "QToolButton:hover {"
                "    background-color: %10;"
                "    border-color: %11;"
                "}")
            .arg(getColor(PanelBackground).name(), getColor(BorderColor).name(),
                 getColor(PrimaryText).name(), getColor(InputBackground).name(),
                 getColor(InputBorder).name(), getColor(InputText).name(),
                 getColor(ButtonBackground).name(), getColor(ButtonBackground).name(),
                 getColor(ButtonText).name(), getColor(ButtonBackgroundHover).name(),
                 getColor(BorderColorHover).name());

    widget->setStyleSheet(panelStyle);
}

void ThemeManager::applySearchWidgetTheme(QWidget* widget) {
    if (!widget)
        return;

    QString searchStyle =
        QString("QLineEdit {"
                "    background-color: %1;"
                "    border: 2px solid %2;"
                "    border-radius: 16px;"
                "    padding: 12px 18px;"
                "    font-size: 15px;"
                "    color: %3;"
                "}"
                "QLineEdit:focus {"
                "    border-color: %4;"
                "    background-color: %5;"
                "}"
                "QLineEdit:hover {"
                "    border-color: %6;"
                "}"
                "QComboBox {"
                "    background-color: %7;"
                "    border: 2px solid %8;"
                "    border-radius: 8px;"
                "    padding: 8px 12px;"
                "    color: %9;"
                "}"
                "QComboBox:hover {"
                "    border-color: %10;"
                "}"
                "QComboBox::drop-down {"
                "    border: none;"
                "    background-color: %11;"
                "}")
            .arg(getColor(InputBackground).name(), getColor(InputBorder).name(),
                 getColor(InputText).name(), getColor(InputBorderFocus).name(),
                 getColor(InputBackgroundFocus).name(), getColor(InputBorderHover).name(),
                 getColor(InputBackground).name(), getColor(InputBorder).name(),
                 getColor(InputText).name(), getColor(InputBorderHover).name(),
                 getColor(SurfaceVariant).name());

    widget->setStyleSheet(searchStyle);
}

void ThemeManager::applyCategoryFilterTheme(QWidget* widget) {
    if (!widget)
        return;

    QString filterStyle = QString("QComboBox {"
                                  "    background-color: %1;"
                                  "    border: 2px solid %2;"
                                  "    border-radius: 8px;"
                                  "    padding: 8px 12px;"
                                  "    color: %3;"
                                  "    min-height: 24px;"
                                  "}"
                                  "QComboBox:hover {"
                                  "    border-color: %4;"
                                  "    background-color: %5;"
                                  "}"
                                  "QComboBox::drop-down {"
                                  "    border: none;"
                                  "    background-color: %6;"
                                  "    border-radius: 4px;"
                                  "}"
                                  "QComboBox::down-arrow {"
                                  "    image: url(:/icons/chevron-down.svg);"
                                  "    width: 16px;"
                                  "    height: 16px;"
                                  "}"
                                  "QCheckBox {"
                                  "    color: %7;"
                                  "    spacing: 8px;"
                                  "}"
                                  "QCheckBox::indicator {"
                                  "    width: 18px;"
                                  "    height: 18px;"
                                  "    border: 2px solid %8;"
                                  "    border-radius: 4px;"
                                  "    background-color: %9;"
                                  "}"
                                  "QCheckBox::indicator:checked {"
                                  "    background-color: %10;"
                                  "    border-color: %11;"
                                  "}")
                              .arg(getColor(InputBackground).name(), getColor(InputBorder).name(),
                                   getColor(InputText).name(), getColor(InputBorderHover).name(),
                                   getColor(InputBackgroundHover).name(),
                                   getColor(SurfaceVariant).name(), getColor(PrimaryText).name(),
                                   getColor(BorderColor).name(), getColor(InputBackground).name(),
                                   getColor(AccentColor).name(), getColor(AccentColor).name());

    widget->setStyleSheet(filterStyle);
}

void ThemeManager::applyCategorySidebarTheme(QWidget* widget) {
    if (!widget)
        return;

    QString sidebarStyle = QString("QWidget {"
                                   "    background-color: %1;"
                                   "    border-right: 1px solid %2;"
                                   "}"
                                   "QListWidget {"
                                   "    background-color: transparent;"
                                   "    border: none;"
                                   "    outline: none;"
                                   "}"
                                   "QListWidget::item {"
                                   "    background-color: transparent;"
                                   "    border: none;"
                                   "    border-radius: 8px;"
                                   "    padding: 8px 12px;"
                                   "    margin: 2px;"
                                   "    color: %3;"
                                   "}"
                                   "QListWidget::item:hover {"
                                   "    background-color: %4;"
                                   "}"
                                   "QListWidget::item:selected {"
                                   "    background-color: %5;"
                                   "    color: %6;"
                                   "}"
                                   "QLabel {"
                                   "    color: %7;"
                                   "    font-weight: 500;"
                                   "}")
                               .arg(getColor(SidebarBackground).name(),
                                    getColor(SeparatorColor).name(), getColor(PrimaryText).name(),
                                    getColor(HoverBackground).name(), getColor(AccentColor).name(),
                                    getColor(ButtonText).name(), getColor(SecondaryText).name());

    widget->setStyleSheet(sidebarStyle);
}

void ThemeManager::applyIconPreviewTheme(QWidget* widget) {
    if (!widget)
        return;

    QString previewStyle = QString("QFrame {"
                                   "    background-color: %1;"
                                   "    border: 1px solid %2;"
                                   "    border-radius: 8px;"
                                   "    padding: 12px;"
                                   "}"
                                   "QSlider::groove:horizontal {"
                                   "    background-color: %3;"
                                   "    height: 6px;"
                                   "    border-radius: 3px;"
                                   "}"
                                   "QSlider::handle:horizontal {"
                                   "    background-color: %4;"
                                   "    border: 2px solid %5;"
                                   "    width: 18px;"
                                   "    height: 18px;"
                                   "    border-radius: 9px;"
                                   "    margin: -6px 0;"
                                   "}"
                                   "QSlider::handle:horizontal:hover {"
                                   "    background-color: %6;"
                                   "}"
                                   "QSpinBox {"
                                   "    background-color: %7;"
                                   "    border: 1px solid %8;"
                                   "    border-radius: 4px;"
                                   "    padding: 4px 8px;"
                                   "    color: %9;"
                                   "}"
                                   "QSpinBox:focus {"
                                   "    border-color: %10;"
                                   "}")
                               .arg(getColor(PanelBackground).name(), getColor(BorderColor).name(),
                                    getColor(SurfaceVariant).name(), getColor(AccentColor).name(),
                                    getColor(AccentColor).name(), getColor(AccentColorHover).name(),
                                    getColor(InputBackground).name(), getColor(InputBorder).name(),
                                    getColor(InputText).name(), getColor(InputBorderFocus).name());

    widget->setStyleSheet(previewStyle);
}

void ThemeManager::applyCodeExampleTheme(QWidget* widget) {
    if (!widget)
        return;

    QString codeStyle = QString("QTextEdit {"
                                "    background-color: %1;"
                                "    border: 1px solid %2;"
                                "    border-radius: 8px;"
                                "    color: %3;"
                                "    font-family: 'Consolas', 'Monaco', 'Courier New', monospace;"
                                "    font-size: 13px;"
                                "    padding: 12px;"
                                "}"
                                "QTabWidget::pane {"
                                "    background-color: %4;"
                                "    border: 1px solid %5;"
                                "    border-radius: 8px;"
                                "}"
                                "QTabBar::tab {"
                                "    background-color: %6;"
                                "    border: 1px solid %7;"
                                "    border-bottom: none;"
                                "    border-top-left-radius: 6px;"
                                "    border-top-right-radius: 6px;"
                                "    padding: 8px 16px;"
                                "    color: %8;"
                                "    margin-right: 2px;"
                                "}"
                                "QTabBar::tab:selected {"
                                "    background-color: %9;"
                                "    color: %10;"
                                "}"
                                "QTabBar::tab:hover {"
                                "    background-color: %11;"
                                "}")
                            .arg(getColor(InputBackground).name(), getColor(InputBorder).name(),
                                 getColor(InputText).name(), getColor(PanelBackground).name(),
                                 getColor(BorderColor).name(), getColor(SurfaceVariant).name(),
                                 getColor(BorderColor).name(), getColor(SecondaryText).name(),
                                 getColor(AccentColor).name(), getColor(ButtonText).name(),
                                 getColor(HoverBackground).name());

    widget->setStyleSheet(codeStyle);
}

void ThemeManager::applyStandardWidgetTheme(QWidget* widget) {
    if (!widget)
        return;

    QString className = widget->metaObject()->className();

    if (className == "QPushButton") {
        QString buttonStyle =
            QString("QPushButton {"
                    "    background-color: %1;"
                    "    color: %2;"
                    "    border: none;"
                    "    border-radius: 8px;"
                    "    padding: 10px 20px;"
                    "    font-weight: 500;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: %3;"
                    "}"
                    "QPushButton:pressed {"
                    "    background-color: %4;"
                    "}"
                    "QPushButton:disabled {"
                    "    background-color: %5;"
                    "    color: %6;"
                    "}")
                .arg(getColor(ButtonBackground).name(), getColor(ButtonText).name(),
                     getColor(ButtonBackgroundHover).name(),
                     getColor(ButtonBackgroundPressed).name(),
                     getColor(ButtonBackgroundDisabled).name(),
                     getColor(ButtonTextDisabled).name());
        widget->setStyleSheet(buttonStyle);
    } else if (className == "QLabel") {
        QString labelStyle = QString("QLabel {"
                                     "    color: %1;"
                                     "    background-color: transparent;"
                                     "}")
                                 .arg(getColor(PrimaryText).name());
        widget->setStyleSheet(labelStyle);
    } else if (className == "QLineEdit") {
        QString lineEditStyle =
            QString("QLineEdit {"
                    "    background-color: %1;"
                    "    border: 2px solid %2;"
                    "    border-radius: 8px;"
                    "    padding: 8px 12px;"
                    "    color: %3;"
                    "}"
                    "QLineEdit:focus {"
                    "    border-color: %4;"
                    "    background-color: %5;"
                    "}"
                    "QLineEdit:hover {"
                    "    border-color: %6;"
                    "}")
                .arg(getColor(InputBackground).name(), getColor(InputBorder).name(),
                     getColor(InputText).name(), getColor(InputBorderFocus).name(),
                     getColor(InputBackgroundFocus).name(), getColor(InputBorderHover).name());
        widget->setStyleSheet(lineEditStyle);
    }
}

void ThemeManager::applyThemeToGalleryComponents(QWidget* rootWidget) {
    if (!rootWidget)
        return;

    // Find specific Gallery components and apply enhanced theming
    QList<QWidget*> iconGrids = rootWidget->findChildren<QWidget*>("IconGridWidget");
    for (QWidget* grid : iconGrids) {
        applyIconGridTheme(grid);
    }

    QList<QWidget*> detailsPanels = rootWidget->findChildren<QWidget*>("IconDetailsPanel");
    for (QWidget* panel : detailsPanels) {
        applyIconDetailsPanelTheme(panel);
    }

    QList<QWidget*> searchWidgets = rootWidget->findChildren<QWidget*>("SearchWidget");
    for (QWidget* search : searchWidgets) {
        applySearchWidgetTheme(search);
    }

    QList<QWidget*> categoryFilters = rootWidget->findChildren<QWidget*>("CategoryFilterWidget");
    for (QWidget* filter : categoryFilters) {
        applyCategoryFilterTheme(filter);
    }

    QList<QWidget*> sidebars = rootWidget->findChildren<QWidget*>("CategorySidebarWidget");
    for (QWidget* sidebar : sidebars) {
        applyCategorySidebarTheme(sidebar);
    }

    QList<QWidget*> previews = rootWidget->findChildren<QWidget*>("IconPreviewWidget");
    for (QWidget* preview : previews) {
        applyIconPreviewTheme(preview);
    }

    QList<QWidget*> codeWidgets = rootWidget->findChildren<QWidget*>("CodeExampleWidget");
    for (QWidget* code : codeWidgets) {
        applyCodeExampleTheme(code);
    }
}

QString ThemeManager::getSpacingStyleSheet(SpacingSize padding, SpacingSize margin) {
    return QString("padding: %1px; margin: %2px;")
        .arg(static_cast<int>(padding))
        .arg(static_cast<int>(margin));
}

QString ThemeManager::getPaddingStyleSheet(SpacingSize size) {
    return QString("padding: %1px;").arg(static_cast<int>(size));
}

QString ThemeManager::getMarginStyleSheet(SpacingSize size) {
    return QString("margin: %1px;").arg(static_cast<int>(size));
}

// Modern Typography System Implementation
void ThemeManager::initializeTypographySystem() {
    QString fontFamily = getFontFamilyStack();

    // Initialize display typography
    m_typographyFonts[DisplayLarge] = createFont(fontFamily, 57, FontWeightRegular);
    m_typographyFonts[DisplayMedium] = createFont(fontFamily, 45, FontWeightRegular);
    m_typographyFonts[DisplaySmall] = createFont(fontFamily, 36, FontWeightRegular);

    // Initialize headline typography
    m_typographyFonts[HeadlineLarge] = createFont(fontFamily, 32, FontWeightRegular);
    m_typographyFonts[HeadlineMedium] = createFont(fontFamily, 28, FontWeightRegular);
    m_typographyFonts[HeadlineSmall] = createFont(fontFamily, 24, FontWeightRegular);

    // Initialize title typography
    m_typographyFonts[TitleLarge] = createFont(fontFamily, 22, FontWeightMedium);
    m_typographyFonts[TitleMedium] = createFont(fontFamily, 16, FontWeightMedium);
    m_typographyFonts[TitleSmall] = createFont(fontFamily, 14, FontWeightMedium);

    // Initialize label typography
    m_typographyFonts[LabelLarge] = createFont(fontFamily, 14, FontWeightMedium);
    m_typographyFonts[LabelMedium] = createFont(fontFamily, 12, FontWeightMedium);
    m_typographyFonts[LabelSmall] = createFont(fontFamily, 11, FontWeightMedium);

    // Initialize body typography
    m_typographyFonts[BodyLarge] = createFont(fontFamily, 16, FontWeightRegular);
    m_typographyFonts[BodyMedium] = createFont(fontFamily, 14, FontWeightRegular);
    m_typographyFonts[BodySmall] = createFont(fontFamily, 12, FontWeightRegular);

    // Initialize code typography (monospace)
    QString codeFamily = "Consolas, Monaco, 'Courier New', monospace";
    m_typographyFonts[CodeLarge] = createFont(codeFamily, 14, FontWeightRegular);
    m_typographyFonts[CodeMedium] = createFont(codeFamily, 13, FontWeightRegular);
    m_typographyFonts[CodeSmall] = createFont(codeFamily, 12, FontWeightRegular);
}

QFont ThemeManager::getTypographyFont(TypographyRole role) const {
    if (m_typographyFonts.contains(role)) {
        return m_typographyFonts[role];
    }
    // Fallback to body medium if role not found
    return m_typographyFonts.value(BodyMedium, QApplication::font());
}

void ThemeManager::setTypographyFont(TypographyRole role, const QFont& font) {
    m_typographyFonts[role] = font;
    emit fontsChanged();
}

QString ThemeManager::getTypographyStyleSheet(TypographyRole role) const {
    QFont font = getTypographyFont(role);
    int lineHeight = getOptimalLineHeight(font.pixelSize());

    return QString("font-family: %1;"
                   "font-size: %2px;"
                   "font-weight: %3;"
                   "line-height: %4px;")
        .arg(font.family(), QString::number(font.pixelSize()), QString::number(font.weight()),
             QString::number(lineHeight));
}

void ThemeManager::applyTypography(QWidget* widget, TypographyRole role) {
    if (!widget)
        return;

    QFont font = getTypographyFont(role);
    widget->setFont(font);

    // Apply additional styling based on role
    QString additionalStyle;
    switch (role) {
        case DisplayLarge:
        case DisplayMedium:
        case DisplaySmall:
            additionalStyle = QString("color: %1; margin-bottom: %2px;")
                                  .arg(getColor(PrimaryText).name())
                                  .arg(getSpacing(SpacingLG));
            break;
        case HeadlineLarge:
        case HeadlineMedium:
        case HeadlineSmall:
            additionalStyle = QString("color: %1; margin-bottom: %2px;")
                                  .arg(getColor(PrimaryText).name())
                                  .arg(getSpacing(SpacingMD));
            break;
        case TitleLarge:
        case TitleMedium:
        case TitleSmall:
            additionalStyle = QString("color: %1; margin-bottom: %2px;")
                                  .arg(getColor(PrimaryText).name())
                                  .arg(getSpacing(SpacingSM));
            break;
        case LabelLarge:
        case LabelMedium:
        case LabelSmall:
            additionalStyle = QString("color: %1;").arg(getColor(SecondaryText).name());
            break;
        case BodyLarge:
        case BodyMedium:
        case BodySmall:
            additionalStyle = QString("color: %1; line-height: %2px;")
                                  .arg(getColor(PrimaryText).name())
                                  .arg(getOptimalLineHeight(font.pixelSize()));
            break;
        case CodeLarge:
        case CodeMedium:
        case CodeSmall:
            additionalStyle =
                QString("color: %1; background-color: %2; padding: %3px; border-radius: 4px;")
                    .arg(getColor(SecondaryText).name())
                    .arg(getColor(InputBackground).name())
                    .arg(getSpacing(SpacingXS));
            break;
    }

    if (!additionalStyle.isEmpty()) {
        QString currentStyle = widget->styleSheet();
        widget->setStyleSheet(currentStyle + " " + additionalStyle);
    }
}

// Typography utility methods
QFont ThemeManager::createFont(const QString& family, int pixelSize, FontWeight weight) {
    QFont font(family);
    font.setPixelSize(pixelSize);

    // Convert our FontWeight enum to QFont::Weight
    QFont::Weight qtWeight = QFont::Normal;
    switch (weight) {
        case FontWeightThin:
            qtWeight = QFont::Thin;
            break;
        case FontWeightExtraLight:
            qtWeight = QFont::ExtraLight;
            break;
        case FontWeightLight:
            qtWeight = QFont::Light;
            break;
        case FontWeightRegular:
            qtWeight = QFont::Normal;
            break;
        case FontWeightMedium:
            qtWeight = QFont::Medium;
            break;
        case FontWeightSemiBold:
            qtWeight = QFont::DemiBold;
            break;
        case FontWeightBold:
            qtWeight = QFont::Bold;
            break;
        case FontWeightExtraBold:
            qtWeight = QFont::ExtraBold;
            break;
        case FontWeightBlack:
            qtWeight = QFont::Black;
            break;
    }

    font.setWeight(qtWeight);
    return font;
}

QString ThemeManager::getFontFamilyStack() {
    // Modern system font stack for cross-platform consistency
    return "-apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, 'Helvetica Neue', Arial, "
           "sans-serif";
}

int ThemeManager::getOptimalLineHeight(int fontSize) {
    // Use 1.5x line height for optimal readability
    return static_cast<int>(fontSize * 1.5);
}

QString ThemeManager::getTypographyClassName(TypographyRole role) {
    switch (role) {
        case DisplayLarge:
            return "display-large";
        case DisplayMedium:
            return "display-medium";
        case DisplaySmall:
            return "display-small";
        case HeadlineLarge:
            return "headline-large";
        case HeadlineMedium:
            return "headline-medium";
        case HeadlineSmall:
            return "headline-small";
        case TitleLarge:
            return "title-large";
        case TitleMedium:
            return "title-medium";
        case TitleSmall:
            return "title-small";
        case LabelLarge:
            return "label-large";
        case LabelMedium:
            return "label-medium";
        case LabelSmall:
            return "label-small";
        case BodyLarge:
            return "body-large";
        case BodyMedium:
            return "body-medium";
        case BodySmall:
            return "body-small";
        case CodeLarge:
            return "code-large";
        case CodeMedium:
            return "code-medium";
        case CodeSmall:
            return "code-small";
    }
    return "body-medium";
}

void ThemeManager::applyButtonStyle(QPushButton* button, ButtonVariant variant, ButtonSize size) {
    if (!button)
        return;

    // Get the appropriate class name
    QString className = getButtonClassName(variant, size);

    // Set the class property for CSS styling
    button->setProperty("class", className);

    // Apply the button animation
    applyButtonAnimation(button);

    // Force style refresh
    button->style()->unpolish(button);
    button->style()->polish(button);
    button->update();
}

void ThemeManager::applyButtonAnimation(QPushButton* button) {
    if (!button)
        return;

    // Create property animation for smooth hover effects
    QPropertyAnimation* animation = new QPropertyAnimation(button, "geometry");
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    // Store animation as a property of the button for cleanup
    button->setProperty("hoverAnimation", QVariant::fromValue(animation));

    // Enable mouse tracking for hover effects
    button->setAttribute(Qt::WA_Hover, true);
}

QString ThemeManager::getButtonClassName(ButtonVariant variant, ButtonSize size) const {
    QStringList classes;

    // Add variant class
    switch (variant) {
        case PrimaryButton:
            classes << "primary";
            break;
        case SecondaryButton:
            classes << "secondary";
            break;
        case OutlineButton:
            classes << "outline";
            break;
        case GhostButton:
            classes << "ghost";
            break;
        case IconButton:
            classes << "icon-button";
            break;
        case SuccessButton:
            classes << "success";
            break;
        case WarningButton:
            classes << "warning";
            break;
        case ErrorButton:
            classes << "error";
            break;
        case DefaultButton:
        default:
            // No additional class for default buttons
            break;
    }

    // Add size class
    switch (size) {
        case SmallButton:
            classes << "small";
            break;
        case LargeButton:
            classes << "large";
            break;
        case MediumButton:
        default:
            // No additional class for medium buttons (default)
            break;
    }

    return classes.join(" ");
}

void ThemeManager::applyThemeTransitions() {
    // Enhanced theme transitions with better performance and visual polish
    if (!m_animationsEnabled) {
        applyThemeToApplication();
        return;
    }

    if (QApplication* app = qobject_cast<QApplication*>(QApplication::instance())) {
        // Use a more efficient approach - batch process widgets by type
        QWidgetList topLevelWidgets = app->topLevelWidgets();

        for (QWidget* topLevel : topLevelWidgets) {
            if (topLevel && topLevel->isVisible()) {
                applySmoothThemeTransition(topLevel);
            }
        }
    }
}

void ThemeManager::applySmoothThemeTransition(QWidget* widget) {
    if (!widget)
        return;

    // Create a sophisticated transition effect
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);

    // Smooth fade out, apply theme, then fade back in
    QPropertyAnimation* fadeOut = new QPropertyAnimation(effect, "opacity", this);
    fadeOut->setDuration(200);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.3);
    fadeOut->setEasingCurve(QEasingCurve::InOutCubic);

    QPropertyAnimation* fadeIn = new QPropertyAnimation(effect, "opacity", this);
    fadeIn->setDuration(200);
    fadeIn->setStartValue(0.3);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutCubic);

    // Apply theme during the transition
    connect(fadeOut, &QPropertyAnimation::finished, this, [this, widget, fadeIn]() {
        if (widget) {
            widget->setStyleSheet(m_currentStyleSheet);
            fadeIn->start();
        }
    });

    // Clean up effect after animation
    connect(fadeIn, &QPropertyAnimation::finished, [widget, effect]() {
        if (widget && effect) {
            widget->setGraphicsEffect(nullptr);
            effect->deleteLater();
        }
    });

    fadeOut->start(QPropertyAnimation::DeleteWhenStopped);
}

QString ThemeManager::getEnhancedAnimationStyles() const {
    return QString("* { "
                   "transition: all 0.2s ease-in-out; "
                   "} "
                   "QWidget { "
                   "transition: background-color 0.2s ease-in-out, "
                   "color 0.2s ease-in-out, "
                   "border-color 0.2s ease-in-out; "
                   "} "
                   "QPushButton { "
                   "transition: background-color 0.15s ease-in-out, "
                   "transform 0.1s ease-in-out; "
                   "} "
                   "QPushButton:hover { "
                   "transform: translateY(-1px); "
                   "} "
                   "QLineEdit, QComboBox { "
                   "transition: border-color 0.2s ease-in-out, "
                   "box-shadow 0.2s ease-in-out; "
                   "} "
                   "QLineEdit:focus, QComboBox:focus { "
                   "box-shadow: 0 0 0 3px rgba(100, 149, 237, 0.3); "
                   "}");
}

// Missing method implementations

void ThemeManager::setupThemeTransitions() {
    // Initialize theme transition timer
    m_themeTransitionTimer = new QTimer(this);
    m_themeTransitionTimer->setSingleShot(true);
    m_themeTransitionTimer->setInterval(m_animationDuration);

    // Connect transition timer to apply theme changes
    connect(m_themeTransitionTimer, &QTimer::timeout, this, [this]() {
        applyThemeToApplication();
        emit themeTransitionCompleted();
    });

    qDebug() << "Theme transitions initialized with duration:" << m_animationDuration << "ms";
}

void ThemeManager::setupPerformanceMonitoring() {
    if (!m_performanceMonitoringEnabled) {
        return;
    }

    // Initialize performance monitoring timer
    m_performanceTimer = new QTimer(this);
    m_performanceTimer->setInterval(1000); // Check every second

    connect(m_performanceTimer, &QTimer::timeout, this, [this]() {
        // Monitor theme application performance
        static int themeChangeCount = 0;
        static qint64 lastCheckTime = QDateTime::currentMSecsSinceEpoch();

        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        if (currentTime - lastCheckTime > 5000) { // Every 5 seconds
            if (m_debugMode) {
                qDebug() << "Theme performance: " << themeChangeCount << "changes in last 5s";
            }
            themeChangeCount = 0;
            lastCheckTime = currentTime;
        }
    });

    if (m_performanceMonitoringEnabled) {
        m_performanceTimer->start();
    }

    qDebug() << "Performance monitoring initialized, enabled:" << m_performanceMonitoringEnabled;
}

void ThemeManager::initializeColorSystem() {
    // Initialize color system with enhanced color management
    if (m_dynamicColorGeneration) {
        // Generate dynamic colors based on system preferences
        QColor accentColor = QApplication::palette().highlight().color();

        // Create color variations for different themes
        for (auto theme : {LightTheme, DarkTheme, SystemTheme}) {
            if (!m_themeColors.contains(theme)) {
                continue;
            }

            ThemeColors& colors = m_themeColors[theme];

            // Only apply dynamic accent color if the theme doesn't already have one defined
            // This preserves the distinct accent colors for light (#1976D2) and dark (#90CAF9)
            // themes
            if (!colors.colors.contains(AccentColor) || !colors.colors[AccentColor].isValid()) {
                // Generate accent color variations
                colors.colors[AccentColor] = accentColor;
                colors.colors[AccentColorHover] = accentColor.lighter(110);
                colors.colors[AccentColorPressed] = accentColor.darker(110);
            }
        }
    }

    // Initialize high contrast mode colors if enabled
    if (m_highContrastMode) {
        ThemeColors highContrastColors;
        highContrastColors.colors[WindowBackground] = QColor(0, 0, 0);
        highContrastColors.colors[PanelBackground] = QColor(32, 32, 32);
        highContrastColors.colors[PrimaryText] = QColor(255, 255, 255);
        highContrastColors.colors[SecondaryText] = QColor(200, 200, 200);
        highContrastColors.colors[AccentColor] = QColor(255, 255, 0);
        highContrastColors.colors[PrimaryText] = QColor(255, 255, 255);
        highContrastColors.colors[SecondaryText] = QColor(200, 200, 200);
        highContrastColors.colors[BorderColor] = QColor(128, 128, 128);

        m_themeColors[HighContrastTheme] = highContrastColors;
    }

    qDebug() << "Color system initialized with dynamic generation:" << m_dynamicColorGeneration
             << "high contrast:" << m_highContrastMode;
}

void ThemeManager::initializeAnimationSystem() {
    // Initialize animation system with enhanced capabilities
    m_animationDuration = m_animationsEnabled ? DEFAULT_ANIMATION_DURATION : 0;
    m_transitionOpacity = 1.0;

    // Clear any existing animations
    for (auto it = m_activeAnimations.begin(); it != m_activeAnimations.end(); ++it) {
        if (it.value()) {
            it.value()->stop();
            it.value()->deleteLater();
        }
    }
    m_activeAnimations.clear();

    // Clear opacity effects
    for (auto it = m_opacityEffects.begin(); it != m_opacityEffects.end(); ++it) {
        if (it.value()) {
            it.value()->deleteLater();
        }
    }
    m_opacityEffects.clear();

    qDebug() << "Animation system initialized, animations enabled:" << m_animationsEnabled
             << "duration:" << m_animationDuration << "ms";
}

void ThemeManager::applyInitialThemeWithTransition() {
    if (!m_animationsEnabled) {
        setThemeInternal(m_currentTheme);
        return;
    }

    // Apply initial theme with smooth transition
    QTimer::singleShot(100, this, [this]() {
        // Start with reduced opacity for smooth fade-in
        m_transitionOpacity = 0.0;
        setThemeInternal(m_currentTheme);

        // Animate to full opacity
        QTimer::singleShot(50, this, [this]() {
            m_transitionOpacity = 1.0;
            applyThemeToApplication();
            emit themeApplied(m_currentTheme);
        });
    });

    qDebug() << "Initial theme applied with transition:" << currentThemeName();
}

void ThemeManager::setThemeInternal(Theme theme) {
    if (m_currentTheme == theme && !m_previewMode) {
        return;
    }

    Theme previousTheme = m_currentTheme;
    m_currentTheme = theme;

    // Load theme data
    loadThemeColors(theme);
    loadThemeStyleSheet(theme);

    // Update application palette and icon theme
    updateApplicationPalette();
    updateIconTheme();

    // Apply theme to application
    applyThemeToApplication();

    // Emit signals
    emit themeChanged(theme);
    emit colorsChanged();

    if (m_debugMode) {
        qDebug() << "Theme changed internally from" << static_cast<int>(previousTheme) << "to"
                 << static_cast<int>(theme) << "(" << currentThemeName() << ")";
    }
}
