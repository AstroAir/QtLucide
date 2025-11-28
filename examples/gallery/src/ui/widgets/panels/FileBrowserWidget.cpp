/**
 * @file FileBrowserWidget.cpp
 * @brief Implementation of FileBrowserWidget
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#include "FileBrowserWidget.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>

namespace gallery {

FileBrowserWidget::FileBrowserWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

FileBrowserWidget::~FileBrowserWidget() = default;

void FileBrowserWidget::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);

    m_placeholderLabel = new QLabel(this);
    m_placeholderLabel->setText(
        "<h3>File Browser</h3>"
        "<p>This feature is coming soon!</p>"
        "<p>Future versions will include:</p>"
        "<ul>"
        "<li>Browse local file system</li>"
        "<li>View icon files and directories</li>"
        "<li>Quick file operations</li>"
        "<li>Recently used locations</li>"
        "</ul>"
    );
    m_placeholderLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_placeholderLabel->setWordWrap(true);
    m_placeholderLabel->setStyleSheet("color: #666666; padding: 20px;");

    layout->addWidget(m_placeholderLabel);
    layout->addStretch();

    setLayout(layout);
}

} // namespace gallery
