# Icon Customization

Learn how to customize QtLucide icons with colors, scaling, effects, and state-specific styling.

## Overview

QtLucide provides extensive customization options that allow you to adapt icons to your application's design and branding. All customization is done through the options system, which uses `QVariantMap` to pass parameters to the icon rendering engine.

## Color Customization

### Basic Color Changes

The most common customization is changing icon colors:

```cpp
// Single color
QVariantMap options;
options["color"] = QColor(Qt::red);
QIcon redIcon = lucide.icon("heart", options);

// Using RGB values
options["color"] = QColor(59, 130, 246);  // Blue
QIcon blueIcon = lucide.icon("star", options);

// Using hex colors
options["color"] = QColor("#10B981");  // Green
QIcon greenIcon = lucide.icon("check", options);
```

### State-Specific Colors

Customize colors for different icon states:

```cpp
QVariantMap options;
options["color"] = QColor(Qt::blue);           // Normal state
options["color-disabled"] = QColor(Qt::gray);  // Disabled state
options["color-active"] = QColor(Qt::cyan);    // Hover state
options["color-selected"] = QColor(Qt::darkBlue); // Selected state

QIcon stateAwareIcon = lucide.icon("settings", options);
```

### Color Schemes

Create consistent color schemes across your application:

```cpp
class AppColors
{
public:
    static QColor primary() { return QColor(59, 130, 246); }
    static QColor secondary() { return QColor(107, 114, 128); }
    static QColor success() { return QColor(16, 185, 129); }
    static QColor warning() { return QColor(245, 158, 11); }
    static QColor danger() { return QColor(239, 68, 68); }
};

// Use consistent colors
QVariantMap primaryOptions;
primaryOptions["color"] = AppColors::primary();

QVariantMap successOptions;
successOptions["color"] = AppColors::success();

QIcon saveIcon = lucide.icon("save", primaryOptions);
QIcon checkIcon = lucide.icon("check", successOptions);
```

## Size and Scaling

### Scale Factor

Adjust icon size relative to the container:

```cpp
QVariantMap options;
options["scale-factor"] = 0.8;  // 80% of container size
QIcon smallIcon = lucide.icon("home", options);

options["scale-factor"] = 1.2;  // 120% of container size
QIcon largeIcon = lucide.icon("star", options);
```

### Responsive Scaling

Create icons that adapt to different contexts:

```cpp
QVariantMap toolbarOptions;
toolbarOptions["scale-factor"] = 0.9;  // Slightly smaller for toolbars

QVariantMap buttonOptions;
buttonOptions["scale-factor"] = 1.0;   // Full size for buttons

QVariantMap statusOptions;
statusOptions["scale-factor"] = 0.7;   // Smaller for status indicators
```

## Opacity and Transparency

### Basic Opacity

Control icon transparency:

```cpp
QVariantMap options;
options["opacity"] = 0.5;  // 50% transparent
QIcon transparentIcon = lucide.icon("image", options);

options["opacity"] = 0.8;  // 80% opaque
QIcon subtleIcon = lucide.icon("info", options);
```

### Context-Aware Opacity

Use opacity to indicate state or importance:

```cpp
// Disabled appearance
QVariantMap disabledOptions;
disabledOptions["opacity"] = 0.4;
disabledOptions["color"] = QColor(Qt::gray);

// Secondary actions
QVariantMap secondaryOptions;
secondaryOptions["opacity"] = 0.7;
secondaryOptions["color"] = QColor(Qt::darkGray);

// Highlighted actions
QVariantMap highlightOptions;
highlightOptions["opacity"] = 1.0;
highlightOptions["color"] = QColor(Qt::blue);
```

## Default Options

### Setting Global Defaults

Avoid repetitive option setting by using defaults:

```cpp
// Set application-wide defaults
lucide.setDefaultOption("color", QColor(64, 64, 64));
lucide.setDefaultOption("scale-factor", 0.9);
lucide.setDefaultOption("opacity", 1.0);

// All subsequent icons use these defaults
QIcon icon1 = lucide.icon("home");     // Uses gray color, 0.9 scale
QIcon icon2 = lucide.icon("settings"); // Uses gray color, 0.9 scale

// Override defaults when needed
QVariantMap redOptions;
redOptions["color"] = QColor(Qt::red);
QIcon redIcon = lucide.icon("heart", redOptions);  // Red, but still 0.9 scale
```

### Theme-Based Defaults

Implement theme switching:

```cpp
class ThemeManager
{
public:
    static void applyLightTheme(lucide::QtLucide& lucide) {
        lucide.setDefaultOption("color", QColor(64, 64, 64));
        lucide.setDefaultOption("color-disabled", QColor(156, 163, 175));
    }
    
    static void applyDarkTheme(lucide::QtLucide& lucide) {
        lucide.setDefaultOption("color", QColor(229, 231, 235));
        lucide.setDefaultOption("color-disabled", QColor(107, 114, 128));
    }
};

// Apply theme
ThemeManager::applyDarkTheme(lucide);
```

## Advanced Customization Techniques

### Conditional Styling

Apply different styles based on context:

```cpp
QVariantMap getIconOptions(const QString& context, bool isEnabled = true)
{
    QVariantMap options;
    
    if (context == "toolbar") {
        options["scale-factor"] = 0.85;
        options["color"] = isEnabled ? QColor(Qt::black) : QColor(Qt::gray);
    } else if (context == "menu") {
        options["scale-factor"] = 0.9;
        options["color"] = isEnabled ? QColor(Qt::darkBlue) : QColor(Qt::lightGray);
    } else if (context == "status") {
        options["scale-factor"] = 0.7;
        options["opacity"] = 0.8;
    }
    
    return options;
}

// Usage
QIcon toolbarIcon = lucide.icon("save", getIconOptions("toolbar"));
QIcon menuIcon = lucide.icon("save", getIconOptions("menu"));
QIcon statusIcon = lucide.icon("save", getIconOptions("status"));
```

### Animation Preparation

Prepare icons for smooth animations:

```cpp
// Create icons at different opacities for fade animations
QList<QIcon> fadeIcons;
for (double opacity = 0.0; opacity <= 1.0; opacity += 0.1) {
    QVariantMap options;
    options["opacity"] = opacity;
    fadeIcons.append(lucide.icon("heart", options));
}

// Create icons at different scales for zoom animations
QList<QIcon> scaleIcons;
for (double scale = 0.5; scale <= 1.5; scale += 0.1) {
    QVariantMap options;
    options["scale-factor"] = scale;
    scaleIcons.append(lucide.icon("star", options));
}
```

## Best Practices

### Performance Optimization

```cpp
// Cache customized icons
class CustomIconCache
{
private:
    static QHash<QString, QIcon> cache;
    
public:
    static QIcon getIcon(const QString& name, const QVariantMap& options) {
        QString key = generateKey(name, options);
        if (!cache.contains(key)) {
            cache[key] = lucide.icon(name, options);
        }
        return cache[key];
    }
    
private:
    static QString generateKey(const QString& name, const QVariantMap& options) {
        QStringList parts;
        parts << name;
        for (auto it = options.begin(); it != options.end(); ++it) {
            parts << QString("%1=%2").arg(it.key()).arg(it.value().toString());
        }
        return parts.join("|");
    }
};
```

### Consistency Guidelines

```cpp
// Define standard icon styles
namespace IconStyles {
    QVariantMap primary() {
        QVariantMap options;
        options["color"] = QColor(59, 130, 246);
        options["scale-factor"] = 1.0;
        return options;
    }
    
    QVariantMap secondary() {
        QVariantMap options;
        options["color"] = QColor(107, 114, 128);
        options["scale-factor"] = 0.9;
        return options;
    }
    
    QVariantMap danger() {
        QVariantMap options;
        options["color"] = QColor(239, 68, 68);
        options["scale-factor"] = 1.0;
        return options;
    }
}

// Use consistently throughout the application
QIcon saveIcon = lucide.icon("save", IconStyles::primary());
QIcon deleteIcon = lucide.icon("trash", IconStyles::danger());
```

### Accessibility Considerations

```cpp
// Ensure sufficient contrast
QVariantMap getAccessibleOptions(const QColor& backgroundColor) {
    QVariantMap options;
    
    // Calculate contrast ratio and adjust color
    double luminance = calculateLuminance(backgroundColor);
    if (luminance > 0.5) {
        // Light background - use dark icon
        options["color"] = QColor(Qt::black);
    } else {
        // Dark background - use light icon
        options["color"] = QColor(Qt::white);
    }
    
    return options;
}

// High contrast mode support
QVariantMap getHighContrastOptions() {
    QVariantMap options;
    options["color"] = QColor(Qt::black);
    options["scale-factor"] = 1.1;  // Slightly larger for better visibility
    return options;
}
```

## See Also

- [Basic Usage](basic-usage.md) - Learn the fundamentals
- [Advanced Features](advanced-features.md) - Custom painters and advanced techniques
- [Performance Guide](performance.md) - Optimization strategies
- [API Reference](../api/qtlucide.md) - Complete API documentation
