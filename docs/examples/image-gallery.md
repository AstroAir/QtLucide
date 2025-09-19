# Image Gallery Extension for QtLucide

The QtLucide Gallery application has been enhanced with comprehensive image gallery functionality, extending beyond icon browsing to support general image viewing and management.

## Overview

The enhanced Gallery application now provides:

- **Unified Content Management** - Browse both icons and regular images in a single interface
- **Advanced Image Viewer** - High-quality image display with zoom, pan, rotation, and flip
- **File Browser** - Intuitive directory navigation with thumbnail previews
- **Image Transformations** - Rotate, flip, scale, and zoom images with smooth animations
- **Slideshow Mode** - Automatic image progression with configurable timing
- **Performance Optimization** - Lazy loading, caching, and efficient memory management
- **Format Support** - PNG, JPEG, GIF, BMP, SVG, WebP, TIFF, ICO, and more

## Architecture

### Core Components

#### ContentManager

Unified manager for both icons and images, providing a single interface for content access:

```cpp
ContentManager* manager = new ContentManager(this);
manager->setLucide(lucide);
manager->setImageMetadataManager(imageManager);

// Access content uniformly
QPixmap pixmap = manager->getPixmap("icon-name", QSize(48, 48));
QPixmap image = manager->getPixmap("/path/to/image.jpg", QSize(200, 200));
```

#### ImageMetadataManager

Handles metadata extraction and thumbnail generation for image files:

```cpp
ImageMetadataManager* imageManager = new ImageMetadataManager(this);
imageManager->loadDirectory("/path/to/images");

// Get image information
ImageMetadata metadata = imageManager->getImageMetadata("/path/to/image.jpg");
QPixmap thumbnail = imageManager->getThumbnail("/path/to/image.jpg", QSize(128, 128));
```

#### ImageViewerWidget

Advanced image viewer with full transformation capabilities:

```cpp
ImageViewerWidget* viewer = new ImageViewerWidget(this);
viewer->setContentManager(contentManager);
viewer->setCurrentImage("/path/to/image.jpg");

// Control viewing
viewer->setViewMode(ImageViewerWidget::FitToWindow);
viewer->rotateLeft();
viewer->startSlideshow();
```

#### FileBrowserWidget

File system browser with image filtering and thumbnail support:

```cpp
FileBrowserWidget* browser = new FileBrowserWidget(this);
browser->setContentManager(contentManager);
browser->setShowOnlyImages(true);
browser->setCurrentDirectory("/path/to/images");
```

## Features

### Image Display and Navigation

- **High-Quality Rendering** - Smooth scaling with antialiasing
- **Multiple View Modes** - Fit to window, actual size, fit to width/height
- **Zoom and Pan** - Mouse wheel zoom, middle-click pan, keyboard shortcuts
- **Navigation Controls** - Previous/next buttons, keyboard navigation
- **Fullscreen Mode** - Distraction-free image viewing

### Image Transformations

- **Rotation** - 90-degree increments with smooth animation
- **Flipping** - Horizontal and vertical flip operations
- **Scaling** - Precise zoom control with quality preservation
- **Reset Functions** - Restore original orientation and size

### File Management

- **Directory Browsing** - Tree view with expandable folders
- **Thumbnail Grid** - Configurable thumbnail sizes
- **File Filtering** - Show only images or all files
- **Bookmarks** - Quick access to favorite directories
- **Search** - Filter files by name pattern

### Performance Features

- **Lazy Loading** - Load images on demand
- **Smart Caching** - Memory-efficient thumbnail and metadata caching
- **Background Processing** - Non-blocking metadata extraction
- **Viewport Culling** - Only render visible thumbnails

## Usage Examples

### Basic Image Viewing

```cpp
#include "ImageViewerWidget.h"
#include "ContentManager.h"

// Create viewer
ImageViewerWidget* viewer = new ImageViewerWidget(this);
ContentManager* manager = new ContentManager(this);
viewer->setContentManager(manager);

// Load and display image
viewer->setCurrentImage("/path/to/image.jpg");
```

### Directory Browsing

```cpp
#include "FileBrowserWidget.h"

// Create browser
FileBrowserWidget* browser = new FileBrowserWidget(this);
browser->setShowOnlyImages(true);
browser->setCurrentDirectory("/path/to/images");

// Connect to viewer
connect(browser, &FileBrowserWidget::imageSelected,
        viewer, &ImageViewerWidget::setCurrentImage);
```

### Slideshow Implementation

```cpp
// Configure slideshow
viewer->setSlideshowSpeed(ImageViewerWidget::Normal);
viewer->startSlideshow();

// Control slideshow
connect(startButton, &QPushButton::clicked, viewer, &ImageViewerWidget::startSlideshow);
connect(stopButton, &QPushButton::clicked, viewer, &ImageViewerWidget::stopSlideshow);
```

### Image Transformations

```cpp
// Rotation
viewer->rotateLeft();
viewer->rotateRight();

// Flipping
viewer->flipHorizontal();
viewer->flipVertical();

// Zoom control
viewer->setZoomFactor(2.0);  // 200% zoom
viewer->fitToWindow();
viewer->actualSize();
```

## Integration with Existing Gallery

The image gallery functionality integrates seamlessly with the existing icon gallery:

1. **Unified Interface** - Switch between icon and image modes
2. **Shared Components** - Reuse search, filtering, and export functionality
3. **Consistent UI** - Maintain the same look and feel
4. **Performance** - Leverage existing optimization techniques

## Building and Testing

### Build Requirements

- Qt 6.0 or later
- QtLucide library
- Optional: Qt Concurrent for background processing

### CMake Configuration

The image gallery components are automatically included in the gallery build:

```cmake
# Image gallery components are included in GALLERY_SOURCES
add_executable(QtLucideGallery ${GALLERY_SOURCES})
target_link_libraries(QtLucideGallery PRIVATE QtLucide Qt6::Widgets Qt6::Svg)
```

### Testing

A test application is provided to verify functionality:

```bash
# Build and run the test
cd examples/gallery
mkdir build && cd build
cmake ..
make
./test_image_gallery
```

## Supported Formats

The image gallery supports all formats provided by Qt's image plugins:

- **Raster Formats**: PNG, JPEG, GIF, BMP, WebP, TIFF, ICO, XPM, PBM, PGM, PPM
- **Vector Formats**: SVG (with Qt SVG module)
- **Animated Formats**: GIF animations (basic support)

## Performance Considerations

### Memory Management

- Thumbnail cache with configurable size limits
- Automatic cache cleanup for unused images
- Lazy loading of large images

### Threading

- Background metadata extraction (when Qt Concurrent available)
- Non-blocking thumbnail generation
- Responsive UI during heavy operations

### Optimization Tips

1. **Thumbnail Size** - Use appropriate thumbnail sizes for your use case
2. **Cache Limits** - Adjust cache sizes based on available memory
3. **Directory Size** - Consider pagination for very large directories
4. **Image Formats** - Prefer efficient formats like WebP for thumbnails

## Future Enhancements

Planned improvements include:

- **EXIF Data Display** - Show camera settings and metadata
- **Image Editing** - Basic editing tools (crop, adjust brightness/contrast)
- **Batch Operations** - Process multiple images simultaneously
- **Cloud Integration** - Support for cloud storage services
- **Advanced Search** - Search by image properties and content

## See Also

- [Gallery Application](gallery-application.md) - Original icon gallery documentation
- [Performance Guide](../user-guide/performance.md) - Optimization strategies
- [API Reference](../api/index.md) - Complete API documentation
