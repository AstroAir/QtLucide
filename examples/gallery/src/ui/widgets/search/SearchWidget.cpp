/**
 * QtLucide Gallery Application - Search Widget Implementation
 */

#include "SearchWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

// SearchLineEdit Implementation
SearchLineEdit::SearchLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , m_metadataManager(nullptr)
    , m_completer(nullptr)
    , m_completerModel(new QStringListModel(this))
    , m_searchTimer(new QTimer(this))
    , m_suggestionLimit(DEFAULT_SUGGESTION_LIMIT)
{
    setPlaceholderText("Search icons...");
    setupCompleter();

    m_searchTimer->setSingleShot(true);
    m_searchTimer->setInterval(DEFAULT_SEARCH_DELAY);

    connect(this, &QLineEdit::textChanged, this, &SearchLineEdit::onTextChanged);
    connect(m_searchTimer, &QTimer::timeout, this, &SearchLineEdit::updateSuggestions);
}

void SearchLineEdit::setupCompleter()
{
    m_completer = new QCompleter(m_completerModel, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchContains);
    setCompleter(m_completer);

    connect(m_completer, QOverload<const QString&>::of(&QCompleter::activated),
            this, &SearchLineEdit::onSuggestionActivated);
}

void SearchLineEdit::setMetadataManager(IconMetadataManager* manager)
{
    m_metadataManager = manager;
    updateCompleterModel();
}

void SearchLineEdit::onTextChanged(const QString& text)
{
    emit searchTextChanged(text);
    m_searchTimer->start();
}

void SearchLineEdit::onSuggestionActivated(const QString& text)
{
    emit suggestionSelected(text);
}

void SearchLineEdit::updateSuggestions()
{
    if (!m_metadataManager) return;

    QString text = this->text();
    if (text.length() >= 2) {
        QStringList suggestions = m_metadataManager->getSearchSuggestions(text, m_suggestionLimit);
        m_completerModel->setStringList(suggestions);
    }
}

void SearchLineEdit::updateCompleterModel()
{
    if (!m_metadataManager) return;

    QStringList allIcons = m_metadataManager->getAllIconNames();
    m_completerModel->setStringList(allIcons);
}

void SearchLineEdit::clearSearch()
{
    clear();
    emit searchCleared();
}

void SearchLineEdit::focusSearch()
{
    setFocus();
    selectAll();
}

// AdvancedSearchPanel Implementation
AdvancedSearchPanel::AdvancedSearchPanel(QWidget *parent)
    : QFrame(parent)
    , m_metadataManager(nullptr)
    , m_isExpanded(false)
{
    setFrameStyle(QFrame::StyledPanel);
    setVisible(false);
    setupUI();
}

void AdvancedSearchPanel::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    // Category filter
    m_categoryGroup = new QGroupBox("Categories", this);
    m_categoryList = new QListWidget(this);
    m_categoryList->setMaximumHeight(100);
    QVBoxLayout* categoryLayout = new QVBoxLayout(m_categoryGroup);
    categoryLayout->addWidget(m_categoryList);

    // Tag filter
    m_tagGroup = new QGroupBox("Tags", this);
    m_tagList = new QListWidget(this);
    m_tagList->setMaximumHeight(100);
    QVBoxLayout* tagLayout = new QVBoxLayout(m_tagGroup);
    tagLayout->addWidget(m_tagList);

    // Sort options
    m_sortGroup = new QGroupBox("Sort", this);
    m_sortCombo = new QComboBox(this);
    m_sortCombo->addItems({"Name", "Category", "Usage", "Recent"});
    m_sortAscending = new QCheckBox("Ascending", this);
    m_sortAscending->setChecked(true);
    QVBoxLayout* sortLayout = new QVBoxLayout(m_sortGroup);
    sortLayout->addWidget(m_sortCombo);
    sortLayout->addWidget(m_sortAscending);

    // Filter options
    m_optionsGroup = new QGroupBox("Options", this);
    m_favoritesOnly = new QCheckBox("Favorites only", this);
    m_recentOnly = new QCheckBox("Recently used only", this);
    QVBoxLayout* optionsLayout = new QVBoxLayout(m_optionsGroup);
    optionsLayout->addWidget(m_favoritesOnly);
    optionsLayout->addWidget(m_recentOnly);

    // Clear button
    m_clearButton = new QPushButton(this);
    m_clearButton->setText("Clear Filters");

    m_mainLayout->addWidget(m_categoryGroup);
    m_mainLayout->addWidget(m_tagGroup);
    m_mainLayout->addWidget(m_sortGroup);
    m_mainLayout->addWidget(m_optionsGroup);
    m_mainLayout->addWidget(m_clearButton);
    m_mainLayout->addStretch();

    // Connect signals
    connect(m_clearButton, &QToolButton::clicked, this, &AdvancedSearchPanel::onClearFilters);
    connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AdvancedSearchPanel::onSortOrderChanged);
    connect(m_sortAscending, &QCheckBox::toggled, this, &AdvancedSearchPanel::onSortOrderChanged);
    connect(m_favoritesOnly, &QCheckBox::toggled, this, &AdvancedSearchPanel::onFilterOptionsChanged);
    connect(m_recentOnly, &QCheckBox::toggled, this, &AdvancedSearchPanel::onFilterOptionsChanged);
}

void AdvancedSearchPanel::setMetadataManager(IconMetadataManager* manager)
{
    m_metadataManager = manager;
    updateCategoryList();
    updateTagList();
}

void AdvancedSearchPanel::updateCategoryList()
{
    if (!m_metadataManager) return;

    m_categoryList->clear();
    QStringList categories = m_metadataManager->getAllCategories();
    for (const QString& category : categories) {
        QListWidgetItem* item = new QListWidgetItem(category, m_categoryList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
}

void AdvancedSearchPanel::updateTagList()
{
    if (!m_metadataManager) return;

    m_tagList->clear();
    QStringList tags = m_metadataManager->getAllTags();
    // Show only first 50 tags to avoid overwhelming the UI
    tags = tags.mid(0, 50);
    for (const QString& tag : tags) {
        QListWidgetItem* item = new QListWidgetItem(tag, m_tagList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
    }
}

IconFilterCriteria AdvancedSearchPanel::getFilterCriteria() const
{
    IconFilterCriteria criteria;

    // Get selected categories
    for (int i = 0; i < m_categoryList->count(); ++i) {
        QListWidgetItem* item = m_categoryList->item(i);
        if (item->checkState() == Qt::Checked) {
            criteria.categories.append(item->text());
        }
    }

    // Get selected tags
    for (int i = 0; i < m_tagList->count(); ++i) {
        QListWidgetItem* item = m_tagList->item(i);
        if (item->checkState() == Qt::Checked) {
            criteria.tags.append(item->text());
        }
    }

    // Get sort options
    criteria.sortOrder = static_cast<IconFilterCriteria::SortOrder>(m_sortCombo->currentIndex());
    criteria.sortAscending = m_sortAscending->isChecked();

    // Get filter options
    criteria.favoritesOnly = m_favoritesOnly->isChecked();
    criteria.recentlyUsedOnly = m_recentOnly->isChecked();

    return criteria;
}

void AdvancedSearchPanel::clearFilters()
{
    // Clear category selections
    for (int i = 0; i < m_categoryList->count(); ++i) {
        m_categoryList->item(i)->setCheckState(Qt::Unchecked);
    }

    // Clear tag selections
    for (int i = 0; i < m_tagList->count(); ++i) {
        m_tagList->item(i)->setCheckState(Qt::Unchecked);
    }

    // Reset options
    m_favoritesOnly->setChecked(false);
    m_recentOnly->setChecked(false);
    m_sortCombo->setCurrentIndex(0);
    m_sortAscending->setChecked(true);

    emit filtersCleared();
}

void AdvancedSearchPanel::onClearFilters()
{
    clearFilters();
}

void AdvancedSearchPanel::onSortOrderChanged()
{
    emitFiltersChanged();
}

void AdvancedSearchPanel::onFilterOptionsChanged()
{
    emitFiltersChanged();
}

void AdvancedSearchPanel::emitFiltersChanged()
{
    emit filtersChanged(getFilterCriteria());
}

void AdvancedSearchPanel::showPanel()
{
    setVisible(true);
    m_isExpanded = true;
}

void AdvancedSearchPanel::hidePanel()
{
    setVisible(false);
    m_isExpanded = false;
}

void AdvancedSearchPanel::togglePanel()
{
    if (m_isExpanded) {
        hidePanel();
    } else {
        showPanel();
    }
}

// SearchWidget Implementation
SearchWidget::SearchWidget(IconMetadataManager* metadataManager, QWidget *parent)
    : QWidget(parent)
    , m_metadataManager(metadataManager)
{
    setupUI();

    connect(m_searchEdit, &SearchLineEdit::searchTextChanged,
            this, &SearchWidget::onSearchTextChanged);
    connect(m_clearButton, &QToolButton::clicked,
            this, &SearchWidget::onSearchCleared);
    connect(m_advancedButton, &QToolButton::clicked,
            this, &SearchWidget::onToggleAdvanced);
    connect(m_advancedPanel, &AdvancedSearchPanel::filtersChanged,
            this, &SearchWidget::onAdvancedFiltersChanged);
}

SearchWidget::~SearchWidget()
{
}

void SearchWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);

    // Search bar
    m_searchBarLayout = new QHBoxLayout();

    m_searchEdit = new SearchLineEdit(this);
    m_searchEdit->setMetadataManager(m_metadataManager);

    m_clearButton = new QToolButton(this);
    m_clearButton->setText("Clear");

    m_advancedButton = new QToolButton(this);
    m_advancedButton->setText("Advanced");
    m_advancedButton->setCheckable(true);

    m_searchBarLayout->addWidget(m_searchEdit, 1);
    m_searchBarLayout->addWidget(m_clearButton);
    m_searchBarLayout->addWidget(m_advancedButton);

    // Advanced panel
    m_advancedPanel = new AdvancedSearchPanel(this);
    m_advancedPanel->setMetadataManager(m_metadataManager);

    m_mainLayout->addLayout(m_searchBarLayout);
    m_mainLayout->addWidget(m_advancedPanel);
}

QString SearchWidget::searchText() const
{
    return m_searchEdit->text();
}

IconFilterCriteria SearchWidget::getFilterCriteria() const
{
    IconFilterCriteria criteria = m_advancedPanel->getFilterCriteria();
    criteria.searchText = searchText();
    return criteria;
}

void SearchWidget::clearSearch()
{
    m_searchEdit->clearSearch();
}

void SearchWidget::clearAllFilters()
{
    clearSearch();
    m_advancedPanel->clearFilters();
    emit filtersCleared();
}

void SearchWidget::onSearchTextChanged(const QString& text)
{
    m_currentCriteria.searchText = text;
    emit searchChanged(text);
    emit filtersChanged(m_currentCriteria);
}

void SearchWidget::onSearchCleared()
{
    emit searchCleared();
}

void SearchWidget::onToggleAdvanced()
{
    m_advancedPanel->togglePanel();
}

void SearchWidget::onAdvancedFiltersChanged(const IconFilterCriteria& criteria)
{
    m_currentCriteria = criteria;
    m_currentCriteria.searchText = searchText();
    emit filtersChanged(m_currentCriteria);
}

// Missing method implementations
void SearchWidget::focusSearchInput() { m_searchEdit->setFocus(); }
void SearchWidget::showAdvancedOptions() { m_advancedPanel->showPanel(); }
void SearchWidget::hideAdvancedOptions() { m_advancedPanel->hidePanel(); }
void SearchWidget::toggleAdvancedOptions() { m_advancedPanel->togglePanel(); }
void SearchWidget::onFiltersCleared() { emit filtersCleared(); }
void SearchWidget::showEvent(QShowEvent *event) { QWidget::showEvent(event); }
void SearchWidget::hideEvent(QHideEvent *event) { QWidget::hideEvent(event); }

bool SearchWidget::isAdvancedPanelVisible() const {
    return m_advancedPanel && m_advancedPanel->isVisible();
}

// SearchLineEdit missing methods
void SearchLineEdit::keyPressEvent(QKeyEvent *event) { QLineEdit::keyPressEvent(event); }
void SearchLineEdit::focusInEvent(QFocusEvent *event) { QLineEdit::focusInEvent(event); }
void SearchLineEdit::focusOutEvent(QFocusEvent *event) { QLineEdit::focusOutEvent(event); }

// AdvancedSearchPanel missing methods
void AdvancedSearchPanel::onCategorySelectionChanged() { emitFiltersChanged(); }
void AdvancedSearchPanel::onTagSelectionChanged() { emitFiltersChanged(); }

// QuickFilterWidget implementation removed - class not defined in header

// Additional missing SearchWidget methods (only those declared in header)

// Missing methods that are called from other code
void SearchLineEdit::selectAll() {
    QLineEdit::selectAll();
}

// Missing destructors
SearchLineEdit::~SearchLineEdit() = default;
AdvancedSearchPanel::~AdvancedSearchPanel() = default;

// Missing methods declared in headers but not implemented
void SearchLineEdit::onSearchTimer() { /* TODO */ }
void SearchLineEdit::onHistoryAction() { /* TODO */ }
void SearchLineEdit::onClearHistoryAction() { /* TODO */ }
void SearchLineEdit::contextMenuEvent(QContextMenuEvent *event) { QLineEdit::contextMenuEvent(event); }

void AdvancedSearchPanel::expandPanel() { /* TODO */ }
void AdvancedSearchPanel::collapsePanel() { /* TODO */ }
void AdvancedSearchPanel::onContributorSelectionChanged() { /* TODO */ }
void AdvancedSearchPanel::onDateRangeChanged() { /* TODO */ }
void AdvancedSearchPanel::onUsageRangeChanged() { /* TODO */ }
void AdvancedSearchPanel::onSavePreset() { /* TODO */ }
void AdvancedSearchPanel::onLoadPreset() { /* TODO */ }
void AdvancedSearchPanel::onDeletePreset() { /* TODO */ }
void AdvancedSearchPanel::onAnimationFinished() { /* TODO */ }

void SearchWidget::performSearch() { /* TODO */ }
void SearchWidget::saveCurrentSearch() { /* TODO */ }
void SearchWidget::loadSavedSearch(const QString& name) { Q_UNUSED(name) /* TODO */ }
void SearchWidget::onSearchTimer() { /* TODO */ }
void SearchWidget::onUpdateStatistics() { /* TODO */ }
void SearchWidget::resizeEvent(QResizeEvent *event) { QWidget::resizeEvent(event); }

// Only methods that are actually declared in headers are implemented above
