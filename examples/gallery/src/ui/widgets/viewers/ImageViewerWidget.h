/**
 * QtLucide Gallery Application - Enhanced Image Viewer Widget
 *
 * A comprehensive image viewer with zoom, pan, rotation, and slideshow capabilities.
 * Supports all common image formats and provides smooth animations.
 */

#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include <QAction>
#include <QComboBox>
#include <QFileInfo>
#include <QGestureEvent>
#include <QGraphicsOpacityEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMimeDatabase>
#include <QMouseEvent>
#include <QMovie>
#include <QPaintEvent>
#include <QPixmap>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSlider>
#include <QSpinBox>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QWidget>

#include "ContentManager.h"

/**
 * @brief Enhanced graphics view for image display with zoom and pan
 */
class ImageGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit ImageGraphicsView(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);
    void setZoomFactor(double factor);
    void fitToWindow();
    void fitToWidth();
    void fitToHeight();
    void actualSize();

    double getZoomFactor() const { return m_zoomFactor; }
    bool hasImage() const { return m_pixmapItem != nullptr; }

public slots:
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void rotateLeft();
    void rotateRight();
    void flipHorizontal();
    void flipVertical();

signals:
    void zoomChanged(double factor);
    void imageClicked(const QPoint& position);
    void imageDoubleClicked(const QPoint& position);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool event(QEvent* event) override;

private:
    void updateTransform();
    void centerImage();

    QGraphicsScene* m_scene;
    QGraphicsPixmapItem* m_pixmapItem;
    double m_zoomFactor;
    int m_rotation;
    bool m_flippedHorizontal;
    bool m_flippedVertical;

    // Pan support
    bool m_panning;
    QPoint m_lastPanPoint;

    // Animation
    QPropertyAnimation* m_zoomAnimation;
};

/**
 * @brief Main image viewer widget with full functionality
 */
class ImageViewerWidget : public QWidget {
    Q_OBJECT

public:
    enum ViewMode { FitToWindow, FitToWidth, FitToHeight, ActualSize, CustomZoom };
    Q_ENUM(ViewMode)

    enum SlideshowSpeed {
        VerySlow = 10000, // 10 seconds
        Slow = 5000,      // 5 seconds
        Normal = 3000,    // 3 seconds
        Fast = 1500,      // 1.5 seconds
        VeryFast = 1000   // 1 second
    };
    Q_ENUM(SlideshowSpeed)

    explicit ImageViewerWidget(QWidget* parent = nullptr);
    ~ImageViewerWidget();

    // Content management
    void setContentManager(ContentManager* manager);
    void setImageList(const QStringList& imageList);
    void setCurrentImage(const QString& identifier);

    // Display properties
    void setViewMode(ViewMode mode);
    void setZoomFactor(double factor);
    void setBackgroundColor(const QColor& color);
    void setShowImageInfo(bool show);

    // Navigation
    void showNextImage();
    void showPreviousImage();
    void showFirstImage();
    void showLastImage();
    void showImageAt(int index);

    // Slideshow
    void startSlideshow();
    void stopSlideshow();
    void setSlideshowSpeed(SlideshowSpeed speed);
    bool isSlideshowActive() const { return m_slideshowTimer->isActive(); }

    // Transformations
    void rotateLeft();
    void rotateRight();
    void flipHorizontal();
    void flipVertical();
    void resetTransformations();

    // Getters
    QString getCurrentImage() const { return m_currentImage; }
    int getCurrentIndex() const { return m_currentIndex; }
    int getImageCount() const { return static_cast<int>(m_imageList.size()); }
    ViewMode getViewMode() const { return m_viewMode; }
    double getZoomFactor() const;

public slots:
    void onZoomIn();
    void onZoomOut();
    void onResetZoom();
    void onFitToWindow();
    void onFitToWidth();
    void onFitToHeight();
    void onActualSize();
    void onToggleFullscreen();
    void onToggleImageInfo();

signals:
    void imageChanged(const QString& identifier, int index);
    void zoomChanged(double factor);
    void viewModeChanged(ViewMode mode);
    void slideshowStateChanged(bool active);
    void loadingStarted(const QString& identifier);
    void imageLoaded(const QString& identifier); // Added for compatibility
    void loadingFinished(const QString& identifier);
    void loadingFailed(const QString& identifier, const QString& error);

private slots:
    void onSlideshowTimer();
    void onImageLoaded();
    void onZoomSliderChanged(int value);
    void onViewModeChanged();

private:
    // UI setup
    void setupUI();
    void setupToolbar();
    void setupImageView();
    void setupInfoPanel();
    void setupAnimations();

    // Image loading
    void loadCurrentImage();
    void loadImageAsync(const QString& identifier);
    void displayImage(const QPixmap& pixmap);
    void displayError(const QString& message);

    // Navigation helpers
    void updateNavigationActions();
    void updateImageInfo();
    void updateWindowTitle();

    // Animation helpers
    void animateTransition();
    void fadeIn();
    void fadeOut();

    // Core components
    ContentManager* m_contentManager;
    ImageGraphicsView* m_imageView;
    QScrollArea* m_scrollArea;

    // Toolbar and controls
    QToolBar* m_toolbar;
    QAction* m_previousAction;
    QAction* m_nextAction;
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_fitToWindowAction;
    QAction* m_actualSizeAction;
    QAction* m_rotateLeftAction;
    QAction* m_rotateRightAction;
    QAction* m_flipHorizontalAction;
    QAction* m_flipVerticalAction;
    QAction* m_slideshowAction;
    QAction* m_fullscreenAction;
    QAction* m_infoAction;

    QSlider* m_zoomSlider;
    QLabel* m_zoomLabel; // Added for compatibility
    QComboBox* m_viewModeCombo;
    QProgressBar* m_loadingProgress;

    // Info panel
    QWidget* m_infoPanel;
    QLabel* m_imageInfoLabel;
    QLabel* m_navigationLabel;

    // Data
    QStringList m_imageList;
    QString m_currentImage;
    int m_currentIndex;
    ViewMode m_viewMode;

    // Slideshow
    QTimer* m_slideshowTimer;
    SlideshowSpeed m_slideshowSpeed;

    // Animation
    QPropertyAnimation* m_fadeAnimation;
    QGraphicsOpacityEffect* m_opacityEffect;

    // State
    bool m_isFullscreen;
    bool m_showImageInfo;
    QColor m_backgroundColor;

    // Loading
    bool m_isLoading;
    QMovie* m_loadingMovie;
};

#endif // IMAGEVIEWERWIDGET_H
