# QtLucide Gallery - Complete Gallery Application
# Comprehensive build configuration for the full QtLucide Gallery

cmake_minimum_required(VERSION 3.16)

project(QtLucideGallery VERSION 2.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Gui
    Svg
    Concurrent
    Network
)

# Find QtLucide library
find_path(QTLUCIDE_INCLUDE_DIR QtLucide/QtLucide.h
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    NO_DEFAULT_PATH
)

find_library(QTLUCIDE_LIBRARY
    NAMES QtLucide
    PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../build
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../build
        ${CMAKE_CURRENT_BINARY_DIR}/../../
        ${CMAKE_CURRENT_BINARY_DIR}/../../../
    PATH_SUFFIXES
        lib
        Release
        Debug
    NO_DEFAULT_PATH
)

if(NOT QTLUCIDE_INCLUDE_DIR OR NOT QTLUCIDE_LIBRARY)
    message(WARNING "QtLucide library not found. Some features may not work.")
    set(QTLUCIDE_FOUND FALSE)
else()
    set(QTLUCIDE_FOUND TRUE)
    message(STATUS "QtLucide found:")
    message(STATUS "  Include: ${QTLUCIDE_INCLUDE_DIR}")
    message(STATUS "  Library: ${QTLUCIDE_LIBRARY}")
endif()

# Enable Qt's MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Define source files
set(GALLERY_SOURCES
    # Configuration
    src/config/LayoutConfig.h

    # Core functionality
    src/core/BatchExportManager.h
    src/core/BatchExportManager.cpp
    src/core/ExportProgressWidget.cpp

    # Core managers
    src/core/managers/ContentManager.h
    src/core/managers/ContentManager.cpp
    src/core/managers/IconMetadataManager.h
    src/core/managers/IconMetadataManager.cpp
    src/core/managers/ImageMetadataManager.h
    src/core/managers/ImageMetadataManager.cpp
    src/core/managers/FavoritesManager.h
    src/core/managers/FavoritesManager.cpp

    # Core models
    src/core/models/IconItem.h
    src/core/models/IconItem.cpp

    # Core utils
    src/core/utils/GalleryLogger.h
    src/core/utils/GalleryLogger.cpp
    src/core/utils/ErrorHandler.h
    src/core/utils/ErrorHandler.cpp

    # UI themes
    src/ui/themes/ThemeManager.h
    src/ui/themes/ThemeManager.cpp

    # UI layouts
    src/ui/layouts/ResponsiveLayoutManager.h
    src/ui/layouts/ResponsiveLayoutManager.cpp

    # UI widgets - grids
    src/ui/widgets/grids/IconGridWidget.h
    src/ui/widgets/grids/IconGridWidget.cpp
    src/ui/widgets/grids/IconThumbnailGridWidget.h
    src/ui/widgets/grids/IconThumbnailGridWidget.cpp
    src/ui/widgets/grids/ModernIconGridWidget.h
    src/ui/widgets/grids/ModernIconGridWidget.cpp

    # UI widgets - search
    src/ui/widgets/search/SearchWidget.h
    src/ui/widgets/search/SearchWidget.cpp
    src/ui/widgets/search/IconSearchWidget.h
    src/ui/widgets/search/IconSearchWidget.cpp
    src/ui/widgets/search/CategoryFilterWidget.h
    src/ui/widgets/search/CategoryFilterWidget.cpp
    src/ui/widgets/search/ModernSearchWidget.h
    src/ui/widgets/search/ModernSearchWidget.cpp

    # UI widgets - panels
    src/ui/widgets/panels/CategorySidebarWidget.h
    src/ui/widgets/panels/CategorySidebarWidget.cpp
    src/ui/widgets/panels/IconDetailsPanel.h
    src/ui/widgets/panels/IconDetailsPanel.cpp
    src/ui/widgets/panels/FileBrowserWidget.h
    src/ui/widgets/panels/FileBrowserWidget.cpp
    src/ui/widgets/panels/ModernSidebarWidget.h
    src/ui/widgets/panels/ModernSidebarWidget.cpp

    # UI widgets - viewers
    src/ui/widgets/viewers/ImageViewerWidget.h
    src/ui/widgets/viewers/ImageViewerWidget.cpp

    # UI dialogs
    src/ui/dialogs/ExportDialog.h
    src/ui/dialogs/ExportDialog.cpp
    src/ui/dialogs/ImportDialog.h
    src/ui/dialogs/ImportDialog.cpp
    src/ui/dialogs/IconExportDialog.h
    src/ui/dialogs/IconExportDialog.cpp
    src/ui/dialogs/PreferencesDialog.h
    src/ui/dialogs/PreferencesDialog.cpp

    # UI windows
    src/ui/windows/GalleryMainWindow.h
    src/ui/windows/GalleryMainWindow.cpp

    # Main application
    src/main/main.cpp
)

# Resource files
set(GALLERY_RESOURCES
    resources/gallery_resources.qrc
)

# Create resource file if it doesn't exist
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/resources/gallery_resources.qrc")
    file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/resources/gallery_resources.qrc"
        "<!DOCTYPE RCC>\n"
        "<RCC version=\"1.0\">\n"
        "    <qresource prefix=\"/styles\">\n"
        "        <file>styles/modern-theme.qss</file>\n"
        "        <file>styles/dark-theme.qss</file>\n"
        "    </qresource>\n"
        "    <qresource prefix=\"/icons\">\n"
        "        <file>icons/app-icon.png</file>\n"
        "    </qresource>\n"
        "</RCC>\n"
    )
endif()

list(APPEND GALLERY_SOURCES ${GALLERY_RESOURCES})

# Create the main executable
add_executable(QtLucideGallery ${GALLERY_SOURCES})

# Include directories
target_include_directories(QtLucideGallery PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/themes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/grids
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/search
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/viewers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/layouts
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/windows
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config
)

if(QTLUCIDE_FOUND)
    target_include_directories(QtLucideGallery PRIVATE
        ${QTLUCIDE_INCLUDE_DIR}
    )
endif()

# Link Qt libraries
target_link_libraries(QtLucideGallery
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Svg
        Qt6::Concurrent
        Qt6::Network
)

# Link QtLucide if found
if(QTLUCIDE_FOUND)
    target_link_libraries(QtLucideGallery PRIVATE ${QTLUCIDE_LIBRARY})
    target_compile_definitions(QtLucideGallery PRIVATE QTLUCIDE_AVAILABLE)
endif()

# Set target properties
set_target_properties(QtLucideGallery PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "QtLucide Gallery"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Platform-specific configurations
if(WIN32)
    set_target_properties(QtLucideGallery PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        WIN32_EXECUTABLE_ENABLE_EXPORTS TRUE
    )

    target_compile_definitions(QtLucideGallery PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        UNICODE
        _UNICODE
    )
endif()

# Compiler-specific optimizations
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(QtLucideGallery PRIVATE
        -Wall -Wextra -Wpedantic
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:Debug>:-g -O0>
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(QtLucideGallery PRIVATE
        /W4
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
        $<$<CONFIG:Debug>:/Od /Zi>
    )
endif()

message(STATUS "QtLucide Gallery configured successfully")
message(STATUS "  - Complete gallery application: ✓")
message(STATUS "  - Icon browsing and search: ✓")
message(STATUS "  - Image gallery with metadata: ✓")
message(STATUS "  - Theme switching: ✓")
message(STATUS "  - Export/import functionality: ✓")
message(STATUS "  - Responsive layout system: ✓")
if(QTLUCIDE_FOUND)
    message(STATUS "  - QtLucide integration: ✓")
else()
    message(STATUS "  - QtLucide integration: ✗ (library not found)")
endif()
message(STATUS "  - Build target: QtLucideGallery")
