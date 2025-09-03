# QtLucide - Lucide Icons for Qt Applications

QtLucide is a library that provides easy access to [Lucide icons](https://lucide.dev/) in Qt applications. It offers a QtAwesome-compatible API while using SVG-based rendering for crisp, scalable icons.

## Features

- **1634 Beautiful Icons**: Complete Lucide icon set with SVG-based rendering
- **QtAwesome Compatible**: Drop-in replacement with familiar API
- **Scalable Vector Graphics**: Perfect quality at any size
- **Color Customization**: Runtime color changes and theming support
- **Qt6 Ready**: Modern CMake-based build system
- **High Performance**: Efficient caching and rendering
- **Cross Platform**: Works on Windows, macOS, and Linux

## Quick Start

### Installation

#### Using CMake

```cmake
find_package(QtLucide REQUIRED)
target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
```

#### Building from Source

##### Using CMake (Traditional)

```bash
git clone https://github.com/AstroAir/QtLucide.git
cd QtLucide
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

##### Using XMake (Modern Alternative)

```bash
git clone https://github.com/AstroAir/QtLucide.git
cd QtLucide
xmake
```

#### System Installation

##### CMake Installation

```bash
sudo make install
```

##### XMake Installation

```bash
sudo xmake install
```

#### Using in Your Project

##### CMake Projects

After installation, add QtLucide to your CMake project:

```cmake
find_package(QtLucide REQUIRED)
target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
```

##### XMake Projects

Add QtLucide to your XMake project:

```lua
add_requires("qtlucide")

target("your_target")
    add_packages("qtlucide")
target_end()
```

For more details on XMake build system, see [XMAKE_BUILD.md](XMAKE_BUILD.md).

### Basic Usage

```cpp
#include <QtLucide/QtLucide.h>

// Create QtLucide instance
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();

// Create icons
QPushButton* button = new QPushButton();
button->setIcon(lucide->icon("activity"));

// With custom options
QVariantMap options;
options["color"] = QColor(Qt::red);
options["scale-factor"] = 0.8;
button->setIcon(lucide->icon("heart", options));
```

## API Reference

### QtLucide Class

The main class for managing Lucide icons.

#### Methods

- `bool initLucide()` - Initialize the icon system
- `QIcon icon(const QString& name, const QVariantMap& options = {})` - Create icon by name
- `QIcon icon(Icons iconId, const QVariantMap& options = {})` - Create icon by enum
- `void setDefaultOption(const QString& name, const QVariant& value)` - Set default option
- `QStringList availableIcons() const` - Get list of available icons

#### Options

- `color` - Icon color (QColor)
- `color-disabled` - Color for disabled state
- `color-active` - Color for active state
- `color-selected` - Color for selected state
- `scale-factor` - Scale factor (0.1 to 2.0, default: 0.9)

## Migration from QtAwesome

QtLucide provides a compatible API for easy migration:

```cpp
// QtAwesome
fa::QtAwesome* awesome = new fa::QtAwesome(this);
awesome->initFontAwesome();
QIcon icon = awesome->icon(fa::fa_solid, fa::fa_heart);

// QtLucide
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();
QIcon icon = lucide->icon("heart");  // or lucide->icon(lucide::Icons::heart)
```

## Examples

See the `examples/` directory for complete examples:

- `basic_usage/` - Basic icon usage and customization

## Building and Testing

```bash
# Build with examples and tests
cmake -DQTLUCIDE_BUILD_EXAMPLES=ON -DQTLUCIDE_BUILD_TESTS=ON ..
make -j4

# Run tests
ctest

# Run examples
./QtLucideExample
./QtLucide\ Gallery
```

## Project Status

✅ **Complete and Fully Functional**

- **1634 Lucide Icons**: All icons from Lucide v0.460.0 included
- **High-DPI Support**: Automatic scaling for high-resolution displays
- **Qt6 Integration**: Native Qt6 QIcon support with proper caching
- **Customizable**: Color, scale factor, and custom painter support
- **Performance Optimized**: Efficient SVG rendering with caching
- **CMake Package**: Easy integration with `find_package(QtLucide)`
- **Comprehensive Tests**: Full test suite with 100% pass rate
- **Cross-Platform**: Works on Windows, Linux, and macOS

## License

MIT License - see [LICENSE](LICENSE) for details.

## Credits

- Icons from [Lucide](https://lucide.dev/) - ISC License
- Inspired by [QtAwesome](https://github.com/gamecreature/QtAwesome)
- Built with Qt6
