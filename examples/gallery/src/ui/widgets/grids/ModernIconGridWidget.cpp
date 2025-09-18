/**
 * QtLucide Gallery Application - Modern Icon Grid Widget Implementation
 */

#include "ModernIconGridWidget.h"
#include <QScrollArea>
#include <QApplication>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
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


// ModernIconItem Implementation
ModernIconItem::ModernIconItem(const QString& iconName, QWidget* parent)
    : QFrame(parent)
    , m_iconName(iconName)
    , m_iconSize(DEFAULT_ICON_SIZE)
    , m_showIconName(true)
    , m_currentState(Normal)
    , m_isFavorite(false)
    , m_isPressed(false)
    , m_hoverEffectsEnabled(true)
    , m_animationsEnabled(true)
    , m_glowEnabled(true)
    , m_hoverOpacity(0.0)
    , m_selectionOpacity(0.0)
    , m_glowOpacity(0.0)
    , m_scaleValue(1.0)
    , m_pixmapCacheValid(false)
{
    setFixedSize(m_iconSize + 2 * PADDING, m_iconSize + TEXT_HEIGHT + 2 * PADDING);
    setFrameStyle(QFrame::NoFrame);
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);

    // Create theme-aware widget
    m_themeWidget = new ThemeAwareWidget(this, this);
    // Note: ThemeAwareWidget doesn't have public signals, we'll handle theme changes differently

    // Apply initial theme
    applyTheme();
}

ModernIconItem::~ModernIconItem()
{
    stopAllAnimations();
}

void ModernIconItem::setIconName(const QString& iconName)
{
    if (m_iconName == iconName) return;

    m_iconName = iconName;
    m_pixmapCacheValid = false;
    update();
}

void ModernIconItem::setIconSize(int size)
{
    if (m_iconSize == size) return;

    m_iconSize = qBound(16, size, 512);
    setFixedSize(m_iconSize + 2 * PADDING, m_iconSize + (m_showIconName ? TEXT_HEIGHT : 0) + 2 * PADDING);
    m_pixmapCacheValid = false;
    update();
}

void ModernIconItem::setShowIconName(bool show)
{
    if (m_showIconName == show) return;

    m_showIconName = show;
    setFixedSize(m_iconSize + 2 * PADDING, m_iconSize + (show ? TEXT_HEIGHT : 0) + 2 * PADDING);
    update();
}

void ModernIconItem::setState(State state, bool animated)
{
    if (m_currentState == state) return;

    State oldState = m_currentState;
    m_currentState = state;

    if (animated && m_animationsEnabled) {
        // Animate state transition
        switch (state) {
            case Normal:
                startAnimation(ScaleAnimation, m_scaleValue, 1.0);
                startAnimation(GlowAnimation, m_glowOpacity, 0.0);
                startAnimation(BorderAnimation, m_selectionOpacity, 0.0);
                break;
            case Hovered:
                startAnimation(ScaleAnimation, m_scaleValue, HOVER_SCALE);
                startAnimation(GlowAnimation, m_glowOpacity, 0.3);
                break;
            case Selected:
                startAnimation(ScaleAnimation, m_scaleValue, SELECTION_SCALE);
                startAnimation(BorderAnimation, m_selectionOpacity, 1.0);
                startAnimation(GlowAnimation, m_glowOpacity, 0.5);
                break;
            case Pressed:
                startAnimation(ScaleAnimation, m_scaleValue, 0.95);
                break;
            case Favorite:
                // Favorite state doesn't change scale/glow, just visual indicator
                break;
        }
    } else {
        // Apply state immediately
        switch (state) {
            case Normal:
                m_scaleValue = 1.0;
                m_glowOpacity = 0.0;
                m_selectionOpacity = 0.0;
                break;
            case Hovered:
                m_scaleValue = HOVER_SCALE;
                m_glowOpacity = 0.3;
                break;
            case Selected:
                m_scaleValue = SELECTION_SCALE;
                m_selectionOpacity = 1.0;
                m_glowOpacity = 0.5;
                break;
            case Pressed:
                m_scaleValue = 0.95;
                break;
            case Favorite:
                // Favorite state doesn't change scale/glow, just visual indicator
                break;
        }
        update();
    }
}

void ModernIconItem::setSelected(bool selected, bool animated)
{
    setState(selected ? Selected : Normal, animated);
}

void ModernIconItem::setFavorite(bool favorite, bool animated)
{
    if (m_isFavorite == favorite) return;

    m_isFavorite = favorite;

    if (animated && m_animationsEnabled) {
        // Animate favorite indicator
        QPropertyAnimation* animation = new QPropertyAnimation(this, "geometry", this);
        animation->setDuration(150);
        animation->setEasingCurve(QEasingCurve::OutBack);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }

    update();
    emit favoriteToggled(m_iconName, favorite);
}

void ModernIconItem::applyTheme()
{
    if (!ThemeManager::instance()) return;

    // Update colors based on current theme
    m_pixmapCacheValid = false;
    update();
}

void ModernIconItem::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect rect = this->rect();

    // Apply scale transformation
    if (m_scaleValue != 1.0) {
        painter.save();
        QPointF center = rect.center();
        painter.translate(center);
        painter.scale(m_scaleValue, m_scaleValue);
        painter.translate(-center);
    }

    // Paint background
    paintBackground(&painter, rect);

    // Paint glow effect
    if (m_glowEnabled && m_glowOpacity > 0.0) {
        paintGlow(&painter, rect);
    }

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

    // Paint icon name
    if (m_showIconName) {
        paintIconName(&painter, rect);
    }

    // Paint favorite indicator
    if (m_isFavorite) {
        paintFavoriteIndicator(&painter, rect);
    }

    if (m_scaleValue != 1.0) {
        painter.restore();
    }
}

void ModernIconItem::paintBackground(QPainter* painter, const QRect& rect)
{
    // Create subtle background gradient
    QLinearGradient gradient = getBackgroundGradient();
    painter->setBrush(QBrush(gradient));
    painter->setPen(Qt::NoPen);

    // Draw rounded rectangle background
    painter->drawRoundedRect(rect.adjusted(2, 2, -2, -2), 8, 8);
}

void ModernIconItem::paintIcon(QPainter* painter, const QRect& rect)
{
    if (m_iconName.isEmpty()) return;

    // Get or create cached pixmap
    if (!m_pixmapCacheValid) {
        // TODO: Generate icon pixmap using QtLucide
        // For now, create a placeholder
        m_iconPixmap = QPixmap(m_iconSize, m_iconSize);
        m_iconPixmap.fill(getTextColor());
        m_pixmapCacheValid = true;
    }

    // Draw icon in center
    QRect iconRect = getIconRect();
    painter->drawPixmap(iconRect, m_iconPixmap);
}

void ModernIconItem::paintIconName(QPainter* painter, const QRect& rect)
{
    if (m_iconName.isEmpty()) return;

    QRect textRect = getTextRect();
    painter->setPen(getTextColor());

    QFont font = painter->font();
    font.setPointSize(9);
    painter->setFont(font);

    QFontMetrics fm(font);
    QString elidedText = fm.elidedText(m_iconName, Qt::ElideMiddle, textRect.width());

    painter->drawText(textRect, Qt::AlignCenter, elidedText);
}

void ModernIconItem::paintSelection(QPainter* painter, const QRect& rect)
{
    QColor selectionColor = THEME_COLOR(AccentColor);
    selectionColor.setAlphaF(m_selectionOpacity);

    QPen pen(selectionColor, 2);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    painter->drawRoundedRect(rect.adjusted(1, 1, -1, -1), 8, 8);
}

void ModernIconItem::paintHover(QPainter* painter, const QRect& rect)
{
    QColor hoverColor = THEME_COLOR(HoverBackground);
    hoverColor.setAlphaF(m_hoverOpacity);

    painter->setBrush(QBrush(hoverColor));
    painter->setPen(Qt::NoPen);

    painter->drawRoundedRect(rect.adjusted(2, 2, -2, -2), 8, 8);
}

void ModernIconItem::paintGlow(QPainter* painter, const QRect& rect)
{
    if (m_glowOpacity <= 0.0) return;

    QRadialGradient glowGradient = getGlowGradient();
    glowGradient.setColorAt(0, QColor(THEME_COLOR(AccentColor)).lighter(150));
    glowGradient.setColorAt(1, Qt::transparent);

    painter->setBrush(QBrush(glowGradient));
    painter->setPen(Qt::NoPen);

    QRect glowRect = rect.adjusted(-GLOW_RADIUS, -GLOW_RADIUS, GLOW_RADIUS, GLOW_RADIUS);
    painter->drawEllipse(glowRect);
}

void ModernIconItem::paintFavoriteIndicator(QPainter* painter, const QRect& rect)
{
    QRect favoriteRect = getFavoriteRect();

    // Draw favorite star
    painter->setPen(QPen(THEME_COLOR(WarningColor), 1));
    painter->setBrush(QBrush(THEME_COLOR(WarningColor)));

    // Simple star shape (could be improved with actual star path)
    painter->drawEllipse(favoriteRect);
}

void ModernIconItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPressed = true;
        setState(Pressed, true);
    }
    QFrame::mousePressEvent(event);
}

void ModernIconItem::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPressed) {
        m_isPressed = false;
        setState(m_currentState == Selected ? Selected : Hovered, true);
        emit clicked(m_iconName);
    }
    QFrame::mouseReleaseEvent(event);
}

void ModernIconItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked(m_iconName);
    }
    QFrame::mouseDoubleClickEvent(event);
}

void ModernIconItem::enterEvent(QEnterEvent* event)
{
    if (m_hoverEffectsEnabled && m_currentState != Selected) {
        setState(Hovered, true);
    }
    emit hoverEntered(m_iconName);
    QFrame::enterEvent(event);
}

void ModernIconItem::leaveEvent(QEvent* event)
{
    if (m_hoverEffectsEnabled && m_currentState == Hovered) {
        setState(Normal, true);
    }
    emit hoverLeft(m_iconName);
    QFrame::leaveEvent(event);
}

void ModernIconItem::contextMenuEvent(QContextMenuEvent* event)
{
    emit rightClicked(m_iconName, event->globalPos());
    QFrame::contextMenuEvent(event);
}

void ModernIconItem::startAnimation(AnimationType type, const QVariant& startValue, const QVariant& endValue, int duration)
{
    // Stop existing animation of this type
    stopAnimation(type);

    QPropertyAnimation* animation = new QPropertyAnimation(this);
    animation->setDuration(duration);
    animation->setStartValue(startValue);
    animation->setEndValue(endValue);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    // Set up property based on animation type
    switch (type) {
        case ScaleAnimation:
            // Custom property handling needed
            break;
        case GlowAnimation:
            // Custom property handling needed
            break;
        case BorderAnimation:
            // Custom property handling needed
            break;
        default:
            break;
    }

    connect(animation, &QPropertyAnimation::finished, this, &ModernIconItem::onAnimationFinished);
    connect(animation, &QPropertyAnimation::valueChanged, this, [this](const QVariant& value) {
        // Update the appropriate property and trigger repaint
        update();
    });

    m_animations[type] = animation;
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void ModernIconItem::stopAnimation(AnimationType type)
{
    if (m_animations.contains(type)) {
        QPropertyAnimation* animation = m_animations.take(type);
        animation->stop();
        animation->deleteLater();
    }
}

void ModernIconItem::stopAllAnimations()
{
    for (auto animation : m_animations) {
        animation->stop();
        animation->deleteLater();
    }
    m_animations.clear();
}

QColor ModernIconItem::getBackgroundColor() const
{
    if (!ThemeManager::instance()) return QColor("#ffffff");

    return ThemeManager::instance()->getColor(ThemeManager::PanelBackground);
}

QColor ModernIconItem::getBorderColor() const
{
    if (!ThemeManager::instance()) return QColor("#e0e0e0");

    return ThemeManager::instance()->getColor(ThemeManager::BorderColor);
}

QColor ModernIconItem::getTextColor() const
{
    if (!ThemeManager::instance()) return QColor("#333333");

    return ThemeManager::instance()->getColor(ThemeManager::PrimaryText);
}

QLinearGradient ModernIconItem::getBackgroundGradient() const
{
    QLinearGradient gradient(0, 0, 0, height());
    QColor baseColor = getBackgroundColor();

    gradient.setColorAt(0, baseColor.lighter(105));
    gradient.setColorAt(1, baseColor.darker(105));

    return gradient;
}

QRadialGradient ModernIconItem::getGlowGradient() const
{
    QRadialGradient gradient(rect().center(), GLOW_RADIUS);
    return gradient;
}

QRect ModernIconItem::getIconRect() const
{
    int x = (width() - m_iconSize) / 2;
    int y = PADDING;
    return QRect(x, y, m_iconSize, m_iconSize);
}

QRect ModernIconItem::getTextRect() const
{
    if (!m_showIconName) return QRect();

    int y = PADDING + m_iconSize + 4;
    return QRect(PADDING, y, width() - 2 * PADDING, TEXT_HEIGHT);
}

QRect ModernIconItem::getFavoriteRect() const
{
    int x = width() - FAVORITE_SIZE - 4;
    int y = 4;
    return QRect(x, y, FAVORITE_SIZE, FAVORITE_SIZE);
}

void ModernIconItem::onAnimationFinished()
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

void ModernIconItem::onThemeChanged()
{
    applyTheme();
}

// ModernIconGridWidget Implementation
ModernIconGridWidget::ModernIconGridWidget(QWidget* parent)
    : QWidget(parent)
    , m_lucide(nullptr)
    , m_themeManager(nullptr)
    , m_layoutManager(nullptr)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_gridLayout(nullptr)
    , m_viewMode(GridView)
    , m_selectionMode(SingleSelection)
    , m_iconSize(DEFAULT_ICON_SIZE)
    , m_showIconNames(true)
    , m_itemSpacing(DEFAULT_SPACING)
    , m_itemMargins(8, 8, 8, 8)
    , m_columns(4)
    , m_animationsEnabled(true)
    , m_hoverEffectsEnabled(true)
    , m_glowEffectsEnabled(true)
    , m_virtualScrollingEnabled(false)
    , m_layoutUpdateTimer(new QTimer(this))
    , m_layoutUpdatePending(false)
{
    setupUI();
    setupLayout();
    setupScrollArea();

    // Set up layout update timer
    m_layoutUpdateTimer->setSingleShot(true);
    m_layoutUpdateTimer->setInterval(LAYOUT_UPDATE_DELAY);
    connect(m_layoutUpdateTimer, &QTimer::timeout, this, &ModernIconGridWidget::updateLayout);

    // Apply initial theme
    if (ThemeManager::instance()) {
        setThemeManager(ThemeManager::instance());
    }
}

ModernIconGridWidget::~ModernIconGridWidget()
{
    clearIcons();
}

void ModernIconGridWidget::setIconNames(const QStringList& iconNames)
{
    if (m_iconNames == iconNames) return;

    clearIcons();
    m_iconNames = iconNames;
    m_filteredIconNames = iconNames;

    createIconItems();
    updateLayout();
}

void ModernIconGridWidget::addIcon(const QString& iconName)
{
    if (m_iconNames.contains(iconName)) return;

    m_iconNames.append(iconName);
    if (m_filter.isEmpty() || iconName.contains(m_filter, Qt::CaseInsensitive)) {
        m_filteredIconNames.append(iconName);

        // Create icon item
        ModernIconItem* item = new ModernIconItem(iconName, m_contentWidget);
        item->setIconSize(m_iconSize);
        item->setShowIconName(m_showIconNames);
        item->setAnimationsEnabled(m_animationsEnabled);
        item->setHoverEffectsEnabled(m_hoverEffectsEnabled);
        item->setGlowEnabled(m_glowEffectsEnabled);

        // Connect signals
        connect(item, &ModernIconItem::clicked, this, &ModernIconGridWidget::onIconClicked);
        connect(item, &ModernIconItem::doubleClicked, this, &ModernIconGridWidget::onIconDoubleClicked);
        connect(item, &ModernIconItem::rightClicked, this, &ModernIconGridWidget::onIconRightClicked);
        connect(item, &ModernIconItem::hoverEntered, this, &ModernIconGridWidget::onIconHovered);
        connect(item, &ModernIconItem::favoriteToggled, this, &ModernIconGridWidget::onFavoriteToggled);

        m_iconItems[iconName] = item;

        // Schedule layout update
        if (!m_layoutUpdatePending) {
            m_layoutUpdatePending = true;
            m_layoutUpdateTimer->start();
        }
    }
}

void ModernIconGridWidget::removeIcon(const QString& iconName)
{
    if (!m_iconNames.contains(iconName)) return;

    m_iconNames.removeAll(iconName);
    m_filteredIconNames.removeAll(iconName);
    m_selectedIcons.remove(iconName);

    if (m_iconItems.contains(iconName)) {
        ModernIconItem* item = m_iconItems.take(iconName);
        item->deleteLater();
        if (!m_layoutUpdatePending) {
            m_layoutUpdatePending = true;
            m_layoutUpdateTimer->start();
        }
    }
}

void ModernIconGridWidget::clearIcons()
{
    m_iconNames.clear();
    m_filteredIconNames.clear();
    m_selectedIcons.clear();

    for (auto item : m_iconItems) {
        item->deleteLater();
    }
    m_iconItems.clear();

    if (!m_layoutUpdatePending) {
        m_layoutUpdatePending = true;
        m_layoutUpdateTimer->start();
    }
}

void ModernIconGridWidget::setViewMode(ViewMode mode)
{
    if (m_viewMode == mode) return;

    ViewMode oldMode = m_viewMode;
    m_viewMode = mode;

    applyViewMode();
    updateLayout();

    emit viewModeChanged(mode);
}

void ModernIconGridWidget::setIconSize(int size)
{
    size = qBound(MIN_ICON_SIZE, size, MAX_ICON_SIZE);
    if (m_iconSize == size) return;

    m_iconSize = size;

    // Update all icon items
    for (auto item : m_iconItems) {
        item->setIconSize(size);
    }

    updateLayout();
    emit iconSizeChanged(size);
}

void ModernIconGridWidget::setShowIconNames(bool show)
{
    if (m_showIconNames == show) return;

    m_showIconNames = show;

    // Update all icon items
    for (auto item : m_iconItems) {
        item->setShowIconName(show);
    }

    updateLayout();
}

void ModernIconGridWidget::setSelectionMode(SelectionMode mode)
{
    if (m_selectionMode == mode) return;

    m_selectionMode = mode;

    // Clear selection if switching to NoSelection
    if (mode == NoSelection) {
        clearSelection();
    }
}

void ModernIconGridWidget::selectIcon(const QString& iconName, bool selected)
{
    if (m_selectionMode == NoSelection) return;

    ModernIconItem* item = m_iconItems.value(iconName);
    if (!item) return;

    bool wasSelected = m_selectedIcons.contains(iconName);

    if (selected) {
        // Handle selection based on mode
        if (m_selectionMode == SingleSelection) {
            // Clear other selections first
            clearSelection();
        }

        m_selectedIcons.insert(iconName);
        item->setSelected(true, m_animationsEnabled);
    } else {
        m_selectedIcons.remove(iconName);
        item->setSelected(false, m_animationsEnabled);
    }

    if (wasSelected != selected) {
        emit selectionChanged(selectedIcons());
    }
}

void ModernIconGridWidget::selectAll()
{
    if (m_selectionMode == NoSelection || m_selectionMode == SingleSelection) return;

    for (const QString& iconName : m_filteredIconNames) {
        m_selectedIcons.insert(iconName);
        ModernIconItem* item = m_iconItems.value(iconName);
        if (item) {
            item->setSelected(true, false); // Don't animate for bulk selection
        }
    }

    emit selectionChanged(selectedIcons());
}

void ModernIconGridWidget::clearSelection()
{
    if (m_selectedIcons.isEmpty()) return;

    for (const QString& iconName : m_selectedIcons) {
        ModernIconItem* item = m_iconItems.value(iconName);
        if (item) {
            item->setSelected(false, m_animationsEnabled);
        }
    }

    m_selectedIcons.clear();
    emit selectionChanged(QStringList());
}

QStringList ModernIconGridWidget::selectedIcons() const
{
    return m_selectedIcons.values();
}

bool ModernIconGridWidget::isSelected(const QString& iconName) const
{
    return m_selectedIcons.contains(iconName);
}

void ModernIconGridWidget::setFilter(const QString& filter)
{
    if (m_filter == filter) return;

    m_filter = filter;

    // Update filtered icons
    m_filteredIconNames.clear();
    for (const QString& iconName : m_iconNames) {
        if (filter.isEmpty() || iconName.contains(filter, Qt::CaseInsensitive)) {
            m_filteredIconNames.append(iconName);
        }
    }

    // Update visibility of icon items
    for (auto it = m_iconItems.begin(); it != m_iconItems.end(); ++it) {
        bool visible = m_filteredIconNames.contains(it.key());
        it.value()->setVisible(visible);
    }

    updateLayout();
}

void ModernIconGridWidget::setAnimationsEnabled(bool enabled)
{
    if (m_animationsEnabled == enabled) return;

    m_animationsEnabled = enabled;

    // Update all icon items
    for (auto item : m_iconItems) {
        item->setAnimationsEnabled(enabled);
    }
}

void ModernIconGridWidget::setHoverEffectsEnabled(bool enabled)
{
    if (m_hoverEffectsEnabled == enabled) return;

    m_hoverEffectsEnabled = enabled;

    // Update all icon items
    for (auto item : m_iconItems) {
        item->setHoverEffectsEnabled(enabled);
    }
}

void ModernIconGridWidget::setGlowEffectsEnabled(bool enabled)
{
    if (m_glowEffectsEnabled == enabled) return;

    m_glowEffectsEnabled = enabled;

    // Update all icon items
    for (auto item : m_iconItems) {
        item->setGlowEnabled(enabled);
    }
}

void ModernIconGridWidget::setLucide(lucide::QtLucide* lucide)
{
    m_lucide = lucide;
}

void ModernIconGridWidget::setThemeManager(ThemeManager* themeManager)
{
    if (m_themeManager == themeManager) return;

    if (m_themeManager) {
        disconnect(m_themeManager, nullptr, this, nullptr);
    }

    m_themeManager = themeManager;

    if (m_themeManager) {
        connect(m_themeManager, &ThemeManager::themeChanged, this, &ModernIconGridWidget::onThemeChanged);
        connect(m_themeManager, &ThemeManager::colorsChanged, this, &ModernIconGridWidget::updateTheme);
    }

    updateTheme();
}

void ModernIconGridWidget::setResponsiveLayoutManager(ResponsiveLayoutManager* layoutManager)
{
    if (m_layoutManager == layoutManager) return;

    if (m_layoutManager) {
        disconnect(m_layoutManager, nullptr, this, nullptr);
    }

    m_layoutManager = layoutManager;

    if (m_layoutManager) {
        connect(m_layoutManager, &ResponsiveLayoutManager::layoutUpdated, this, &ModernIconGridWidget::onLayoutChanged);
        connect(m_layoutManager, &ResponsiveLayoutManager::screenSizeChanged, this, &ModernIconGridWidget::onLayoutChanged);
    }
}

void ModernIconGridWidget::refreshLayout()
{
    updateLayout();
}

void ModernIconGridWidget::updateTheme()
{
    applyTheme();

    // Update all icon items
    for (auto item : m_iconItems) {
        item->applyTheme();
    }
}

void ModernIconGridWidget::zoomIn()
{
    int newSize = qMin(m_iconSize + ZOOM_STEP, MAX_ICON_SIZE);
    setIconSize(newSize);
}

void ModernIconGridWidget::zoomOut()
{
    int newSize = qMax(m_iconSize - ZOOM_STEP, MIN_ICON_SIZE);
    setIconSize(newSize);
}

void ModernIconGridWidget::resetZoom()
{
    setIconSize(DEFAULT_ICON_SIZE);
}

void ModernIconGridWidget::onIconClicked(const QString& iconName)
{
    // Handle selection
    if (m_selectionMode != NoSelection) {
        bool isCurrentlySelected = isSelected(iconName);

        if (m_selectionMode == SingleSelection) {
            clearSelection();
            if (!isCurrentlySelected) {
                selectIcon(iconName, true);
            }
        } else {
            selectIcon(iconName, !isCurrentlySelected);
        }
    }

    emit iconClicked(iconName);
}

void ModernIconGridWidget::onIconDoubleClicked(const QString& iconName)
{
    emit iconDoubleClicked(iconName);
}

void ModernIconGridWidget::onIconRightClicked(const QString& iconName, const QPoint& globalPos)
{
    emit iconRightClicked(iconName, globalPos);
}

void ModernIconGridWidget::onIconHovered(const QString& iconName)
{
    emit iconHovered(iconName);
}

void ModernIconGridWidget::onFavoriteToggled(const QString& iconName, bool favorite)
{
    emit favoriteToggled(iconName, favorite);
}

void ModernIconGridWidget::onLayoutChanged()
{
    if (m_layoutManager) {
        // Update layout based on responsive layout manager
        int optimalSize = m_layoutManager->getOptimalItemSize();
        if (optimalSize != m_iconSize) {
            setIconSize(optimalSize);
        }

        QMargins optimalMargins = m_layoutManager->getOptimalMargins();
        if (optimalMargins != m_itemMargins) {
            setItemMargins(optimalMargins);
        }

        int optimalSpacing = m_layoutManager->getOptimalSpacing();
        if (optimalSpacing != m_itemSpacing) {
            setItemSpacing(optimalSpacing);
        }
    }

    updateLayout();
}

void ModernIconGridWidget::onThemeChanged()
{
    updateTheme();
}

void ModernIconGridWidget::setupUI()
{
    // Create main layout
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
}

void ModernIconGridWidget::setupLayout()
{
    // This will be implemented based on view mode
    applyViewMode();
}

void ModernIconGridWidget::setupScrollArea()
{
    // Create scroll area
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);

    // Create content widget
    m_contentWidget = new QWidget();
    m_scrollArea->setWidget(m_contentWidget);

    // Create grid layout
    m_gridLayout = new QGridLayout(m_contentWidget);
    m_gridLayout->setSpacing(m_itemSpacing);
    m_gridLayout->setContentsMargins(m_itemMargins);

    // Add scroll area to main layout
    m_mainLayout->addWidget(m_scrollArea);
}

void ModernIconGridWidget::createIconItems()
{
    // Clear existing items
    for (auto item : m_iconItems) {
        item->deleteLater();
    }
    m_iconItems.clear();

    // Create new items
    for (const QString& iconName : m_filteredIconNames) {
        ModernIconItem* item = new ModernIconItem(iconName, m_contentWidget);
        item->setIconSize(m_iconSize);
        item->setShowIconName(m_showIconNames);
        item->setAnimationsEnabled(m_animationsEnabled);
        item->setHoverEffectsEnabled(m_hoverEffectsEnabled);
        item->setGlowEnabled(m_glowEffectsEnabled);

        // Connect signals
        connect(item, &ModernIconItem::clicked, this, &ModernIconGridWidget::onIconClicked);
        connect(item, &ModernIconItem::doubleClicked, this, &ModernIconGridWidget::onIconDoubleClicked);
        connect(item, &ModernIconItem::rightClicked, this, &ModernIconGridWidget::onIconRightClicked);
        connect(item, &ModernIconItem::hoverEntered, this, &ModernIconGridWidget::onIconHovered);
        connect(item, &ModernIconItem::favoriteToggled, this, &ModernIconGridWidget::onFavoriteToggled);

        m_iconItems[iconName] = item;
    }
}

void ModernIconGridWidget::updateLayout()
{
    if (!m_gridLayout || !m_contentWidget) return;

    m_layoutUpdatePending = false;

    // Calculate optimal columns
    calculateOptimalColumns();

    // Clear current layout
    while (QLayoutItem* item = m_gridLayout->takeAt(0)) {
        // Don't delete the widget, just remove from layout
        delete item;
    }

    // Add visible items to grid
    int row = 0, col = 0;
    for (const QString& iconName : m_filteredIconNames) {
        ModernIconItem* item = m_iconItems.value(iconName);
        if (item && item->isVisible()) {
            m_gridLayout->addWidget(item, row, col);

            col++;
            if (col >= m_columns) {
                col = 0;
                row++;
            }
        }
    }

    // Update layout properties
    m_gridLayout->setSpacing(m_itemSpacing);
    m_gridLayout->setContentsMargins(m_itemMargins);

    // Force layout update
    m_contentWidget->updateGeometry();
    m_scrollArea->updateGeometry();
}

void ModernIconGridWidget::updateItemSizes()
{
    for (auto item : m_iconItems) {
        item->setIconSize(m_iconSize);
    }
}

void ModernIconGridWidget::calculateOptimalColumns()
{
    if (!m_scrollArea) return;

    int availableWidth = m_scrollArea->viewport()->width();
    int itemWidth = m_iconSize + 2 * 8; // Icon size + padding
    int spacing = m_itemSpacing;
    int margins = m_itemMargins.left() + m_itemMargins.right();

    availableWidth -= margins;

    if (itemWidth > 0) {
        m_columns = qMax(1, (availableWidth + spacing) / (itemWidth + spacing));
    } else {
        m_columns = 4; // Default fallback
    }

    // Apply view mode constraints
    switch (m_viewMode) {
        case CompactView:
            m_columns = qMin(m_columns, 8);
            break;
        case ListView:
            m_columns = 1;
            break;
        case GridView:
            m_columns = qMin(m_columns, 6);
            break;
        case DetailedView:
            m_columns = qMin(m_columns, 4);
            break;
    }
}

void ModernIconGridWidget::applyViewMode()
{
    switch (m_viewMode) {
        case GridView:
            m_showIconNames = true;
            break;
        case ListView:
            m_showIconNames = true;
            break;
        case CompactView:
            m_showIconNames = false;
            break;
        case DetailedView:
            m_showIconNames = true;
            break;
    }

    // Update all items
    for (auto item : m_iconItems) {
        item->setShowIconName(m_showIconNames);
    }
}

void ModernIconGridWidget::applyTheme()
{
    if (!m_themeManager) return;

    // Apply theme to scroll area
    if (m_scrollArea) {
        QString scrollAreaStyle = QString(
            "QScrollArea { "
            "    background-color: %1; "
            "    border: none; "
            "} "
            "QScrollBar:vertical { "
            "    background-color: %2; "
            "    width: 12px; "
            "    border-radius: 6px; "
            "} "
            "QScrollBar::handle:vertical { "
            "    background-color: %3; "
            "    border-radius: 6px; "
            "    min-height: 20px; "
            "} "
            "QScrollBar::handle:vertical:hover { "
            "    background-color: %4; "
            "}"
        ).arg(
            m_themeManager->getColor(ThemeManager::WindowBackground).name(),
            m_themeManager->getColor(ThemeManager::PanelBackground).name(),
            m_themeManager->getColor(ThemeManager::BorderColor).name(),
            m_themeManager->getColor(ThemeManager::HoverBackground).name()
        );

        m_scrollArea->setStyleSheet(scrollAreaStyle);
    }
}

void ModernIconGridWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    // Schedule layout update on resize
    if (!m_layoutUpdatePending) {
        m_layoutUpdatePending = true;
        m_layoutUpdateTimer->start();
    }
}

void ModernIconGridWidget::wheelEvent(QWheelEvent* event)
{
    // Handle zoom with Ctrl+Wheel
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else {
            zoomOut();
        }
        event->accept();
        return;
    }

    QWidget::wheelEvent(event);
}

void ModernIconGridWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomIn();
                event->accept();
                return;
            }
            break;
        case Qt::Key_Minus:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomOut();
                event->accept();
                return;
            }
            break;
        case Qt::Key_0:
            if (event->modifiers() & Qt::ControlModifier) {
                resetZoom();
                event->accept();
                return;
            }
            break;
        case Qt::Key_A:
            if (event->modifiers() & Qt::ControlModifier) {
                selectAll();
                event->accept();
                return;
            }
            break;
    }

    QWidget::keyPressEvent(event);
}
