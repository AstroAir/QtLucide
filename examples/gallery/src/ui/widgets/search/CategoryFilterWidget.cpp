/**
 * QtLucide Gallery Application - Category Filter Widget Implementation
 */

#include "CategoryFilterWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDebug>

// CategoryTreeItem Implementation
CategoryTreeItem::CategoryTreeItem(QTreeWidget *parent, const QString& category, int iconCount, ItemType type)
    : QTreeWidgetItem(parent, type)
    , m_name(category)
    , m_iconCount(iconCount)
    , m_itemType(type)
{
    updateDisplay();
}

CategoryTreeItem::CategoryTreeItem(CategoryTreeItem *parent, const QString& tag, int iconCount, ItemType type)
    : QTreeWidgetItem(parent, type)
    , m_name(tag)
    , m_iconCount(iconCount)
    , m_itemType(type)
{
    updateDisplay();
}

void CategoryTreeItem::setIconCount(int count)
{
    m_iconCount = count;
    updateDisplay();
}

void CategoryTreeItem::updateDisplay()
{
    QString text = QString("%1 (%2)").arg(m_name).arg(m_iconCount);
    setText(0, text);
    setFlags(flags() | Qt::ItemIsUserCheckable);
    setCheckState(0, Qt::Unchecked);
}

// CategoryTreeWidget Implementation
CategoryTreeWidget::CategoryTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
    , m_metadataManager(nullptr)
    , m_updatingSelection(false)
{
    setHeaderLabel("Categories");
    setRootIsDecorated(true);
    setupContextMenu();

    connect(this, &QTreeWidget::itemChanged, this, &CategoryTreeWidget::onItemChanged);
    connect(this, &QTreeWidget::itemClicked, this, &CategoryTreeWidget::onItemClicked);
}

void CategoryTreeWidget::setupContextMenu()
{
    m_contextMenu = new QMenu(this);
    m_expandAllAction = m_contextMenu->addAction("Expand All");
    m_collapseAllAction = m_contextMenu->addAction("Collapse All");
    m_contextMenu->addSeparator();
    m_selectAllAction = m_contextMenu->addAction("Select All");
    m_deselectAllAction = m_contextMenu->addAction("Deselect All");

    connect(m_expandAllAction, &QAction::triggered, this, &CategoryTreeWidget::onExpandAll);
    connect(m_collapseAllAction, &QAction::triggered, this, &CategoryTreeWidget::onCollapseAll);
    connect(m_selectAllAction, &QAction::triggered, this, &CategoryTreeWidget::onSelectAll);
    connect(m_deselectAllAction, &QAction::triggered, this, &CategoryTreeWidget::onDeselectAll);
}

void CategoryTreeWidget::setMetadataManager(IconMetadataManager* manager)
{
    m_metadataManager = manager;
    updateCategories();
}

void CategoryTreeWidget::updateCategories()
{
    if (!m_metadataManager) return;

    clear();
    populateTree();
}

void CategoryTreeWidget::populateTree()
{
    QStringList categories = m_metadataManager->getAllCategories();

    for (const QString& category : categories) {
        QStringList icons = m_metadataManager->getIconsByCategory(category);
        addCategoryItem(category, static_cast<int>(icons.size()));
    }

    expandAll();
}

void CategoryTreeWidget::addCategoryItem(const QString& category, int iconCount)
{
    CategoryTreeItem* item = new CategoryTreeItem(this, category, iconCount);
    addTagItems(item, category);
}

void CategoryTreeWidget::addTagItems(CategoryTreeItem* categoryItem, const QString& category)
{
    Q_UNUSED(categoryItem)
    Q_UNUSED(category)
    // For simplicity, we'll just show the category without sub-tags
    // In a full implementation, you would add related tags as children
}

QStringList CategoryTreeWidget::selectedCategories() const
{
    QStringList selected;

    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        if (item->checkState(0) == Qt::Checked) {
            CategoryTreeItem* catItem = static_cast<CategoryTreeItem*>(item);
            if (catItem->getItemType() == CategoryTreeItem::CategoryItem) {
                selected.append(catItem->getName());
            }
        }
    }

    return selected;
}

void CategoryTreeWidget::setSelectedCategories(const QStringList& categories)
{
    m_updatingSelection = true;

    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        CategoryTreeItem* catItem = static_cast<CategoryTreeItem*>(item);
        if (catItem->getItemType() == CategoryTreeItem::CategoryItem) {
            Qt::CheckState state = categories.contains(catItem->getName()) ? Qt::Checked : Qt::Unchecked;
            item->setCheckState(0, state);
        }
    }

    m_updatingSelection = false;
}

void CategoryTreeWidget::clearSelection()
{
    m_updatingSelection = true;

    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        item->setCheckState(0, Qt::Unchecked);
    }

    m_updatingSelection = false;
    emitSelectionChanged();
}

void CategoryTreeWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(item)
    Q_UNUSED(column)
    if (!m_updatingSelection) {
        emitSelectionChanged();
    }
}

void CategoryTreeWidget::onItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(item)
    Q_UNUSED(column)
}

void CategoryTreeWidget::onExpandAll()
{
    expandAll();
}

void CategoryTreeWidget::onCollapseAll()
{
    collapseAll();
}

void CategoryTreeWidget::onSelectAll()
{
    m_updatingSelection = true;

    for (int i = 0; i < topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = topLevelItem(i);
        item->setCheckState(0, Qt::Checked);
    }

    m_updatingSelection = false;
    emitSelectionChanged();
}

void CategoryTreeWidget::onDeselectAll()
{
    clearSelection();
}

void CategoryTreeWidget::emitSelectionChanged()
{
    emit categorySelectionChanged(selectedCategories());
    emit selectionChanged();
}

void CategoryTreeWidget::contextMenuEvent(QContextMenuEvent *event)
{
    m_contextMenu->exec(event->globalPos());
}

// CategoryFilterWidget Implementation
CategoryFilterWidget::CategoryFilterWidget(IconMetadataManager* metadataManager, QWidget *parent)
    : QWidget(parent)
    , m_metadataManager(metadataManager)
    , m_viewMode(TreeView)
{
    setupUI();

    connect(m_treeWidget, &CategoryTreeWidget::categorySelectionChanged,
            this, &CategoryFilterWidget::onCategorySelectionChanged);
    connect(m_clearButton, &QToolButton::clicked,
            this, &CategoryFilterWidget::onClearFilters);
}

CategoryFilterWidget::~CategoryFilterWidget()
{
}

void CategoryFilterWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);

    setupToolbar();
    setupTreeView();

    m_mainLayout->addWidget(m_toolbar);
    m_mainLayout->addWidget(m_treeWidget, 1);
}

void CategoryFilterWidget::setupToolbar()
{
    m_toolbar = new QWidget(this);
    m_toolbarLayout = new QHBoxLayout(m_toolbar);
    m_toolbarLayout->setContentsMargins(0, 0, 0, 0);

    m_titleLabel = new QLabel("Categories", this);
    m_titleLabel->setStyleSheet("font-weight: bold;");

    m_clearButton = new QToolButton(this);
    m_clearButton->setText("Clear");
    m_clearButton->setToolTip("Clear all filters");

    m_expandButton = new QToolButton(this);
    m_expandButton->setText("Expand");
    m_expandButton->setToolTip("Expand all categories");

    m_collapseButton = new QToolButton(this);
    m_collapseButton->setText("Collapse");
    m_collapseButton->setToolTip("Collapse all categories");

    m_toolbarLayout->addWidget(m_titleLabel);
    m_toolbarLayout->addStretch();
    m_toolbarLayout->addWidget(m_expandButton);
    m_toolbarLayout->addWidget(m_collapseButton);
    m_toolbarLayout->addWidget(m_clearButton);

    connect(m_expandButton, &QToolButton::clicked, this, &CategoryFilterWidget::expandAll);
    connect(m_collapseButton, &QToolButton::clicked, this, &CategoryFilterWidget::collapseAll);
}

void CategoryFilterWidget::setupTreeView()
{
    m_treeWidget = new CategoryTreeWidget(this);
    m_treeWidget->setMetadataManager(m_metadataManager);
}

QStringList CategoryFilterWidget::selectedCategories() const
{
    return m_treeWidget->selectedCategories();
}

void CategoryFilterWidget::setSelectedCategories(const QStringList& categories)
{
    m_treeWidget->setSelectedCategories(categories);
}

void CategoryFilterWidget::clearSelection()
{
    m_treeWidget->clearSelection();
}

void CategoryFilterWidget::refreshCategories()
{
    m_treeWidget->updateCategories();
}

void CategoryFilterWidget::expandAll()
{
    m_treeWidget->expandAll();
}

void CategoryFilterWidget::collapseAll()
{
    m_treeWidget->collapseAll();
}

bool CategoryFilterWidget::hasActiveFilters() const
{
    return !selectedCategories().isEmpty();
}

int CategoryFilterWidget::selectedCategoryCount() const
{
    return static_cast<int>(selectedCategories().size());
}

void CategoryFilterWidget::onCategorySelectionChanged(const QStringList& categories)
{
    m_selectedCategories = categories;
    emit categorySelectionChanged(categories);
    emit selectionChanged();
}

void CategoryFilterWidget::onClearFilters()
{
    clearSelection();
    emit filtersCleared();
}

// Missing method implementations
void CategoryFilterWidget::selectAll() { /* TODO: Call public method */ }
void CategoryFilterWidget::deselectAll() { /* TODO: Call public method */ }
void CategoryFilterWidget::toggleViewMode() { /* TODO */ }
void CategoryFilterWidget::onTagSelectionChanged(const QStringList& tags) { Q_UNUSED(tags) }
void CategoryFilterWidget::onViewModeChanged() { /* TODO */ }
QStringList CategoryFilterWidget::selectedTags() const { return QStringList(); }
void CategoryFilterWidget::setSelectedTags(const QStringList& tags) { Q_UNUSED(tags) }
int CategoryFilterWidget::selectedTagCount() const { return 0; }

// CategoryListWidget missing methods
CategoryListWidget::CategoryListWidget(QWidget *parent) : QListWidget(parent), m_metadataManager(nullptr), m_updatingSelection(false) {}
void CategoryListWidget::setMetadataManager(IconMetadataManager* manager) { m_metadataManager = manager; }
void CategoryListWidget::updateCategories() { /* TODO */ }
QStringList CategoryListWidget::selectedCategories() const { return QStringList(); }
void CategoryListWidget::setSelectedCategories(const QStringList& categories) { Q_UNUSED(categories) }
void CategoryListWidget::onItemChanged(QListWidgetItem *item) { Q_UNUSED(item) }
void CategoryListWidget::populateList() { /* TODO */ }
void CategoryListWidget::emitSelectionChanged() { /* TODO */ }

// QuickFilterWidget is implemented in SearchWidget.cpp

// CategoryFilterPanel implementation removed - class not defined in header

// Missing methods declared in headers but not implemented
void CategoryFilterWidget::refreshTags() { /* TODO */ }
void CategoryFilterWidget::refreshContributors() { /* TODO */ }
void CategoryFilterWidget::refreshAll() { /* TODO */ }
void CategoryFilterWidget::resetFilters() { /* TODO */ }
void CategoryFilterWidget::saveFilterPreset(const QString& name) { Q_UNUSED(name) /* TODO */ }
void CategoryFilterWidget::loadFilterPreset(const QString& name) { Q_UNUSED(name) /* TODO */ }
void CategoryFilterWidget::onContributorSelectionChanged(const QStringList& contributors) { Q_UNUSED(contributors) /* TODO */ }
void CategoryFilterWidget::onFilterModeChanged() { /* TODO */ }
void CategoryFilterWidget::onSearchFilterChanged() { /* TODO */ }
void CategoryFilterWidget::onUpdateStatistics() { /* TODO */ }

// Only methods that are actually declared in headers are implemented above
