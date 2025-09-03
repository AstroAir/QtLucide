# Installation Guide

This guide covers all the ways to install QtLucide in your development environment.

## System Requirements

- **Qt**: Version 6.0 or later
- **CMake**: Version 3.16 or later
- **Compiler**: C++17 compatible (GCC 7+, Clang 5+, MSVC 2017+)
- **Python**: 3.6+ (optional, for building from source)

## Installation Methods

=== "CMake (Recommended)"

    ### Using find_package

    If QtLucide is installed system-wide, you can use it directly in your CMake project:

    ```cmake
    find_package(QtLucide REQUIRED)
    target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
    ```

    ### Using FetchContent

    For automatic download and build:

    ```cmake
    include(FetchContent)
    
    FetchContent_Declare(
        QtLucide
        GIT_REPOSITORY https://github.com/AstroAir/QtLucide.git
        GIT_TAG        v1.0.0  # or main for latest
    )
    
    FetchContent_MakeAvailable(QtLucide)
    target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
    ```

    ### Using add_subdirectory

    If you've added QtLucide as a git submodule:

    ```cmake
    add_subdirectory(third_party/QtLucide)
    target_link_libraries(your_target PRIVATE QtLucide::QtLucide)
    ```

=== "Build from Source"

    ### Clone and Build

    ```bash
    # Clone the repository
    git clone https://github.com/AstroAir/QtLucide.git
    cd QtLucide
    
    # Create build directory
    mkdir build && cd build
    
    # Configure with CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build
    cmake --build . --parallel
    
    # Install (optional)
    sudo cmake --install .
    ```

    ### Build Options

    Configure the build with these options:

    ```bash
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DQTLUCIDE_BUILD_EXAMPLES=ON \
        -DQTLUCIDE_BUILD_TESTS=ON \
        -DCMAKE_INSTALL_PREFIX=/usr/local
    ```

    | Option | Default | Description |
    |--------|---------|-------------|
    | `QTLUCIDE_BUILD_EXAMPLES` | `ON` | Build example applications |
    | `QTLUCIDE_BUILD_TESTS` | `ON` | Build test suite |
    | `CMAKE_INSTALL_PREFIX` | System default | Installation directory |

=== "qmake (Legacy)"

    ### Using QtLucide.pri

    For qmake projects, include the provided `.pri` file:

    ```pro
    # In your .pro file
    include(path/to/QtLucide/QtLucide.pri)
    ```

    ### Manual Setup

    ```pro
    QT += core gui widgets svg
    
    INCLUDEPATH += path/to/QtLucide/include
    
    CONFIG(debug, debug|release) {
        LIBS += -Lpath/to/QtLucide/lib -lQtLucided
    } else {
        LIBS += -Lpath/to/QtLucide/lib -lQtLucide
    }
    ```

## Platform-Specific Instructions

=== "Windows"

    ### Using vcpkg

    ```cmd
    vcpkg install qtlucide
    ```

    ### Visual Studio

    1. Clone or download QtLucide
    2. Open CMakeLists.txt in Visual Studio
    3. Configure and build using the CMake integration

    ### Qt Creator

    1. Open the QtLucide CMakeLists.txt in Qt Creator
    2. Configure the project with your Qt kit
    3. Build and install

=== "macOS"

    ### Using Homebrew

    ```bash
    # Add the tap (if available)
    brew tap astroair/qt
    brew install qtlucide
    ```

    ### Manual Build

    ```bash
    # Install dependencies
    brew install qt6 cmake
    
    # Clone and build
    git clone https://github.com/AstroAir/QtLucide.git
    cd QtLucide
    mkdir build && cd build
    cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt6)
    make -j$(sysctl -n hw.ncpu)
    sudo make install
    ```

=== "Linux"

    ### Ubuntu/Debian

    ```bash
    # Install dependencies
    sudo apt update
    sudo apt install qt6-base-dev qt6-svg-dev cmake build-essential
    
    # Clone and build
    git clone https://github.com/AstroAir/QtLucide.git
    cd QtLucide
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
    sudo make install
    ```

    ### Fedora/CentOS

    ```bash
    # Install dependencies
    sudo dnf install qt6-qtbase-devel qt6-qtsvg-devel cmake gcc-c++
    
    # Build process same as Ubuntu
    ```

    ### Arch Linux

    ```bash
    # Install dependencies
    sudo pacman -S qt6-base qt6-svg cmake gcc
    
    # Build process same as Ubuntu
    ```

## Verification

After installation, verify QtLucide is working:

### Test Installation

Create a simple test file `test_qtlucide.cpp`:

```cpp
#include <QApplication>
#include <QPushButton>
#include <QtLucide/QtLucide.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    lucide::QtLucide lucide;
    if (!lucide.initLucide()) {
        qDebug() << "Failed to initialize QtLucide";
        return 1;
    }
    
    QPushButton button("Test");
    button.setIcon(lucide.icon("heart"));
    button.show();
    
    qDebug() << "QtLucide initialized successfully!";
    qDebug() << "Available icons:" << lucide.availableIcons().size();
    
    return app.exec();
}
```

### CMakeLists.txt for Test

```cmake
cmake_minimum_required(VERSION 3.16)
project(QtLucideTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(QtLucide REQUIRED)

qt6_standard_project_setup()

qt6_add_executable(QtLucideTest test_qtlucide.cpp)
target_link_libraries(QtLucideTest PRIVATE 
    Qt6::Core 
    Qt6::Widgets 
    QtLucide::QtLucide
)
```

### Build and Run Test

```bash
mkdir test_build && cd test_build
cmake ..
cmake --build .
./QtLucideTest  # or QtLucideTest.exe on Windows
```

## Troubleshooting

### Common Issues

**Qt not found**
```bash
# Specify Qt installation path
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6
```

**CMake version too old**
```bash
# Update CMake or use older syntax
cmake .. -DCMAKE_CXX_STANDARD=17
```

**Missing SVG support**
```bash
# Install Qt SVG module
sudo apt install qt6-svg-dev  # Ubuntu/Debian
brew install qt6             # macOS (includes SVG)
```

**Permission denied during install**
```bash
# Use sudo for system installation
sudo cmake --install .

# Or install to user directory
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
```

### Getting Help

If you encounter issues:

1. Check the [FAQ](../faq/common-issues.md) for common problems
2. Search [GitHub Issues](https://github.com/AstroAir/QtLucide/issues)
3. Create a new issue with your system details and error messages

## Next Steps

Once QtLucide is installed, continue with:

- [Quick Start Guide](quick-start.md) - Get your first icon working
- [First Steps Tutorial](first-steps.md) - Detailed walkthrough
- [User Guide](../user-guide/index.md) - Comprehensive feature overview
