/**
 * @file ImageViewerWidget.h
 * @brief Simple icon/image viewer widget
 * @details Provides a widget for displaying icons with zoom controls and
 *          background color selection.
 * @author QtLucide Contributors
 * @date 2025
 * @version 1.0
 * @copyright MIT Licensed
 */

#ifndef IMAGE_VIEWER_WIDGET_H
#define IMAGE_VIEWER_WIDGET_H

#include <QColor>
#include <QPixmap>
#include <QString>
#include <QWidget>

class QLabel;
class QPushButton;
class QSlider;
class QComboBox;
class QVBoxLayout;
class QHBoxLayout;

namespace gallery {

/**
 * @enum BackgroundMode
 * @brief Background display mode for the viewer
 */
enum class BackgroundMode {
    Transparent = 0, ///< Transparent background (checkered pattern)
    White = 1,       ///< White background
    Black = 2        ///< Black background
};

/**
 * @class ImageViewerWidget
 * @brief Widget for viewing and previewing icons/images
 * @details Provides functionality to:
 *          - Display a single icon at custom size
 *          - Zoom in/out with controls
 *          - Change background color (transparent, white, black)
 *          - Display icon information
 *
 * @par Usage:
 * @code
 * ImageViewerWidget viewer;
 * viewer.setImage(pixmap);
 * viewer.setZoomLevel(100);
 * @endcode
 */
class ImageViewerWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Construct an ImageViewerWidget
     * @param parent Parent widget
     */
    explicit ImageViewerWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ImageViewerWidget() override;

    /**
     * @brief Set the image/icon to display
     * @param pixmap The QPixmap to display
     */
    void setImage(const QPixmap& pixmap);

    /**
     * @brief Set the zoom level
     * @param zoomPercent Zoom level as percentage (50-400)
     */
    void setZoomLevel(int zoomPercent);

    /**
     * @brief Get the current zoom level
     * @return Current zoom level as percentage
     */
    [[nodiscard]] int getZoomLevel() const;

    /**
     * @brief Set the background mode
     * @param mode The background mode to use
     */
    void setBackgroundMode(BackgroundMode mode);

    /**
     * @brief Get the current background mode
     * @return Current background mode
     */
    [[nodiscard]] BackgroundMode getBackgroundMode() const;

    /**
     * @brief Set image information text
     * @param info Text to display (e.g., "Icon: house (48x48)")
     */
    void setImageInfo(const QString& info);

Q_SIGNALS:
    /**
     * @brief Emitted when zoom level changes
     * @param zoomPercent The new zoom level
     */
    void zoomChanged(int zoomPercent);

    /**
     * @brief Emitted when background mode changes
     * @param mode The new background mode
     */
    void backgroundChanged(BackgroundMode mode);

private Q_SLOTS:
    /**
     * @brief Called when zoom in button is clicked
     */
    void onZoomIn();

    /**
     * @brief Called when zoom out button is clicked
     */
    void onZoomOut();

    /**
     * @brief Called when zoom slider value changes
     * @param value The new slider value
     */
    void onZoomSliderChanged(int value);

    /**
     * @brief Called when background selection changes
     * @param index The selected background mode index
     */
    void onBackgroundChanged(int index);

    /**
     * @brief Reset zoom to 100%
     */
    void onZoomReset();

private:
    /**
     * @brief Initialize the widget UI
     */
    void setupUI();

    /**
     * @brief Update the displayed image based on current zoom and background
     */
    void updateDisplay();

    /**
     * @brief Create a checkered pattern for transparent backgrounds
     * @param size The size of the pattern
     * @return QPixmap with checkered pattern
     */
    [[nodiscard]] QPixmap createCheckerPattern(int size) const;

    /**
     * @brief Apply background color to image
     * @param source The source pixmap
     * @param bg The background color
     * @return Pixmap with background applied
     */
    [[nodiscard]] QPixmap applyBackground(const QPixmap& source, const QColor& bg) const;

    // UI Components
    QLabel* m_imageLabel;           ///< Label displaying the image
    QLabel* m_infoLabel;            ///< Information label
    QPushButton* m_zoomInButton;    ///< Zoom in button
    QPushButton* m_zoomOutButton;   ///< Zoom out button
    QPushButton* m_zoomResetButton; ///< Reset zoom button
    QSlider* m_zoomSlider;          ///< Zoom level slider
    QLabel* m_zoomLabel;            ///< Zoom percentage label
    QComboBox* m_backgroundCombo;   ///< Background mode selection

    // State
    QPixmap m_originalPixmap;        ///< Original image
    int m_zoomLevel;                 ///< Current zoom level (%)
    BackgroundMode m_backgroundMode; ///< Current background mode
};

} // namespace gallery

#endif // IMAGE_VIEWER_WIDGET_H
