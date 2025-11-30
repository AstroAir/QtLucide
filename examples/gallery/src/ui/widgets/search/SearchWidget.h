/**
 * @file SearchWidget.h
 * @brief Search bar widget for the gallery application
 * @details Provides a simple search input with magnifying glass icon,
 *          placeholder text, and clear button with keyboard shortcuts.
 * @author Max Qian
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed - Copyright 2025 Max Qian. All Rights Reserved.
 */

#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QString>
#include <QWidget>
#include <memory>

class QLineEdit;
class QPushButton;
class QHBoxLayout;
class QLabel;
class QTimer;

namespace gallery {

/**
 * @class SearchWidget
 * @brief Simple search input widget with debouncing support
 * @details Provides a search bar with:
 *          - Magnifying glass icon on the left
 *          - Clear button when text is present
 *          - Ctrl+K shortcut hint
 *          - Optional debouncing for search queries
 *          - Professional styling
 *
 * @par Usage:
 * @code
 * SearchWidget search;
 * search.setDebounceTime(300);
 * connect(&search, &SearchWidget::searchTextChanged,
 *         this, &MyWidget::onSearch);
 * @endcode
 *
 * @see IconSearchWidget for icon-specific search with filters
 */
class SearchWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct a SearchWidget
     * @param parent The parent widget
     */
    explicit SearchWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~SearchWidget() override;

    /**
     * @brief Get the current search text
     * @return The text in the search input
     */
    [[nodiscard]] QString searchText() const;

    /**
     * @brief Set the search text
     * @param text The text to set
     */
    void setSearchText(const QString& text);

    /**
     * @brief Set the placeholder text
     * @param text The placeholder text to display
     * @default "Search 1634 icons..."
     */
    void setPlaceholderText(const QString& text);

    /**
     * @brief Set the debounce time for search emission
     * @param milliseconds The debounce time in milliseconds
     * @default 300ms
     * @details When debouncing is enabled, the searchTextChanged signal
     *          is delayed until this many milliseconds have passed without
     *          further text changes. This reduces excessive signals while typing.
     */
    void setDebounceTime(int milliseconds);

    /**
     * @brief Get the current debounce time
     * @return The debounce time in milliseconds
     */
    [[nodiscard]] int debounceTime() const;

    /**
     * @brief Clear the search text
     */
    void clear();

    /**
     * @brief Set focus to the search input
     */
    void focusSearchInput();

Q_SIGNALS:
    /**
     * @brief Emitted when search text changes (with optional debouncing)
     * @param text The new search text
     * @details If debounceTime is > 0, this signal is delayed until the user
     *          stops typing. Otherwise it's emitted immediately.
     */
    void searchTextChanged(const QString& text);

protected:
    /**
     * @brief Handle key press events
     * @param event The key event
     * @details Used to implement Ctrl+K shortcut
     */
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    /**
     * @brief Called when search text changes in the input
     * @param text The new text
     */
    void onSearchTextChanged(const QString& text);

    /**
     * @brief Called when clear button is clicked
     */
    void onClearClicked();

    /**
     * @brief Called when debounce timer expires
     */
    void onDebounceTimeout();

private:
    /**
     * @brief Initialize the UI layout
     */
    void setupUI();

    /**
     * @brief Update the visibility of the clear button
     */
    void updateClearButtonVisibility();

    // UI Components
    QLineEdit* m_searchInput;   ///< Main search input field
    QPushButton* m_clearButton; ///< Clear button
    QLabel* m_searchIconLabel;  ///< Magnifying glass icon label
    QLabel* m_shortcutLabel;    ///< Ctrl+K hint label

    // Debouncing
    std::unique_ptr<QTimer> m_debounceTimer; ///< Timer for debouncing
    int m_debounceTime;                      ///< Debounce time in milliseconds
};

} // namespace gallery

#endif // SEARCH_WIDGET_H
