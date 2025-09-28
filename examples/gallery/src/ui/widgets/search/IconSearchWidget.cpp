/**
 * QtLucide Gallery Application - Icon Search Widget Implementation
 */

#include "IconSearchWidget.h"
#include "GalleryLogger.h"
#include "IconMetadataManager.h"

#include <QAction>
#include <QApplication>
#include <QCompleter>
#include <QDebug>
#include <QFocusEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QPushButton>
#include <QShortcut>
#include <QStringListModel>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

// Static constant definitions (required for ODR-used constants)
const int SearchSuggestionsWidget::MAX_SUGGESTIONS;
const int SearchSuggestionsWidget::ITEM_HEIGHT;
const int IconSearchWidget::DEFAULT_SEARCH_DELAY;
const int IconSearchWidget::MAX_HISTORY_ITEMS;
const int IconSearchWidget::MAX_SUGGESTIONS;

// SearchSuggestionsWidget implementation
SearchSuggestionsWidget::SearchSuggestionsWidget(QWidget* parent)
    : QFrame(parent), m_currentIndex(-1) {
    setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    setWindowFlags(Qt::Popup);
    setAttribute(Qt::WA_DeleteOnClose, false);

    setupUI();
}

void SearchSuggestionsWidget::setupUI() {
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(0);

    m_listWidget = new QListWidget;
    m_listWidget->setFrameStyle(QFrame::NoFrame);
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(m_listWidget, &QListWidget::itemClicked, this, &SearchSuggestionsWidget::onItemClicked);

    m_layout->addWidget(m_listWidget);

    setMaximumHeight(MAX_SUGGESTIONS * ITEM_HEIGHT + 10);
}

void SearchSuggestionsWidget::setSuggestions(const QStringList& suggestions) {
    m_listWidget->clear();
    m_currentIndex = -1;

    int count = qMin(static_cast<int>(suggestions.size()), MAX_SUGGESTIONS);
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = new QListWidgetItem(suggestions[i]);
        item->setSizeHint(QSize(0, ITEM_HEIGHT));
        m_listWidget->addItem(item);
    }

    if (count > 0) {
        setCurrentSuggestion(0);
    }
}

void SearchSuggestionsWidget::setCurrentSuggestion(int index) {
    if (index >= 0 && index < m_listWidget->count()) {
        m_currentIndex = index;
        m_listWidget->setCurrentRow(index);
    }
}

QString SearchSuggestionsWidget::selectedSuggestion() const {
    if (m_currentIndex >= 0 && m_currentIndex < m_listWidget->count()) {
        return m_listWidget->item(m_currentIndex)->text();
    }
    return QString();
}

void SearchSuggestionsWidget::selectNext() {
    if (m_listWidget->count() > 0) {
        int newIndex = (m_currentIndex + 1) % m_listWidget->count();
        setCurrentSuggestion(newIndex);
    }
}

void SearchSuggestionsWidget::selectPrevious() {
    if (m_listWidget->count() > 0) {
        int newIndex = (m_currentIndex - 1 + m_listWidget->count()) % m_listWidget->count();
        setCurrentSuggestion(newIndex);
    }
}

void SearchSuggestionsWidget::selectFirst() {
    if (m_listWidget->count() > 0) {
        setCurrentSuggestion(0);
    }
}

void SearchSuggestionsWidget::selectLast() {
    if (m_listWidget->count() > 0) {
        setCurrentSuggestion(m_listWidget->count() - 1);
    }
}

void SearchSuggestionsWidget::handleKeyEvent(QKeyEvent* event) {
    keyPressEvent(event);
}

void SearchSuggestionsWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Up:
            selectPrevious();
            break;
        case Qt::Key_Down:
            selectNext();
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            emit suggestionActivated(selectedSuggestion());
            hide();
            break;
        case Qt::Key_Escape:
            hide();
            break;
        default:
            QFrame::keyPressEvent(event);
    }
}

void SearchSuggestionsWidget::mousePressEvent(QMouseEvent* event) {
    QFrame::mousePressEvent(event);
}

void SearchSuggestionsWidget::onItemClicked(QListWidgetItem* item) {
    if (item) {
        emit suggestionActivated(item->text());
        hide();
    }
}

// IconSearchWidget implementation
IconSearchWidget::IconSearchWidget(QWidget* parent)
    : QWidget(parent), m_iconMetadataManager(nullptr),
      m_suggestionsWidget(new SearchSuggestionsWidget(this)), m_suggestionsVisible(false),
      m_suggestionsEnabled(true), m_searchMode(SimpleSearch), m_searchTimer(new QTimer(this)),
      m_searchDelay(DEFAULT_SEARCH_DELAY), m_maxHistoryItems(MAX_HISTORY_ITEMS),
      m_maxSuggestions(MAX_SUGGESTIONS), m_caseSensitive(false), m_wholeWords(false) {
    GALLERY_LOG_INFO(galleryInit, "IconSearchWidget constructor started");

    setupUI();
    setupShortcuts();

    // Setup search timer
    m_searchTimer->setSingleShot(true);
    connect(m_searchTimer, &QTimer::timeout, this, &IconSearchWidget::onSearchTimer);

    // Setup suggestions
    connect(m_suggestionsWidget, &SearchSuggestionsWidget::suggestionSelected, this,
            &IconSearchWidget::onSuggestionSelected);
    connect(m_suggestionsWidget, &SearchSuggestionsWidget::suggestionActivated, this,
            &IconSearchWidget::onSuggestionActivated);

    GALLERY_LOG_INFO(galleryInit, "IconSearchWidget initialized successfully");
}

IconSearchWidget::~IconSearchWidget() {
    // Cleanup handled by Qt parent-child system
}

void IconSearchWidget::setIconMetadataManager(IconMetadataManager* manager) {
    m_iconMetadataManager = manager;
    updateSuggestions();
}

void IconSearchWidget::setSearchText(const QString& text) {
    m_searchInput->setText(text);
    m_currentSearchText = text;
}

QString IconSearchWidget::searchText() const {
    return m_searchInput->text();
}

void IconSearchWidget::clearSearch() {
    m_searchInput->clear();
    m_currentSearchText.clear();
    hideSuggestions();
    emit searchCleared();
}

void IconSearchWidget::focusSearch() {
    m_searchInput->setFocus();
    m_searchInput->selectAll();
}

void IconSearchWidget::setSearchMode(SearchMode mode) {
    if (m_searchMode != mode) {
        m_searchMode = mode;
        updateSearchMode();
        emit searchModeChanged(mode);
    }
}

void IconSearchWidget::addToHistory(const QString& searchText) {
    if (searchText.isEmpty()) {
        return;
    }

    m_searchHistory.removeOne(searchText);
    m_searchHistory.prepend(searchText);

    while (m_searchHistory.size() > m_maxHistoryItems) {
        m_searchHistory.removeLast();
    }

    // Update history menu
    m_historyMenu->clear();
    for (const QString& item : m_searchHistory) {
        QAction* action = m_historyMenu->addAction(item);
        connect(action, &QAction::triggered, [this, item]() {
            setSearchText(item);
            performSearch();
        });
    }

    if (!m_searchHistory.isEmpty()) {
        m_historyMenu->addSeparator();
        QAction* clearAction = m_historyMenu->addAction("Clear History");
        connect(clearAction, &QAction::triggered, this, &IconSearchWidget::clearHistory);
    }
}

void IconSearchWidget::clearHistory() {
    m_searchHistory.clear();
    m_historyMenu->clear();
}

void IconSearchWidget::updateSuggestions() {
    // This will be called when metadata is loaded
    // For now, just log that it's ready
    if (m_iconMetadataManager) {
        GALLERY_LOG_INFO(galleryMain, "Search suggestions updated with metadata");
    }
}

void IconSearchWidget::setSuggestionsEnabled(bool enabled) {
    m_suggestionsEnabled = enabled;
    if (!enabled) {
        hideSuggestions();
    }
}

void IconSearchWidget::setupUI() {
    // Enhanced main layout with better spacing and alignment
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(12, 8, 12, 8);
    m_mainLayout->setSpacing(8);

    // Add search icon
    QLabel* searchIcon = new QLabel("🔍");
    searchIcon->setFixedSize(20, 20);
    searchIcon->setAlignment(Qt::AlignCenter);
    searchIcon->setStyleSheet("QLabel {"
                              "  color: #7f8c8d;"
                              "  font-size: 14px;"
                              "  padding: 2px;"
                              "}");
    m_mainLayout->addWidget(searchIcon);

    setupSearchInput();
    setupButtons();

    // Add visual separator before buttons
    QFrame* separator = new QFrame;
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("QFrame {"
                             "  color: #bdc3c7;"
                             "  margin: 4px 2px;"
                             "}");
    m_mainLayout->addWidget(separator);

    updatePlaceholderText();

    // Set overall widget styling
    setStyleSheet("IconSearchWidget {"
                  "  background-color: #f8f9fa;"
                  "  border: 2px solid #e9ecef;"
                  "  border-radius: 8px;"
                  "}"
                  "IconSearchWidget:focus-within {"
                  "  border-color: #52a2ff;"
                  "  background-color: #ffffff;"
                  "}");
}

void IconSearchWidget::setupSearchInput() {
    m_searchInput = new QLineEdit;
    m_searchInput->setPlaceholderText("Search icons...");
    m_searchInput->setClearButtonEnabled(true);
    m_searchInput->setMinimumHeight(32);
    m_searchInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Enhanced styling for the search input
    m_searchInput->setStyleSheet("QLineEdit {"
                                 "  border: none;"
                                 "  background-color: transparent;"
                                 "  font-size: 14px;"
                                 "  color: #2c3e50;"
                                 "  padding: 6px 8px;"
                                 "  selection-background-color: #3498db;"
                                 "}"
                                 "QLineEdit:focus {"
                                 "  background-color: rgba(255, 255, 255, 0.8);"
                                 "}"
                                 "QLineEdit::placeholder {"
                                 "  color: #95a5a6;"
                                 "  font-style: italic;"
                                 "}");

    connect(m_searchInput, &QLineEdit::textChanged, this, &IconSearchWidget::onSearchTextChanged);
    connect(m_searchInput, &QLineEdit::textEdited, this, &IconSearchWidget::onSearchTextEdited);

    m_mainLayout->addWidget(m_searchInput);
}

void IconSearchWidget::setupButtons() {
    // Enhanced button styling
    QString buttonStyle = "QToolButton {"
                          "  background-color: #ecf0f1;"
                          "  border: 1px solid #bdc3c7;"
                          "  border-radius: 4px;"
                          "  padding: 6px 12px;"
                          "  font-size: 12px;"
                          "  font-weight: 500;"
                          "  color: #2c3e50;"
                          "  min-width: 60px;"
                          "}"
                          "QToolButton:hover {"
                          "  background-color: #d5dbdb;"
                          "  border-color: #95a5a6;"
                          "}"
                          "QToolButton:pressed {"
                          "  background-color: #bdc3c7;"
                          "}"
                          "QToolButton:checked {"
                          "  background-color: #3498db;"
                          "  color: white;"
                          "  border-color: #2980b9;"
                          "}";

    // Clear button with icon
    m_clearButton = new QToolButton;
    m_clearButton->setText("✕");
    m_clearButton->setToolTip("Clear search (Esc)");
    m_clearButton->setFixedSize(28, 28);
    m_clearButton->setStyleSheet(buttonStyle);
    connect(m_clearButton, &QToolButton::clicked, this, &IconSearchWidget::onClearClicked);
    m_mainLayout->addWidget(m_clearButton);

    // Search mode button
    m_modeButton = new QToolButton;
    m_modeButton->setText("Simple");
    m_modeButton->setToolTip("Search mode");
    m_modeButton->setPopupMode(QToolButton::InstantPopup);
    m_modeButton->setStyleSheet(buttonStyle);

    // Create mode menu
    m_modeMenu = new QMenu(this);
    m_simpleSearchAction = m_modeMenu->addAction("Simple Search");
    m_simpleSearchAction->setCheckable(true);
    m_simpleSearchAction->setChecked(true);

    m_tagSearchAction = m_modeMenu->addAction("Tag Search");
    m_tagSearchAction->setCheckable(true);

    m_regexSearchAction = m_modeMenu->addAction("Regex Search");
    m_regexSearchAction->setCheckable(true);

    m_modeMenu->addSeparator();
    m_advancedSearchAction = m_modeMenu->addAction("Advanced Search...");

    connect(m_simpleSearchAction, &QAction::triggered, [this]() { setSearchMode(SimpleSearch); });
    connect(m_tagSearchAction, &QAction::triggered, [this]() { setSearchMode(TagSearch); });
    connect(m_regexSearchAction, &QAction::triggered, [this]() { setSearchMode(RegexSearch); });
    connect(m_advancedSearchAction, &QAction::triggered, this,
            &IconSearchWidget::onAdvancedSearchClicked);

    m_modeButton->setMenu(m_modeMenu);
    m_mainLayout->addWidget(m_modeButton);

    // History button with icon
    m_historyButton = new QToolButton;
    m_historyButton->setText("📋");
    m_historyButton->setToolTip("Search history");
    m_historyButton->setPopupMode(QToolButton::InstantPopup);
    m_historyButton->setFixedSize(28, 28);
    m_historyButton->setStyleSheet(buttonStyle);

    m_historyMenu = new QMenu(this);
    m_historyButton->setMenu(m_historyMenu);
    m_mainLayout->addWidget(m_historyButton);

    // Advanced search button
    m_advancedButton = new QToolButton;
    m_advancedButton->setText("⚙");
    m_advancedButton->setToolTip("Advanced search options");
    m_advancedButton->setFixedSize(28, 28);
    m_advancedButton->setStyleSheet(buttonStyle);
    connect(m_advancedButton, &QToolButton::clicked, this,
            &IconSearchWidget::onAdvancedSearchClicked);
    m_mainLayout->addWidget(m_advancedButton);
}

void IconSearchWidget::setupShortcuts() {
    m_focusShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(m_focusShortcut, &QShortcut::activated, this, &IconSearchWidget::focusSearch);

    m_clearShortcut = new QShortcut(QKeySequence("Escape"), this);
    connect(m_clearShortcut, &QShortcut::activated, this, &IconSearchWidget::clearSearch);

    m_advancedShortcut = new QShortcut(QKeySequence("Ctrl+Shift+F"), this);
    connect(m_advancedShortcut, &QShortcut::activated, this,
            &IconSearchWidget::onAdvancedSearchClicked);
}

void IconSearchWidget::updateSearchMode() {
    // Update mode button text and menu selection
    switch (m_searchMode) {
        case SimpleSearch:
            m_modeButton->setText("Simple");
            m_simpleSearchAction->setChecked(true);
            m_tagSearchAction->setChecked(false);
            m_regexSearchAction->setChecked(false);
            break;
        case TagSearch:
            m_modeButton->setText("Tags");
            m_simpleSearchAction->setChecked(false);
            m_tagSearchAction->setChecked(true);
            m_regexSearchAction->setChecked(false);
            break;
        case RegexSearch:
            m_modeButton->setText("Regex");
            m_simpleSearchAction->setChecked(false);
            m_tagSearchAction->setChecked(false);
            m_regexSearchAction->setChecked(true);
            break;
        case AdvancedSearch:
            m_modeButton->setText("Advanced");
            break;
    }

    updatePlaceholderText();
}

void IconSearchWidget::updatePlaceholderText() {
    QString placeholder;
    switch (m_searchMode) {
        case SimpleSearch:
            placeholder = "Search icons by name...";
            break;
        case TagSearch:
            placeholder = "Search icons by tags...";
            break;
        case RegexSearch:
            placeholder = "Search icons with regex...";
            break;
        case AdvancedSearch:
            placeholder = "Advanced search active...";
            break;
    }

    m_searchInput->setPlaceholderText(placeholder);
}

void IconSearchWidget::performSearch() {
    QString searchText = m_searchInput->text().trimmed();

    if (searchText != m_currentSearchText) {
        m_currentSearchText = searchText;

        if (!searchText.isEmpty()) {
            addToHistory(searchText);
        }

        emit searchChanged(searchText);
    }
}

void IconSearchWidget::generateSuggestions(const QString& partialText) {
    if (!m_suggestionsEnabled || !m_iconMetadataManager || partialText.length() < 2) {
        hideSuggestions();
        return;
    }

    QStringList suggestions;

    switch (m_searchMode) {
        case SimpleSearch:
            suggestions = getIconSuggestions(partialText);
            break;
        case TagSearch:
            suggestions = getTagSuggestions(partialText);
            break;
        case RegexSearch:
            // No suggestions for regex
            break;
        case AdvancedSearch:
            suggestions = getCategorySuggestions(partialText);
            break;
    }

    if (!suggestions.isEmpty()) {
        m_suggestionsWidget->setSuggestions(suggestions);
        showSuggestions();
    } else {
        hideSuggestions();
    }
}

QStringList IconSearchWidget::getIconSuggestions(const QString& partialText) const {
    if (!m_iconMetadataManager) {
        return QStringList();
    }

    QStringList allIcons = m_iconMetadataManager->getAllIconNames();
    QStringList suggestions;

    for (const QString& iconName : allIcons) {
        if (iconName.contains(partialText, Qt::CaseInsensitive)) {
            suggestions.append(iconName);
            if (suggestions.size() >= m_maxSuggestions) {
                break;
            }
        }
    }

    return suggestions;
}

QStringList IconSearchWidget::getTagSuggestions(const QString& partialText) const {
    if (!m_iconMetadataManager) {
        return QStringList();
    }

    QStringList allTags = m_iconMetadataManager->getAllTags();
    QStringList suggestions;

    for (const QString& tag : allTags) {
        if (tag.contains(partialText, Qt::CaseInsensitive)) {
            suggestions.append(tag);
            if (suggestions.size() >= m_maxSuggestions) {
                break;
            }
        }
    }

    return suggestions;
}

QStringList IconSearchWidget::getCategorySuggestions(const QString& partialText) const {
    if (!m_iconMetadataManager) {
        return QStringList();
    }

    QStringList categories = m_iconMetadataManager->getAllCategories();
    QStringList suggestions;

    for (const QString& category : categories) {
        if (category.contains(partialText, Qt::CaseInsensitive)) {
            suggestions.append(category);
        }
    }

    return suggestions;
}

void IconSearchWidget::showSuggestions() {
    if (!m_suggestionsVisible) {
        positionSuggestions();
        m_suggestionsWidget->show();
        m_suggestionsVisible = true;
    }
}

void IconSearchWidget::hideSuggestions() {
    if (m_suggestionsVisible) {
        m_suggestionsWidget->hide();
        m_suggestionsVisible = false;
    }
}

void IconSearchWidget::positionSuggestions() {
    QPoint pos = m_searchInput->mapToGlobal(QPoint(0, m_searchInput->height()));
    m_suggestionsWidget->move(pos);
    m_suggestionsWidget->resize(m_searchInput->width(), m_suggestionsWidget->sizeHint().height());
}

// Slot implementations
void IconSearchWidget::onSearchTextChanged() {
    m_searchTimer->start(m_searchDelay);
}

void IconSearchWidget::onSearchTextEdited(const QString& text) {
    generateSuggestions(text);
}

void IconSearchWidget::onClearClicked() {
    clearSearch();
}

void IconSearchWidget::onAdvancedSearchClicked() {
    emit advancedSearchRequested();
}

void IconSearchWidget::onSearchModeChanged() {
    // Handle search mode changes from menu
    QAction* action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    if (action == m_simpleSearchAction) {
        setSearchMode(SimpleSearch);
    } else if (action == m_tagSearchAction) {
        setSearchMode(TagSearch);
    } else if (action == m_regexSearchAction) {
        setSearchMode(RegexSearch);
    }
}

void IconSearchWidget::onHistoryClicked() {
    // History menu is handled by the menu itself
    // This slot is for any additional history-related actions
}

void IconSearchWidget::onSuggestionSelected(const QString& suggestion) {
    // Update search input but don't trigger search yet
    m_searchInput->setText(suggestion);
}

void IconSearchWidget::onSuggestionActivated(const QString& suggestion) {
    m_searchInput->setText(suggestion);
    performSearch();
    hideSuggestions();
}

void IconSearchWidget::onSearchTimer() {
    performSearch();
}

void IconSearchWidget::keyPressEvent(QKeyEvent* event) {
    if (m_suggestionsVisible) {
        switch (event->key()) {
            case Qt::Key_Up:
            case Qt::Key_Down:
            case Qt::Key_Return:
            case Qt::Key_Enter:
            case Qt::Key_Escape:
                m_suggestionsWidget->handleKeyEvent(event);
                return;
        }
    }

    QWidget::keyPressEvent(event);
}

void IconSearchWidget::focusInEvent(QFocusEvent* event) {
    QWidget::focusInEvent(event);
    m_searchInput->setFocus();
}

void IconSearchWidget::focusOutEvent(QFocusEvent* event) {
    QWidget::focusOutEvent(event);
    // Hide suggestions when focus is lost
    QTimer::singleShot(100, this, &IconSearchWidget::hideSuggestions);
}
