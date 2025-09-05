# Migration Guide

Complete guide for migrating to QtLucide from other icon libraries and upgrading between QtLucide versions.

## Overview

This section helps you migrate your existing Qt applications to use QtLucide, whether you're coming from other icon libraries or upgrading from an older version of QtLucide.

<div class="grid cards" markdown>

- :material-swap-horizontal: **From QtAwesome**

  ***

  Step-by-step guide for migrating from QtAwesome to QtLucide with minimal code changes.

  [:octicons-arrow-right-24: QtAwesome Migration](from-qtawesome.md)

- :material-update: **Version Upgrades**

  ***

  Upgrade guides for moving between QtLucide versions, including breaking changes.

  [:octicons-arrow-right-24: Version Upgrades](version-upgrades.md)

</div>

## Why Migrate to QtLucide?

### Advantages Over Other Icon Libraries

=== "vs QtAwesome"

    **Modern Icon Set**
    - 1,634+ contemporary Lucide icons vs older FontAwesome icons
    - Consistent design language optimized for modern UIs
    - Regular updates with new icons

    **Better Performance**
    - SVG-based rendering vs font-based rendering
    - Automatic caching and optimization
    - No font loading overhead

    **Enhanced Customization**
    - Runtime color changes without font regeneration
    - Flexible scaling and effects
    - Custom painter support

=== "vs Custom SVG Solutions"

    **Simplified Management**
    - No need to manage individual SVG files
    - Automatic resource bundling
    - Consistent naming and organization

    **Optimized Performance**
    - Built-in caching system
    - Efficient memory usage
    - Qt-native integration

    **Rich Feature Set**
    - State-aware rendering (disabled, active, selected)
    - Theme integration
    - Extensible painter system

=== "vs Icon Fonts"

    **Scalability**
    - True vector rendering at any size
    - No font hinting issues
    - Perfect quality on high-DPI displays

    **Flexibility**
    - Easy color customization
    - No font installation required
    - Works with any Qt deployment

    **Maintainability**
    - Type-safe icon references
    - IDE auto-completion
    - Compile-time error checking

## Migration Strategies

### 1. Gradual Migration

Replace icons incrementally while keeping your existing system:

```cpp
// Keep existing icon system
QtAwesome* awesome = new QtAwesome(this);
awesome->initFontAwesome();

// Add QtLucide alongside
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();

// Migrate icons one by one
// Old: button->setIcon(awesome->icon(fa::fa_heart));
// New: button->setIcon(lucide->icon("heart"));
```

### 2. Wrapper Approach

Create a wrapper class to ease transition:

```cpp
class IconManager
{
private:
    QtAwesome* m_awesome;
    lucide::QtLucide* m_lucide;
    bool m_useLucide;

public:
    IconManager() : m_useLucide(true) {
        m_awesome = new QtAwesome();
        m_awesome->initFontAwesome();

        m_lucide = new lucide::QtLucide();
        m_lucide->initLucide();
    }

    QIcon icon(const QString& name) {
        if (m_useLucide) {
            return m_lucide->icon(name);
        } else {
            // Fallback to QtAwesome
            return m_awesome->icon(/* convert name */);
        }
    }
};
```

### 3. Complete Replacement

Replace the entire icon system at once:

```cpp
// Remove QtAwesome includes and initialization
// #include <QtAwesome.h>
// QtAwesome* awesome = new QtAwesome(this);

// Add QtLucide
#include <QtLucide/QtLucide.h>
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();

// Update all icon creation calls
// Old: awesome->icon(fa::fa_heart)
// New: lucide->icon("heart")
```

## Common Migration Patterns

### Menu and Toolbar Icons

=== "Before (QtAwesome)"

    ```cpp
    QMenu* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(awesome->icon(fa::fa_file), "New");
    fileMenu->addAction(awesome->icon(fa::fa_folder_open), "Open");
    fileMenu->addAction(awesome->icon(fa::fa_save), "Save");

    QToolBar* toolbar = addToolBar("Main");
    toolbar->addAction(awesome->icon(fa::fa_cut), "Cut");
    toolbar->addAction(awesome->icon(fa::fa_copy), "Copy");
    toolbar->addAction(awesome->icon(fa::fa_paste), "Paste");
    ```

=== "After (QtLucide)"

    ```cpp
    QMenu* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(lucide->icon("file-plus"), "New");
    fileMenu->addAction(lucide->icon("folder-open"), "Open");
    fileMenu->addAction(lucide->icon("save"), "Save");

    QToolBar* toolbar = addToolBar("Main");
    toolbar->addAction(lucide->icon("scissors"), "Cut");
    toolbar->addAction(lucide->icon("copy"), "Copy");
    toolbar->addAction(lucide->icon("clipboard"), "Paste");
    ```

### Custom Colors

=== "Before (QtAwesome)"

    ```cpp
    QVariantMap options;
    options.insert("color", QColor(Qt::red));
    options.insert("color-disabled", QColor(Qt::gray));
    QIcon redIcon = awesome->icon(fa::fa_heart, options);
    ```

=== "After (QtLucide)"

    ```cpp
    QVariantMap options;
    options["color"] = QColor(Qt::red);
    options["color-disabled"] = QColor(Qt::gray);
    QIcon redIcon = lucide->icon("heart", options);
    ```

### Button Icons

=== "Before (QtAwesome)"

    ```cpp
    QPushButton* playButton = new QPushButton("Play");
    playButton->setIcon(awesome->icon(fa::fa_play));

    QPushButton* stopButton = new QPushButton("Stop");
    stopButton->setIcon(awesome->icon(fa::fa_stop));
    ```

=== "After (QtLucide)"

    ```cpp
    QPushButton* playButton = new QPushButton("Play");
    playButton->setIcon(lucide->icon("play"));

    QPushButton* stopButton = new QPushButton("Stop");
    stopButton->setIcon(lucide->icon("square"));  // Lucide uses "square" for stop
    ```

## Icon Name Mapping

Common icon name translations:

| QtAwesome            | QtLucide                  | Notes                           |
| -------------------- | ------------------------- | ------------------------------- |
| `fa_home`            | `"home"`                  | Direct equivalent               |
| `fa_heart`           | `"heart"`                 | Direct equivalent               |
| `fa_user`            | `"user"`                  | Direct equivalent               |
| `fa_cog` / `fa_gear` | `"settings"`              | Lucide uses "settings"          |
| `fa_trash`           | `"trash-2"`               | Lucide has multiple trash icons |
| `fa_edit`            | `"edit"` or `"pencil"`    | Multiple edit icons available   |
| `fa_save`            | `"save"`                  | Direct equivalent               |
| `fa_folder_open`     | `"folder-open"`           | Direct equivalent               |
| `fa_file`            | `"file"` or `"file-plus"` | Multiple file icons             |
| `fa_stop`            | `"square"`                | Lucide uses square for stop     |

## Build System Changes

### CMakeLists.txt Updates

=== "Before (QtAwesome)"

    ```cmake
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    # QtAwesome typically included as source or submodule

    target_link_libraries(your_target PRIVATE
        Qt6::Core
        Qt6::Widgets
        QtAwesome  # or however QtAwesome was integrated
    )
    ```

=== "After (QtLucide)"

    ```cmake
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)
    find_package(QtLucide REQUIRED)

    target_link_libraries(your_target PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Svg
        QtLucide::QtLucide
    )
    ```

### qmake Project Files

=== "Before (QtAwesome)"

    ```pro
    QT += core widgets
    # QtAwesome integration varies
    ```

=== "After (QtLucide)"

    ```pro
    QT += core widgets svg
    include(path/to/QtLucide/QtLucide.pri)
    ```

## Testing Your Migration

### Verification Checklist

- [ ] All icons display correctly
- [ ] Custom colors work as expected
- [ ] Performance is acceptable
- [ ] High-DPI displays render properly
- [ ] Application startup time is reasonable
- [ ] Memory usage is within expected bounds

### Test Code

Create a simple test to verify migration:

```cpp
void testIconMigration()
{
    lucide::QtLucide lucide;
    QVERIFY(lucide.initLucide());

    // Test basic icon creation
    QIcon homeIcon = lucide.icon("home");
    QVERIFY(!homeIcon.isNull());

    // Test custom colors
    QVariantMap options;
    options["color"] = QColor(Qt::red);
    QIcon redIcon = lucide.icon("heart", options);
    QVERIFY(!redIcon.isNull());

    // Test icon count
    QVERIFY(lucide.availableIcons().size() > 1000);

    qDebug() << "Migration test passed!";
}
```

## Common Issues and Solutions

### Missing Icons

**Problem**: Some icons from your old library don't exist in Lucide.

**Solution**: Use the [icon gallery](../examples/gallery.md) to find suitable alternatives, or create custom painters for specific icons.

### Performance Differences

**Problem**: Different performance characteristics compared to font-based icons.

**Solution**: Review the [performance guide](../user-guide/performance.md) and implement caching strategies.

### Color Handling

**Problem**: Colors don't apply the same way as with font icons.

**Solution**: SVG color replacement works differently. Check the [customization guide](../user-guide/customization.md).

## Getting Help

If you encounter issues during migration:

1. Check the [FAQ](../faq/common-issues.md) for common problems
2. Search [GitHub Issues](https://github.com/AstroAir/QtLucide/issues) for similar problems
3. Ask questions in [GitHub Discussions](https://github.com/AstroAir/QtLucide/discussions)
4. Create a new issue with migration details

## Migration Tools

We're working on tools to help automate migration:

- **Icon Name Converter** - Maps old icon names to QtLucide equivalents
- **Code Scanner** - Finds icon usage in your codebase
- **Migration Assistant** - GUI tool for interactive migration

Check the [tools section](../development/tools.md) for the latest migration utilities.

## Success Stories

Learn from others who have successfully migrated:

- **Case Study 1**: Large desktop application migration
- **Case Study 2**: Cross-platform mobile app update
- **Case Study 3**: Plugin system modernization

_Contact us to share your migration story!_
