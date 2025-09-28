/**
 * QtLucide Gallery Application - Enhanced Icon Search Widget
 *
 * Advanced search functionality with:
 * - Real-time filtering using 916 available tags
 * - Auto-completion and suggestions
 * - Search history
 * - Advanced search dialog with filters
 * - Tag-based filtering
 * - Regular expression support
 */

#ifndef ICONSEARCHWIDGET_H
#define ICONSEARCHWIDGET_H

#include <QAction>
#include <QCompleter>
#include <QFocusEvent>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QShortcut>
#include <QShowEvent>
#include <QStringListModel>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

// Forward declarations
class IconMetadataManager;

/**
 * @brief Search suggestions popup widget
 */
class SearchSuggestionsWidget : public QFrame {
    Q_OBJECT

public:
    explicit SearchSuggestionsWidget(QWidget* parent = nullptr);

    void setSuggestions(const QStringList& suggestions);
    void setCurrentSuggestion(int index);
    int currentSuggestion() const { return m_currentIndex; }

    QString selectedSuggestion() const;
    void selectNext();
    void selectPrevious();
    void selectFirst();
    void selectLast();

    // Public method for external key event handling
    void handleKeyEvent(QKeyEvent* event);

signals:
    void suggestionSelected(const QString& suggestion);
    void suggestionActivated(const QString& suggestion);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private slots:
    void onItemClicked(QListWidgetItem* item);

private:
    void setupUI();
    void updateSelection();

    QVBoxLayout* m_layout;
    QListWidget* m_listWidget;
    int m_currentIndex;

    static const int MAX_SUGGESTIONS = 10;
    static const int ITEM_HEIGHT = 24;
};

/**
 * @brief Enhanced search widget with real-time filtering
 */
class IconSearchWidget : public QWidget {
    Q_OBJECT

public:
    explicit IconSearchWidget(QWidget* parent = nullptr);
    ~IconSearchWidget();

    // Manager setup
    void setIconMetadataManager(IconMetadataManager* manager);

    // Search functionality
    void setSearchText(const QString& text);
    QString searchText() const;
    void clearSearch();
    void focusSearch();

    // Search modes
    enum SearchMode {
        SimpleSearch,  // Basic text matching
        TagSearch,     // Tag-based filtering
        RegexSearch,   // Regular expression
        AdvancedSearch // Multiple criteria
    };

    void setSearchMode(SearchMode mode);
    SearchMode searchMode() const { return m_searchMode; }

    // Search history
    void addToHistory(const QString& searchText);
    QStringList searchHistory() const { return m_searchHistory; }
    void clearHistory();

    // Auto-completion
    void updateSuggestions();
    void setSuggestionsEnabled(bool enabled);
    bool suggestionsEnabled() const { return m_suggestionsEnabled; }

signals:
    void searchChanged(const QString& searchText);
    void searchCleared();
    void advancedSearchRequested();
    void searchModeChanged(SearchMode mode);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private slots:
    void onSearchTextChanged();
    void onSearchTextEdited(const QString& text);
    void onClearClicked();
    void onAdvancedSearchClicked();
    void onSearchModeChanged();
    void onHistoryClicked();
    void onSuggestionSelected(const QString& suggestion);
    void onSuggestionActivated(const QString& suggestion);
    void onSearchTimer();

private:
    void setupUI();
    void setupSearchInput();
    void setupButtons();
    void setupSuggestions();
    void setupShortcuts();

    void updateSearchMode();
    void updatePlaceholderText();
    void updateSuggestionsPopup();
    void showSuggestions();
    void hideSuggestions();
    void positionSuggestions();

    void performSearch();
    void generateSuggestions(const QString& partialText);

    // Search helpers
    QStringList getIconSuggestions(const QString& partialText) const;
    QStringList getTagSuggestions(const QString& partialText) const;
    QStringList getCategorySuggestions(const QString& partialText) const;

    // UI Components
    QHBoxLayout* m_mainLayout;
    QLineEdit* m_searchInput;
    QToolButton* m_clearButton;
    QToolButton* m_advancedButton;
    QToolButton* m_modeButton;
    QToolButton* m_historyButton;

    // Managers (moved to match constructor order)
    IconMetadataManager* m_iconMetadataManager;

    // Search suggestions
    SearchSuggestionsWidget* m_suggestionsWidget;
    bool m_suggestionsVisible;
    bool m_suggestionsEnabled;

    // Search mode menu
    QMenu* m_modeMenu;
    QAction* m_simpleSearchAction;
    QAction* m_tagSearchAction;
    QAction* m_regexSearchAction;
    QAction* m_advancedSearchAction;

    // History menu
    QMenu* m_historyMenu;
    QStringList m_searchHistory;

    // Advanced search dialog (to be implemented later)
    // AdvancedSearchDialog* m_advancedDialog;

    // State
    SearchMode m_searchMode;
    QString m_currentSearchText;
    QTimer* m_searchTimer;

    // Shortcuts
    QShortcut* m_focusShortcut;
    QShortcut* m_clearShortcut;
    QShortcut* m_advancedShortcut;

    // Settings
    int m_searchDelay;
    int m_maxHistoryItems;
    int m_maxSuggestions;
    bool m_caseSensitive;
    bool m_wholeWords;

    static const int DEFAULT_SEARCH_DELAY = 300; // ms
    static const int MAX_HISTORY_ITEMS = 20;
    static const int MAX_SUGGESTIONS = 10;
};

// Advanced search dialog will be implemented later

#endif // ICONSEARCHWIDGET_H
