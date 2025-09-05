# QtLucide Submodule Integration Guide

This guide explains how to integrate QtLucide as a Git submodule in your project.

## Overview

QtLucide is optimized for use as a submodule with the following features:

- **Automatic detection**: QtLucide automatically detects when it's being built as a submodule
- **Minimal footprint**: Examples and tests are disabled by default when used as submodule
- **Flexible configuration**: Parent projects can still enable examples/tests if needed
- **No installation conflicts**: Installation is disabled by default for submodules
- **Multiple build systems**: Supports CMake, Meson, and XMake

## Adding QtLucide as Submodule

### Step 1: Add the Submodule

```bash
# Add QtLucide as a submodule in your project
git submodule add https://github.com/AstroAir/QtLucide.git third-party/QtLucide

# Initialize and update the submodule
git submodule update --init --recursive
```

### Step 2: Integration with Different Build Systems

#### CMake Integration

Add the following to your `CMakeLists.txt`:

```cmake
# Add QtLucide subdirectory
add_subdirectory(third-party/QtLucide)

# Link QtLucide to your target
target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
```

**Optional: Enable examples/tests in submodule**
```cmake
# Enable examples and tests even when used as submodule
set(QTLUCIDE_BUILD_EXAMPLES ON CACHE BOOL "Build QtLucide examples" FORCE)
set(QTLUCIDE_BUILD_TESTS ON CACHE BOOL "Build QtLucide tests" FORCE)

add_subdirectory(third-party/QtLucide)
```

#### Meson Integration

Add to your `meson.build`:

```meson
# Add QtLucide as subproject
qtlucide_proj = subproject('QtLucide')
qtlucide_dep = qtlucide_proj.get_variable('qtlucide_dep')

# Use in your executable
executable('your_app',
  'src/main.cpp',
  dependencies: [qtlucide_dep]
)
```

**Optional: Enable examples/tests in subproject**
```meson
# Enable examples and tests even when used as subproject
qtlucide_proj = subproject('QtLucide', 
  default_options: ['examples=enabled', 'tests=enabled']
)
```

#### XMake Integration

Add to your `xmake.lua`:

```lua
-- Add QtLucide subdirectory
includes("third-party/QtLucide")

-- Your target
target("your_app")
    set_kind("binary")
    add_files("src/main.cpp")
    add_deps("QtLucide")
target_end()
```

**Optional: Enable examples/tests in submodule**
```lua
-- Enable examples and tests even when used as subproject
set_config("examples", true)
set_config("tests", true)

includes("third-party/QtLucide")
```

## Usage Example

Here's a simple example of using QtLucide in your application:

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QtLucide/QtLucide.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    QMainWindow window;
    QToolBar *toolbar = window.addToolBar("Main");
    
    // Create actions with Lucide icons
    QAction *openAction = new QAction("Open", &window);
    openAction->setIcon(QtLucide::icon(QtLucide::FolderOpen));
    toolbar->addAction(openAction);
    
    QAction *saveAction = new QAction("Save", &window);
    saveAction->setIcon(QtLucide::icon(QtLucide::Save));
    toolbar->addAction(saveAction);
    
    window.show();
    return app.exec();
}
```

## Build Configuration Options

When QtLucide is used as a submodule, the following default behaviors apply:

### CMake Options
- `QTLUCIDE_BUILD_EXAMPLES`: OFF (default when submodule)
- `QTLUCIDE_BUILD_TESTS`: OFF (default when submodule)
- `QTLUCIDE_INSTALL`: OFF (default when submodule)

### Meson Options
- `examples`: auto (disabled when subproject)
- `tests`: auto (disabled when subproject)
- `install`: true (but only applies to main project)

### XMake Options
- `examples`: false (default when submodule)
- `tests`: false (default when submodule)
- `install`: false (default when submodule)

## Troubleshooting

### Resource Generation Issues

If you encounter issues with resource generation, ensure Python3 is available:

```bash
# Check Python availability
python3 --version
# or
python --version
```

### Qt6 Dependencies

Make sure your parent project has the required Qt6 components:
- Qt6::Core
- Qt6::Gui
- Qt6::Widgets
- Qt6::Svg

### Build System Conflicts

If you encounter build system conflicts:

1. **CMake**: Ensure you're using compatible CMake versions (3.16+)
2. **Meson**: Ensure Meson version 0.59.0+
3. **XMake**: Ensure XMake is properly configured for Qt6

## Advanced Configuration

### Custom Resource Generation

If you need to customize resource generation in a submodule setup:

```cmake
# Disable automatic resource generation
set(QTLUCIDE_GENERATE_RESOURCES OFF)

# Add custom resource generation step
add_custom_target(generate_custom_resources
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/third-party/QtLucide/tools/build_resources.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third-party/QtLucide
)
```

### Selective Icon Inclusion

For reduced binary size, you can modify the resource generation to include only specific icons. See the main documentation for details on customizing icon sets.

## Best Practices

1. **Pin to specific version**: Use a specific commit or tag for reproducible builds
2. **Regular updates**: Keep the submodule updated for bug fixes and new features
3. **Test integration**: Verify that your integration works with different build configurations
4. **Documentation**: Document the QtLucide version and configuration in your project

## Support

For issues related to submodule integration, please check:

1. [Main QtLucide Documentation](../README.md)
2. [GitHub Issues](https://github.com/AstroAir/QtLucide/issues)
3. [Examples Directory](../examples/) for reference implementations
