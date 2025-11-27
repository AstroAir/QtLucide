/**
 * QtLucide Gallery Application - Enhanced Search and Filtering System
 *
 * A comprehensive, high-performance search and filtering system with:
 * - Real-time search with fuzzy matching and relevance scoring
 * - Advanced filtering with multiple criteria (categories, tags, favorites, etc.)
 * - Intelligent autocomplete with context-aware suggestions
 * - Search history and saved searches
 * - Performance-optimized with debouncing and caching
 * - Modern UI with smooth animations and visual feedback
 * - Accessibility support with keyboard navigation
 * - Export/import of search configurations
 */

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QFocusEvent>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QScrollArea>
#include <QSettings>
#include <QShowEvent>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStringListModel>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "IconMetadataManager.h"
#include "core/GalleryTypes.h"

// Forward declarations
class SearchSuggestionPopup;
class AdvancedFilterWidget;
class SearchHistoryWidget;

/**
 * @brief Enhanced search line edit with intelligent autocomplete and suggestions
 */
class SearchLineEdit : public QLineEdit {
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget* parent = nullptr);
    ~SearchLineEdit();

    void setMetadataManager(IconMetadataManager* manager);
    void setSuggestionLimit(int limit);
    void setFuzzySearchEnabled(bool enabled);
    void setSearchHistoryEnabled(bool enabled);
    void setHighlightMatches(bool enabled);

    // Search configuration
    void setMinSearchLength(int length);
    void setSearchDelay(int milliseconds);
    void setCaseSensitive(bool sensitive);
    void setRegexEnabled(bool enabled);

    // History management
    void addToHistory(const QString& searchTerm);
    void clearHistory();
    QStringList getHistory() const;

    // Suggestions
    void updateSuggestions();
    void showSuggestions();
    void hideSuggestions();
    void updateCompleter();

public slots:
    void clearSearch();
    void focusSearch();
    void selectAll();

signals:
    void searchTextChanged(const QString& text);
    void searchCleared();
    void suggestionSelected(const QString& suggestion);
    void searchSubmitted(const QString& text);
    void historyRequested();
    void searchRequested(const QString& text);
    void historyCleared();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onTextChanged(const QString& text);
    void onSuggestionActivated(const QString& text);
    void onSearchTimer();
    void onHistoryAction();
    void onClearHistoryAction();

private:
    void setupCompleter();
    void setupSuggestionPopup();
    void updateCompleterModel();
    void updateSearchIcon();
    void performSearch();

    IconMetadataManager* m_metadataManager;
    // std::unique_ptr<SearchSuggestionPopup> m_suggestionPopup; // Commented out - class not
    // defined
    QCompleter* m_completer;
    QStringListModel* m_completerModel;
    QTimer* m_searchTimer;
    QAction* m_clearAction;
    QAction* m_historyAction;

    // Configuration
    int m_suggestionLimit;
    int m_minSearchLength;
    int m_searchDelay;
    bool m_fuzzySearchEnabled;
    bool m_searchHistoryEnabled;
    bool m_highlightMatches;
    bool m_caseSensitive;
    bool m_regexEnabled;

    // History
    QStringList m_searchHistory;
    QSettings* m_settings;

    // Constants
    static constexpr int DEFAULT_SUGGESTION_LIMIT = 20;
    static constexpr int DEFAULT_MIN_SEARCH_LENGTH = 1;
    static constexpr int DEFAULT_SEARCH_DELAY = 300;
    static constexpr int MAX_HISTORY_SIZE = 50;
};

/**
 * @brief Enhanced widget for advanced search options and comprehensive filtering
 */
class AdvancedSearchPanel : public QFrame {
    Q_OBJECT

public:
    explicit AdvancedSearchPanel(QWidget* parent = nullptr);
    ~AdvancedSearchPanel();

    void setMetadataManager(IconMetadataManager* manager);
    IconFilterCriteria getFilterCriteria() const;
    void setFilterCriteria(const IconFilterCriteria& criteria);
    void clearFilters();
    void resetToDefaults();

    // Criteria management
    SearchCriteria getCurrentCriteria() const;
    void applyCriteria(const SearchCriteria& criteria);

    // Panel management
    bool isExpanded() const;
    void setAnimationsEnabled(bool enabled);

    // Filter presets
    void saveFilterPreset(const QString& name);
    void loadFilterPreset(const QString& name);
    void deleteFilterPreset(const QString& name);
    QStringList getFilterPresets() const;

public slots:
    void showPanel();
    void hidePanel();
    void togglePanel();
    void expandPanel();
    void collapsePanel();

signals:
    void filtersChanged(const IconFilterCriteria& criteria);
    void filtersCleared();
    void panelToggled(bool visible);
    void filterPresetSaved(const QString& name);
    void filterPresetLoaded(const QString& name);
    void panelExpanded();
    void panelCollapsed();
    void contributorFilterChanged(const QStringList& contributors);
    void dateRangeChanged(const QDateTime& startDate, const QDateTime& endDate);
    void usageRangeChanged(int minUsage, int maxUsage);
    void presetSaved(const QString& name);
    void presetLoaded(const QString& name);
    void presetDeleted(const QString& name);

private slots:
    void onCategorySelectionChanged();
    void onTagSelectionChanged();
    void onContributorSelectionChanged();
    void onSortOrderChanged();
    void onFilterOptionsChanged();
    void onDateRangeChanged();
    void onUsageRangeChanged();
    void onClearFilters();
    void onSavePreset();
    void onLoadPreset();
    void onDeletePreset();
    void onAnimationFinished();

private:
    void setupUI();
    void setupBasicFilters();
    void setupCategoryFilter();
    void setupTagFilter();
    void setupContributorFilter();
    void setupSortOptions();
    void setupAdvancedOptions();
    void setupDateRangeFilter();
    void setupUsageFilter();
    void setupPresetManager();
    void setupAnimations();

    void updateCategoryList();
    void updateTagList();
    void updateContributorList();
    void updateFilterCounts();
    void emitFiltersChanged();
    void validateFilters();

    // Animation helpers
    void animateExpansion(bool expand);
    void updatePanelHeight();

    IconMetadataManager* m_metadataManager;

    // Enhanced UI components
    QVBoxLayout* m_mainLayout;
    QTabWidget* m_tabWidget;
    QScrollArea* m_scrollArea;

    // Basic filters tab
    QWidget* m_basicTab;
    QGroupBox* m_categoryGroup;
    QGroupBox* m_tagGroup;
    QGroupBox* m_contributorGroup;
    QListWidget* m_categoryList;
    QListWidget* m_tagList;
    QListWidget* m_contributorList;

    // Advanced filters tab
    QWidget* m_advancedTab;
    QGroupBox* m_sortGroup;
    QGroupBox* m_optionsGroup;
    QGroupBox* m_dateRangeGroup;
    QGroupBox* m_usageGroup;

    QComboBox* m_sortCombo;
    QCheckBox* m_sortAscending;
    QCheckBox* m_favoritesOnly;
    QCheckBox* m_recentOnly;
    QCheckBox* m_exactMatch;
    QCheckBox* m_useRegex;
    QCheckBox* m_fuzzySearch;

    // Date range controls
    QComboBox* m_dateRangeCombo;
    QSpinBox* m_minUsageSpinBox;
    QSpinBox* m_maxUsageSpinBox;

    // Preset management
    QWidget* m_presetTab;
    QComboBox* m_presetCombo;
    QPushButton* m_savePresetButton;
    QPushButton* m_deletePresetButton;

    // Control buttons
    QHBoxLayout* m_buttonLayout;
    QPushButton* m_clearButton;
    QPushButton* m_resetButton;
    QPushButton* m_applyButton;

    // Animation and state
    QPropertyAnimation* m_slideAnimation;
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
    bool m_isExpanded;
    bool m_animationsEnabled;
    int m_collapsedHeight;
    int m_expandedHeight;

    // Settings
    QSettings* m_settings;
    QHash<QString, IconFilterCriteria> m_filterPresets;
    QHash<QString, SearchCriteria> m_savedPresets;
    bool m_expanded;
};

/**
 * @brief Enhanced main search widget with comprehensive search and filtering capabilities
 */
class SearchWidget : public QWidget {
    Q_OBJECT

public:
    explicit SearchWidget(IconMetadataManager* metadataManager, QWidget* parent = nullptr);
    ~SearchWidget();

    // Core search functionality
    QString searchText() const;
    IconFilterCriteria getFilterCriteria() const;
    void setFilterCriteria(const IconFilterCriteria& criteria);
    void clearSearch();
    void clearFilters();
    void resetToDefaults();

    // UI state management
    bool isAdvancedPanelVisible() const;
    void setAdvancedPanelVisible(bool visible);
    bool isCompactMode() const;
    void setCompactMode(bool compact);

    // Search configuration
    void setSearchDelay(int milliseconds);
    void setMinSearchLength(int length);
    void setMaxResults(int maxResults);
    void setFuzzySearchEnabled(bool enabled);
    void setSearchHistoryEnabled(bool enabled);

    // Performance and optimization
    void setPerformanceMode(bool enabled);
    void preloadSuggestions();
    void clearCache();

    // Import/Export
    bool exportSearchConfig(const QString& filePath) const;
    bool importSearchConfig(const QString& filePath);

    // Statistics
    int getSearchCount() const;
    QStringList getMostUsedSearchTerms(int limit = 10) const;
    QVariantMap getSearchStatistics() const;
    void setPlaceholderText(const QString& text);
    void setAnimationsEnabled(bool enabled) { m_animationsEnabled = enabled; }

public slots:
    void clearAllFilters();
    void focusSearchInput();
    void showAdvancedOptions();
    void hideAdvancedOptions();
    void toggleAdvancedOptions();
    void performSearch();
    void saveCurrentSearch();
    void loadSavedSearch(const QString& name);

signals:
    void searchChanged(const QString& searchText);
    void filtersChanged(const IconFilterCriteria& criteria);
    void searchCleared();
    void filtersCleared();
    void advancedPanelToggled(bool visible);
    void searchPerformed(const IconFilterCriteria& criteria);
    void searchStatisticsUpdated(const QVariantMap& stats);
    void searchRequested(const QString& text);
    void searchSaved(const QString& name);
    void searchLoaded(const QString& name);
    void statisticsUpdated(int totalSearches, int currentSearch);

protected:
    void showEvent(QShowEvent* event) override;
    void hideEvent(QHideEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onAdvancedFiltersChanged(const IconFilterCriteria& criteria);
    void onSearchCleared();
    void onFiltersCleared();
    void onToggleAdvanced();
    void onSearchTimer();
    void onUpdateStatistics();

private:
    void setupUI();
    void setupSearchBar();
    void setupAdvancedPanel();
    void setupToolButtons();
    void setupConnections();
    void setupAnimations();
    void setupPerformanceMonitoring();

    void updateFilterIndicator();
    void updateSearchStatistics();
    void saveSearchHistory();
    void loadSearchHistory();
    void updateSearchHistory();
    void saveSettings();
    void loadSettings();

    // Performance optimization
    void optimizeSearch();
    void debounceSearch();
    void cacheResults();

    IconMetadataManager* m_metadataManager;

    // Enhanced UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_searchBarLayout;
    QFrame* m_searchBar;
    QFrame* m_toolBar;

    SearchLineEdit* m_searchEdit;
    AdvancedSearchPanel* m_advancedPanel;

    // Tool buttons
    QToolButton* m_clearButton;
    QToolButton* m_advancedButton;
    QToolButton* m_filterIndicator;
    QToolButton* m_historyButton;
    QToolButton* m_settingsButton;

    // Status and progress
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;

    // Search state and performance
    QTimer* m_searchTimer;
    QTimer* m_statisticsTimer;
    QElapsedTimer m_performanceTimer;
    IconFilterCriteria m_currentCriteria;
    QString m_lastSearchText;

    // Configuration and state
    bool m_compactMode;
    bool m_performanceMode;
    bool m_animationsEnabled;
    int m_searchDelay;
    int m_minSearchLength;
    int m_maxResults;

    // Statistics and monitoring
    int m_searchCount;
    QStringList m_searchHistory;
    QHash<QString, int> m_searchTermFrequency;
    QVariantMap m_searchStatistics;

    // Settings
    QSettings* m_settings;

    // Constants
    static constexpr int DEFAULT_SEARCH_DELAY = 300;
    static constexpr int DEFAULT_MIN_SEARCH_LENGTH = 1;
    static constexpr int DEFAULT_MAX_RESULTS = 1000;
    static constexpr int STATISTICS_UPDATE_INTERVAL = 5000; // 5 seconds
    static constexpr int MAX_SEARCH_HISTORY = 100;
    static constexpr const char* SETTINGS_GROUP = "SearchWidget";
};

#endif // SEARCHWIDGET_H
