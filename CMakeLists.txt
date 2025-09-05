cmake_minimum_required(VERSION 3.16)

project(QtLucide VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect if QtLucide is being built as a submodule/subdirectory
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(QTLUCIDE_IS_MAIN_PROJECT TRUE)
else()
    set(QTLUCIDE_IS_MAIN_PROJECT FALSE)
endif()

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

# Find Python3 for resource generation scripts (only if needed)
if(QTLUCIDE_IS_MAIN_PROJECT OR QTLUCIDE_BUILD_EXAMPLES OR QTLUCIDE_BUILD_TESTS)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()

# Enable Qt6 features
qt6_standard_project_setup()

# Project options - default to OFF when used as submodule
option(QTLUCIDE_BUILD_EXAMPLES "Build QtLucide examples" ${QTLUCIDE_IS_MAIN_PROJECT})
option(QTLUCIDE_BUILD_TESTS "Build QtLucide tests" ${QTLUCIDE_IS_MAIN_PROJECT})
option(QTLUCIDE_INSTALL "Install QtLucide" ${QTLUCIDE_IS_MAIN_PROJECT})

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

# Debug executable for Gallery issues
add_executable(QtLucideGalleryDebug
    debug_gallery.cpp
    examples/gallery/IconMetadataManager.cpp
    examples/gallery/IconGridWidget.cpp
    examples/gallery/IconItem.cpp
)

target_include_directories(QtLucideGalleryDebug PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/gallery
)

target_link_libraries(QtLucideGalleryDebug
    Qt6::Core
    Qt6::Widgets
    Qt6::Svg
    QtLucide
)


# Install configuration (only when building as main project or explicitly requested)
if(QTLUCIDE_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install headers
    install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )

    # Create and install package config files
    set(QTLUCIDE_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/QtLucide)

    # Generate package config file
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtLucideConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfig.cmake"
        INSTALL_DESTINATION ${QTLUCIDE_CMAKE_DIR}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
    )

    # Generate package version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    # Install package config files
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfigVersion.cmake"
        DESTINATION ${QTLUCIDE_CMAKE_DIR}
    )

    # Export targets
    install(EXPORT QtLucideTargets
        FILE QtLucideTargets.cmake
        NAMESPACE QtLucide::
        DESTINATION ${QTLUCIDE_CMAKE_DIR}
    )
endif()

# When used as submodule, create an alias target for easier integration
if(NOT QTLUCIDE_IS_MAIN_PROJECT)
    add_library(QtLucide::QtLucide ALIAS QtLucide)
endif()
