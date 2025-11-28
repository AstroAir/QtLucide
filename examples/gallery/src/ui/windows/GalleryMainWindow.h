/**
 * @file GalleryMainWindow.h
 * @brief Main application window for the QtLucide gallery
 * @details This file contains the GalleryMainWindow class which manages the main UI layout,
 *          menu bar, and coordinates all major components of the gallery application.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef GALLERY_MAIN_WINDOW_H
#define GALLERY_MAIN_WINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QAction>
#include <QMenu>
#include <QLabel>
#include <memory>

// Forward declarations
class QCloseEvent;

namespace lucide {
class QtLucide;
}

namespace gallery {
class ContentManager;
class ThemeManager;
class CategorySidebarWidget;
class IconGridWidget;
class IconDetailsPanel;
struct IconOptions;
}

namespace gallery {

/**
 * @class GalleryMainWindow
 * @brief Main application window for the icon gallery
 * @details Manages the overall layout with a three-panel design:
 *          - Left: Category sidebar (280px, collapsible)
 *          - Center: Icon grid view (flexible)
 *          - Right: Icon details panel (collapsible)
 *
 * Features:
 * - QSplitter-based layout for resizable panels
 * - Menu bar with File, Edit, View, and Help menus
 * - Status bar showing icon count information
 * - Keyboard shortcuts (Ctrl+K for search focus)
 * - Automatic metadata loading on construction
 * - Theme management integration
 */
class GalleryMainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Construct GalleryMainWindow
     * @param parent The parent QWidget
     */
    explicit GalleryMainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~GalleryMainWindow() override;

protected:
    /**
     * @brief Handle close events
     * @param event The close event
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * @brief Handle the export action from menu
     */
    void onExportTriggered();

    /**
     * @brief Handle the preferences/settings action
     */
    void onPreferencesTriggered();

    /**
     * @brief Toggle the visibility of the details panel
     */
    void onToggleDetailsPanel();

    /**
     * @brief Handle about action
     */
    void onAboutTriggered();

    /**
     * @brief Handle about Qt action
     */
    void onAboutQtTriggered();

    /**
     * @brief Handle icon selection change
     * @param iconName The newly selected icon name
     */
    void onIconSelected(const QString &iconName);

    /**
     * @brief Update status bar with current icon count
     * @param icons The list of currently displayed icons
     */
    void onIconsFiltered(const QStringList &icons);

    /**
     * @brief Handle theme change notification
     * @param mode The new theme mode
     */
    void onThemeChanged();

    /**
     * @brief Handle icon options change from sidebar
     * @param options The new icon options
     */
    void onOptionsChanged(const IconOptions &options);

private:
    /**
     * @brief Initialize the UI components
     */
    void initializeUI();

    /**
     * @brief Create the menu bar
     */
    void createMenuBar();

    /**
     * @brief Create menu actions
     */
    void createActions();

    /**
     * @brief Create status bar
     */
    void createStatusBar();

    /**
     * @brief Setup keyboard shortcuts
     */
    void setupShortcuts();

    /**
     * @brief Load application settings
     */
    void loadSettings();

    /**
     * @brief Save application settings
     */
    void saveSettings() const;

    /**
     * @brief Initialize metadata loading from resources
     */
    void initializeMetadata();

    /**
     * @brief Connect all signals and slots
     */
    void connectSignals();

    /**
     * @brief Update category list in sidebar
     */
    void updateCategoryList();

    // UI Components
    QSplitter *m_mainSplitter = nullptr;                    ///< Main horizontal splitter
    CategorySidebarWidget *m_categoryPanel = nullptr;       ///< Left category sidebar
    IconGridWidget *m_iconGrid = nullptr;                   ///< Center icon grid
    IconDetailsPanel *m_detailsPanel = nullptr;             ///< Right details panel

    // Status bar labels
    QLabel *m_statusLabel = nullptr;                        ///< Status bar label

    // Managers
    std::unique_ptr<lucide::QtLucide> m_lucide;             ///< QtLucide instance for icon rendering
    std::unique_ptr<ContentManager> m_contentManager;       ///< Content manager instance
    std::unique_ptr<ThemeManager> m_themeManager;           ///< Theme manager instance

    // Menu actions
    QAction *m_exportAction = nullptr;                      ///< Export action
    QAction *m_quitAction = nullptr;                        ///< Quit action
    QAction *m_preferencesAction = nullptr;                 ///< Preferences action
    QAction *m_toggleDetailsPanelAction = nullptr;          ///< Toggle details panel action
    QAction *m_aboutAction = nullptr;                       ///< About action
    QAction *m_aboutQtAction = nullptr;                     ///< About Qt action

    // Menus
    QMenu *m_fileMenu = nullptr;                            ///< File menu
    QMenu *m_editMenu = nullptr;                            ///< Edit menu
    QMenu *m_viewMenu = nullptr;                            ///< View menu
    QMenu *m_helpMenu = nullptr;                            ///< Help menu

    bool m_detailsPanelVisible = true;                      ///< Track details panel visibility
};

}  // namespace gallery

#endif  // GALLERY_MAIN_WINDOW_H
