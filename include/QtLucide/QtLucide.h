/**
 * QtLucide - use Lucide icons in your Qt Application
 *
 * MIT Licensed
 *
 * Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef QTLUCIDE_H
#define QTLUCIDE_H

#include <QIcon>
#include <QObject>
#include <QVariantMap>
#include <QHash>
#include <QString>

#include "QtLucideIconPainter.h"
#include "QtLucideEnums.h"

namespace lucide {

class QtLucideIconEngine;

/**
 * @brief The main class for managing Lucide icons
 * 
 * This class provides a QtAwesome-compatible API for using Lucide icons
 * in Qt applications. Icons are rendered from embedded SVG data with
 * support for color customization and styling.
 */
class QtLucide : public QObject
{
    Q_OBJECT

public:
    explicit QtLucide(QObject* parent = nullptr);
    virtual ~QtLucide();

    /**
     * @brief Initialize the QtLucide system
     * @return true if initialization was successful
     */
    virtual bool initLucide();

    /**
     * @brief Set a default option for icon rendering
     * @param name Option name (e.g., "color", "scale-factor")
     * @param value Option value
     */
    void setDefaultOption(const QString& name, const QVariant& value);

    /**
     * @brief Get a default option value
     * @param name Option name
     * @return Option value
     */
    QVariant defaultOption(const QString& name) const;

    /**
     * @brief Create an icon from an icon identifier
     * @param iconId Icon identifier from lucide::Icons enum
     * @param options Custom options for this icon
     * @return QIcon instance
     */
    QIcon icon(Icons iconId, const QVariantMap& options = QVariantMap());

    /**
     * @brief Create an icon from a string name
     * @param name Icon name (e.g., "activity", "alert-circle")
     * @param options Custom options for this icon
     * @return QIcon instance
     */
    QIcon icon(const QString& name, const QVariantMap& options = QVariantMap());

    /**
     * @brief Create an icon using a custom painter
     * @param painter Custom icon painter
     * @param options Custom options for this icon
     * @return QIcon instance
     */
    QIcon icon(QtLucideIconPainter* painter, const QVariantMap& options = QVariantMap());

    /**
     * @brief Register a custom icon painter
     * @param name Name for the custom icon
     * @param painter Custom painter instance
     */
    void give(const QString& name, QtLucideIconPainter* painter);

    /**
     * @brief Get the SVG data for an icon
     * @param iconId Icon identifier
     * @return SVG data as QByteArray
     */
    QByteArray svgData(Icons iconId) const;

    /**
     * @brief Get the SVG data for an icon by name
     * @param name Icon name
     * @return SVG data as QByteArray
     */
    QByteArray svgData(const QString& name) const;

    /**
     * @brief Get all available icon names
     * @return List of icon names
     */
    QStringList availableIcons() const;

Q_SIGNALS:
    /**
     * @brief Emitted when default options are reset
     */
    void defaultOptionsReset();

public Q_SLOTS:
    /**
     * @brief Reset default options to system defaults
     */
    void resetDefaultOptions();

private:
    void initializeIconMap();
    Icons stringToIconId(const QString& name) const;
    QString iconIdToString(Icons iconId) const;

    QHash<QString, Icons> m_nameToIconMap;
    QHash<Icons, QString> m_iconToNameMap;
    QHash<QString, QtLucideIconPainter*> m_customPainters;
    QVariantMap m_defaultOptions;
    QtLucideIconPainter* m_svgIconPainter;
    bool m_initialized;
};

} // namespace lucide

#endif // QTLUCIDE_H
