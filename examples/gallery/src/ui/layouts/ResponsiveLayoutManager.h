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

#include <QObject>
#include <QWidget>
#include <QSplitter>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>
#include <QTimer>
#include <QScreen>
#include <QApplication>
#include <QResizeEvent>
#include <QHash>
#include <QVariant>
#include <QMargins>
#include <QSize>

/**
 * @brief Advanced responsive layout management for the Gallery application
 */
class ResponsiveLayoutManager : public QObject
{
    Q_OBJECT

public:
    enum ScreenSize {
        Mobile = 0,     // < 768px width
        Tablet = 1,     // 768-1024px width
        Desktop = 2,    // 1024-1440px width
        Large = 3,      // 1440-1920px width
        XLarge = 4      // > 1920px width
    };
    Q_ENUM(ScreenSize)

    enum LayoutMode {
        CompactMode = 0,    // Single column, minimal spacing
        StandardMode = 1,   // Two columns, standard spacing
        WideMode = 2,       // Three columns, generous spacing
        UltraWideMode = 3   // Four+ columns, maximum spacing
    };
    Q_ENUM(LayoutMode)

    enum PanelState {
        Hidden = 0,
        Collapsed = 1,
        Visible = 2,
        Expanded = 3
    };
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
    bool isMobileLayout() const { return m_currentScreenSize == Mobile; }
    bool isTabletLayout() const { return m_currentScreenSize == Tablet; }
    bool isDesktopLayout() const { return m_currentScreenSize >= Desktop; }

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

    // Layout utilities
    QMargins getOptimalMargins() const;
    int getOptimalSpacing() const;
    int getOptimalItemSize() const;
    QSize getOptimalThumbnailSize() const;

    // Responsive breakpoints
    static int getMobileBreakpoint() { return 768; }
    static int getTabletBreakpoint() { return 1024; }
    static int getDesktopBreakpoint() { return 1440; }
    static int getLargeBreakpoint() { return 1920; }

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

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onAnimationFinished();
    void onSplitterMoved();
    void checkScreenSize();

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

    // Layout application
    void applyGridLayout();
    void applySplitterLayout();
    void applyPanelLayout();
    void applySpacingAndMargins();

    // Utility methods
    void connectSignals();
    void saveCurrentLayout();
    void restoreLayout();

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
    
    // Screen monitoring
    QTimer* m_screenCheckTimer;
    QSize m_lastScreenSize;
    
    // Layout history for undo/redo
    QList<LayoutConfig> m_layoutHistory;
    int m_currentHistoryIndex;
    
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
class ResponsiveWidget : public QObject
{
    Q_OBJECT

public:
    explicit ResponsiveWidget(QWidget* widget, ResponsiveLayoutManager* manager, QObject* parent = nullptr);
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
