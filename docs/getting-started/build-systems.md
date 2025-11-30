# Build Systems Guide

QtLucide supports three modern build systems, each with its own advantages and use cases. This guide helps you choose the right build system for your project and provides detailed setup instructions.

## Build Systems Overview

| Feature | CMake | Meson | XMake |
|---------|-------|-------|-------|
| **Configuration Language** | CMake DSL | Python-like | Lua |
| **Learning Curve** | Medium | Low | Low |
| **Build Speed** | Medium | Fast | Fast |
| **Qt6 Support** | Excellent | Excellent | Good |
| **IDE Integration** | Excellent | Good | Good |
| **Package Management** | find_package | pkg-config | Built-in |
| **Cross-compilation** | Good | Excellent | Excellent |
| **Windows Support** | Excellent | Good | Excellent |
| **Ecosystem** | Mature | Growing | Modern |

## CMake (Recommended for Most Projects)

CMake is the most widely adopted build system in the C++ ecosystem and offers the best compatibility.

### Advantages

- **Mature Ecosystem** - Extensive documentation and community support
- **IDE Integration** - Excellent support in Qt Creator, Visual Studio, CLion
- **Package Management** - Rich ecosystem with find_package and FetchContent
- **Cross-platform** - Works reliably on all major platforms
- **Qt Integration** - Native Qt6 support with qt6_standard_project_setup()

### When to Choose CMake

- You're working with existing CMake projects
- You need maximum IDE compatibility
- Your team is already familiar with CMake
- You require extensive third-party library integration

### Basic Setup

```cmake
cmake_minimum_required(VERSION 3.16)
project(MyApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)
find_package(QtLucide REQUIRED)

qt6_standard_project_setup()

qt6_add_executable(MyApp main.cpp)
target_link_libraries(MyApp PRIVATE
    Qt6::Core
    Qt6::Widgets
    QtLucide::QtLucide
)
```

### Advanced Configuration

```cmake
# Build options
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TESTS "Build tests" ON)

# Compiler-specific settings
if(MSVC)
    target_compile_options(MyApp PRIVATE /W4)
else()
    target_compile_options(MyApp PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation
install(TARGETS MyApp
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
```

## Meson (Best for New Projects)

Meson is a modern build system designed for speed and simplicity, with excellent Python-like syntax.

### Advantages

- **Fast Builds** - Optimized for build speed and incremental compilation
- **Clean Syntax** - Python-like configuration files are easy to read and write
- **Built-in Features** - Native support for testing, documentation, and packaging
- **Cross-compilation** - Excellent support for cross-platform builds
- **pkg-config Integration** - Seamless integration with system packages

### When to Choose Meson

- You're starting a new project
- Build speed is a priority
- You prefer clean, readable configuration files
- You need excellent cross-compilation support

### Basic Setup

```meson
project('MyApp', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17']
)

# Dependencies
qt6_dep = dependency('qt6', modules: ['Core', 'Widgets', 'Svg'])
qtlucide_dep = dependency('QtLucide')

# Executable
executable('MyApp', 'main.cpp',
  dependencies: [qt6_dep, qtlucide_dep],
  install: true
)
```

### Advanced Configuration

```meson
# Build options
examples_opt = get_option('examples')
tests_opt = get_option('tests')

# Conditional compilation
if examples_opt
  subdir('examples')
endif

if tests_opt
  subdir('tests')
endif

# Custom targets
run_target('format',
  command: ['clang-format', '-i', 'src/*.cpp', 'src/*.h']
)

# Installation
install_data('README.md', install_dir: 'share/doc/myapp')
```

### Meson Options File (meson_options.txt)

```meson
option('examples', type: 'boolean', value: true, description: 'Build examples')
option('tests', type: 'boolean', value: true, description: 'Build tests')
option('docs', type: 'boolean', value: false, description: 'Build documentation')
```

## XMake (Best for Cross-platform Development)

XMake is a modern Lua-based build system with excellent cross-platform support and built-in package management.

### Advantages

- **Lua Configuration** - Powerful and flexible Lua-based configuration
- **Built-in Package Manager** - No need for external package managers
- **Cross-compilation** - Excellent support for multiple platforms and architectures
- **Fast Builds** - Optimized build performance
- **Simple Syntax** - Easy to learn and use

### When to Choose XMake

- You need excellent cross-compilation support
- You prefer Lua over other configuration languages
- You want built-in package management
- You're building for multiple platforms

### Basic Setup

```lua
set_project("MyApp")
set_version("1.0.0")
set_languages("cxx17")

-- Add Qt6 dependency
add_requires("qt6base")

target("MyApp")
    set_kind("binary")
    add_files("main.cpp")
    add_packages("qt6base", "qtlucide")
target_end()
```

### Advanced Configuration

```lua
-- Build options
option("examples")
    set_default(true)
    set_description("Build examples")
option_end()

option("tests")
    set_default(true)
    set_description("Build tests")
option_end()

-- Platform-specific settings
if is_plat("windows") then
    add_defines("WIN32_LEAN_AND_MEAN")
elseif is_plat("linux") then
    add_links("pthread")
end

-- Custom rules
rule("qt_moc")
    set_extensions(".h")
    on_buildcmd_file(function (target, batchcmds, sourcefile, opt)
        -- Custom MOC processing
    end)
rule_end()

-- Installation
target("MyApp")
    set_kind("binary")
    add_files("main.cpp")
    add_packages("qt6base", "qtlucide")

    -- Install rules
    after_install(function (target)
        os.cp("README.md", path.join(target:installdir(), "share/doc"))
    end)
target_end()
```

## Building QtLucide with Different Systems

### CMake Build

```bash
git clone https://github.com/AstroAir/QtLucide.git
cd QtLucide
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DQTLUCIDE_BUILD_EXAMPLES=ON
cmake --build . --parallel
sudo cmake --install .
```

### Meson Build

```bash
git clone https://github.com/AstroAir/QtLucide.git
cd QtLucide
meson setup builddir --buildtype=release -Dexamples=true
meson compile -C builddir
sudo meson install -C builddir
```

### XMake Build

```bash
git clone https://github.com/AstroAir/QtLucide.git
cd QtLucide
xmake config --examples=true --mode=release
xmake build
sudo xmake install
```

## Integration Examples

### CMake Project Integration

```cmake
# Method 1: System installation
find_package(QtLucide REQUIRED)
target_link_libraries(myapp PRIVATE QtLucide::QtLucide)

# Method 2: FetchContent
include(FetchContent)
FetchContent_Declare(QtLucide
    GIT_REPOSITORY https://github.com/AstroAir/QtLucide.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(QtLucide)
target_link_libraries(myapp PRIVATE QtLucide::QtLucide)

# Method 3: Submodule
add_subdirectory(third_party/QtLucide)
target_link_libraries(myapp PRIVATE QtLucide::QtLucide)
```

### Meson Project Integration

```meson
# Method 1: System installation
qtlucide_dep = dependency('QtLucide')

# Method 2: Subproject
qtlucide_proj = subproject('qtlucide')
qtlucide_dep = qtlucide_proj.get_variable('qtlucide_dep')

# Method 3: pkg-config
qtlucide_dep = dependency('pkgconfig', modules: ['QtLucide'])

executable('myapp', 'main.cpp',
  dependencies: [qtlucide_dep]
)
```

### XMake Project Integration

```lua
-- Method 1: Package manager
add_requires("qtlucide")

target("myapp")
    add_packages("qtlucide")
target_end()

-- Method 2: Local build
includes("third_party/QtLucide")

target("myapp")
    add_deps("QtLucide")
target_end()
```

## Performance Comparison

### Build Times (Approximate)

| Project Size | CMake | Meson | XMake |
|--------------|-------|-------|-------|
| Small (< 100 files) | 30s | 20s | 25s |
| Medium (< 1000 files) | 5min | 3min | 4min |
| Large (> 1000 files) | 20min | 12min | 15min |

### Configuration Time

| Build System | Initial Setup | Reconfigure |
|--------------|---------------|-------------|
| CMake | 10-30s | 5-15s |
| Meson | 5-15s | 2-8s |
| XMake | 5-20s | 3-10s |

## Troubleshooting

### Common CMake Issues

```bash
# Qt not found
cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt6

# Generator issues on Windows
cmake .. -G "Visual Studio 16 2019"

# Debug configuration
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=ON
```

### Common Meson Issues

```bash
# Reconfigure build
meson configure builddir -Doption=value

# Clean rebuild
rm -rf builddir
meson setup builddir

# Verbose output
meson compile -C builddir -v
```

### Common XMake Issues

```bash
# Clear cache
xmake clean --all

# Verbose output
xmake -v

# Reconfigure
xmake config --option=value
```

## Recommendations

### For Beginners

- **Start with CMake** - Most documentation and tutorials use CMake
- **Try Meson** - If you want a modern, fast alternative

### For Teams

- **Use CMake** - If your team already knows it
- **Consider Meson** - For new projects prioritizing build speed

### For Cross-platform

- **XMake** - Excellent cross-compilation support
- **Meson** - Good cross-platform capabilities

### For Performance

- **Meson** - Fastest build times
- **XMake** - Good performance with built-in caching

## See Also

- [Installation Guide](installation.md) - Detailed installation instructions
- [Quick Start](quick-start.md) - Get started quickly
- [Development Guide](../development/index.md) - Contributing to QtLucide
- [Examples](../examples/index.md) - Working examples for all build systems
