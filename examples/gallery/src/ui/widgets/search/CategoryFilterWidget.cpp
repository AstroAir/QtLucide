/**
 * @file CategoryFilterWidget.cpp
 * @brief Implementation of CategoryFilterWidget
 * @author Max Qian
 * @date 2025
 * @copyright MIT Licensed
 */

#include "CategoryFilterWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>

namespace gallery {

CategoryFilterWidget::CategoryFilterWidget(FilterMode mode, QWidget* parent)
    : QWidget(parent), m_mode(mode), m_selectedCategory(""), m_favoritesOnly(false),
      m_favoritesCheckBox(nullptr), m_categoryComboBox(nullptr), m_categoryListWidget(nullptr),
      m_mainLayout(nullptr) {
    setupUI();
}

CategoryFilterWidget::~CategoryFilterWidget() = default;

void CategoryFilterWidget::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(6);

    if (m_mode == FilterMode::Dropdown) {
        m_mainLayout->addWidget(createDropdownLayout());
    } else {
        m_mainLayout->addWidget(createListLayout());
    }

    setLayout(m_mainLayout);
}

QWidget* CategoryFilterWidget::createDropdownLayout() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    // Category selection row
    QHBoxLayout* categoryLayout = new QHBoxLayout();
    categoryLayout->setContentsMargins(0, 0, 0, 0);
    categoryLayout->setSpacing(6);

    QLabel* categoryLabel = new QLabel("Category:", this);
    categoryLabel->setStyleSheet("color: #aaa;");
    categoryLayout->addWidget(categoryLabel);

    m_categoryComboBox = new QComboBox(this);
    m_categoryComboBox->setMinimumWidth(120);
    m_categoryComboBox->setStyleSheet("QComboBox {"
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
                                      "}");
    connect(m_categoryComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &CategoryFilterWidget::onCategoryChanged);
    categoryLayout->addWidget(m_categoryComboBox);
    categoryLayout->addStretch();

    layout->addLayout(categoryLayout);

    // Favorites checkbox
    m_favoritesCheckBox = new QCheckBox("Show only favorites", this);
    m_favoritesCheckBox->setStyleSheet("QCheckBox {"
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
                                       "}");
    connect(m_favoritesCheckBox, &QCheckBox::toggled, this,
            &CategoryFilterWidget::onFavoritesToggled);
    layout->addWidget(m_favoritesCheckBox);

    widget->setLayout(layout);
    return widget;
}

QWidget* CategoryFilterWidget::createListLayout() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    // Category list
    QLabel* categoryLabel = new QLabel("Categories:", this);
    categoryLabel->setStyleSheet("color: #aaa; font-weight: bold;");
    layout->addWidget(categoryLabel);

    m_categoryListWidget = new QListWidget(this);
    m_categoryListWidget->setMinimumHeight(150);
    m_categoryListWidget->setMaximumHeight(250);
    m_categoryListWidget->setStyleSheet("QListWidget::item {"
                                        "  padding: 4px;"
                                        "  height: 24px;"
                                        "}"
                                        "QListWidget::item:selected {"
                                        "  background-color: #0066CC;"
                                        "  color: white;"
                                        "}"
                                        "QListWidget::item:hover {"
                                        "  background-color: #333;"
                                        "}");
    connect(m_categoryListWidget, &QListWidget::itemClicked, this,
            &CategoryFilterWidget::onCategoryListItemSelected);
    layout->addWidget(m_categoryListWidget);

    // Favorites checkbox
    m_favoritesCheckBox = new QCheckBox("Show only favorites", this);
    m_favoritesCheckBox->setStyleSheet("QCheckBox {"
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
                                       "}");
    connect(m_favoritesCheckBox, &QCheckBox::toggled, this,
            &CategoryFilterWidget::onFavoritesToggled);
    layout->addWidget(m_favoritesCheckBox);

    widget->setLayout(layout);
    return widget;
}

void CategoryFilterWidget::setMode(FilterMode mode) {
    if (m_mode == mode) {
        return;
    }

    m_mode = mode;

    // Clear main layout
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Recreate UI with new mode
    if (m_mode == FilterMode::Dropdown) {
        m_mainLayout->addWidget(createDropdownLayout());
    } else {
        m_mainLayout->addWidget(createListLayout());
    }

    // Restore state
    setSelectedCategory(m_selectedCategory);
    setFavoritesOnly(m_favoritesOnly);
}

FilterMode CategoryFilterWidget::mode() const {
    return m_mode;
}

void CategoryFilterWidget::setCategories(const QStringList& categories) {
    m_categories = categories;

    if (m_mode == FilterMode::Dropdown && m_categoryComboBox) {
        m_categoryComboBox->blockSignals(true);
        m_categoryComboBox->clear();
        m_categoryComboBox->addItems(categories);
        if (!m_selectedCategory.isEmpty()) {
            int index = m_categoryComboBox->findText(m_selectedCategory);
            if (index >= 0) {
                m_categoryComboBox->setCurrentIndex(index);
            }
        }
        m_categoryComboBox->blockSignals(false);
    } else if (m_mode == FilterMode::List && m_categoryListWidget) {
        m_categoryListWidget->blockSignals(true);
        m_categoryListWidget->clear();
        for (const QString& category : categories) {
            QListWidgetItem* item = new QListWidgetItem(category, m_categoryListWidget);
            if (category == m_selectedCategory) {
                item->setSelected(true);
            }
        }
        m_categoryListWidget->blockSignals(false);
    }
}

QStringList CategoryFilterWidget::categories() const {
    return m_categories;
}

void CategoryFilterWidget::setSelectedCategory(const QString& category) {
    m_selectedCategory = category;

    if (m_mode == FilterMode::Dropdown && m_categoryComboBox) {
        m_categoryComboBox->blockSignals(true);
        int index = m_categoryComboBox->findText(category);
        if (index >= 0) {
            m_categoryComboBox->setCurrentIndex(index);
        } else if (m_categoryComboBox->count() > 0) {
            m_categoryComboBox->setCurrentIndex(0);
        }
        m_categoryComboBox->blockSignals(false);
    } else if (m_mode == FilterMode::List && m_categoryListWidget) {
        m_categoryListWidget->blockSignals(true);
        for (int i = 0; i < m_categoryListWidget->count(); ++i) {
            QListWidgetItem* item = m_categoryListWidget->item(i);
            item->setSelected(item->text() == category);
        }
        m_categoryListWidget->blockSignals(false);
    }
}

QString CategoryFilterWidget::selectedCategory() const {
    return m_selectedCategory;
}

void CategoryFilterWidget::setFavoritesOnly(bool favoritesOnly) {
    m_favoritesOnly = favoritesOnly;
    if (m_favoritesCheckBox) {
        m_favoritesCheckBox->blockSignals(true);
        m_favoritesCheckBox->setChecked(favoritesOnly);
        m_favoritesCheckBox->blockSignals(false);
    }
}

bool CategoryFilterWidget::isFavoritesOnly() const {
    return m_favoritesOnly;
}

CategoryFilterSettings CategoryFilterWidget::filterSettings() const {
    return CategoryFilterSettings{m_selectedCategory, m_favoritesOnly};
}

void CategoryFilterWidget::setFilterSettings(const CategoryFilterSettings& settings) {
    setSelectedCategory(settings.selectedCategory);
    setFavoritesOnly(settings.favoritesOnly);
}

void CategoryFilterWidget::clear() {
    setSelectedCategory("");
    setFavoritesOnly(false);
}

void CategoryFilterWidget::onCategoryChanged(int index) {
    if (index >= 0 && m_categoryComboBox) {
        m_selectedCategory = m_categoryComboBox->itemText(index);
        emit categoryFilterChanged(m_selectedCategory);
        emit filterChanged(filterSettings());
    }
}

void CategoryFilterWidget::onCategoryListItemSelected(QListWidgetItem* item) {
    m_selectedCategory = item->text();
    emit categoryFilterChanged(m_selectedCategory);
    emit filterChanged(filterSettings());
}

void CategoryFilterWidget::onFavoritesToggled(bool checked) {
    m_favoritesOnly = checked;
    emit favoritesOnlyChanged(checked);
    emit filterChanged(filterSettings());
}

} // namespace gallery
