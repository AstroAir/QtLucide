/**
 * @file PreferencesDialog.h
 * @brief Application preferences dialog
 * @details Provides a dialog for configuring application preferences including
 *          theme selection, default icon size, stroke width, and other settings.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>
#include <QString>

class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;

namespace gallery {

enum class ThemeMode;

/**
 * @struct PreferencesSettings
 * @brief Application preferences structure
 */
struct PreferencesSettings {
    int themeMode;              ///< Theme mode (0=Dark, 1=Light, 2=System)
    int defaultIconSize;        ///< Default icon size in pixels
    double defaultStrokeWidth;  ///< Default stroke width
    bool showTooltips;          ///< Whether to show tooltips
};

/**
 * @class PreferencesDialog
 * @brief Dialog for managing application preferences
 * @details Allows users to configure:
 *          - Theme selection (Dark, Light, System)
 *          - Default icon size
 *          - Default stroke width
 *          - Tooltip visibility
 *          - Settings are persisted using QSettings
 *
 * @par Usage:
 * @code
 * PreferencesDialog dialog;
 * dialog.loadSettings();
 * if (dialog.exec() == QDialog::Accepted) {
 *     dialog.saveSettings();
 * }
 * @endcode
 */
class PreferencesDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construct a PreferencesDialog
     * @param parent Parent widget
     */
    explicit PreferencesDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~PreferencesDialog() override;

    /**
     * @brief Load preferences from QSettings
     */
    void loadSettings();

    /**
     * @brief Save preferences to QSettings
     */
    void saveSettings();

    /**
     * @brief Get the current preferences
     * @return PreferencesSettings structure with current values
     */
    [[nodiscard]] PreferencesSettings getPreferences() const;

private Q_SLOTS:
    /**
     * @brief Called when theme selection changes
     * @param index The new theme index
     */
    void onThemeChanged(int index);

    /**
     * @brief Called when default icon size changes
     * @param value The new size value
     */
    void onDefaultSizeChanged(int value);

    /**
     * @brief Called when default stroke width changes
     * @param value The new stroke width value
     */
    void onDefaultStrokeChanged(double value);

    /**
     * @brief Called when tooltips option changes
     * @param checked Whether tooltips are enabled
     */
    void onTooltipsToggled(bool checked);

    /**
     * @brief Called when Reset to Defaults button is clicked
     */
    void onResetToDefaults();

    /**
     * @brief Called when OK button is clicked
     */
    void onOkClicked();

    /**
     * @brief Called when Cancel button is clicked
     */
    void onCancelClicked();

private:
    /**
     * @brief Initialize the dialog UI
     */
    void setupUI();

    /**
     * @brief Create the appearance section
     * @return The appearance group box
     */
    QGroupBox* createAppearanceSection();

    /**
     * @brief Create the behavior section
     * @return The behavior group box
     */
    QGroupBox* createBehaviorSection();

    /**
     * @brief Create the button section
     * @return Layout containing buttons
     */
    QHBoxLayout* createButtonSection();

    /**
     * @brief Reset all settings to default values
     */
    void resetToDefaults();

    // UI Components
    QComboBox* m_themeComboBox;          ///< Theme selection combo box
    QSpinBox* m_defaultSizeSpinBox;      ///< Default icon size
    QDoubleSpinBox* m_defaultStrokeSpinBox; ///< Default stroke width
    QCheckBox* m_showTooltipsCheckBox;   ///< Show tooltips checkbox
    QPushButton* m_resetButton;          ///< Reset to defaults button
    QPushButton* m_okButton;             ///< OK button
    QPushButton* m_cancelButton;         ///< Cancel button

    // State
    PreferencesSettings m_currentSettings;  ///< Current settings
    PreferencesSettings m_lastSavedSettings; ///< Last saved settings
};

} // namespace gallery

#endif // PREFERENCES_DIALOG_H
