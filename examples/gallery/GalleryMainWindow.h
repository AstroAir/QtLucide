/**
 * QtLucide Gallery Application - Main Window Header
 * 
 * The main window class that orchestrates all UI components and provides
 * the primary interface for the icon gallery application.
 */

#ifndef GALLERYMAINWINDOW_H
#define GALLERYMAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QProgressBar>
#include <QSettings>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QActionGroup>

#include <QtLucide/QtLucide.h>

// Forward declarations
class IconGridWidget;
class IconDetailsPanel;
class SearchWidget;
class CategoryFilterWidget;
class IconMetadataManager;

class GalleryMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GalleryMainWindow(QWidget *parent = nullptr);
    ~GalleryMainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Menu and toolbar actions
    void onAboutApplication();
    void onAboutQt();
    void onShowPreferences();
    void onToggleFullscreen();
    void onExportIcons();
    void onImportFavorites();
    void onExportFavorites();
    
    // View actions
    void onToggleSidebar();
    void onToggleDetailsPanel();
    void onToggleStatusBar();
    void onChangeGridSize(int size);
    void onChangeViewMode();
    
    // Icon selection and interaction
    void onIconSelected(const QString& iconName);
    void onIconDoubleClicked(const QString& iconName);
    void onCopyIconName();
    void onCopyIconCode();
    void onCopyIconSvg();
    void onToggleFavorite();
    
    // Search and filtering
    void onSearchTextChanged(const QString& text);
    void onCategoryFilterChanged(const QStringList& categories);
    void onTagFilterChanged(const QStringList& tags);
    void onClearAllFilters();
    
    // Application state
    void onIconsLoaded(int totalCount);
    void onFilteredIconsChanged(int visibleCount, int totalCount);
    void onApplicationStateChanged();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupCentralWidget();
    void setupConnections();
    
    void createActions();
    void updateActions();
    void updateStatusBar();
    
    void loadSettings();
    void saveSettings();
    void restoreGeometry();
    void saveGeometry();
    
    void initializeIconSystem();
    void setupApplicationIcon();
    
    // Core components
    lucide::QtLucide* m_lucide;
    IconMetadataManager* m_metadataManager;
    
    // UI Components
    QWidget* m_centralWidget;
    QSplitter* m_mainSplitter;
    QSplitter* m_rightSplitter;
    
    SearchWidget* m_searchWidget;
    CategoryFilterWidget* m_categoryFilter;
    IconGridWidget* m_iconGrid;
    IconDetailsPanel* m_detailsPanel;
    
    // Menu and toolbar
    QMenuBar* m_menuBar;
    QToolBar* m_mainToolBar;
    QToolBar* m_viewToolBar;
    QStatusBar* m_statusBar;
    
    // Status bar widgets
    QLabel* m_statusLabel;
    QLabel* m_iconCountLabel;
    QProgressBar* m_progressBar;
    
    // Actions
    QAction* m_aboutAction;
    QAction* m_aboutQtAction;
    QAction* m_preferencesAction;
    QAction* m_exitAction;
    QAction* m_fullscreenAction;
    QAction* m_exportAction;
    QAction* m_importFavoritesAction;
    QAction* m_exportFavoritesAction;
    
    QAction* m_toggleSidebarAction;
    QAction* m_toggleDetailsPanelAction;
    QAction* m_toggleStatusBarAction;
    QActionGroup* m_gridSizeActionGroup;
    QActionGroup* m_viewModeActionGroup;
    
    QAction* m_copyNameAction;
    QAction* m_copyCodeAction;
    QAction* m_copySvgAction;
    QAction* m_toggleFavoriteAction;
    QAction* m_clearFiltersAction;
    
    // Settings and state
    QSettings* m_settings;
    QString m_currentIconName;
    QTimer* m_statusUpdateTimer;
    
    // Configuration
    static constexpr int DEFAULT_GRID_SIZE = 64;
    static constexpr int MIN_GRID_SIZE = 32;
    static constexpr int MAX_GRID_SIZE = 128;
};

#endif // GALLERYMAINWINDOW_H
