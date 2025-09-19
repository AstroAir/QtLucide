# QtLucideIconPainter Class

Abstract base class for custom icon rendering implementations.

## Class Declaration

```cpp
namespace lucide {
    class QtLucideIconPainter
    {
    public:
        QtLucideIconPainter();
        virtual ~QtLucideIconPainter();

        // Pure virtual interface
        virtual void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
                          QIcon::Mode mode, QIcon::State state,
                          const QVariantMap& options) = 0;
        virtual QString iconText() const = 0;
        virtual QtLucideIconPainter* clone() const = 0;
    };

    class QtLucideSvgIconPainter : public QtLucideIconPainter
    {
    public:
        explicit QtLucideSvgIconPainter(const QString& iconName);
        explicit QtLucideSvgIconPainter(Icons iconId);
        
        void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
                  QIcon::Mode mode, QIcon::State state,
                  const QVariantMap& options) override;
        QString iconText() const override;
        QtLucideIconPainter* clone() const override;
    };
}
```

## Overview

QtLucideIconPainter provides the interface for custom icon rendering implementations. It allows developers to:

- **Create Custom Renderers**: Implement completely custom icon drawing logic
- **Extend SVG Rendering**: Modify or enhance the default SVG rendering
- **Add Effects**: Implement shadows, glows, or other visual effects
- **Support New Formats**: Add support for additional icon formats

## Base Class Interface

### Pure Virtual Methods

#### void paint(QtLucide\* lucide, QPainter\* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state, const QVariantMap& options)

Renders the icon to the specified painter and rectangle.

**Parameters:**

- `lucide` - QtLucide instance for accessing icon data
- `painter` - Target QPainter for rendering
- `rect` - Target rectangle for the icon
- `mode` - Icon mode (Normal, Disabled, Active, Selected)
- `state` - Icon state (Off, On)
- `options` - Rendering options map

#### QString iconText() const

Returns a unique identifier for this painter instance.

**Returns:** String identifier used for caching and debugging

#### QtLucideIconPainter\* clone() const

Creates a copy of this painter.

**Returns:** New painter instance (caller takes ownership)

## Default SVG Painter

### QtLucideSvgIconPainter

The default implementation that renders Lucide SVG icons.

#### Constructors

```cpp
// Create by icon name
QtLucideSvgIconPainter painter("heart");

// Create by enum
QtLucideSvgIconPainter painter(Icons::heart);
```

#### Features

- **SVG Rendering**: High-quality vector rendering
- **Color Customization**: Runtime color changes
- **State Handling**: Different appearances for different modes
- **Scaling**: Perfect quality at any size

## Creating Custom Painters

### Basic Custom Painter

```cpp
class MyCustomPainter : public QtLucideIconPainter
{
public:
    MyCustomPainter(const QString& text) : m_text(text) {}

    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state,
               const QVariantMap& options) override
    {
        // Get color from options
        QColor color = options.value("color", Qt::black).value<QColor>();
        
        // Adjust color based on mode
        if (mode == QIcon::Disabled) {
            color = color.lighter(150);
        } else if (mode == QIcon::Active) {
            color = color.darker(110);
        }
        
        // Draw custom content
        painter->setPen(color);
        painter->setFont(QFont("Arial", rect.height() * 0.6));
        painter->drawText(rect, Qt::AlignCenter, m_text);
    }

    QString iconText() const override
    {
        return QString("custom-text-%1").arg(m_text);
    }

    QtLucideIconPainter* clone() const override
    {
        return new MyCustomPainter(m_text);
    }

private:
    QString m_text;
};
```

### Advanced Custom Painter

```cpp
class GradientIconPainter : public QtLucideIconPainter
{
public:
    GradientIconPainter(const QString& iconName, 
                       const QColor& startColor, 
                       const QColor& endColor)
        : m_iconName(iconName), m_startColor(startColor), m_endColor(endColor) {}

    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state,
               const QVariantMap& options) override
    {
        // Create gradient
        QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
        gradient.setColorAt(0, m_startColor);
        gradient.setColorAt(1, m_endColor);
        
        // Adjust for mode
        if (mode == QIcon::Disabled) {
            gradient.setColorAt(0, m_startColor.lighter(150));
            gradient.setColorAt(1, m_endColor.lighter(150));
        }
        
        // Get SVG data and render with gradient
        QByteArray svgData = lucide->svgData(m_iconName);
        if (!svgData.isEmpty()) {
            // Custom SVG rendering with gradient fill
            renderSvgWithGradient(painter, rect, svgData, gradient);
        }
    }

    QString iconText() const override
    {
        return QString("gradient-%1-%2-%3")
            .arg(m_iconName)
            .arg(m_startColor.name())
            .arg(m_endColor.name());
    }

    QtLucideIconPainter* clone() const override
    {
        return new GradientIconPainter(m_iconName, m_startColor, m_endColor);
    }

private:
    QString m_iconName;
    QColor m_startColor;
    QColor m_endColor;
    
    void renderSvgWithGradient(QPainter* painter, const QRect& rect,
                              const QByteArray& svgData, 
                              const QLinearGradient& gradient);
};
```

## Using Custom Painters

### Direct Usage

```cpp
// Create custom painter
MyCustomPainter* painter = new MyCustomPainter("★");

// Create icon with custom painter
QIcon icon = lucide.icon(painter, options);

// Use in UI
button->setIcon(icon);
```

### Registration with QtLucide

```cpp
// Register custom painter for a name
lucide.give("star-text", new MyCustomPainter("★"));

// Use like any other icon
QIcon icon = lucide.icon("star-text");
```

## Rendering Options

Custom painters receive all rendering options:

### Common Options

- `color` - Primary icon color
- `color-disabled` - Color for disabled state
- `color-active` - Color for active state
- `color-selected` - Color for selected state
- `scale-factor` - Size multiplier
- `opacity` - Icon opacity

### Custom Options

```cpp
// Add custom options
QVariantMap options;
options["color"] = QColor(Qt::blue);
options["shadow-color"] = QColor(Qt::gray);
options["shadow-offset"] = QPoint(2, 2);
options["border-width"] = 2;

QIcon icon = lucide.icon(customPainter, options);
```

## Performance Considerations

### Caching

- Painters are cached based on their `iconText()` return value
- Make `iconText()` unique for different configurations
- Avoid expensive operations in `iconText()`

### Rendering Optimization

```cpp
void paint(...) override
{
    // Cache expensive computations
    static QHash<QString, QPixmap> cache;
    QString key = QString("%1-%2x%3").arg(iconText()).arg(rect.width()).arg(rect.height());
    
    if (cache.contains(key)) {
        painter->drawPixmap(rect, cache[key]);
        return;
    }
    
    // Expensive rendering...
    QPixmap pixmap = renderExpensiveIcon(rect.size());
    cache[key] = pixmap;
    painter->drawPixmap(rect, pixmap);
}
```

## Thread Safety

- **Painter Creation**: Thread-safe
- **Rendering**: Must be called from GUI thread (QPainter requirement)
- **Cloning**: Thread-safe

## Memory Management

```cpp
// QtLucide takes ownership when using give()
lucide.give("custom", new MyPainter());  // QtLucide owns painter

// QtLucideIconEngine takes ownership when creating icons
QIcon icon = lucide.icon(new MyPainter());  // Engine owns painter

// Manual cleanup only if not passed to QtLucide
MyPainter* painter = new MyPainter();
// ... use painter
delete painter;  // Manual cleanup required
```

## See Also

- [QtLucide Class](qtlucide.md) - Main icon management class
- [QtLucideIconEngine](icon-engine.md) - Icon rendering engine
- [User Guide](../user-guide/basic-usage.md) - Usage examples
- [Examples](../examples/index.md) - Complete working examples
