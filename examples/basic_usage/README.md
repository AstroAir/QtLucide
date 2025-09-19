# QtLucide Basic Usage Example

This example demonstrates the basic usage of QtLucide icons in a Qt application, showcasing API compatibility with QtAwesome and various customization options.

## Features

### 🎨 **Icon Customization**

- **Color Control**: Change icon colors using a color picker
- **Size Control**: Adjust icon size from 16px to 128px
- **Scale Factor**: Fine-tune icon scaling from 50% to 150%

### 🔍 **Icon Browsing**

- **Sample Icons**: View a curated set of commonly used icons
- **All Icons**: Toggle to view all 1600+ available Lucide icons
- **Search Filter**: Type to filter icons by name
- **Interactive Grid**: Click any icon to see its details

### 🛠️ **Developer Features**

- **Icon Validation**: Automatic validation of icon names at startup
- **Error Reporting**: Clear feedback about loading success/failures
- **Debug Information**: Helpful suggestions for invalid icon names

## Fixed Issues

This version fixes the following icon loading problems from the original example:

| ❌ **Old Name** | ✅ **Correct Name** | 📝 **Note** |
|----------------|-------------------|-------------|
| `alert-circle` | `circle-alert` | Icon name order corrected |
| `bar-chart` | `chart-bar` | Follows Lucide naming convention |
| `pie-chart` | `chart-pie` | Follows Lucide naming convention |
| `download-cloud` | `cloud-download` | Removed duplicate entry |

## Usage

### Basic Icon Loading

```cpp
#include <QtLucide/QtLucide.h>

// Initialize QtLucide
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();

// Create icons
QPushButton* button = new QPushButton();
button->setIcon(lucide->icon("heart"));
```

### With Custom Options

```cpp
QVariantMap options;
options["color"] = QColor(Qt::red);
options["scale-factor"] = 0.8;
button->setIcon(lucide->icon("heart", options));
```

## Building

### Using Meson (Recommended)

```bash
cd QtLucide
meson setup builddir
meson compile -C builddir examples/basic_usage/QtLucideExample
./builddir/examples/basic_usage/QtLucideExample
```

### Using CMake

```bash
cd QtLucide
mkdir build && cd build
cmake -DQTLUCIDE_BUILD_EXAMPLES=ON ..
make QtLucideExample
./examples/basic_usage/QtLucideExample
```

## Tips

1. **Icon Names**: Always use exact Lucide icon names in kebab-case format
2. **Validation**: Check the debug output for icon name validation results
3. **Search**: Use the search box to quickly find specific icons
4. **All Icons**: Enable "Show all icons" to browse the complete icon set
5. **Performance**: The example efficiently caches rendered icons for better performance

## Troubleshooting

- **Icons not loading**: Check that icon names match exactly with Lucide names
- **Build errors**: Ensure Qt6 and QtLucide dependencies are properly installed
- **Performance issues**: Limit the number of icons displayed when using "Show all icons"
