/**
 * @file IconDetailsPanel.cpp
 * @brief Implementation of IconDetailsPanel
 * @details Handles display and interaction with detailed icon information
 */

#include "IconDetailsPanel.h"

#include "FavoritesManager.h"
#include "IconThumbnailGridWidget.h"

#include <QtLucide/QtLucide.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
// QFlowLayout is not standard Qt, using QHBoxLayout instead
#include <QApplication>
#include <QClipboard>
#include <QDebug>

namespace gallery {

IconDetailsPanel::IconDetailsPanel(QWidget* parent)
    : QFrame(parent), m_lucide(nullptr), m_favManager(nullptr), m_isCollapsed(false),
      m_defaultWidth(320) {
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    initializeUI();

    // Set initial size
    setFixedWidth(m_defaultWidth);
}

IconDetailsPanel::~IconDetailsPanel() = default;

void IconDetailsPanel::initializeUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create header with icon name and buttons
    QWidget* headerWidget = new QWidget();
    m_headerLayout = new QHBoxLayout(headerWidget);
    m_headerLayout->setContentsMargins(12, 8, 8, 8);

    m_iconNameLabel = new QLabel();
    m_iconNameLabel->setObjectName("title");
    m_headerLayout->addWidget(m_iconNameLabel, 1);

    m_favoriteButton = new QPushButton("♡");
    m_favoriteButton->setFixedSize(28, 28);
    m_favoriteButton->setToolTip("Add to favorites");
    m_headerLayout->addWidget(m_favoriteButton);

    m_exportButton = new QPushButton("⬇");
    m_exportButton->setFixedSize(28, 28);
    m_exportButton->setToolTip("Export icon");
    m_headerLayout->addWidget(m_exportButton);

    m_collapseButton = new QPushButton("✕");
    m_collapseButton->setFixedSize(28, 28);
    m_collapseButton->setToolTip("Collapse panel");
    m_headerLayout->addWidget(m_collapseButton);

    m_mainLayout->addWidget(headerWidget);

    // Create scroll area for content
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet("QScrollArea { border: none; }");

    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(12, 0, 12, 12);
    m_contentLayout->setSpacing(12);

    // Preview section
    m_thumbnailGrid = new IconThumbnailGridWidget();
    m_thumbnailGrid->setPreviewSizes({24, 48, 96, 192});
    m_thumbnailGrid->setSpacing(8);
    m_contentLayout->addWidget(m_thumbnailGrid);

    // Tags section
    m_tagsLabel = new QLabel("Tags");
    m_tagsLabel->setObjectName("sectionHeader");
    m_contentLayout->addWidget(m_tagsLabel);

    m_tagsContainer = new QWidget();
    m_tagsLayout = new QHBoxLayout(m_tagsContainer);
    m_tagsLayout->setContentsMargins(0, 0, 0, 0);
    m_tagsLayout->setSpacing(6);
    m_contentLayout->addWidget(m_tagsContainer);

    // Categories section
    m_categoriesLabel = new QLabel("Categories");
    m_categoriesLabel->setObjectName("sectionHeader");
    m_contentLayout->addWidget(m_categoriesLabel);

    m_categoriesContainer = new QWidget();
    m_categoriesLayout = new QVBoxLayout(m_categoriesContainer);
    m_categoriesLayout->setContentsMargins(0, 0, 0, 0);
    m_categoriesLayout->setSpacing(4);
    m_contentLayout->addWidget(m_categoriesContainer);

    // Code snippets section
    m_snippetsLabel = new QLabel("Code Snippets");
    m_snippetsLabel->setObjectName("sectionHeader");
    m_contentLayout->addWidget(m_snippetsLabel);

    // C++ Enum snippet
    m_enumSnippetWidget = new QWidget();
    m_enumSnippetLayout = new QHBoxLayout(m_enumSnippetWidget);
    m_enumSnippetLayout->setContentsMargins(0, 0, 0, 0);
    m_enumSnippetLayout->setSpacing(4);

    m_enumSnippetLabel = new QLabel();
    m_enumSnippetLabel->setObjectName("codeSnippet");
    m_enumSnippetLabel->setWordWrap(true);
    m_enumSnippetLayout->addWidget(m_enumSnippetLabel, 1);

    m_enumCopyButton = new QPushButton("Copy");
    m_enumCopyButton->setFixedWidth(70);
    m_enumSnippetLayout->addWidget(m_enumCopyButton);

    m_contentLayout->addWidget(m_enumSnippetWidget);

    // C++ String snippet
    m_stringSnippetWidget = new QWidget();
    m_stringSnippetLayout = new QHBoxLayout(m_stringSnippetWidget);
    m_stringSnippetLayout->setContentsMargins(0, 0, 0, 0);
    m_stringSnippetLayout->setSpacing(4);

    m_stringSnippetLabel = new QLabel();
    m_stringSnippetLabel->setObjectName("codeSnippet");
    m_stringSnippetLabel->setWordWrap(true);
    m_stringSnippetLayout->addWidget(m_stringSnippetLabel, 1);

    m_stringCopyButton = new QPushButton("Copy");
    m_stringCopyButton->setFixedWidth(70);
    m_stringSnippetLayout->addWidget(m_stringCopyButton);

    m_contentLayout->addWidget(m_stringSnippetWidget);

    // Action buttons
    m_actionButtonsLayout = new QHBoxLayout();
    m_actionButtonsLayout->setSpacing(6);

    m_copyNameButton = new QPushButton("Copy Name");
    m_actionButtonsLayout->addWidget(m_copyNameButton);

    m_contentLayout->addLayout(m_actionButtonsLayout);

    // Add stretch at bottom
    m_contentLayout->addStretch();

    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea, 1);

    // Connect signals
    connect(m_favoriteButton, &QPushButton::clicked, this,
            &IconDetailsPanel::onFavoriteButtonClicked);
    connect(m_exportButton, &QPushButton::clicked, this, &IconDetailsPanel::onExportButtonClicked);
    connect(m_collapseButton, &QPushButton::clicked, this,
            &IconDetailsPanel::onCollapseButtonClicked);
    connect(m_copyNameButton, &QPushButton::clicked, this, &IconDetailsPanel::copyIconName);
    connect(m_enumCopyButton, &QPushButton::clicked, this, &IconDetailsPanel::copyCppEnumSnippet);
    connect(m_stringCopyButton, &QPushButton::clicked, this,
            &IconDetailsPanel::copyCppStringSnippet);
}

void IconDetailsPanel::setSelectedIcon(const QString& iconName, const IconMetadata& metadata) {
    m_selectedIcon = iconName;
    m_currentMetadata = metadata;
    updateUI();
}

QString IconDetailsPanel::selectedIcon() const {
    return m_selectedIcon;
}

void IconDetailsPanel::clearSelection() {
    m_selectedIcon.clear();
    m_currentMetadata = IconMetadata();
    m_scrollArea->hide();
}

void IconDetailsPanel::setLucideInstance(lucide::QtLucide* lucide) {
    m_lucide = lucide;
    m_thumbnailGrid->setLucideInstance(lucide);

    // Update button icons now that we have the lucide instance
    if (m_lucide) {
        m_favoriteButton->setIcon(m_lucide->icon("heart"));
        m_favoriteButton->setText("");
        m_exportButton->setIcon(m_lucide->icon("download"));
        m_exportButton->setText("");
        m_collapseButton->setIcon(m_lucide->icon("x"));
        m_collapseButton->setText("");
        m_enumCopyButton->setIcon(m_lucide->icon("copy"));
        m_stringCopyButton->setIcon(m_lucide->icon("copy"));
        m_copyNameButton->setIcon(m_lucide->icon("clipboard"));
    }
}

void IconDetailsPanel::setFavoritesManager(FavoritesManager* favManager) {
    m_favManager = favManager;
}

void IconDetailsPanel::toggleCollapsed() {
    setCollapsed(!m_isCollapsed);
}

bool IconDetailsPanel::isCollapsed() const {
    return m_isCollapsed;
}

void IconDetailsPanel::setCollapsed(bool collapsed) {
    m_isCollapsed = collapsed;
    if (collapsed) {
        setFixedWidth(30);
        m_scrollArea->hide();
        if (m_lucide) {
            m_collapseButton->setIcon(m_lucide->icon("chevron-left"));
        } else {
            m_collapseButton->setText("◀");
        }
        m_collapseButton->setToolTip("Expand panel");
    } else {
        setFixedWidth(m_defaultWidth);
        m_scrollArea->show();
        if (m_lucide) {
            m_collapseButton->setIcon(m_lucide->icon("x"));
        } else {
            m_collapseButton->setText("✕");
        }
        m_collapseButton->setToolTip("Collapse panel");
    }
    emit collapsedStateChanged(collapsed);
}

void IconDetailsPanel::setDefaultWidth(int width) {
    m_defaultWidth = std::max(200, width);
    if (!m_isCollapsed) {
        setFixedWidth(m_defaultWidth);
    }
}

void IconDetailsPanel::copyIconName() {
    if (!m_selectedIcon.isEmpty()) {
        QApplication::clipboard()->setText(m_selectedIcon);
    }
}

void IconDetailsPanel::copyCppEnumSnippet() {
    if (!m_selectedIcon.isEmpty()) {
        QApplication::clipboard()->setText(generateCppEnumSnippet(m_selectedIcon));
    }
}

void IconDetailsPanel::copyCppStringSnippet() {
    if (!m_selectedIcon.isEmpty()) {
        QApplication::clipboard()->setText(generateCppStringSnippet(m_selectedIcon));
    }
}

void IconDetailsPanel::onTagClicked() {
    // Find which tag button was clicked
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (button) {
        emit tagClicked(button->text());
    }
}

void IconDetailsPanel::onFavoriteButtonClicked() {
    if (m_selectedIcon.isEmpty()) {
        return;
    }

    bool isFavorite = false;
    if (m_favManager) {
        isFavorite = m_favManager->isFavorite(m_selectedIcon);
        if (isFavorite) {
            m_favManager->removeFavorite(m_selectedIcon);
        } else {
            m_favManager->addFavorite(m_selectedIcon);
        }
        isFavorite = !isFavorite; // Toggle
    }

    updateFavoriteButton();
    emit favoriteToggled(m_selectedIcon, !isFavorite);
}

void IconDetailsPanel::onExportButtonClicked() {
    if (!m_selectedIcon.isEmpty()) {
        emit exportRequested(m_selectedIcon);
    }
}

void IconDetailsPanel::onCollapseButtonClicked() {
    toggleCollapsed();
}

void IconDetailsPanel::updateUI() {
    if (m_selectedIcon.isEmpty()) {
        m_scrollArea->hide();
        return;
    }

    m_scrollArea->show();

    // Update header
    m_iconNameLabel->setText(m_selectedIcon);

    // Update thumbnail grid
    m_thumbnailGrid->setIconName(m_selectedIcon);

    // Clear and populate tags
    for (int i = m_tagsLayout->count() - 1; i >= 0; --i) {
        delete m_tagsLayout->takeAt(i)->widget();
    }

    if (!m_currentMetadata.tags.isEmpty()) {
        for (const QString& tag : m_currentMetadata.tags) {
            QPushButton* tagButton = new QPushButton(tag);
            tagButton->setObjectName("tagButton");
            tagButton->setFlat(true);
            tagButton->setMaximumWidth(120);
            connect(tagButton, &QPushButton::clicked, this, &IconDetailsPanel::onTagClicked);
            m_tagsLayout->addWidget(tagButton);
        }
    }

    // Clear and populate categories
    for (int i = m_categoriesLayout->count() - 1; i >= 0; --i) {
        delete m_categoriesLayout->takeAt(i)->widget();
    }

    if (!m_currentMetadata.categories.isEmpty()) {
        for (const QString& category : m_currentMetadata.categories) {
            QLabel* categoryLabel = new QLabel(category);
            categoryLabel->setObjectName("subtitle");
            m_categoriesLayout->addWidget(categoryLabel);
        }
    }

    // Update code snippets
    m_enumSnippetLabel->setText(generateCppEnumSnippet(m_selectedIcon));
    m_stringSnippetLabel->setText(generateCppStringSnippet(m_selectedIcon));

    // Update favorite button
    updateFavoriteButton();
}

QString IconDetailsPanel::generateCppEnumSnippet(const QString& iconName) const {
    QString enumId = iconNameToEnumId(iconName);
    return QString("lucide.icon(Icons::%1)").arg(enumId);
}

QString IconDetailsPanel::generateCppStringSnippet(const QString& iconName) const {
    return QString("lucide.icon(\"%1\")").arg(iconName);
}

QString IconDetailsPanel::iconNameToEnumId(const QString& iconName) {
    // Convert kebab-case to snake_case
    QString result = iconName;
    return result.replace("-", "_").toLower();
}

void IconDetailsPanel::updateFavoriteButton() {
    bool isFavorite =
        !m_selectedIcon.isEmpty() && m_favManager && m_favManager->isFavorite(m_selectedIcon);
    if (m_lucide) {
        // Use Lucide icons - heart-off for not favorite, heart for favorite
        if (isFavorite) {
            m_favoriteButton->setIcon(m_lucide->icon("heart", {{"color", QColor(Qt::red)}}));
            m_favoriteButton->setToolTip("Remove from favorites");
        } else {
            m_favoriteButton->setIcon(m_lucide->icon("heart"));
            m_favoriteButton->setToolTip("Add to favorites");
        }
        m_favoriteButton->setText("");
        m_favoriteButton->setStyleSheet("");
    } else {
        // Fallback to text
        if (isFavorite) {
            m_favoriteButton->setText("♥");
            m_favoriteButton->setStyleSheet("color: red;");
            m_favoriteButton->setToolTip("Remove from favorites");
        } else {
            m_favoriteButton->setText("♡");
            m_favoriteButton->setStyleSheet("");
            m_favoriteButton->setToolTip("Add to favorites");
        }
    }
}

} // namespace gallery
