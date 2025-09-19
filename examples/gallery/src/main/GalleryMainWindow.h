/**
 * QtLucide Gallery Application - Main Window with Export/Import
 *
 * Enhanced main window that integrates all the modern UI components
 * including the new export and import functionality:
 * - Modern icon grid with export capabilities
 * - Comprehensive export dialog with multiple formats
 * - Settings import/export functionality
 * - Theme management integration
 * - Responsive layout system
 * - Modern search and filtering
 */

#ifndef GALLERYMAINWINDOW_H
#define GALLERYMAINWINDOW_H

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QCloseEvent>
#include <QComboBox>
#include <QDesktopServices>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QPushButton>
#include <QResizeEvent>
#include <QSettings>
#include <QShowEvent>
#include <QSlider>
#include <QSplitter>
#include <QStandardPaths>
#include <QStatusBar>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

#include "../core/BatchExportManager.h"
#include "../ui/dialogs/ExportDialog.h"
#include "../ui/dialogs/ImportDialog.h"
#include "../ui/layouts/ResponsiveLayoutManager.h"
#include "../ui/themes/ThemeManager.h"
#include "../ui/widgets/grids/ModernIconGridWidget.h"
#include "../ui/widgets/panels/ModernSidebarWidget.h"
#include "../ui/widgets/search/ModernSearchWidget.h"
#include <QtLucide/QtLucide.h>

// Forward declarations
class QSplitter;
class QStackedWidget;

/**
 * @brief Main Gallery window with modern UI and export/import functionality
 */
class GalleryMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit GalleryMainWindow(QWidget* parent = nullptr);
    ~GalleryMainWindow();

    // Integration
    void setLucide(lucide::QtLucide* lucide);
    lucide::QtLucide* lucide() const { return m_lucide; }

    // Theme management
    void setTheme(ThemeManager::Theme theme);
    ThemeManager::Theme currentTheme() const;

    // Window state
    void saveWindowState();
    void restoreWindowState();

public slots:
    void showExportDialog();
    void showImportDialog();
    void exportSelectedIcons();
    void exportAllIcons();
    void importSettings();
    void exportSettings();

signals:
    void iconSelectionChanged(const QStringList& selectedIcons);
    void themeChanged(ThemeManager::Theme theme);
    void exportRequested(const QStringList& iconNames);
    void importRequested(const QString& filePath);

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    // Menu actions
    void onNewCollection();
    void onOpenCollection();
    void onSaveCollection();
    void onSaveCollectionAs();
    void onExportIcons();
    void onImportSettings();
    void onExportSettings();
    void onPreferences();
    void onExit();

    // View actions
    void onToggleFullscreen();
    void onToggleSidebar();
    void onToggleStatusBar();
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();
    void onFitToWindow();

    // Theme actions
    void onThemeChanged();
    void onLightTheme();
    void onDarkTheme();
    void onSystemTheme();

    // Help actions
    void onAbout();
    void onAboutQt();
    void onShowHelp();
    void onReportIssue();

    // Widget interactions
    void onIconSelectionChanged(const QStringList& selectedIcons);
    void onIconDoubleClicked(const QString& iconName);
    void onSearchTextChanged(const QString& text);
    void onFilterChanged(const QStringList& categories, const QStringList& tags);
    void onCategorySelected(const QString& category);

    // Export/Import
    void onExportDialogFinished(bool success, const QStringList& exportedFiles);
    void onImportDialogFinished(bool success, const QStringList& importedTypes);
    void onExportProgress(int current, int total, const QString& currentFile);

    // Layout and responsive design
    void onWindowSizeChanged();
    void onLayoutModeChanged();
    void updateResponsiveLayout();

    // Status updates
    void updateStatusBar();
    void updateWindowTitle();
    void updateIconCount();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupCentralWidget();
    void setupConnections();
    void setupTheme();
    void setupResponsiveLayout();

    // Menu creation helpers
    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createToolsMenu();
    void createHelpMenu();

    // Toolbar creation helpers
    void createMainToolBar();
    void createViewToolBar();
    void createExportToolBar();

    // UI update helpers
    void updateMenuStates();
    void updateToolBarStates();
    void updateThemeActions();
    void applyCurrentTheme();

    // Settings management
    void loadSettings();
    void saveSettings();
    void resetSettings();

    // Export/Import helpers
    QStringList getSelectedIconNames() const;
    QStringList getAllIconNames() const;
    bool validateExportSelection() const;
    void showExportSuccessMessage(const QStringList& exportedFiles);
    void showImportSuccessMessage(const QStringList& importedTypes);

    // Core components
    lucide::QtLucide* m_lucide;
    ThemeManager* m_themeManager;
    ResponsiveLayoutManager* m_layoutManager;
    BatchExportManager* m_exportManager;
    SettingsManager* m_settingsManager;

    // UI components
    QWidget* m_centralWidget;
    QSplitter* m_mainSplitter;
    QSplitter* m_contentSplitter;

    ModernSidebarWidget* m_sidebarWidget;
    ModernSearchWidget* m_searchWidget;
    ModernIconGridWidget* m_iconGridWidget;

    // Dialogs
    ExportDialog* m_exportDialog;
    ImportDialog* m_importDialog;

    // Menu bar
    QMenu* m_fileMenu;
    QMenu* m_editMenu;
    QMenu* m_viewMenu;
    QMenu* m_toolsMenu;
    QMenu* m_helpMenu;

    // Actions
    QActionGroup* m_themeActionGroup;
    QAction* m_lightThemeAction;
    QAction* m_darkThemeAction;
    QAction* m_systemThemeAction;

    QAction* m_exportIconsAction;
    QAction* m_importSettingsAction;
    QAction* m_exportSettingsAction;
    QAction* m_toggleSidebarAction;
    QAction* m_toggleStatusBarAction;
    QAction* m_fullscreenAction;

    // Toolbars
    QToolBar* m_mainToolBar;
    QToolBar* m_viewToolBar;
    QToolBar* m_exportToolBar;

    // Status bar widgets
    QLabel* m_statusLabel;
    QLabel* m_iconCountLabel;
    QLabel* m_selectionCountLabel;
    QProgressBar* m_progressBar;
    QPushButton* m_themeToggleButton;

    // State management
    QSettings* m_settings;
    bool m_isFullscreen;
    bool m_sidebarVisible;
    bool m_statusBarVisible;
    QStringList m_selectedIcons;
    QString m_currentCollection;
    bool m_hasUnsavedChanges;

    // Layout state
    ResponsiveLayoutManager::ScreenSize m_currentScreenSize;
    ResponsiveLayoutManager::PanelState m_sidebarState;
    int m_iconGridZoomLevel;

    // Constants
    static constexpr int DEFAULT_ICON_SIZE = 64;
    static constexpr int MIN_ICON_SIZE = 32;
    static constexpr int MAX_ICON_SIZE = 256;
    static constexpr int ZOOM_STEP = 16;
    static constexpr int STATUS_MESSAGE_TIMEOUT = 3000;
};

#endif // GALLERYMAINWINDOW_H
