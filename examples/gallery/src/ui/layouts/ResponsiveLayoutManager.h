/**
 * QtLucide Gallery Application - Responsive Layout Manager
 *
 * Advanced layout management system that provides:
 * - Responsive design with breakpoints
 * - Adaptive grid layouts with optimal column calculation
 * - Smart sidebar and panel management
 * - Smooth layout transitions and animations
 * - Screen size detection and optimization
 * - Dynamic spacing and sizing adjustments
 * - Mobile-friendly responsive behavior
 */

#ifndef RESPONSIVELAYOUTMANAGER_H
#define RESPONSIVELAYOUTMANAGER_H

#include <QApplication>
#include <QEasingCurve>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHash>
#include <QMargins>
#include <QObject>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QScreen>
#include <QScrollArea>
#include <QSequentialAnimationGroup>
#include <QSize>
#include <QSplitter>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>

/**
 * @brief Advanced responsive layout management for the Gallery application
 */
class ResponsiveLayoutManager : public QObject {
    Q_OBJECT

public:
    enum ScreenSize {
        Mobile = 0,      // < 640px width (phones)
        MobileLarge = 1, // 640-768px width (large phones)
        Tablet = 2,      // 768-1024px width (tablets)
        Desktop = 3,     // 1024-1366px width (laptops)
        Large = 4,       // 1366-1920px width (desktops)
        XLarge = 5,      // 1920-2560px width (large monitors)
        XXLarge = 6      // > 2560px width (ultra-wide/4K+)
    };
    Q_ENUM(ScreenSize)

    enum LayoutMode {
        CompactMode = 0,    // Single column, minimal spacing (mobile)
        StandardMode = 1,   // Two columns, standard spacing (tablet)
        WideMode = 2,       // Three columns, generous spacing (desktop)
        UltraWideMode = 3,  // Four columns, maximum spacing (large)
        ExtremeWideMode = 4 // Five+ columns, ultra spacing (ultra-wide)
    };
    Q_ENUM(LayoutMode)

    enum PanelState { Hidden = 0, Collapsed = 1, Visible = 2, Expanded = 3 };
    Q_ENUM(PanelState)

    struct LayoutConfig {
        ScreenSize screenSize;
        LayoutMode layoutMode;
        int columns;
        int itemSize;
        int spacing;
        QMargins margins;
        bool showSidebar;
        bool showDetailsPanel;
        PanelState sidebarState;
        PanelState detailsState;
        QList<int> splitterSizes;
    };

    explicit ResponsiveLayoutManager(QWidget* mainWidget, QObject* parent = nullptr);
    ~ResponsiveLayoutManager();

    // Screen size and layout detection
    ScreenSize currentScreenSize() const { return m_currentScreenSize; }
    LayoutMode currentLayoutMode() const { return m_currentLayoutMode; }
    QSize currentScreenResolution() const;
    bool isMobileLayout() const { return m_currentScreenSize <= MobileLarge; }
    bool isTabletLayout() const { return m_currentScreenSize == Tablet; }
    bool isDesktopLayout() const { return m_currentScreenSize >= Desktop; }
    bool isLargeLayout() const { return m_currentScreenSize >= Large; }
    bool isUltraWideLayout() const { return m_currentScreenSize >= XLarge; }

    // Layout configuration
    void setLayoutMode(LayoutMode mode);
    void setAdaptiveMode(bool enabled) { m_adaptiveMode = enabled; }
    bool adaptiveMode() const { return m_adaptiveMode; }

    // Grid layout management
    void setGridWidget(QWidget* gridWidget);
    void setOptimalColumns(int itemWidth, int minColumns = 1, int maxColumns = 10);
    int calculateOptimalColumns(int containerWidth, int itemWidth, int spacing = 8) const;
    void updateGridLayout();

    // Splitter management
    void addSplitter(const QString& name, QSplitter* splitter);
    void setSplitterSizes(const QString& name, const QList<int>& sizes);
    QList<int> getSplitterSizes(const QString& name) const;
    void setSplitterCollapsible(const QString& name, int index, bool collapsible);

    // Panel management
    void addPanel(const QString& name, QWidget* panel, PanelState initialState = Visible);
    void setPanelState(const QString& name, PanelState state, bool animated = true);
    PanelState getPanelState(const QString& name) const;
    void togglePanel(const QString& name, bool animated = true);

    // Animation settings
    void setAnimationsEnabled(bool enabled) { m_animationsEnabled = enabled; }
    bool animationsEnabled() const { return m_animationsEnabled; }
    void setAnimationDuration(int duration) { m_animationDuration = duration; }
    int animationDuration() const { return m_animationDuration; }
    void animateLayoutTransition(LayoutMode fromMode, LayoutMode toMode);

    // Layout utilities
    QMargins getOptimalMargins() const;
    int getOptimalSpacing() const;
    int getOptimalItemSize() const;
    QSize getOptimalThumbnailSize() const;
    int getOptimalCollapsedWidth() const;
    int getOptimalExpandedWidth() const;
    int getOptimalVisibleWidth() const;

    // Enhanced responsive breakpoints (modern standards)
    static int getMobileBreakpoint() { return 640; }      // Small phones
    static int getMobileLargeBreakpoint() { return 768; } // Large phones
    static int getTabletBreakpoint() { return 1024; }     // Tablets
    static int getDesktopBreakpoint() { return 1366; }    // Laptops
    static int getLargeBreakpoint() { return 1920; }      // Desktops
    static int getXLargeBreakpoint() { return 2560; }     // Large monitors

    // Utility methods for breakpoint checking
    static ScreenSize getScreenSizeForWidth(int width);
    static LayoutMode getOptimalLayoutModeForScreenSize(ScreenSize screenSize);
    static int getOptimalColumnsForScreenSize(ScreenSize screenSize);
    static int getOptimalSpacingForScreenSize(ScreenSize screenSize);
    static QMargins getOptimalMarginsForScreenSize(ScreenSize screenSize);

public slots:
    void onScreenSizeChanged();
    void onWindowResized(const QSize& newSize);
    void updateLayout();
    void optimizeForCurrentScreen();
    void resetToDefaults();

signals:
    void screenSizeChanged(ScreenSize newSize, ScreenSize oldSize);
    void layoutModeChanged(LayoutMode newMode, LayoutMode oldMode);
    void layoutUpdated();
    void panelStateChanged(const QString& panelName, PanelState newState);
    void splitterSizesChanged(const QString& splitterName, const QList<int>& sizes);
    void initializationCompleted(ScreenSize screenSize, LayoutMode layoutMode);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onAnimationFinished();
    void onSplitterMoved();
    void checkScreenSize();
    void onLayoutAnimationFinished();
    void onStateAnimationFinished();
    void onScreenAdded(QScreen* screen);
    void onScreenRemoved(QScreen* screen);
    void onPrimaryScreenChanged(QScreen* screen);
    void updatePerformanceMetrics();

private:
    // Layout detection and calculation
    ScreenSize detectScreenSize() const;
    LayoutMode calculateOptimalLayoutMode() const;
    LayoutConfig createLayoutConfig() const;
    void applyLayoutConfig(const LayoutConfig& config);

    // Animation helpers
    void animatePanelTransition(QWidget* panel, PanelState fromState, PanelState toState);
    void animateSplitterResize(QSplitter* splitter, const QList<int>& newSizes);
    void animateLayoutTransition();
    void createMajorLayoutTransition(LayoutMode fromMode, LayoutMode toMode);
    void createMinorLayoutTransition(LayoutMode fromMode, LayoutMode toMode);
    QPropertyAnimation* createPanelSizeAnimation(QWidget* panel, PanelState targetState);

    // Layout application
    void applyGridLayout();
    void applySplitterLayout();
    void applyPanelLayout();
    void applySpacingAndMargins();

    // Utility methods
    void connectSignals();
    void saveCurrentLayout();
    void restoreLayout();
    void initializeResponsiveStates();

    // Core components
    QWidget* m_mainWidget;
    QWidget* m_gridWidget;

    // Layout state
    ScreenSize m_currentScreenSize;
    LayoutMode m_currentLayoutMode;
    LayoutConfig m_currentConfig;
    bool m_adaptiveMode;

    // Managed components
    QHash<QString, QSplitter*> m_splitters;
    QHash<QString, QWidget*> m_panels;
    QHash<QString, PanelState> m_panelStates;
    QHash<QString, QList<int>> m_splitterSizes;

    // Animation system
    bool m_animationsEnabled;
    int m_animationDuration;
    QHash<QWidget*, QPropertyAnimation*> m_activeAnimations;
    QParallelAnimationGroup* m_layoutAnimationGroup;
    QSequentialAnimationGroup* m_stateAnimationGroup;
    QEasingCurve m_animationEasingCurve;

    // Screen monitoring
    QTimer* m_screenCheckTimer;
    QTimer* m_performanceTimer;
    QSize m_lastScreenSize;

    // Layout history for undo/redo
    QList<LayoutConfig> m_layoutHistory;
    int m_currentHistoryIndex;

    // Enhanced mode flags
    bool m_touchMode;
    bool m_highPerformanceMode;
    bool m_accessibilityMode;
    bool m_debugMode;
    bool m_performanceMonitoringEnabled;
    int m_resizeDebounceTime;

    // Constants
    static constexpr int DEFAULT_ANIMATION_DURATION = 300;
    static constexpr int SCREEN_CHECK_INTERVAL = 1000; // 1 second
    static constexpr int MIN_PANEL_WIDTH = 200;
    static constexpr int MIN_PANEL_HEIGHT = 150;
    static constexpr int DEFAULT_SPACING = 8;
    static constexpr int MOBILE_SPACING = 4;
    static constexpr int DESKTOP_SPACING = 12;
};

/**
 * @brief Helper class for responsive widgets
 */
class ResponsiveWidget : public QObject {
    Q_OBJECT

public:
    explicit ResponsiveWidget(QWidget* widget, ResponsiveLayoutManager* manager,
                              QObject* parent = nullptr);
    ~ResponsiveWidget();

    void setBreakpointBehavior(ResponsiveLayoutManager::ScreenSize breakpoint,
                               const QVariantMap& properties);
    void setAdaptiveProperty(const QString& property, const QVariantMap& values);

private slots:
    void onScreenSizeChanged(ResponsiveLayoutManager::ScreenSize newSize,
                             ResponsiveLayoutManager::ScreenSize oldSize);

private:
    void applyPropertiesForScreenSize(ResponsiveLayoutManager::ScreenSize screenSize);

    QWidget* m_widget;
    ResponsiveLayoutManager* m_manager;
    QHash<ResponsiveLayoutManager::ScreenSize, QVariantMap> m_breakpointProperties;
    QHash<QString, QVariantMap> m_adaptiveProperties;
};

#endif // RESPONSIVELAYOUTMANAGER_H
