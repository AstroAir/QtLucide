/**
 * QtLucide Gallery Application - Category Sidebar Widget Implementation
 */

#include "CategorySidebarWidget.h"
#include "../../core/managers/IconMetadataManager.h"
#include "../../core/managers/FavoritesManager.h"
#include "../../core/utils/GalleryLogger.h"
#include "../../config/LayoutConfig.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QShortcut>
#include <QKeyEvent>
#include <QDebug>

// CategoryItem implementation
CategoryItem::CategoryItem(const QString& categoryName, const QString& displayName,
                          int iconCount, QWidget* parent)
    : QWidget(parent)
    , m_categoryName(categoryName)
    , m_displayName(displayName)
    , m_iconCount(iconCount)
    , m_selected(false)
    , m_hovered(false)
{
    setFixedHeight(ITEM_HEIGHT + 8); // Increased height for better spacing
    setMouseTracking(true);

    // Enhanced layout with better margins
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 6, 12, 6);
    layout->setSpacing(8);

    // Category icon placeholder (could be enhanced with actual icons)
    QLabel* iconLabel = new QLabel("📁");
    iconLabel->setFixedSize(16, 16);
    iconLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(iconLabel);

    // Name label with improved typography
    m_nameLabel = new QLabel(m_displayName);
    m_nameLabel->setStyleSheet(
        "QLabel {"
        "  font-weight: 600;"
        "  font-size: 13px;"
        "  color: #2c3e50;"
        "}"
    );
    layout->addWidget(m_nameLabel);

    layout->addStretch();

    // Count badge with enhanced styling
    m_countLabel = new QLabel(QString::number(m_iconCount));
    m_countLabel->setStyleSheet(
        "QLabel {"
        "  background-color: #ecf0f1;"
        "  color: #7f8c8d;"
        "  font-size: 11px;"
        "  font-weight: 500;"
        "  padding: 2px 6px;"
        "  border-radius: 8px;"
        "  min-width: 20px;"
        "}"
    );
    m_countLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_countLabel);

    // Shortcut label with better styling
    m_shortcutLabel = new QLabel();
    m_shortcutLabel->setStyleSheet(
        "QLabel {"
        "  color: #95a5a6;"
        "  font-size: 10px;"
        "  font-family: 'Consolas', monospace;"
        "  padding: 1px 4px;"
        "  border: 1px solid #bdc3c7;"
        "  border-radius: 3px;"
        "  background-color: #f8f9fa;"
        "}"
    );
    layout->addWidget(m_shortcutLabel);

    updateAppearance();
}

void CategoryItem::setDisplayName(const QString& name)
{
    m_displayName = name;
    m_nameLabel->setText(name);
}

void CategoryItem::setIconCount(int count)
{
    m_iconCount = count;
    m_countLabel->setText(QString::number(count));
}

void CategoryItem::setSelected(bool selected)
{
    if (m_selected != selected) {
        m_selected = selected;
        updateAppearance();
        update();
    }
}

void CategoryItem::setShortcutKey(const QString& key)
{
    m_shortcutKey = key;
    m_shortcutLabel->setText(key);
}

void CategoryItem::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();
    QRect contentRect = rect.adjusted(4, 2, -4, -2);

    if (m_selected) {
        // Enhanced selection background with gradient
        QLinearGradient gradient(contentRect.topLeft(), contentRect.bottomLeft());
        gradient.setColorAt(0, QColor(52, 152, 219, 150));
        gradient.setColorAt(1, QColor(41, 128, 185, 120));

        painter.setBrush(gradient);
        painter.setPen(QPen(QColor(41, 128, 185), 2));
        painter.drawRoundedRect(contentRect, 6, 6);

        // Left accent bar
        QRect accentRect(rect.left(), rect.top() + 4, 3, rect.height() - 8);
        painter.fillRect(accentRect, QColor(41, 128, 185));

    } else if (m_hovered) {
        // Enhanced hover background
        QLinearGradient gradient(contentRect.topLeft(), contentRect.bottomLeft());
        gradient.setColorAt(0, QColor(236, 240, 241, 120));
        gradient.setColorAt(1, QColor(220, 221, 225, 80));

        painter.setBrush(gradient);
        painter.setPen(QPen(QColor(189, 195, 199, 150), 1));
        painter.drawRoundedRect(contentRect, 4, 4);

    } else {
        // Subtle normal state background
        painter.setBrush(QColor(255, 255, 255, 30));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(contentRect, 3, 3);
    }
}

void CategoryItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(m_categoryName);
    }
    QWidget::mousePressEvent(event);
}

void CategoryItem::contextMenuEvent(QContextMenuEvent* event)
{
    emit contextMenuRequested(m_categoryName, event->globalPos());
}

void CategoryItem::enterEvent(QEnterEvent* event)
{
    Q_UNUSED(event)
    m_hovered = true;
    update();
}

void CategoryItem::leaveEvent(QEvent* event)
{
    Q_UNUSED(event)
    m_hovered = false;
    update();
}

void CategoryItem::updateAppearance()
{
    // Update styling based on state
    if (m_selected) {
        m_nameLabel->setStyleSheet("font-weight: bold; color: white;");
        m_countLabel->setStyleSheet("color: lightgray; font-size: 11px;");
        m_shortcutLabel->setStyleSheet("color: lightgray; font-size: 10px;");
    } else {
        m_nameLabel->setStyleSheet("font-weight: bold;");
        m_countLabel->setStyleSheet("color: gray; font-size: 11px;");
        m_shortcutLabel->setStyleSheet("color: gray; font-size: 10px;");
    }
}

// CategorySidebarWidget implementation
CategorySidebarWidget::CategorySidebarWidget(QWidget* parent)
    : QWidget(parent)
    , m_iconMetadataManager(nullptr)
    , m_favoritesManager(nullptr)
    , m_currentCategory("all")
    , m_compactMode(false)
{
    GALLERY_LOG_INFO(galleryInit, "CategorySidebarWidget constructor started");

    setupUI();

    // Initialize category information
    m_categoryInfos = {
        {"business", "Business & Finance", "Charts, finance, and business-related icons", "Ctrl+1", 0},
        {"communication", "Communication", "Mail, messaging, and communication icons", "Ctrl+2", 0},
        {"editing", "Editing & Tools", "Text editing, formatting, and tool icons", "Ctrl+3", 0},
        {"files", "Files & Folders", "File management and folder icons", "Ctrl+4", 0},
        {"general", "General", "Common utility and general-purpose icons", "Ctrl+5", 0},
        {"media", "Media & Entertainment", "Audio, video, and entertainment icons", "Ctrl+6", 0},
        {"navigation", "Navigation", "Arrows, directions, and navigation icons", "Ctrl+7", 0},
        {"social", "Social & People", "People, sharing, and social media icons", "Ctrl+8", 0},
        {"system", "System & Settings", "Settings, system, and configuration icons", "Ctrl+9", 0}
    };

    GALLERY_LOG_INFO(galleryInit, "CategorySidebarWidget initialized successfully");
}

CategorySidebarWidget::~CategorySidebarWidget()
{
    // Cleanup shortcuts
    for (QShortcut* shortcut : m_categoryShortcuts) {
        delete shortcut;
    }
}

void CategorySidebarWidget::setIconMetadataManager(IconMetadataManager* manager)
{
    m_iconMetadataManager = manager;
    if (manager) {
        refreshCategories();
    }
}

void CategorySidebarWidget::setFavoritesManager(FavoritesManager* manager)
{
    m_favoritesManager = manager;
    if (manager) {
        connect(manager, &FavoritesManager::favoritesChanged,
                this, [this]() {
                    if (m_favoritesManager) {
                        updateFavoritesCount(m_favoritesManager->getTotalFavorites());
                    }
                });
    }
}

void CategorySidebarWidget::refreshCategories()
{
    if (!m_iconMetadataManager) {
        return;
    }

    updateCategoryCounts();

    // Update special categories
    QStringList allIcons = m_iconMetadataManager->getAllIconNames();
    if (m_allCategoriesItem) {
        m_allCategoriesItem->setIconCount(allIcons.size());
    }

    if (m_favoritesManager && m_favoritesItem) {
        m_favoritesItem->setIconCount(m_favoritesManager->getTotalFavorites());
    }

    if (m_favoritesManager && m_recentItem) {
        m_recentItem->setIconCount(m_favoritesManager->getRecentlyViewed(50).size());
    }
}

void CategorySidebarWidget::setCurrentCategory(const QString& category)
{
    if (m_currentCategory != category) {
        m_currentCategory = category;
        selectCategory(category);
    }
}

void CategorySidebarWidget::updateFavoritesCount(int count)
{
    if (m_favoritesItem) {
        m_favoritesItem->setIconCount(count);
    }
}

void CategorySidebarWidget::updateRecentCount(int count)
{
    if (m_recentItem) {
        m_recentItem->setIconCount(count);
    }
}

void CategorySidebarWidget::setCompactMode(bool compact)
{
    m_compactMode = compact;
    // TODO: Implement compact mode layout changes
}

void CategorySidebarWidget::setupUI()
{
    // Enhanced main layout with standardized spacing
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(GalleryLayout::Margins::PANEL_MARGIN,
                                    GalleryLayout::Margins::PANEL_MARGIN,
                                    GalleryLayout::Margins::PANEL_MARGIN,
                                    GalleryLayout::Margins::PANEL_MARGIN);
    m_mainLayout->setSpacing(GalleryLayout::Spacing::WIDGET_SPACING);

    // Sidebar title
    QLabel* titleLabel = new QLabel("Categories");
    titleLabel->setStyleSheet(
        "QLabel {"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "  color: #2c3e50;"
        "  padding: 8px 4px;"
        "  border-bottom: 2px solid #ecf0f1;"
        "}"
    );
    m_mainLayout->addWidget(titleLabel);

    // Enhanced scroll area with better styling
    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setStyleSheet(
        "QScrollArea {"
        "  background-color: transparent;"
        "  border: none;"
        "}"
        "QScrollBar:vertical {"
        "  background-color: #f8f9fa;"
        "  width: 8px;"
        "  border-radius: 4px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background-color: #bdc3c7;"
        "  border-radius: 4px;"
        "  min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background-color: #95a5a6;"
        "}"
    );

    m_scrollContent = new QWidget;
    m_contentLayout = new QVBoxLayout(m_scrollContent);
    m_contentLayout->setContentsMargins(4, 4, 4, 4);
    m_contentLayout->setSpacing(6);

    setupSpecialCategories();
    setupMainCategories();
    setupRecentCategories();

    m_contentLayout->addStretch();

    m_scrollArea->setWidget(m_scrollContent);
    m_mainLayout->addWidget(m_scrollArea);

    // Set responsive sizes based on screen size
    GalleryLayout::ResponsiveLayout::ScreenSize screenSize =
        GalleryLayout::ResponsiveLayout::getScreenSize(this);
    int sidebarWidth = GalleryLayout::ResponsiveLayout::getSidebarWidth(screenSize);

    setMinimumWidth(GalleryLayout::Sizing::SIDEBAR_MIN_WIDTH);
    setMaximumWidth(GalleryLayout::Sizing::SIDEBAR_MAX_WIDTH);
    setFixedWidth(sidebarWidth);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void CategorySidebarWidget::setupSpecialCategories()
{
    m_specialGroup = new QGroupBox("Quick Access");
    m_specialGroup->setStyleSheet(
        "QGroupBox {"
        "  font-weight: 600;"
        "  font-size: 12px;"
        "  color: #34495e;"
        "  border: 2px solid #ecf0f1;"
        "  border-radius: 8px;"
        "  margin-top: 8px;"
        "  padding-top: 4px;"
        "  background-color: #fdfdfd;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 8px;"
        "  padding: 0 8px 0 8px;"
        "  background-color: #fdfdfd;"
        "}"
    );

    QVBoxLayout* specialLayout = new QVBoxLayout(m_specialGroup);
    specialLayout->setContentsMargins(8, 16, 8, 8);
    specialLayout->setSpacing(4);

    // All categories item
    m_allCategoriesItem = new CategoryItem("all", "All Icons", 0);
    connect(m_allCategoriesItem, &CategoryItem::clicked, this, &CategorySidebarWidget::onShowAll);
    specialLayout->addWidget(m_allCategoriesItem);

    // Favorites item
    m_favoritesItem = new CategoryItem("favorites", "Favorites", 0);
    connect(m_favoritesItem, &CategoryItem::clicked, this, &CategorySidebarWidget::onShowFavorites);
    specialLayout->addWidget(m_favoritesItem);

    // Recent item
    m_recentItem = new CategoryItem("recent", "Recently Viewed", 0);
    connect(m_recentItem, &CategoryItem::clicked, this, &CategorySidebarWidget::onShowRecent);
    specialLayout->addWidget(m_recentItem);

    m_contentLayout->addWidget(m_specialGroup);
}

void CategorySidebarWidget::setupMainCategories()
{
    m_mainGroup = new QGroupBox("Icon Categories");
    m_mainGroup->setStyleSheet(
        "QGroupBox {"
        "  font-weight: 600;"
        "  font-size: 12px;"
        "  color: #34495e;"
        "  border: 2px solid #ecf0f1;"
        "  border-radius: 8px;"
        "  margin-top: 8px;"
        "  padding-top: 4px;"
        "  background-color: #fdfdfd;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  left: 8px;"
        "  padding: 0 8px 0 8px;"
        "  background-color: #fdfdfd;"
        "}"
    );

    QVBoxLayout* mainLayout = new QVBoxLayout(m_mainGroup);
    mainLayout->setContentsMargins(8, 16, 8, 8);
    mainLayout->setSpacing(4);

    // Create category items
    for (int i = 0; i < m_categoryInfos.size(); ++i) {
        const CategoryInfo& info = m_categoryInfos[i];

        CategoryItem* item = new CategoryItem(info.name, info.displayName, info.iconCount);
        item->setShortcutKey(info.shortcut);

        connect(item, &CategoryItem::clicked, this, &CategorySidebarWidget::onCategoryClicked);
        connect(item, &CategoryItem::contextMenuRequested,
                this, &CategorySidebarWidget::onCategoryContextMenu);

        m_categoryItems.append(item);
        m_categoryMap[info.name] = item;
        mainLayout->addWidget(item);
    }

    m_contentLayout->addWidget(m_mainGroup);
}

void CategorySidebarWidget::setupRecentCategories()
{
    m_recentGroup = new QGroupBox("Recent Categories");
    QVBoxLayout* recentLayout = new QVBoxLayout(m_recentGroup);
    recentLayout->setContentsMargins(4, 8, 4, 4);
    recentLayout->setSpacing(2);

    // Initially hidden - will be shown when there are recent categories
    m_recentGroup->setVisible(false);

    m_contentLayout->addWidget(m_recentGroup);
}

void CategorySidebarWidget::updateCategoryCounts()
{
    if (!m_iconMetadataManager) {
        return;
    }

    for (int i = 0; i < m_categoryInfos.size(); ++i) {
        CategoryInfo& info = m_categoryInfos[i];
        QStringList categoryIcons = m_iconMetadataManager->getIconsByCategory(info.name);
        info.iconCount = categoryIcons.size();

        if (m_categoryMap.contains(info.name)) {
            m_categoryMap[info.name]->setIconCount(info.iconCount);
        }
    }
}

void CategorySidebarWidget::selectCategory(const QString& category)
{
    // Deselect all items
    if (m_allCategoriesItem) m_allCategoriesItem->setSelected(false);
    if (m_favoritesItem) m_favoritesItem->setSelected(false);
    if (m_recentItem) m_recentItem->setSelected(false);

    for (CategoryItem* item : m_categoryItems) {
        item->setSelected(false);
    }

    // Select the appropriate item
    if (category == "all") {
        if (m_allCategoriesItem) m_allCategoriesItem->setSelected(true);
    } else if (category == "favorites") {
        if (m_favoritesItem) m_favoritesItem->setSelected(true);
    } else if (category == "recent") {
        if (m_recentItem) m_recentItem->setSelected(true);
    } else if (m_categoryMap.contains(category)) {
        m_categoryMap[category]->setSelected(true);
    }
}

// Slot implementations
void CategorySidebarWidget::onCategoryClicked(const QString& category)
{
    setCurrentCategory(category);
    emit categorySelected(category);
}

void CategorySidebarWidget::onCategoryContextMenu(const QString& category, const QPoint& globalPos)
{
    QMenu menu;
    menu.addAction("Clear Category", [this, category]() {
        emit clearCategoryRequested(category);
    });
    menu.addAction("Refresh", this, &CategorySidebarWidget::onRefreshCategories);
    menu.exec(globalPos);
}

void CategorySidebarWidget::onShowAll()
{
    setCurrentCategory("all");
    emit showAllRequested();
}

void CategorySidebarWidget::onShowFavorites()
{
    setCurrentCategory("favorites");
    emit showFavoritesRequested();
}

void CategorySidebarWidget::onShowRecent()
{
    setCurrentCategory("recent");
    emit showRecentRequested();
}

void CategorySidebarWidget::onClearCategory()
{
    // Clear the current category (placeholder implementation)
    QString currentCategory = m_currentCategory;
    if (!currentCategory.isEmpty()) {
        emit clearCategoryRequested(currentCategory);
    }
}

void CategorySidebarWidget::onRefreshCategories()
{
    refreshCategories();
}

void CategorySidebarWidget::onCategoryShortcut(int index)
{
    if (index >= 0 && index < m_categoryInfos.size()) {
        const QString& category = m_categoryInfos[index].name;
        onCategoryClicked(category);
    }
}

void CategorySidebarWidget::keyPressEvent(QKeyEvent* event)
{
    // Handle keyboard navigation within the sidebar
    QWidget::keyPressEvent(event);
}
