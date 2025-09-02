/**
 * QtLucide Gallery Application - Search Widget Header
 * 
 * Provides comprehensive search functionality with real-time filtering,
 * autocomplete suggestions, and advanced search options.
 */

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QCompleter>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QShowEvent>
#include <QHideEvent>

#include "IconMetadataManager.h"

/**
 * @brief Advanced search line edit with autocomplete and suggestions
 */
class SearchLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit SearchLineEdit(QWidget *parent = nullptr);
    
    void setMetadataManager(IconMetadataManager* manager);
    void setSuggestionLimit(int limit);

public slots:
    void clearSearch();
    void focusSearch();

signals:
    void searchTextChanged(const QString& text);
    void searchCleared();
    void suggestionSelected(const QString& suggestion);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void onTextChanged(const QString& text);
    void onSuggestionActivated(const QString& text);
    void updateSuggestions();

private:
    void setupCompleter();
    void updateCompleterModel();
    
    IconMetadataManager* m_metadataManager;
    QCompleter* m_completer;
    QStringListModel* m_completerModel;
    QTimer* m_suggestionTimer;
    int m_suggestionLimit;
    
    static constexpr int SUGGESTION_DELAY = 300;
    static constexpr int DEFAULT_SUGGESTION_LIMIT = 20;
};

/**
 * @brief Widget for advanced search options and filters
 */
class AdvancedSearchPanel : public QFrame
{
    Q_OBJECT

public:
    explicit AdvancedSearchPanel(QWidget *parent = nullptr);
    
    void setMetadataManager(IconMetadataManager* manager);
    IconFilterCriteria getFilterCriteria() const;
    void setFilterCriteria(const IconFilterCriteria& criteria);
    void clearFilters();

public slots:
    void showPanel();
    void hidePanel();
    void togglePanel();

signals:
    void filtersChanged(const IconFilterCriteria& criteria);
    void filtersCleared();

private slots:
    void onCategorySelectionChanged();
    void onTagSelectionChanged();
    void onSortOrderChanged();
    void onFilterOptionsChanged();
    void onClearFilters();

private:
    void setupUI();
    void setupCategoryFilter();
    void setupTagFilter();
    void setupSortOptions();
    void setupFilterOptions();
    void updateCategoryList();
    void updateTagList();
    void emitFiltersChanged();
    
    IconMetadataManager* m_metadataManager;
    
    // UI components
    QVBoxLayout* m_layout;
    QGroupBox* m_categoryGroup;
    QGroupBox* m_tagGroup;
    QGroupBox* m_sortGroup;
    QGroupBox* m_optionsGroup;
    
    QListWidget* m_categoryList;
    QListWidget* m_tagList;
    QComboBox* m_sortCombo;
    QCheckBox* m_sortAscending;
    QCheckBox* m_favoritesOnly;
    QCheckBox* m_recentOnly;
    
    QToolButton* m_clearButton;
    
    // Animation
    QPropertyAnimation* m_slideAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
    bool m_isVisible;
};

/**
 * @brief Main search widget combining search input and advanced options
 */
class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(IconMetadataManager* metadataManager, QWidget *parent = nullptr);
    ~SearchWidget();
    
    // Search functionality
    QString searchText() const;
    IconFilterCriteria getFilterCriteria() const;
    void setFilterCriteria(const IconFilterCriteria& criteria);
    
    // UI state
    bool isAdvancedPanelVisible() const;
    void setPlaceholderText(const QString& text);

public slots:
    void clearSearch();
    void clearAllFilters();
    void focusSearchInput();
    void showAdvancedOptions();
    void hideAdvancedOptions();
    void toggleAdvancedOptions();

signals:
    void searchChanged(const QString& searchText);
    void filtersChanged(const IconFilterCriteria& criteria);
    void searchCleared();
    void filtersCleared();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onAdvancedFiltersChanged(const IconFilterCriteria& criteria);
    void onSearchCleared();
    void onFiltersCleared();
    void onToggleAdvanced();

private:
    void setupUI();
    void setupSearchBar();
    void setupToolButtons();
    void updateFilterIndicator();
    void saveSearchHistory();
    void loadSearchHistory();
    
    IconMetadataManager* m_metadataManager;
    
    // UI components
    QVBoxLayout* m_layout;
    QHBoxLayout* m_searchLayout;
    
    SearchLineEdit* m_searchEdit;
    QToolButton* m_clearButton;
    QToolButton* m_advancedButton;
    QToolButton* m_filterIndicator;
    
    AdvancedSearchPanel* m_advancedPanel;
    
    // State
    IconFilterCriteria m_currentCriteria;
    QStringList m_searchHistory;
    
    // Configuration
    static constexpr int MAX_SEARCH_HISTORY = 50;
};

/**
 * @brief Quick filter buttons for common search categories
 */
class QuickFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QuickFilterWidget(IconMetadataManager* metadataManager, QWidget *parent = nullptr);
    
    void updateFilters();

signals:
    void quickFilterSelected(const QString& category);
    void showAllIcons();

private slots:
    void onFilterButtonClicked();
    void onShowAllClicked();

private:
    void setupUI();
    void createFilterButtons();
    
    IconMetadataManager* m_metadataManager;
    QHBoxLayout* m_layout;
    QButtonGroup* m_buttonGroup;
    QToolButton* m_showAllButton;
    QList<QToolButton*> m_filterButtons;
};

#endif // SEARCHWIDGET_H
