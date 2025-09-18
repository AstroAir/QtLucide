/**
 * QtLucide Gallery Application - Modern Sidebar Widget Implementation
 */

#include "ModernSidebarWidget.h"
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QFontMetrics>
#include <QDebug>

// CategoryItemWidget Implementation
CategoryItemWidget::CategoryItemWidget(const QString& categoryName, int itemCount, QWidget* parent)
    : QFrame(parent)
    , m_categoryName(categoryName)
    , m_itemCount(itemCount)
    , m_currentState(Normal)
    , m_expanded(false)
    , m_isPressed(false)
    , m_animationsEnabled(true)
    , m_showItemCount(true)
    , m_hoverOpacity(0.0)
    , m_selectionOpacity(0.0)
    , m_expandRotation(0.0)
{
    setupUI();

    // Create theme-aware widget
    m_themeWidget = new ThemeAwareWidget(this);

    // Apply initial theme
    applyTheme();
}

CategoryItemWidget::~CategoryItemWidget()
{
    stopAllAnimations();
}

void CategoryItemWidget::setCategoryName(const QString& name)
{
    if (m_categoryName == name) return;

    m_categoryName = name;
    update();
}

void CategoryItemWidget::setItemCount(int count)
{
    if (m_itemCount == count) return;

    m_itemCount = count;
    update();
}

void CategoryItemWidget::setIcon(const QIcon& icon)
{
    m_icon = icon;
    update();
}

void CategoryItemWidget::setState(ItemState state, bool animated)
{
    if (m_currentState == state) return;

    ItemState oldState = m_currentState;
    m_currentState = state;

    if (animated && m_animationsEnabled) {
        // Animate state transition
        switch (state) {
            case Normal:
                startAnimation("hoverOpacity", m_hoverOpacity, 0.0);
                startAnimation("selectionOpacity", m_selectionOpacity, 0.0);
                break;
            case Hovered:
                startAnimation("hoverOpacity", m_hoverOpacity, 0.3);
                break;
            case Selected:
                startAnimation("selectionOpacity", m_selectionOpacity, 1.0);
                startAnimation("hoverOpacity", m_hoverOpacity, 0.0);
                break;
            case Expanded:
                // Expanded state is handled separately
                break;
        }
    } else {
        // Apply state immediately
        switch (state) {
            case Normal:
                m_hoverOpacity = 0.0;
                m_selectionOpacity = 0.0;
                break;
            case Hovered:
                m_hoverOpacity = 0.3;
                break;
            case Selected:
                m_selectionOpacity = 1.0;
                m_hoverOpacity = 0.0;
                break;
            case Expanded:
                break;
        }
        update();
    }
}

void CategoryItemWidget::setSelected(bool selected, bool animated)
{
    setState(selected ? Selected : Normal, animated);
}

void CategoryItemWidget::setExpanded(bool expanded, bool animated)
{
    if (m_expanded == expanded) return;

    m_expanded = expanded;

    if (animated && m_animationsEnabled) {
        qreal targetRotation = expanded ? 90.0 : 0.0;
        startAnimation("expandRotation", m_expandRotation, targetRotation);
    } else {
        m_expandRotation = expanded ? 90.0 : 0.0;
        update();
    }

    emit expandToggled(m_categoryName, expanded);
}

void CategoryItemWidget::setShowItemCount(bool show)
{
    if (m_showItemCount == show) return;

    m_showItemCount = show;
    update();
}

void CategoryItemWidget::applyTheme()
{
    if (!ThemeManager::instance()) return;

    // Apply theme styling
    update();
}

void CategoryItemWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // Paint background
    paintBackground(&painter, rect);

    // Paint selection
    if (m_selectionOpacity > 0.0) {
        paintSelection(&painter, rect);
    }

    // Paint hover effect
    if (m_hoverOpacity > 0.0) {
        paintHover(&painter, rect);
    }

    // Paint icon
    paintIcon(&painter, rect);

    // Paint text
    paintText(&painter, rect);

    // Paint item count
    if (m_showItemCount && m_itemCount > 0) {
        paintItemCount(&painter, rect);
    }

    // Paint expand indicator
    paintExpandIndicator(&painter, rect);
}

void CategoryItemWidget::paintBackground(QPainter* painter, const QRect& rect)
{
    // Create subtle background gradient
    QLinearGradient gradient = getBackgroundGradient();
    painter->setBrush(QBrush(gradient));
    painter->setPen(Qt::NoPen);

    // Draw rounded rectangle background
    painter->drawRoundedRect(rect.adjusted(2, 1, -2, -1), 6, 6);
}

void CategoryItemWidget::paintIcon(QPainter* painter, const QRect& rect)
{
    if (m_icon.isNull()) return;

    QRect iconRect = getIconRect();
    QPixmap pixmap = m_icon.pixmap(ICON_SIZE, ICON_SIZE);

    // Apply color tint based on state
    if (m_currentState == Selected) {
        // Tint icon with accent color
        QColor tintColor = THEME_COLOR(AccentColor);
        // TODO: Apply color tint to pixmap
    }

    painter->drawPixmap(iconRect, pixmap);
}

void CategoryItemWidget::paintText(QPainter* painter, const QRect& rect)
{
    QRect textRect = getTextRect();
    painter->setPen(getTextColor());

    QFont font = painter->font();
    font.setPointSize(10);
    if (m_currentState == Selected) {
        font.setBold(true);
    }
    painter->setFont(font);

    QFontMetrics fm(font);
    QString elidedText = fm.elidedText(m_categoryName, Qt::ElideRight, textRect.width());

    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, elidedText);
}

void CategoryItemWidget::paintItemCount(QPainter* painter, const QRect& rect)
{
    if (m_itemCount <= 0) return;

    QRect countRect = getCountRect();

    // Draw count badge background
    QColor badgeColor = THEME_COLOR(AccentColor);
    badgeColor.setAlpha(180);
    painter->setBrush(QBrush(badgeColor));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(countRect, COUNT_BADGE_SIZE / 2, COUNT_BADGE_SIZE / 2);

    // Draw count text
    painter->setPen(getCountColor());
    QFont font = painter->font();
    font.setPointSize(8);
    font.setBold(true);
    painter->setFont(font);

    QString countText = QString::number(m_itemCount);
    if (m_itemCount > 999) {
        countText = "999+";
    }

    painter->drawText(countRect, Qt::AlignCenter, countText);
}

void CategoryItemWidget::paintExpandIndicator(QPainter* painter, const QRect& rect)
{
    QRect expandRect = getExpandRect();

    painter->save();
    painter->translate(expandRect.center());
    painter->rotate(m_expandRotation);

    // Draw expand arrow
    QPen pen(getTextColor(), 2);
    pen.setCapStyle(Qt::RoundCap);
    painter->setPen(pen);

    int arrowSize = 6;
    painter->drawLine(-arrowSize/2, -arrowSize/2, arrowSize/2, 0);
    painter->drawLine(-arrowSize/2, arrowSize/2, arrowSize/2, 0);

    painter->restore();
}

void CategoryItemWidget::paintSelection(QPainter* painter, const QRect& rect)
{
    QColor selectionColor = THEME_COLOR(AccentColor);
    selectionColor.setAlphaF(m_selectionOpacity * 0.2);

    painter->setBrush(QBrush(selectionColor));
    painter->setPen(Qt::NoPen);

    painter->drawRoundedRect(rect.adjusted(2, 1, -2, -1), 6, 6);

    // Draw selection border
    QPen borderPen(THEME_COLOR(AccentColor), 2);
    borderPen.setCapStyle(Qt::RoundCap);
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);

    painter->drawRoundedRect(rect.adjusted(2, 1, -2, -1), 6, 6);
}

void CategoryItemWidget::paintHover(QPainter* painter, const QRect& rect)
{
    QColor hoverColor = THEME_COLOR(HoverBackground);
    hoverColor.setAlphaF(m_hoverOpacity);

    painter->setBrush(QBrush(hoverColor));
    painter->setPen(Qt::NoPen);

    painter->drawRoundedRect(rect.adjusted(2, 1, -2, -1), 6, 6);
}

void CategoryItemWidget::setupUI()
{
    setFixedHeight(DEFAULT_HEIGHT);
    setFrameStyle(QFrame::NoFrame);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
}

void CategoryItemWidget::startAnimation(const QString& property, const QVariant& startValue, const QVariant& endValue, int duration)
{
    // Stop existing animation for this property
    if (m_animations.contains(property)) {
        QPropertyAnimation* oldAnimation = m_animations.take(property);
        oldAnimation->stop();
        oldAnimation->deleteLater();
    }

    QPropertyAnimation* animation = new QPropertyAnimation(this);
    animation->setDuration(duration);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(animation, &QPropertyAnimation::finished, this, &CategoryItemWidget::onAnimationFinished);
    connect(animation, &QPropertyAnimation::valueChanged, this, [this](const QVariant& value) {
        // Update the appropriate property and trigger repaint
        update();
    });

    m_animations[property] = animation;
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void CategoryItemWidget::stopAllAnimations()
{
    for (auto animation : m_animations) {
        animation->stop();
        animation->deleteLater();
    }
    m_animations.clear();
}

QColor CategoryItemWidget::getBackgroundColor() const
{
    if (!ThemeManager::instance()) return QColor("#ffffff");

    return ThemeManager::instance()->getColor(ThemeManager::PanelBackground);
}

QColor CategoryItemWidget::getTextColor() const
{
    if (!ThemeManager::instance()) return QColor("#333333");

    if (m_currentState == Selected) {
        return ThemeManager::instance()->getColor(ThemeManager::AccentColor);
    }

    return ThemeManager::instance()->getColor(ThemeManager::PrimaryText);
}

QColor CategoryItemWidget::getCountColor() const
{
    if (!ThemeManager::instance()) return QColor("#ffffff");

    return QColor("#ffffff"); // Always white on colored badge
}

QLinearGradient CategoryItemWidget::getBackgroundGradient() const
{
    QLinearGradient gradient(0, 0, 0, height());
    QColor baseColor = getBackgroundColor();

    gradient.setColorAt(0, baseColor.lighter(102));
    gradient.setColorAt(1, baseColor.darker(102));

    return gradient;
}

QRect CategoryItemWidget::getIconRect() const
{
    int x = PADDING;
    int y = (height() - ICON_SIZE) / 2;
    return QRect(x, y, ICON_SIZE, ICON_SIZE);
}

QRect CategoryItemWidget::getTextRect() const
{
    int x = PADDING + ICON_SIZE + 8;
    int y = 0;
    int w = width() - x - (m_showItemCount ? COUNT_BADGE_SIZE + 8 : 0) - EXPAND_INDICATOR_SIZE - 8;
    int h = height();
    return QRect(x, y, w, h);
}

QRect CategoryItemWidget::getCountRect() const
{
    if (!m_showItemCount || m_itemCount <= 0) return QRect();

    int x = width() - PADDING - COUNT_BADGE_SIZE - EXPAND_INDICATOR_SIZE - 8;
    int y = (height() - COUNT_BADGE_SIZE) / 2;
    return QRect(x, y, COUNT_BADGE_SIZE, COUNT_BADGE_SIZE);
}

QRect CategoryItemWidget::getExpandRect() const
{
    int x = width() - PADDING - EXPAND_INDICATOR_SIZE;
    int y = (height() - EXPAND_INDICATOR_SIZE) / 2;
    return QRect(x, y, EXPAND_INDICATOR_SIZE, EXPAND_INDICATOR_SIZE);
}

void CategoryItemWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
    }
    QFrame::mousePressEvent(event);
}

void CategoryItemWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;
        emit clicked(m_categoryName);
    }
    QFrame::mouseReleaseEvent(event);
}

void CategoryItemWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(m_categoryName);
    }
    QFrame::mouseDoubleClickEvent(event);
}

void CategoryItemWidget::enterEvent(QEnterEvent* event)
{
    if (m_currentState != Selected) {
        setState(Hovered, true);
    }
    emit hoverEntered(m_categoryName);
    QFrame::enterEvent(event);
}

void CategoryItemWidget::leaveEvent(QEvent* event)
{
    if (m_currentState == Hovered) {
        setState(Normal, true);
    }
    emit hoverLeft(m_categoryName);
    QFrame::leaveEvent(event);
}

void CategoryItemWidget::contextMenuEvent(QContextMenuEvent* event)
{
    emit rightClicked(m_categoryName, event->globalPos());
    QFrame::contextMenuEvent(event);
}

void CategoryItemWidget::onAnimationFinished()
{
    QPropertyAnimation* animation = qobject_cast<QPropertyAnimation*>(sender());
    if (!animation) return;

    // Remove from active animations
    for (auto it = m_animations.begin(); it != m_animations.end(); ++it) {
        if (it.value() == animation) {
            m_animations.erase(it);
            break;
        }
    }
}

void CategoryItemWidget::onThemeChanged()
{
    applyTheme();
}

// CategoryTreeWidget Implementation
CategoryTreeWidget::CategoryTreeWidget(QWidget* parent)
    : QFrame(parent)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_contentLayout(nullptr)
    , m_animationsEnabled(true)
    , m_showItemCounts(true)
    , m_layoutUpdateTimer(new QTimer(this))
    , m_layoutUpdatePending(false)
{
    setupUI();

    // Set up layout update timer
    m_layoutUpdateTimer->setSingleShot(true);
    m_layoutUpdateTimer->setInterval(LAYOUT_UPDATE_DELAY);
    connect(m_layoutUpdateTimer, &QTimer::timeout, this, &CategoryTreeWidget::onLayoutUpdateTimer);

    // Create theme-aware widget
    m_themeWidget = new ThemeAwareWidget(this);

    // Apply initial theme
    applyTheme();
}

CategoryTreeWidget::~CategoryTreeWidget()
{
    // Clean up category items
    for (auto item : m_categoryItems) {
        item->deleteLater();
    }
}

void CategoryTreeWidget::setCategories(const QHash<QString, CategoryData>& categories)
{
    // Simple check - just update if different size or keys
    bool needsUpdate = (m_categories.size() != categories.size());
    if (!needsUpdate) {
        for (auto it = categories.begin(); it != categories.end(); ++it) {
            if (!m_categories.contains(it.key())) {
                needsUpdate = true;
                break;
            }
        }
    }

    if (!needsUpdate) return;

    m_categories = categories;
    createCategoryItems();
    scheduleLayoutUpdate();
}

void CategoryTreeWidget::addCategory(const QString& name, const CategoryData& data)
{
    if (m_categories.contains(name)) return;

    m_categories[name] = data;

    // Create category item
    CategoryItemWidget* item = new CategoryItemWidget(data.displayName.isEmpty() ? name : data.displayName, data.itemCount, m_contentWidget);
    item->setIcon(data.icon);
    item->setAnimationsEnabled(m_animationsEnabled);
    item->setShowItemCount(m_showItemCounts);
    item->setExpanded(data.expanded);

    // Connect signals
    connect(item, &CategoryItemWidget::clicked, this, &CategoryTreeWidget::onCategoryClicked);
    connect(item, &CategoryItemWidget::expandToggled, this, &CategoryTreeWidget::onCategoryExpanded);
    connect(item, &CategoryItemWidget::hoverEntered, this, &CategoryTreeWidget::onCategoryHovered);

    m_categoryItems[name] = item;
    m_contentLayout->addWidget(item);

    scheduleLayoutUpdate();
}

void CategoryTreeWidget::removeCategory(const QString& name)
{
    if (!m_categories.contains(name)) return;

    m_categories.remove(name);

    if (m_categoryItems.contains(name)) {
        CategoryItemWidget* item = m_categoryItems.take(name);
        item->deleteLater();
        scheduleLayoutUpdate();
    }

    // Clear selection if this was the selected category
    if (m_selectedCategory == name) {
        clearSelection();
    }
}

void CategoryTreeWidget::updateCategoryCount(const QString& name, int count)
{
    if (!m_categories.contains(name)) return;

    m_categories[name].itemCount = count;

    CategoryItemWidget* item = m_categoryItems.value(name);
    if (item) {
        item->setItemCount(count);
    }

    emit categoryCountChanged(name, count);
}

void CategoryTreeWidget::setSelectedCategory(const QString& categoryName)
{
    if (m_selectedCategory == categoryName) return;

    // Clear previous selection
    if (!m_selectedCategory.isEmpty()) {
        CategoryItemWidget* prevItem = m_categoryItems.value(m_selectedCategory);
        if (prevItem) {
            prevItem->setSelected(false, m_animationsEnabled);
        }
    }

    m_selectedCategory = categoryName;

    // Set new selection
    if (!categoryName.isEmpty()) {
        CategoryItemWidget* newItem = m_categoryItems.value(categoryName);
        if (newItem) {
            newItem->setSelected(true, m_animationsEnabled);
        }
    }

    emit categorySelected(categoryName);
}

void CategoryTreeWidget::clearSelection()
{
    setSelectedCategory(QString());
}

void CategoryTreeWidget::setExpandedCategories(const QStringList& categories)
{
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        bool shouldExpand = categories.contains(it.key());
        if (it.value().expanded != shouldExpand) {
            it.value().expanded = shouldExpand;

            CategoryItemWidget* item = m_categoryItems.value(it.key());
            if (item) {
                item->setExpanded(shouldExpand, m_animationsEnabled);
            }
        }
    }
}

QStringList CategoryTreeWidget::expandedCategories() const
{
    QStringList expanded;
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        if (it.value().expanded) {
            expanded.append(it.key());
        }
    }
    return expanded;
}

void CategoryTreeWidget::expandCategory(const QString& categoryName, bool expanded)
{
    if (!m_categories.contains(categoryName)) return;

    if (m_categories[categoryName].expanded == expanded) return;

    m_categories[categoryName].expanded = expanded;

    CategoryItemWidget* item = m_categoryItems.value(categoryName);
    if (item) {
        item->setExpanded(expanded, m_animationsEnabled);
    }

    emit categoryExpanded(categoryName, expanded);
}

void CategoryTreeWidget::collapseAll()
{
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        expandCategory(it.key(), false);
    }
}

void CategoryTreeWidget::expandAll()
{
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        expandCategory(it.key(), true);
    }
}

void CategoryTreeWidget::setAnimationsEnabled(bool enabled)
{
    if (m_animationsEnabled == enabled) return;

    m_animationsEnabled = enabled;

    // Update all category items
    for (auto item : m_categoryItems) {
        item->setAnimationsEnabled(enabled);
    }
}

void CategoryTreeWidget::setShowItemCounts(bool show)
{
    if (m_showItemCounts == show) return;

    m_showItemCounts = show;

    // Update all category items
    for (auto item : m_categoryItems) {
        item->setShowItemCount(show);
    }
}

void CategoryTreeWidget::setFilter(const QString& filter)
{
    if (m_filter == filter) return;

    m_filter = filter;
    applyCategoryFilter();
    emit filterChanged(filter);
}

void CategoryTreeWidget::applyTheme()
{
    if (!ThemeManager::instance()) return;

    // Apply theme to the tree widget
    QString treeStyle = QString(
        "CategoryTreeWidget { "
        "    background-color: %1; "
        "    border: none; "
        "}"
    ).arg(
        THEME_COLOR(PanelBackground).name()
    );

    setStyleSheet(treeStyle);

    // Update all category items
    for (auto item : m_categoryItems) {
        item->applyTheme();
    }
}

void CategoryTreeWidget::refreshCategories()
{
    createCategoryItems();
    updateLayout();
}

void CategoryTreeWidget::updateLayout()
{
    m_layoutUpdatePending = false;

    // Update category visibility based on filter
    updateCategoryVisibility();

    // Force layout update
    if (m_contentWidget) {
        m_contentWidget->updateGeometry();
    }
    if (m_scrollArea) {
        m_scrollArea->updateGeometry();
    }
}

void CategoryTreeWidget::setupUI()
{
    setFrameStyle(QFrame::NoFrame);

    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // Create scroll area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);

    // Create content widget
    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(4, 4, 4, 4);
    m_contentLayout->setSpacing(2);
    m_contentLayout->addStretch(); // Add stretch at the end

    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);
}

void CategoryTreeWidget::createCategoryItems()
{
    // Clear existing items
    for (auto item : m_categoryItems) {
        item->deleteLater();
    }
    m_categoryItems.clear();

    // Create new items
    for (auto it = m_categories.begin(); it != m_categories.end(); ++it) {
        const QString& name = it.key();
        const CategoryData& data = it.value();

        CategoryItemWidget* item = new CategoryItemWidget(data.displayName.isEmpty() ? name : data.displayName, data.itemCount, m_contentWidget);
        item->setIcon(data.icon);
        item->setAnimationsEnabled(m_animationsEnabled);
        item->setShowItemCount(m_showItemCounts);
        item->setExpanded(data.expanded);
        item->setVisible(data.visible);

        // Connect signals
        connect(item, &CategoryItemWidget::clicked, this, &CategoryTreeWidget::onCategoryClicked);
        connect(item, &CategoryItemWidget::expandToggled, this, &CategoryTreeWidget::onCategoryExpanded);
        connect(item, &CategoryItemWidget::hoverEntered, this, &CategoryTreeWidget::onCategoryHovered);

        m_categoryItems[name] = item;

        // Insert before the stretch
        int insertIndex = m_contentLayout->count() - 1;
        m_contentLayout->insertWidget(insertIndex, item);
    }
}

void CategoryTreeWidget::updateCategoryVisibility()
{
    for (auto it = m_categoryItems.begin(); it != m_categoryItems.end(); ++it) {
        const QString& name = it.key();
        CategoryItemWidget* item = it.value();

        bool visible = m_categories[name].visible;

        // Apply filter if set
        if (!m_filter.isEmpty() && visible) {
            visible = name.contains(m_filter, Qt::CaseInsensitive) ||
                     m_categories[name].displayName.contains(m_filter, Qt::CaseInsensitive);
        }

        item->setVisible(visible);
    }
}

void CategoryTreeWidget::applyCategoryFilter()
{
    updateCategoryVisibility();
    scheduleLayoutUpdate();
}

void CategoryTreeWidget::scheduleLayoutUpdate()
{
    if (!m_layoutUpdatePending) {
        m_layoutUpdatePending = true;
        m_layoutUpdateTimer->start();
    }
}

void CategoryTreeWidget::onCategoryClicked(const QString& categoryName)
{
    setSelectedCategory(categoryName);
}

void CategoryTreeWidget::onCategoryExpanded(const QString& categoryName, bool expanded)
{
    expandCategory(categoryName, expanded);
}

void CategoryTreeWidget::onCategoryHovered(const QString& categoryName)
{
    // Could be used for tooltips or other hover effects
    Q_UNUSED(categoryName)
}

void CategoryTreeWidget::onLayoutUpdateTimer()
{
    updateLayout();
}

void CategoryTreeWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw subtle background
    QColor backgroundColor = THEME_COLOR(PanelBackground);
    painter.fillRect(rect(), backgroundColor);

    QFrame::paintEvent(event);
}

void CategoryTreeWidget::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    scheduleLayoutUpdate();
}

void CategoryTreeWidget::wheelEvent(QWheelEvent* event)
{
    // Let the scroll area handle wheel events
    QFrame::wheelEvent(event);
}

void CategoryTreeWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Up:
            // TODO: Navigate to previous category
            break;
        case Qt::Key_Down:
            // TODO: Navigate to next category
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // TODO: Select current category
            break;
        case Qt::Key_Space:
            // TODO: Toggle expansion of current category
            break;
    }

    QFrame::keyPressEvent(event);
}
