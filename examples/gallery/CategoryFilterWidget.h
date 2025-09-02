/**
 * QtLucide Gallery Application - Category Filter Widget Header
 * 
 * Provides hierarchical category filtering with expandable sections,
 * multi-selection capabilities, and visual feedback.
 */

#ifndef CATEGORYFILTERWIDGET_H
#define CATEGORYFILTERWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QSplitter>
#include <QListWidget>
#include <QListWidgetItem>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>

#include "IconMetadataManager.h"

/**
 * @brief Custom tree widget item for category display
 */
class CategoryTreeItem : public QTreeWidgetItem
{
public:
    enum ItemType {
        CategoryItem = QTreeWidgetItem::UserType + 1,
        TagItem,
        IconCountItem
    };

    CategoryTreeItem(QTreeWidget *parent, const QString& category, int iconCount);
    CategoryTreeItem(CategoryTreeItem *parent, const QString& tag, int iconCount);
    
    QString categoryName() const { return m_categoryName; }
    int iconCount() const { return m_iconCount; }
    bool isCategory() const { return type() == CategoryItem; }
    bool isTag() const { return type() == TagItem; }
    
    void setIconCount(int count);
    void updateDisplay();

private:
    QString m_categoryName;
    int m_iconCount;
};

/**
 * @brief Tree widget for displaying categories and tags hierarchically
 */
class CategoryTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit CategoryTreeWidget(QWidget *parent = nullptr);
    
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
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onItemChanged(QTreeWidgetItem *item, int column);
    void onItemClicked(QTreeWidgetItem *item, int column);
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
class CategoryListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit CategoryListWidget(QWidget *parent = nullptr);
    
    void setMetadataManager(IconMetadataManager* manager);
    void updateCategories();
    
    QStringList selectedCategories() const;
    void setSelectedCategories(const QStringList& categories);

signals:
    void categorySelectionChanged(const QStringList& categories);

private slots:
    void onItemChanged(QListWidgetItem *item);

private:
    void populateList();
    void emitSelectionChanged();
    
    IconMetadataManager* m_metadataManager;
    bool m_updatingSelection;
};

/**
 * @brief Main category filter widget with multiple view modes
 */
class CategoryFilterWidget : public QWidget
{
    Q_OBJECT

public:
    enum ViewMode {
        TreeView,       // Hierarchical tree with categories and tags
        ListView,       // Simple list of categories
        CompactView     // Horizontal button bar
    };

    explicit CategoryFilterWidget(IconMetadataManager* metadataManager, QWidget *parent = nullptr);
    ~CategoryFilterWidget();
    
    // Selection management
    QStringList selectedCategories() const;
    QStringList selectedTags() const;
    void setSelectedCategories(const QStringList& categories);
    void setSelectedTags(const QStringList& tags);
    void clearSelection();
    
    // View mode
    ViewMode viewMode() const { return m_viewMode; }
    void setViewMode(ViewMode mode);
    
    // Filter state
    bool hasActiveFilters() const;
    int selectedCategoryCount() const;
    int selectedTagCount() const;

public slots:
    void refreshCategories();
    void expandAll();
    void collapseAll();
    void selectAll();
    void deselectAll();
    void toggleViewMode();

signals:
    void categorySelectionChanged(const QStringList& categories);
    void tagSelectionChanged(const QStringList& tags);
    void selectionChanged();
    void filtersCleared();

private slots:
    void onCategorySelectionChanged(const QStringList& categories);
    void onTagSelectionChanged(const QStringList& tags);
    void onClearFilters();
    void onViewModeChanged();

private:
    void setupUI();
    void setupTreeView();
    void setupListView();
    void setupCompactView();
    void setupToolbar();
    void updateViewMode();
    void updateFilterIndicator();
    void createCompactButtons();
    
    IconMetadataManager* m_metadataManager;
    
    // UI components
    QVBoxLayout* m_layout;
    QHBoxLayout* m_toolbarLayout;
    QWidget* m_toolbar;
    QWidget* m_contentWidget;
    
    // View widgets
    CategoryTreeWidget* m_treeWidget;
    CategoryListWidget* m_listWidget;
    QWidget* m_compactWidget;
    QHBoxLayout* m_compactLayout;
    
    // Toolbar controls
    QLabel* m_titleLabel;
    QToolButton* m_viewModeButton;
    QToolButton* m_expandButton;
    QToolButton* m_collapseButton;
    QToolButton* m_selectAllButton;
    QToolButton* m_clearButton;
    QLabel* m_filterIndicator;
    
    // Compact view buttons
    QButtonGroup* m_compactButtonGroup;
    QList<QToolButton*> m_categoryButtons;
    
    // State
    ViewMode m_viewMode;
    QStringList m_selectedCategories;
    QStringList m_selectedTags;
    
    // Animation
    QPropertyAnimation* m_switchAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;
};

/**
 * @brief Floating category filter panel for overlay display
 */
class CategoryFilterPanel : public QFrame
{
    Q_OBJECT

public:
    explicit CategoryFilterPanel(IconMetadataManager* metadataManager, QWidget *parent = nullptr);
    
    CategoryFilterWidget* filterWidget() const { return m_filterWidget; }
    
    void showPanel();
    void hidePanel();
    bool isVisible() const { return m_isVisible; }

signals:
    void panelClosed();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onCloseButtonClicked();

private:
    void setupUI();
    void setupAnimation();
    
    CategoryFilterWidget* m_filterWidget;
    QVBoxLayout* m_layout;
    QHBoxLayout* m_headerLayout;
    QLabel* m_titleLabel;
    QToolButton* m_closeButton;
    
    QPropertyAnimation* m_slideAnimation;
    bool m_isVisible;
};

#endif // CATEGORYFILTERWIDGET_H
