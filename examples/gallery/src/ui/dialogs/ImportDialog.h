/**
 * @file ImportDialog.h
 * @brief Import dialog for custom icon sets (placeholder)
 * @details Provides a dialog for importing custom icon sets.
 *          Currently a placeholder for future implementation.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef IMPORT_DIALOG_H
#define IMPORT_DIALOG_H

#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

namespace gallery {

/**
 * @class ImportDialog
 * @brief Dialog for importing custom icon sets
 * @details This is a placeholder dialog showing "Coming soon" message.
 *          Future implementation will allow users to import custom icon sets
 *          from various sources.
 *
 * @par Usage:
 * @code
 * ImportDialog dialog;
 * dialog.exec();
 * @endcode
 */
class ImportDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construct an ImportDialog
     * @param parent Parent widget
     */
    explicit ImportDialog(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ImportDialog() override;

private:
    /**
     * @brief Initialize the dialog UI
     */
    void setupUI();

    // UI Components
    QLabel* m_messageLabel;     ///< Main message label
    QPushButton* m_closeButton; ///< Close button
};

} // namespace gallery

#endif // IMPORT_DIALOG_H
