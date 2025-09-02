cmake_minimum_required(VERSION 3.16)

project(QtLucide VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

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

# Export targets
install(TARGETS QtLucide
    EXPORT QtLucideTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export resource targets if they exist
if(TARGET QtLucide_resources_1)
    install(TARGETS QtLucide_resources_1
        EXPORT QtLucideTargets
    )
endif()

# Install headers
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)

# Create and install package config files
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtLucideConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtLucide
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/QtLucideConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtLucide
)

install(EXPORT QtLucideTargets
    FILE QtLucideTargets.cmake
    NAMESPACE QtLucide::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QtLucide
)
