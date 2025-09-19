# QtLucideIconEngine Class

The QIconEngine implementation providing efficient rendering and caching for Qt's icon system.

## Class Declaration

```cpp
namespace lucide {
    class QtLucideIconEngine : public QIconEngine
    {
    public:
        explicit QtLucideIconEngine(QtLucide* lucide, 
                                   QtLucideIconPainter* painter,
                                   const QVariantMap& options = {});
        ~QtLucideIconEngine();

        // QIconEngine interface
        void paint(QPainter* painter, const QRect& rect, 
                  QIcon::Mode mode, QIcon::State state) override;
        QPixmap pixmap(const QSize& size, QIcon::Mode mode, 
                      QIcon::State state) override;
        QIconEngine* clone() const override;
        QString key() const override;
    };
}
```

## Overview

QtLucideIconEngine is the core rendering engine that implements Qt's QIconEngine interface. It provides:

- **Efficient Caching**: Automatic pixmap caching with intelligent cache keys
- **State-Aware Rendering**: Support for all Qt icon modes and states
- **High-DPI Support**: Proper scaling for high-resolution displays
- **Custom Painter Integration**: Works with QtLucideIconPainter system

## Constructor

### QtLucideIconEngine(QtLucide\* lucide, QtLucideIconPainter\* painter, const QVariantMap& options)

Creates a new icon engine instance.

**Parameters:**

- `lucide` - Pointer to the QtLucide instance (not owned)
- `painter` - Pointer to the icon painter (takes ownership)
- `options` - Rendering options map

**Example:**

```cpp
// Usually created internally by QtLucide::icon()
QtLucideIconEngine* engine = new QtLucideIconEngine(
    lucideInstance, 
    new QtLucideSvgIconPainter("heart"), 
    options
);
QIcon icon(engine);  // QIcon takes ownership
```

## Core Methods

### void paint(QPainter\* painter, const QRect& rect, QIcon::Mode mode, QIcon::State state)

Renders the icon directly to a painter.

**Parameters:**

- `painter` - Target QPainter
- `rect` - Target rectangle
- `mode` - Icon mode (Normal, Disabled, Active, Selected)
- `state` - Icon state (Off, On)

### QPixmap pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state)

Returns a cached pixmap for the specified parameters.

**Parameters:**

- `size` - Requested pixmap size
- `mode` - Icon mode
- `state` - Icon state

**Returns:** Cached or newly rendered QPixmap

### QIconEngine\* clone() const

Creates a copy of this icon engine.

**Returns:** New QtLucideIconEngine instance

### QString key() const

Returns a unique cache key for this icon configuration.

**Returns:** String key based on painter and options

## Caching Strategy

The icon engine implements intelligent caching:

### Cache Key Generation

Cache keys are generated from:

- Icon painter identifier
- Rendering options (color, scale-factor, etc.)
- Size and mode/state parameters

### Cache Management

- **Automatic Caching**: Pixmaps are cached on first render
- **Memory Efficient**: LRU eviction when cache limits are reached
- **Thread-Safe**: Cache operations are protected by mutexes

### Cache Performance

```cpp
// First access - renders and caches
QPixmap pixmap1 = engine->pixmap(QSize(32, 32), QIcon::Normal, QIcon::Off);  // ~1-5ms

// Subsequent access - cache hit
QPixmap pixmap2 = engine->pixmap(QSize(32, 32), QIcon::Normal, QIcon::Off);  // ~0.1ms
```

## Icon Modes and States

### Supported Modes

- **QIcon::Normal** - Default appearance
- **QIcon::Disabled** - Grayed out appearance
- **QIcon::Active** - Mouse hover appearance  
- **QIcon::Selected** - Selection appearance

### Supported States

- **QIcon::Off** - Default state
- **QIcon::On** - Toggle/checked state

### Mode/State Handling

```cpp
// Different appearances for different modes
QPixmap normal = engine->pixmap(size, QIcon::Normal, QIcon::Off);
QPixmap disabled = engine->pixmap(size, QIcon::Disabled, QIcon::Off);
QPixmap active = engine->pixmap(size, QIcon::Active, QIcon::Off);
```

## High-DPI Support

The engine automatically handles high-DPI displays:

```cpp
// Automatically scales for device pixel ratio
QPixmap pixmap = engine->pixmap(QSize(32, 32), mode, state);
// Returns 64x64 pixmap on 2x displays, 96x96 on 3x displays
```

## Integration with Qt

### QIcon Integration

```cpp
// Engine is automatically used by QIcon
QIcon icon(new QtLucideIconEngine(lucide, painter, options));

// Works with all Qt widgets
button->setIcon(icon);
action->setIcon(icon);
```

### Widget Compatibility

Works seamlessly with:

- QPushButton
- QAction (menus/toolbars)
- QLabel
- QTreeWidget/QListWidget items
- Custom widgets using QIcon

## Performance Characteristics

### Rendering Performance

- **Cold render**: 1-5ms (SVG parsing and rendering)
- **Cached render**: 0.1ms (cache lookup)
- **Memory usage**: 2-10KB per unique size/mode/state combination

### Optimization Tips

```cpp
// Pre-render common sizes
engine->pixmap(QSize(16, 16), QIcon::Normal, QIcon::Off);
engine->pixmap(QSize(32, 32), QIcon::Normal, QIcon::Off);

// Reuse QIcon instances
QIcon sharedIcon(engine->clone());
```

## Thread Safety

- **Icon Creation**: Thread-safe after QtLucide initialization
- **Rendering**: Thread-safe pixmap generation
- **Caching**: Protected by internal mutexes

## Memory Management

```cpp
// Engine is owned by QIcon
QIcon icon(new QtLucideIconEngine(...));  // QIcon takes ownership

// Painter is owned by engine
QtLucideIconPainter* painter = new MyPainter();
QtLucideIconEngine* engine = new QtLucideIconEngine(lucide, painter, options);
// painter will be deleted when engine is destroyed
```

## See Also

- [QtLucide Class](qtlucide.md) - Main icon management class
- [QtLucideIconPainter](icon-painter.md) - Custom painter base class
- [User Guide](../user-guide/basic-usage.md) - Usage examples
- [Performance Guide](../user-guide/performance.md) - Optimization strategies
