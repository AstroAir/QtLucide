# QtLucideIconEngine Class

The QIconEngine implementation providing efficient rendering and caching for Qt's icon system.

## Class Declaration

```cpp
namespace lucide {
    class QtLucideIconEngine : public QIconEngine
    {
    public:
        QtLucideIconEngine(QtLucide* lucide,
                          QtLucideIconPainter* painter,
                          const QVariantMap& options);
        ~QtLucideIconEngine() override;

        // QIconEngine interface
        void paint(QPainter* painter, const QRect& rect,
                  QIcon::Mode mode, QIcon::State state) override;
        [[nodiscard]] QPixmap pixmap(const QSize& size, QIcon::Mode mode,
                                     QIcon::State state) override;
        [[nodiscard]] QIconEngine* clone() const override;
        [[nodiscard]] QString key() const override;
        [[nodiscard]] QSize actualSize(const QSize& size, QIcon::Mode mode,
                                       QIcon::State state) override;
        [[nodiscard]] QList<QSize> availableSizes(QIcon::Mode mode,
                                                   QIcon::State state) override;
        [[nodiscard]] QString iconName() override;
        [[nodiscard]] bool isNull() override;
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
- `painter` - Pointer to the icon painter (shared, not owned by engine)
- `options` - Rendering options map (includes merged default options)

**Example:**

```cpp
// Usually created internally by QtLucide::icon()
// The painter is shared with QtLucide, not owned by the engine
QtLucideIconEngine* engine = new QtLucideIconEngine(
    lucideInstance,
    svgPainter,  // Shared painter from QtLucide
    mergedOptions
);
QIcon icon(engine);  // QIcon takes ownership of engine
```

!!! note "Ownership Semantics"
    The engine does not own the painter - it's shared with QtLucide. The painter lifetime is managed by `QtLucide::m_svgIconPainter` or `m_customPainters`.

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

Returns a unique identifier for this engine type.

**Returns:** String `"QtLucideIconEngine"`

### QSize actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state)

Returns the actual size the icon will be rendered at, accounting for scale factor.

**Returns:** Scaled size based on `scale-factor` option (clamped to 0.1-10.0)

### QList\<QSize\> availableSizes(QIcon::Mode mode, QIcon::State state)

Returns available icon sizes.

**Returns:** Empty list (SVG icons are infinitely scalable)

### QString iconName()

Returns the icon name if available.

**Returns:** String in format `"lucide-{iconId}"` or empty string

### bool isNull()

Checks if this icon engine has valid icon data.

**Returns:** `true` if painter or lucide is null, or if iconId is invalid for built-in icons. Custom painters are always valid if the painter exists.

## Caching Strategy

The icon engine implements intelligent caching:

### Cache Key Generation

Cache keys are generated from:

- Icon ID (`iconId` option)
- Size (width × height)
- Mode and state (as integers)
- Color for the specific mode/state
- Scale factor
- Opacity
- Stroke width

**Cache Key Format:**

```text
{iconId}_{width}x{height}_{mode}_{state}_{color}_{scale}_{opacity}_{strokeWidth}
```

### Cache Management

- **Automatic Caching**: Pixmaps are cached on first render
- **Cache Limit**: Maximum 100 entries per engine instance
- **Per-Engine Cache**: Each engine maintains its own pixmap cache
- **Clone Behavior**: Cloned engines start with empty caches

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

// Painter is NOT owned by engine - it's shared with QtLucide
// The painter lifetime is managed by QtLucide
QtLucideIconEngine* engine = new QtLucideIconEngine(lucide, sharedPainter, options);
// sharedPainter will NOT be deleted when engine is destroyed
```

!!! warning "Painter Lifetime"
    The engine maintains a reference to the painter but does not own it. The painter must remain valid for the lifetime of all engines using it. This is automatically handled when using `QtLucide::icon()`.

## See Also

- [QtLucide Class](qtlucide.md) - Main icon management class
- [QtLucideIconPainter](icon-painter.md) - Custom painter base class
- [User Guide](../user-guide/basic-usage.md) - Usage examples
- [Performance Guide](../user-guide/performance.md) - Optimization strategies
