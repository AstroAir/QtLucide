# API Reference

Complete reference documentation for all QtLucide classes, methods, and enumerations.

## Overview

QtLucide provides a clean, modern API for integrating Lucide icons into Qt applications. The API is designed to be:

- **Intuitive**: Familiar patterns for Qt developers
- **Flexible**: Extensive customization options
- **Performant**: Efficient caching and rendering
- **Extensible**: Custom painter support

## Core Classes

<div class="grid cards" markdown>

- :material-class: **QtLucide**

  ***

  Main class for icon management and creation. Your primary interface to the QtLucide system.

  [:octicons-arrow-right-24: QtLucide Class](qtlucide.md)

- :material-engine-outline: **QtLucideIconEngine**

  ***

  QIconEngine implementation providing efficient rendering and caching for Qt's icon system.

  [:octicons-arrow-right-24: Icon Engine](icon-engine.md)

- :material-brush-outline: **QtLucideIconPainter**

  ***

  Abstract base class for custom icon rendering. Extend this for custom icon painters.

  [:octicons-arrow-right-24: Icon Painter](icon-painter.md)

- :material-format-list-numbered: **Enums & Constants**

  ***

  Icon enumerations, constants, and utility definitions used throughout QtLucide.

  [:octicons-arrow-right-24: Enums & Constants](enums.md)

</div>

## Quick Reference

### Essential Methods

```cpp
// Initialization
lucide::QtLucide lucide;
bool success = lucide.initLucide();

// Basic icon creation
QIcon icon = lucide.icon("heart");
QIcon enumIcon = lucide.icon(lucide::Icons::heart);

// Customized icons
QVariantMap options;
options["color"] = QColor(Qt::red);
QIcon redIcon = lucide.icon("heart", options);

// Default options
lucide.setDefaultOption("color", QColor(Qt::blue));
lucide.setDefaultOption("scale-factor", 1.2);

// Information
QStringList icons = lucide.availableIcons();
int count = lucide::ICON_COUNT;
```

### Common Options

| Option           | Type     | Description               | Default         |
| ---------------- | -------- | ------------------------- | --------------- |
| `color`          | `QColor` | Primary icon color        | `Qt::black`     |
| `color-disabled` | `QColor` | Color when disabled       | Lighter primary |
| `color-active`   | `QColor` | Color when active         | Primary color   |
| `color-selected` | `QColor` | Color when selected       | Primary color   |
| `scale-factor`   | `double` | Size multiplier (0.1-2.0) | `0.9`           |
| `opacity`        | `double` | Icon opacity (0.0-1.0)    | `1.0`           |

### Icon States

QtLucide supports all Qt icon modes and states:

```cpp
// Icon modes
QIcon::Normal    // Default appearance
QIcon::Disabled  // Grayed out appearance
QIcon::Active    // Mouse hover appearance
QIcon::Selected  // Selection appearance

// Icon states
QIcon::Off       // Default state
QIcon::On        // Toggle/checked state
```

## Namespace Structure

```cpp
namespace lucide {
    class QtLucide;
    class QtLucideIconEngine;
    class QtLucideIconPainter;
    class QtLucideSvgIconPainter;

    enum class Icons {
        // 1634+ icon definitions
        home,
        heart,
        settings,
        // ... more icons
    };

    const int ICON_COUNT = 1634;
}
```

## Error Handling

QtLucide uses Qt's standard error handling patterns:

```cpp
// Initialization errors
lucide::QtLucide lucide;
if (!lucide.initLucide()) {
    qWarning() << "Failed to initialize QtLucide";
    // Handle error appropriately
}

// Invalid icon names return empty QIcon
QIcon icon = lucide.icon("invalid-name");
if (icon.isNull()) {
    qWarning() << "Icon not found";
    // Use fallback icon or handle error
}

// Check available icons
QStringList available = lucide.availableIcons();
if (available.contains("my-icon")) {
    QIcon icon = lucide.icon("my-icon");
}
```

## Memory Management

QtLucide follows Qt's memory management conventions:

```cpp
// Stack allocation (recommended)
lucide::QtLucide lucide;
lucide.initLucide();

// Heap allocation with parent
lucide::QtLucide* lucide = new lucide::QtLucide(parentWidget);
// Will be deleted when parent is deleted

// Manual cleanup (if no parent)
lucide::QtLucide* lucide = new lucide::QtLucide();
// ... use lucide
delete lucide;  // Clean up manually
```

## Thread Safety

QtLucide is designed to be thread-safe for icon creation:

```cpp
// Safe to call from multiple threads after initialization
lucide::QtLucide lucide;
lucide.initLucide();  // Call from main thread

// These are thread-safe:
QIcon icon1 = lucide.icon("home");     // Thread A
QIcon icon2 = lucide.icon("settings"); // Thread B

// Not thread-safe (call from main thread only):
lucide.setDefaultOption("color", Qt::red);
lucide.give("custom", new MyPainter());
```

## Performance Characteristics

### Icon Creation

- **First access**: ~1-5ms (SVG parsing and rendering)
- **Cached access**: ~0.1ms (cache lookup)
- **Memory usage**: ~2-10KB per unique icon/size/option combination

### Caching Strategy

- Automatic caching based on icon name, size, and options
- LRU eviction when cache size limits are reached
- Cache keys include all rendering parameters

### Optimization Tips

```cpp
// Pre-create frequently used icons
QIcon homeIcon = lucide.icon("home");
QIcon settingsIcon = lucide.icon("settings");

// Use default options when possible
lucide.setDefaultOption("color", myAppColor);
QIcon icon = lucide.icon("heart");  // Uses default color

// Avoid creating identical icons repeatedly
// Bad:
for (int i = 0; i < 100; ++i) {
    button[i]->setIcon(lucide.icon("star"));  // Creates 100 times
}

// Good:
QIcon starIcon = lucide.icon("star");  // Create once
for (int i = 0; i < 100; ++i) {
    button[i]->setIcon(starIcon);      // Reuse
}
```

## Version Compatibility

| QtLucide Version | Qt Version | C++ Standard | Features             |
| ---------------- | ---------- | ------------ | -------------------- |
| 1.0.x            | Qt 6.0+    | C++17        | Full feature set     |
| Future 1.1.x     | Qt 6.2+    | C++17        | Enhanced performance |
| Future 2.0.x     | Qt 6.5+    | C++20        | New API features     |

## Migration Notes

### From QtAwesome

QtLucide provides a compatible API for easy migration:

```cpp
// QtAwesome
fa::QtAwesome awesome;
awesome.initFontAwesome();
QIcon icon = awesome.icon(fa::fa_solid, fa::fa_heart);

// QtLucide equivalent
lucide::QtLucide lucide;
lucide.initLucide();
QIcon icon = lucide.icon("heart");
```

### API Changes

No breaking changes are planned for the 1.x series. New features will be added in a backward-compatible manner.

## See Also

- [User Guide](../user-guide/index.md) - Practical usage examples
- [Examples](../examples/index.md) - Complete working examples
- [Migration Guide](../migration/index.md) - Migrating from other icon libraries
- [GitHub Repository](https://github.com/AstroAir/QtLucide) - Source code and issues
