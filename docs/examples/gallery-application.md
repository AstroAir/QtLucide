# Icon Gallery Application

The QtLucide Gallery is a comprehensive example application that demonstrates advanced usage of QtLucide while providing a practical tool for browsing and exploring all available icons.

## Overview

The Gallery application showcases:

- **Complete Icon Browser** - Browse all 1,634 Lucide icons
- **Advanced Search** - Find icons by name, category, or keywords
- **Category Filtering** - Filter icons by functional categories
- **Icon Details** - View icon properties, SVG data, and usage examples
- **Copy Functionality** - Copy icon names and code snippets
- **Theme Support** - Dark and light theme compatibility
- **Performance Optimization** - Efficient rendering of large icon sets

## Features

### Icon Grid Display

The main interface displays icons in a responsive grid layout:

```cpp
// IconGridWidget provides efficient icon display
class IconGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IconGridWidget(QWidget* parent = nullptr);

    void setIcons(const QStringList& iconNames);
    void setIconSize(int size);
    void setColumnsPerRow(int columns);

signals:
    void iconSelected(const QString& iconName);
    void iconDoubleClicked(const QString& iconName);
};
```

### Search and Filtering

Advanced search capabilities help users find the right icons:

```cpp
// SearchWidget provides real-time icon filtering
class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget* parent = nullptr);

    void setSearchPlaceholder(const QString& placeholder);

signals:
    void searchTextChanged(const QString& text);
    void searchCleared();
};
```

### Category Management

Icons are organized into logical categories:

```cpp
// CategoryFilterWidget manages icon categorization
class CategoryFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CategoryFilterWidget(QWidget* parent = nullptr);

    void setCategories(const QStringList& categories);
    void setSelectedCategory(const QString& category);

signals:
    void categorySelected(const QString& category);
    void allCategoriesSelected();
};
```

### Icon Details Panel

Detailed information about selected icons:

```cpp
// IconDetailsPanel shows comprehensive icon information
class IconDetailsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit IconDetailsPanel(QWidget* parent = nullptr);

    void setSelectedIcon(const QString& iconName);
    void showIconPreview(const QIcon& icon);
    void showSvgData(const QByteArray& svgData);

signals:
    void copyIconName();
    void copyCodeSnippet();
    void exportIcon();
};
```

## Building the Gallery

### CMake

```bash
cd QtLucide
mkdir build && cd build
cmake .. -DQTLUCIDE_BUILD_EXAMPLES=ON
cmake --build .

# Run the gallery
./examples/gallery/QtLucideGallery
```

### Meson

```bash
cd QtLucide
meson setup builddir -Dexamples=true
meson compile -C builddir

# Run the gallery
./builddir/examples/gallery/QtLucideGallery
```

### XMake

```bash
cd QtLucide
xmake config --examples=true
xmake build

# Run the gallery
xmake run QtLucideGallery
```

## Usage Examples

### Basic Icon Browser

The Gallery demonstrates how to create an efficient icon browser:

```cpp
void GalleryMainWindow::setupIconGrid()
{
    // Initialize QtLucide
    m_lucide = new lucide::QtLucide(this);
    if (!m_lucide->initLucide()) {
        QMessageBox::critical(this, "Error", "Failed to initialize QtLucide");
        return;
    }

    // Get all available icons
    QStringList allIcons = m_lucide->availableIcons();

    // Setup the icon grid
    m_iconGrid = new IconGridWidget(this);
    m_iconGrid->setIcons(allIcons);
    m_iconGrid->setIconSize(48);

    // Connect signals
    connect(m_iconGrid, &IconGridWidget::iconSelected,
            this, &GalleryMainWindow::onIconSelected);
}
```

### Search Implementation

Real-time search with performance optimization:

```cpp
void GalleryMainWindow::onSearchTextChanged(const QString& searchText)
{
    if (searchText.isEmpty()) {
        m_iconGrid->setIcons(m_allIcons);
        return;
    }

    // Filter icons based on search text
    QStringList filteredIcons;
    for (const QString& iconName : m_allIcons) {
        if (iconName.contains(searchText, Qt::CaseInsensitive)) {
            filteredIcons.append(iconName);
        }
    }

    m_iconGrid->setIcons(filteredIcons);
    updateStatusBar(filteredIcons.size());
}
```

### Icon Export Functionality

Export icons in various formats:

```cpp
void GalleryMainWindow::exportSelectedIcon()
{
    if (m_selectedIcon.isEmpty()) {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, "Export Icon", m_selectedIcon + ".svg",
        "SVG Files (*.svg);;PNG Files (*.png);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    if (fileName.endsWith(".svg")) {
        // Export as SVG
        QByteArray svgData = m_lucide->svgData(m_selectedIcon);
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(svgData);
        }
    } else {
        // Export as raster image
        QIcon icon = m_lucide->icon(m_selectedIcon);
        QPixmap pixmap = icon.pixmap(256, 256);
        pixmap.save(fileName);
    }
}
```

## Performance Considerations

The Gallery application demonstrates several performance optimization techniques:

### Lazy Loading

Icons are loaded on-demand to reduce startup time:

```cpp
void IconGridWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Only render visible icons
    QRect visibleRect = event->rect();
    int startRow = visibleRect.top() / m_itemHeight;
    int endRow = (visibleRect.bottom() + m_itemHeight - 1) / m_itemHeight;

    for (int row = startRow; row <= endRow && row < m_rowCount; ++row) {
        renderRow(painter, row);
    }
}
```

### Icon Caching

Frequently used icons are cached for better performance:

```cpp
QIcon IconGridWidget::getCachedIcon(const QString& iconName)
{
    static QHash<QString, QIcon> iconCache;

    if (!iconCache.contains(iconName)) {
        iconCache[iconName] = m_lucide->icon(iconName);

        // Limit cache size
        if (iconCache.size() > 500) {
            iconCache.clear();
        }
    }

    return iconCache[iconName];
}
```

## Customization

The Gallery application can be customized for specific use cases:

### Custom Categories

Add your own icon categories:

```cpp
void GalleryMainWindow::setupCustomCategories()
{
    QHash<QString, QStringList> customCategories;

    customCategories["UI Controls"] = {
        "button", "slider", "toggle-left", "toggle-right",
        "checkbox", "radio", "input"
    };

    customCategories["Data Visualization"] = {
        "bar-chart", "line-chart", "pie-chart", "trending-up",
        "trending-down", "activity"
    };

    m_categoryFilter->setCustomCategories(customCategories);
}
```

### Theme Integration

Integrate with your application's theme system:

```cpp
void GalleryMainWindow::applyTheme(const QString& themeName)
{
    if (themeName == "dark") {
        // Set dark theme colors for icons
        m_lucide->setDefaultOption("color", QColor(220, 220, 220));
        m_lucide->setDefaultOption("color-disabled", QColor(100, 100, 100));
    } else {
        // Set light theme colors
        m_lucide->setDefaultOption("color", QColor(64, 64, 64));
        m_lucide->setDefaultOption("color-disabled", QColor(180, 180, 180));
    }

    // Refresh the icon grid
    m_iconGrid->refreshIcons();
}
```

## Learning from the Gallery

The Gallery application serves as an excellent learning resource for:

- **Large-scale icon management**
- **Performance optimization techniques**
- **User interface design patterns**
- **Search and filtering implementation**
- **Theme and customization support**

Study the Gallery source code in `examples/gallery/` to understand advanced QtLucide usage patterns and best practices.

## See Also

- [Basic Usage Example](basic-usage.md) - Simple QtLucide integration
- [Performance Guide](../user-guide/performance.md) - Optimization strategies
- [API Reference](../api/index.md) - Complete API documentation
- [User Guide](../user-guide/index.md) - Comprehensive usage guide
