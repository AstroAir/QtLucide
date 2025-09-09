/**
 * QtLucide Gallery Application - Category Sidebar Widget
 *
 * Sidebar widget for category-based filtering with:
 * - 9 icon categories (business, communication, editing, files, general, media, navigation, social, system)
 * - Icon count per category
 * - Keyboard shortcuts (Ctrl+1-9)
 * - Favorites category
 * - Recently viewed category
 * - Search history
 */

#ifndef CATEGORYSIDEBARWIDGET_H
#define CATEGORYSIDEBARWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QMenu>

#include "../../config/LayoutConfig.h"
#include <QAction>
#include <QShortcut>

// Forward declarations
class IconMetadataManager;
class FavoritesManager;

/**
 * @brief Individual category item widget
 */
class CategoryItem : public QWidget
{
    Q_OBJECT

public:
    explicit CategoryItem(const QString& categoryName, const QString& displayName,
                         int iconCount = 0, QWidget* parent = nullptr);

    void setCategoryName(const QString& name) { m_categoryName = name; }
    QString categoryName() const { return m_categoryName; }

    void setDisplayName(const QString& name);
    QString displayName() const { return m_displayName; }

    void setIconCount(int count);
    int iconCount() const { return m_iconCount; }

    void setSelected(bool selected);
    bool isSelected() const { return m_selected; }

    void setShortcutKey(const QString& key);
    QString shortcutKey() const { return m_shortcutKey; }

signals:
    void clicked(const QString& categoryName);
    void contextMenuRequested(const QString& categoryName, const QPoint& globalPos);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    void updateAppearance();

    QString m_categoryName;
    QString m_displayName;
    int m_iconCount;
    bool m_selected;
    bool m_hovered;
    QString m_shortcutKey;

    QLabel* m_nameLabel;
    QLabel* m_countLabel;
    QLabel* m_shortcutLabel;

    static const int ITEM_HEIGHT = 40;
    static const int ICON_SIZE = 16;
};

/**
 * @brief Category sidebar widget with filtering capabilities
 */
class CategorySidebarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CategorySidebarWidget(QWidget* parent = nullptr);
    ~CategorySidebarWidget();

    // Manager setup
    void setIconMetadataManager(IconMetadataManager* manager);
    void setFavoritesManager(FavoritesManager* manager);

    // Category management
    void refreshCategories();
    void setCurrentCategory(const QString& category);
    QString currentCategory() const { return m_currentCategory; }

    // Special categories
    void updateFavoritesCount(int count);
    void updateRecentCount(int count);
    void addToRecentCategories(const QString& category);

    // Appearance
    void setCompactMode(bool compact);
    bool isCompactMode() const { return m_compactMode; }

    // Keyboard shortcuts
    void setupCategoryShortcuts(QWidget* parent);

signals:
    void categorySelected(const QString& category);
    void showAllRequested();
    void showFavoritesRequested();
    void showRecentRequested();
    void clearCategoryRequested(const QString& category);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onCategoryClicked(const QString& category);
    void onCategoryContextMenu(const QString& category, const QPoint& globalPos);
    void onShowAll();
    void onShowFavorites();
    void onShowRecent();
    void onClearCategory();
    void onRefreshCategories();
    void onCategoryShortcut(int index);

private:
    void setupUI();
    void setupSpecialCategories();
    void setupMainCategories();
    void setupRecentCategories();
    void createCategoryItem(const QString& categoryName, const QString& displayName,
                           int iconCount, const QString& shortcut = QString());
    void updateCategoryCounts();
    void selectCategory(const QString& category);

    // UI Components
    QVBoxLayout* m_mainLayout;
    QScrollArea* m_scrollArea;
    QWidget* m_scrollContent;
    QVBoxLayout* m_contentLayout;

    // Category sections
    QGroupBox* m_specialGroup;
    QGroupBox* m_mainGroup;
    QGroupBox* m_recentGroup;

    // Special category items
    CategoryItem* m_allCategoriesItem;
    CategoryItem* m_favoritesItem;
    CategoryItem* m_recentItem;

    // Main category items (9 categories)
    QList<CategoryItem*> m_categoryItems;
    QHash<QString, CategoryItem*> m_categoryMap;

    // Recent categories
    QList<CategoryItem*> m_recentCategoryItems;
    QStringList m_recentCategories;

    // Managers
    IconMetadataManager* m_iconMetadataManager;
    FavoritesManager* m_favoritesManager;

    // State
    QString m_currentCategory;
    bool m_compactMode;

    // Shortcuts
    QList<QShortcut*> m_categoryShortcuts;

    // Category information
    struct CategoryInfo {
        QString name;
        QString displayName;
        QString description;
        QString shortcut;
        int iconCount;
    };

    QList<CategoryInfo> m_categoryInfos;

    // Constants
    static const int MAX_RECENT_CATEGORIES = 5;
};

#endif // CATEGORYSIDEBARWIDGET_H
