/**
 * QtLucide Gallery Application - Modern Search Widget Implementation
 */

#include "ModernSearchWidget.h"
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QLinearGradient>
#include <QFontMetrics>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <algorithm>

// SearchInputWidget Implementation
SearchInputWidget::SearchInputWidget(QWidget* parent)
    : QFrame(parent)
    , m_layout(nullptr)
    , m_searchInput(nullptr)
    , m_clearButton(nullptr)
    , m_searchButton(nullptr)
    , m_searchIcon(nullptr)
    , m_suggestionsWidget(nullptr)
    , m_placeholderText("Search icons...")
    , m_animationsEnabled(true)
    , m_showClearButton(true)
    , m_showSearchIcon(true)
    , m_showSuggestions(true)
    , m_hasFocus(false)
    , m_focusAnimation(nullptr)
    , m_clearButtonAnimation(nullptr)
    , m_clearButtonOpacity(nullptr)
{
    setupUI();
    setupAnimations();

    // Create theme-aware widget
    m_themeWidget = new ThemeAwareWidget(this);

    // Apply initial theme
    applyTheme();
}

SearchInputWidget::~SearchInputWidget()
{
    if (m_focusAnimation) {
        m_focusAnimation->stop();
    }
    if (m_clearButtonAnimation) {
        m_clearButtonAnimation->stop();
    }
}

void SearchInputWidget::setSearchText(const QString& text)
{
    if (m_searchInput && m_searchInput->text() != text) {
        m_searchInput->setText(text);
        updateClearButtonVisibility();
    }
}

QString SearchInputWidget::searchText() const
{
    return m_searchInput ? m_searchInput->text() : QString();
}

void SearchInputWidget::clearSearch()
{
    if (m_searchInput) {
        m_searchInput->clear();
        updateClearButtonVisibility();
        emit searchCleared();
    }
}

void SearchInputWidget::focusSearch()
{
    if (m_searchInput) {
        m_searchInput->setFocus();
    }
}

void SearchInputWidget::setPlaceholderText(const QString& text)
{
    if (m_placeholderText == text) return;

    m_placeholderText = text;
    if (m_searchInput) {
        m_searchInput->setPlaceholderText(text);
    }
}

QString SearchInputWidget::placeholderText() const
{
    return m_placeholderText;
}

void SearchInputWidget::setSuggestions(const QStringList& suggestions)
{
    m_suggestions = suggestions;
    // TODO: Update suggestions widget
}

QStringList SearchInputWidget::suggestions() const
{
    return m_suggestions;
}

void SearchInputWidget::applyTheme()
{
    if (!ThemeManager::instance()) return;

    // Apply modern search input styling
    QString searchInputStyle = QString(
        "QLineEdit { "
        "    background-color: %1; "
        "    border: 2px solid %2; "
        "    border-radius: %3px; "
        "    padding: %4px %5px; "
        "    font-size: 14px; "
        "    color: %6; "
        "    selection-background-color: %7; "
        "} "
        "QLineEdit:focus { "
        "    border-color: %8; "
        "    background-color: %9; "
        "} "
        "QLineEdit:hover { "
        "    border-color: %10; "
        "}"
    ).arg(
        THEME_COLOR(InputBackground).name(),
        THEME_COLOR(BorderColor).name(),
        QString::number(BORDER_RADIUS),
        QString::number(PADDING / 2),
        QString::number(PADDING),
        THEME_COLOR(PrimaryText).name(),
        THEME_COLOR(AccentColor).name(),
        THEME_COLOR(AccentColor).name(),
        THEME_COLOR(InputBackground).lighter(105).name(),
        THEME_COLOR(BorderColor).lighter(120).name()
    );

    if (m_searchInput) {
        m_searchInput->setStyleSheet(searchInputStyle);
    }

    // Apply button styling
    QString buttonStyle = QString(
        "QPushButton { "
        "    background-color: transparent; "
        "    border: none; "
        "    border-radius: %1px; "
        "    padding: 4px; "
        "    color: %2; "
        "} "
        "QPushButton:hover { "
        "    background-color: %3; "
        "} "
        "QPushButton:pressed { "
        "    background-color: %4; "
        "}"
    ).arg(
        QString::number(CLEAR_BUTTON_SIZE / 2),
        THEME_COLOR(SecondaryText).name(),
        THEME_COLOR(HoverBackground).name(),
        THEME_COLOR(PressedBackground).name()
    );

    if (m_clearButton) {
        m_clearButton->setStyleSheet(buttonStyle);
    }
    if (m_searchButton) {
        m_searchButton->setStyleSheet(buttonStyle);
    }

    // Apply frame styling
    setStyleSheet(QString(
        "SearchInputWidget { "
        "    background-color: transparent; "
        "    border: none; "
        "}"
    ));
}

void SearchInputWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw background with subtle gradient
    QLinearGradient gradient(0, 0, 0, height());
    QColor baseColor = THEME_COLOR(InputBackground);
    gradient.setColorAt(0, baseColor.lighter(102));
    gradient.setColorAt(1, baseColor.darker(102));

    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), BORDER_RADIUS, BORDER_RADIUS);

    // Draw focus ring if focused
    if (m_hasFocus) {
        QPen focusPen(THEME_COLOR(AccentColor), 2);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), BORDER_RADIUS, BORDER_RADIUS);
    }

    QFrame::paintEvent(event);
}

void SearchInputWidget::setupUI()
{
    setFrameStyle(QFrame::NoFrame);
    setFixedHeight(44); // Modern search input height

    // Create main layout
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(PADDING, 0, PADDING, 0);
    m_layout->setSpacing(8);

    // Create search icon
    if (m_showSearchIcon) {
        m_searchIcon = new QLabel(this);
        m_searchIcon->setFixedSize(ICON_SIZE, ICON_SIZE);
        m_searchIcon->setAlignment(Qt::AlignCenter);
        m_searchIcon->setText("🔍"); // Unicode search icon, could be replaced with actual icon
        m_layout->addWidget(m_searchIcon);
    }

    // Create search input
    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText(m_placeholderText);
    m_searchInput->setFrame(false);
    m_searchInput->setAttribute(Qt::WA_MacShowFocusRect, false);
    m_layout->addWidget(m_searchInput, 1);

    // Create clear button
    if (m_showClearButton) {
        m_clearButton = new QPushButton(this);
        m_clearButton->setFixedSize(CLEAR_BUTTON_SIZE, CLEAR_BUTTON_SIZE);
        m_clearButton->setText("✕"); // Unicode X symbol
        m_clearButton->setVisible(false);
        m_clearButton->setToolTip("Clear search");

        // Add opacity effect for animations
        m_clearButtonOpacity = new QGraphicsOpacityEffect(this);
        m_clearButtonOpacity->setOpacity(0.0);
        m_clearButton->setGraphicsEffect(m_clearButtonOpacity);

        m_layout->addWidget(m_clearButton);
    }

    // Connect signals
    if (m_searchInput) {
        connect(m_searchInput, &QLineEdit::textChanged, this, &SearchInputWidget::onTextChanged);
        connect(m_searchInput, &QLineEdit::returnPressed, this, &SearchInputWidget::onSearchRequested);
        connect(m_searchInput, &QLineEdit::textChanged, this, &SearchInputWidget::searchTextChanged);
    }

    if (m_clearButton) {
        connect(m_clearButton, &QPushButton::clicked, this, &SearchInputWidget::onClearClicked);
    }
}

void SearchInputWidget::setupAnimations()
{
    if (!m_animationsEnabled) return;

    // Focus animation
    m_focusAnimation = new QPropertyAnimation(this, "geometry", this);
    m_focusAnimation->setDuration(ANIMATION_DURATION);
    m_focusAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // Clear button animation
    if (m_clearButtonOpacity) {
        m_clearButtonAnimation = new QPropertyAnimation(m_clearButtonOpacity, "opacity", this);
        m_clearButtonAnimation->setDuration(ANIMATION_DURATION);
        m_clearButtonAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }
}

void SearchInputWidget::onTextChanged()
{
    updateClearButtonVisibility();

    if (m_showSuggestions && !searchText().isEmpty()) {
        // TODO: Show suggestions popup
    } else {
        // TODO: Hide suggestions popup
    }
}

void SearchInputWidget::onClearClicked()
{
    clearSearch();
    focusSearch();
}

void SearchInputWidget::onSearchRequested()
{
    emit searchRequested(searchText());
}

void SearchInputWidget::updateClearButtonVisibility()
{
    if (!m_clearButton || !m_clearButtonAnimation) return;

    bool shouldShow = !searchText().isEmpty();
    bool isCurrentlyVisible = m_clearButton->isVisible();

    if (shouldShow && !isCurrentlyVisible) {
        m_clearButton->setVisible(true);
        m_clearButtonAnimation->setStartValue(0.0);
        m_clearButtonAnimation->setEndValue(1.0);
        m_clearButtonAnimation->start();
    } else if (!shouldShow && isCurrentlyVisible) {
        m_clearButtonAnimation->setStartValue(1.0);
        m_clearButtonAnimation->setEndValue(0.0);
        connect(m_clearButtonAnimation, &QPropertyAnimation::finished, this, [this]() {
            if (m_clearButton) {
                m_clearButton->setVisible(false);
            }
            disconnect(m_clearButtonAnimation, &QPropertyAnimation::finished, this, nullptr);
        });
        m_clearButtonAnimation->start();
    }
}

void SearchInputWidget::focusInEvent(QFocusEvent* event)
{
    m_hasFocus = true;
    update(); // Trigger repaint for focus ring
    emit focusReceived();
    QFrame::focusInEvent(event);
}

void SearchInputWidget::focusOutEvent(QFocusEvent* event)
{
    m_hasFocus = false;
    update(); // Trigger repaint to remove focus ring
    emit focusLost();
    QFrame::focusOutEvent(event);
}

void SearchInputWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
            clearSearch();
            event->accept();
            return;
        case Qt::Key_Down:
            // TODO: Navigate suggestions
            break;
        case Qt::Key_Up:
            // TODO: Navigate suggestions
            break;
    }

    QFrame::keyPressEvent(event);
}

// FilterPanelWidget Implementation
FilterPanelWidget::FilterPanelWidget(QWidget* parent)
    : QFrame(parent)
    , m_mainLayout(nullptr)
    , m_headerLayout(nullptr)
    , m_titleLabel(nullptr)
    , m_expandButton(nullptr)
    , m_clearButton(nullptr)
    , m_contentFrame(nullptr)
    , m_contentLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_categoryGroup(nullptr)
    , m_tagGroup(nullptr)
    , m_propertyGroup(nullptr)
    , m_customGroup(nullptr)
    , m_logicFrame(nullptr)
    , m_logicGroup(nullptr)
    , m_matchAllCheckbox(nullptr)
    , m_metadataManager(nullptr)
    , m_expanded(false)
    , m_animationsEnabled(true)
    , m_expansionAnimation(nullptr)
    , m_collapsedHeight(40)
    , m_expandedHeight(300)
{
    setupUI();

    // Create theme-aware widget
    m_themeWidget = new ThemeAwareWidget(this);

    // Apply initial theme
    applyTheme();
}

FilterPanelWidget::~FilterPanelWidget()
{
    if (m_expansionAnimation) {
        m_expansionAnimation->stop();
    }
}

void FilterPanelWidget::setFilterCriteria(const FilterCriteria& criteria)
{
    if (m_currentCriteria.categories == criteria.categories &&
        m_currentCriteria.tags == criteria.tags &&
        m_currentCriteria.properties == criteria.properties &&
        m_currentCriteria.customFilters == criteria.customFilters &&
        m_currentCriteria.matchAll == criteria.matchAll) {
        return;
    }

    m_currentCriteria = criteria;
    applyFilterCriteria();
}

FilterPanelWidget::FilterCriteria FilterPanelWidget::filterCriteria() const
{
    return m_currentCriteria;
}

void FilterPanelWidget::clearFilters()
{
    FilterCriteria emptyCriteria;
    setFilterCriteria(emptyCriteria);
    emit filtersCleared();
}

void FilterPanelWidget::setAvailableCategories(const QStringList& categories)
{
    if (m_availableCategories == categories) return;

    m_availableCategories = categories;
    setupCategoryFilter();
}

void FilterPanelWidget::setAvailableTags(const QStringList& tags)
{
    if (m_availableTags == tags) return;

    m_availableTags = tags;
    setupTagFilter();
}

void FilterPanelWidget::setExpanded(bool expanded)
{
    if (m_expanded == expanded) return;

    m_expanded = expanded;

    if (m_animationsEnabled) {
        animateExpansion();
    } else {
        setFixedHeight(expanded ? m_expandedHeight : m_collapsedHeight);
        if (m_contentFrame) {
            m_contentFrame->setVisible(expanded);
        }
    }

    // Update expand button text
    if (m_expandButton) {
        m_expandButton->setText(expanded ? "▼" : "▶");
    }

    emit expandedChanged(expanded);
}

void FilterPanelWidget::setupUI()
{
    setFrameStyle(QFrame::StyledPanel);
    setFixedHeight(m_collapsedHeight);

    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(8, 4, 8, 4);
    m_mainLayout->setSpacing(4);

    // Create header
    m_headerLayout = new QHBoxLayout();
    m_headerLayout->setSpacing(8);

    m_expandButton = new QPushButton("▶", this);
    m_expandButton->setFixedSize(24, 24);
    m_expandButton->setToolTip("Expand/Collapse filters");
    m_headerLayout->addWidget(m_expandButton);

    m_titleLabel = new QLabel("Filters", this);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_headerLayout->addWidget(m_titleLabel);

    m_headerLayout->addStretch();

    m_clearButton = new QPushButton("Clear", this);
    m_clearButton->setToolTip("Clear all filters");
    m_headerLayout->addWidget(m_clearButton);

    m_mainLayout->addLayout(m_headerLayout);

    // Create content frame (initially hidden)
    m_contentFrame = new QFrame(this);
    m_contentFrame->setVisible(false);
    m_contentLayout = new QVBoxLayout(m_contentFrame);
    m_contentLayout->setContentsMargins(0, 4, 0, 0);
    m_contentLayout->setSpacing(8);

    // Create scroll area for filters
    m_scrollArea = new QScrollArea(m_contentFrame);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(4, 4, 4, 4);
    scrollLayout->setSpacing(12);

    // Setup filter sections
    setupCategoryFilter();
    setupTagFilter();
    setupPropertyFilter();
    setupCustomFilter();

    // Add filter groups to scroll layout
    if (m_categoryGroup) scrollLayout->addWidget(m_categoryGroup);
    if (m_tagGroup) scrollLayout->addWidget(m_tagGroup);
    if (m_propertyGroup) scrollLayout->addWidget(m_propertyGroup);
    if (m_customGroup) scrollLayout->addWidget(m_customGroup);

    scrollLayout->addStretch();
    m_scrollArea->setWidget(scrollContent);
    m_contentLayout->addWidget(m_scrollArea);

    // Create logic controls
    m_logicFrame = new QFrame(m_contentFrame);
    m_logicLayout = new QHBoxLayout(m_logicFrame);
    m_logicLayout->setContentsMargins(4, 4, 4, 4);

    m_logicLabel = new QLabel("Match:", m_logicFrame);
    m_logicLayout->addWidget(m_logicLabel);

    m_matchAllCheckbox = new QCheckBox("All conditions (AND)", m_logicFrame);
    m_matchAllCheckbox->setToolTip("Check to require all conditions, uncheck for any condition (OR)");
    m_logicLayout->addWidget(m_matchAllCheckbox);

    m_logicLayout->addStretch();
    m_contentLayout->addWidget(m_logicFrame);

    m_mainLayout->addWidget(m_contentFrame);

    // Connect signals
    connect(m_expandButton, &QPushButton::clicked, this, &FilterPanelWidget::onExpandToggled);
    connect(m_clearButton, &QPushButton::clicked, this, &FilterPanelWidget::onClearFiltersClicked);
    connect(m_matchAllCheckbox, &QCheckBox::toggled, this, &FilterPanelWidget::onCustomFilterChanged);

    // Setup expansion animation
    if (m_animationsEnabled) {
        m_expansionAnimation = new QPropertyAnimation(this, "maximumHeight", this);
        m_expansionAnimation->setDuration(300);
        m_expansionAnimation->setEasingCurve(QEasingCurve::OutCubic);
    }
}

void FilterPanelWidget::setupCategoryFilter()
{
    if (!m_categoryGroup) {
        m_categoryGroup = new QGroupBox("Categories", this);
        m_categoryLayout = new QVBoxLayout(m_categoryGroup);
        m_categoryLayout->setContentsMargins(8, 8, 8, 8);
        m_categoryLayout->setSpacing(4);
    }

    // Clear existing checkboxes
    for (auto checkbox : m_categoryCheckboxes) {
        checkbox->deleteLater();
    }
    m_categoryCheckboxes.clear();

    // Create checkboxes for available categories
    for (const QString& category : m_availableCategories) {
        QCheckBox* checkbox = new QCheckBox(category, m_categoryGroup);
        checkbox->setToolTip(QString("Filter by %1 category").arg(category));
        connect(checkbox, &QCheckBox::toggled, this, &FilterPanelWidget::onCategoryFilterChanged);

        m_categoryCheckboxes[category] = checkbox;
        m_categoryLayout->addWidget(checkbox);
    }

    if (m_availableCategories.isEmpty()) {
        QLabel* emptyLabel = new QLabel("No categories available", m_categoryGroup);
        emptyLabel->setStyleSheet("color: gray; font-style: italic;");
        m_categoryLayout->addWidget(emptyLabel);
    }
}

void FilterPanelWidget::setupTagFilter()
{
    if (!m_tagGroup) {
        m_tagGroup = new QGroupBox("Tags", this);
        m_tagLayout = new QVBoxLayout(m_tagGroup);
        m_tagLayout->setContentsMargins(8, 8, 8, 8);
        m_tagLayout->setSpacing(4);
    }

    // Clear existing checkboxes
    for (auto checkbox : m_tagCheckboxes) {
        checkbox->deleteLater();
    }
    m_tagCheckboxes.clear();

    // Limit number of tags shown (show most common ones)
    QStringList tagsToShow = m_availableTags;
    if (tagsToShow.size() > 20) {
        tagsToShow = tagsToShow.mid(0, 20);
    }

    // Create checkboxes for available tags
    for (const QString& tag : tagsToShow) {
        QCheckBox* checkbox = new QCheckBox(tag, m_tagGroup);
        checkbox->setToolTip(QString("Filter by %1 tag").arg(tag));
        connect(checkbox, &QCheckBox::toggled, this, &FilterPanelWidget::onTagFilterChanged);

        m_tagCheckboxes[tag] = checkbox;
        m_tagLayout->addWidget(checkbox);
    }

    if (tagsToShow.isEmpty()) {
        QLabel* emptyLabel = new QLabel("No tags available", m_tagGroup);
        emptyLabel->setStyleSheet("color: gray; font-style: italic;");
        m_tagLayout->addWidget(emptyLabel);
    }
}

void FilterPanelWidget::setupPropertyFilter()
{
    if (!m_propertyGroup) {
        m_propertyGroup = new QGroupBox("Properties", this);
        m_propertyLayout = new QVBoxLayout(m_propertyGroup);
        m_propertyLayout->setContentsMargins(8, 8, 8, 8);
        m_propertyLayout->setSpacing(4);
    }

    // Clear existing checkboxes
    for (auto checkbox : m_propertyCheckboxes) {
        checkbox->deleteLater();
    }
    m_propertyCheckboxes.clear();

    // Create checkboxes for available properties
    for (const QString& property : m_availableProperties) {
        QCheckBox* checkbox = new QCheckBox(property, m_propertyGroup);
        checkbox->setToolTip(QString("Filter by %1 property").arg(property));
        connect(checkbox, &QCheckBox::toggled, this, &FilterPanelWidget::onPropertyFilterChanged);

        m_propertyCheckboxes[property] = checkbox;
        m_propertyLayout->addWidget(checkbox);
    }

    if (m_availableProperties.isEmpty()) {
        QLabel* emptyLabel = new QLabel("No properties available", m_propertyGroup);
        emptyLabel->setStyleSheet("color: gray; font-style: italic;");
        m_propertyLayout->addWidget(emptyLabel);
    }
}

void FilterPanelWidget::setupCustomFilter()
{
    if (!m_customGroup) {
        m_customGroup = new QGroupBox("Custom", this);
        m_customLayout = new QVBoxLayout(m_customGroup);
        m_customLayout->setContentsMargins(8, 8, 8, 8);
        m_customLayout->setSpacing(4);
    }

    // Add placeholder for custom filters
    QLabel* placeholderLabel = new QLabel("Custom filters will be added here", m_customGroup);
    placeholderLabel->setStyleSheet("color: gray; font-style: italic;");
    m_customLayout->addWidget(placeholderLabel);
}

void FilterPanelWidget::updateFilterCriteria()
{
    FilterCriteria newCriteria;

    // Collect selected categories
    for (auto it = m_categoryCheckboxes.begin(); it != m_categoryCheckboxes.end(); ++it) {
        if (it.value()->isChecked()) {
            newCriteria.categories.append(it.key());
        }
    }

    // Collect selected tags
    for (auto it = m_tagCheckboxes.begin(); it != m_tagCheckboxes.end(); ++it) {
        if (it.value()->isChecked()) {
            newCriteria.tags.append(it.key());
        }
    }

    // Collect selected properties
    for (auto it = m_propertyCheckboxes.begin(); it != m_propertyCheckboxes.end(); ++it) {
        if (it.value()->isChecked()) {
            newCriteria.properties.append(it.key());
        }
    }

    // Get match logic
    newCriteria.matchAll = m_matchAllCheckbox ? m_matchAllCheckbox->isChecked() : false;

    // Update current criteria and emit signal
    if (m_currentCriteria.categories != newCriteria.categories ||
        m_currentCriteria.tags != newCriteria.tags ||
        m_currentCriteria.properties != newCriteria.properties ||
        m_currentCriteria.matchAll != newCriteria.matchAll) {

        m_currentCriteria = newCriteria;
        emit filterChanged(m_currentCriteria);
    }
}

void FilterPanelWidget::applyFilterCriteria()
{
    // Update category checkboxes
    for (auto it = m_categoryCheckboxes.begin(); it != m_categoryCheckboxes.end(); ++it) {
        it.value()->setChecked(m_currentCriteria.categories.contains(it.key()));
    }

    // Update tag checkboxes
    for (auto it = m_tagCheckboxes.begin(); it != m_tagCheckboxes.end(); ++it) {
        it.value()->setChecked(m_currentCriteria.tags.contains(it.key()));
    }

    // Update property checkboxes
    for (auto it = m_propertyCheckboxes.begin(); it != m_propertyCheckboxes.end(); ++it) {
        it.value()->setChecked(m_currentCriteria.properties.contains(it.key()));
    }

    // Update match logic
    if (m_matchAllCheckbox) {
        m_matchAllCheckbox->setChecked(m_currentCriteria.matchAll);
    }
}

void FilterPanelWidget::animateExpansion()
{
    if (!m_expansionAnimation) return;

    int startHeight = m_expanded ? m_collapsedHeight : m_expandedHeight;
    int endHeight = m_expanded ? m_expandedHeight : m_collapsedHeight;

    m_expansionAnimation->setStartValue(startHeight);
    m_expansionAnimation->setEndValue(endHeight);

    if (m_expanded) {
        // Show content before expanding
        if (m_contentFrame) {
            m_contentFrame->setVisible(true);
        }
    } else {
        // Hide content after collapsing
        connect(m_expansionAnimation, &QPropertyAnimation::finished, this, [this]() {
            if (m_contentFrame && !m_expanded) {
                m_contentFrame->setVisible(false);
            }
            disconnect(m_expansionAnimation, &QPropertyAnimation::finished, this, nullptr);
        });
    }

    m_expansionAnimation->start();
}

void FilterPanelWidget::applyTheme()
{
    if (!ThemeManager::instance()) return;

    // Apply theme to the filter panel
    QString panelStyle = QString(
        "FilterPanelWidget { "
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: 8px; "
        "} "
        "QGroupBox { "
        "    font-weight: bold; "
        "    border: 1px solid %3; "
        "    border-radius: 4px; "
        "    margin-top: 8px; "
        "    padding-top: 4px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 8px; "
        "    padding: 0 4px 0 4px; "
        "}"
    ).arg(
        THEME_COLOR(PanelBackground).name(),
        THEME_COLOR(BorderColor).name(),
        THEME_COLOR(BorderColor).name()
    );

    setStyleSheet(panelStyle);
}

void FilterPanelWidget::onCategoryFilterChanged()
{
    updateFilterCriteria();
}

void FilterPanelWidget::onTagFilterChanged()
{
    updateFilterCriteria();
}

void FilterPanelWidget::onPropertyFilterChanged()
{
    updateFilterCriteria();
}

void FilterPanelWidget::onCustomFilterChanged()
{
    updateFilterCriteria();
}

void FilterPanelWidget::onClearFiltersClicked()
{
    clearFilters();
}

void FilterPanelWidget::onExpandToggled()
{
    setExpanded(!m_expanded);
}
