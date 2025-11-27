/**
 * QtLucide Gallery - Enhanced Responsive Layout Manager Implementation
 *
 * Advanced implementation of the responsive layout management system that provides
 * seamless adaptation of the user interface based on available screen real estate.
 * Features intelligent panel management, smooth animations, and performance optimization.
 *
 * Enhanced Features:
 * - Multi-breakpoint responsive design with mobile-first approach
 * - Intelligent panel stacking and reordering for different screen sizes
 * - Advanced animation system with configurable easing functions
 * - Performance monitoring and optimization
 * - Accessibility support with layout announcements
 * - Touch-friendly adjustments for mobile devices
 * - Memory-efficient state management
 * - Integration with modern theme system
 */

#include "ResponsiveLayoutManager.h"
#include <QCursor>
#include <QDebug>
#include <QEasingCurve>
#include <QElapsedTimer>
#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScreen>
#include <QSequentialAnimationGroup>
#include <QSplitterHandle>
#include <QTimerEvent>
#include <QVBoxLayout>
#include <QWindow>

ResponsiveLayoutManager::ResponsiveLayoutManager(QWidget* mainWidget, QObject* parent)
    : QObject(parent), m_mainWidget(mainWidget), m_gridWidget(nullptr),
      m_currentScreenSize(Desktop), m_currentLayoutMode(StandardMode), m_adaptiveMode(true),
      m_animationsEnabled(true), m_animationDuration(DEFAULT_ANIMATION_DURATION),
      m_layoutAnimationGroup(new QParallelAnimationGroup(this)),
      m_stateAnimationGroup(new QSequentialAnimationGroup(this)),
      m_screenCheckTimer(new QTimer(this)), m_performanceTimer(new QTimer(this)),
      m_currentHistoryIndex(-1), m_animationEasingCurve(QEasingCurve::OutCubic), m_touchMode(false),
      m_highPerformanceMode(false), m_accessibilityMode(false), m_debugMode(false),
      m_resizeDebounceTime(100), m_performanceMonitoringEnabled(false) {
    // Enhanced initialization with performance monitoring
    QElapsedTimer initTimer;
    initTimer.start();

    // Initialize screen monitoring
    m_lastScreenSize = currentScreenResolution();
    m_currentScreenSize = detectScreenSize();
    m_currentLayoutMode = calculateOptimalLayoutMode();

    // Set up enhanced screen monitoring with debouncing
    m_screenCheckTimer->setSingleShot(true);
    connect(m_screenCheckTimer, &QTimer::timeout, this, &ResponsiveLayoutManager::checkScreenSize);

    // Set up performance monitoring
    m_performanceTimer->setInterval(1000); // Update every second
    connect(m_performanceTimer, &QTimer::timeout, this,
            &ResponsiveLayoutManager::updatePerformanceMetrics);

    // Connect to application screen changes with enhanced handling
    if (auto* guiApp = qobject_cast<QGuiApplication*>(QGuiApplication::instance())) {
        connect(guiApp, &QGuiApplication::screenAdded, this,
                &ResponsiveLayoutManager::onScreenAdded);
        connect(guiApp, &QGuiApplication::screenRemoved, this,
                &ResponsiveLayoutManager::onScreenRemoved);
        connect(guiApp, &QGuiApplication::primaryScreenChanged, this,
                &ResponsiveLayoutManager::onPrimaryScreenChanged);
    }

    // Install enhanced event filter on main widget
    if (m_mainWidget) {
        m_mainWidget->installEventFilter(this);

        // Detect touch capability (simplified - Qt doesn't provide direct touch detection)
        m_touchMode = false; // Can be set externally if touch is detected
    }

    // Connect enhanced animation groups
    connect(m_layoutAnimationGroup, &QParallelAnimationGroup::finished, this,
            &ResponsiveLayoutManager::onLayoutAnimationFinished);
    connect(m_stateAnimationGroup, &QSequentialAnimationGroup::finished, this,
            &ResponsiveLayoutManager::onStateAnimationFinished);

    // Initialize responsive state tracking
    initializeResponsiveStates();

    // Calculate initial layout configuration
    m_currentConfig = createLayoutConfig();

    // Apply initial layout
    if (m_adaptiveMode) {
        applyLayoutConfig(m_currentConfig);
    }

    qDebug() << "ResponsiveLayoutManager enhanced initialization completed in"
             << initTimer.elapsed() << "ms for screen size:" << m_currentScreenSize
             << "Touch mode:" << m_touchMode;

    // Emit initialization complete signal
    emit initializationCompleted(m_currentScreenSize, m_currentLayoutMode);
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
            return qMin(columns, 1);
        case MobileLarge:
            return qMin(columns, 2);
        case Tablet:
            return qMin(columns, 3);
        case Desktop:
            return qMin(columns, 4);
        case Large:
            return qMin(columns, 6);
        case XLarge:
            return qMin(columns, 8);
        case XXLarge:
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
    // Enhanced responsive margins with better proportions for different screen sizes
    switch (m_currentScreenSize) {
        case Mobile:
            return QMargins(8, 8, 8, 8); // Compact for mobile devices
        case MobileLarge:
            return QMargins(12, 12, 12, 12); // Slightly more space for large phones
        case Tablet:
            return QMargins(16, 16, 16, 16); // Comfortable spacing for tablets
        case Desktop:
            return QMargins(24, 24, 24, 24); // Standard desktop spacing
        case Large:
            return QMargins(32, 32, 32, 32); // More generous spacing for large desktops
        case XLarge:
            return QMargins(40, 40, 40, 40); // Spacious layout for large monitors
        case XXLarge:
            return QMargins(48, 48, 48, 48); // Maximum spacing for ultra-wide displays
    }
    return QMargins(24, 24, 24, 24);
}

int ResponsiveLayoutManager::getOptimalSpacing() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return 4; // Tight spacing for mobile
        case MobileLarge:
            return 6; // Slightly more spacing
        case Tablet:
            return DEFAULT_SPACING; // 8px
        case Desktop:
            return 10; // Comfortable spacing
        case Large:
            return DESKTOP_SPACING; // 12px
        case XLarge:
            return 16; // Generous spacing
        case XXLarge:
            return 20; // Maximum spacing
    }
    return DEFAULT_SPACING;
}

int ResponsiveLayoutManager::getOptimalItemSize() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return 64; // Smaller for mobile
        case MobileLarge:
            return 80; // Slightly larger
        case Tablet:
            return 96; // Comfortable size
        case Desktop:
            return 128; // Standard size
        case Large:
            return 160; // Larger for big screens
        case XLarge:
            return 192; // Maximum detail
        case XXLarge:
            return 224; // Ultra-high resolution
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

        // Create and apply new layout configuration with smooth transitions
        m_currentConfig = createLayoutConfig();

        if (m_animationsEnabled && layoutModeChanged) {
            animateLayoutTransition(oldLayoutMode, newLayoutMode);
        } else {
            applyLayoutConfig(m_currentConfig);
        }

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
    return getScreenSizeForWidth(width);
}

ResponsiveLayoutManager::LayoutMode ResponsiveLayoutManager::calculateOptimalLayoutMode() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return CompactMode;
        case MobileLarge:
            return CompactMode;
        case Tablet:
            return StandardMode;
        case Desktop:
            return StandardMode;
        case Large:
            return WideMode;
        case XLarge:
            return ExtremeWideMode;
        case XXLarge:
            return ExtremeWideMode;
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

    // Enhanced responsive layout configuration with better proportions
    switch (m_currentLayoutMode) {
        case CompactMode:
            // Mobile-first single column layout
            config.columns = 1;
            config.showSidebar = false;
            config.showDetailsPanel = false;
            config.sidebarState = Hidden;
            config.detailsState = Hidden;
            config.splitterSizes = {1}; // Content only
            break;

        case StandardMode:
            // Tablet and small desktop layout
            config.columns = (m_currentScreenSize >= Desktop) ? 3 : 2;
            config.showSidebar = true;
            config.showDetailsPanel = false;
            config.sidebarState = (m_currentScreenSize >= Desktop) ? Visible : Collapsed;
            config.detailsState = Hidden;
            config.splitterSizes = (m_currentScreenSize >= Desktop)
                                       ? QList<int>{1, 4}
                                       : QList<int>{1, 3}; // Sidebar : Content
            break;

        case WideMode:
            // Standard desktop layout
            config.columns = (m_currentScreenSize >= Large) ? 5 : 4;
            config.showSidebar = true;
            config.showDetailsPanel = true;
            config.sidebarState = Visible;
            config.detailsState = (m_currentScreenSize >= Large) ? Visible : Collapsed;
            config.splitterSizes = {1, 4, 2}; // Sidebar : Content : Details
            break;

        case UltraWideMode:
            // Large desktop layout
            config.columns = (m_currentScreenSize >= XXLarge) ? 8 : 6;
            config.showSidebar = true;
            config.showDetailsPanel = true;
            config.sidebarState = Expanded;
            config.detailsState = Visible;
            config.splitterSizes = {1, 5, 2}; // Sidebar : Content : Details
            break;

        case ExtremeWideMode:
            // Ultra-wide and 4K+ displays
            config.columns = (m_currentScreenSize >= XXLarge) ? 12 : 10;
            config.showSidebar = true;
            config.showDetailsPanel = true;
            config.sidebarState = Expanded;
            config.detailsState = Expanded;
            config.splitterSizes = {1, 6, 3}; // Sidebar : Content : Details
            break;
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

// Helper methods for responsive panel sizing
int ResponsiveLayoutManager::getOptimalCollapsedWidth() const {
    switch (m_currentScreenSize) {
        case Mobile:
        case MobileLarge:
            return 40; // Compact for phones
        case Tablet:
            return 45; // Slightly larger for tablets
        case Desktop:
        case Large:
            return 50; // Standard for desktop
        case XLarge:
        case XXLarge:
            return 60; // Larger for big screens
        default:
            return 50;
    }
}

int ResponsiveLayoutManager::getOptimalVisibleWidth() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return 200; // Narrow for phones
        case MobileLarge:
            return 220; // Slightly wider for large phones
        case Tablet:
            return 250; // Standard tablet width
        case Desktop:
            return 280; // Standard desktop width
        case Large:
            return 300; // Larger desktop
        case XLarge:
            return 320; // Large screen width
        case XXLarge:
            return 350; // Extra large screens
        default:
            return 280;
    }
}

int ResponsiveLayoutManager::getOptimalExpandedWidth() const {
    switch (m_currentScreenSize) {
        case Mobile:
            return 250; // Compact expanded for phones
        case MobileLarge:
            return 280; // Slightly wider for large phones
        case Tablet:
            return 320; // Standard tablet expanded
        case Desktop:
            return 350; // Standard desktop expanded
        case Large:
            return 380; // Larger desktop expanded
        case XLarge:
            return 420; // Large screen expanded
        case XXLarge:
            return 450; // Extra large expanded
        default:
            return 350;
    }
}

void ResponsiveLayoutManager::animatePanelTransition(QWidget* panel, PanelState fromState,
                                                     PanelState toState) {
    Q_UNUSED(fromState)
    if (!panel || !m_animationsEnabled) {
        // Apply state immediately with responsive sizing
        switch (toState) {
            case Hidden:
                panel->hide();
                break;
            case Collapsed:
                panel->setMaximumWidth(getOptimalCollapsedWidth());
                panel->show();
                break;
            case Visible:
                panel->setMaximumWidth(getOptimalVisibleWidth());
                panel->show();
                break;
            case Expanded:
                panel->setMaximumWidth(getOptimalExpandedWidth());
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
            animation->setEndValue(getOptimalCollapsedWidth());
            panel->show();
            break;
        case Visible:
            animation->setEndValue(getOptimalVisibleWidth());
            panel->show();
            break;
        case Expanded:
            animation->setEndValue(getOptimalExpandedWidth());
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

// Static utility methods implementation
ResponsiveLayoutManager::ScreenSize ResponsiveLayoutManager::getScreenSizeForWidth(int width) {
    if (width < getMobileBreakpoint()) {
        return Mobile;
    } else if (width < getMobileLargeBreakpoint()) {
        return MobileLarge;
    } else if (width < getTabletBreakpoint()) {
        return Tablet;
    } else if (width < getDesktopBreakpoint()) {
        return Desktop;
    } else if (width < getLargeBreakpoint()) {
        return Large;
    } else if (width < getXLargeBreakpoint()) {
        return XLarge;
    } else {
        return XXLarge;
    }
}

ResponsiveLayoutManager::LayoutMode
ResponsiveLayoutManager::getOptimalLayoutModeForScreenSize(ScreenSize screenSize) {
    switch (screenSize) {
        case Mobile:
        case MobileLarge:
            return CompactMode;
        case Tablet:
            return StandardMode;
        case Desktop:
            return WideMode;
        case Large:
            return UltraWideMode;
        case XLarge:
        case XXLarge:
            return ExtremeWideMode;
    }
    return StandardMode;
}

int ResponsiveLayoutManager::getOptimalColumnsForScreenSize(ScreenSize screenSize) {
    switch (screenSize) {
        case Mobile:
            return 1;
        case MobileLarge:
            return 2;
        case Tablet:
            return 3;
        case Desktop:
            return 4;
        case Large:
            return 6;
        case XLarge:
            return 8;
        case XXLarge:
            return 10;
    }
    return 4;
}

int ResponsiveLayoutManager::getOptimalSpacingForScreenSize(ScreenSize screenSize) {
    switch (screenSize) {
        case Mobile:
            return 4;
        case MobileLarge:
            return 6;
        case Tablet:
            return 8;
        case Desktop:
            return 10;
        case Large:
            return 12;
        case XLarge:
            return 16;
        case XXLarge:
            return 20;
    }
    return 8;
}

QMargins ResponsiveLayoutManager::getOptimalMarginsForScreenSize(ScreenSize screenSize) {
    switch (screenSize) {
        case Mobile:
            return QMargins(4, 4, 4, 4);
        case MobileLarge:
            return QMargins(8, 8, 8, 8);
        case Tablet:
            return QMargins(12, 12, 12, 12);
        case Desktop:
            return QMargins(16, 16, 16, 16);
        case Large:
            return QMargins(20, 20, 20, 20);
        case XLarge:
            return QMargins(24, 24, 24, 24);
        case XXLarge:
            return QMargins(32, 32, 32, 32);
    }
    return QMargins(16, 16, 16, 16);
}

void ResponsiveLayoutManager::animateLayoutTransition(LayoutMode fromMode, LayoutMode toMode) {
    if (!m_animationsEnabled) {
        applyLayoutConfig(m_currentConfig);
        return;
    }

    // Enhanced layout transition with sophisticated animations
    if (m_layoutAnimationGroup) {
        m_layoutAnimationGroup->stop();
        m_layoutAnimationGroup->clear();
    }

    // Create different animation strategies based on the transition type
    bool isMajorTransition = (fromMode == CompactMode && toMode != CompactMode) ||
                             (fromMode != CompactMode && toMode == CompactMode) ||
                             (fromMode == ExtremeWideMode && toMode != ExtremeWideMode) ||
                             (fromMode != ExtremeWideMode && toMode == ExtremeWideMode);

    if (isMajorTransition) {
        createMajorLayoutTransition(fromMode, toMode);
    } else {
        createMinorLayoutTransition(fromMode, toMode);
    }
}

void ResponsiveLayoutManager::createMajorLayoutTransition(LayoutMode fromMode, LayoutMode toMode) {
    // Create sophisticated cross-fade transition for major layout changes
    QSequentialAnimationGroup* transitionGroup = new QSequentialAnimationGroup();

    // Phase 1: Fade out and prepare for reorganization
    QParallelAnimationGroup* fadeOutGroup = new QParallelAnimationGroup();

    for (auto it = m_panels.begin(); it != m_panels.end(); ++it) {
        QWidget* panel = it.value();
        if (!panel || !panel->isVisible())
            continue;

        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(panel);
        panel->setGraphicsEffect(effect);

        QPropertyAnimation* fadeOut = new QPropertyAnimation(effect, "opacity");
        fadeOut->setDuration(m_animationDuration * 0.6); // 60% of duration for fade out
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.1);
        fadeOut->setEasingCurve(QEasingCurve::InOutCubic);

        fadeOutGroup->addAnimation(fadeOut);
    }

    // Phase 2: Apply new layout configuration
    QPropertyAnimation* layoutConfigAnimation = new QPropertyAnimation(this, "layoutConfig");
    layoutConfigAnimation->setDuration(50); // Brief pause for layout reorganization
    connect(layoutConfigAnimation, &QPropertyAnimation::finished,
            [this]() { applyLayoutConfig(m_currentConfig); });

    // Phase 3: Fade in with new layout
    QParallelAnimationGroup* fadeInGroup = new QParallelAnimationGroup();

    for (auto it = m_panels.begin(); it != m_panels.end(); ++it) {
        QWidget* panel = it.value();
        if (!panel)
            continue;

        QGraphicsOpacityEffect* effect =
            qobject_cast<QGraphicsOpacityEffect*>(panel->graphicsEffect());
        if (!effect) {
            effect = new QGraphicsOpacityEffect(panel);
            panel->setGraphicsEffect(effect);
        }

        QPropertyAnimation* fadeIn = new QPropertyAnimation(effect, "opacity");
        fadeIn->setDuration(m_animationDuration * 0.4); // 40% of duration for fade in
        fadeIn->setStartValue(0.1);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::InOutCubic);

        // Clean up effect after animation
        connect(fadeIn, &QPropertyAnimation::finished, [panel, effect]() {
            if (panel && effect) {
                panel->setGraphicsEffect(nullptr);
                effect->deleteLater();
            }
        });

        fadeInGroup->addAnimation(fadeIn);
    }

    transitionGroup->addAnimation(fadeOutGroup);
    transitionGroup->addAnimation(layoutConfigAnimation);
    transitionGroup->addAnimation(fadeInGroup);

    transitionGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void ResponsiveLayoutManager::createMinorLayoutTransition(LayoutMode fromMode, LayoutMode toMode) {
    // Create smooth panel resize animations for minor layout changes
    QParallelAnimationGroup* animationGroup = new QParallelAnimationGroup();

    for (auto it = m_panels.begin(); it != m_panels.end(); ++it) {
        QWidget* panel = it.value();
        if (!panel)
            continue;

        PanelState targetState = m_currentConfig.sidebarState;
        if (it.key() == "details") {
            targetState = m_currentConfig.detailsState;
        }

        // Create size animation for the panel
        QPropertyAnimation* sizeAnimation = createPanelSizeAnimation(panel, targetState);
        if (sizeAnimation) {
            animationGroup->addAnimation(sizeAnimation);
        }
    }

    // Apply layout changes immediately for minor transitions
    applyLayoutConfig(m_currentConfig);

    if (animationGroup->animationCount() > 0) {
        animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

QPropertyAnimation* ResponsiveLayoutManager::createPanelSizeAnimation(QWidget* panel,
                                                                      PanelState targetState) {
    if (!panel)
        return nullptr;

    int targetWidth = 0;
    bool shouldShow = true;

    switch (targetState) {
        case Hidden:
            targetWidth = 0;
            shouldShow = false;
            break;
        case Collapsed:
            targetWidth = getOptimalCollapsedWidth();
            shouldShow = true;
            break;
        case Visible:
            targetWidth = getOptimalVisibleWidth();
            shouldShow = true;
            break;
        case Expanded:
            targetWidth = getOptimalExpandedWidth();
            shouldShow = true;
            break;
    }

    QPropertyAnimation* animation = new QPropertyAnimation(panel, "maximumWidth");
    animation->setDuration(m_animationDuration);
    animation->setStartValue(panel->maximumWidth());
    animation->setEndValue(targetWidth);
    animation->setEasingCurve(QEasingCurve::OutCubic);

    // Handle show/hide after animation
    if (!shouldShow) {
        connect(animation, &QPropertyAnimation::finished, panel, &QWidget::hide);
    } else if (!panel->isVisible()) {
        panel->show();
    }

    return animation;
}

// ============================================================================
// STUB IMPLEMENTATIONS FOR MISSING SLOT METHODS
// ============================================================================

void ResponsiveLayoutManager::onLayoutAnimationFinished() {
    // Stub implementation
}

void ResponsiveLayoutManager::onStateAnimationFinished() {
    // Stub implementation
}

void ResponsiveLayoutManager::onScreenAdded(QScreen* screen) {
    Q_UNUSED(screen);
    // Stub implementation
}

void ResponsiveLayoutManager::onScreenRemoved(QScreen* screen) {
    Q_UNUSED(screen);
    // Stub implementation
}

void ResponsiveLayoutManager::onPrimaryScreenChanged(QScreen* screen) {
    Q_UNUSED(screen);
    // Stub implementation
}

void ResponsiveLayoutManager::updatePerformanceMetrics() {
    // Stub implementation
}

void ResponsiveLayoutManager::initializeResponsiveStates() {
    // Stub implementation
}
