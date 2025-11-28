/**
 * @file ExportProgressWidget.h
 * @brief Header for the progress widget for batch export operations
 * @details Provides a simple dialog showing export progress with cancel functionality.
 */

#ifndef EXPORTPROGRESSWIDGET_H
#define EXPORTPROGRESSWIDGET_H

#include <QDialog>

class QLabel;
class QProgressBar;

namespace gallery {

/**
 * @brief Dialog showing export progress with cancel functionality
 */
class ExportProgressDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportProgressDialog(QWidget* parent = nullptr);
    ~ExportProgressDialog() override = default;

    /**
     * @brief Update the progress display
     * @param current Current item number
     * @param total Total number of items
     * @param currentIcon Name of the current icon being exported
     */
    void updateProgress(int current, int total, const QString& currentIcon);

    /**
     * @brief Check if cancel was requested
     * @return true if user clicked cancel
     */
    bool wasCancelled() const { return m_cancelled; }

signals:
    /**
     * @brief Emitted when the user clicks cancel
     */
    void cancelRequested();

private slots:
    void onCancelClicked();

private:
    bool m_cancelled{false};
    QLabel* m_statusLabel{nullptr};
    QProgressBar* m_progressBar{nullptr};
    int m_total{0};
};

} // namespace gallery

#endif // EXPORTPROGRESSWIDGET_H
