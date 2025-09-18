/**
 * QtLucide Gallery Application - Modern Icon Grid Widget
 *
 * A completely redesigned icon grid widget with modern visual effects:
 * - Smooth hover animations with scale and glow effects
 * - Beautiful selection states with animated borders
 * - Gradient backgrounds and subtle shadows
 * - Responsive grid layout with optimal spacing
 * - Virtual scrolling for performance
 * - Smooth transitions between view modes
 * - Advanced visual feedback for user interactions
 * - Theme-aware styling with automatic color adaptation
 */

#ifndef MODERNICONGRIDWIDGET_H
#define MODERNICONGRIDWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include <QGraphicsColorizeEffect>
#include <QEasingCurve>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QPixmapCache>
#include <QHash>
#include <QSet>
#include <QStringList>
#include <QVariantAnimation>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QLinearGradient>
#include <QRadialGradient>

#include <QtLucide/QtLucide.h>
#include "../themes/ThemeManager.h"
#include "../layouts/ResponsiveLayoutManager.h"

// Forward declarations
class ModernIconItem;

/**
 * @brief Modern icon item widget with advanced visual effects
 */
class ModernIconItem : public QFrame
{
    Q_OBJECT

public:
    enum State {
        Normal = 0,
        Hovered = 1,
        Selected = 2,
        Pressed = 3,
        Favorite = 4
    };
    Q_ENUM(State)

    enum AnimationType {
        ScaleAnimation = 0,
        GlowAnimation = 1,
        BorderAnimation = 2,
        BackgroundAnimation = 3,
        ShadowAnimation = 4
    };
    Q_ENUM(AnimationType)

    explicit ModernIconItem(const QString& iconName, QWidget* parent = nullptr);
    ~ModernIconItem();

    // Icon properties
    void setIconName(const QString& iconName);
    QString iconName() const { return m_iconName; }

    void setIconSize(int size);
    int iconSize() const { return m_iconSize; }

    void setShowIconName(bool show);
    bool showIconName() const { return m_showIconName; }

    // State management
    void setState(State state, bool animated = true);
    State currentState() const { return m_currentState; }

    void setSelected(bool selected, bool animated = true);
    bool isSelected() const { return m_currentState == Selected; }

    void setFavorite(bool favorite, bool animated = true);
    bool isFavorite() const { return m_isFavorite; }

    // Visual effects
    void setHoverEffectsEnabled(bool enabled) { m_hoverEffectsEnabled = enabled; }
    bool hoverEffectsEnabled() const { return m_hoverEffectsEnabled; }

    void setAnimationsEnabled(bool enabled) { m_animationsEnabled = enabled; }
    bool animationsEnabled() const { return m_animationsEnabled; }

    void setGlowEnabled(bool enabled) { m_glowEnabled = enabled; }
    bool glowEnabled() const { return m_glowEnabled; }

    // Theme integration
    void applyTheme();
    void setCustomColors(const QHash<QString, QColor>& colors);

signals:
    void clicked(const QString& iconName);
    void doubleClicked(const QString& iconName);
    void rightClicked(const QString& iconName, const QPoint& globalPos);
    void hoverEntered(const QString& iconName);
    void hoverLeft(const QString& iconName);
    void favoriteToggled(const QString& iconName, bool favorite);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onAnimationFinished();
    void onThemeChanged();
    void updateVisualEffects();

private:
    // Painting methods
    void paintBackground(QPainter* painter, const QRect& rect);
    void paintIcon(QPainter* painter, const QRect& rect);
    void paintIconName(QPainter* painter, const QRect& rect);
    void paintSelection(QPainter* painter, const QRect& rect);
    void paintHover(QPainter* painter, const QRect& rect);
    void paintGlow(QPainter* painter, const QRect& rect);
    void paintFavoriteIndicator(QPainter* painter, const QRect& rect);

    // Animation methods
    void startAnimation(AnimationType type, const QVariant& startValue, const QVariant& endValue, int duration = 200);
    void stopAnimation(AnimationType type);
    void stopAllAnimations();

    // Color and style methods
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getTextColor() const;
    QColor getGlowColor() const;
    QLinearGradient getBackgroundGradient() const;
    QRadialGradient getGlowGradient() const;

    // Geometry methods
    QRect getIconRect() const;
    QRect getTextRect() const;
    QRect getFavoriteRect() const;

    // Core properties
    QString m_iconName;
    int m_iconSize;
    bool m_showIconName;
    State m_currentState;
    bool m_isFavorite;
    bool m_isPressed;

    // Visual effects
    bool m_hoverEffectsEnabled;
    bool m_animationsEnabled;
    bool m_glowEnabled;
    qreal m_hoverOpacity;
    qreal m_selectionOpacity;
    qreal m_glowOpacity;
    qreal m_scaleValue;

    // Animation system
    QHash<AnimationType, QPropertyAnimation*> m_animations;
    QHash<AnimationType, QVariant> m_animationValues;

    // Theme and colors
    QHash<QString, QColor> m_customColors;
    ThemeAwareWidget* m_themeWidget;

    // Cached resources
    QPixmap m_iconPixmap;
    bool m_pixmapCacheValid;

    // Constants
    static constexpr int DEFAULT_ICON_SIZE = 64;
    static constexpr int PADDING = 8;
    static constexpr int TEXT_HEIGHT = 20;
    static constexpr int FAVORITE_SIZE = 16;
    static constexpr int ANIMATION_DURATION = 200;
    static constexpr qreal HOVER_SCALE = 1.05;
    static constexpr qreal SELECTION_SCALE = 1.02;
    static constexpr qreal GLOW_RADIUS = 10.0;
};

/**
 * @brief Modern icon grid widget with advanced layout and visual effects
 */
class ModernIconGridWidget : public QWidget
{
    Q_OBJECT

public:
    enum ViewMode {
        GridView = 0,
        ListView = 1,
        CompactView = 2,
        DetailedView = 3
    };
    Q_ENUM(ViewMode)

    enum SelectionMode {
        NoSelection = 0,
        SingleSelection = 1,
        MultiSelection = 2,
        ExtendedSelection = 3
    };
    Q_ENUM(SelectionMode)

    explicit ModernIconGridWidget(QWidget* parent = nullptr);
    ~ModernIconGridWidget();

    // Icon management
    void setIconNames(const QStringList& iconNames);
    QStringList iconNames() const { return m_iconNames; }
    void addIcon(const QString& iconName);
    void removeIcon(const QString& iconName);
    void clearIcons();

    // Layout and appearance
    void setViewMode(ViewMode mode);
    ViewMode viewMode() const { return m_viewMode; }

    void setIconSize(int size);
    int iconSize() const { return m_iconSize; }

    void setShowIconNames(bool show);
    bool showIconNames() const { return m_showIconNames; }

    void setItemSpacing(int spacing);
    int itemSpacing() const { return m_itemSpacing; }

    void setItemMargins(const QMargins& margins);
    QMargins itemMargins() const { return m_itemMargins; }

    // Selection
    void setSelectionMode(SelectionMode mode);
    SelectionMode selectionMode() const { return m_selectionMode; }

    void selectIcon(const QString& iconName, bool selected = true);
    void selectAll();
    void clearSelection();
    QStringList selectedIcons() const;
    bool isSelected(const QString& iconName) const;

    // Filtering and search
    void setFilter(const QString& filter);
    QString filter() const { return m_filter; }
    void setFilteredIcons(const QStringList& iconNames);

    // Visual effects
    void setAnimationsEnabled(bool enabled);
    bool animationsEnabled() const { return m_animationsEnabled; }

    void setHoverEffectsEnabled(bool enabled);
    bool hoverEffectsEnabled() const { return m_hoverEffectsEnabled; }

    void setGlowEffectsEnabled(bool enabled);
    bool glowEffectsEnabled() const { return m_glowEffectsEnabled; }

    // Performance
    void setVirtualScrollingEnabled(bool enabled);
    bool virtualScrollingEnabled() const { return m_virtualScrollingEnabled; }

    // Integration
    void setLucide(lucide::QtLucide* lucide);
    void setThemeManager(ThemeManager* themeManager);
    void setResponsiveLayoutManager(ResponsiveLayoutManager* layoutManager);

public slots:
    void refreshLayout();
    void updateTheme();
    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void iconClicked(const QString& iconName);
    void iconDoubleClicked(const QString& iconName);
    void iconRightClicked(const QString& iconName, const QPoint& globalPos);
    void iconHovered(const QString& iconName);
    void selectionChanged(const QStringList& selectedIcons);
    void favoriteToggled(const QString& iconName, bool favorite);
    void viewModeChanged(ViewMode mode);
    void iconSizeChanged(int size);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onIconClicked(const QString& iconName);
    void onIconDoubleClicked(const QString& iconName);
    void onIconRightClicked(const QString& iconName, const QPoint& globalPos);
    void onIconHovered(const QString& iconName);
    void onFavoriteToggled(const QString& iconName, bool favorite);
    void onLayoutChanged();
    void onThemeChanged();

private:
    void setupUI();
    void setupLayout();
    void setupScrollArea();
    void createIconItems();
    void updateLayout();
    void updateItemSizes();
    void calculateOptimalColumns();
    void applyViewMode();
    void applyTheme();

    // Core components
    lucide::QtLucide* m_lucide;
    ThemeManager* m_themeManager;
    ResponsiveLayoutManager* m_layoutManager;

    // UI components
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QGridLayout* m_gridLayout;

    // Icon items
    QStringList m_iconNames;
    QStringList m_filteredIconNames;
    QHash<QString, ModernIconItem*> m_iconItems;
    QSet<QString> m_selectedIcons;

    // Layout properties
    ViewMode m_viewMode;
    SelectionMode m_selectionMode;
    int m_iconSize;
    bool m_showIconNames;
    int m_itemSpacing;
    QMargins m_itemMargins;
    int m_columns;

    // Visual effects
    bool m_animationsEnabled;
    bool m_hoverEffectsEnabled;
    bool m_glowEffectsEnabled;
    bool m_virtualScrollingEnabled;

    // Filtering
    QString m_filter;

    // Performance
    QTimer* m_layoutUpdateTimer;
    bool m_layoutUpdatePending;

    // Constants
    static constexpr int DEFAULT_ICON_SIZE = 64;
    static constexpr int MIN_ICON_SIZE = 32;
    static constexpr int MAX_ICON_SIZE = 256;
    static constexpr int DEFAULT_SPACING = 8;
    static constexpr int ZOOM_STEP = 16;
    static constexpr int LAYOUT_UPDATE_DELAY = 50;
};

#endif // MODERNICONGRIDWIDGET_H
