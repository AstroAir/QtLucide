/**
 * QtLucide Gallery Application - Theme Manager Implementation
 */

#include "ThemeManager.h"
#include <QStandardPaths>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonParseError>
#include <QCoreApplication>
#include <QStyleHints>

// Static instance
ThemeManager* ThemeManager::s_instance = nullptr;

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme(SystemTheme)
    , m_settings(new QSettings(this))
    , m_systemThemeTimer(new QTimer(this))
    , m_lastSystemDarkMode(false)
    , m_animationsEnabled(true)
    , m_lucide(nullptr)
{
    // Set up singleton
    if (!s_instance) {
        s_instance = this;
    }

    // Initialize paths
    m_customThemesPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/themes";
    QDir().mkpath(m_customThemesPath);

    // Initialize themes
    initializeThemes();
    setupSystemThemeDetection();

    // Load settings
    loadSettings();

    qDebug() << "ThemeManager initialized with theme:" << currentThemeName();
}

ThemeManager::~ThemeManager()
{
    saveSettings();
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

ThemeManager* ThemeManager::instance()
{
    return s_instance;
}

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme == theme) return;

    Theme oldTheme = m_currentTheme;
    m_currentTheme = theme;

    // Load theme data
    loadThemeColors(theme);
    loadThemeStyleSheet(theme);

    // Update application
    updateApplicationPalette();
    updateIconTheme();

    // Save settings
    saveSettings();

    // Emit signals
    emit themeChanged(theme);
    emit colorsChanged();
    emit styleSheetChanged();

    qDebug() << "Theme changed from" << oldTheme << "to" << theme;
}

QString ThemeManager::currentThemeName() const
{
    switch (m_currentTheme) {
        case SystemTheme: return "System";
        case LightTheme: return "Light";
        case DarkTheme: return "Dark";
        case CustomTheme: return "Custom";
        default: return "Unknown";
    }
}

bool ThemeManager::isDarkTheme() const
{
    if (m_currentTheme == SystemTheme) {
        return isSystemDarkMode();
    }
    return m_currentColors.isDark;
}

bool ThemeManager::isSystemDarkMode() const
{
    // Check system dark mode
    QStyleHints* hints = QApplication::styleHints();
    return hints->colorScheme() == Qt::ColorScheme::Dark;
}

QColor ThemeManager::getColor(ColorRole role) const
{
    return m_currentColors.colors.value(role, QColor("#000000"));
}

void ThemeManager::setColor(ColorRole role, const QColor& color)
{
    m_currentColors.colors[role] = color;
    emit colorsChanged();
}

ThemeManager::ThemeColors ThemeManager::getCurrentColors() const
{
    return m_currentColors;
}

QString ThemeManager::getStyleSheet() const
{
    return m_currentStyleSheet;
}

void ThemeManager::applyThemeToWidget(QWidget* widget)
{
    if (!widget) return;

    widget->setStyleSheet(m_currentStyleSheet);

    if (m_animationsEnabled) {
        animateThemeChange(widget);
    }
}

void ThemeManager::applyThemeToApplication()
{
    if (QApplication* app = qobject_cast<QApplication*>(QApplication::instance())) {
        app->setStyleSheet(m_currentStyleSheet);
    }
    updateApplicationPalette();
}

void ThemeManager::animateThemeChange(QWidget* widget, int duration)
{
    if (!widget || !m_animationsEnabled) return;

    // Create opacity effect if needed
    if (!m_opacityEffects.contains(widget)) {
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
        m_opacityEffects[widget] = effect;
    }

    // Animate opacity
    fadeWidget(widget, 0.7, 1.0, duration);
}

void ThemeManager::loadSettings()
{
    m_settings->beginGroup(SETTINGS_GROUP);

    int themeValue = m_settings->value(THEME_KEY, static_cast<int>(SystemTheme)).toInt();
    m_animationsEnabled = m_settings->value(ANIMATIONS_KEY, true).toBool();

    m_settings->endGroup();

    // Apply loaded theme
    setTheme(static_cast<Theme>(themeValue));
}

void ThemeManager::saveSettings()
{
    m_settings->beginGroup(SETTINGS_GROUP);

    m_settings->setValue(THEME_KEY, static_cast<int>(m_currentTheme));
    m_settings->setValue(ANIMATIONS_KEY, m_animationsEnabled);

    m_settings->endGroup();
    m_settings->sync();
}

void ThemeManager::onSystemThemeChanged()
{
    if (m_currentTheme == SystemTheme) {
        loadThemeColors(SystemTheme);
        loadThemeStyleSheet(SystemTheme);
        updateApplicationPalette();
        emit themeChanged(SystemTheme);
        emit colorsChanged();
    }
}

void ThemeManager::refreshTheme()
{
    Theme current = m_currentTheme;
    m_currentTheme = static_cast<Theme>(-1); // Force refresh
    setTheme(current);
}

void ThemeManager::checkSystemTheme()
{
    bool currentSystemDark = isSystemDarkMode();
    if (currentSystemDark != m_lastSystemDarkMode) {
        m_lastSystemDarkMode = currentSystemDark;
        emit systemThemeDetected(currentSystemDark);

        if (m_currentTheme == SystemTheme) {
            onSystemThemeChanged();
        }
    }
}

void ThemeManager::initializeThemes()
{
    // Initialize built-in themes
    m_themeColors[LightTheme] = loadLightTheme();
    m_themeColors[DarkTheme] = loadDarkTheme();
    m_themeColors[SystemTheme] = loadSystemTheme();

    // Load stylesheets
    m_themeStyleSheets[LightTheme] = loadStyleSheetFromFile(":/styles/modern-theme.qss");
    m_themeStyleSheets[DarkTheme] = loadStyleSheetFromFile(":/styles/dark-theme.qss");
    m_themeStyleSheets[SystemTheme] = isSystemDarkMode() ?
        m_themeStyleSheets[DarkTheme] : m_themeStyleSheets[LightTheme];
}

void ThemeManager::setupSystemThemeDetection()
{
    m_lastSystemDarkMode = isSystemDarkMode();

    connect(m_systemThemeTimer, &QTimer::timeout, this, &ThemeManager::checkSystemTheme);
    m_systemThemeTimer->start(SYSTEM_THEME_CHECK_INTERVAL);
}

void ThemeManager::loadThemeColors(Theme theme)
{
    if (m_themeColors.contains(theme)) {
        m_currentColors = m_themeColors[theme];
    } else {
        // Fallback to light theme
        m_currentColors = m_themeColors[LightTheme];
    }
}

void ThemeManager::loadThemeStyleSheet(Theme theme)
{
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

ThemeManager::ThemeColors ThemeManager::loadLightTheme()
{
    ThemeColors colors;
    colors.name = "Light";
    colors.description = "Modern light theme";
    colors.isDark = false;

    // Background colors
    colors.colors[WindowBackground] = QColor("#f8f9fa");
    colors.colors[PanelBackground] = QColor("#ffffff");
    colors.colors[SidebarBackground] = QColor("#f8f9fa");
    colors.colors[ContentBackground] = QColor("#ffffff");

    // Text colors
    colors.colors[PrimaryText] = QColor("#2c3e50");
    colors.colors[SecondaryText] = QColor("#7f8c8d");
    colors.colors[DisabledText] = QColor("#bdc3c7");

    // Accent colors
    colors.colors[AccentColor] = QColor("#3498db");
    colors.colors[AccentColorHover] = QColor("#2980b9");
    colors.colors[AccentColorPressed] = QColor("#21618c");

    // Border colors
    colors.colors[BorderColor] = QColor("#e9ecef");
    colors.colors[BorderColorHover] = QColor("#bdc3c7");
    colors.colors[BorderColorFocus] = QColor("#3498db");

    // State colors
    colors.colors[SuccessColor] = QColor("#27ae60");
    colors.colors[WarningColor] = QColor("#f39c12");
    colors.colors[ErrorColor] = QColor("#e74c3c");
    colors.colors[InfoColor] = QColor("#3498db");

    // Selection colors
    colors.colors[SelectionBackground] = QColor("#3498db");
    colors.colors[SelectionText] = QColor("#ffffff");

    // Hover colors
    colors.colors[HoverBackground] = QColor("#ecf0f1");
    colors.colors[HoverText] = QColor("#2c3e50");

    return colors;
}

ThemeManager::ThemeColors ThemeManager::loadDarkTheme()
{
    ThemeColors colors;
    colors.name = "Dark";
    colors.description = "Modern dark theme";
    colors.isDark = true;

    // Background colors
    colors.colors[WindowBackground] = QColor("#1e1e1e");
    colors.colors[PanelBackground] = QColor("#2b2b2b");
    colors.colors[SidebarBackground] = QColor("#252525");
    colors.colors[ContentBackground] = QColor("#2b2b2b");

    // Text colors
    colors.colors[PrimaryText] = QColor("#e8e8e8");
    colors.colors[SecondaryText] = QColor("#b0b0b0");
    colors.colors[DisabledText] = QColor("#888888");

    // Accent colors
    colors.colors[AccentColor] = QColor("#3498db");
    colors.colors[AccentColorHover] = QColor("#2980b9");
    colors.colors[AccentColorPressed] = QColor("#21618c");

    // Border colors
    colors.colors[BorderColor] = QColor("#404040");
    colors.colors[BorderColorHover] = QColor("#606060");
    colors.colors[BorderColorFocus] = QColor("#3498db");

    // State colors
    colors.colors[SuccessColor] = QColor("#27ae60");
    colors.colors[WarningColor] = QColor("#f39c12");
    colors.colors[ErrorColor] = QColor("#e74c3c");
    colors.colors[InfoColor] = QColor("#3498db");

    // Selection colors
    colors.colors[SelectionBackground] = QColor("#3498db");
    colors.colors[SelectionText] = QColor("#ffffff");

    // Hover colors
    colors.colors[HoverBackground] = QColor("#404040");
    colors.colors[HoverText] = QColor("#e8e8e8");

    return colors;
}

ThemeManager::ThemeColors ThemeManager::loadSystemTheme()
{
    // Return appropriate theme based on system
    return isSystemDarkMode() ? loadDarkTheme() : loadLightTheme();
}

QString ThemeManager::loadStyleSheetFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to load stylesheet from:" << filePath;
        return QString();
    }

    QTextStream stream(&file);
    return stream.readAll();
}

QString ThemeManager::processStyleSheetVariables(const QString& styleSheet)
{
    QString processed = styleSheet;

    // Replace color variables with actual colors
    processed = replaceColorVariables(processed, m_currentColors);

    return processed;
}

QString ThemeManager::replaceColorVariables(const QString& styleSheet, const ThemeColors& colors)
{
    QString result = styleSheet;

    // Define color variable mappings
    QHash<QString, ColorRole> colorVars = {
        {"@window-bg", WindowBackground},
        {"@panel-bg", PanelBackground},
        {"@sidebar-bg", SidebarBackground},
        {"@content-bg", ContentBackground},
        {"@primary-text", PrimaryText},
        {"@secondary-text", SecondaryText},
        {"@disabled-text", DisabledText},
        {"@accent-color", AccentColor},
        {"@accent-hover", AccentColorHover},
        {"@accent-pressed", AccentColorPressed},
        {"@border-color", BorderColor},
        {"@border-hover", BorderColorHover},
        {"@border-focus", BorderColorFocus},
        {"@success-color", SuccessColor},
        {"@warning-color", WarningColor},
        {"@error-color", ErrorColor},
        {"@info-color", InfoColor},
        {"@selection-bg", SelectionBackground},
        {"@selection-text", SelectionText},
        {"@hover-bg", HoverBackground},
        {"@hover-text", HoverText}
    };

    // Replace variables with actual color values
    for (auto it = colorVars.begin(); it != colorVars.end(); ++it) {
        QColor color = colors.colors.value(it.value(), QColor("#000000"));
        result.replace(it.key(), color.name());
    }

    return result;
}

void ThemeManager::updateApplicationPalette()
{
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

void ThemeManager::updateIconTheme()
{
    // Update icon colors based on theme
    // This would integrate with QtLucide to set appropriate icon colors
    QString iconColor = isDarkTheme() ? "#e8e8e8" : "#2c3e50";

    // Integrate with QtLucide icon color settings
    if (m_lucide) {
        m_lucide->setDefaultOption("color", QColor(iconColor));
        emit iconColorChanged(QColor(iconColor));
    }
}

void ThemeManager::fadeWidget(QWidget* widget, qreal startOpacity, qreal endOpacity, int duration)
{
    if (!widget || !m_animationsEnabled) return;

    QGraphicsOpacityEffect* effect = m_opacityEffects.value(widget);
    if (!effect) return;

    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity", this);
    animation->setDuration(duration);
    animation->setStartValue(startOpacity);
    animation->setEndValue(endOpacity);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(animation, &QPropertyAnimation::finished, this, &ThemeManager::onAnimationFinished);

    m_activeAnimations[widget] = animation;
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void ThemeManager::onAnimationFinished()
{
    QPropertyAnimation* animation = qobject_cast<QPropertyAnimation*>(sender());
    if (!animation) return;

    // Remove from active animations
    for (auto it = m_activeAnimations.begin(); it != m_activeAnimations.end(); ++it) {
        if (it.value() == animation) {
            m_activeAnimations.erase(it);
            break;
        }
    }
}

QString ThemeManager::getThemeResourcePath(Theme theme)
{
    switch (theme) {
        case LightTheme: return ":/styles/modern-theme.qss";
        case DarkTheme: return ":/styles/dark-theme.qss";
        case SystemTheme: {
            // Check system dark mode statically
            QStyleHints* hints = QApplication::styleHints();
            bool isDark = hints->colorScheme() == Qt::ColorScheme::Dark;
            return isDark ? ":/styles/dark-theme.qss" : ":/styles/modern-theme.qss";
        }
        default: return ":/styles/modern-theme.qss";
    }
}

// ThemeAwareWidget implementation
ThemeAwareWidget::ThemeAwareWidget(QWidget* widget, QObject* parent)
    : QObject(parent), m_widget(widget)
{
    if (ThemeManager::instance()) {
        connect(ThemeManager::instance(), &ThemeManager::themeChanged,
                this, &ThemeAwareWidget::onThemeManagerChanged);
        connect(ThemeManager::instance(), &ThemeManager::colorsChanged,
                this, &ThemeAwareWidget::onColorsChanged);
    }
}

ThemeAwareWidget::~ThemeAwareWidget() = default;

void ThemeAwareWidget::setThemeClass(const QString& themeClass)
{
    m_themeClass = themeClass;
    refreshTheme();
}

void ThemeAwareWidget::refreshTheme()
{
    if (!m_widget || !ThemeManager::instance()) return;

    ThemeManager::instance()->applyThemeToWidget(m_widget);
    onThemeChanged();
    applyThemeColors();
}

void ThemeAwareWidget::onThemeManagerChanged()
{
    refreshTheme();
}

void ThemeAwareWidget::onColorsChanged()
{
    applyThemeColors();
}

void ThemeAwareWidget::setCustomColors(const QHash<ThemeManager::ColorRole, QColor>& colors)
{
    m_customColors = colors;
    applyThemeColors();
}

void ThemeManager::resetToDefaults()
{
    // Reset to system theme and clear customizations
    m_fonts.clear();
    m_customThemes.clear();
    setAnimationsEnabled(true);
    setTheme(SystemTheme);
}

// Missing method implementations
bool ThemeManager::loadCustomTheme(const QString& filePath)
{
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

bool ThemeManager::saveCustomTheme(const QString& filePath, const ThemeColors& colors)
{
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

QStringList ThemeManager::availableCustomThemes() const
{
    return m_customThemes.keys();
}

void ThemeManager::setCustomColors(const ThemeColors& colors)
{
    m_currentColors = colors;
    emit colorsChanged();
}

QFont ThemeManager::getFont(const QString& role) const
{
    if (m_fonts.contains(role)) {
        return m_fonts[role];
    }
    return QApplication::font(); // Default font
}

void ThemeManager::setFont(const QString& role, const QFont& font)
{
    m_fonts[role] = font;
    emit fontsChanged();
}

QString ThemeManager::getWidgetStyleSheet(const QString& widgetClass) const
{
    // Return class-specific stylesheet
    QString classStyleSheet = m_currentStyleSheet;

    // Filter for specific widget class if needed
    if (!widgetClass.isEmpty()) {
        // This would parse the stylesheet and return only rules for the specified class
        // For now, return the full stylesheet
    }

    return classStyleSheet;
}

bool ThemeManager::isValidThemeFile(const QString& filePath)
{
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
