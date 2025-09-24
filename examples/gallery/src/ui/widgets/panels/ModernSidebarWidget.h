/**
 * QtLucide Gallery Application - Modern Sidebar Widget
 *
 * A beautifully designed sidebar with:
 * - Collapsible category tree with smooth animations
 * - Modern visual design with hover effects and selection states
 * - Icon badges showing item counts
 * - Search integration with category filtering
 * - Resizable panels with minimum/maximum width constraints
 * - Theme-aware styling with automatic color adaptation
 * - Keyboard navigation and accessibility support
 * - Smooth transitions between expanded/collapsed states
 */

#ifndef MODERNSIDEBARWIDGET_H
#define MODERNSIDEBARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QSplitter>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QHash>
#include <QStringList>
#include <QVariant>
#include <QIcon>
#include <QPixmap>
#include <QBrush>
#include <QPen>
#include <QLinearGradient>
#include <QRadialGradient>

#include "../themes/ThemeManager.h"

// Forward declarations
class ModernCategoryTreeWidget;
class SidebarHeaderWidget;
class CategoryItemWidget;

/**
 * @brief Custom category item widget with modern styling
 */
class CategoryItemWidget : public QFrame
{
    Q_OBJECT

public:
    enum ItemState {
        Normal = 0,
        Hovered = 1,
        Selected = 2,
        Expanded = 3
    };
    Q_ENUM(ItemState)

    explicit CategoryItemWidget(const QString& categoryName, int itemCount = 0, QWidget* parent = nullptr);
    ~CategoryItemWidget();

    // Category properties
    void setCategoryName(const QString& name);
    QString categoryName() const { return m_categoryName; }
    
    void setItemCount(int count);
    int itemCount() const { return m_itemCount; }
    
    void setIcon(const QIcon& icon);
    QIcon icon() const { return m_icon; }

    // State management
    void setState(ItemState state, bool animated = true);
    ItemState currentState() const { return m_currentState; }
    
    void setSelected(bool selected, bool animated = true);
    bool isSelected() const { return m_currentState == Selected; }
    
    void setExpanded(bool expanded, bool animated = true);
    bool isExpanded() const { return m_expanded; }

    // Visual effects
    void setAnimationsEnabled(bool enabled) { m_animationsEnabled = enabled; }
    bool animationsEnabled() const { return m_animationsEnabled; }
    
    void setShowItemCount(bool show);
    bool showItemCount() const { return m_showItemCount; }

    // Theme integration
    void applyTheme();

signals:
    void clicked(const QString& categoryName);
    void doubleClicked(const QString& categoryName);
    void rightClicked(const QString& categoryName, const QPoint& globalPos);
    void expandToggled(const QString& categoryName, bool expanded);
    void hoverEntered(const QString& categoryName);
    void hoverLeft(const QString& categoryName);

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

private:
    void setupUI();
    void updateLayout();
    void startAnimation(const QString& property, const QVariant& startValue, const QVariant& endValue, int duration = 200);
    void stopAllAnimations();
    
    // Painting methods
    void paintBackground(QPainter* painter, const QRect& rect);
    void paintIcon(QPainter* painter, const QRect& rect);
    void paintText(QPainter* painter, const QRect& rect);
    void paintItemCount(QPainter* painter, const QRect& rect);
    void paintExpandIndicator(QPainter* painter, const QRect& rect);
    void paintSelection(QPainter* painter, const QRect& rect);
    void paintHover(QPainter* painter, const QRect& rect);
    
    // Color and style methods
    QColor getBackgroundColor() const;
    QColor getBorderColor() const;
    QColor getTextColor() const;
    QColor getCountColor() const;
    QLinearGradient getBackgroundGradient() const;
    
    // Geometry methods
    QRect getIconRect() const;
    QRect getTextRect() const;
    QRect getCountRect() const;
    QRect getExpandRect() const;

    // Core properties
    QString m_categoryName;
    int m_itemCount;
    QIcon m_icon;
    ItemState m_currentState;
    bool m_expanded;
    bool m_isPressed;

    // Visual effects
    bool m_animationsEnabled;
    bool m_showItemCount;
    qreal m_hoverOpacity;
    qreal m_selectionOpacity;
    qreal m_expandRotation;
    
    // Animation system
    QHash<QString, QPropertyAnimation*> m_animations;
    
    // Theme
    ThemeAwareWidget* m_themeWidget;
    
    // Constants
    static constexpr int DEFAULT_HEIGHT = 36;
    static constexpr int PADDING = 12;
    static constexpr int ICON_SIZE = 20;
    static constexpr int COUNT_BADGE_SIZE = 24;
    static constexpr int EXPAND_INDICATOR_SIZE = 16;
    static constexpr int ANIMATION_DURATION = 200;
};

/**
 * @brief Modern category tree widget with enhanced features
 */
class ModernCategoryTreeWidget : public QFrame
{
    Q_OBJECT

public:
    struct CategoryData {
        QString name;
        QString displayName;
        int itemCount;
        QIcon icon;
        QStringList subcategories;
        bool expanded;
        bool visible;
    };

    explicit ModernCategoryTreeWidget(QWidget* parent = nullptr);
    ~ModernCategoryTreeWidget();

    // Category management
    void setCategories(const QHash<QString, CategoryData>& categories);
    QHash<QString, CategoryData> categories() const { return m_categories; }
    void addCategory(const QString& name, const CategoryData& data);
    void removeCategory(const QString& name);
    void updateCategoryCount(const QString& name, int count);

    // Selection
    void setSelectedCategory(const QString& categoryName);
    QString selectedCategory() const { return m_selectedCategory; }
    void clearSelection();

    // Expansion
    void setExpandedCategories(const QStringList& categories);
    QStringList expandedCategories() const;
    void expandCategory(const QString& categoryName, bool expanded = true);
    void collapseAll();
    void expandAll();

    // Visual effects
    void setAnimationsEnabled(bool enabled);
    bool animationsEnabled() const { return m_animationsEnabled; }
    
    void setShowItemCounts(bool show);
    bool showItemCounts() const { return m_showItemCounts; }

    // Filtering
    void setFilter(const QString& filter);
    QString filter() const { return m_filter; }

    // Theme integration
    void applyTheme();

public slots:
    void refreshCategories();
    void updateLayout();

signals:
    void categorySelected(const QString& categoryName);
    void categoryExpanded(const QString& categoryName, bool expanded);
    void categoryCountChanged(const QString& categoryName, int count);
    void filterChanged(const QString& filter);
    void categorySelectionChanged(const QStringList& categories);
    void tagSelectionChanged(const QStringList& tags);
    void selectionChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onCategoryClicked(const QString& categoryName);
    void onCategoryExpanded(const QString& categoryName, bool expanded);
    void onCategoryHovered(const QString& categoryName);
    void onLayoutUpdateTimer();

private:
    void setupUI();
    void createCategoryItems();
    void updateCategoryVisibility();
    void applyCategoryFilter();
    void scheduleLayoutUpdate();

    // UI components
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QVBoxLayout* m_contentLayout;
    
    // Category items
    QHash<QString, CategoryData> m_categories;
    QHash<QString, CategoryItemWidget*> m_categoryItems;
    QString m_selectedCategory;
    
    // Visual effects
    bool m_animationsEnabled;
    bool m_showItemCounts;
    
    // Filtering
    QString m_filter;
    
    // Layout optimization
    QTimer* m_layoutUpdateTimer;
    bool m_layoutUpdatePending;
    
    // Theme
    ThemeAwareWidget* m_themeWidget;
    
    // Constants
    static constexpr int LAYOUT_UPDATE_DELAY = 50;
};

/**
 * @brief Sidebar header with title and controls
 */
class SidebarHeaderWidget : public QFrame
{
    Q_OBJECT

public:
    explicit SidebarHeaderWidget(QWidget* parent = nullptr);
    ~SidebarHeaderWidget();

    // Header properties
    void setTitle(const QString& title);
    QString title() const;
    
    void setSubtitle(const QString& subtitle);
    QString subtitle() const;

    // Controls
    void setShowCollapseButton(bool show);
    bool showCollapseButton() const { return m_showCollapseButton; }
    
    void setShowSettingsButton(bool show);
    bool showSettingsButton() const { return m_showSettingsButton; }
    
    void setCollapsed(bool collapsed);
    bool isCollapsed() const { return m_collapsed; }

    // Theme integration
    void applyTheme();

signals:
    void collapseToggled(bool collapsed);
    void settingsClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onCollapseClicked();
    void onSettingsClicked();

private:
    void setupUI();
    void updateLayout();

    // UI components
    QHBoxLayout* m_layout;
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
    QPushButton* m_collapseButton;
    QPushButton* m_settingsButton;
    
    // Properties
    QString m_title;
    QString m_subtitle;
    bool m_showCollapseButton;
    bool m_showSettingsButton;
    bool m_collapsed;
    
    // Theme
    ThemeAwareWidget* m_themeWidget;
    
    // Constants
    static constexpr int HEADER_HEIGHT = 48;
    static constexpr int BUTTON_SIZE = 32;
};

/**
 * @brief Main modern sidebar widget
 */
class ModernSidebarWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ModernSidebarWidget(QWidget* parent = nullptr);
    ~ModernSidebarWidget();

    // Category management
    void setCategories(const QHash<QString, ModernCategoryTreeWidget::CategoryData>& categories);
    QHash<QString, ModernCategoryTreeWidget::CategoryData> categories() const;
    void updateCategoryCount(const QString& categoryName, int count);

    // Selection
    void setSelectedCategory(const QString& categoryName);
    QString selectedCategory() const;

    // Sidebar state
    void setCollapsed(bool collapsed, bool animated = true);
    bool isCollapsed() const { return m_collapsed; }
    
    void setMinimumWidth(int width);
    void setMaximumWidth(int width);
    
    // Visual effects
    void setAnimationsEnabled(bool enabled);
    bool animationsEnabled() const { return m_animationsEnabled; }

    // Integration
    void setThemeManager(ThemeManager* themeManager);

public slots:
    void toggleCollapsed();
    void refreshCategories();

signals:
    void categorySelected(const QString& categoryName);
    void collapsedChanged(bool collapsed);
    void widthChanged(int width);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onCategorySelected(const QString& categoryName);
    void onCollapseToggled(bool collapsed);
    void onThemeChanged();

private:
    void setupUI();
    void updateLayout();
    void animateCollapse();
    void applyTheme();

    // UI components
    QVBoxLayout* m_mainLayout;
    SidebarHeaderWidget* m_headerWidget;
    ModernCategoryTreeWidget* m_categoryTree;
    
    // State
    bool m_collapsed;
    bool m_animationsEnabled;
    int m_expandedWidth;
    int m_collapsedWidth;
    
    // Animation
    QPropertyAnimation* m_collapseAnimation;
    
    // Integration
    ThemeManager* m_themeManager;
    
    // Constants
    static constexpr int DEFAULT_EXPANDED_WIDTH = 280;
    static constexpr int DEFAULT_COLLAPSED_WIDTH = 48;
    static constexpr int MIN_WIDTH = 200;
    static constexpr int MAX_WIDTH = 400;
    static constexpr int COLLAPSE_ANIMATION_DURATION = 300;
};

#endif // MODERNSIDEBARWIDGET_H
