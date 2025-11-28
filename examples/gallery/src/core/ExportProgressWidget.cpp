/**
 * @file ExportProgressWidget.cpp
 * @brief Implementation of the progress widget for batch export operations
 * @details Provides a simple dialog showing export progress with cancel functionality.
 */

#include "ExportProgressWidget.h"

#include <QApplication>
#include <QDebug>
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

namespace gallery {

ExportProgressDialog::ExportProgressDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Exporting Icons");
    setModal(true);
    setMinimumWidth(400);

    auto layout = new QVBoxLayout(this);

    m_statusLabel = new QLabel("Preparing export...");
    layout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar();
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    layout->addWidget(m_progressBar);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::rejected, this, &ExportProgressDialog::onCancelClicked);

    setLayout(layout);
}

void ExportProgressDialog::updateProgress(int current, int total, const QString& currentIcon) {
    m_total = total;
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(current);

    if (m_total > 0) {
        int percent = (current * 100) / m_total;
        m_statusLabel->setText(QString("Exporting: %1 (%2%)").arg(currentIcon).arg(percent));
    } else {
        m_statusLabel->setText(QString("Exporting: %1").arg(currentIcon));
    }
}

void ExportProgressDialog::onCancelClicked() {
    m_cancelled = true;
    emit cancelRequested();
    accept();
}

} // namespace gallery
