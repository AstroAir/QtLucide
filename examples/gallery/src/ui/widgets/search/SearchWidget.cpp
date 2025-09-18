/**
 * QtLucide Gallery Application - Search Widget Implementation
 */

#include "SearchWidget.h"
#include "../../../core/utils/ErrorHandler.h"
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
void SearchLineEdit::onSearchTimer()
{
    QString searchText = text().trimmed();
    if (!searchText.isEmpty()) {
        // Add to search history if not already present
        if (!m_searchHistory.contains(searchText)) {
            m_searchHistory.prepend(searchText);
            // Limit history size
            if (m_searchHistory.size() > 20) {
                m_searchHistory.removeLast();
            }
            updateCompleter();
        }

        // Emit search signal
        emit searchRequested(searchText);
    }
}

void SearchLineEdit::onHistoryAction()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QString searchText = action->data().toString();
        setText(searchText);
        emit searchRequested(searchText);
    }
}

void SearchLineEdit::onClearHistoryAction()
{
    m_searchHistory.clear();
    updateCompleter();
    emit historyCleared();
}

void SearchLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu* menu = createStandardContextMenu();

    if (!m_searchHistory.isEmpty()) {
        menu->addSeparator();

        // Add search history submenu
        QMenu* historyMenu = menu->addMenu(tr("Search History"));
        for (const QString& searchText : m_searchHistory) {
            QAction* historyAction = historyMenu->addAction(searchText);
            historyAction->setData(searchText);
            connect(historyAction, &QAction::triggered, this, &SearchLineEdit::onHistoryAction);
        }

        historyMenu->addSeparator();
        QAction* clearHistoryAction = historyMenu->addAction(tr("Clear History"));
        connect(clearHistoryAction, &QAction::triggered, this, &SearchLineEdit::onClearHistoryAction);
    }

    menu->exec(event->globalPos());
    delete menu;
}

void AdvancedSearchPanel::expandPanel()
{
    if (!m_expanded) {
        m_expanded = true;

        // Create expand animation
        QPropertyAnimation* animation = new QPropertyAnimation(this, "maximumHeight");
        animation->setDuration(250);
        animation->setStartValue(m_collapsedHeight);
        animation->setEndValue(m_expandedHeight);
        animation->setEasingCurve(QEasingCurve::OutCubic);

        connect(animation, &QPropertyAnimation::finished, this, &AdvancedSearchPanel::onAnimationFinished);
        animation->start(QAbstractAnimation::DeleteWhenStopped);

        emit panelExpanded();
    }
}

void AdvancedSearchPanel::collapsePanel()
{
    if (m_expanded) {
        m_expanded = false;

        // Create collapse animation
        QPropertyAnimation* animation = new QPropertyAnimation(this, "maximumHeight");
        animation->setDuration(200);
        animation->setStartValue(m_expandedHeight);
        animation->setEndValue(m_collapsedHeight);
        animation->setEasingCurve(QEasingCurve::InCubic);

        connect(animation, &QPropertyAnimation::finished, this, &AdvancedSearchPanel::onAnimationFinished);
        animation->start(QAbstractAnimation::DeleteWhenStopped);

        emit panelCollapsed();
    }
}

void AdvancedSearchPanel::onContributorSelectionChanged()
{
    QStringList selectedContributors;

    // Collect selected contributors from UI controls
    // This would depend on the actual UI implementation
    // For now, emit the signal with empty list

    emit contributorFilterChanged(selectedContributors);
    emitFiltersChanged();
}

void AdvancedSearchPanel::onDateRangeChanged()
{
    // Handle date range changes
    QDate startDate = QDate::currentDate().addYears(-1); // Default to last year
    QDate endDate = QDate::currentDate();

    // This would get actual dates from date picker widgets
    emit dateRangeChanged(startDate, endDate);
    emitFiltersChanged();
}

void AdvancedSearchPanel::onUsageRangeChanged()
{
    // Handle usage frequency range changes
    int minUsage = 0;
    int maxUsage = 100;

    // This would get actual values from range slider widgets
    emit usageRangeChanged(minUsage, maxUsage);
    emitFiltersChanged();
}

void AdvancedSearchPanel::onSavePreset()
{
    bool ok;
    QString presetName = QInputDialog::getText(
        this,
        tr("Save Search Preset"),
        tr("Enter preset name:"),
        QLineEdit::Normal,
        QString(),
        &ok
    );

    if (ok && !presetName.isEmpty()) {
        // Save current search criteria as preset
        SearchCriteria criteria = getCurrentCriteria();
        m_savedPresets[presetName] = criteria;

        // Save to settings
        QSettings settings;
        settings.beginGroup("SearchPresets");
        settings.setValue(presetName + "/searchText", criteria.searchText);
        settings.setValue(presetName + "/categories", criteria.categories);
        settings.setValue(presetName + "/tags", criteria.tags);
        settings.setValue(presetName + "/contributors", criteria.contributors);
        settings.endGroup();

        emit presetSaved(presetName);
    }
}

void AdvancedSearchPanel::onLoadPreset()
{
    QSettings settings;
    settings.beginGroup("SearchPresets");
    QStringList presetNames = settings.childGroups();
    settings.endGroup();

    if (presetNames.isEmpty()) {
        QMessageBox::information(this, tr("Load Preset"), tr("No saved presets found."));
        return;
    }

    bool ok;
    QString selectedPreset = QInputDialog::getItem(
        this,
        tr("Load Search Preset"),
        tr("Select preset to load:"),
        presetNames,
        0,
        false,
        &ok
    );

    if (ok && !selectedPreset.isEmpty()) {
        // Load preset from settings
        settings.beginGroup("SearchPresets");
        SearchCriteria criteria;
        criteria.searchText = settings.value(selectedPreset + "/searchText").toString();
        criteria.categories = settings.value(selectedPreset + "/categories").toStringList();
        criteria.tags = settings.value(selectedPreset + "/tags").toStringList();
        criteria.contributors = settings.value(selectedPreset + "/contributors").toStringList();
        settings.endGroup();

        // Apply loaded criteria
        applyCriteria(criteria);
        emit presetLoaded(selectedPreset);
    }
}

void AdvancedSearchPanel::onDeletePreset()
{
    QSettings settings;
    settings.beginGroup("SearchPresets");
    QStringList presetNames = settings.childGroups();
    settings.endGroup();

    if (presetNames.isEmpty()) {
        QMessageBox::information(this, tr("Delete Preset"), tr("No saved presets found."));
        return;
    }

    bool ok;
    QString selectedPreset = QInputDialog::getItem(
        this,
        tr("Delete Search Preset"),
        tr("Select preset to delete:"),
        presetNames,
        0,
        false,
        &ok
    );

    if (ok && !selectedPreset.isEmpty()) {
        int ret = QMessageBox::question(
            this,
            tr("Delete Preset"),
            tr("Are you sure you want to delete the preset '%1'?").arg(selectedPreset),
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (ret == QMessageBox::Yes) {
            // Remove from settings
            settings.beginGroup("SearchPresets");
            settings.remove(selectedPreset);
            settings.endGroup();

            // Remove from memory
            m_savedPresets.remove(selectedPreset);

            emit presetDeleted(selectedPreset);
        }
    }
}

void AdvancedSearchPanel::onAnimationFinished()
{
    // Animation completed - update UI state if needed
    update();
}

void SearchWidget::performSearch()
{
    QString searchText = m_searchEdit->text().trimmed();

    if (searchText.isEmpty()) {
        // Clear search results
        emit searchCleared();
        return;
    }

    // Update current criteria
    m_currentCriteria.searchText = searchText;

    // Add to search history
    if (!m_searchHistory.contains(searchText)) {
        m_searchHistory.prepend(searchText);
        if (m_searchHistory.size() > 20) {
            m_searchHistory.removeLast();
        }
        updateSearchHistory();
    }

    // Emit search signal
    emit searchRequested(searchText);
    emit filtersChanged(m_currentCriteria);

    // Update statistics
    onUpdateStatistics();
}

void SearchWidget::saveCurrentSearch()
{
    QString searchText = m_searchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        return;
    }

    bool ok;
    QString searchName = QInputDialog::getText(
        this,
        tr("Save Search"),
        tr("Enter search name:"),
        QLineEdit::Normal,
        searchText,
        &ok
    );

    if (ok && !searchName.isEmpty()) {
        // Save current search criteria
        QSettings settings;
        settings.beginGroup("SavedSearches");
        settings.setValue(searchName + "/searchText", m_currentCriteria.searchText);
        settings.setValue(searchName + "/categories", m_currentCriteria.categories);
        settings.setValue(searchName + "/tags", m_currentCriteria.tags);
        settings.setValue(searchName + "/contributors", m_currentCriteria.contributors);
        settings.setValue(searchName + "/timestamp", QDateTime::currentDateTime());
        settings.endGroup();

        emit searchSaved(searchName);
    }
}

void SearchWidget::loadSavedSearch(const QString& name)
{
    if (name.isEmpty()) {
        return;
    }

    QSettings settings;
    settings.beginGroup("SavedSearches");

    if (!settings.childGroups().contains(name)) {
        return;
    }

    // Load search criteria
    SearchCriteria criteria;
    criteria.searchText = settings.value(name + "/searchText").toString();
    criteria.categories = settings.value(name + "/categories").toStringList();
    criteria.tags = settings.value(name + "/tags").toStringList();
    criteria.contributors = settings.value(name + "/contributors").toStringList();

    settings.endGroup();

    // Apply loaded criteria
    m_searchEdit->setText(criteria.searchText);
    m_currentCriteria = criteria;

    // Apply to advanced panel if available
    if (m_advancedPanel) {
        m_advancedPanel->applyCriteria(criteria);
    }

    // Perform the search
    performSearch();

    emit searchLoaded(name);
}

void SearchWidget::onSearchTimer()
{
    // Delayed search execution
    performSearch();
}

void SearchWidget::onUpdateStatistics()
{
    // Update search statistics
    int totalSearches = m_searchHistory.size();
    QString currentSearch = m_searchEdit->text().trimmed();

    // This would update a statistics display if available
    emit statisticsUpdated(totalSearches, currentSearch);
}

void SearchWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // Adjust layout based on new size
    if (event->size().width() < 400) {
        // Compact layout for narrow widths
        if (m_advancedPanel && m_advancedPanel->isVisible()) {
            m_advancedPanel->collapsePanel();
        }
    }
}

// Helper method implementations
void SearchLineEdit::updateCompleter()
{
    if (m_completer && m_completerModel) {
        m_completerModel->setStringList(m_searchHistory);
        m_completer->setModel(m_completerModel);
    }
}

SearchCriteria AdvancedSearchPanel::getCurrentCriteria() const
{
    SearchCriteria criteria;
    // This would collect criteria from actual UI controls
    // For now, return empty criteria
    return criteria;
}

void AdvancedSearchPanel::applyCriteria(const SearchCriteria& criteria)
{
    // This would apply criteria to actual UI controls
    // For now, just store the criteria
    Q_UNUSED(criteria)
}

void SearchWidget::updateSearchHistory()
{
    // Update search history in UI if there's a history widget
    // This would update a dropdown or list widget showing recent searches
}

// Only methods that are actually declared in headers are implemented above
