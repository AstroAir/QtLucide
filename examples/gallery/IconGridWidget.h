/**
 * QtLucide Gallery Application - Icon Grid Widget Header
 * 
 * A high-performance widget for displaying large numbers of icons in a
 * scrollable grid with virtual scrolling, selection, and filtering capabilities.
 */

#ifndef ICONGRIDWIDGET_H
#define ICONGRIDWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QScrollBar>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRubberBand>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QStyledItemDelegate>
#include <QListView>
#include <QStandardItemModel>

#include <QtLucide/QtLucide.h>
#include "IconItem.h"
#include "IconMetadataManager.h"

/**
 * @brief Custom model for icon data to support efficient virtual scrolling
 */
class IconGridModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit IconGridModel(QObject *parent = nullptr);
    
    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    
    // Custom methods
    void setIconNames(const QStringList& iconNames);
    void setIconSize(int size);
    void setMetadataManager(IconMetadataManager* manager);
    void setLucide(lucide::QtLucide* lucide);
    
    QString iconNameAt(int index) const;
    QStringList selectedIconNames() const;
    
    // Custom roles
    enum CustomRoles {
        IconNameRole = Qt::UserRole + 1,
        IconMetadataRole,
        IsFavoriteRole,
        IsSelectedRole
    };

public slots:
    void refreshData();
    void updateFavorites();

signals:
    void iconSelectionChanged();

private:
    QStringList m_iconNames;
    int m_iconSize;
    IconMetadataManager* m_metadataManager;
    lucide::QtLucide* m_lucide;
};

/**
 * @brief Custom delegate for rendering icon items in the grid
 */
class IconGridDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit IconGridDelegate(QObject *parent = nullptr);
    
    // QStyledItemDelegate interface
    void paint(QPainter *painter, const QStyleOptionViewItem &option, 
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, 
                   const QModelIndex &index) const override;
    
    // Custom methods
    void setIconSize(int size);
    void setDisplayMode(IconItem::DisplayMode mode);
    void setShowIconNames(bool show);

protected:
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option, 
                     const QModelIndex &index) override;

signals:
    void iconClicked(const QString& iconName);
    void iconDoubleClicked(const QString& iconName);
    void favoriteToggled(const QString& iconName);

private:
    void paintIcon(QPainter *painter, const QRect &rect, 
                   const QIcon &icon, bool selected, bool favorite) const;
    void paintIconName(QPainter *painter, const QRect &rect, 
                       const QString &name, bool selected) const;
    void paintFavoriteIndicator(QPainter *painter, const QRect &rect, 
                                bool favorite) const;
    
    QRect getIconRect(const QRect &itemRect) const;
    QRect getNameRect(const QRect &itemRect) const;
    QRect getFavoriteRect(const QRect &itemRect) const;

private:
    int m_iconSize;
    IconItem::DisplayMode m_displayMode;
    bool m_showIconNames;
    
    static constexpr int ITEM_MARGIN = 8;
    static constexpr int TEXT_HEIGHT = 20;
    static constexpr int FAVORITE_SIZE = 16;
};

/**
 * @brief Main widget for displaying icons in a grid layout
 */
class IconGridWidget : public QWidget
{
    Q_OBJECT

public:
    enum ViewMode {
        GridView,
        ListView,
        CompactView
    };

    explicit IconGridWidget(lucide::QtLucide* lucide,
                           IconMetadataManager* metadataManager,
                           QWidget *parent = nullptr);
    ~IconGridWidget();

    // Icon management
    void setIconNames(const QStringList& iconNames);
    QStringList iconNames() const;
    QStringList selectedIconNames() const;
    QString currentIconName() const;
    
    // Display settings
    void setIconSize(int size);
    int iconSize() const { return m_iconSize; }
    void setViewMode(ViewMode mode);
    ViewMode viewMode() const { return m_viewMode; }
    void setShowIconNames(bool show);
    bool showIconNames() const { return m_showIconNames; }
    
    // Selection
    void selectIcon(const QString& iconName);
    void selectIcons(const QStringList& iconNames);
    void clearSelection();
    void selectAll();
    
    // Navigation
    void scrollToIcon(const QString& iconName);
    void scrollToTop();
    void scrollToBottom();

public slots:
    void refreshIcons();
    void updateIconSizes();
    void updateFavorites();
    void zoomIn();
    void zoomOut();
    void resetZoom();

signals:
    void iconSelected(const QString& iconName);
    void iconDoubleClicked(const QString& iconName);
    void selectionChanged(const QStringList& selectedIcons);
    void favoriteToggled(const QString& iconName, bool favorite);
    void contextMenuRequested(const QString& iconName, const QPoint& globalPos);
    void iconSizeChanged(int size);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onIconClicked(const QString& iconName);
    void onIconDoubleClicked(const QString& iconName);
    void onFavoriteToggled(const QString& iconName);
    void onSelectionChanged();
    void onScrollValueChanged();
    void updateVisibleItems();

private:
    void setupUI();
    void setupModel();
    void setupView();
    void updateViewSettings();
    void updateItemSize();
    void calculateOptimalColumns();
    
    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    
    // UI components
    QVBoxLayout* m_layout;
    QListView* m_listView;
    QLabel* m_emptyLabel;
    QProgressBar* m_loadingProgress;
    
    // Model and delegate
    IconGridModel* m_model;
    IconGridDelegate* m_delegate;
    
    // Settings
    int m_iconSize;
    ViewMode m_viewMode;
    bool m_showIconNames;
    QString m_currentIcon;
    
    // Performance optimization
    QTimer* m_updateTimer;
    bool m_needsUpdate;
    
    // Configuration
    static constexpr int DEFAULT_ICON_SIZE = 64;
    static constexpr int MIN_ICON_SIZE = 24;
    static constexpr int MAX_ICON_SIZE = 256;
    static constexpr int ZOOM_STEP = 8;
    static constexpr int UPDATE_DELAY = 100;
};

#endif // ICONGRIDWIDGET_H
