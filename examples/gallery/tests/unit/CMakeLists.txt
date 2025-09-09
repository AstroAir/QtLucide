# QtLucide Gallery Unit Tests
cmake_minimum_required(VERSION 3.16)

project(QtLucideGalleryUnitTests VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Test Svg)

# Enable Qt's MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/config
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/widgets/grids
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/widgets/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/widgets/search
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/widgets/viewers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ui/windows
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
)

# Gallery component sources (needed for testing)
set(GALLERY_SOURCES
    # Core managers
    ../../src/core/managers/IconMetadataManager.cpp
    ../../src/core/managers/ImageMetadataManager.cpp
    ../../src/core/managers/ContentManager.cpp
    ../../src/core/managers/FavoritesManager.cpp

    # UI widgets
    ../../src/ui/widgets/grids/IconGridWidget.cpp
    ../../src/ui/widgets/grids/IconThumbnailGridWidget.cpp
    ../../src/ui/widgets/search/SearchWidget.cpp
    ../../src/ui/widgets/search/IconSearchWidget.cpp
    ../../src/ui/widgets/search/CategoryFilterWidget.cpp
    ../../src/ui/widgets/panels/CategorySidebarWidget.cpp
    ../../src/ui/widgets/viewers/ImageViewerWidget.cpp

    # Utils
    ../../src/core/utils/GalleryLogger.cpp
)

# Gallery component headers
set(GALLERY_HEADERS
    ../../src/core/managers/IconMetadataManager.h
    ../../src/core/managers/ImageMetadataManager.h
    ../../src/core/managers/ContentManager.h
    ../../src/core/managers/FavoritesManager.h
    ../../src/ui/widgets/grids/IconGridWidget.h
    ../../src/ui/widgets/grids/IconThumbnailGridWidget.h
    ../../src/ui/widgets/search/SearchWidget.h
    ../../src/ui/widgets/search/IconSearchWidget.h
    ../../src/ui/widgets/search/CategoryFilterWidget.h
    ../../src/ui/widgets/panels/CategorySidebarWidget.h
    ../../src/ui/widgets/viewers/ImageViewerWidget.h
    ../../src/core/utils/GalleryLogger.h
)

# Test sources
set(TEST_SOURCES
    test_gallery_components.cpp
)

# Create the test executable
add_executable(QtLucideGalleryUnitTests
    ${TEST_SOURCES}
    ${GALLERY_SOURCES}
    ${GALLERY_HEADERS}
)

# Link Qt libraries
target_link_libraries(QtLucideGalleryUnitTests
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
        Qt6::Svg
)

# Try to link QtLucide library if available
find_library(QTLUCIDE_LIB
    NAMES QtLucide libQtLucide
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../build/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../build_minimal/src
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../build-fresh/src
    NO_DEFAULT_PATH
)

if(QTLUCIDE_LIB)
    target_link_libraries(QtLucideGalleryUnitTests PRIVATE ${QTLUCIDE_LIB})
    message(STATUS "Found QtLucide library: ${QTLUCIDE_LIB}")
else()
    message(WARNING "QtLucide library not found, tests may not link properly")
endif()

# Set target properties
set_target_properties(QtLucideGalleryUnitTests PROPERTIES
    WIN32_EXECUTABLE FALSE
    MACOSX_BUNDLE FALSE
    OUTPUT_NAME "QtLucideGalleryUnitTests"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Platform-specific configurations
if(WIN32)
    target_compile_definitions(QtLucideGalleryUnitTests PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        UNICODE
        _UNICODE
    )
endif()

# Compiler-specific optimizations
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(QtLucideGalleryUnitTests PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:Debug>:-g -O0>
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(QtLucideGalleryUnitTests PRIVATE
        /W4
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
        $<$<CONFIG:Debug>:/Od /Zi>
    )
endif()

# Enable testing
enable_testing()
add_test(NAME GalleryComponentTests COMMAND QtLucideGalleryUnitTests)

message(STATUS "QtLucide Gallery Unit Tests configured successfully")
message(STATUS "  - Test executable: QtLucideGalleryUnitTests")
message(STATUS "  - Gallery components included for testing")
message(STATUS "  - Qt Test framework enabled")
