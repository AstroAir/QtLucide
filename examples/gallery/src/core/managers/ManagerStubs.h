/**
 * QtLucide Gallery Application - Manager Stubs
 *
 * Provides stub implementations and forward declarations for manager classes
 * used throughout the gallery application. This file helps resolve circular
 * dependencies and provides minimal implementations for testing.
 */

#ifndef MANAGERSTUBS_H
#define MANAGERSTUBS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

// Forward declaration for ImportDataType (defined in ImportDialog.h)
enum class ImportDataType;

// Forward declarations for existing managers
class IconMetadataManager;
class ImageMetadataManager;
class ContentManager;

// Only declare if not already defined
#ifndef FAVORITESMANAGER_H
class FavoritesManager;
#endif

#ifndef THEMEMANAGER_H
class ThemeManager;
#endif

/**
 * @brief Stub implementation for settings management
 */
class SettingsManager : public QObject {
    Q_OBJECT

public:
    explicit SettingsManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~SettingsManager() = default;

    // Basic settings interface
    virtual QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const {
        Q_UNUSED(key);
        return defaultValue;
    }
    virtual void setValue(const QString& key, const QVariant& value) {
        Q_UNUSED(key);
        Q_UNUSED(value);
    }
    virtual void remove(const QString& key) { Q_UNUSED(key); }
    virtual bool contains(const QString& key) const {
        Q_UNUSED(key);
        return false;
    }

    // Static constants for compatibility
    static const QString EXPORT_VERSION;
    static const QString EXPORT_FORMAT_VERSION;

signals:
    void settingChanged(const QString& key, const QVariant& value);
    void importProgress(const QString& message);
    void exportProgress(const QString& message);
    void backupCreated(const QString& path);
    void settingsImported(int dataType);
};

// ThemeManager is defined in ui/themes/ThemeManager.h

/**
 * @brief Stub implementation for export management
 */
class ExportManager : public QObject {
    Q_OBJECT

public:
    explicit ExportManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ExportManager() = default;

    virtual bool exportIcons(const QStringList& iconNames, const QString& outputPath) {
        Q_UNUSED(iconNames);
        Q_UNUSED(outputPath);
        return true;
    }
    virtual bool exportIcon(const QString& iconName, const QString& outputPath) {
        Q_UNUSED(iconName);
        Q_UNUSED(outputPath);
        return true;
    }

signals:
    void exportStarted();
    void exportFinished(bool success);
    void exportProgress(int percentage);
};

/**
 * @brief Stub implementation for import management
 */
class ImportManager : public QObject {
    Q_OBJECT

public:
    explicit ImportManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ImportManager() = default;

    virtual bool importIcons(const QString& sourcePath) {
        Q_UNUSED(sourcePath);
        return true;
    }
    virtual QStringList supportedFormats() const {
        return QStringList() << "svg" << "png" << "jpg";
    }

signals:
    void importStarted();
    void importFinished(bool success);
    void importProgress(int percentage);
};

/**
 * @brief Stub implementation for plugin management
 */
class PluginManager : public QObject {
    Q_OBJECT

public:
    explicit PluginManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~PluginManager() = default;

    virtual bool loadPlugin(const QString& pluginPath) {
        Q_UNUSED(pluginPath);
        return false;
    }
    virtual void unloadPlugin(const QString& pluginName) { Q_UNUSED(pluginName); }
    virtual QStringList availablePlugins() const { return QStringList(); }

signals:
    void pluginLoaded(const QString& pluginName);
    void pluginUnloaded(const QString& pluginName);
};

/**
 * @brief Stub implementation for update management
 */
class UpdateManager : public QObject {
    Q_OBJECT

public:
    explicit UpdateManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~UpdateManager() = default;

    virtual void checkForUpdates() {}
    virtual bool hasUpdates() const { return false; }
    virtual QString latestVersion() const { return "1.0.0"; }

signals:
    void updateAvailable(const QString& version);
    void updateCheckFinished();
};

// FavoritesManager is defined in FavoritesManager.h

/**
 * @brief Stub implementation for export preset management
 */
class ExportPresetManager : public QObject {
    Q_OBJECT

public:
    explicit ExportPresetManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ExportPresetManager() = default;

    virtual QStringList getPresets() const { return QStringList(); }
    virtual void savePreset(const QString& name, const QVariantMap& settings) {
        Q_UNUSED(name);
        Q_UNUSED(settings);
    }
    virtual QVariantMap loadPreset(const QString& name) const {
        Q_UNUSED(name);
        return QVariantMap();
    }
    virtual void deletePreset(const QString& name) { Q_UNUSED(name); }

signals:
    void presetSaved(const QString& name);
    void presetLoaded(const QString& name);
    void presetDeleted(const QString& name);
};

/**
 * @brief Stub implementation for cloud sync management
 */
class CloudSyncManager : public QObject {
    Q_OBJECT

public:
    explicit CloudSyncManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~CloudSyncManager() = default;

    virtual bool isConnected() const { return false; }
    virtual void connect() {}
    virtual void disconnect() {}
    virtual void uploadFile(const QString& filePath) { Q_UNUSED(filePath); }
    virtual void downloadFile(const QString& fileName) { Q_UNUSED(fileName); }

signals:
    void connected();
    void disconnected();
    void uploadProgress(int percentage);
    void uploadFinished(bool success, const QString& url);
    void downloadProgress(int percentage);
    void downloadFinished(bool success, const QString& filePath);
};

/**
 * @brief Stub implementation for user profile management
 */
class UserProfileManager : public QObject {
    Q_OBJECT

public:
    explicit UserProfileManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~UserProfileManager() = default;

    virtual QString currentProfile() const { return "default"; }
    virtual void setCurrentProfile(const QString& profile) { Q_UNUSED(profile); }
    virtual QStringList availableProfiles() const { return QStringList() << "default"; }
    virtual void createProfile(const QString& name) { Q_UNUSED(name); }
    virtual void deleteProfile(const QString& name) { Q_UNUSED(name); }

signals:
    void profileChanged(const QString& profile);
    void profileCreated(const QString& profile);
    void profileDeleted(const QString& profile);
};

/**
 * @brief Stub implementation for shortcut management
 */
class ShortcutManager : public QObject {
    Q_OBJECT

public:
    explicit ShortcutManager(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~ShortcutManager() = default;

    virtual void registerShortcut(const QString& action, const QString& shortcut) {
        Q_UNUSED(action);
        Q_UNUSED(shortcut);
    }
    virtual QString getShortcut(const QString& action) const {
        Q_UNUSED(action);
        return QString();
    }
    virtual void setShortcut(const QString& action, const QString& shortcut) {
        Q_UNUSED(action);
        Q_UNUSED(shortcut);
    }
    virtual QStringList availableActions() const { return QStringList(); }

signals:
    void shortcutChanged(const QString& action, const QString& shortcut);
};

#endif // MANAGERSTUBS_H
