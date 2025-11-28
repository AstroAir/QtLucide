/**
 * @file ExportDialog.h
 * @brief Batch export dialog for the gallery application
 * @details Provides a dialog for batch exporting multiple icons with consistent
 *          format, size, and output directory settings.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <memory>

class QLabel;
class QComboBox;
class QSpinBox;
class QPushButton;
class QLineEdit;
class QProgressBar;
class QListWidget;
class QCheckBox;
class QRadioButton;
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;

namespace gallery {

enum class ExportFormat;
class BatchExportManager;

/**
 * @struct BatchExportSettings
 * @brief Settings for batch export operation
 */
struct BatchExportSettings {
    QStringList iconNames;          ///< Icons to export (empty = all)
    ExportFormat format;            ///< Export format (SVG, PNG, ICO)
    int size;                       ///< Export size in pixels
    QString outputDirectory;        ///< Output directory path
    QString filePattern;            ///< File naming pattern ({name}, {category}, {ext})
    bool groupByCategory;           ///< Whether to create subdirectories by category
    bool overwriteExisting;         ///< Whether to overwrite existing files
};

/**
 * @class ExportDialog
 * @brief Dialog for batch exporting multiple icons
 * @details Allows users to:
 *          - Select multiple icons or export all
 *          - Choose export format (SVG, PNG, ICO)
 *          - Select export size
 *          - Choose output directory
 *          - Configure file naming pattern
 *          - Group by category (create subdirectories)
 *          - View export progress
 *          - Start and cancel export
 *
 * @par Usage:
 * @code
 * ExportDialog dialog;
 * dialog.setAvailableIcons({"house", "settings", "user"});
 * if (dialog.exec() == QDialog::Accepted) {
 *     BatchExportSettings settings = dialog.getExportSettings();
 *     // Start export with BatchExportManager
 * }
 * @endcode
 */
class ExportDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construct an ExportDialog
     * @param parent Parent widget
     */
    explicit ExportDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ExportDialog() override;

    /**
     * @brief Set the list of available icons
     * @param iconNames List of all available icon names
     */
    void setAvailableIcons(const QStringList& iconNames);

    /**
     * @brief Get the current batch export settings
     * @return The BatchExportSettings configured by the user
     */
    [[nodiscard]] BatchExportSettings getExportSettings() const;

private Q_SLOTS:
    /**
     * @brief Called when "Export All" checkbox state changes
     * @param checked Whether export all is checked
     */
    void onExportAllToggled(bool checked);

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
     * @brief Called when browse button is clicked for output directory
     */
    void onBrowseOutputClicked();

    /**
     * @brief Called when grouping option changes
     * @param checked Whether to group by category
     */
    void onGroupByCategoryToggled(bool checked);

    /**
     * @brief Handle export completion
     * @param success Whether export was successful
     * @param exported Number of exported files
     * @param failed Number of failed exports
     * @param errorMessage Error message if any
     */
    void onExportFinished(bool success, int exported, int failed, const QString& errorMessage);

    /**
     * @brief Handle export progress update
     * @param current Current progress count
     * @param total Total count
     */
    void onProgressChanged(int current, int total);

    /**
     * @brief Start the export operation
     */
    void onExportClicked();

    /**
     * @brief Cancel the export operation
     */
    void onCancelClicked();

private:
    /**
     * @brief Initialize the dialog UI
     */
    void setupUI();

    /**
     * @brief Create the icon selection section
     * @return The icon selection group box
     */
    QGroupBox* createIconSelectionSection();

    /**
     * @brief Create the format and size section
     * @return The format/size group box
     */
    QGroupBox* createFormatSizeSection();

    /**
     * @brief Create the output section
     * @return The output group box
     */
    QGroupBox* createOutputSection();

    /**
     * @brief Create the progress section
     * @return The progress group box
     */
    QGroupBox* createProgressSection();

    /**
     * @brief Create the button section
     * @return Layout containing action buttons
     */
    QHBoxLayout* createButtonSection();

    /**
     * @brief Validate dialog settings
     * @return true if settings are valid, false otherwise
     */
    [[nodiscard]] bool validateSettings() const;

    /**
     * @brief Update UI state during export
     * @param exporting Whether export is in progress
     */
    void setExportingState(bool exporting);

    // UI Components
    QCheckBox* m_exportAllCheckBox;      ///< Export all icons checkbox
    QListWidget* m_iconListWidget;       ///< List of selected icons
    QComboBox* m_formatComboBox;         ///< Format selection (SVG, PNG, ICO)
    QComboBox* m_sizeComboBox;           ///< Preset size selection
    QLineEdit* m_outputDirEdit;          ///< Output directory path
    QPushButton* m_browseButton;         ///< Browse for output directory
    QCheckBox* m_groupByCategoryCheckBox; ///< Group by category checkbox
    QLineEdit* m_filePatternEdit;        ///< File naming pattern
    QProgressBar* m_progressBar;         ///< Export progress bar
    QLabel* m_statusLabel;               ///< Status message label
    QPushButton* m_exportButton;         ///< Export button
    QPushButton* m_cancelButton;         ///< Cancel button

    // State
    QStringList m_availableIcons;        ///< All available icons
    std::unique_ptr<BatchExportManager> m_exportManager; ///< Export manager
    bool m_isExporting;                  ///< Whether export is in progress
};

} // namespace gallery

#endif // EXPORT_DIALOG_H
