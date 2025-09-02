cmake_minimum_required(VERSION 3.16)

project(QtLucide VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

# Find Python3 for resource generation scripts
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Enable Qt6 features
qt6_standard_project_setup()

# Project options
option(QTLUCIDE_BUILD_EXAMPLES "Build QtLucide examples" ON)
option(QTLUCIDE_BUILD_TESTS "Build QtLucide tests" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src)

if(QTLUCIDE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(QTLUCIDE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
