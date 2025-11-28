/**
 * @file IconSearchWidget.cpp
 * @brief Implementation of IconSearchWidget
 * @author Max Qian
 * @date 2025
 * @copyright MIT Licensed
 */

#include "IconSearchWidget.h"

#include <QComboBox>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>

namespace gallery {

IconSearchWidget::IconSearchWidget(QWidget* parent)
    : QWidget(parent),
      m_searchWidget(std::make_unique<SearchWidget>(this)),
      m_categoryComboBox(nullptr),
      m_favoritesCheckBox(nullptr) {
    setupUI();
}

IconSearchWidget::~IconSearchWidget() = default;

void IconSearchWidget::setupUI() {
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(8);

    // Add search widget (takes most of the space)
    layout->addWidget(m_searchWidget.get(), 1);

    // Category filter
    QLabel* categoryLabel = new QLabel("Category:", this);
    categoryLabel->setStyleSheet("color: #aaa;");
    layout->addWidget(categoryLabel);

    m_categoryComboBox = new QComboBox(this);
    m_categoryComboBox->setMinimumWidth(120);
    m_categoryComboBox->setMaximumWidth(200);
    m_categoryComboBox->setStyleSheet(
        "QComboBox {"
        "  border: 1px solid #555;"
        "  border-radius: 4px;"
        "  padding: 4px 8px;"
        "  background-color: #2a2a2a;"
        "  color: white;"
        "  font-size: 10pt;"
        "}"
        "QComboBox:hover {"
        "  border: 1px solid #666;"
        "}"
        "QComboBox::drop-down {"
        "  border: none;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #2a2a2a;"
        "  color: white;"
        "  selection-background-color: #0066CC;"
        "}"
    );
    connect(m_categoryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &IconSearchWidget::onCategoryChanged);
    layout->addWidget(m_categoryComboBox);

    // Favorites checkbox
    m_favoritesCheckBox = new QCheckBox("Favorites", this);
    m_favoritesCheckBox->setStyleSheet(
        "QCheckBox {"
        "  color: #aaa;"
        "}"
        "QCheckBox::indicator {"
        "  width: 18px;"
        "  height: 18px;"
        "  border-radius: 3px;"
        "  border: 1px solid #555;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "  background-color: #2a2a2a;"
        "}"
        "QCheckBox::indicator:checked {"
        "  background-color: #0066CC;"
        "  border: 1px solid #0066CC;"
        "  image: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAcAAAAHCAYAAADEUlfTAAAAMElEQVQIHWP8//8/AwMxwOjIkXGjoaGBkYECYHRkyChQCkwOqsLIyMDAwPj//38GBgYGAO5/CXiktc97AAAAAElFTkSuQmCC);"
        "}"
    );
    connect(m_favoritesCheckBox, &QCheckBox::toggled, this, &IconSearchWidget::onFavoritesToggled);
    layout->addWidget(m_favoritesCheckBox);

    // Connect search widget signals
    connect(m_searchWidget.get(), &SearchWidget::searchTextChanged,
            this, &IconSearchWidget::onSearchTextChanged);

    setLayout(layout);
    setMaximumHeight(40);
}

void IconSearchWidget::setCategories(const QStringList& categories) {
    m_categoryComboBox->blockSignals(true);
    m_categoryComboBox->clear();
    m_categoryComboBox->addItems(categories);
    m_categoryComboBox->blockSignals(false);
}

QString IconSearchWidget::searchText() const {
    return m_searchWidget->searchText();
}

void IconSearchWidget::setSearchText(const QString& text) {
    m_searchWidget->setSearchText(text);
}

QString IconSearchWidget::selectedCategory() const {
    return m_categoryComboBox->currentText();
}

void IconSearchWidget::setSelectedCategory(const QString& category) {
    int index = m_categoryComboBox->findText(category);
    if (index >= 0) {
        m_categoryComboBox->blockSignals(true);
        m_categoryComboBox->setCurrentIndex(index);
        m_categoryComboBox->blockSignals(false);
    }
}

bool IconSearchWidget::isFavoritesOnly() const {
    return m_favoritesCheckBox->isChecked();
}

void IconSearchWidget::setFavoritesOnly(bool favoritesOnly) {
    m_favoritesCheckBox->blockSignals(true);
    m_favoritesCheckBox->setChecked(favoritesOnly);
    m_favoritesCheckBox->blockSignals(false);
}

void IconSearchWidget::clear() {
    m_searchWidget->clear();
    m_categoryComboBox->blockSignals(true);
    m_categoryComboBox->setCurrentIndex(0);
    m_categoryComboBox->blockSignals(false);
    m_favoritesCheckBox->blockSignals(true);
    m_favoritesCheckBox->setChecked(false);
    m_favoritesCheckBox->blockSignals(false);
}

void IconSearchWidget::focusSearchInput() {
    m_searchWidget->focusSearchInput();
}

void IconSearchWidget::onCategoryChanged(int index) {
    if (index >= 0) {
        emit categoryFilterChanged(m_categoryComboBox->itemText(index));
    }
}

void IconSearchWidget::onFavoritesToggled(bool checked) {
    emit favoritesOnlyChanged(checked);
}

void IconSearchWidget::onSearchTextChanged(const QString& text) {
    emit searchTextChanged(text);
}

} // namespace gallery
