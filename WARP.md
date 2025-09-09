# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Development Commands

### Build Commands

**Quick Setup (Any Build System):**
```bash
# Auto-detect best build system and build
python scripts\build.py --examples --tests

# On Windows, use batch script
scripts\build.bat --examples --tests
```

**CMake Build (Recommended):**
```bash
cmake -S . -B build -DQTLUCIDE_BUILD_TESTS=ON -DQTLUCIDE_BUILD_EXAMPLES=ON
cmake --build build -j
```

**Meson Build (Fast Alternative):**
```bash
meson setup builddir_meson -Dtests=true -Dexamples=true
meson compile -C builddir_meson
```

**XMake Build (Modern Alternative):**
```bash
xmake config --examples=true --tests=true
xmake build
```

### Testing Commands

```bash
# CMake
ctest --test-dir build --output-on-failure

# Meson
meson test -C builddir_meson

# XMake
xmake run QtLucideTests
```

### Resource Generation

Icons are automatically generated during build, but can be manually triggered:
```bash
python build_all_resources.py
```

### Running Examples

```bash
# Gallery example (comprehensive icon browser)
./build/examples/gallery/QtLucideGallery

# Basic usage example
./build/examples/basic_usage/QtLucideExample
```

## High-Level Architecture

### Core Components

**QtLucide Class (`src/QtLucide.cpp`, `include/QtLucide/QtLucide.h`)**
- Main API entry point providing QtAwesome-compatible interface
- Manages icon creation, customization options, and resource loading
- Handles initialization of icon mappings and SVG icon painter
- Key methods: `initLucide()`, `icon()`, `setDefaultOption()`, `availableIcons()`

**QtLucideIconEngine (`src/QtLucideIconEngine.cpp`)**
- QIconEngine implementation for Qt integration
- Handles actual icon rendering for different states (Normal, Disabled, Active, Selected)
- Manages caching and efficient SVG rendering
- Integrates with Qt's icon system for widgets like buttons, menus, toolbars

**QtLucideIconPainter (`src/QtLucideIconPainter.cpp`)**
- Abstract painter interface for icon rendering customization
- QtLucideSvgIconPainter: Default SVG-based painter implementation
- Supports custom painters for specialized icon rendering needs

**Generated Resources and Enums**
- `QtLucideEnums.h`: Auto-generated enum values for all 1634+ Lucide icons
- `QtLucideStrings.h`: Auto-generated string-to-enum mappings
- `lucide_icons.qrc`: Qt resource file containing all SVG icons

### Build System Architecture

**Multi-Build System Support:**
- CMake: Primary build system with Qt6 integration and package configuration
- Meson: Fast alternative with pkg-config integration
- XMake: Modern Lua-based build system

**Submodule Detection:**
- Automatically disables examples/tests when used as git submodule
- Provides alias target `QtLucide::QtLucide` for easy integration
- Optimized for use in larger projects without conflicts

**Resource Pipeline:**
- Python scripts in `tools/` directory generate icon enums and resource files
- SVG icons from Lucide are embedded as Qt resources
- Build systems automatically trigger resource generation when needed

### Icon System Flow

1. **Initialization**: `QtLucide::initLucide()` loads icon name→enum mappings
2. **Icon Request**: User calls `icon(name)` or `icon(enum)`
3. **Engine Creation**: QtLucideIconEngine created with rendering options
4. **Painter Assignment**: SVG painter or custom painter assigned to engine
5. **Rendering**: Engine renders SVG with applied customizations (color, scale)
6. **Caching**: Qt handles caching of rendered pixmaps for performance

### Examples Architecture

**Gallery Example (`examples/gallery/`)**
- Comprehensive icon browser with search, filtering, and export capabilities
- Modular architecture with separate widgets for grid display, search, and details
- Demonstrates advanced QtLucide usage patterns and customization options
- Uses IconMetadataManager for icon categorization and search functionality

**Basic Usage Example (`examples/basic_usage/`)**
- Simple demonstration of core QtLucide functionality
- Shows basic icon creation, customization, and widget integration
- Minimal example suitable for learning and quick reference

## Development Guidelines

### Code Style

- C++17 standard with Qt idioms
- 4-space indentation, 100-column limit (enforced by `.clang-format`)
- PascalCase for classes, lowerCamelCase for methods
- snake_case for enum values (e.g., `Icons::alert_circle`)

### Testing

- Qt Test framework used for unit and integration tests
- Tests located in `tests/` directory
- Run with `ctest` (CMake) or `meson test` (Meson)
- Some heavy UI tests disabled in `tests/main.cpp` for stability

### Resource Management

- Never edit generated headers (`QtLucideEnums.h`, `QtLucideStrings.h`)
- Regenerate resources using `python build_all_resources.py`
- SVG files located in `resources/icons/svg/`
- Resource validation via `tools/check_example_icons.py`

### Dependencies

- **Required**: Qt6 (Core, Gui, Widgets, Svg), C++17 compiler, Python3
- **Optional**: Meson, XMake (alternatives to CMake)
- **Development**: clang-format for code formatting

### Integration Patterns

**As Git Submodule (Recommended):**
```cmake
add_subdirectory(third-party/QtLucide)
target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
```

**System Installation:**
```cmake
find_package(QtLucide REQUIRED)
target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
```

**Migration from QtAwesome:**
- Drop-in replacement with compatible API
- Change `initFontAwesome()` to `initLucide()`
- Use string names or enum values instead of font character codes
