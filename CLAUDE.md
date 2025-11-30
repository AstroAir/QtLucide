# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

QtLucide is a Qt library that provides access to 1634+ Lucide icons with SVG-based rendering. It's designed as a QtAwesome-compatible replacement using modern Qt6 features and efficient vector graphics.

## Build Commands

QtLucide supports three build systems:

### CMake (Traditional)

```bash
# Configure with examples and tests
cmake -DQTLUCIDE_BUILD_EXAMPLES=ON -DQTLUCIDE_BUILD_TESTS=ON ..

# Build
make -j4

# Run tests
ctest

# Run examples
./QtLucideExample
./QtLucideGallery
```

### XMake (Modern)

```bash
# Configure with examples and tests
xmake config --examples=true --tests=true

# Build
xmake build

# Generate resources (required before first build)
xmake generate-resources

# Run tests
xmake run QtLucideTests

# Run examples
xmake run QtLucideExample
xmake run QtLucideGallery
```

### Meson (Modern)

```bash
# Setup and build
meson setup builddir -Dexamples=true -Dtests=true
meson compile -C builddir

# Run tests
meson test -C builddir

# Run examples
./builddir/examples/basic_usage/QtLucideExample
./builddir/examples/gallery/QtLucideGallery
```

## Resource Generation

Before building, ensure icon resources are generated:

```bash
# For CMake/Meson (automatic if Python3 found)
python3 tools/build_resources.py .

# For XMake (manual)
xmake generate-resources
```

## Architecture

### Core Components

- **QtLucide Class** (`src/QtLucide.cpp`, `include/QtLucide/QtLucide.h`): Main API entry point
  - Manages icon creation and customization
  - Handles default options and custom painters
  - Provides both enum-based and string-based icon access

- **QtLucideIconEngine** (`src/QtLucideIconEngine.cpp`): QIconEngine implementation
  - Handles Qt icon rendering pipeline
  - Manages different icon states (normal, disabled, active, selected)
  - Integrates with Qt's caching system

- **QtLucideIconPainter** (`src/QtLucideIconPainter.cpp`): SVG rendering engine
  - Processes SVG data and applies customizations
  - Handles color changes, scaling, and effects
  - Supports custom painter registration

### Gallery Application

The gallery example (`examples/gallery/`) demonstrates advanced QtLucide usage:

- **IconMetadataManager**: Loads and manages icon metadata
- **ThemeManager**: Handles dark/light theme switching
- **ResponsiveLayoutManager**: Adaptive UI layouts
- **BatchExportManager**: Bulk icon export functionality

### Testing Structure

Tests are organized by category:

- `tests/unit/core/`: Core functionality tests
- `tests/unit/components/`: Gallery component tests
- `tests/integration/`: Integration tests
- `tests/performance/`: Performance regression tests
- `tests/gallery/`: Gallery-specific tests

## Key Design Patterns

1. **Qt Resource System**: Icons are embedded using Qt's resource system
2. **Enum + String API**: Icons accessible by both enum (performance) and string (flexibility)
3. **Option Merging**: Default options merged with icon-specific options
4. **Custom Painter Support**: Extensible rendering system for custom icons
5. **Submodule Detection**: Automatically detects when built as Git submodule

## Common Development Tasks

### Adding New Icons

1. Add SVG files to `resources/icons/svg/`
2. Run `python3 tools/build_resources.py .` to regenerate resources
3. Icons will be automatically available in the API

### Testing Single Components

```bash
# CMake
ctest -R "test_icon_loading"

# XMake
xmake run QtLucideTests --test-filter="icon loading"

# Meson
meson test -C builddir --suite="icon loading"
```

### Building as Submodule

QtLucide automatically detects submodule usage and disables examples/tests by default:

```cmake
add_subdirectory(third-party/QtLucide)
target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
```

## Dependencies

- **Qt6** (Core, Gui, Widgets, Svg): Required framework
- **Python3**: For resource generation scripts
- **C++17**: Minimum language standard

## Platform-Specific Notes

### Windows

- Qt6 packages often include SVG support in qt6base
- MSYS2 paths are hardcoded in xmake.lua for convenience

### macOS

- Uses frameworks for Qt6 integration
- Proper rpath settings for deployment

### Linux

- pkg-config integration available
- Standard Qt6 package installation

## Memory Management

- QtLucide uses Qt's parent-child system for memory management
- Custom painters are owned by QtLucide instance
- QIcon instances remain valid after QtLucide destruction
- Resource cleanup handled automatically in destructor

## Performance Considerations

- Enum-based icon creation is faster than string-based
- SVG rendering is cached by Qt's QIcon system
- Default options are applied efficiently during creation
- Resource embedding eliminates file system access at runtime
