/**
 * QtLucide Gallery Application - Icon Thumbnail Grid Widget
 *
 * High-performance thumbnail grid with virtual scrolling for displaying
 * large numbers of icons efficiently. Supports:
 * - Virtual scrolling for 1,634+ icons
 * - Customizable thumbnail sizes
 * - Keyboard navigation
 * - Selection and multi-selection
 * - Tooltips with metadata
 * - Drag and drop support
 */

#ifndef ICONTHUMBNAILGRIDWIDGET_H
#define ICONTHUMBNAILGRIDWIDGET_H

#include <QContextMenuEvent>
#include <QGraphicsEffect>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMenu>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QToolTip>
#include <QVBoxLayout>
#include <QWidget>

#include "../../config/LayoutConfig.h"
#include <QApplication>
#include <QClipboard>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMouseEvent>
#include <QWheelEvent>

// Forward declarations
class ContentManager;
class IconMetadataManager;
class FavoritesManager;

/**
 * @brief Individual thumbnail item widget
 */
class IconThumbnailItem : public QWidget {
    Q_OBJECT

public:
    explicit IconThumbnailItem(const QString& iconName, QWidget* parent = nullptr);

    void setIconName(const QString& iconName);
    QString iconName() const { return m_iconName; }

    void setThumbnailSize(int size);
    int thumbnailSize() const { return m_thumbnailSize; }

    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    void setFavorite(bool favorite);
    bool isFavorite() const { return m_favorite; }

    void updateThumbnail();
    void setContentManager(ContentManager* manager) { m_contentManager = manager; }
    void setIconMetadataManager(IconMetadataManager* manager) { m_iconMetadataManager = manager; }

signals:
    void clicked(const QString& iconName);
    void doubleClicked(const QString& iconName);
    void rightClicked(const QString& iconName, const QPoint& globalPos);
    void favoriteToggled(const QString& iconName, bool favorite);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onHoverAnimationFinished();

private:
    void setupUI();
    void updateAppearance();
    void showTooltip();
    void hideTooltip();

    QString m_iconName;
    int m_thumbnailSize;
    bool m_selected;
    bool m_favorite;
    bool m_hovered;

    QPixmap m_thumbnail;
    QPixmap m_cachedThumbnail;

    ContentManager* m_contentManager;
    IconMetadataManager* m_iconMetadataManager;

    QPropertyAnimation* m_hoverAnimation;
    QTimer* m_tooltipTimer;

    static const int DEFAULT_THUMBNAIL_SIZE = 64;
    static const int SELECTION_BORDER_WIDTH = 2;
    static const int FAVORITE_ICON_SIZE = 16;
};

/**
 * @brief Virtual scrolling thumbnail grid widget
 */
class IconThumbnailGridWidget : public QWidget {
    Q_OBJECT

public:
    explicit IconThumbnailGridWidget(QWidget* parent = nullptr);
    ~IconThumbnailGridWidget();

    // Content management
    void setContentManager(ContentManager* manager);
    void setIconMetadataManager(IconMetadataManager* manager);
    void setFavoritesManager(FavoritesManager* manager);

    // Icon list management
    void setIconList(const QStringList& icons);
    QStringList iconList() const { return m_iconList; }

    // Selection
    void setCurrentIcon(const QString& iconName);
    QString currentIcon() const { return m_currentIcon; }
    QStringList selectedIcons() const;
    void clearSelection();
    void selectAll();

    // Appearance
    void setThumbnailSize(int size);
    int thumbnailSize() const { return m_thumbnailSize; }

    void setColumnsPerRow(int columns);
    int columnsPerRow() const { return m_columnsPerRow; }

    // Navigation
    void scrollToIcon(const QString& iconName);
    void scrollToTop();
    void scrollToBottom();

    // Virtual scrolling
    void updateVisibleItems();
    void refreshThumbnails();

signals:
    void iconSelected(const QString& iconName);
    void iconDoubleClicked(const QString& iconName);
    void selectionChanged(const QStringList& selectedIcons);
    void contextMenuRequested(const QString& iconName, const QPoint& globalPos);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void onScrollValueChanged(int value);
    void onItemClicked(const QString& iconName);
    void onItemDoubleClicked(const QString& iconName);
    void onItemRightClicked(const QString& iconName, const QPoint& globalPos);
    void onFavoriteToggled(const QString& iconName, bool favorite);

private:
    void setupUI();
    void setupScrollArea();
    void calculateLayout();
    void updateScrollArea();
    void createVisibleItems();
    void destroyInvisibleItems();

    // Navigation helpers
    int getIconIndex(const QString& iconName) const;
    QString getIconAt(int index) const;
    void selectIcon(const QString& iconName, bool multiSelect = false);
    void navigateToIcon(int direction); // -1 = previous, +1 = next

    // Virtual scrolling helpers
    int getFirstVisibleIndex() const;
    int getLastVisibleIndex() const;
    int getItemsPerRow() const;
    int getTotalRows() const;
    QRect getItemRect(int index) const;

    // Core data
    QStringList m_iconList;
    QString m_currentIcon;
    QStringList m_selectedIcons;

    // Managers
    ContentManager* m_contentManager;
    IconMetadataManager* m_iconMetadataManager;
    FavoritesManager* m_favoritesManager;

    // UI components
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QGridLayout* m_gridLayout;

    // Virtual scrolling
    QHash<QString, IconThumbnailItem*> m_visibleItems;
    int m_firstVisibleIndex;
    int m_lastVisibleIndex;

    // Layout settings
    int m_thumbnailSize;
    int m_columnsPerRow;
    int m_itemSpacing;
    int m_contentMargin;

    // Performance
    QTimer* m_updateTimer;
    bool m_needsUpdate;

    static const int DEFAULT_THUMBNAIL_SIZE = 64;
    static const int MIN_THUMBNAIL_SIZE = 32;
    static const int MAX_THUMBNAIL_SIZE = 256;
    static const int DEFAULT_ITEM_SPACING = 8;
    static const int DEFAULT_CONTENT_MARGIN = 16;
    static const int UPDATE_DELAY_MS = 16; // ~60 FPS
};

#endif // ICONTHUMBNAILGRIDWIDGET_H
