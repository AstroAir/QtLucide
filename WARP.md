# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

## Development Commands

### Build Commands

**Quick Setup (Auto-detect Build System):**
```bash
# Auto-detect best build system and build
python scripts/build.py --examples --tests

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
# Configure and build with examples and tests
xmake config --examples=true --tests=true
xmake build

# Or use Python wrapper script
python scripts/build_xmake.py --examples --tests

# Windows batch script
scripts\build_xmake.bat --examples --tests
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
# Generate all resources (QRC, enums, metadata)
python build_all_resources.py

# Using XMake task
xmake generate-resources

# Direct tool usage
python tools/build_resources.py .

# Clean generated resources
xmake clean-resources
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
- Generated files: `lucide_icons.qrc`, `icons.json`, `categories.json`, `tags.json`

### Dependencies

- **Required**: Qt6 (Core, Gui, Widgets, Svg), C++17 compiler, Python3
- **Optional**: Meson, XMake (alternatives to CMake)
- **Development**: clang-format for code formatting
- **CI/CD**: GitHub Actions with Qt 6.7.3/6.8.0, security scanning, performance monitoring

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

## CI/CD and Development Workflow

### GitHub Actions Workflows

**Main CI Pipeline (`ci.yml`)**:
- Multi-platform builds (Ubuntu 22.04, Windows 2022, macOS 13/14)
- Qt versions: 6.7.3 and 6.8.0
- Security hardening with Step Security Harden Runner
- Code coverage reporting (LCOV + Codecov)
- Code quality tools: clang-format, cppcheck, clang-tidy
- Container builds with SBOM generation

**Security Workflow (`security.yml`)**:
- CodeQL analysis for semantic code scanning
- Dependency vulnerability scanning
- Trivy, Semgrep, OSV Scanner integration
- TruffleHog secret detection
- OSSF Scorecard security assessment

**Performance Workflow (`performance.yml`)**:
- Google Benchmark integration
- Valgrind memory profiling and leak detection
- HTML reports deployed to GitHub Pages
- Automated performance regression detection

**XMake Workflow (`xmake.yml`)**:
- Alternative build system validation
- Build output comparison (CMake vs XMake)
- Installation testing

### Release Process

```bash
# Automated release on git tag
git tag v1.0.1
git push origin v1.0.1
```

Features automatic:
- Multi-platform package generation
- SBOM and build provenance attestation
- SHA256 checksums for all artifacts
- Container image publishing to GHCR
- Pre-release detection for alpha/beta/rc tags

### Quality Assurance

**Before Committing:**
```bash
# Format code
clang-format -i src/*.cpp include/QtLucide/*.h

# Run tests
ctest --test-dir build --output-on-failure

# Validate examples
python tools/check_example_icons.py
```

**Icon Validation:**
- Compile-time validation in `tools/check_example_icons.py`
- Ensures all referenced icons exist in SVG collection
- Validates icon name consistency across examples

## Troubleshooting Common Issues

### Build Issues

**Resource Generation Fails:**
```bash
# Manual resource generation
python tools/build_resources.py .

# Check Python dependencies
python -c "import json, pathlib"

# Verify SVG files exist
ls resources/icons/svg/*.svg | wc -l  # Should be 1600+
```

**Qt Not Found:**
```bash
# Set Qt path explicitly
export Qt6_DIR=/path/to/qt6/lib/cmake/Qt6

# Or use system package manager
sudo apt install qt6-base-dev qt6-svg-dev  # Ubuntu
brew install qt@6  # macOS
```

**XMake Issues:**
```bash
# Verify XMake installation
xmake --version

# Update packages
xmake require --upgrade

# Clean and reconfigure
xmake clean
xmake config --examples=true --tests=true
```

### Runtime Issues

**Icons Not Loading:**
- Check icon names match exactly (case-sensitive)
- Verify `initLucide()` was called before creating icons
- Use `availableIcons()` to list all available icons
- Check Qt resource system with `qrc://` paths

**Performance Issues:**
- Enable icon caching (default behavior)
- Use appropriate scale factors (0.8-1.2 range)
- Consider pre-loading frequently used icons
- Monitor SVG rendering performance in profiler

### Development Environment Setup

**Required Tools:**
- Qt6 (6.7.3+ recommended)
- CMake 3.16+ or XMake 2.8+ or Meson 0.59+
- Python 3.6+ with standard library
- C++17 compatible compiler (GCC 8+, Clang 10+, MSVC 2019+)

**Optional Development Tools:**
- clang-format for code formatting
- Ninja build system for faster builds
- Valgrind for memory debugging (Linux/macOS)
- Qt Creator or Visual Studio Code for development
