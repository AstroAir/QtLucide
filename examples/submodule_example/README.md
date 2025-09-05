# QtLucide Submodule Example

This example demonstrates how to use QtLucide as a Git submodule in your Qt application.

## Features Demonstrated

- **Submodule Integration**: Shows how QtLucide automatically detects when used as submodule
- **Icon Usage**: Various Lucide icons used in menus and toolbars
- **Minimal Configuration**: Simple CMakeLists.txt with QtLucide as subdirectory
- **No Installation Required**: Works without installing QtLucide system-wide

## Building the Example

### Prerequisites

- Qt6 (Core, Gui, Widgets, Svg modules)
- CMake 3.16+
- C++17 compatible compiler
- Python3 (for QtLucide resource generation)

### Build Steps

```bash
# Navigate to the example directory
cd examples/submodule_example

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Run the example
./QtLucideSubmoduleExample  # Linux/macOS
# or
QtLucideSubmoduleExample.exe  # Windows
```

## What This Example Shows

### 1. Submodule Detection

When you build this example, QtLucide automatically detects that it's being used as a submodule and:

- Disables building examples and tests by default
- Skips installation configuration
- Provides the `QtLucide::QtLucide` target for linking

### 2. Icon Integration

The example uses various QtLucide icons:

- **File Plus** (`QtLucide::FilePlus`) - New file action
- **Folder Open** (`QtLucide::FolderOpen`) - Open file action  
- **Save** (`QtLucide::Save`) - Save file action
- **X** (`QtLucide::X`) - Exit action
- **Info** (`QtLucide::Info`) - About action
- **Home** (`QtLucide::Home`) - Home toolbar button
- **Settings** (`QtLucide::Settings`) - Settings toolbar button
- **Search** (`QtLucide::Search`) - Search toolbar button

### 3. Simple Configuration

The `CMakeLists.txt` shows the minimal configuration needed:

```cmake
# Add QtLucide as subdirectory (would be submodule in real project)
add_subdirectory(../.. QtLucide)

# Link to your target
target_link_libraries(QtLucideSubmoduleExample PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    QtLucide::QtLucide
)
```

## Real Project Integration

In a real project, you would:

1. **Add QtLucide as submodule**:
   ```bash
   git submodule add https://github.com/AstroAir/QtLucide.git third-party/QtLucide
   ```

2. **Update your CMakeLists.txt**:
   ```cmake
   add_subdirectory(third-party/QtLucide)
   target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
   ```

3. **Use QtLucide icons in your code**:
   ```cpp
   #include <QtLucide/QtLucide.h>
   
   QAction *action = new QAction("Save", this);
   action->setIcon(QtLucide::icon(QtLucide::Save));
   ```

## Benefits of Submodule Approach

- **Version Control**: Pin to specific QtLucide version
- **No System Dependencies**: No need to install QtLucide system-wide
- **Reproducible Builds**: Same QtLucide version across all environments
- **Automatic Configuration**: QtLucide detects submodule usage automatically
- **Clean Integration**: No conflicts with other projects

## Troubleshooting

### Python Not Found

If you get Python-related errors during build:

```bash
# Make sure Python3 is available
python3 --version
# or
python --version
```

### Qt6 Not Found

Make sure Qt6 is properly installed and CMAKE_PREFIX_PATH is set:

```bash
export CMAKE_PREFIX_PATH=/path/to/qt6
```

### Resource Generation Issues

If resource generation fails, you can manually run:

```bash
cd path/to/QtLucide
python3 tools/build_resources.py .
```

## See Also

- [Main QtLucide Documentation](../../README.md)
- [Submodule Integration Guide](../../docs/submodule-integration.md)
- [Basic Usage Example](../basic_usage/)
- [Gallery Example](../gallery/)
