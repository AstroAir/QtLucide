/**
 * QtLucide Gallery Application - Icon Thumbnail Grid Widget Implementation
 */

#include "IconThumbnailGridWidget.h"
#include "../../core/managers/ContentManager.h"
#include "../../core/managers/IconMetadataManager.h"
#include "../../core/managers/FavoritesManager.h"
#include "../../core/utils/GalleryLogger.h"
#include "../../config/LayoutConfig.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QToolTip>
#include <QMenu>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QClipboard>
#include <QDebug>

// Static constant definitions (required for ODR-used constants)
const int IconThumbnailGridWidget::MIN_THUMBNAIL_SIZE;
const int IconThumbnailGridWidget::MAX_THUMBNAIL_SIZE;
const int IconThumbnailGridWidget::DEFAULT_THUMBNAIL_SIZE;
const int IconThumbnailGridWidget::DEFAULT_ITEM_SPACING;
const int IconThumbnailGridWidget::DEFAULT_CONTENT_MARGIN;
const int IconThumbnailGridWidget::UPDATE_DELAY_MS;

// IconThumbnailItem implementation
IconThumbnailItem::IconThumbnailItem(const QString& iconName, QWidget* parent)
    : QWidget(parent)
    , m_iconName(iconName)
    , m_thumbnailSize(DEFAULT_THUMBNAIL_SIZE)
    , m_selected(false)
    , m_favorite(false)
    , m_hovered(false)
    , m_contentManager(nullptr)
    , m_iconMetadataManager(nullptr)
    , m_hoverAnimation(new QPropertyAnimation(this, "geometry", this))
    , m_tooltipTimer(new QTimer(this))
{
    setFixedSize(m_thumbnailSize + 16, m_thumbnailSize + 16);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    // Setup hover animation
    m_hoverAnimation->setDuration(150);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_hoverAnimation, &QPropertyAnimation::finished,
            this, &IconThumbnailItem::onHoverAnimationFinished);

    // Setup tooltip timer
    m_tooltipTimer->setSingleShot(true);
    m_tooltipTimer->setInterval(500);
    connect(m_tooltipTimer, &QTimer::timeout, this, &IconThumbnailItem::showTooltip);

    updateThumbnail();
}

void IconThumbnailItem::setIconName(const QString& iconName)
{
    if (m_iconName != iconName) {
        m_iconName = iconName;
        updateThumbnail();
        update();
    }
}

void IconThumbnailItem::setThumbnailSize(int size)
{
    if (m_thumbnailSize != size) {
        m_thumbnailSize = size;
        setFixedSize(size + 16, size + 16);
        updateThumbnail();
        update();
    }
}

void IconThumbnailItem::setSelected(bool selected)
{
    if (m_selected != selected) {
        m_selected = selected;
        updateAppearance();
        update();
    }
}

void IconThumbnailItem::setFavorite(bool favorite)
{
    if (m_favorite != favorite) {
        m_favorite = favorite;
        update();
    }
}

void IconThumbnailItem::updateThumbnail()
{
    if (!m_contentManager || m_iconName.isEmpty()) {
        return;
    }

    QSize size(m_thumbnailSize, m_thumbnailSize);
    m_thumbnail = m_contentManager->getPixmap(m_iconName, size);
    m_cachedThumbnail = m_thumbnail;

    update();
}

void IconThumbnailItem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect rect = this->rect();
    QRect contentRect = rect.adjusted(4, 4, -4, -4); // Improved padding
    QRect iconRect = contentRect.adjusted(4, 4, -4, -4);

    // Enhanced background with rounded corners and better visual feedback
    if (m_selected) {
        // Selected state with gradient and glow effect
        QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
        gradient.setColorAt(0, QColor(42, 130, 218, 180));
        gradient.setColorAt(1, QColor(42, 130, 218, 120));

        painter.setBrush(gradient);
        painter.setPen(QPen(QColor(42, 130, 218), 2));
        painter.drawRoundedRect(contentRect, 6, 6);

        // Inner glow effect
        painter.setPen(QPen(QColor(255, 255, 255, 100), 1));
        painter.drawRoundedRect(contentRect.adjusted(1, 1, -1, -1), 5, 5);

    } else if (m_hovered) {
        // Hover state with subtle elevation effect
        QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
        gradient.setColorAt(0, QColor(240, 240, 240, 150));
        gradient.setColorAt(1, QColor(220, 220, 220, 100));

        painter.setBrush(gradient);
        painter.setPen(QPen(QColor(180, 180, 180), 1));
        painter.drawRoundedRect(contentRect, 4, 4);

        // Subtle shadow effect
        painter.setPen(QPen(QColor(0, 0, 0, 30), 1));
        painter.drawRoundedRect(contentRect.adjusted(1, 1, 1, 1), 4, 4);

    } else {
        // Normal state with subtle border
        painter.setBrush(QColor(250, 250, 250, 80));
        painter.setPen(QPen(QColor(230, 230, 230), 1));
        painter.drawRoundedRect(contentRect, 3, 3);
    }

    // Draw icon with improved centering
    if (!m_thumbnail.isNull()) {
        QSize pixmapSize = m_thumbnail.size();
        QSize targetSize = iconRect.size();

        // Scale to fit while maintaining aspect ratio
        QSize scaledSize = pixmapSize.scaled(targetSize, Qt::KeepAspectRatio);

        // Center the scaled pixmap
        int x = iconRect.x() + (iconRect.width() - scaledSize.width()) / 2;
        int y = iconRect.y() + (iconRect.height() - scaledSize.height()) / 2;
        QRect targetRect(x, y, scaledSize.width(), scaledSize.height());

        // Add subtle drop shadow for better visual separation
        if (!m_selected) {
            painter.setOpacity(0.3);
            painter.drawPixmap(targetRect.adjusted(1, 1, 1, 1), m_thumbnail);
            painter.setOpacity(1.0);
        }

        painter.drawPixmap(targetRect, m_thumbnail);
    }

    // Enhanced favorite indicator with better positioning
    if (m_favorite) {
        QRect favoriteRect(contentRect.right() - FAVORITE_ICON_SIZE - 4,
                          contentRect.top() + 4,
                          FAVORITE_ICON_SIZE, FAVORITE_ICON_SIZE);

        // Gold background with gradient
        QRadialGradient gradient(favoriteRect.center(), FAVORITE_ICON_SIZE / 2);
        gradient.setColorAt(0, QColor(255, 215, 0));
        gradient.setColorAt(1, QColor(218, 165, 32));

        painter.setBrush(gradient);
        painter.setPen(QPen(QColor(184, 134, 11), 1));
        painter.drawEllipse(favoriteRect);

        // Star icon
        painter.setPen(Qt::black);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(favoriteRect, Qt::AlignCenter, "★");
    }
}

void IconThumbnailItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_iconName);
    }
    QWidget::mousePressEvent(event);
}

void IconThumbnailItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(m_iconName);
    }
    QWidget::mouseDoubleClickEvent(event);
}

void IconThumbnailItem::contextMenuEvent(QContextMenuEvent* event)
{
    emit rightClicked(m_iconName, event->globalPos());
}

void IconThumbnailItem::enterEvent(QEnterEvent* event)
{
    Q_UNUSED(event)
    m_hovered = true;
    m_tooltipTimer->start();
    update();
}

void IconThumbnailItem::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    m_hovered = false;
    m_tooltipTimer->stop();
    hideTooltip();
    update();
}

void IconThumbnailItem::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit doubleClicked(m_iconName);
        break;
    case Qt::Key_Space:
        emit clicked(m_iconName);
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void IconThumbnailItem::onHoverAnimationFinished()
{
    // Animation completed
}

void IconThumbnailItem::updateAppearance()
{
    // Update visual appearance based on state
    update();
}

void IconThumbnailItem::showTooltip()
{
    if (!m_iconMetadataManager || m_iconName.isEmpty()) {
        return;
    }

    IconMetadata metadata = m_iconMetadataManager->getIconMetadata(m_iconName);
    QString tooltipText;

    if (metadata.isValid()) {
        tooltipText = QString("<b>%1</b><br>").arg(metadata.getDisplayName());
        if (!metadata.categories.isEmpty()) {
            tooltipText += QString("Category: %1<br>").arg(metadata.categories.first());
        }
        if (!metadata.tags.isEmpty()) {
            QStringList limitedTags = metadata.tags.mid(0, 5);
            tooltipText += QString("Tags: %1").arg(limitedTags.join(", "));
            if (metadata.tags.size() > 5) {
                tooltipText += QString(" (+%1 more)").arg(metadata.tags.size() - 5);
            }
        }
    } else {
        tooltipText = m_iconName;
    }

    QToolTip::showText(mapToGlobal(rect().center()), tooltipText, this);
}

void IconThumbnailItem::hideTooltip()
{
    QToolTip::hideText();
}

// IconThumbnailGridWidget implementation
IconThumbnailGridWidget::IconThumbnailGridWidget(QWidget* parent)
    : QWidget(parent)
    , m_contentManager(nullptr)
    , m_iconMetadataManager(nullptr)
    , m_favoritesManager(nullptr)
    , m_scrollArea(nullptr)
    , m_scrollContent(nullptr)
    , m_gridLayout(nullptr)
    , m_firstVisibleIndex(0)
    , m_lastVisibleIndex(-1)
    , m_thumbnailSize(DEFAULT_THUMBNAIL_SIZE)
    , m_columnsPerRow(0)
    , m_itemSpacing(DEFAULT_ITEM_SPACING)
    , m_contentMargin(DEFAULT_CONTENT_MARGIN)
    , m_updateTimer(new QTimer(this))
    , m_needsUpdate(false)
{
    GALLERY_LOG_INFO(galleryInit, "IconThumbnailGridWidget constructor started");

    setupUI();

    // Setup update timer for performance
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(UPDATE_DELAY_MS);
    connect(m_updateTimer, &QTimer::timeout, this, &IconThumbnailGridWidget::updateVisibleItems);

    GALLERY_LOG_INFO(galleryInit, "IconThumbnailGridWidget initialized successfully");
}

IconThumbnailGridWidget::~IconThumbnailGridWidget()
{
    // Cleanup visible items
    for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
        delete it.value();
    }
    m_visibleItems.clear();
}

void IconThumbnailGridWidget::setContentManager(ContentManager* manager)
{
    m_contentManager = manager;
    refreshThumbnails();
}

void IconThumbnailGridWidget::setIconMetadataManager(IconMetadataManager* manager)
{
    m_iconMetadataManager = manager;
}

void IconThumbnailGridWidget::setFavoritesManager(FavoritesManager* manager)
{
    m_favoritesManager = manager;
    if (manager) {
        connect(manager, &FavoritesManager::favoriteToggled,
                this, &IconThumbnailGridWidget::onFavoriteToggled);
    }
}

void IconThumbnailGridWidget::setIconList(const QStringList& icons)
{
    m_iconList = icons;
    m_selectedIcons.clear();
    m_currentIcon.clear();

    calculateLayout();
    updateScrollArea();
    updateVisibleItems();

    if (!icons.isEmpty()) {
        setCurrentIcon(icons.first());
    }
}

void IconThumbnailGridWidget::setCurrentIcon(const QString& iconName)
{
    if (m_currentIcon != iconName && m_iconList.contains(iconName)) {
        m_currentIcon = iconName;

        // Update selection
        m_selectedIcons.clear();
        m_selectedIcons.append(iconName);

        // Update visible items
        for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
            it.value()->setSelected(it.key() == iconName);
        }

        // Scroll to icon if needed
        scrollToIcon(iconName);

        emit iconSelected(iconName);
        emit selectionChanged(m_selectedIcons);
    }
}

QStringList IconThumbnailGridWidget::selectedIcons() const
{
    return m_selectedIcons;
}

void IconThumbnailGridWidget::clearSelection()
{
    m_selectedIcons.clear();
    m_currentIcon.clear();

    for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
        it.value()->setSelected(false);
    }

    emit selectionChanged(m_selectedIcons);
}

void IconThumbnailGridWidget::selectAll()
{
    m_selectedIcons = m_iconList;

    for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
        it.value()->setSelected(true);
    }

    emit selectionChanged(m_selectedIcons);
}

void IconThumbnailGridWidget::setThumbnailSize(int size)
{
    size = qBound(MIN_THUMBNAIL_SIZE, size, MAX_THUMBNAIL_SIZE);

    if (m_thumbnailSize != size) {
        m_thumbnailSize = size;

        // Update all visible items
        for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
            it.value()->setThumbnailSize(size);
        }

        calculateLayout();
        updateScrollArea();
        updateVisibleItems();
    }
}

void IconThumbnailGridWidget::scrollToIcon(const QString& iconName)
{
    int index = getIconIndex(iconName);
    if (index >= 0) {
        QRect itemRect = getItemRect(index);
        m_scrollArea->ensureVisible(itemRect.center().x(), itemRect.center().y(),
                                   itemRect.width() / 2, itemRect.height() / 2);
    }
}

void IconThumbnailGridWidget::scrollToTop()
{
    m_scrollArea->verticalScrollBar()->setValue(0);
}

void IconThumbnailGridWidget::scrollToBottom()
{
    m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
}

void IconThumbnailGridWidget::updateVisibleItems()
{
    if (m_iconList.isEmpty() || !m_scrollArea) {
        return;
    }

    int firstVisible = getFirstVisibleIndex();
    int lastVisible = getLastVisibleIndex();

    // Remove items that are no longer visible
    destroyInvisibleItems();

    // Create new visible items
    for (int i = firstVisible; i <= lastVisible && i < m_iconList.size(); ++i) {
        const QString& iconName = m_iconList[i];

        if (!m_visibleItems.contains(iconName)) {
            IconThumbnailItem* item = new IconThumbnailItem(iconName, m_scrollContent);
            item->setContentManager(m_contentManager);
            item->setIconMetadataManager(m_iconMetadataManager);
            item->setThumbnailSize(m_thumbnailSize);
            item->setSelected(m_selectedIcons.contains(iconName));

            // Update thumbnail after setting content manager
            item->updateThumbnail();

            if (m_favoritesManager) {
                item->setFavorite(m_favoritesManager->isFavorite(iconName));
            }

            connect(item, &IconThumbnailItem::clicked, this, &IconThumbnailGridWidget::onItemClicked);
            connect(item, &IconThumbnailItem::doubleClicked, this, &IconThumbnailGridWidget::onItemDoubleClicked);
            connect(item, &IconThumbnailItem::rightClicked, this, &IconThumbnailGridWidget::onItemRightClicked);
            connect(item, &IconThumbnailItem::favoriteToggled, this, &IconThumbnailGridWidget::onFavoriteToggled);

            // Position the item
            QRect itemRect = getItemRect(i);
            item->move(itemRect.topLeft());
            item->show();

            m_visibleItems[iconName] = item;
        }
    }

    m_firstVisibleIndex = firstVisible;
    m_lastVisibleIndex = lastVisible;
    m_needsUpdate = false;
}

void IconThumbnailGridWidget::refreshThumbnails()
{
    for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
        it.value()->updateThumbnail();
    }
}

void IconThumbnailGridWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setupScrollArea();
    mainLayout->addWidget(m_scrollArea);
}

void IconThumbnailGridWidget::setupScrollArea()
{
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_scrollContent = new QWidget;
    m_scrollContent->setMinimumSize(100, 100);

    m_scrollArea->setWidget(m_scrollContent);

    connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
            this, &IconThumbnailGridWidget::onScrollValueChanged);
}

void IconThumbnailGridWidget::calculateLayout()
{
    if (!m_scrollArea || m_iconList.isEmpty()) {
        return;
    }

    // Get responsive screen size and adjust thumbnail size accordingly
    GalleryLayout::ResponsiveLayout::ScreenSize screenSize =
        GalleryLayout::ResponsiveLayout::getScreenSize(this);
    int responsiveThumbnailSize =
        GalleryLayout::ResponsiveLayout::getThumbnailSize(screenSize);

    // Update thumbnail size if it differs significantly from current
    if (qAbs(m_thumbnailSize - responsiveThumbnailSize) > 16) {
        m_thumbnailSize = responsiveThumbnailSize;
    }

    int viewportWidth = m_scrollArea->viewport()->width();
    int availableWidth = viewportWidth - 2 * GalleryLayout::Margins::CONTENT_MARGIN;

    // Use standardized spacing values
    int itemPadding = GalleryLayout::Spacing::SMALL; // 8px padding around each item
    int itemWidth = m_thumbnailSize + itemPadding;
    int itemSpacing = GalleryLayout::Spacing::WIDGET_SPACING; // 8px spacing between items

    // Use the responsive grid calculation from LayoutConfig
    int idealColumns = GalleryLayout::ResponsiveLayout::calculateGridColumns(
        availableWidth, m_thumbnailSize, itemSpacing);

    // Optimize spacing distribution for better visual balance
    if (idealColumns > 1) {
        int totalItemsWidth = idealColumns * itemWidth;
        int remainingWidth = availableWidth - totalItemsWidth;
        int spacingSlots = idealColumns + 1; // spaces before, between, and after items

        // Calculate optimal spacing, but keep within reasonable bounds
        int optimalSpacing = remainingWidth / spacingSlots;
        itemSpacing = qBound(GalleryLayout::Spacing::TINY, optimalSpacing, GalleryLayout::Spacing::LARGE);

        // If spacing becomes too large, reduce columns
        if (itemSpacing > GalleryLayout::Spacing::LARGE) {
            idealColumns = qMax(1, idealColumns - 1);
            // Recalculate spacing with fewer columns
            totalItemsWidth = idealColumns * itemWidth;
            remainingWidth = availableWidth - totalItemsWidth;
            spacingSlots = idealColumns + 1;
            itemSpacing = qBound(GalleryLayout::Spacing::SMALL,
                               remainingWidth / spacingSlots,
                               GalleryLayout::Spacing::LARGE);
        }
    }

    m_columnsPerRow = idealColumns;
    m_itemSpacing = itemSpacing;

    // Calculate content dimensions with standardized margins
    int totalRows = getTotalRows();
    int rowHeight = m_thumbnailSize + itemPadding + m_itemSpacing;
    int contentHeight = totalRows * rowHeight + 2 * GalleryLayout::Margins::CONTENT_MARGIN;

    // Ensure minimum content width for proper scrollbar behavior
    int contentWidth = qMax(viewportWidth,
                           m_columnsPerRow * (itemWidth + m_itemSpacing) +
                           2 * GalleryLayout::Margins::CONTENT_MARGIN);

    m_scrollContent->setMinimumSize(contentWidth, contentHeight);

    // Update content margin to use standardized value
    m_contentMargin = GalleryLayout::Margins::CONTENT_MARGIN;
}

void IconThumbnailGridWidget::updateScrollArea()
{
    calculateLayout();
}

void IconThumbnailGridWidget::createVisibleItems()
{
    // This is handled by updateVisibleItems()
}

void IconThumbnailGridWidget::destroyInvisibleItems()
{
    QStringList toRemove;

    for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
        int index = getIconIndex(it.key());
        if (index < m_firstVisibleIndex || index > m_lastVisibleIndex) {
            toRemove.append(it.key());
        }
    }

    for (const QString& iconName : toRemove) {
        delete m_visibleItems.take(iconName);
    }
}

int IconThumbnailGridWidget::getIconIndex(const QString& iconName) const
{
    return m_iconList.indexOf(iconName);
}

QString IconThumbnailGridWidget::getIconAt(int index) const
{
    if (index >= 0 && index < m_iconList.size()) {
        return m_iconList[index];
    }
    return QString();
}

void IconThumbnailGridWidget::selectIcon(const QString& iconName, bool multiSelect)
{
    if (!multiSelect) {
        m_selectedIcons.clear();
    }

    if (!m_selectedIcons.contains(iconName)) {
        m_selectedIcons.append(iconName);
    }

    m_currentIcon = iconName;

    // Update visual selection
    for (auto it = m_visibleItems.begin(); it != m_visibleItems.end(); ++it) {
        it.value()->setSelected(m_selectedIcons.contains(it.key()));
    }

    emit selectionChanged(m_selectedIcons);
}

void IconThumbnailGridWidget::navigateToIcon(int direction)
{
    int currentIndex = getIconIndex(m_currentIcon);
    int newIndex = currentIndex + direction;

    if (newIndex >= 0 && newIndex < m_iconList.size()) {
        setCurrentIcon(m_iconList[newIndex]);
    }
}

int IconThumbnailGridWidget::getFirstVisibleIndex() const
{
    if (!m_scrollArea || m_columnsPerRow <= 0) {
        return 0;
    }

    int scrollValue = m_scrollArea->verticalScrollBar()->value();
    int itemHeight = m_thumbnailSize + 16 + m_itemSpacing;
    int visibleRow = qMax(0, (scrollValue - m_contentMargin) / itemHeight);

    return visibleRow * m_columnsPerRow;
}

int IconThumbnailGridWidget::getLastVisibleIndex() const
{
    if (!m_scrollArea || m_columnsPerRow <= 0) {
        return qMin(50, m_iconList.size() - 1); // Show at least 50 items
    }

    int scrollValue = m_scrollArea->verticalScrollBar()->value();
    int viewportHeight = m_scrollArea->viewport()->height();
    int itemHeight = m_thumbnailSize + 16 + m_itemSpacing;

    int firstVisibleRow = qMax(0, (scrollValue - m_contentMargin) / itemHeight);
    int visibleRows = (viewportHeight / itemHeight) + 2; // Add buffer
    int lastVisibleRow = firstVisibleRow + visibleRows;

    return qMin((lastVisibleRow + 1) * m_columnsPerRow - 1, m_iconList.size() - 1);
}

int IconThumbnailGridWidget::getItemsPerRow() const
{
    return m_columnsPerRow;
}

int IconThumbnailGridWidget::getTotalRows() const
{
    if (m_columnsPerRow <= 0) {
        return 0;
    }
    return (m_iconList.size() + m_columnsPerRow - 1) / m_columnsPerRow;
}

QRect IconThumbnailGridWidget::getItemRect(int index) const
{
    if (m_columnsPerRow <= 0 || index < 0) {
        return QRect();
    }

    int row = index / m_columnsPerRow;
    int col = index % m_columnsPerRow;

    // Enhanced item dimensions with consistent padding
    int itemPadding = 8;
    int itemWidth = m_thumbnailSize + itemPadding;
    int itemHeight = m_thumbnailSize + itemPadding;

    // Improved positioning with better spacing distribution
    int totalItemsWidth = m_columnsPerRow * itemWidth;
    int totalSpacingWidth = m_scrollArea->viewport()->width() - 2 * m_contentMargin - totalItemsWidth;
    int spacingBetweenItems = (m_columnsPerRow > 1) ? totalSpacingWidth / (m_columnsPerRow - 1) : 0;
    spacingBetweenItems = qMax(m_itemSpacing, qMin(spacingBetweenItems, m_itemSpacing * 2));

    int x = m_contentMargin + col * (itemWidth + spacingBetweenItems);
    int y = m_contentMargin + row * (itemHeight + m_itemSpacing);

    return QRect(x, y, itemWidth, itemHeight);
}

// Event handlers
void IconThumbnailGridWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (!m_updateTimer->isActive()) {
        m_updateTimer->start();
    }
    m_needsUpdate = true;
}

void IconThumbnailGridWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        navigateToIcon(-1);
        break;
    case Qt::Key_Right:
        navigateToIcon(1);
        break;
    case Qt::Key_Up:
        navigateToIcon(-m_columnsPerRow);
        break;
    case Qt::Key_Down:
        navigateToIcon(m_columnsPerRow);
        break;
    case Qt::Key_Home:
        if (!m_iconList.isEmpty()) {
            setCurrentIcon(m_iconList.first());
        }
        break;
    case Qt::Key_End:
        if (!m_iconList.isEmpty()) {
            setCurrentIcon(m_iconList.last());
        }
        break;
    case Qt::Key_PageUp:
        navigateToIcon(-m_columnsPerRow * 5);
        break;
    case Qt::Key_PageDown:
        navigateToIcon(m_columnsPerRow * 5);
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void IconThumbnailGridWidget::wheelEvent(QWheelEvent* event)
{
    // Handle zoom with Ctrl+Wheel
    if (event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y();
        if (delta > 0) {
            setThumbnailSize(m_thumbnailSize + 8);
        } else {
            setThumbnailSize(m_thumbnailSize - 8);
        }
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

// Slot implementations
void IconThumbnailGridWidget::onScrollValueChanged(int value)
{
    Q_UNUSED(value)

    if (!m_updateTimer->isActive()) {
        m_updateTimer->start();
    }
    m_needsUpdate = true;
}

void IconThumbnailGridWidget::onItemClicked(const QString& iconName)
{
    selectIcon(iconName, QApplication::keyboardModifiers() & Qt::ControlModifier);
    emit iconSelected(iconName);
}

void IconThumbnailGridWidget::onItemDoubleClicked(const QString& iconName)
{
    emit iconDoubleClicked(iconName);
}

void IconThumbnailGridWidget::onItemRightClicked(const QString& iconName, const QPoint& globalPos)
{
    emit contextMenuRequested(iconName, globalPos);
}

void IconThumbnailGridWidget::onFavoriteToggled(const QString& iconName, bool favorite)
{
    if (m_visibleItems.contains(iconName)) {
        m_visibleItems[iconName]->setFavorite(favorite);
    }
}
