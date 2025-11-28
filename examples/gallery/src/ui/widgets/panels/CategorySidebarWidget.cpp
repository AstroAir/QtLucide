/**
 * @file CategorySidebarWidget.cpp
 * @brief Implementation of CategorySidebarWidget
 * @author Max Qian
 * @date 2025
 * @copyright MIT Licensed
 */

#include "CategorySidebarWidget.h"

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QColorDialog>
#include <QFont>
#include <QFontMetrics>

namespace gallery {

CategorySidebarWidget::CategorySidebarWidget(QWidget* parent)
    : QWidget(parent),
      m_resetButton(nullptr),
      m_colorLabel(nullptr),
      m_colorButton(nullptr),
      m_strokeWidthLabel(nullptr),
      m_strokeWidthValueLabel(nullptr),
      m_strokeWidthSlider(nullptr),
      m_sizeLabel(nullptr),
      m_sizeValueLabel(nullptr),
      m_sizeSlider(nullptr),
      m_categoryListWidget(nullptr),
      m_isDarkTheme(true),
      m_defaultColor(Qt::white) {
    setupUI();
    setTheme(true);
}

CategorySidebarWidget::~CategorySidebarWidget() = default;

void CategorySidebarWidget::setupUI() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    // Header section
    mainLayout->addWidget(createHeaderSection());

    // Color section
    mainLayout->addWidget(createColorSection());

    // Stroke width section
    mainLayout->addWidget(createStrokeWidthSection());

    // Size section
    mainLayout->addWidget(createSizeSection());

    // Separator
    QFrame* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);

    // Category section
    mainLayout->addWidget(createCategorySection());

    // Add stretch to push everything to the top
    mainLayout->addStretch();

    setLayout(mainLayout);
}

QWidget* CategorySidebarWidget::createHeaderSection() {
    QWidget* headerWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(headerWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QLabel* titleLabel = new QLabel("Customizer", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    m_resetButton = new QPushButton("Reset", this);
    m_resetButton->setMaximumWidth(80);
    m_resetButton->setToolTip("Reset all options to default");
    connect(m_resetButton, &QPushButton::clicked, this, &CategorySidebarWidget::onResetClicked);

    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(m_resetButton);

    headerWidget->setLayout(layout);
    return headerWidget;
}

QWidget* CategorySidebarWidget::createColorSection() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    m_colorLabel = new QLabel("Color", this);
    QFont labelFont = m_colorLabel->font();
    labelFont.setBold(true);
    m_colorLabel->setFont(labelFont);

    QHBoxLayout* colorRowLayout = new QHBoxLayout();
    colorRowLayout->setContentsMargins(0, 0, 0, 0);
    colorRowLayout->setSpacing(6);

    m_colorButton = new QPushButton(this);
    m_colorButton->setFixedSize(36, 36);
    m_colorButton->setToolTip("Click to change icon color");
    connect(m_colorButton, &QPushButton::clicked, this, &CategorySidebarWidget::onColorButtonClicked);

    colorRowLayout->addWidget(m_colorButton);
    colorRowLayout->addStretch();

    layout->addWidget(m_colorLabel);
    layout->addLayout(colorRowLayout);

    widget->setLayout(layout);
    return widget;
}

QWidget* CategorySidebarWidget::createStrokeWidthSection() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(0, 0, 0, 0);
    labelLayout->setSpacing(6);

    m_strokeWidthLabel = new QLabel("Stroke width", this);
    QFont labelFont = m_strokeWidthLabel->font();
    labelFont.setBold(true);
    m_strokeWidthLabel->setFont(labelFont);

    m_strokeWidthValueLabel = new QLabel("2px", this);
    m_strokeWidthValueLabel->setStyleSheet("color: #888;");

    labelLayout->addWidget(m_strokeWidthLabel);
    labelLayout->addStretch();
    labelLayout->addWidget(m_strokeWidthValueLabel);

    m_strokeWidthSlider = new QSlider(Qt::Horizontal, this);
    m_strokeWidthSlider->setMinimum(5);      // 0.5px
    m_strokeWidthSlider->setMaximum(40);     // 4.0px
    m_strokeWidthSlider->setValue(20);       // 2.0px (default)
    m_strokeWidthSlider->setTickPosition(QSlider::TicksBelow);
    m_strokeWidthSlider->setTickInterval(5);
    m_strokeWidthSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "  border: 1px solid #999;"
        "  height: 4px;"
        "  background: #444;"
        "  margin: 0px;"
        "  border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #0066CC;"
        "  border: 1px solid #0066CC;"
        "  width: 12px;"
        "  margin: -4px 0;"
        "  border-radius: 6px;"
        "}"
    );
    connect(m_strokeWidthSlider, &QSlider::valueChanged, this, &CategorySidebarWidget::onStrokeWidthChanged);

    layout->addLayout(labelLayout);
    layout->addWidget(m_strokeWidthSlider);

    widget->setLayout(layout);
    return widget;
}

QWidget* CategorySidebarWidget::createSizeSection() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(0, 0, 0, 0);
    labelLayout->setSpacing(6);

    m_sizeLabel = new QLabel("Size", this);
    QFont labelFont = m_sizeLabel->font();
    labelFont.setBold(true);
    m_sizeLabel->setFont(labelFont);

    m_sizeValueLabel = new QLabel("24px", this);
    m_sizeValueLabel->setStyleSheet("color: #888;");

    labelLayout->addWidget(m_sizeLabel);
    labelLayout->addStretch();
    labelLayout->addWidget(m_sizeValueLabel);

    m_sizeSlider = new QSlider(Qt::Horizontal, this);
    m_sizeSlider->setMinimum(16);
    m_sizeSlider->setMaximum(256);
    m_sizeSlider->setValue(24);
    m_sizeSlider->setTickPosition(QSlider::TicksBelow);
    m_sizeSlider->setTickInterval(32);
    m_sizeSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "  border: 1px solid #999;"
        "  height: 4px;"
        "  background: #444;"
        "  margin: 0px;"
        "  border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #0066CC;"
        "  border: 1px solid #0066CC;"
        "  width: 12px;"
        "  margin: -4px 0;"
        "  border-radius: 6px;"
        "}"
    );
    connect(m_sizeSlider, &QSlider::valueChanged, this, &CategorySidebarWidget::onSizeChanged);

    layout->addLayout(labelLayout);
    layout->addWidget(m_sizeSlider);

    widget->setLayout(layout);
    return widget;
}

QWidget* CategorySidebarWidget::createCategorySection() {
    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    // View header
    QLabel* viewLabel = new QLabel("View", this);
    QFont labelFont = viewLabel->font();
    labelFont.setBold(true);
    viewLabel->setFont(labelFont);
    layout->addWidget(viewLabel);

    // "All" button
    QListWidget* allListWidget = new QListWidget(this);
    QListWidgetItem* allItem = new QListWidgetItem("All", allListWidget);
    allItem->setData(Qt::UserRole, "all");
    allItem->setSelected(true);
    allListWidget->addItem(allItem);
    allListWidget->setMaximumHeight(36);
    allListWidget->setStyleSheet(
        "QListWidget::item:selected {"
        "  background-color: #0066CC;"
        "  color: white;"
        "}"
    );
    connect(allListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        emit showAllClicked();
        emit categorySelected("");
    });
    layout->addWidget(allListWidget);

    // Categories header
    QLabel* categoriesLabel = new QLabel("Categories", this);
    labelFont = categoriesLabel->font();
    labelFont.setPointSize(9);
    labelFont.setBold(true);
    categoriesLabel->setFont(labelFont);
    layout->addWidget(categoriesLabel);

    // Category list
    m_categoryListWidget = new QListWidget(this);
    m_categoryListWidget->setStyleSheet(
        "QListWidget::item {"
        "  padding: 4px;"
        "  height: 24px;"
        "}"
        "QListWidget::item:selected {"
        "  background-color: #0066CC;"
        "  color: white;"
        "}"
        "QListWidget::item:hover {"
        "  background-color: #333;"
        "}"
    );
    m_categoryListWidget->setSpacing(2);
    connect(m_categoryListWidget, &QListWidget::itemClicked, this, &CategorySidebarWidget::onCategoryClicked);
    layout->addWidget(m_categoryListWidget);

    widget->setLayout(layout);
    return widget;
}

void CategorySidebarWidget::setCategories(const QStringList& categories, const QMap<QString, int>& iconCounts) {
    m_categoryIconCounts = iconCounts;
    m_categoryListWidget->clear();

    for (const QString& category : categories) {
        int count = iconCounts.value(category, 0);
        QString displayText = QString("%1    %2").arg(category).arg(count);
        QListWidgetItem* item = new QListWidgetItem(displayText, m_categoryListWidget);
        item->setData(Qt::UserRole, category);
        m_categoryListWidget->addItem(item);
    }
}

void CategorySidebarWidget::setIconOptions(const IconOptions& options) {
    m_currentOptions = options;

    // Update color button
    m_colorButton->setStyleSheet(
        QString("background-color: %1; border: 1px solid #666; border-radius: 3px;")
            .arg(options.color.name())
    );

    // Update stroke width slider
    int sliderValue = static_cast<int>(options.strokeWidth * 10);
    m_strokeWidthSlider->blockSignals(true);
    m_strokeWidthSlider->setValue(sliderValue);
    m_strokeWidthSlider->blockSignals(false);
    updateStrokeWidthDisplay();

    // Update size slider
    m_sizeSlider->blockSignals(true);
    m_sizeSlider->setValue(options.size);
    m_sizeSlider->blockSignals(false);
    updateSizeDisplay();
}

IconOptions CategorySidebarWidget::getIconOptions() const {
    return m_currentOptions;
}

void CategorySidebarWidget::setTheme(bool isDark) {
    m_isDarkTheme = isDark;
    m_defaultColor = isDark ? Qt::white : Qt::black;

    if (m_currentOptions.color == (isDark ? Qt::black : Qt::white)) {
        m_currentOptions.color = m_defaultColor;
        updateColorButtonAppearance();
    }
}

void CategorySidebarWidget::onColorButtonClicked() {
    QColor color = QColorDialog::getColor(m_currentOptions.color, this, "Select Icon Color");
    if (color.isValid()) {
        m_currentOptions.color = color;
        updateColorButtonAppearance();
        emit optionsChanged(m_currentOptions);
    }
}

void CategorySidebarWidget::onStrokeWidthChanged(int value) {
    m_currentOptions.strokeWidth = value / 10.0;
    updateStrokeWidthDisplay();
    emit optionsChanged(m_currentOptions);
}

void CategorySidebarWidget::onSizeChanged(int value) {
    m_currentOptions.size = value;
    updateSizeDisplay();
    emit optionsChanged(m_currentOptions);
}

void CategorySidebarWidget::onCategoryClicked(QListWidgetItem* item) {
    QString category = item->data(Qt::UserRole).toString();
    emit categorySelected(category);
}

void CategorySidebarWidget::onResetClicked() {
    m_currentOptions = IconOptions();
    m_currentOptions.color = m_defaultColor;

    updateColorButtonAppearance();
    m_strokeWidthSlider->blockSignals(true);
    m_strokeWidthSlider->setValue(20);
    m_strokeWidthSlider->blockSignals(false);
    updateStrokeWidthDisplay();

    m_sizeSlider->blockSignals(true);
    m_sizeSlider->setValue(24);
    m_sizeSlider->blockSignals(false);
    updateSizeDisplay();

    emit optionsChanged(m_currentOptions);
}

void CategorySidebarWidget::updateColorButtonAppearance() {
    m_colorButton->setStyleSheet(
        QString("background-color: %1; border: 1px solid #666; border-radius: 3px;")
            .arg(m_currentOptions.color.name())
    );
}

void CategorySidebarWidget::updateStrokeWidthDisplay() {
    m_strokeWidthValueLabel->setText(QString("%1px").arg(m_currentOptions.strokeWidth, 0, 'f', 1));
}

void CategorySidebarWidget::updateSizeDisplay() {
    m_sizeValueLabel->setText(QString("%1px").arg(m_currentOptions.size));
}

} // namespace gallery
