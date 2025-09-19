# Enums & Constants

Complete reference for all QtLucide enumerations, constants, and utility definitions.

## Icons Enumeration

### enum class Icons

Strongly-typed enumeration of all 1634 Lucide icons.

```cpp
namespace lucide {
    enum class Icons {
        a_arrow_down = 0,
        a_arrow_up = 1,
        a_large_small = 2,
        accessibility = 3,
        activity = 4,
        // ... 1629 more icons
        zoom_in = 1632,
        zoom_out = 1633
    };
}
```

### Icon Count Constant

```cpp
namespace lucide {
    constexpr int ICON_COUNT = 1634;
}
```

### Naming Convention

Icon enum names follow these rules:

- **Hyphens to Underscores**: `arrow-left` becomes `arrow_left`
- **Numbers**: `3d-rotate` becomes `rotate_3d` (numbers moved to end)
- **Special Characters**: Removed or converted to underscores

#### Common Name Mappings

| Icon Name (String) | Enum Value | Description |
|-------------------|------------|-------------|
| `"home"` | `Icons::home` | Home/house icon |
| `"arrow-left"` | `Icons::arrow_left` | Left arrow |
| `"circle-alert"` | `Icons::circle_alert` | Alert in circle |
| `"file-plus"` | `Icons::file_plus` | Add file icon |
| `"user-check"` | `Icons::user_check` | User with checkmark |

## Icon Categories

Icons are organized into logical categories:

### Navigation Icons

```cpp
Icons::home, Icons::arrow_left, Icons::arrow_right, Icons::arrow_up, Icons::arrow_down,
Icons::chevron_left, Icons::chevron_right, Icons::menu, Icons::more_horizontal
```

### File & Folder Icons

```cpp
Icons::file, Icons::file_plus, Icons::file_minus, Icons::folder, Icons::folder_open,
Icons::save, Icons::download, Icons::upload, Icons::archive
```

### UI & Interface Icons

```cpp
Icons::settings, Icons::cog, Icons::search, Icons::filter, Icons::sort_asc,
Icons::sort_desc, Icons::grid_3x3, Icons::list, Icons::layout_dashboard
```

### Communication Icons

```cpp
Icons::mail, Icons::message_circle, Icons::phone, Icons::video, Icons::mic,
Icons::mic_off, Icons::bell, Icons::notification
```

### Media & Entertainment Icons

```cpp
Icons::play, Icons::pause, Icons::stop, Icons::skip_back, Icons::skip_forward,
Icons::volume_2, Icons::volume_off, Icons::music, Icons::headphones
```

### Status & Feedback Icons

```cpp
Icons::check, Icons::x, Icons::alert_triangle, Icons::info, Icons::help_circle,
Icons::thumbs_up, Icons::thumbs_down, Icons::star, Icons::heart
```

## String Mappings

### Icon Name to Enum Conversion

```cpp
// Convert string name to enum
lucide::Icons iconId = lucide.stringToIconId("heart");
if (iconId != static_cast<lucide::Icons>(-1)) {
    // Valid icon found
    QIcon icon = lucide.icon(iconId);
}
```

### Enum to String Conversion

```cpp
// Convert enum to string name
QString iconName = lucide.iconIdToString(lucide::Icons::heart);
// Returns: "heart"
```

### Available Icons List

```cpp
// Get all available icon names
QStringList iconNames = lucide.availableIcons();
qDebug() << "Total icons:" << iconNames.size();  // 1634

// Check if icon exists
if (iconNames.contains("heart")) {
    QIcon heartIcon = lucide.icon("heart");
}
```

## Icon States and Modes

### QIcon::Mode Enumeration

Standard Qt icon modes supported by QtLucide:

```cpp
QIcon::Normal    // Default appearance
QIcon::Disabled  // Grayed out appearance
QIcon::Active    // Mouse hover appearance
QIcon::Selected  // Selection appearance
```

### QIcon::State Enumeration

Standard Qt icon states:

```cpp
QIcon::Off       // Default state
QIcon::On        // Toggle/checked state
```

### Usage with QtLucide

```cpp
// Icons automatically adapt to different modes/states
QIcon icon = lucide.icon("heart");

// Qt automatically uses appropriate mode/state
button->setIcon(icon);           // Uses Normal/Off
button->setEnabled(false);       // Qt uses Disabled mode
// Hover over button             // Qt uses Active mode
```

## Option Constants

### Supported Option Keys

```cpp
// Color options
static const QString COLOR = "color";
static const QString COLOR_DISABLED = "color-disabled";
static const QString COLOR_ACTIVE = "color-active";
static const QString COLOR_SELECTED = "color-selected";

// Scaling options
static const QString SCALE_FACTOR = "scale-factor";
static const QString OPACITY = "opacity";
```

### Default Values

```cpp
// Default option values
QColor DEFAULT_COLOR = Qt::black;
QColor DEFAULT_DISABLED_COLOR = Qt::gray;
double DEFAULT_SCALE_FACTOR = 0.9;
double DEFAULT_OPACITY = 1.0;
```

### Option Value Ranges

| Option | Type | Range | Default |
|--------|------|-------|---------|
| `color` | `QColor` | Any valid QColor | `Qt::black` |
| `color-disabled` | `QColor` | Any valid QColor | Lighter primary |
| `color-active` | `QColor` | Any valid QColor | Primary color |
| `color-selected` | `QColor` | Any valid QColor | Primary color |
| `scale-factor` | `double` | 0.1 to 2.0 | 0.9 |
| `opacity` | `double` | 0.0 to 1.0 | 1.0 |

## Version Information

### Library Version

```cpp
namespace lucide {
    constexpr const char* VERSION = "1.0.0";
    constexpr int VERSION_MAJOR = 1;
    constexpr int VERSION_MINOR = 0;
    constexpr int VERSION_PATCH = 0;
}
```

### Lucide Icon Set Version

```cpp
namespace lucide {
    constexpr const char* LUCIDE_VERSION = "0.460.0";
}
```

## Utility Functions

### Icon Validation

```cpp
// Check if icon name is valid
bool isValidIcon(const QString& name) {
    return lucide.availableIcons().contains(name);
}

// Check if enum value is valid
bool isValidIcon(Icons iconId) {
    return static_cast<int>(iconId) >= 0 && 
           static_cast<int>(iconId) < ICON_COUNT;
}
```

### Icon Search

```cpp
// Find icons by pattern
QStringList findIcons(const QString& pattern) {
    QStringList allIcons = lucide.availableIcons();
    return allIcons.filter(QRegularExpression(pattern));
}

// Example: Find all arrow icons
QStringList arrowIcons = findIcons(".*arrow.*");
```

### Category Helpers

```cpp
// Get icons by category (example implementation)
QStringList getNavigationIcons() {
    return QStringList() << "home" << "arrow-left" << "arrow-right" 
                         << "arrow-up" << "arrow-down" << "chevron-left"
                         << "chevron-right" << "menu";
}

QStringList getFileIcons() {
    return QStringList() << "file" << "file-plus" << "folder" 
                         << "folder-open" << "save" << "download";
}
```

## Performance Constants

### Cache Limits

```cpp
namespace lucide {
    constexpr int DEFAULT_CACHE_SIZE = 1000;      // Maximum cached pixmaps
    constexpr int MAX_CACHE_SIZE = 10000;         // Absolute maximum
    constexpr int CACHE_CLEANUP_THRESHOLD = 800;  // When to start cleanup
}
```

### Rendering Thresholds

```cpp
namespace lucide {
    constexpr int MIN_ICON_SIZE = 8;      // Minimum renderable size
    constexpr int MAX_ICON_SIZE = 1024;   // Maximum renderable size
    constexpr double MIN_SCALE_FACTOR = 0.1;  // Minimum scale factor
    constexpr double MAX_SCALE_FACTOR = 2.0;  // Maximum scale factor
}
```

## Error Codes

### Return Values

```cpp
// Icon ID for invalid/not found icons
constexpr Icons INVALID_ICON = static_cast<Icons>(-1);

// Return codes for initialization
enum class InitResult {
    Success = 0,
    ResourceLoadFailed = 1,
    QtNotAvailable = 2,
    UnknownError = 99
};
```

## See Also

- [QtLucide Class](qtlucide.md) - Main icon management class
- [User Guide](../user-guide/basic-usage.md) - Usage examples
- [Icon Gallery](../examples/gallery-application.md) - Browse all icons
- [Lucide Icons Website](https://lucide.dev/) - Official icon documentation
