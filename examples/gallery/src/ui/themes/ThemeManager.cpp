/**
 * @file ThemeManager.cpp
 * @brief Implementation of the ThemeManager class
 * @details Handles theme loading, stylesheet application, and theme switching logic.
 */

#include "ThemeManager.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QPalette>
#include <QStyle>

namespace gallery {

ThemeManager::ThemeManager(QObject* parent)
    : QObject(parent), m_currentTheme(ThemeMode::Dark) {
    // Initialize with dark theme by default
}

ThemeManager::~ThemeManager() = default;

void ThemeManager::setTheme(ThemeMode mode) {
    // Determine which theme to actually apply
    ThemeMode themeToApply = mode;
    if (mode == ThemeMode::System) {
        themeToApply = detectSystemTheme();
    }

    // Only apply if different from current
    if (m_currentTheme == themeToApply && mode != ThemeMode::System) {
        return;
    }

    // Apply the stylesheet
    if (applyStylesheet(mode)) {
        m_currentTheme = themeToApply;
        emit themeChanged(mode);
    }
}

ThemeMode ThemeManager::currentTheme() const {
    return m_currentTheme;
}

bool ThemeManager::applyStylesheet(ThemeMode mode) {
    QString resourcePath;

    switch (mode) {
        case ThemeMode::Dark:
            resourcePath = ":/styles/dark-theme.qss";
            break;
        case ThemeMode::Light:
            resourcePath = ":/styles/light-theme.qss";
            break;
        case ThemeMode::System:
            // For system mode, detect and apply accordingly
            resourcePath = detectSystemTheme() == ThemeMode::Dark
                               ? ":/styles/dark-theme.qss"
                               : ":/styles/light-theme.qss";
            break;
    }

    QString stylesheet = loadStylesheet(resourcePath);
    if (stylesheet.isEmpty()) {
        qWarning() << "Failed to load stylesheet:" << resourcePath;
        return false;
    }

    if (auto app = qApp) {
        app->setStyleSheet(stylesheet);
        return true;
    }

    return false;
}

QString ThemeManager::loadStylesheet(const QString& resourcePath) const {
    QFile styleFile(resourcePath);
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open stylesheet resource:" << resourcePath;
        return QString();
    }

    return QString::fromUtf8(styleFile.readAll());
}

ThemeMode ThemeManager::detectSystemTheme() const {
    // Check system palette to determine if system is in dark mode
    // Use palette brightness as an indicator
    const QPalette systemPalette = QApplication::palette();
    const QColor backgroundColor = systemPalette.color(QPalette::Window);

    // If background is dark (luminance < 128), return Dark, else Light
    return backgroundColor.lightness() < 128 ? ThemeMode::Dark : ThemeMode::Light;
}

} // namespace gallery
