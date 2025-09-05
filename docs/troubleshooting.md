# QtLucide Troubleshooting Guide

This guide helps resolve common issues when building or using QtLucide.

## Build Issues

### Resource Generation Problems

**Problem**: Resource generation fails with Python errors
```
FileNotFoundError: [Errno 2] No such file or directory: 'python3'
```

**Solutions**:
1. **Install Python 3**: Make sure Python 3.7+ is installed
2. **Check PATH**: Ensure Python is in your system PATH
3. **Manual generation**: Run resource generation manually:
   ```bash
   cd QtLucide
   python tools/build_resources.py .
   ```

### CMake Issues

**Problem**: CMake can't find Qt6
```
CMake Error: Could not find a package configuration file provided by "Qt6"
```

**Solutions**:
1. **Set CMAKE_PREFIX_PATH**:
   ```bash
   export CMAKE_PREFIX_PATH=/path/to/qt6
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6
   ```
2. **Install Qt6 development packages**:
   ```bash
   # Ubuntu/Debian
   sudo apt install qt6-base-dev qt6-svg-dev
   
   # Fedora
   sudo dnf install qt6-qtbase-devel qt6-qtsvg-devel
   
   # macOS with Homebrew
   brew install qt6
   ```

**Problem**: Submodule detection not working
```
Examples and tests are building when they shouldn't
```

**Solutions**:
1. **Force disable**: Override the detection
   ```cmake
   set(QTLUCIDE_BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
   set(QTLUCIDE_BUILD_TESTS OFF CACHE BOOL "Build tests" FORCE)
   ```

### Meson Issues

**Problem**: Meson can't find Qt6
```
Run-time dependency qt6 found: NO
```

**Solutions**:
1. **Install pkg-config files**: Ensure Qt6 pkg-config files are installed
2. **Set PKG_CONFIG_PATH**:
   ```bash
   export PKG_CONFIG_PATH=/path/to/qt6/lib/pkgconfig:$PKG_CONFIG_PATH
   ```

**Problem**: Resource compilation fails
```
qt.compile_resources got unknown keyword arguments "depends"
```

**Solution**: This is fixed in newer versions. Update your Meson version or use the fixed configuration.

### XMake Issues

**Problem**: XMake can't find Qt6
```
Qt SDK directory not found
```

**Solutions**:
1. **Set Qt path**: Configure Qt6 path for XMake
   ```bash
   xmake config --qt=/path/to/qt6
   ```
2. **Install Qt6**: Make sure Qt6 is properly installed

**Problem**: Linking errors with tests
```
undefined reference to vtable
```

**Solution**: This is a known issue with some test files. The main library builds correctly.

## Runtime Issues

### Icons Not Loading

**Problem**: Icons appear as empty/blank
```cpp
QIcon icon = qtlucide.icon(lucide::Icons::save);
// icon.isNull() returns true
```

**Solutions**:
1. **Check resource initialization**: Ensure resources are properly initialized
2. **Verify icon name**: Make sure you're using the correct icon enum
3. **Debug resource loading**:
   ```cpp
   lucide::QtLucide qtlucide;
   qDebug() << "Available icons:" << qtlucide.availableIcons().size();
   ```

### Memory Issues

**Problem**: Memory leaks or crashes
```
Segmentation fault or memory access violations
```

**Solutions**:
1. **Check Qt version compatibility**: Ensure you're using a compatible Qt6 version
2. **Proper cleanup**: Make sure QtLucide objects are properly destroyed
3. **Thread safety**: Don't share QtLucide instances across threads without proper synchronization

## Integration Issues

### Submodule Integration

**Problem**: Submodule not building correctly
```
CMake Error: add_subdirectory given source "third-party/QtLucide" which is not an existing directory
```

**Solutions**:
1. **Initialize submodule**:
   ```bash
   git submodule update --init --recursive
   ```
2. **Check submodule path**: Ensure the path in CMakeLists.txt matches your directory structure

**Problem**: Version conflicts
```
Different versions of QtLucide in different parts of the project
```

**Solutions**:
1. **Pin to specific commit**: Use a specific commit hash for reproducible builds
2. **Update submodule**: Regularly update to the latest stable version
   ```bash
   git submodule update --remote
   ```

### Package Manager Issues

**Problem**: vcpkg/Conan integration issues
```
Package not found or version conflicts
```

**Solutions**:
1. **Use submodule approach**: For now, submodule integration is the most reliable
2. **Manual installation**: Build and install QtLucide manually
3. **Check package manager documentation**: Refer to specific package manager docs

## Performance Issues

### Slow Icon Loading

**Problem**: Icons take too long to load
```
First icon load is very slow
```

**Solutions**:
1. **Preload icons**: Load commonly used icons at startup
2. **Use caching**: QtLucide automatically caches icons, but first load will be slower
3. **Optimize SVG files**: Ensure SVG files are optimized

### Large Binary Size

**Problem**: Binary size is too large
```
Executable size increased significantly after adding QtLucide
```

**Solutions**:
1. **Selective icon inclusion**: Modify resource generation to include only needed icons
2. **Link-time optimization**: Enable LTO in your build system
3. **Strip debug symbols**: Remove debug information from release builds

## Platform-Specific Issues

### Windows

**Problem**: MSVC compilation errors
```
C++ standard library issues
```

**Solutions**:
1. **Use C++17**: Ensure C++17 standard is enabled
2. **Update MSVC**: Use Visual Studio 2019 or newer
3. **Check Qt6 MSVC compatibility**: Ensure Qt6 was built with compatible MSVC version

### macOS

**Problem**: Code signing issues
```
Developer cannot be verified
```

**Solutions**:
1. **Disable Gatekeeper temporarily**: For development only
2. **Sign your application**: Use proper code signing for distribution

### Linux

**Problem**: Missing dependencies
```
Shared library not found
```

**Solutions**:
1. **Install development packages**: Install Qt6 development packages
2. **Check library paths**: Ensure Qt6 libraries are in LD_LIBRARY_PATH
3. **Use system package manager**: Install Qt6 through your distribution's package manager

## Getting Help

If you encounter issues not covered here:

1. **Check GitHub Issues**: Search existing issues at https://github.com/AstroAir/QtLucide/issues
2. **Create detailed bug report**: Include:
   - Operating system and version
   - Qt6 version
   - Build system (CMake/Meson/XMake)
   - Complete error messages
   - Minimal reproduction case
3. **Check documentation**: Review the main README and integration guides
4. **Community support**: Ask questions in GitHub Discussions

## Debugging Tips

### Enable Verbose Output

**CMake**:
```bash
cmake --build . --verbose
```

**Meson**:
```bash
meson compile -v
```

**XMake**:
```bash
xmake -v
```

### Debug Resource Loading

```cpp
#include <QDebug>
#include <QDir>

// Check if resources are available
qDebug() << "Resource search paths:" << QDir::searchPaths("icons");

// List available resources
QDir resourceDir(":/icons");
qDebug() << "Available resources:" << resourceDir.entryList();
```

### Memory Debugging

Use tools like Valgrind (Linux), AddressSanitizer, or Visual Studio Diagnostic Tools to identify memory issues.

### Build System Debugging

1. **Clean builds**: Always try a clean build first
2. **Check generated files**: Verify that resource files are generated correctly
3. **Dependency tracking**: Ensure all dependencies are properly specified
