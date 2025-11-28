/**
 * @file IconExportDialog.h
 * @brief Single icon export dialog for the gallery application
 * @details Provides a dialog for exporting a single icon with customizable options
 *          including format selection, size, color override, and output path.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef ICON_EXPORT_DIALOG_H
#define ICON_EXPORT_DIALOG_H

#include <QDialog>
#include <QString>
#include <QColor>
#include <memory>

class QLabel;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QSlider;
class QPushButton;
class QLineEdit;
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
class QCheckBox;

namespace gallery {

enum class ExportFormat;
struct IconOptions;

/**
 * @struct IconExportSettings
 * @brief Settings for exporting a single icon
 */
struct IconExportSettings {
    QString iconName;           ///< Name of the icon to export
    ExportFormat format;        ///< Export format (SVG, PNG, ICO)
    int size;                   ///< Export size in pixels
    bool useCustomSize;         ///< Whether to use custom size
    int customSize;             ///< Custom size value
    QColor colorOverride;       ///< Color override (if any)
    bool useColorOverride;      ///< Whether to use color override
    double strokeWidth;         ///< Stroke width override
    QString outputPath;         ///< Output file path
};

/**
 * @class IconExportDialog
 * @brief Dialog for exporting a single icon with customizable options
 * @details Allows users to:
 *          - Preview icon at selected size
 *          - Choose export format (SVG, PNG, ICO)
 *          - Select or customize export size
 *          - Override icon color
 *          - Adjust stroke width
 *          - Choose output path
 *          - Copy SVG code or PNG to clipboard
 *          - Export the icon
 *
 * @par Usage:
 * @code
 * IconExportDialog dialog;
 * dialog.setIconName("house");
 * if (dialog.exec() == QDialog::Accepted) {
 *     IconExportSettings settings = dialog.getExportSettings();
 *     // Process export
 * }
 * @endcode
 */
class IconExportDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construct an IconExportDialog
     * @param parent Parent widget
     */
    explicit IconExportDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~IconExportDialog() override;

    /**
     * @brief Set the icon to export
     * @param iconName The name of the icon to export
     */
    void setIconName(const QString& iconName);

    /**
     * @brief Get the current export settings
     * @return The IconExportSettings configured by the user
     */
    [[nodiscard]] IconExportSettings getExportSettings() const;

private Q_SLOTS:
    /**
     * @brief Called when format selection changes
     * @param index The new selected format index
     */
    void onFormatChanged(int index);

    /**
     * @brief Called when size selection changes
     * @param index The new selected size index
     */
    void onSizeChanged(int index);

    /**
     * @brief Called when custom size value changes
     * @param value The new custom size value
     */
    void onCustomSizeChanged(int value);

    /**
     * @brief Called when color override button is clicked
     */
    void onColorButtonClicked();

    /**
     * @brief Called when browse button is clicked for output path
     */
    void onBrowseOutputClicked();

    /**
     * @brief Called when "Copy SVG Code" button is clicked
     */
    void onCopySvgCodeClicked();

    /**
     * @brief Called when "Copy PNG to Clipboard" button is clicked
     */
    void onCopyPngClicked();

    /**
     * @brief Update the icon preview based on current settings
     */
    void updatePreview();

    /**
     * @brief Validate dialog and accept if valid
     */
    void onExportClicked();

private:
    /**
     * @brief Initialize the dialog UI
     */
    void setupUI();

    /**
     * @brief Create the preview section
     * @return The preview group box
     */
    QGroupBox* createPreviewSection();

    /**
     * @brief Create the format and size section
     * @return The format/size group box
     */
    QGroupBox* createFormatSizeSection();

    /**
     * @brief Create the customization section
     * @return The customization group box
     */
    QGroupBox* createCustomizationSection();

    /**
     * @brief Create the output section
     * @return The output group box
     */
    QGroupBox* createOutputSection();

    /**
     * @brief Create the button section
     * @return Layout containing action buttons
     */
    QHBoxLayout* createButtonSection();

    /**
     * @brief Get the current icon options based on dialog settings
     * @return The configured IconOptions
     */
    [[nodiscard]] IconOptions getCurrentIconOptions() const;

    /**
     * @brief Export SVG code and copy to clipboard
     */
    void copySvgCodeToClipboard();

    /**
     * @brief Export PNG and copy to clipboard
     */
    void copyPngToClipboard();

    /**
     * @brief Check if output path is valid
     * @return true if path is valid, false otherwise
     */
    [[nodiscard]] bool validateOutputPath() const;

    // UI Components
    QLabel* m_iconPreviewLabel;          ///< Label for icon preview
    QComboBox* m_formatComboBox;         ///< Format selection (SVG, PNG, ICO)
    QComboBox* m_sizeComboBox;           ///< Preset size selection
    QSpinBox* m_customSizeSpinBox;       ///< Custom size input
    QDoubleSpinBox* m_strokeWidthSpinBox; ///< Stroke width slider/spinbox
    QPushButton* m_colorButton;          ///< Color override button
    QLineEdit* m_outputPathEdit;         ///< Output file path
    QPushButton* m_browseButton;         ///< Browse for output path
    QPushButton* m_copySvgButton;        ///< Copy SVG code button
    QPushButton* m_copyPngButton;        ///< Copy PNG button
    QPushButton* m_exportButton;         ///< Export button
    QPushButton* m_cancelButton;         ///< Cancel button
    QCheckBox* m_useColorOverrideCheckBox; ///< Enable color override checkbox

    // State
    QString m_currentIconName;           ///< Current icon name
    QColor m_selectedColor;              ///< Currently selected color override
    int m_lastPreviewSize;               ///< Last preview size used
};

} // namespace gallery

#endif // ICON_EXPORT_DIALOG_H
