# QtLucide Build Scripts

This directory contains comprehensive build scripts for all supported build systems: CMake, Meson, and XMake.

## Overview

QtLucide supports three build systems, each with their own advantages:

- **CMake**: Industry standard, excellent IDE integration, widely supported
- **Meson**: Fast, modern, Python-based configuration
- **XMake**: Lua-based, cross-platform, package management integration

## Available Scripts

### Unified Build Scripts (Recommended)

- `build.py` - Unified Python script that auto-detects best build system
- `build.bat` - Windows batch wrapper for unified build script

### CMake Build Scripts

- `build_cmake.py` - Python script for CMake builds
- `build_cmake.bat` - Windows batch wrapper for CMake

### Meson Build Scripts

- `build_meson.py` - Python script for Meson builds
- `build_meson.bat` - Windows batch wrapper for Meson

### XMake Build Scripts

- `build_xmake.py` - Python script for XMake builds
- `build_xmake.bat` - Windows batch wrapper for XMake
- `test_xmake_build.py` - XMake build system test script

## Quick Start

### Unified Build Script (Recommended)

The unified build script automatically detects the best available build system:

```batch
# Windows - Auto-detect build system
scripts\build.bat --buildtype release --clean --test

# Linux/macOS - Auto-detect build system
python3 scripts/build.py --buildtype release --clean --test

# Force specific build system
scripts\build.bat --system cmake --buildtype debug --clean
python3 scripts/build.py --system meson --buildtype release --install
```

### Individual Build System Scripts

#### Windows Users

```batch
# CMake build (Debug)
scripts\build_cmake.bat --debug --clean

# Meson build (Release)
scripts\build_meson.bat --release --clean

# XMake build (Release with tests)
scripts\build_xmake.bat --release --test --clean
```

#### Linux/macOS Users

```bash
# CMake build (Debug)
python3 scripts/build_cmake.py --buildtype Debug --clean

# Meson build (Release)
python3 scripts/build_meson.py --buildtype release --clean

# XMake build (Release with tests)
python3 scripts/build_xmake.py --mode release --test --clean
```

## Common Options

All build scripts support similar options:

### Build Types/Modes

- **CMake**: `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`
- **Meson**: `debug`, `release`, `debugoptimized`
- **XMake**: `debug`, `release`, `releasedbg`, `minsizerel`

### Common Flags

- `--clean` - Clean build directory before building
- `--install` - Install after building
- `--test` - Run tests after building
- `--no-examples` - Don't build examples
- `--no-tests` - Don't build tests
- `--verbose` - Verbose output
- `--jobs N` - Number of parallel jobs (CMake/XMake)

### Unified Build Script Features

- `--system SYSTEM` - Force specific build system (cmake/meson/xmake)
- `--list-systems` - List available build systems
- Automatic build system detection and selection
- Consistent interface across all build systems
- Automatic build type normalization

## Prerequisites

### All Build Systems

- Qt6 (Core, Gui, Widgets, Svg components)
- Python 3.6+ (for resource generation)
- C++17 compatible compiler

### CMake

- CMake 3.16 or later
- Ninja (recommended) or Make/Visual Studio

### Meson

- Meson 0.59.0 or later
- Ninja build system
- Install: `pip install meson ninja`

### XMake

- XMake 2.5.0 or later
- Download from: <https://xmake.io>

## Build Examples

### Full Development Build

```bash
# CMake
python3 scripts/build_cmake.py --buildtype Debug --generate-resources --test --verbose

# Meson
python3 scripts/build_meson.py --buildtype debug --debug-executables --test

# XMake
python3 scripts/build_xmake.py --mode debug --generate-resources --test --verbose
```

### Release Build for Distribution

```bash
# CMake
python3 scripts/build_cmake.py --buildtype Release --clean --install

# Meson
python3 scripts/build_meson.py --buildtype release --clean --install

# XMake
python3 scripts/build_xmake.py --mode release --clean --install
```

### Build Specific Targets

```bash
# CMake - build only library
cmake --build build_cmake --target QtLucide

# Meson - build only library
meson compile -C build_meson QtLucide

# XMake - build only examples
xmake build QtLucideExample QtLucideGallery
```

## Build Directories

Each build system uses its own build directory:

- CMake: `build_cmake/`
- Meson: `build_meson/`
- XMake: `build/`

## Resource Generation

QtLucide requires resource generation from SVG icons. This can be done:

1. **Automatically** (recommended):
   - Use `--generate-resources` flag with build scripts
2. **Manually**:

   ```bash
   # Using Python script
   python3 tools/build_resources.py .

   # Using XMake task
   xmake generate-resources
   ```

## Troubleshooting

### Common Issues

1. **Qt6 not found**:

   - Ensure Qt6 is installed and in PATH
   - Set `CMAKE_PREFIX_PATH` for CMake
   - Use `PKG_CONFIG_PATH` for Meson

2. **Resource generation fails**:

   - Ensure Python 3 is available
   - Check that `tools/build_resources.py` exists
   - Run resource generation manually first

3. **Build fails on Windows**:
   - Use Visual Studio Developer Command Prompt
   - Ensure MSVC or MinGW is properly configured
   - Check that all dependencies are in PATH

### Getting Help

Run any script with `--help` or `-h` for detailed usage information:

```bash
python3 scripts/build_cmake.py --help
python3 scripts/build_meson.py --help
python3 scripts/build_xmake.py --help
```

## Integration with IDEs

### Visual Studio Code

- Use CMake Tools extension with `build_cmake.py`
- Configure tasks.json to use build scripts

### Qt Creator

- Open CMakeLists.txt directly
- Configure build directory to `build_cmake`

### CLion

- Open CMakeLists.txt directly
- Works best with CMake build system

## Contributing

When adding new build script features:

1. Update all three build systems consistently
2. Add corresponding batch file options
3. Update this README
4. Test on multiple platforms
