/**
 * QtLucide Gallery Application - Enhanced Icon Grid Widget
 *
 * A completely reimplemented, high-performance widget for displaying large
 * collections of icons with advanced features:
 *
 * - Ultra-efficient virtual scrolling with viewport culling
 * - Smooth animations and transitions
 * - Advanced selection modes (single, multiple, rubber band)
 * - Drag and drop support
 * - Customizable layouts (grid, list, compact)
 * - Real-time filtering and search highlighting
 * - Accessibility support with keyboard navigation
 * - Performance monitoring and optimization
 * - Lazy loading with progressive rendering
 * - Memory-efficient caching system
 */

#ifndef ICONGRIDWIDGET_H
#define ICONGRIDWIDGET_H

#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QCache>
#include <QDrag>
#include <QEasingCurve>
#include <QElapsedTimer>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHash>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLabel>
#include <QListView>
#include <QMimeData>
#include <QModelIndex>
#include <QMouseEvent>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmapCache>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QRubberBand>
#include <QRunnable>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStringList>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QThreadPool>
#include <QTimer>
#include <QVBoxLayout>
#include <QVariant>
#include <QWheelEvent>
#include <QWidget>

#include "core/managers/IconMetadataManager.h"
#include "core/models/IconItem.h"
#include <QtLucide/QtLucide.h>
#include <memory>

// Forward declarations
class IconGridDelegate;
class IconRenderTask;
class IconGridViewport;
class FavoritesManager;

/**
 * @brief Enhanced model for icon data with advanced caching and virtual scrolling
 */
class IconGridModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit IconGridModel(QObject* parent = nullptr);
    ~IconGridModel();

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    // Enhanced custom methods
    void setIconNames(const QStringList& iconNames);
    void setIconSize(int size);
    void setIconSize(const QSize& size); // Added for compatibility
    void setMetadataManager(IconMetadataManager* manager);
    void setLucide(lucide::QtLucide* lucide);
    void setShowIconNames(bool show);
    void setHighlightSearchTerm(const QString& term);
    void setFavorites(const QStringList& favorites);
    void setFilteredIcons(const QStringList& iconNames); // Added for compatibility
    void clearFilter();                                  // Added for compatibility

    QString iconNameAt(int index) const;
    QStringList iconNames() const;
    QStringList selectedIconNames() const;
    int iconSize() const { return m_iconSize; }
    bool showIconNames() const { return m_showIconNames; }

    // Selection management
    void selectIcon(const QString& iconName);
    void deselectIcon(const QString& iconName);
    void clearSelection();
    void selectAll();
    bool isSelected(const QString& iconName) const;
    int selectionCount() const;

    // Performance optimization
    void preloadRange(int start, int count);
    void clearCache();
    void setCacheLimit(int limit);

    // Performance metrics
    int getCacheHits() const { return m_cacheHits; }
    int getCacheMisses() const { return m_cacheMisses; }

    // Custom roles
    enum CustomRoles {
        IconNameRole = Qt::UserRole + 1,
        IconMetadataRole,
        IsFavoriteRole,
        IsSelectedRole,
        IsHighlightedRole,
        RenderStateRole,
        CacheKeyRole
    };

public slots:
    void refreshData();
    void updateFavorites();
    void updateSelection();
    void invalidateCache();

signals:
    void iconSelectionChanged();
    void dataPreloaded(int start, int count);
    void cacheUpdated();
    void favoritesUpdated();
    void filterCleared();                 // Added for compatibility
    void filteredIconsChanged(int count); // Added for compatibility
    void itemCountChanged(int count);

private:
    void updateSearchHighlights();
    void precomputeDisplayData();
    QString createCacheKey(const QString& iconName, int size) const;
    QString formatIconName(const QString& name) const;
    QPixmap getIconPixmap(const QString& iconName, int size) const;

    QStringList m_iconNames;
    QSet<QString> m_selectedIcons;
    int m_iconSize;
    bool m_showIconNames;
    QString m_highlightTerm;
    IconMetadataManager* m_metadataManager;
    lucide::QtLucide* m_lucide;
    FavoritesManager* m_favoritesManager;
    QSet<QString> m_favorites;

    // Filtering system
    QStringList m_filteredIcons; // Added for compatibility
    bool m_isFiltered;           // Added for compatibility

    // Caching system
    mutable QCache<QString, QPixmap> m_pixmapCache;
    mutable QHash<QString, QVariant> m_metadataCache;
    int m_cacheLimit;

    // Performance monitoring
    mutable QElapsedTimer m_performanceTimer;
    mutable int m_cacheHits;
    mutable int m_cacheMisses;
};

/**
 * @brief Enhanced delegate for rendering icon items with advanced visual effects
 */
class IconGridDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit IconGridDelegate(QObject* parent = nullptr);
    ~IconGridDelegate();

    // QStyledItemDelegate interface
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

    // Enhanced custom methods
    void setIconSize(int size);
    void setIconSize(const QSize& size); // Added for compatibility
    void setShowIconNames(bool show);
    void setAnimationsEnabled(bool enabled);
    void setHighlightColor(const QColor& color);
    void setSelectionColor(const QColor& color);
    void setHoverEffectsEnabled(bool enabled);
    void setDropShadowEnabled(bool enabled);
    void setDisplayMode(IconItem::DisplayMode mode);

    int iconSize() const { return m_iconSize; }
    bool showIconNames() const { return m_showIconNames; }
    bool animationsEnabled() const { return m_animationsEnabled; }

    // Layout calculation
    QRect getIconRect(const QRect& itemRect) const;
    QRect getTextRect(const QRect& itemRect) const;
    QRect getFavoriteButtonRect(const QRect& itemRect) const;

public slots:
    void onAnimationFinished();
    void updateHoverState();
    void startHoverAnimation(const QString& iconName, bool entering);

signals:
    void iconClicked(const QString& iconName, const QPoint& position);
    void iconDoubleClicked(const QString& iconName);
    void iconHovered(const QString& iconName);
    void favoriteToggled(const QString& iconName);

private:
    void paintBackground(QPainter* painter, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const;
    void paintIcon(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;
    void paintText(QPainter* painter, const QStyleOptionViewItem& option,
                   const QModelIndex& index) const;
    void paintOverlay(QPainter* painter, const QStyleOptionViewItem& option,
                      const QModelIndex& index) const;
    void paintSelection(QPainter* painter, const QStyleOptionViewItem& option) const;
    void paintHover(QPainter* painter, const QStyleOptionViewItem& option) const;
    void paintFavoriteButton(QPainter* painter, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const;
    void paintHighlight(QPainter* painter, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const;

    QString formatIconName(const QString& name) const;
    QFont getTextFont() const;
    QColor getTextColor(const QStyleOptionViewItem& option, bool isSelected) const;

    int m_iconSize;
    bool m_showIconNames;
    bool m_animationsEnabled;
    bool m_hoverEffectsEnabled;
    bool m_dropShadowEnabled;
    QColor m_highlightColor;
    QColor m_selectionColor;
    IconItem::DisplayMode m_displayMode;

    // Animation support
    mutable QHash<QString, QPropertyAnimation*> m_animations;
    mutable QHash<QString, qreal> m_hoverOpacity;

    // Constants
    static constexpr int DEFAULT_ICON_SIZE = 64;
    static constexpr int PADDING = 8;
    static constexpr int TEXT_HEIGHT = 20;
    static constexpr int FAVORITE_BUTTON_SIZE = 16;
    static constexpr int ANIMATION_DURATION = 200;
};

/**
 * @brief Enhanced main widget for displaying icons with advanced features
 */
class IconGridWidget : public QWidget {
    Q_OBJECT

public:
    enum ViewMode { GridView = 0, ListView = 1, CompactView = 2, DetailedView = 3 };
    Q_ENUM(ViewMode)

    enum SelectionMode {
        NoSelection = 0,
        SingleSelection = 1,
        MultiSelection = 2,
        ExtendedSelection = 3
    };
    Q_ENUM(SelectionMode)

    enum ScrollMode { ScrollPerItem = 0, ScrollPerPixel = 1 };
    Q_ENUM(ScrollMode)

    explicit IconGridWidget(QWidget* parent = nullptr);
    ~IconGridWidget();

    // Core functionality
    void setIconNames(const QStringList& iconNames);
    void setLucide(lucide::QtLucide* lucide);
    void setMetadataManager(IconMetadataManager* manager);

    // Appearance and layout
    void setIconSize(int size);
    void setViewMode(ViewMode mode);
    void setShowIconNames(bool show);
    void setItemSpacing(int spacing);
    void setItemMargins(int margins);
    void setColumnsCount(int columns); // -1 for auto-calculate

    // Selection
    void setSelectionMode(SelectionMode mode);
    void selectIcon(const QString& iconName);
    void deselectIcon(const QString& iconName);
    void clearSelection();
    void selectAll();
    QStringList selectedIcons() const;
    QString currentIcon() const;
    bool isSelected(const QString& iconName) const;

    // Filtering and search
    void setFilter(const QString& filter);
    void setHighlightSearchTerm(const QString& term);
    void applyFilter(const IconFilterCriteria& criteria);
    void clearFilter();

    // Performance and optimization
    void setVirtualScrollingEnabled(bool enabled);
    void setAnimationsEnabled(bool enabled);
    void setLazyLoadingEnabled(bool enabled);
    void setCacheLimit(int limit);
    void preloadVisibleItems();
    void setSpacing(int spacing);
    void setMargin(int margin);
    void setFilteredIcons(const QStringList& icons);

    // Getters
    QStringList iconNames() const;
    int iconSize() const;
    ViewMode viewMode() const;
    bool showIconNames() const;
    SelectionMode selectionMode() const;
    int itemCount() const;
    int visibleItemCount() const;

    // Navigation and scrolling
    void scrollToIcon(const QString& iconName);
    void scrollToTop();
    void scrollToBottom();
    void ensureIconVisible(const QString& iconName);

    // Drag and drop
    void setDragEnabled(bool enabled);
    void setDropEnabled(bool enabled);
    bool dragEnabled() const;
    bool dropEnabled() const;

    // Context menu
    void setContextMenuPolicy(Qt::ContextMenuPolicy policy);
    void showContextMenu(const QPoint& position);

public slots:
    void refresh() { refreshIcons(); } // Alias for refreshIcons
    void refreshIcons();
    void updateIconSizes();
    void updateFavorites();
    void updateSelection();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void fitToWindow();
    void onFilterChanged();
    void onSearchHighlightChanged(const QString& term);

signals:
    void iconSelected(const QString& iconName);
    void iconDoubleClicked(const QString& iconName);
    void iconHovered(const QString& iconName);
    void iconContextMenuRequested(const QString& iconName, const QPoint& globalPos);
    void selectionChanged(const QStringList& selectedIcons);
    void favoriteToggled(const QString& iconName, bool favorite);
    void iconSizeChanged(int size);
    void filteredIconsChanged(int count); // Added for compatibility
    void filterCleared();                 // Added for compatibility
    void viewModeChanged(ViewMode mode);
    void visibleRangeChanged(int startIndex, int endIndex);
    void scrollPositionChanged(int value, int maximum);
    void nearBottomReached();
    void itemCountChanged(int count);
    void visibleItemCountChanged(int count);
    void loadingStarted();
    void loadingFinished();
    void performanceMetricsUpdated(const QVariantMap& metrics);
    void favoritesUpdated(int count);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private slots:
    void onIconClicked(const QString& iconName, const QPoint& position);
    void onIconDoubleClicked(const QString& iconName);
    void onIconHovered(const QString& iconName);
    void onFavoriteToggled(const QString& iconName);
    void onSelectionChanged();
    void onModelDataChanged();
    void onScrollValueChanged();
    void onUpdateTimer();
    void onAnimationFinished();
    void updateVisibleItems();
    void updatePerformanceMetrics();
    void onItemEntered(const QModelIndex& index);
    void onItemClicked(const QModelIndex& index);
    void onItemDoubleClicked(const QModelIndex& index);
    void scheduleUpdate();

private:
    // Enhanced UI setup methods
    void setupUI();
    void setupHeader();
    void setupViewArea();
    void setupFooter();
    void setupModel();
    void setupView();
    void setupScrollArea();
    void setupAnimations();
    void setupPerformanceMonitoring();
    void applyModernStyling();

    // Layout and rendering
    void updateViewSettings();
    void updateItemSize();
    void updateLayout();
    void calculateOptimalColumns();
    void calculateItemPositions();
    void updateViewport();
    void updateStylingForViewMode();
    void updateEmptyState();

    // Selection and interaction
    void handleItemClick(const QString& iconName, const QPoint& position);
    void handleItemDoubleClick(const QString& iconName);
    void handleItemHover(const QString& iconName);
    void startDrag(const QString& iconName, const QPoint& position);

    // Performance optimization
    void optimizeRendering();
    void preloadVisibleRange();
    void cullInvisibleItems();
    void updateRenderCache();

    // Animation helpers
    void animateSelection(const QString& iconName);
    void animateHover(const QString& iconName, bool entering);
    void animateZoom(int fromSize, int toSize);

    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;

    // Enhanced UI components
    QVBoxLayout* m_layout;
    QScrollArea* m_scrollArea;
    QWidget* m_viewport;
    QVBoxLayout* m_viewportLayout;
    QListView* m_listView;
    QLabel* m_emptyLabel;
    QWidget* m_emptyWidget;
    QProgressBar* m_loadingProgress;
    QLabel* m_statusLabel;
    QWidget* m_gridView;                  // Grid view widget
    FavoritesManager* m_favoritesManager; // Favorites manager

    // Model and delegate
    std::unique_ptr<IconGridModel> m_model;
    std::unique_ptr<IconGridDelegate> m_delegate;

    // Enhanced settings and state
    int m_iconSize;
    int m_margin; // Added for compatibility
    ViewMode m_viewMode;
    SelectionMode m_selectionMode;
    ScrollMode m_scrollMode;
    bool m_showIconNames;
    bool m_virtualScrollingEnabled;
    bool m_animationsEnabled;
    bool m_lazyLoadingEnabled;
    bool m_dragEnabled;
    bool m_dropEnabled;
    QString m_currentIcon;
    QString m_hoveredIcon;
    QString m_searchHighlight;

    // Layout properties
    int m_itemSpacing;
    int m_itemMargins;
    int m_columnsCount;
    QSize m_itemSize;
    QSize m_viewportSize;
    int m_spacing; // Grid spacing

    // Performance optimization
    QTimer* m_updateTimer;
    QTimer* m_performanceTimer;
    bool m_needsUpdate;
    bool m_needsLayout;
    QElapsedTimer m_renderTimer;
    QVariantMap m_performanceMetrics;

    // Animation system
    QPropertyAnimation* m_selectionAnimation;
    QPropertyAnimation* m_hoverAnimation;
    QPropertyAnimation* m_zoomAnimation;
    QHash<QString, QPropertyAnimation*> m_itemAnimations;

    // Caching and optimization
    QPixmapCache* m_pixmapCache;
    QHash<QString, QRect> m_itemRects;
    QRect m_visibleRect;
    QStringList m_visibleItems;

    // Enhanced configuration
    static constexpr int DEFAULT_ICON_SIZE = 64;
    static constexpr int MIN_ICON_SIZE = 16;
    static constexpr int MAX_ICON_SIZE = 512;
    static constexpr int ZOOM_STEP = 8;
    static constexpr int DEFAULT_ITEM_SPACING = 8;
    static constexpr int DEFAULT_ITEM_MARGINS = 4;
    static constexpr int UPDATE_DELAY = 50;                  // ms
    static constexpr int PERFORMANCE_UPDATE_INTERVAL = 1000; // ms
    static constexpr int ANIMATION_DURATION = 200;           // ms
    static constexpr int HOVER_ANIMATION_DURATION = 150;     // ms
    static constexpr int PRELOAD_BUFFER = 50;                // items
    static constexpr int MAX_VISIBLE_ITEMS = 1000;           // for performance
};

#endif // ICONGRIDWIDGET_H
