/**
 * QtLucide Gallery Application - Responsive Layout Manager Implementation
 */

#include "ResponsiveLayoutManager.h"
#include <QDebug>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QScreen>
#include <QSequentialAnimationGroup>
#include <QSplitterHandle>
#include <QVBoxLayout>
#include <QWindow>


ResponsiveLayoutManager::ResponsiveLayoutManager(QWidget* mainWidget, QObject* parent)
    : QObject(parent), m_mainWidget(mainWidget), m_gridWidget(nullptr),
      m_currentScreenSize(Desktop), m_currentLayoutMode(StandardMode), m_adaptiveMode(true),
      m_animationsEnabled(true), m_animationDuration(DEFAULT_ANIMATION_DURATION),
      m_layoutAnimationGroup(new QParallelAnimationGroup(this)),
      m_screenCheckTimer(new QTimer(this)), m_currentHistoryIndex(-1) {
    // Initialize screen monitoring
    m_lastScreenSize = currentScreenResolution();
    m_currentScreenSize = detectScreenSize();
    m_currentLayoutMode = calculateOptimalLayoutMode();

    // Set up screen monitoring
    connect(m_screenCheckTimer, &QTimer::timeout, this, &ResponsiveLayoutManager::checkScreenSize);
    m_screenCheckTimer->start(SCREEN_CHECK_INTERVAL);

    // Connect to application screen changes
    if (auto* guiApp = qobject_cast<QGuiApplication*>(QGuiApplication::instance())) {
        connect(guiApp, &QGuiApplication::screenAdded, this,
                &ResponsiveLayoutManager::onScreenSizeChanged);
        connect(guiApp, &QGuiApplication::screenRemoved, this,
                &ResponsiveLayoutManager::onScreenSizeChanged);
    }

    // Install event filter on main widget
    if (m_mainWidget) {
        m_mainWidget->installEventFilter(this);
    }

    // Connect animation group
    connect(m_layoutAnimationGroup, &QParallelAnimationGroup::finished, this,
            &ResponsiveLayoutManager::onAnimationFinished);

    qDebug() << "ResponsiveLayoutManager initialized for screen size:" << m_currentScreenSize;
}

ResponsiveLayoutManager::~ResponsiveLayoutManager() {
    // Clean up animations
    m_layoutAnimationGroup->stop();
    for (auto animation : m_activeAnimations) {
        animation->stop();
        animation->deleteLater();
    }
}

QSize ResponsiveLayoutManager::currentScreenResolution() const {
    if (m_mainWidget && m_mainWidget->window()) {
        QScreen* screen = m_mainWidget->window()->screen();
        if (screen) {
            return screen->availableGeometry().size();
        }
    }

    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    return primaryScreen ? primaryScreen->availableGeometry().size() : QSize(1920, 1080);
}

void ResponsiveLayoutManager::setLayoutMode(LayoutMode mode) {
    if (m_currentLayoutMode == mode)
        return;

    LayoutMode oldMode = m_currentLayoutMode;
    m_currentLayoutMode = mode;

    // Update layout configuration
    m_currentConfig = createLayoutConfig();
    applyLayoutConfig(m_currentConfig);

    emit layoutModeChanged(mode, oldMode);
    emit layoutUpdated();

    qDebug() << "Layout mode changed from" << oldMode << "to" << mode;
}

void ResponsiveLayoutManager::setGridWidget(QWidget* gridWidget) {
    m_gridWidget = gridWidget;
    if (m_gridWidget) {
        updateGridLayout();
    }
}

int ResponsiveLayoutManager::calculateOptimalColumns(int containerWidth, int itemWidth,
                                                     int spacing) const {
    if (itemWidth <= 0 || containerWidth <= 0)
        return 1;

    // Calculate how many items can fit with spacing
    int availableWidth = containerWidth - (2 * getOptimalMargins().left());
    int itemWithSpacing = itemWidth + spacing;
    int columns = qMax(1, (availableWidth + spacing) / itemWithSpacing);

    // Apply screen size constraints
    switch (m_currentScreenSize) {
        case Mobile:
            return qMin(columns, 2);
        case Tablet:
            return qMin(columns, 4);
        case Desktop:
            return qMin(columns, 6);
        case Large:
            return qMin(columns, 8);
        case XLarge:
            return qMin(columns, 12);
    }

    return columns;
}

void ResponsiveLayoutManager::setOptimalColumns(int itemWidth, int minColumns, int maxColumns) {
    if (!m_gridWidget)
        return;

    int containerWidth = m_gridWidget->width();
    int optimalColumns = calculateOptimalColumns(containerWidth, itemWidth, getOptimalSpacing());

    // Apply constraints
    optimalColumns = qMax(minColumns, qMin(maxColumns, optimalColumns));

    // Apply to grid layout if it exists
    QGridLayout* gridLayout = qobject_cast<QGridLayout*>(m_gridWidget->layout());
    if (gridLayout) {
        // Store current column count for comparison
        int currentColumns = gridLayout->columnCount();
        if (currentColumns != optimalColumns) {
            updateGridLayout();
        }
    }
}

void ResponsiveLayoutManager::updateGridLayout() {
    if (!m_gridWidget)
        return;

    applyGridLayout();
    emit layoutUpdated();
}

void ResponsiveLayoutManager::addSplitter(const QString& name, QSplitter* splitter) {
    if (!splitter)
        return;

    m_splitters[name] = splitter;

    // Connect to splitter signals
    connect(splitter, &QSplitter::splitterMoved, this, &ResponsiveLayoutManager::onSplitterMoved);

    // Apply responsive sizing
    applySplitterLayout();
}

void ResponsiveLayoutManager::setSplitterSizes(const QString& name, const QList<int>& sizes) {
    QSplitter* splitter = m_splitters.value(name);
    if (!splitter || sizes.isEmpty())
        return;

    m_splitterSizes[name] = sizes;

    if (m_animationsEnabled) {
        animateSplitterResize(splitter, sizes);
    } else {
        splitter->setSizes(sizes);
    }

    emit splitterSizesChanged(name, sizes);
}

QList<int> ResponsiveLayoutManager::getSplitterSizes(const QString& name) const {
    QSplitter* splitter = m_splitters.value(name);
    return splitter ? splitter->sizes() : QList<int>();
}

void ResponsiveLayoutManager::addPanel(const QString& name, QWidget* panel,
                                       PanelState initialState) {
    if (!panel)
        return;

    m_panels[name] = panel;
    m_panelStates[name] = initialState;

    // Apply initial state
    setPanelState(name, initialState, false);
}

void ResponsiveLayoutManager::setPanelState(const QString& name, PanelState state, bool animated) {
    QWidget* panel = m_panels.value(name);
    if (!panel)
        return;

    PanelState oldState = m_panelStates.value(name, Visible);
    if (oldState == state)
        return;

    m_panelStates[name] = state;

    if (animated && m_animationsEnabled) {
        animatePanelTransition(panel, oldState, state);
    } else {
        // Apply state immediately
        switch (state) {
            case Hidden:
                panel->hide();
                break;
            case Collapsed:
                panel->show();
                panel->setMaximumWidth(MIN_PANEL_WIDTH / 2);
                break;
            case Visible:
                panel->show();
                panel->setMaximumWidth(QWIDGETSIZE_MAX);
                panel->setMinimumWidth(MIN_PANEL_WIDTH);
                break;
            case Expanded:
                panel->show();
                panel->setMaximumWidth(QWIDGETSIZE_MAX);
                panel->setMinimumWidth(MIN_PANEL_WIDTH * 2);
                break;
        }
    }

    emit panelStateChanged(name, state);
}

ResponsiveLayoutManager::PanelState
ResponsiveLayoutManager::getPanelState(const QString& name) const {
    return m_panelStates.value(name, Visible);
}

void ResponsiveLayoutManager::togglePanel(const QString& name, bool animated) {
    PanelState currentState = getPanelState(name);
    PanelState newState = (currentState == Hidden) ? Visible : Hidden;
    setPanelState(name, newState, animated);
}

QMargins ResponsiveLayoutManager::getOptimalMargins() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return QMargins(8, 8, 8, 8);
        case Tablet:
            return QMargins(12, 12, 12, 12);
        case Desktop:
            return QMargins(16, 16, 16, 16);
        case Large:
            return QMargins(20, 20, 20, 20);
        case XLarge:
            return QMargins(24, 24, 24, 24);
    }
    return QMargins(16, 16, 16, 16);
}

int ResponsiveLayoutManager::getOptimalSpacing() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return MOBILE_SPACING;
        case Tablet:
            return DEFAULT_SPACING;
        case Desktop:
            return DEFAULT_SPACING + 2;
        case Large:
            return DESKTOP_SPACING;
        case XLarge:
            return DESKTOP_SPACING + 4;
    }
    return DEFAULT_SPACING;
}

int ResponsiveLayoutManager::getOptimalItemSize() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return 80;
        case Tablet:
            return 96;
        case Desktop:
            return 128;
        case Large:
            return 160;
        case XLarge:
            return 192;
    }
    return 128;
}

QSize ResponsiveLayoutManager::getOptimalThumbnailSize() const {
    int size = getOptimalItemSize();
    return QSize(size, size);
}

void ResponsiveLayoutManager::onScreenSizeChanged() {
    checkScreenSize();
}

void ResponsiveLayoutManager::onWindowResized(const QSize& newSize) {
    Q_UNUSED(newSize)

    // Delay layout update to avoid too frequent updates during resize
    QTimer::singleShot(100, this, &ResponsiveLayoutManager::updateLayout);
}

void ResponsiveLayoutManager::updateLayout() {
    if (!m_adaptiveMode)
        return;

    // Detect current screen size
    ScreenSize newScreenSize = detectScreenSize();
    LayoutMode newLayoutMode = calculateOptimalLayoutMode();

    bool screenSizeChanged = (newScreenSize != m_currentScreenSize);
    bool layoutModeChanged = (newLayoutMode != m_currentLayoutMode);

    if (screenSizeChanged || layoutModeChanged) {
        ScreenSize oldScreenSize = m_currentScreenSize;
        LayoutMode oldLayoutMode = m_currentLayoutMode;

        m_currentScreenSize = newScreenSize;
        m_currentLayoutMode = newLayoutMode;

        // Create and apply new layout configuration
        m_currentConfig = createLayoutConfig();
        applyLayoutConfig(m_currentConfig);

        // Emit signals
        if (screenSizeChanged) {
            emit ResponsiveLayoutManager::screenSizeChanged(newScreenSize, oldScreenSize);
        }
        if (layoutModeChanged) {
            emit ResponsiveLayoutManager::layoutModeChanged(newLayoutMode, oldLayoutMode);
        }
        emit layoutUpdated();

        qDebug() << "Layout updated - Screen:" << newScreenSize << "Mode:" << newLayoutMode;
    }
}

void ResponsiveLayoutManager::optimizeForCurrentScreen() {
    updateLayout();

    // Additional optimizations based on screen size
    if (isMobileLayout()) {
        // Hide non-essential panels on mobile
        for (auto it = m_panels.begin(); it != m_panels.end(); ++it) {
            if (it.key() != "main") {
                setPanelState(it.key(), Hidden, true);
            }
        }
    } else if (isDesktopLayout()) {
        // Show all panels on desktop
        for (auto it = m_panels.begin(); it != m_panels.end(); ++it) {
            setPanelState(it.key(), Visible, true);
        }
    }
}

bool ResponsiveLayoutManager::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_mainWidget && event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        onWindowResized(resizeEvent->size());
    }
    return QObject::eventFilter(watched, event);
}

void ResponsiveLayoutManager::checkScreenSize() {
    QSize currentSize = currentScreenResolution();
    if (currentSize != m_lastScreenSize) {
        m_lastScreenSize = currentSize;
        onScreenSizeChanged();
    }
}

ResponsiveLayoutManager::ScreenSize ResponsiveLayoutManager::detectScreenSize() const {
    QSize screenSize = currentScreenResolution();
    int width = screenSize.width();

    if (width < getMobileBreakpoint()) {
        return Mobile;
    } else if (width < getTabletBreakpoint()) {
        return Tablet;
    } else if (width < getDesktopBreakpoint()) {
        return Desktop;
    } else if (width < getLargeBreakpoint()) {
        return Large;
    } else {
        return XLarge;
    }
}

ResponsiveLayoutManager::LayoutMode ResponsiveLayoutManager::calculateOptimalLayoutMode() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return CompactMode;
        case Tablet:
            return StandardMode;
        case Desktop:
            return StandardMode;
        case Large:
            return WideMode;
        case XLarge:
            return UltraWideMode;
    }
    return StandardMode;
}

ResponsiveLayoutManager::LayoutConfig ResponsiveLayoutManager::createLayoutConfig() const {
    LayoutConfig config;
    config.screenSize = m_currentScreenSize;
    config.layoutMode = m_currentLayoutMode;
    config.margins = getOptimalMargins();
    config.spacing = getOptimalSpacing();
    config.itemSize = getOptimalItemSize();

    // Calculate columns based on layout mode
    switch (m_currentLayoutMode) {
        case CompactMode:
            config.columns = 2;
            config.showSidebar = false;
            config.showDetailsPanel = false;
            config.sidebarState = Hidden;
            config.detailsState = Hidden;
            break;
        case StandardMode:
            config.columns = 4;
            config.showSidebar = true;
            config.showDetailsPanel = false;
            config.sidebarState = Visible;
            config.detailsState = Hidden;
            break;
        case WideMode:
            config.columns = 6;
            config.showSidebar = true;
            config.showDetailsPanel = true;
            config.sidebarState = Visible;
            config.detailsState = Visible;
            break;
        case UltraWideMode:
            config.columns = 8;
            config.showSidebar = true;
            config.showDetailsPanel = true;
            config.sidebarState = Expanded;
            config.detailsState = Expanded;
            break;
    }

    // Calculate splitter sizes
    if (config.showSidebar && config.showDetailsPanel) {
        config.splitterSizes = {1, 4, 2}; // Sidebar : Content : Details
    } else if (config.showSidebar) {
        config.splitterSizes = {1, 4}; // Sidebar : Content
    } else {
        config.splitterSizes = {1}; // Content only
    }

    return config;
}

void ResponsiveLayoutManager::applyLayoutConfig(const LayoutConfig& config) {
    Q_UNUSED(config)
    // Apply grid layout
    applyGridLayout();

    // Apply splitter layout
    applySplitterLayout();

    // Apply panel states
    applyPanelLayout();

    // Apply spacing and margins
    applySpacingAndMargins();

    // Animate the transition if enabled
    if (m_animationsEnabled) {
        animateLayoutTransition();
    }
}

void ResponsiveLayoutManager::applyGridLayout() {
    if (!m_gridWidget)
        return;

    QLayout* layout = m_gridWidget->layout();
    if (!layout)
        return;

    // Apply optimal spacing
    layout->setSpacing(getOptimalSpacing());
    layout->setContentsMargins(getOptimalMargins());

    // If it's a grid layout, update column count
    QGridLayout* gridLayout = qobject_cast<QGridLayout*>(layout);
    if (gridLayout) {
        // This would require reorganizing items, which is complex
        // For now, just update spacing and margins
        gridLayout->setHorizontalSpacing(getOptimalSpacing());
        gridLayout->setVerticalSpacing(getOptimalSpacing());
    }
}

void ResponsiveLayoutManager::applySplitterLayout() {
    for (auto it = m_splitters.begin(); it != m_splitters.end(); ++it) {
        QSplitter* splitter = it.value();
        if (!splitter)
            continue;

        // Apply responsive splitter sizes
        QList<int> sizes = m_currentConfig.splitterSizes;
        if (sizes.size() == splitter->count()) {
            setSplitterSizes(it.key(), sizes);
        }
    }
}

void ResponsiveLayoutManager::applyPanelLayout() {
    // Apply sidebar state
    if (m_panels.contains("sidebar")) {
        setPanelState("sidebar", m_currentConfig.sidebarState, m_animationsEnabled);
    }

    // Apply details panel state
    if (m_panels.contains("details")) {
        setPanelState("details", m_currentConfig.detailsState, m_animationsEnabled);
    }
}

void ResponsiveLayoutManager::applySpacingAndMargins() {
    if (!m_mainWidget)
        return;

    QLayout* mainLayout = m_mainWidget->layout();
    if (mainLayout) {
        mainLayout->setSpacing(getOptimalSpacing());
        mainLayout->setContentsMargins(getOptimalMargins());
    }
}

// Missing ResponsiveLayoutManager methods
void ResponsiveLayoutManager::resetToDefaults() {
    // Reset to default configuration
    m_currentConfig = LayoutConfig();
    applyLayoutConfig(m_currentConfig);
    emit layoutUpdated();
}

void ResponsiveLayoutManager::onAnimationFinished() {
    // Handle animation completion
    emit layoutUpdated();
}

void ResponsiveLayoutManager::onSplitterMoved() {
    QSplitter* splitter = qobject_cast<QSplitter*>(sender());
    if (!splitter)
        return;

    // Find the splitter name and update sizes
    for (auto it = m_splitters.begin(); it != m_splitters.end(); ++it) {
        if (it.value() == splitter) {
            m_splitterSizes[it.key()] = splitter->sizes();
            emit splitterSizesChanged(it.key(), splitter->sizes());
            break;
        }
    }
}

void ResponsiveLayoutManager::animateSplitterResize(QSplitter* splitter, const QList<int>& sizes) {
    if (!splitter || !m_animationsEnabled) {
        splitter->setSizes(sizes);
        return;
    }

    // Create animation for splitter resize
    QPropertyAnimation* animation = new QPropertyAnimation(splitter, "sizes");
    animation->setDuration(m_animationDuration);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setEndValue(QVariant::fromValue(sizes));

    m_layoutAnimationGroup->addAnimation(animation);
    m_layoutAnimationGroup->start();
}

void ResponsiveLayoutManager::animatePanelTransition(QWidget* panel, PanelState fromState,
                                                     PanelState toState) {
    Q_UNUSED(fromState)
    if (!panel || !m_animationsEnabled) {
        // Apply state immediately
        switch (toState) {
            case Hidden:
                panel->hide();
                break;
            case Collapsed:
                panel->setMaximumWidth(50);
                panel->show();
                break;
            case Visible:
                panel->setMaximumWidth(200);
                panel->show();
                break;
            case Expanded:
                panel->setMaximumWidth(QWIDGETSIZE_MAX);
                panel->show();
                break;
        }
        return;
    }

    // Create animation for panel transition
    QPropertyAnimation* animation = new QPropertyAnimation(panel, "maximumWidth");
    animation->setDuration(m_animationDuration);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    switch (toState) {
        case Hidden:
            animation->setEndValue(0);
            connect(animation, &QPropertyAnimation::finished, panel, &QWidget::hide);
            break;
        case Collapsed:
            animation->setEndValue(50);
            panel->show();
            break;
        case Visible:
            animation->setEndValue(200);
            panel->show();
            break;
        case Expanded:
            animation->setEndValue(300);
            panel->show();
            break;
    }

    m_layoutAnimationGroup->addAnimation(animation);
    m_layoutAnimationGroup->start();
}

void ResponsiveLayoutManager::animateLayoutTransition() {
    if (!m_animationsEnabled)
        return;

    // Start the layout animation group
    if (m_layoutAnimationGroup->animationCount() > 0) {
        m_layoutAnimationGroup->start();
    }
}

// ResponsiveWidget Implementation
ResponsiveWidget::ResponsiveWidget(QWidget* widget, ResponsiveLayoutManager* manager,
                                   QObject* parent)
    : QObject(parent), m_widget(widget), m_manager(manager) {
    if (m_manager) {
        connect(m_manager, &ResponsiveLayoutManager::screenSizeChanged, this,
                &ResponsiveWidget::onScreenSizeChanged);
    }
}

ResponsiveWidget::~ResponsiveWidget() = default;

void ResponsiveWidget::onScreenSizeChanged(ResponsiveLayoutManager::ScreenSize oldSize,
                                           ResponsiveLayoutManager::ScreenSize newSize) {
    Q_UNUSED(oldSize)
    Q_UNUSED(newSize)
    // Handle screen size change
    if (m_widget) {
        m_widget->update();
    }
}
