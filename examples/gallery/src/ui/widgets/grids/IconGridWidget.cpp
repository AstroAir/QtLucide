/**
 * QtLucide Gallery Application - Icon Grid Widget Implementation
 */

#include "IconGridWidget.h"
#include "IconItem.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>
#include <QFocusEvent>
#include <QPaintEvent>
#include <QContextMenuEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

// IconGridModel Implementation
IconGridModel::IconGridModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_iconSize(64)
    , m_metadataManager(nullptr)
    , m_lucide(nullptr)
{
}

int IconGridModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(m_iconNames.size());
}

QVariant IconGridModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_iconNames.size()) {
        return QVariant();
    }

    const QString iconName = m_iconNames.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return iconName;
    case Qt::DecorationRole:
        if (m_lucide) {
            return m_lucide->icon(iconName);
        }
        break;
    case IconNameRole:
        return iconName;
    case IsFavoriteRole:
        if (m_metadataManager) {
            return m_metadataManager->isFavorite(iconName);
        }
        break;
    }

    return QVariant();
}

Qt::ItemFlags IconGridModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void IconGridModel::setIconNames(const QStringList& iconNames)
{
    beginResetModel();
    m_iconNames = iconNames;
    endResetModel();
}

void IconGridModel::setIconSize(int size)
{
    m_iconSize = size;
}

void IconGridModel::setMetadataManager(IconMetadataManager* manager)
{
    m_metadataManager = manager;
}

void IconGridModel::setLucide(lucide::QtLucide* lucide)
{
    m_lucide = lucide;
}

QString IconGridModel::iconNameAt(int index) const
{
    if (index >= 0 && index < m_iconNames.size()) {
        return m_iconNames.at(index);
    }
    return QString();
}

QStringList IconGridModel::iconNames() const
{
    return m_iconNames;
}

void IconGridModel::refreshData()
{
    beginResetModel();
    endResetModel();
}

// IconGridDelegate Implementation
IconGridDelegate::IconGridDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
    , m_iconSize(64)
    , m_showIconNames(true)
    , m_animationsEnabled(true)
    , m_hoverEffectsEnabled(true)
    , m_dropShadowEnabled(true)
    , m_highlightColor(QColor(100, 149, 237))
    , m_selectionColor(QColor(70, 130, 180))
    , m_displayMode(IconItem::GridMode)
{
}

void IconGridDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    if (!index.isValid()) {
        return;
    }

    painter->save();

    QRect rect = option.rect;
    QString iconName = index.data(IconGridModel::IconNameRole).toString();
    QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();
    bool selected = option.state & QStyle::State_Selected;
    bool favorite = index.data(IconGridModel::IsFavoriteRole).toBool();

    // Draw background
    if (selected) {
        painter->fillRect(rect, option.palette.highlight());
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(rect, option.palette.alternateBase());
    }

    // Draw icon
    QRect iconRect = getIconRect(rect);
    if (!icon.isNull()) {
        icon.paint(painter, iconRect);
    }

    // Draw icon name
    if (m_showIconNames) {
        QRect nameRect = getTextRect(rect);
        painter->setPen(selected ? option.palette.highlightedText().color() : option.palette.text().color());
        painter->drawText(nameRect, Qt::AlignCenter | Qt::TextWordWrap, iconName);
    }

    // Draw favorite indicator
    if (favorite) {
        QRect favRect = getFavoriteButtonRect(rect);
        painter->setPen(Qt::yellow);
        painter->drawText(favRect, Qt::AlignCenter, "★");
    }

    painter->restore();
}

QSize IconGridDelegate::sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    int width = m_iconSize + 2 * PADDING;
    int height = m_iconSize + 2 * PADDING;

    if (m_showIconNames) {
        height += TEXT_HEIGHT;
    }

    return QSize(width, height);
}

void IconGridDelegate::setIconSize(int size)
{
    m_iconSize = size;
}

void IconGridDelegate::setDisplayMode(IconItem::DisplayMode mode)
{
    m_displayMode = mode;
}

void IconGridDelegate::setShowIconNames(bool show)
{
    m_showIconNames = show;
}

QRect IconGridDelegate::getIconRect(const QRect &itemRect) const
{
    int x = itemRect.x() + (itemRect.width() - m_iconSize) / 2;
    int y = itemRect.y() + PADDING;
    return QRect(x, y, m_iconSize, m_iconSize);
}

QRect IconGridDelegate::getTextRect(const QRect &itemRect) const
{
    int y = itemRect.y() + PADDING + m_iconSize;
    return QRect(itemRect.x() + PADDING, y,
                 itemRect.width() - 2 * PADDING, TEXT_HEIGHT);
}

QRect IconGridDelegate::getFavoriteButtonRect(const QRect &itemRect) const
{
    int x = itemRect.right() - FAVORITE_BUTTON_SIZE - PADDING;
    int y = itemRect.y() + PADDING;
    return QRect(x, y, FAVORITE_BUTTON_SIZE, FAVORITE_BUTTON_SIZE);
}

// IconGridWidget Implementation
IconGridWidget::IconGridWidget(QWidget *parent)
    : QWidget(parent)
    , m_lucide(nullptr)
    , m_metadataManager(nullptr)
    , m_iconSize(DEFAULT_ICON_SIZE)
    , m_viewMode(GridView)
    , m_showIconNames(true)
    , m_updateTimer(new QTimer(this))
    , m_needsUpdate(false)
{
    setupUI();
    setupModel();
    setupView();

    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(UPDATE_DELAY);
    connect(m_updateTimer, &QTimer::timeout, this, &IconGridWidget::updateVisibleItems);
}

IconGridWidget::~IconGridWidget()
{
}

void IconGridWidget::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_listView = new QListView(this);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setUniformItemSizes(true);
    m_listView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_emptyLabel = new QLabel("No icons to display", this);
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setVisible(false);

    m_layout->addWidget(m_listView);
    m_layout->addWidget(m_emptyLabel);
}

void IconGridWidget::setupModel()
{
    m_model = std::make_unique<IconGridModel>(this);
    m_model->setLucide(m_lucide);
    m_model->setMetadataManager(m_metadataManager);
    m_model->setIconSize(m_iconSize);
}

void IconGridWidget::setupView()
{
    m_delegate = std::make_unique<IconGridDelegate>(this);
    m_delegate->setIconSize(m_iconSize);
    m_delegate->setShowIconNames(m_showIconNames);

    m_listView->setModel(m_model.get());
    m_listView->setItemDelegate(m_delegate.get());

    connect(m_listView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &IconGridWidget::onSelectionChanged);
    connect(m_listView, &QListView::doubleClicked,
            this, [this](const QModelIndex& index) {
                QString iconName = m_model->iconNameAt(index.row());
                if (!iconName.isEmpty()) {
                    emit iconDoubleClicked(iconName);
                }
            });
}

void IconGridWidget::setIconNames(const QStringList& iconNames)
{
    m_model->setIconNames(iconNames);
    m_emptyLabel->setVisible(iconNames.isEmpty());
    m_listView->setVisible(!iconNames.isEmpty());

    if (!iconNames.isEmpty()) {
        m_listView->scrollToTop();
    }
}

QStringList IconGridWidget::iconNames() const
{
    return m_model->iconNames();
}

void IconGridWidget::setIconSize(int size)
{
    size = qBound(MIN_ICON_SIZE, size, MAX_ICON_SIZE);
    if (m_iconSize != size) {
        m_iconSize = size;
        m_model->setIconSize(size);
        m_delegate->setIconSize(size);
        updateItemSize();
        emit iconSizeChanged(size);
    }
}

void IconGridWidget::setViewMode(ViewMode mode)
{
    if (mode != m_viewMode) {
        m_viewMode = mode;
        updateViewSettings();
        updateItemSize();
    }
}

void IconGridWidget::setShowIconNames(bool show)
{
    if (m_showIconNames != show) {
        m_showIconNames = show;
        m_delegate->setShowIconNames(show);
        updateItemSize();
    }
}

void IconGridWidget::selectIcon(const QString& iconName)
{
    // Find and select the icon
    for (int i = 0; i < m_model->rowCount(); ++i) {
        if (m_model->iconNameAt(i) == iconName) {
            QModelIndex index = m_model->index(i);
            m_listView->setCurrentIndex(index);
            m_listView->scrollTo(index);
            break;
        }
    }
}

void IconGridWidget::clearSelection()
{
    if (m_listView && m_listView->selectionModel()) {
        m_listView->selectionModel()->clearSelection();
        m_listView->clearSelection();
    }
}

QString IconGridWidget::currentIcon() const
{
    QModelIndex current = m_listView->currentIndex();
    if (current.isValid()) {
        return m_model->iconNameAt(current.row());
    }
    return QString();
}

void IconGridWidget::refreshIcons()
{
    m_model->refreshData();
}

void IconGridWidget::onSelectionChanged()
{
    QString iconName = currentIcon();
    if (!iconName.isEmpty()) {
        emit iconSelected(iconName);
    }
}

void IconGridWidget::updateItemSize()
{
    m_listView->update();
}

void IconGridWidget::updateVisibleItems()
{
    m_needsUpdate = false;
    // Update visible items if needed
}

// Missing method implementations
void IconGridWidget::updateIconSizes()
{
    if (m_model) {
        m_model->setIconSize(QSize(m_iconSize, m_iconSize));
    }

    if (m_delegate) {
        m_delegate->setIconSize(QSize(m_iconSize, m_iconSize));
    }

    // Update grid item size
    if (m_gridView) {
        int itemSize = m_iconSize + 20; // Add padding
        m_gridView->setGridSize(QSize(itemSize, itemSize));
        m_gridView->setIconSize(QSize(m_iconSize, m_iconSize));
    }

    // Force layout update
    update();
    emit iconSizeChanged(m_iconSize);
}

void IconGridWidget::updateFavorites()
{
    if (m_model && m_favoritesManager) {
        QStringList favorites = m_favoritesManager->getFavorites();
        m_model->setFavorites(favorites);

        // Update view to reflect favorite status changes
        if (m_gridView) {
            m_gridView->update();
        }

        emit favoritesUpdated(favorites.size());
    }
}
void IconGridWidget::zoomIn() { setIconSize(m_iconSize + ZOOM_STEP); }
void IconGridWidget::zoomOut() { setIconSize(m_iconSize - ZOOM_STEP); }
void IconGridWidget::resetZoom() { setIconSize(DEFAULT_ICON_SIZE); }
void IconGridWidget::onIconClicked(const QString& iconName, const QPoint& position) {
    Q_UNUSED(position);
    emit iconSelected(iconName);
}
void IconGridWidget::onIconDoubleClicked(const QString& iconName) { emit iconDoubleClicked(iconName); }
void IconGridWidget::onFavoriteToggled(const QString& iconName) { emit favoriteToggled(iconName, true); }
void IconGridWidget::onScrollValueChanged()
{
    if (m_gridView) {
        QScrollBar* vScrollBar = m_gridView->verticalScrollBar();
        QScrollBar* hScrollBar = m_gridView->horizontalScrollBar();

        if (vScrollBar) {
            int value = vScrollBar->value();
            int maximum = vScrollBar->maximum();

            // Emit scroll position for potential lazy loading
            emit scrollPositionChanged(value, maximum);

            // Check if we're near the bottom for infinite scroll
            if (maximum > 0 && value >= maximum * 0.9) {
                emit nearBottomReached();
            }
        }

        // Update visible items for performance optimization
        updateVisibleItems();
    }
}
void IconGridWidget::keyPressEvent(QKeyEvent *event) { QWidget::keyPressEvent(event); }
void IconGridWidget::wheelEvent(QWheelEvent *event) { QWidget::wheelEvent(event); }
void IconGridWidget::resizeEvent(QResizeEvent *event) { QWidget::resizeEvent(event); }

void IconGridWidget::updateViewSettings()
{
    if (!m_listView) return;

    switch (m_viewMode) {
        case GridView:
            m_listView->setViewMode(QListView::IconMode);
            m_listView->setFlow(QListView::LeftToRight);
            m_listView->setWrapping(true);
            m_listView->setResizeMode(QListView::Adjust);
            m_listView->setSpacing(8);
            break;

        case ListView:
            m_listView->setViewMode(QListView::ListMode);
            m_listView->setFlow(QListView::TopToBottom);
            m_listView->setWrapping(false);
            m_listView->setResizeMode(QListView::Fixed);
            m_listView->setSpacing(2);
            break;

        case CompactView:
            m_listView->setViewMode(QListView::IconMode);
            m_listView->setFlow(QListView::LeftToRight);
            m_listView->setWrapping(true);
            m_listView->setResizeMode(QListView::Adjust);
            m_listView->setSpacing(4);
            break;

        case DetailedView:
            m_listView->setViewMode(QListView::ListMode);
            m_listView->setFlow(QListView::TopToBottom);
            m_listView->setWrapping(false);
            m_listView->setResizeMode(QListView::Fixed);
            m_listView->setSpacing(1);
            break;
    }

    // Update delegate display mode
    if (m_delegate) {
        IconItem::DisplayMode displayMode = IconItem::GridMode; // Initialize with default
        switch (m_viewMode) {
            case GridView:
                displayMode = IconItem::GridMode;
                break;
            case ListView:
                displayMode = IconItem::ListMode;
                break;
            case CompactView:
                displayMode = IconItem::CompactMode;
                break;
            case DetailedView:
                displayMode = IconItem::ListMode;
                break;
        }
        m_delegate->setDisplayMode(displayMode);
    }
}

// IconGridModel missing methods
void IconGridModel::updateFavorites()
{
    if (m_favoritesManager) {
        m_favorites = m_favoritesManager->getFavorites().toSet();

        // Update all items to reflect favorite status changes
        emit dataChanged(index(0), index(rowCount() - 1), {Qt::DecorationRole, Qt::UserRole});

        emit favoritesUpdated();
    }
}

void IconGridModel::updateSelection() {
    // Update selection state
    emit dataChanged(index(0), index(rowCount() - 1));
}

void IconGridModel::invalidateCache() {
    // Clear any cached data
    beginResetModel();
    endResetModel();
}

IconGridModel::~IconGridModel() {
    // Destructor implementation
}

bool IconGridModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    Q_UNUSED(index) Q_UNUSED(value) Q_UNUSED(role)
    return false;
}

// IconGridDelegate missing methods
IconGridDelegate::~IconGridDelegate() {
    // Destructor implementation
}

void IconGridDelegate::onAnimationFinished() {
    // Handle animation completion
}

void IconGridDelegate::updateHoverState() {
    // Update hover visual state
}

bool IconGridDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) {
    Q_UNUSED(event) Q_UNUSED(model) Q_UNUSED(option) Q_UNUSED(index)
    return false;
}

// IconGridWidget missing methods
void IconGridWidget::updateSelection() {
    // Update selection state
    if (m_model) {
        m_model->updateSelection();
    }
}

void IconGridWidget::fitToWindow() {
    // Fit content to window
    if (m_listView) {
        m_listView->scrollToTop();
    }
}

void IconGridWidget::onFilterChanged() {
    // Handle filter changes
    updateViewSettings();
}

void IconGridWidget::onSearchHighlightChanged(const QString& highlight) {
    Q_UNUSED(highlight)
    // Update search highlighting
}

void IconGridWidget::onIconHovered(const QString& iconName) {
    Q_UNUSED(iconName)
    // Handle icon hover
}

void IconGridWidget::onModelDataChanged() {
    // Handle model data changes
    updateViewSettings();
}

void IconGridWidget::onUpdateTimer() {
    // Handle update timer
}

void IconGridWidget::onAnimationFinished() {
    // Handle animation completion
}

void IconGridWidget::updatePerformanceMetrics() {
    // Update performance metrics
}

// Event handlers
void IconGridWidget::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
}

void IconGridWidget::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
}

void IconGridWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    QWidget::mouseDoubleClickEvent(event);
}

void IconGridWidget::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
}

void IconGridWidget::focusInEvent(QFocusEvent *event) {
    QWidget::focusInEvent(event);
}

void IconGridWidget::focusOutEvent(QFocusEvent *event) {
    QWidget::focusOutEvent(event);
}

void IconGridWidget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
}

void IconGridWidget::contextMenuEvent(QContextMenuEvent *event) {
    QWidget::contextMenuEvent(event);
}

void IconGridWidget::dragEnterEvent(QDragEnterEvent *event) {
    QWidget::dragEnterEvent(event);
}

void IconGridWidget::dragMoveEvent(QDragMoveEvent *event) {
    QWidget::dragMoveEvent(event);
}

void IconGridWidget::dropEvent(QDropEvent *event) {
    QWidget::dropEvent(event);
}

// Helper method implementations
void IconGridWidget::updateVisibleItems()
{
    if (!m_gridView) return;

    // Get visible rect
    QRect visibleRect = m_gridView->viewport()->rect();
    QModelIndex topLeft = m_gridView->indexAt(visibleRect.topLeft());
    QModelIndex bottomRight = m_gridView->indexAt(visibleRect.bottomRight());

    if (topLeft.isValid() && bottomRight.isValid()) {
        int startRow = topLeft.row();
        int endRow = bottomRight.row();

        // Emit visible range for potential optimizations
        emit visibleRangeChanged(startRow, endRow);
    }
}

void IconGridWidget::setAnimationsEnabled(bool enabled)
{
    m_animationsEnabled = enabled;

    if (m_delegate) {
        m_delegate->setAnimationsEnabled(enabled);
    }

    // Update animation duration
    int duration = enabled ? 250 : 0;
    setProperty("animationDuration", duration);
}

void IconGridWidget::setSpacing(int spacing)
{
    m_spacing = spacing;
    if (m_gridView) {
        m_gridView->setSpacing(spacing);
    }
    update();
}

void IconGridWidget::setMargin(int margin)
{
    m_margin = margin;
    if (m_gridView) {
        m_gridView->setContentsMargins(margin, margin, margin, margin);
    }
    update();
}

void IconGridWidget::setFilteredIcons(const QStringList& icons)
{
    if (m_model) {
        m_model->setFilteredIcons(icons);
    }
    emit filteredIconsChanged(icons.size());
}

void IconGridWidget::clearFilter()
{
    if (m_model) {
        m_model->clearFilter();
    }
    emit filterCleared();
}

// Missing methods that are called from other code
void IconGridWidget::setLucide(lucide::QtLucide* lucide) {
    m_lucide = lucide;
    if (m_model) {
        m_model->setLucide(lucide);
    }
}

void IconGridWidget::setMetadataManager(IconMetadataManager* manager) {
    m_metadataManager = manager;
    if (m_model) {
        m_model->setMetadataManager(manager);
    }
}

// IconGridModel helper methods
void IconGridModel::setIconSize(const QSize& size)
{
    m_iconSize = size;
    emit dataChanged(index(0), index(rowCount() - 1), {Qt::SizeHintRole});
}

void IconGridModel::setFavorites(const QStringList& favorites)
{
    m_favorites = favorites.toSet();
    emit dataChanged(index(0), index(rowCount() - 1), {Qt::DecorationRole, Qt::UserRole});
}

void IconGridModel::setFilteredIcons(const QStringList& icons)
{
    beginResetModel();
    m_filteredIcons = icons;
    m_isFiltered = true;
    endResetModel();
}

void IconGridModel::clearFilter()
{
    beginResetModel();
    m_filteredIcons.clear();
    m_isFiltered = false;
    endResetModel();
}

// IconGridDelegate helper methods
void IconGridDelegate::setIconSize(const QSize& size)
{
    m_iconSize = size;
}

void IconGridDelegate::setAnimationsEnabled(bool enabled)
{
    m_animationsEnabled = enabled;
}

int IconGridWidget::iconSize() const {
    return m_iconSize;
}

IconGridWidget::ViewMode IconGridWidget::viewMode() const {
    return m_viewMode;
}
