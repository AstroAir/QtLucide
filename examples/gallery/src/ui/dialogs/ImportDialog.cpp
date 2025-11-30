/**
 * @file ImportDialog.cpp
 * @brief Implementation of ImportDialog
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "ImportDialog.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace gallery {

ImportDialog::ImportDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Import Custom Icons");
    setMinimumWidth(400);
    setMinimumHeight(250);
    setupUI();
}

ImportDialog::~ImportDialog() = default;

void ImportDialog::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Main message
    m_messageLabel = new QLabel(this);
    m_messageLabel->setText("<h2>Import Custom Icon Sets</h2>"
                            "<p>This feature is coming soon!</p>"
                            "<p>In future versions, you will be able to:</p>"
                            "<ul>"
                            "<li>Import custom SVG icon sets</li>"
                            "<li>Load icon packs from directories</li>"
                            "<li>Merge with existing icons</li>"
                            "<li>Manage multiple icon libraries</li>"
                            "</ul>"
                            "<p>Please check back later for updates.</p>");
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);
    mainLayout->addWidget(m_messageLabel);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_closeButton = new QPushButton("Close", this);
    m_closeButton->setMinimumWidth(100);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(m_closeButton);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

} // namespace gallery
