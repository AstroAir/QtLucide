/**
 * QtLucide Gallery Application - Icon Item Header
 *
 * Represents a single icon item in the gallery grid with visual feedback,
 * selection states, and interaction capabilities.
 */

#ifndef ICONITEM_H
#define ICONITEM_H

#include <QAction>
#include <QContextMenuEvent>
#include <QEnterEvent>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWidget>

#include "managers/IconMetadataManager.h"
#include <QtLucide/QtLucide.h>

/**
 * @brief A widget representing a single icon in the gallery grid
 *
 * This widget displays an icon with its name and provides visual feedback
 * for hover, selection, and favorite states. It supports context menus
 * and various interaction modes.
 */
class IconItem : public QFrame {
    Q_OBJECT
    Q_PROPERTY(bool selected READ isSelected WRITE setSelected NOTIFY selectionChanged)
    Q_PROPERTY(bool favorite READ isFavorite WRITE setFavorite NOTIFY favoriteChanged)
    Q_PROPERTY(int iconSize READ iconSize WRITE setIconSize NOTIFY iconSizeChanged)

public:
    enum DisplayMode {
        GridMode,   // Icon with name below
        ListMode,   // Icon with name to the right
        CompactMode // Icon only, name in tooltip
    };

    explicit IconItem(const QString& iconName, lucide::QtLucide* lucide,
                      IconMetadataManager* metadataManager, QWidget* parent = nullptr);
    ~IconItem();

    // Basic properties
    QString iconName() const { return m_iconName; }
    bool isSelected() const { return m_selected; }
    bool isFavorite() const { return m_favorite; }
    int iconSize() const { return m_iconSize; }
    DisplayMode displayMode() const { return m_displayMode; }

    // Icon metadata
    IconMetadata metadata() const;
    QStringList categories() const;
    QStringList tags() const;

public slots:
    void setSelected(bool selected);
    void setFavorite(bool favorite);
    void setIconSize(int size);
    void setDisplayMode(DisplayMode mode);
    void setIconColor(const QColor& color);
    void refreshIcon();
    void updateFromMetadata();

signals:
    void selectionChanged(bool selected);
    void favoriteChanged(bool favorite);
    void iconSizeChanged(int size);
    void clicked(const QString& iconName);
    void doubleClicked(const QString& iconName);
    void rightClicked(const QString& iconName, const QPoint& globalPos);
    void favoriteToggled(const QString& iconName, bool favorite);
    void copyRequested(const QString& iconName, const QString& format);

protected:
    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onCopyIconName();
    void onCopyIconCode();
    void onCopyIconSvg();
    void onToggleFavorite();
    void onShowDetails();
    void onHoverAnimationFinished();

private:
    void setupUI();
    void setupContextMenu();
    void setupAnimations();
    void updateLayout();
    void updateIconDisplay();
    void updateNameDisplay();
    void updateSelectionState();
    void updateHoverState();
    void updateFavoriteState();
    void updateFavoriteButton();
    void updateTooltip();

    void startHoverAnimation(bool entering);
    void applyVisualEffects();
    void removeVisualEffects();

    QSize calculateOptimalSize() const;
    QString formatIconName() const;
    QString createTooltipText() const;

    // Core data
    QString m_iconName;
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;

    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_contentLayout;
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    QPushButton* m_favoriteButton;

    // Context menu
    QMenu* m_contextMenu;
    QAction* m_copyNameAction;
    QAction* m_copyCodeAction;
    QAction* m_copySvgAction;
    QAction* m_toggleFavoriteAction;
    QAction* m_showDetailsAction;

    // State
    bool m_selected;
    bool m_favorite;
    bool m_hovered;
    int m_iconSize;
    DisplayMode m_displayMode;
    QColor m_iconColor;

    // Visual effects
    QPropertyAnimation* m_hoverAnimation;
    QGraphicsDropShadowEffect* m_shadowEffect;
    QTimer* m_tooltipTimer;

    // Styling
    static constexpr int DEFAULT_ICON_SIZE = 48;
    static constexpr int MIN_ICON_SIZE = 16;
    static constexpr int MAX_ICON_SIZE = 256;
    static constexpr int ANIMATION_DURATION = 150;
    static constexpr int TOOLTIP_DELAY = 500;

    // Colors
    static const QColor SELECTION_COLOR;
    static const QColor HOVER_COLOR;
    static const QColor FAVORITE_COLOR;
    static const QColor BORDER_COLOR;
};

/**
 * @brief Custom widget for displaying icon items in a flow layout
 *
 * This widget automatically arranges IconItem widgets in a grid layout
 * that adapts to the available space and icon size.
 */
class IconItemContainer : public QWidget {
    Q_OBJECT

public:
    explicit IconItemContainer(QWidget* parent = nullptr);

    void addIconItem(IconItem* item);
    void removeIconItem(IconItem* item);
    void clearItems();

    QList<IconItem*> iconItems() const { return m_items; }
    int itemCount() const { return static_cast<int>(m_items.size()); }

    void setItemSize(int size);
    void setDisplayMode(IconItem::DisplayMode mode);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLayout();

private:
    void calculateLayout();

    QList<IconItem*> m_items;
    int m_itemSize;
    IconItem::DisplayMode m_displayMode;
    int m_columns;
    int m_spacing;
};

#endif // ICONITEM_H
