/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#include "QtLucide/QtLucide.h"
#include "QtLucide/QtLucideIconEngine.h"
#include "QtLucide/QtLucideStrings.h"

#include <QApplication>
#include <QPalette>
#include <QDebug>
#include <QFile>
#include <QDir>

namespace lucide {

QtLucide::QtLucide(QObject* parent)
    : QObject(parent)
    , m_svgIconPainter(nullptr)
    , m_initialized(false)
{
    // Initialize default options
    resetDefaultOptions();
}

QtLucide::~QtLucide()
{
    delete m_svgIconPainter;
    
    // Clean up custom painters
    for (auto painter : m_customPainters) {
        delete painter;
    }
}

bool QtLucide::initLucide()
{
    if (m_initialized) {
        return true;
    }

    // Initialize icon mappings
    initializeIconMap();
    
    // Create the SVG icon painter
    m_svgIconPainter = new QtLucideSvgIconPainter();
    
    m_initialized = true;
    
    qDebug() << "QtLucide initialized with" << ICON_COUNT << "icons";
    return true;
}

void QtLucide::setDefaultOption(const QString& name, const QVariant& value)
{
    m_defaultOptions[name] = value;
}

QVariant QtLucide::defaultOption(const QString& name) const
{
    return m_defaultOptions.value(name);
}

QIcon QtLucide::icon(Icons iconId, const QVariantMap& options)
{
    if (!m_initialized) {
        qWarning() << "QtLucide not initialized. Call initLucide() first.";
        return QIcon();
    }

    // Merge default options with provided options
    QVariantMap mergedOptions = m_defaultOptions;
    for (auto it = options.begin(); it != options.end(); ++it) {
        mergedOptions[it.key()] = it.value();
    }

    // Store the icon ID for the painter
    mergedOptions["iconId"] = static_cast<int>(iconId);

    return QIcon(new QtLucideIconEngine(this, m_svgIconPainter, mergedOptions));
}

QIcon QtLucide::icon(const QString& name, const QVariantMap& options)
{
    // Check for custom painters first
    if (m_customPainters.contains(name)) {
        QVariantMap mergedOptions = m_defaultOptions;
        for (auto it = options.begin(); it != options.end(); ++it) {
            mergedOptions[it.key()] = it.value();
        }
        return QIcon(new QtLucideIconEngine(this, m_customPainters[name], mergedOptions));
    }

    // Convert name to icon ID
    Icons iconId = stringToIconId(name);
    if (iconId == static_cast<Icons>(-1)) {
        qWarning() << "Unknown icon name:" << name;
        return QIcon();
    }

    return icon(iconId, options);
}

QIcon QtLucide::icon(QtLucideIconPainter* painter, const QVariantMap& options)
{
    if (!painter) {
        qWarning() << "Null painter provided to QtLucide::icon()";
        return QIcon();
    }

    QVariantMap mergedOptions = m_defaultOptions;
    for (auto it = options.begin(); it != options.end(); ++it) {
        mergedOptions[it.key()] = it.value();
    }

    return QIcon(new QtLucideIconEngine(this, painter, mergedOptions));
}

void QtLucide::give(const QString& name, QtLucideIconPainter* painter)
{
    if (m_customPainters.contains(name)) {
        delete m_customPainters[name];
    }
    m_customPainters[name] = painter;
}

QByteArray QtLucide::svgData(Icons iconId) const
{
    QString iconName = iconIdToString(iconId);
    if (iconName.isEmpty()) {
        return QByteArray();
    }
    
    return svgData(iconName);
}

QByteArray QtLucide::svgData(const QString& name) const
{
    QString resourcePath = QString(":/lucide/%1").arg(name);
    QFile file(resourcePath);
    
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load SVG data for icon:" << name;
        return QByteArray();
    }
    
    return file.readAll();
}

QStringList QtLucide::availableIcons() const
{
    return m_nameToIconMap.keys();
}

void QtLucide::resetDefaultOptions()
{
    m_defaultOptions.clear();
    
    // Set default colors based on application palette
    QPalette palette = QApplication::palette();
    
    m_defaultOptions["color"] = palette.color(QPalette::Normal, QPalette::Text);
    m_defaultOptions["color-disabled"] = palette.color(QPalette::Disabled, QPalette::Text);
    m_defaultOptions["color-active"] = palette.color(QPalette::Active, QPalette::Text);
    m_defaultOptions["color-selected"] = palette.color(QPalette::Active, QPalette::Text);
    
    m_defaultOptions["scale-factor"] = 0.9;
    
    emit defaultOptionsReset();
}

void QtLucide::initializeIconMap()
{
    m_nameToIconMap = STRING_TO_ICON_MAP;
    m_iconToNameMap = ICON_TO_STRING_MAP;
}

Icons QtLucide::stringToIconId(const QString& name) const
{
    return m_nameToIconMap.value(name, static_cast<Icons>(-1));
}

QString QtLucide::iconIdToString(Icons iconId) const
{
    return m_iconToNameMap.value(iconId, QString());
}

} // namespace lucide
