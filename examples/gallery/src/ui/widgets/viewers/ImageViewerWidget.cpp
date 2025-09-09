/**
 * QtLucide Gallery Application - Enhanced Image Viewer Widget Implementation
 */

#include "ImageViewerWidget.h"
#include "GalleryLogger.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QSlider>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QFileInfo>
#include <QPixmap>
#include <QMovie>
#include <QMimeDatabase>
#include <QDebug>

// ImageGraphicsView Implementation
ImageGraphicsView::ImageGraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(new QGraphicsScene(this))
    , m_pixmapItem(nullptr)
    , m_zoomFactor(1.0)
    , m_rotation(0)
    , m_flippedHorizontal(false)
    , m_flippedVertical(false)
    , m_panning(false)
    , m_zoomAnimation(new QPropertyAnimation(this))
{
    setScene(m_scene);
    setDragMode(QGraphicsView::RubberBandDrag);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // Enable mouse tracking for pan operations
    setMouseTracking(true);

    // Set background
    setBackgroundBrush(QBrush(QColor(64, 64, 64)));
}

void ImageGraphicsView::setPixmap(const QPixmap& pixmap)
{
    m_scene->clear();
    m_pixmapItem = nullptr;

    if (!pixmap.isNull()) {
        m_pixmapItem = m_scene->addPixmap(pixmap);
        m_scene->setSceneRect(pixmap.rect());
        fitToWindow();
    }
}

void ImageGraphicsView::setZoomFactor(double factor)
{
    if (factor <= 0.0) return;

    m_zoomFactor = factor;
    updateTransform();
    emit zoomChanged(m_zoomFactor);
}

void ImageGraphicsView::fitToWindow()
{
    if (!m_pixmapItem) return;

    fitInView(m_pixmapItem, Qt::KeepAspectRatio);
    QTransform transform = this->transform();
    m_zoomFactor = transform.m11(); // Get scale factor
    emit zoomChanged(m_zoomFactor);
}

void ImageGraphicsView::fitToWidth()
{
    if (!m_pixmapItem) return;

    QRectF itemRect = m_pixmapItem->boundingRect();
    QRectF viewRect = viewport()->rect();

    double scale = viewRect.width() / itemRect.width();
    setZoomFactor(scale);
}

void ImageGraphicsView::fitToHeight()
{
    if (!m_pixmapItem) return;

    QRectF itemRect = m_pixmapItem->boundingRect();
    QRectF viewRect = viewport()->rect();

    double scale = viewRect.height() / itemRect.height();
    setZoomFactor(scale);
}

void ImageGraphicsView::actualSize()
{
    setZoomFactor(1.0);
}

void ImageGraphicsView::zoomIn()
{
    setZoomFactor(m_zoomFactor * 1.25);
}

void ImageGraphicsView::zoomOut()
{
    setZoomFactor(m_zoomFactor / 1.25);
}

void ImageGraphicsView::resetZoom()
{
    setZoomFactor(1.0);
}

void ImageGraphicsView::rotateLeft()
{
    m_rotation -= 90;
    if (m_rotation < 0) m_rotation += 360;
    updateTransform();
}

void ImageGraphicsView::rotateRight()
{
    m_rotation += 90;
    if (m_rotation >= 360) m_rotation -= 360;
    updateTransform();
}

void ImageGraphicsView::flipHorizontal()
{
    m_flippedHorizontal = !m_flippedHorizontal;
    updateTransform();
}

void ImageGraphicsView::flipVertical()
{
    m_flippedVertical = !m_flippedVertical;
    updateTransform();
}

void ImageGraphicsView::updateTransform()
{
    if (!m_pixmapItem) return;

    QTransform transform;
    transform.scale(m_zoomFactor, m_zoomFactor);
    transform.rotate(m_rotation);

    if (m_flippedHorizontal) {
        transform.scale(-1, 1);
    }
    if (m_flippedVertical) {
        transform.scale(1, -1);
    }

    setTransform(transform);
}

void ImageGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        // Zoom with Ctrl+Wheel
        const double scaleFactor = 1.15;
        if (event->angleDelta().y() > 0) {
            setZoomFactor(m_zoomFactor * scaleFactor);
        } else {
            setZoomFactor(m_zoomFactor / scaleFactor);
        }
        event->accept();
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void ImageGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
        if (event->button() == Qt::LeftButton) {
            emit imageClicked(event->pos());
        }
    }
}

void ImageGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();

        QScrollBar* hBar = horizontalScrollBar();
        QScrollBar* vBar = verticalScrollBar();
        hBar->setValue(hBar->value() - delta.x());
        vBar->setValue(vBar->value() - delta.y());

        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void ImageGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ImageGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit imageDoubleClicked(event->pos());
        event->accept();
    } else {
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}

void ImageGraphicsView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_0:
        actualSize();
        break;
    case Qt::Key_F:
        fitToWindow();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}

void ImageGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    // Optionally maintain fit-to-window on resize
}

bool ImageGraphicsView::event(QEvent *event)
{
    // Handle gesture events for touch devices
    return QGraphicsView::event(event);
}

// ImageViewerWidget Implementation
ImageViewerWidget::ImageViewerWidget(QWidget *parent)
    : QWidget(parent)
    , m_contentManager(nullptr)
    , m_imageView(new ImageGraphicsView(this))
    , m_currentIndex(-1)
    , m_viewMode(FitToWindow)
    , m_slideshowTimer(new QTimer(this))
    , m_slideshowSpeed(Normal)
    , m_fadeAnimation(new QPropertyAnimation(this))
    , m_opacityEffect(new QGraphicsOpacityEffect(this))
    , m_isFullscreen(false)
    , m_showImageInfo(true)
    , m_backgroundColor(QColor(64, 64, 64))
    , m_isLoading(false)
    , m_loadingMovie(nullptr)
{
    GALLERY_LOG_INFO(galleryInit, "ImageViewerWidget constructor started");

    setupUI();
    setupAnimations();

    // Connect slideshow timer
    connect(m_slideshowTimer, &QTimer::timeout, this, &ImageViewerWidget::onSlideshowTimer);

    // Connect image view signals
    connect(m_imageView, &ImageGraphicsView::zoomChanged, this, &ImageViewerWidget::zoomChanged);
    connect(m_imageView, &ImageGraphicsView::imageDoubleClicked,
            this, &ImageViewerWidget::onToggleFullscreen);

    GALLERY_LOG_INFO(galleryInit, "ImageViewerWidget initialized");
}

ImageViewerWidget::~ImageViewerWidget()
{
    stopSlideshow();
}

void ImageViewerWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setupToolbar();
    setupImageView();
    setupInfoPanel();

    mainLayout->addWidget(m_toolbar);
    mainLayout->addWidget(m_imageView, 1);
    mainLayout->addWidget(m_infoPanel);
}

void ImageViewerWidget::setupToolbar()
{
    m_toolbar = new QToolBar(this);
    m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolbar->setIconSize(QSize(16, 16));

    // Navigation actions
    m_previousAction = m_toolbar->addAction("Previous");
    m_nextAction = m_toolbar->addAction("Next");
    m_toolbar->addSeparator();

    // Zoom actions
    m_zoomInAction = m_toolbar->addAction("Zoom In");
    m_zoomOutAction = m_toolbar->addAction("Zoom Out");
    m_fitToWindowAction = m_toolbar->addAction("Fit to Window");
    m_actualSizeAction = m_toolbar->addAction("Actual Size");
    m_toolbar->addSeparator();

    // Transform actions
    m_rotateLeftAction = m_toolbar->addAction("Rotate Left");
    m_rotateRightAction = m_toolbar->addAction("Rotate Right");
    m_flipHorizontalAction = m_toolbar->addAction("Flip Horizontal");
    m_flipVerticalAction = m_toolbar->addAction("Flip Vertical");
    m_toolbar->addSeparator();

    // Slideshow and other actions
    m_slideshowAction = m_toolbar->addAction("Slideshow");
    m_fullscreenAction = m_toolbar->addAction("Fullscreen");
    m_infoAction = m_toolbar->addAction("Info");

    // Connect actions
    connect(m_previousAction, &QAction::triggered, this, &ImageViewerWidget::showPreviousImage);
    connect(m_nextAction, &QAction::triggered, this, &ImageViewerWidget::showNextImage);
    connect(m_zoomInAction, &QAction::triggered, this, &ImageViewerWidget::onZoomIn);
    connect(m_zoomOutAction, &QAction::triggered, this, &ImageViewerWidget::onZoomOut);
    connect(m_fitToWindowAction, &QAction::triggered, this, &ImageViewerWidget::onFitToWindow);
    connect(m_actualSizeAction, &QAction::triggered, this, &ImageViewerWidget::onActualSize);
    connect(m_rotateLeftAction, &QAction::triggered, this, &ImageViewerWidget::rotateLeft);
    connect(m_rotateRightAction, &QAction::triggered, this, &ImageViewerWidget::rotateRight);
    connect(m_flipHorizontalAction, &QAction::triggered, this, &ImageViewerWidget::flipHorizontal);
    connect(m_flipVerticalAction, &QAction::triggered, this, &ImageViewerWidget::flipVertical);
    connect(m_slideshowAction, &QAction::triggered, this, [this]() {
        if (isSlideshowActive()) {
            stopSlideshow();
        } else {
            startSlideshow();
        }
    });
    connect(m_fullscreenAction, &QAction::triggered, this, &ImageViewerWidget::onToggleFullscreen);
    connect(m_infoAction, &QAction::triggered, this, &ImageViewerWidget::onToggleImageInfo);
}

void ImageViewerWidget::setupImageView()
{
    // Image view is already created in constructor
    m_imageView->setBackgroundBrush(QBrush(m_backgroundColor));
}

void ImageViewerWidget::setupInfoPanel()
{
    m_infoPanel = new QWidget(this);
    QHBoxLayout* infoLayout = new QHBoxLayout(m_infoPanel);

    m_imageInfoLabel = new QLabel(this);
    m_navigationLabel = new QLabel(this);

    infoLayout->addWidget(m_imageInfoLabel, 1);
    infoLayout->addWidget(m_navigationLabel);

    m_infoPanel->setVisible(m_showImageInfo);
}

void ImageViewerWidget::setupAnimations()
{
    m_fadeAnimation->setTargetObject(m_opacityEffect);
    m_fadeAnimation->setPropertyName("opacity");
    m_fadeAnimation->setDuration(300);

    setGraphicsEffect(m_opacityEffect);
}

void ImageViewerWidget::setContentManager(ContentManager* manager)
{
    m_contentManager = manager;
}

void ImageViewerWidget::setImageList(const QStringList& imageList)
{
    m_imageList = imageList;
    m_currentIndex = -1;
    m_currentImage.clear();
    updateNavigationActions();
}

void ImageViewerWidget::setCurrentImage(const QString& identifier)
{
    if (identifier.isEmpty()) {
        return;
    }

    m_currentImage = identifier;
    m_currentIndex = m_imageList.indexOf(identifier);

    loadCurrentImage();
    updateNavigationActions();
    updateImageInfo();
    updateWindowTitle();

    emit imageChanged(identifier, m_currentIndex);
}

void ImageViewerWidget::loadCurrentImage()
{
    if (m_currentImage.isEmpty()) {
        return;
    }

    emit loadingStarted(m_currentImage);
    m_isLoading = true;

    // Load pixmap directly from file
    QPixmap pixmap(m_currentImage);

    if (!pixmap.isNull()) {
        displayImage(pixmap);
        emit loadingFinished(m_currentImage);
    } else {
        displayError("Failed to load image");
        emit loadingFailed(m_currentImage, "Unable to load image file");
    }

    m_isLoading = false;
}

void ImageViewerWidget::displayImage(const QPixmap& pixmap)
{
    m_imageView->setPixmap(pixmap);

    // Apply view mode
    switch (m_viewMode) {
    case FitToWindow:
        m_imageView->fitToWindow();
        break;
    case FitToWidth:
        m_imageView->fitToWidth();
        break;
    case FitToHeight:
        m_imageView->fitToHeight();
        break;
    case ActualSize:
        m_imageView->actualSize();
        break;
    case CustomZoom:
        // Keep current zoom
        break;
    }
}

void ImageViewerWidget::displayError(const QString& message)
{
    // Create error pixmap
    QPixmap errorPixmap(400, 300);
    errorPixmap.fill(Qt::gray);

    QPainter painter(&errorPixmap);
    painter.setPen(Qt::white);
    painter.drawText(errorPixmap.rect(), Qt::AlignCenter, message);

    m_imageView->setPixmap(errorPixmap);
}

void ImageViewerWidget::showNextImage()
{
    if (m_imageList.isEmpty()) return;

    int nextIndex = (m_currentIndex + 1) % m_imageList.size();
    setCurrentImage(m_imageList.at(nextIndex));
}

void ImageViewerWidget::showPreviousImage()
{
    if (m_imageList.isEmpty()) return;

    int prevIndex = (m_currentIndex - 1 + m_imageList.size()) % m_imageList.size();
    setCurrentImage(m_imageList.at(prevIndex));
}

void ImageViewerWidget::showFirstImage()
{
    if (!m_imageList.isEmpty()) {
        setCurrentImage(m_imageList.first());
    }
}

void ImageViewerWidget::showLastImage()
{
    if (!m_imageList.isEmpty()) {
        setCurrentImage(m_imageList.last());
    }
}

void ImageViewerWidget::showImageAt(int index)
{
    if (index >= 0 && index < m_imageList.size()) {
        setCurrentImage(m_imageList.at(index));
    }
}

void ImageViewerWidget::startSlideshow()
{
    if (m_imageList.isEmpty()) return;

    m_slideshowTimer->start(static_cast<int>(m_slideshowSpeed));
    m_slideshowAction->setText("Stop Slideshow");
    emit slideshowStateChanged(true);
}

void ImageViewerWidget::stopSlideshow()
{
    m_slideshowTimer->stop();
    m_slideshowAction->setText("Start Slideshow");
    emit slideshowStateChanged(false);
}

void ImageViewerWidget::setSlideshowSpeed(SlideshowSpeed speed)
{
    m_slideshowSpeed = speed;
    if (m_slideshowTimer->isActive()) {
        m_slideshowTimer->setInterval(static_cast<int>(speed));
    }
}

void ImageViewerWidget::rotateLeft()
{
    m_imageView->rotateLeft();
}

void ImageViewerWidget::rotateRight()
{
    m_imageView->rotateRight();
}

void ImageViewerWidget::flipHorizontal()
{
    m_imageView->flipHorizontal();
}

void ImageViewerWidget::flipVertical()
{
    m_imageView->flipVertical();
}

void ImageViewerWidget::resetTransformations()
{
    m_imageView->resetZoom();
    // Reset rotation and flips would need additional methods in ImageGraphicsView
}

void ImageViewerWidget::setViewMode(ViewMode mode)
{
    m_viewMode = mode;

    if (m_imageView->hasImage()) {
        switch (mode) {
        case FitToWindow:
            m_imageView->fitToWindow();
            break;
        case FitToWidth:
            m_imageView->fitToWidth();
            break;
        case FitToHeight:
            m_imageView->fitToHeight();
            break;
        case ActualSize:
            m_imageView->actualSize();
            break;
        case CustomZoom:
            // Keep current zoom
            break;
        }
    }

    emit viewModeChanged(mode);
}

double ImageViewerWidget::getZoomFactor() const
{
    return m_imageView->getZoomFactor();
}

void ImageViewerWidget::updateNavigationActions()
{
    bool hasMultiple = m_imageList.size() > 1;

    m_previousAction->setEnabled(hasMultiple);
    m_nextAction->setEnabled(hasMultiple);
    m_slideshowAction->setEnabled(hasMultiple);
}

void ImageViewerWidget::updateImageInfo()
{
    if (m_currentImage.isEmpty()) {
        m_imageInfoLabel->clear();
        m_navigationLabel->clear();
        return;
    }

    // Get basic file info
    QFileInfo fileInfo(m_currentImage);
    QPixmap pixmap(m_currentImage);

    if (!pixmap.isNull()) {
        QString info = QString("%1 (%2x%3)")
                      .arg(fileInfo.fileName())
                      .arg(pixmap.width())
                      .arg(pixmap.height());
        m_imageInfoLabel->setText(info);
    } else {
        m_imageInfoLabel->setText(fileInfo.fileName());
    }

    if (m_currentIndex >= 0 && !m_imageList.isEmpty()) {
        QString nav = QString("%1 of %2").arg(m_currentIndex + 1).arg(m_imageList.size());
        m_navigationLabel->setText(nav);
    }
}

void ImageViewerWidget::updateWindowTitle()
{
    if (m_currentImage.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(m_currentImage);
    QString title = QString("Image Viewer - %1").arg(fileInfo.fileName());

    if (QWidget* topLevel = window()) {
        topLevel->setWindowTitle(title);
    }
}

void ImageViewerWidget::onSlideshowTimer()
{
    showNextImage();
}

void ImageViewerWidget::onZoomIn()
{
    m_imageView->zoomIn();
    m_viewMode = CustomZoom;
}

void ImageViewerWidget::onZoomOut()
{
    m_imageView->zoomOut();
    m_viewMode = CustomZoom;
}

void ImageViewerWidget::onResetZoom()
{
    m_imageView->resetZoom();
    m_viewMode = CustomZoom;
}

void ImageViewerWidget::onFitToWindow()
{
    setViewMode(FitToWindow);
}

void ImageViewerWidget::onFitToWidth()
{
    setViewMode(FitToWidth);
}

void ImageViewerWidget::onFitToHeight()
{
    setViewMode(FitToHeight);
}

void ImageViewerWidget::onActualSize()
{
    setViewMode(ActualSize);
}

void ImageViewerWidget::onToggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;

    if (m_isFullscreen) {
        showFullScreen();
        m_toolbar->hide();
        m_infoPanel->hide();
    } else {
        showNormal();
        m_toolbar->show();
        if (m_showImageInfo) {
            m_infoPanel->show();
        }
    }
}

void ImageViewerWidget::onToggleImageInfo()
{
    m_showImageInfo = !m_showImageInfo;
    m_infoPanel->setVisible(m_showImageInfo && !m_isFullscreen);
    m_infoAction->setChecked(m_showImageInfo);
}

void ImageViewerWidget::onImageLoaded()
{
    // Placeholder for image loaded handling
    qDebug() << "Image loaded";
}

void ImageViewerWidget::onZoomSliderChanged(int value)
{
    // Placeholder for zoom slider handling
    Q_UNUSED(value)
    qDebug() << "Zoom slider changed:" << value;
}

void ImageViewerWidget::onViewModeChanged()
{
    // Placeholder for view mode change handling
    qDebug() << "View mode changed";
}
