/**
 * QtLucide Gallery Application - Icon Grid Widget Implementation
 */

#include "IconGridWidget.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QApplication>
#include <QDebug>

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
    return m_iconNames.size();
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
    , m_displayMode(IconItem::GridMode)
    , m_showIconNames(true)
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
        QRect nameRect = getNameRect(rect);
        painter->setPen(selected ? option.palette.highlightedText().color() : option.palette.text().color());
        painter->drawText(nameRect, Qt::AlignCenter | Qt::TextWordWrap, iconName);
    }

    // Draw favorite indicator
    if (favorite) {
        QRect favRect = getFavoriteRect(rect);
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

    int width = m_iconSize + 2 * ITEM_MARGIN;
    int height = m_iconSize + 2 * ITEM_MARGIN;

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
    int y = itemRect.y() + ITEM_MARGIN;
    return QRect(x, y, m_iconSize, m_iconSize);
}

QRect IconGridDelegate::getNameRect(const QRect &itemRect) const
{
    int y = itemRect.y() + ITEM_MARGIN + m_iconSize;
    return QRect(itemRect.x() + ITEM_MARGIN, y,
                 itemRect.width() - 2 * ITEM_MARGIN, TEXT_HEIGHT);
}

QRect IconGridDelegate::getFavoriteRect(const QRect &itemRect) const
{
    int x = itemRect.right() - FAVORITE_SIZE - ITEM_MARGIN;
    int y = itemRect.y() + ITEM_MARGIN;
    return QRect(x, y, FAVORITE_SIZE, FAVORITE_SIZE);
}

// IconGridWidget Implementation
IconGridWidget::IconGridWidget(lucide::QtLucide* lucide,
                             IconMetadataManager* metadataManager,
                             QWidget *parent)
    : QWidget(parent)
    , m_lucide(lucide)
    , m_metadataManager(metadataManager)
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
    m_model = new IconGridModel(this);
    m_model->setLucide(m_lucide);
    m_model->setMetadataManager(m_metadataManager);
    m_model->setIconSize(m_iconSize);
}

void IconGridWidget::setupView()
{
    m_delegate = new IconGridDelegate(this);
    m_delegate->setIconSize(m_iconSize);
    m_delegate->setShowIconNames(m_showIconNames);

    m_listView->setModel(m_model);
    m_listView->setItemDelegate(m_delegate);

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

QString IconGridWidget::currentIconName() const
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
    QString iconName = currentIconName();
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
void IconGridWidget::updateIconSizes() { /* TODO */ }
void IconGridWidget::updateFavorites() { /* TODO */ }
void IconGridWidget::zoomIn() { setIconSize(m_iconSize + ZOOM_STEP); }
void IconGridWidget::zoomOut() { setIconSize(m_iconSize - ZOOM_STEP); }
void IconGridWidget::resetZoom() { setIconSize(DEFAULT_ICON_SIZE); }
void IconGridWidget::onIconClicked(const QString& iconName) { emit iconSelected(iconName); }
void IconGridWidget::onIconDoubleClicked(const QString& iconName) { emit iconDoubleClicked(iconName); }
void IconGridWidget::onFavoriteToggled(const QString& iconName) { emit favoriteToggled(iconName, true); }
void IconGridWidget::onScrollValueChanged() { /* TODO */ }
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
    }

    // Update delegate display mode
    if (m_delegate) {
        IconItem::DisplayMode displayMode;
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
        }
        m_delegate->setDisplayMode(displayMode);
    }
}

// IconGridModel missing methods
void IconGridModel::updateFavorites() { /* TODO */ }

// IconGridDelegate missing methods
bool IconGridDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                 const QStyleOptionViewItem &option,
                                 const QModelIndex &index) {
    Q_UNUSED(event) Q_UNUSED(model) Q_UNUSED(option) Q_UNUSED(index)
    return false;
}
