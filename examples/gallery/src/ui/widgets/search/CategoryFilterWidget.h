/**
 * QtLucide Gallery Application - Enhanced Category Filter Widget
 *
 * A comprehensive, high-performance category and tag filtering system with:
 * - Hierarchical category tree with smart grouping
 * - Advanced multi-selection with visual feedback
 * - Real-time search and filtering within categories
 * - Tag cloud visualization with frequency-based sizing
 * - Drag and drop support for custom organization
 * - Export/import of filter configurations
 * - Performance-optimized for large category sets
 * - Accessibility support with keyboard navigation
 * - Customizable appearance and layout options
 */

#ifndef CATEGORYFILTERWIDGET_H
#define CATEGORYFILTERWIDGET_H

#include <QAction>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QContextMenuEvent>
#include <QDrag>
#include <QElapsedTimer>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMimeData>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSettings>
#include <QSlider>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTimer>
#include <QToolButton>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include "IconMetadataManager.h"

// Forward declarations
class TagCloudWidget;
class CategoryStatisticsWidget;

/**
 * @brief Enhanced tree widget item for category display with advanced features
 */
class CategoryTreeItem : public QTreeWidgetItem {
public:
    enum ItemType {
        CategoryItem = QTreeWidgetItem::UserType + 1,
        TagItem = QTreeWidgetItem::UserType + 2,
        ContributorItem = QTreeWidgetItem::UserType + 3,
        IconCountItem = QTreeWidgetItem::UserType + 4,
        StatisticsItem = QTreeWidgetItem::UserType + 5
    };

    CategoryTreeItem(QTreeWidget* parent, const QString& category, int iconCount,
                     ItemType type = CategoryItem);
    CategoryTreeItem(CategoryTreeItem* parent, const QString& name, int iconCount,
                     ItemType type = TagItem);

    // Enhanced functionality
    QString getName() const { return m_name; }
    int getIconCount() const { return m_iconCount; }
    ItemType getItemType() const { return m_itemType; }
    double getFrequency() const { return m_frequency; }
    bool isFiltered() const { return m_isFiltered; }

    void setIconCount(int count);
    void setFrequency(double frequency);
    void setFiltered(bool filtered);
    void updateDisplay();

    // Visual enhancements
    void setHighlighted(bool highlighted);
    void setCustomIcon(const QIcon& icon);
    void setToolTipInfo(const QString& info);

private:
    void updateText();
    void updateIcon();
    void updateColors();

    QString m_name;
    int m_iconCount;
    ItemType m_itemType;
    double m_frequency;
    bool m_isFiltered;
    bool m_isHighlighted;
    QIcon m_customIcon;
};

/**
 * @brief Tree widget for displaying categories and tags hierarchically
 */
class CategoryTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    explicit CategoryTreeWidget(QWidget* parent = nullptr);

    void setMetadataManager(IconMetadataManager* manager);
    void updateCategories();
    void updateIconCounts();

    QStringList selectedCategories() const;
    QStringList selectedTags() const;
    void setSelectedCategories(const QStringList& categories);
    void setSelectedTags(const QStringList& tags);
    void clearSelection();

signals:
    void categorySelectionChanged(const QStringList& categories);
    void tagSelectionChanged(const QStringList& tags);
    void selectionChanged();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void onItemChanged(QTreeWidgetItem* item, int column);
    void onItemClicked(QTreeWidgetItem* item, int column);
    void onExpandAll();
    void onCollapseAll();
    void onSelectAll();
    void onDeselectAll();

private:
    void setupContextMenu();
    void populateTree();
    void addCategoryItem(const QString& category, int iconCount);
    void addTagItems(CategoryTreeItem* categoryItem, const QString& category);
    void updateItemCheckState(QTreeWidgetItem* item);
    void emitSelectionChanged();

    IconMetadataManager* m_metadataManager;
    QMenu* m_contextMenu;
    QAction* m_expandAllAction;
    QAction* m_collapseAllAction;
    QAction* m_selectAllAction;
    QAction* m_deselectAllAction;

    bool m_updatingSelection;
};

/**
 * @brief Compact list widget for quick category selection
 */
class CategoryListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit CategoryListWidget(QWidget* parent = nullptr);

    void setMetadataManager(IconMetadataManager* manager);
    void updateCategories();

    QStringList selectedCategories() const;
    void setSelectedCategories(const QStringList& categories);

signals:
    void categorySelectionChanged(const QStringList& categories);

private slots:
    void onItemChanged(QListWidgetItem* item);

private:
    void populateList();
    void emitSelectionChanged();

    IconMetadataManager* m_metadataManager;
    bool m_updatingSelection;
};

/**
 * @brief Enhanced main category filter widget with comprehensive filtering capabilities
 */
class CategoryFilterWidget : public QWidget {
    Q_OBJECT

public:
    enum ViewMode {
        TreeView = 0,      // Hierarchical tree with categories and tags
        ListView = 1,      // Simple list of categories
        CompactView = 2,   // Horizontal button bar
        TagCloudView = 3,  // Tag cloud visualization
        StatisticsView = 4 // Statistics and analytics view
    };
    Q_ENUM(ViewMode)

    enum FilterMode {
        InclusiveFilter = 0, // Show icons that match ANY selected category/tag
        ExclusiveFilter = 1, // Show icons that match ALL selected categories/tags
        ExcludeFilter = 2    // Hide icons that match selected categories/tags
    };
    Q_ENUM(FilterMode)

    explicit CategoryFilterWidget(IconMetadataManager* metadataManager, QWidget* parent = nullptr);
    ~CategoryFilterWidget();

    // Enhanced selection management
    QStringList selectedCategories() const;
    QStringList selectedTags() const;
    QStringList selectedContributors() const;
    void setSelectedCategories(const QStringList& categories);
    void setSelectedTags(const QStringList& tags);
    void setSelectedContributors(const QStringList& contributors);
    void clearSelection();
    void clearCategorySelection();
    void clearTagSelection();
    void clearContributorSelection();

    // View and filter modes
    ViewMode viewMode() const { return m_viewMode; }
    void setViewMode(ViewMode mode);
    FilterMode filterMode() const { return m_filterMode; }
    void setFilterMode(FilterMode mode);

    // Enhanced filter state
    bool hasActiveFilters() const;
    int selectedCategoryCount() const;
    int selectedTagCount() const;
    int selectedContributorCount() const;
    int totalFilterCount() const;

    // Search and filtering
    void setSearchFilter(const QString& filter);
    void clearSearchFilter();
    void setMinimumIconCount(int count);
    void setShowEmptyCategories(bool show);

    // Display options
    void setShowIconCounts(bool show);
    void setShowStatistics(bool show);
    void setAnimationsEnabled(bool enabled);
    void setCompactMode(bool compact);

    // Import/Export
    bool exportFilterConfig(const QString& filePath) const;
    bool importFilterConfig(const QString& filePath);

    // Statistics
    QVariantMap getFilterStatistics() const;

public slots:
    void refreshCategories();
    void refreshTags();
    void refreshContributors();
    void refreshAll();
    void expandAll();
    void collapseAll();
    void selectAll();
    void deselectAll();
    void toggleViewMode();
    void resetFilters();
    void saveFilterPreset(const QString& name);
    void loadFilterPreset(const QString& name);

signals:
    void categorySelectionChanged(const QStringList& categories);
    void tagSelectionChanged(const QStringList& tags);
    void contributorSelectionChanged(const QStringList& contributors);
    void selectionChanged();
    void filtersCleared();
    void filterModeChanged(FilterMode mode);
    void viewModeChanged(ViewMode mode);
    void filterStatisticsUpdated(const QVariantMap& stats);

private slots:
    void onCategorySelectionChanged(const QStringList& categories);
    void onTagSelectionChanged(const QStringList& tags);
    void onContributorSelectionChanged(const QStringList& contributors);
    void onClearFilters();
    void onViewModeChanged();
    void onFilterModeChanged();
    void onSearchFilterChanged();
    void onUpdateStatistics();

private:
    // Enhanced UI setup methods
    void setupUI();
    void setupTreeView();
    void setupListView();
    void setupCompactView();
    void setupTagCloudView();
    void setupStatisticsView();
    void setupToolbar();
    void setupAnimations();
    void setupConnections();

    // View management
    void updateViewMode();
    void updateFilterIndicator();
    void updateStatistics();
    void createCompactButtons();
    void createTagCloudButtons();
    void switchToView(ViewMode mode);

    // Filter management
    void applyFilters();
    void validateFilters();
    void saveFilterState();
    void loadFilterState();

    // Performance optimization
    void optimizeView();
    void preloadData();

    IconMetadataManager* m_metadataManager;

    // Enhanced UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_toolbarLayout;
    QStackedWidget* m_stackedWidget;
    QWidget* m_toolbar;
    QFrame* m_separator;

    // Enhanced view widgets
    CategoryTreeWidget* m_treeWidget;
    CategoryListWidget* m_listWidget;
    QWidget* m_compactWidget;
    TagCloudWidget* m_tagCloudWidget;
    CategoryStatisticsWidget* m_statisticsWidget;
    QHBoxLayout* m_compactLayout;

    // Enhanced toolbar controls
    QLabel* m_titleLabel;
    QComboBox* m_viewModeCombo;
    QComboBox* m_filterModeCombo;
    QToolButton* m_expandButton;
    QToolButton* m_collapseButton;
    QToolButton* m_selectAllButton;
    QToolButton* m_clearButton;
    QToolButton* m_settingsButton;
    QLabel* m_filterIndicator;
    QLabel* m_statisticsLabel;
    QLineEdit* m_searchEdit;

    // Compact view buttons
    QButtonGroup* m_compactButtonGroup;
    QList<QToolButton*> m_categoryButtons;
    QList<QToolButton*> m_tagButtons;

    // Enhanced state management
    ViewMode m_viewMode;
    FilterMode m_filterMode;
    QStringList m_selectedCategories;
    QStringList m_selectedTags;
    QStringList m_selectedContributors;
    QString m_searchFilter;
    int m_minimumIconCount;
    bool m_showEmptyCategories;
    bool m_showIconCounts;
    bool m_showStatistics;
    bool m_animationsEnabled;
    bool m_compactMode;

    // Animation system
    QPropertyAnimation* m_switchAnimation;
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;

    // Performance and statistics
    QTimer* m_statisticsTimer;
    QElapsedTimer m_performanceTimer;
    QVariantMap m_filterStatistics;

    // Settings
    QSettings* m_settings;
    QHash<QString, QVariant> m_filterPresets;

    // Constants
    static constexpr int ANIMATION_DURATION = 250;
    static constexpr int STATISTICS_UPDATE_INTERVAL = 2000; // 2 seconds
    static constexpr int DEFAULT_MIN_ICON_COUNT = 1;
    static constexpr const char* SETTINGS_GROUP = "CategoryFilterWidget";
};

#endif // CATEGORYFILTERWIDGET_H
