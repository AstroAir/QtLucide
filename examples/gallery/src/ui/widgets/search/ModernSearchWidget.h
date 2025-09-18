/**
 * QtLucide Gallery Application - Modern Search Widget
 *
 * A beautifully designed search and filter interface with:
 * - Modern search input with animated placeholder and clear button
 * - Real-time search suggestions with fuzzy matching
 * - Advanced filter panel with category, tag, and property filters
 * - Search history with quick access to recent searches
 * - Keyboard shortcuts and accessibility support
 * - Smooth animations and visual feedback
 * - Theme-aware styling with automatic color adaptation
 */

#ifndef MODERNSEARCHWIDGET_H
#define MODERNSEARCHWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QScrollArea>
#include <QListWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QCollapsibleGroupBox>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QCompleter>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QListView>
#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QSplitter>
#include <QTabWidget>
#include <QStackedWidget>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QHash>
#include <QSet>
#include <QStringList>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "../themes/ThemeManager.h"

// Forward declarations
class SearchInputWidget;
class FilterPanelWidget;
class SearchSuggestionsWidget;
class SearchHistoryWidget;
class AdvancedFilterWidget;
class IconMetadataManager;

/**
 * @brief Modern search input with enhanced features
 */
class SearchInputWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SearchInputWidget(QWidget* parent = nullptr);
    ~SearchInputWidget();

    // Search functionality
    void setSearchText(const QString& text);
    QString searchText() const;
    void clearSearch();
    void focusSearch();

    // Placeholder and hints
    void setPlaceholderText(const QString& text);
    QString placeholderText() const;
    void setSearchHint(const QString& hint);
    QString searchHint() const;

    // Visual effects
    void setAnimationsEnabled(bool enabled);
    bool animationsEnabled() const;
    void setShowClearButton(bool show);
    bool showClearButton() const;
    void setShowSearchIcon(bool show);
    bool showSearchIcon() const;

    // Suggestions
    void setSuggestions(const QStringList& suggestions);
    QStringList suggestions() const;
    void setShowSuggestions(bool show);
    bool showSuggestions() const;

    // Theme integration
    void applyTheme();

signals:
    void searchTextChanged(const QString& text);
    void searchRequested(const QString& text);
    void searchCleared();
    void focusReceived();
    void focusLost();
    void suggestionSelected(const QString& suggestion);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onTextChanged();
    void onClearClicked();
    void onSearchRequested();
    void onSuggestionClicked(const QString& suggestion);
    void updateAnimations();

private:
    void setupUI();
    void setupAnimations();
    void updateLayout();
    void updateClearButtonVisibility();
    void showSuggestionsPopup();
    void hideSuggestionsPopup();

    // UI components
    QHBoxLayout* m_layout;
    QLineEdit* m_searchInput;
    QPushButton* m_clearButton;
    QPushButton* m_searchButton;
    QLabel* m_searchIcon;
    SearchSuggestionsWidget* m_suggestionsWidget;

    // Properties
    QString m_placeholderText;
    QString m_searchHint;
    QStringList m_suggestions;
    bool m_animationsEnabled;
    bool m_showClearButton;
    bool m_showSearchIcon;
    bool m_showSuggestions;
    bool m_hasFocus;

    // Animations
    QPropertyAnimation* m_focusAnimation;
    QPropertyAnimation* m_clearButtonAnimation;
    QGraphicsOpacityEffect* m_clearButtonOpacity;

    // Theme
    ThemeAwareWidget* m_themeWidget;

    // Constants
    static constexpr int BORDER_RADIUS = 8;
    static constexpr int PADDING = 12;
    static constexpr int ICON_SIZE = 20;
    static constexpr int CLEAR_BUTTON_SIZE = 24;
    static constexpr int ANIMATION_DURATION = 200;
};

/**
 * @brief Advanced filter panel with multiple filter types
 */
class FilterPanelWidget : public QFrame
{
    Q_OBJECT

public:
    enum FilterType {
        CategoryFilter = 0,
        TagFilter = 1,
        PropertyFilter = 2,
        CustomFilter = 3
    };
    Q_ENUM(FilterType)

    struct FilterCriteria {
        QStringList categories;
        QStringList tags;
        QStringList properties;
        QHash<QString, QVariant> customFilters;
        bool matchAll = false; // true = AND, false = OR
    };

    explicit FilterPanelWidget(QWidget* parent = nullptr);
    ~FilterPanelWidget();

    // Filter management
    void setFilterCriteria(const FilterCriteria& criteria);
    FilterCriteria filterCriteria() const;
    void clearFilters();
    void resetToDefaults();

    // Available options
    void setAvailableCategories(const QStringList& categories);
    QStringList availableCategories() const;
    void setAvailableTags(const QStringList& tags);
    QStringList availableTags() const;
    void setAvailableProperties(const QStringList& properties);
    QStringList availableProperties() const;

    // UI state
    void setExpanded(bool expanded);
    bool isExpanded() const;
    void setAnimationsEnabled(bool enabled);
    bool animationsEnabled() const;

    // Integration
    void setMetadataManager(IconMetadataManager* manager);

signals:
    void filterChanged(const FilterCriteria& criteria);
    void filtersCleared();
    void expandedChanged(bool expanded);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onCategoryFilterChanged();
    void onTagFilterChanged();
    void onPropertyFilterChanged();
    void onCustomFilterChanged();
    void onClearFiltersClicked();
    void onExpandToggled();

private:
    void setupUI();
    void setupCategoryFilter();
    void setupTagFilter();
    void setupPropertyFilter();
    void setupCustomFilter();
    void updateFilterCriteria();
    void applyFilterCriteria();
    void animateExpansion();
    void applyTheme();

    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_headerLayout;
    QLabel* m_titleLabel;
    QPushButton* m_expandButton;
    QPushButton* m_clearButton;
    QFrame* m_contentFrame;
    QVBoxLayout* m_contentLayout;
    QScrollArea* m_scrollArea;

    // Filter sections
    QGroupBox* m_categoryGroup;
    QVBoxLayout* m_categoryLayout;
    QHash<QString, QCheckBox*> m_categoryCheckboxes;

    QGroupBox* m_tagGroup;
    QVBoxLayout* m_tagLayout;
    QHash<QString, QCheckBox*> m_tagCheckboxes;

    QGroupBox* m_propertyGroup;
    QVBoxLayout* m_propertyLayout;
    QHash<QString, QCheckBox*> m_propertyCheckboxes;

    QGroupBox* m_customGroup;
    QVBoxLayout* m_customLayout;

    // Logic controls
    QFrame* m_logicFrame;
    QHBoxLayout* m_logicLayout;
    QLabel* m_logicLabel;
    QButtonGroup* m_logicGroup;
    QCheckBox* m_matchAllCheckbox;

    // Data
    FilterCriteria m_currentCriteria;
    QStringList m_availableCategories;
    QStringList m_availableTags;
    QStringList m_availableProperties;
    IconMetadataManager* m_metadataManager;

    // State
    bool m_expanded;
    bool m_animationsEnabled;

    // Animations
    QPropertyAnimation* m_expansionAnimation;
    int m_collapsedHeight;
    int m_expandedHeight;

    // Theme
    ThemeAwareWidget* m_themeWidget;
};

/**
 * @brief Main modern search widget combining all search features
 */
class ModernSearchWidget : public QWidget
{
    Q_OBJECT

public:
    struct SearchResult {
        QString iconName;
        QString displayName;
        QStringList categories;
        QStringList tags;
        double relevanceScore;
        QString matchReason;
    };

    explicit ModernSearchWidget(QWidget* parent = nullptr);
    ~ModernSearchWidget();

    // Search functionality
    void setSearchText(const QString& text);
    QString searchText() const;
    void performSearch();
    void clearSearch();
    QList<SearchResult> searchResults() const;

    // Filter functionality
    void setFilterCriteria(const FilterPanelWidget::FilterCriteria& criteria);
    FilterPanelWidget::FilterCriteria filterCriteria() const;
    void clearFilters();

    // UI state
    void setShowFilterPanel(bool show);
    bool showFilterPanel() const;
    void setShowSearchHistory(bool show);
    bool showSearchHistory() const;
    void setAnimationsEnabled(bool enabled);
    bool animationsEnabled() const;

    // Integration
    void setMetadataManager(IconMetadataManager* manager);
    void setThemeManager(ThemeManager* themeManager);

    // Search history
    void addToSearchHistory(const QString& searchText);
    QStringList searchHistory() const;
    void clearSearchHistory();

public slots:
    void focusSearch();
    void toggleFilterPanel();
    void refreshSuggestions();

signals:
    void searchTextChanged(const QString& text);
    void searchResultsChanged(const QList<SearchResult>& results);
    void filterChanged(const FilterPanelWidget::FilterCriteria& criteria);
    void searchHistoryChanged(const QStringList& history);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchRequested(const QString& text);
    void onFilterChanged(const FilterPanelWidget::FilterCriteria& criteria);
    void onSearchHistoryItemSelected(const QString& text);
    void performDelayedSearch();

private:
    void setupUI();
    void setupLayout();
    void setupConnections();
    void updateLayout();
    void updateSearchResults();
    void calculateRelevanceScores();
    void applyFilters();
    void updateSuggestions();
    void saveSearchHistory();
    void loadSearchHistory();

    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_topLayout;
    SearchInputWidget* m_searchInput;
    QPushButton* m_filterToggleButton;
    QSplitter* m_splitter;
    FilterPanelWidget* m_filterPanel;
    SearchHistoryWidget* m_historyWidget;

    // Data
    IconMetadataManager* m_metadataManager;
    ThemeManager* m_themeManager;
    QList<SearchResult> m_searchResults;
    QStringList m_searchHistory;
    QString m_currentSearchText;
    FilterPanelWidget::FilterCriteria m_currentCriteria;

    // State
    bool m_showFilterPanel;
    bool m_showSearchHistory;
    bool m_animationsEnabled;

    // Search optimization
    QTimer* m_searchDelayTimer;
    QElapsedTimer* m_searchTimer;
    static constexpr int SEARCH_DELAY_MS = 300;
    static constexpr int MAX_SEARCH_HISTORY = 50;
    static constexpr int MAX_SUGGESTIONS = 10;
};

#endif // MODERNSEARCHWIDGET_H
