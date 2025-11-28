/**
 * @file CategorySidebarWidget.h
 * @brief Left sidebar customizer panel for the gallery application
 * @details Provides customization options for icon rendering including color,
 *          stroke width, and size. Also displays available icon categories with
 *          selection capability, mimicking the Lucide website design.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef CATEGORY_SIDEBAR_WIDGET_H
#define CATEGORY_SIDEBAR_WIDGET_H

#include <QWidget>
#include <QString>
#include <QColor>
#include <memory>

#include "../../core/GalleryTypes.h"

class QLabel;
class QPushButton;
class QSlider;
class QListWidget;
class QListWidgetItem;
class QVBoxLayout;
class QHBoxLayout;
class QFrame;

namespace gallery {

/**
 * @class CategorySidebarWidget
 * @brief Left sidebar widget for customization and category selection
 * @details Provides a professional sidebar interface with:
 *          - "Customizer" header with reset button
 *          - Color picker section
 *          - Stroke width slider (0.5-4.0px)
 *          - Icon size slider (16-256px)
 *          - Category list with item counts
 *          - "All" option for viewing all icons
 *
 * @par Usage:
 * @code
 * CategorySidebarWidget sidebar;
 * connect(&sidebar, &CategorySidebarWidget::optionsChanged,
 *         this, &MyWidget::onOptionsChanged);
 * connect(&sidebar, &CategorySidebarWidget::categorySelected,
 *         this, &MyWidget::onCategorySelected);
 * @endcode
 *
 * @see IconOptions
 */
class CategorySidebarWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct a CategorySidebarWidget
     * @param parent The parent widget
     */
    explicit CategorySidebarWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~CategorySidebarWidget() override;

    /**
     * @brief Set the available categories with their icon counts
     * @param categories A list of category names
     * @param iconCounts A map of category name to icon count
     * @details Updates the category list widget with the provided categories
     */
    void setCategories(const QStringList& categories, const QMap<QString, int>& iconCounts);

    /**
     * @brief Set the current icon options
     * @param options The IconOptions to display
     * @details Updates all UI elements to reflect the provided options
     */
    void setIconOptions(const IconOptions& options);

    /**
     * @brief Get the current icon options from the sidebar
     * @return The current IconOptions
     */
    [[nodiscard]] IconOptions getIconOptions() const;

    /**
     * @brief Set the current theme color
     * @param isDark True if dark theme is active
     * @details Adjusts the default color based on theme
     */
    void setTheme(bool isDark);

Q_SIGNALS:
    /**
     * @brief Emitted when icon customization options change
     * @param options The new IconOptions
     * @details Emitted whenever color, stroke width, or size changes
     */
    void optionsChanged(const IconOptions& options);

    /**
     * @brief Emitted when a category is selected
     * @param category The selected category name (empty string for "All")
     * @details Emitted when a category list item is clicked
     */
    void categorySelected(const QString& category);

    /**
     * @brief Emitted when "Show All" is clicked
     * @details Convenience signal for clearing category filter
     */
    void showAllClicked();

private slots:
    /**
     * @brief Called when color preview button is clicked
     */
    void onColorButtonClicked();

    /**
     * @brief Called when stroke width slider value changes
     * @param value The new slider value (5-40, mapped to 0.5-4.0)
     */
    void onStrokeWidthChanged(int value);

    /**
     * @brief Called when size slider value changes
     * @param value The new slider value (16-256)
     */
    void onSizeChanged(int value);

    /**
     * @brief Called when a category list item is clicked
     * @param item The clicked list widget item
     */
    void onCategoryClicked(QListWidgetItem* item);

    /**
     * @brief Called when reset button is clicked
     */
    void onResetClicked();

private:
    /**
     * @brief Initialize the UI layout
     */
    void setupUI();

    /**
     * @brief Create the header section with title and reset button
     * @return The header widget
     */
    QWidget* createHeaderSection();

    /**
     * @brief Create the color customization section
     * @return The color widget
     */
    QWidget* createColorSection();

    /**
     * @brief Create the stroke width customization section
     * @return The stroke width widget
     */
    QWidget* createStrokeWidthSection();

    /**
     * @brief Create the size customization section
     * @return The size widget
     */
    QWidget* createSizeSection();

    /**
     * @brief Create the category selection section
     * @return The category widget
     */
    QWidget* createCategorySection();

    /**
     * @brief Update the color preview button appearance
     */
    void updateColorButtonAppearance();

    /**
     * @brief Update the stroke width value display
     */
    void updateStrokeWidthDisplay();

    /**
     * @brief Update the size value display
     */
    void updateSizeDisplay();

    // UI Components - Header
    QPushButton* m_resetButton;                  ///< Reset all options to default

    // UI Components - Color section
    QLabel* m_colorLabel;                        ///< "Color" label
    QPushButton* m_colorButton;                  ///< Color preview button

    // UI Components - Stroke width section
    QLabel* m_strokeWidthLabel;                  ///< "Stroke width" label
    QLabel* m_strokeWidthValueLabel;             ///< Stroke width value display (e.g., "2px")
    QSlider* m_strokeWidthSlider;                ///< Stroke width slider (0.5-4.0)

    // UI Components - Size section
    QLabel* m_sizeLabel;                         ///< "Size" label
    QLabel* m_sizeValueLabel;                    ///< Size value display (e.g., "24px")
    QSlider* m_sizeSlider;                       ///< Size slider (16-256)

    // UI Components - Category section
    QListWidget* m_categoryListWidget;           ///< Category list with counts
    QMap<QString, int> m_categoryIconCounts;     ///< Cache of category icon counts

    // Current state
    IconOptions m_currentOptions;                ///< Current customization options
    bool m_isDarkTheme;                          ///< Whether dark theme is active
    QColor m_defaultColor;                       ///< Default color based on theme
};

} // namespace gallery

#endif // CATEGORY_SIDEBAR_WIDGET_H
