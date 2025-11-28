/**
 * @file IconDetailsPanel.h
 * @brief Right-side panel for displaying detailed icon information
 * @details Shows icon previews at multiple sizes, name, tags, categories,
 *          and code snippets for using the icon. Panel can be collapsed/expanded.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef ICONDETAILSPANEL_H
#define ICONDETAILSPANEL_H

#include "IconThumbnailGridWidget.h"
#include "IconMetadataManager.h"

#include <QFrame>
#include <QString>
#include <memory>

class QLabel;
class QPushButton;
class QScrollArea;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;

namespace lucide {
class QtLucide;
}

namespace gallery {

/**
 * @class IconDetailsPanel
 * @brief Collapsible right-side panel with detailed icon information
 * @details IconDetailsPanel displays comprehensive information about a selected icon,
 *          including preview at multiple sizes, metadata, tags, categories, and
 *          code snippets for different platforms. The panel can be collapsed to
 *          save horizontal space.
 *
 * @par Features:
 * - Icon preview at multiple sizes (24, 48, 96, 192px)
 * - Icon name display in large font
 * - Clickable tag chips
 * - Categories list
 * - Code snippets:
 *   - C++ enum: lucide.icon(Icons::icon_name)
 *   - C++ string: lucide.icon("icon-name")
 * - Copy buttons for each code snippet
 * - Action buttons: Copy Name, Export, Toggle Favorite
 * - Collapsible design (320px default width)
 * - Smooth animations when collapsing/expanding
 *
 * @par Usage:
 * @code
 * IconDetailsPanel detailsPanel;
 * detailsPanel.setSelectedIcon("star", metadata);
 *
 * connect(&detailsPanel, &IconDetailsPanel::exportRequested,
 *         this, &MyWindow::onExportRequested);
 * connect(&detailsPanel, &IconDetailsPanel::favoriteToggled,
 *         this, &MyWindow::onFavoriteToggled);
 * @endcode
 */
class IconDetailsPanel : public QFrame {
    Q_OBJECT

public:
    /**
     * @brief Construct an IconDetailsPanel
     * @param parent Parent widget
     */
    explicit IconDetailsPanel(QWidget* parent = nullptr);

    /**
     * @brief Destroy the panel
     */
    ~IconDetailsPanel() override;

    /**
     * @brief Set the selected icon and display its details
     * @param iconName The icon name in kebab-case format
     * @param metadata Icon metadata including tags and categories
     * @note If metadata is empty, only the icon name will be displayed
     */
    void setSelectedIcon(const QString& iconName, const IconMetadata& metadata = IconMetadata());

    /**
     * @brief Get the currently displayed icon name
     * @return Icon name, or empty string if no icon selected
     */
    [[nodiscard]] QString selectedIcon() const;

    /**
     * @brief Clear the current selection and hide details
     */
    void clearSelection();

    /**
     * @brief Set the QtLucide instance for icon rendering
     * @param lucide QtLucide instance (must remain valid during widget lifetime)
     */
    void setLucideInstance(lucide::QtLucide* lucide);

    /**
     * @brief Set the FavoritesManager for favorite tracking
     * @param favManager FavoritesManager instance
     */
    void setFavoritesManager(class FavoritesManager* favManager);

    /**
     * @brief Toggle the collapsed/expanded state
     */
    void toggleCollapsed();

    /**
     * @brief Check if the panel is collapsed
     * @return true if panel is collapsed, false if expanded
     */
    [[nodiscard]] bool isCollapsed() const;

    /**
     * @brief Set the collapsed state
     * @param collapsed true to collapse, false to expand
     */
    void setCollapsed(bool collapsed);

    /**
     * @brief Set the default width (when expanded)
     * @param width Width in pixels (default 320)
     */
    void setDefaultWidth(int width);

Q_SIGNALS:
    /**
     * @brief Emitted when export is requested
     * @param iconName The name of the icon to export
     */
    void exportRequested(const QString& iconName);

    /**
     * @brief Emitted when favorite status is toggled
     * @param iconName The name of the icon
     * @param isFavorite true if added to favorites, false if removed
     */
    void favoriteToggled(const QString& iconName, bool isFavorite);

    /**
     * @brief Emitted when a tag is clicked
     * @param tag The tag that was clicked
     */
    void tagClicked(const QString& tag);

    /**
     * @brief Emitted when the collapsed state changes
     * @param isCollapsed true if now collapsed, false if expanded
     */
    void collapsedStateChanged(bool isCollapsed);

public Q_SLOTS:
    /**
     * @brief Copy the icon name to clipboard
     */
    void copyIconName();

    /**
     * @brief Copy the C++ enum snippet to clipboard
     */
    void copyCppEnumSnippet();

    /**
     * @brief Copy the C++ string snippet to clipboard
     */
    void copyCppStringSnippet();

private Q_SLOTS:
    /**
     * @brief Handle tag click
     */
    void onTagClicked();

    /**
     * @brief Handle favorite button click
     */
    void onFavoriteButtonClicked();

    /**
     * @brief Handle export button click
     */
    void onExportButtonClicked();

    /**
     * @brief Handle collapse button click
     */
    void onCollapseButtonClicked();

private:
    /**
     * @brief Initialize the UI layout
     */
    void initializeUI();

    /**
     * @brief Update the UI with selected icon information
     */
    void updateUI();

    /**
     * @brief Generate the C++ enum snippet
     * @param iconName The icon name
     * @return C++ enum usage snippet
     */
    [[nodiscard]] QString generateCppEnumSnippet(const QString& iconName) const;

    /**
     * @brief Generate the C++ string snippet
     * @param iconName The icon name
     * @return C++ string usage snippet
     */
    [[nodiscard]] QString generateCppStringSnippet(const QString& iconName) const;

    /**
     * @brief Convert icon name to C++ enum identifier
     * @param iconName The icon name in kebab-case
     * @return C++ enum identifier (snake_case)
     */
    [[nodiscard]] static QString iconNameToEnumId(const QString& iconName);

    /**
     * @brief Update favorite button appearance
     */
    void updateFavoriteButton();

    // Member variables
    lucide::QtLucide* m_lucide{nullptr};
    class FavoritesManager* m_favManager{nullptr};
    IconMetadata m_currentMetadata;
    QString m_selectedIcon;
    bool m_isCollapsed{false};
    int m_defaultWidth{320};

    // UI Components
    QScrollArea* m_scrollArea{nullptr};
    QWidget* m_contentWidget{nullptr};
    QVBoxLayout* m_mainLayout{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};

    // Header section
    QHBoxLayout* m_headerLayout{nullptr};
    QLabel* m_iconNameLabel{nullptr};
    QPushButton* m_collapseButton{nullptr};
    QPushButton* m_favoriteButton{nullptr};
    QPushButton* m_exportButton{nullptr};

    // Preview section
    IconThumbnailGridWidget* m_thumbnailGrid{nullptr};

    // Info section
    QLabel* m_tagsLabel{nullptr};
    QWidget* m_tagsContainer{nullptr};
    QHBoxLayout* m_tagsLayout{nullptr};

    QLabel* m_categoriesLabel{nullptr};
    QWidget* m_categoriesContainer{nullptr};
    QVBoxLayout* m_categoriesLayout{nullptr};

    // Code snippets section
    QLabel* m_snippetsLabel{nullptr};

    // C++ Enum snippet
    QWidget* m_enumSnippetWidget{nullptr};
    QHBoxLayout* m_enumSnippetLayout{nullptr};
    QLabel* m_enumSnippetLabel{nullptr};
    QPushButton* m_enumCopyButton{nullptr};

    // C++ String snippet
    QWidget* m_stringSnippetWidget{nullptr};
    QHBoxLayout* m_stringSnippetLayout{nullptr};
    QLabel* m_stringSnippetLabel{nullptr};
    QPushButton* m_stringCopyButton{nullptr};

    // Action buttons section
    QHBoxLayout* m_actionButtonsLayout{nullptr};
    QPushButton* m_copyNameButton{nullptr};
};

} // namespace gallery

#endif // ICONDETAILSPANEL_H
