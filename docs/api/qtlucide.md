# QtLucide Class

The main class for managing Lucide icons in Qt applications.

## Class Declaration

```cpp
namespace lucide {
    class QtLucide : public QObject
    {
        Q_OBJECT
    public:
        explicit QtLucide(QObject* parent = nullptr);
        ~QtLucide();

        // Initialization
        bool initLucide();

        // Icon creation
        QIcon icon(const QString& name, const QVariantMap& options = {});
        QIcon icon(Icons iconId, const QVariantMap& options = {});
        QIcon icon(QtLucideIconPainter* painter, const QVariantMap& options = {});

        // Configuration
        void setDefaultOption(const QString& name, const QVariant& value);
        QVariant defaultOption(const QString& name) const;

        // Custom painters
        void give(const QString& name, QtLucideIconPainter* painter);

        // Information
        QStringList availableIcons() const;
        QByteArray svgData(Icons iconId) const;
        QByteArray svgData(const QString& name) const;

    signals:
        void defaultOptionsReset();

    public slots:
        void resetDefaultOptions();
    };
}
```

## Constructor & Destructor

### QtLucide(QObject\* parent = nullptr)

Creates a new QtLucide instance.

**Parameters:**

- `parent` - Parent QObject for memory management

**Example:**

```cpp
// Stack allocation
lucide::QtLucide lucide;

// Heap allocation with parent
lucide::QtLucide* lucide = new lucide::QtLucide(this);

// Heap allocation without parent (manual cleanup required)
lucide::QtLucide* lucide = new lucide::QtLucide();
```

### ~QtLucide()

Destructor that cleans up resources and custom painters.

**Note:** Custom painters registered with `give()` are automatically deleted.

## Initialization

### bool initLucide()

Initializes the QtLucide icon system. Must be called before creating icons.

**Returns:** `true` if initialization succeeded, `false` otherwise

**Example:**

```cpp
lucide::QtLucide lucide;
if (!lucide.initLucide()) {
    qWarning() << "Failed to initialize QtLucide";
    return false;
}
qDebug() << "QtLucide ready with" << lucide::ICON_COUNT << "icons";
```

**Thread Safety:** Must be called from the main thread.

## Icon Creation

### QIcon icon(const QString& name, const QVariantMap& options = {})

Creates an icon by name with optional customization.

**Parameters:**

- `name` - Icon name (e.g., "heart", "home", "settings")
- `options` - Customization options (color, scale-factor, etc.)

**Returns:** QIcon instance, or null QIcon if name is invalid

**Example:**

```cpp
// Basic usage
QIcon homeIcon = lucide.icon("home");

// With custom color
QVariantMap options;
options["color"] = QColor(Qt::red);
QIcon redHeart = lucide.icon("heart", options);

// Multiple options
QVariantMap options;
options["color"] = QColor(59, 130, 246);  // Blue
options["scale-factor"] = 1.2;
options["opacity"] = 0.8;
QIcon customIcon = lucide.icon("star", options);
```

### QIcon icon(Icons iconId, const QVariantMap& options = {})

Creates an icon using the strongly-typed enum.

**Parameters:**

- `iconId` - Icon enum value (e.g., `Icons::heart`)
- `options` - Customization options

**Returns:** QIcon instance

**Example:**

```cpp
// Using enum (preferred for performance)
QIcon homeIcon = lucide.icon(lucide::Icons::home);
QIcon heartIcon = lucide.icon(lucide::Icons::heart);

// With options
QVariantMap options;
options["color"] = QColor(Qt::green);
QIcon greenCheck = lucide.icon(lucide::Icons::check_circle, options);
```

### QIcon icon(QtLucideIconPainter\* painter, const QVariantMap& options = {})

Creates an icon using a custom painter.

**Parameters:**

- `painter` - Custom painter instance (QtLucide takes ownership)
- `options` - Options passed to the painter

**Returns:** QIcon instance

**Example:**

```cpp
class MyCustomPainter : public QtLucideIconPainter {
public:
    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state,
               const QVariantMap& options) override {
        // Custom rendering logic
        QColor color = options.value("color", Qt::black).value<QColor>();
        painter->fillRect(rect, color);
    }
};

// Use custom painter
QIcon customIcon = lucide.icon(new MyCustomPainter(), options);
```

## Configuration

### void setDefaultOption(const QString& name, const QVariant& value)

Sets a default option that applies to all subsequently created icons.

**Parameters:**

- `name` - Option name (e.g., "color", "scale-factor")
- `value` - Option value

**Example:**

```cpp
// Set default color for all icons
lucide.setDefaultOption("color", QColor(64, 64, 64));
lucide.setDefaultOption("scale-factor", 1.1);

// All icons created after this will use these defaults
QIcon icon1 = lucide.icon("home");     // Uses gray color and 1.1 scale
QIcon icon2 = lucide.icon("settings"); // Uses gray color and 1.1 scale

// Override defaults for specific icons
QVariantMap redOptions;
redOptions["color"] = QColor(Qt::red);
QIcon redIcon = lucide.icon("heart", redOptions);  // Red, but still 1.1 scale
```

### QVariant defaultOption(const QString& name) const

Gets the current default value for an option.

**Parameters:**

- `name` - Option name

**Returns:** Current default value, or invalid QVariant if not set

**Example:**

```cpp
lucide.setDefaultOption("color", QColor(Qt::blue));

QColor defaultColor = lucide.defaultOption("color").value<QColor>();
if (defaultColor.isValid()) {
    qDebug() << "Default color:" << defaultColor.name();
}
```

## Custom Painters

### void give(const QString& name, QtLucideIconPainter\* painter)

Registers a custom painter for a specific icon name.

**Parameters:**

- `name` - Custom icon name
- `painter` - Custom painter (QtLucide takes ownership)

**Example:**

```cpp
class LogoPainter : public QtLucideIconPainter {
public:
    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state,
               const QVariantMap& options) override {
        // Draw company logo
        painter->drawPixmap(rect, companyLogo);
    }
};

// Register custom painter
lucide.give("company-logo", new LogoPainter());

// Use custom icon
QIcon logo = lucide.icon("company-logo");
```

**Note:** If a painter with the same name already exists, it will be deleted and replaced.

## Information Methods

### QStringList availableIcons() const

Returns a list of all available icon names.

**Returns:** List of icon names that can be used with `icon(QString)`

**Example:**

```cpp
QStringList icons = lucide.availableIcons();
qDebug() << "Available icons:" << icons.size();

// Check if specific icon exists
if (icons.contains("heart")) {
    QIcon heartIcon = lucide.icon("heart");
}

// Populate a combo box
QComboBox* iconCombo = new QComboBox();
iconCombo->addItems(icons);
```

### QByteArray svgData(Icons iconId) const

Gets the raw SVG data for an icon by enum.

**Parameters:**

- `iconId` - Icon enum value

**Returns:** SVG data as QByteArray, or empty array if invalid

**Example:**

```cpp
QByteArray svgData = lucide.svgData(lucide::Icons::heart);
if (!svgData.isEmpty()) {
    // Save to file
    QFile file("heart.svg");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(svgData);
    }
}
```

### QByteArray svgData(const QString& name) const

Gets the raw SVG data for an icon by name.

**Parameters:**

- `name` - Icon name

**Returns:** SVG data as QByteArray, or empty array if invalid

## Signals

### void defaultOptionsReset()

Emitted when `resetDefaultOptions()` is called. Allows connected objects to respond to option changes.

## Slots

### void resetDefaultOptions()

Resets all default options to system defaults based on the application palette.

**Default Values After Reset:**

- `color` - `QPalette::Text` (Normal)
- `color-disabled` - `QPalette::Text` (Disabled)
- `color-active` - `QPalette::Text` (Active)
- `color-selected` - `QPalette::HighlightedText` (Active)
- `scale-factor` - `0.9`
- `opacity` - `1.0`

**Example:**

```cpp
// Connect to signal
connect(&lucide, &lucide::QtLucide::defaultOptionsReset, this, [this]() {
    qDebug() << "Options were reset";
    refreshIcons();
});

// Reset options
lucide.resetDefaultOptions();
```

## Supported Options

| Option           | Type     | Description                  | Default                   | Range            |
| ---------------- | -------- | ---------------------------- | ------------------------- | ---------------- |
| `color`          | `QColor` | Primary icon color           | `QPalette::Text`          | Any valid QColor |
| `color-disabled` | `QColor` | Color for disabled state     | `QPalette::Text` (Disabled) or auto-lightened | Any valid QColor |
| `color-active`   | `QColor` | Color for active/hover state | `QPalette::Text` (Active) | Any valid QColor |
| `color-selected` | `QColor` | Color for selected state     | `QPalette::HighlightedText` | Any valid QColor |
| `scale-factor`   | `double` | Size multiplier              | `0.9`                     | `0.1` to `10.0`  |
| `opacity`        | `double` | Icon opacity                 | `1.0`                     | `0.0` to `1.0`   |
| `stroke-width`   | `double` | SVG stroke width             | `2.0`                     | `0.5` to `4.0`   |

!!! note "Palette-Based Defaults"
    Default colors are derived from `QApplication::palette()` at initialization time. This ensures icons automatically match your application's theme.

## Performance Notes

- **Icon caching**: Icons are automatically cached based on name, size, mode, state, and options
- **Enum vs String**: Using enum values is slightly faster than string names
- **Default options**: Setting default options reduces memory usage for similar icons
- **Custom painters**: Custom painters are cached like built-in icons
- **Cache key**: Includes `iconId`, size, mode, state, color, scale-factor, opacity, and stroke-width

## Thread Safety

- **Resource initialization**: Thread-safe (protected by mutex)
- **Icon creation**: Thread-safe after `initLucide()` is called
- **Configuration**: `setDefaultOption()` and `give()` are not thread-safe
- **Information methods**: All const methods are thread-safe
- **SVG data access**: Thread-safe with warning deduplication

## See Also

- [QtLucideIconEngine](icon-engine.md) - Icon rendering engine
- [QtLucideIconPainter](icon-painter.md) - Custom painter base class
- [Icons Enum](enums.md) - Complete icon enumeration
- [User Guide](../user-guide/basic-usage.md) - Usage examples
