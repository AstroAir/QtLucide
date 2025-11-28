# QtLucide Gallery Example - CMakeLists.txt
# Modern Qt6 icon gallery showcasing QtLucide library

cmake_minimum_required(VERSION 3.16)

project(QtLucideGallery
    VERSION 1.0.0
    DESCRIPTION "QtLucide Icon Gallery Application"
    LANGUAGES CXX
)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 auto-tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find required Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Svg
    Concurrent
    Network
)

# Gallery source files
set(GALLERY_SOURCES
    # Main
    src/main/main.cpp

    # Core - Utils
    src/core/utils/GalleryLogger.cpp
    src/core/utils/ErrorHandler.cpp

    # Core - Managers
    src/core/managers/IconMetadataManager.cpp
    src/core/managers/FavoritesManager.cpp
    src/core/managers/ContentManager.cpp
    src/core/managers/ManagerStubs.cpp

    # Core - Models
    src/core/models/IconItem.cpp

    # Core - Export
    src/core/BatchExportManager.cpp
    src/core/ExportProgressWidget.cpp

    # UI - Themes
    src/ui/themes/ThemeManager.cpp

    # UI - Layouts
    src/ui/layouts/ResponsiveLayoutManager.cpp

    # UI - Controllers
    src/ui/controllers/SearchController.cpp
    src/ui/controllers/PerformanceMonitor.cpp

    # UI - Windows
    src/ui/windows/GalleryMainWindow.cpp

    # UI - Widgets - Grids
    src/ui/widgets/grids/IconGridWidget.cpp
    src/ui/widgets/grids/IconThumbnailGridWidget.cpp

    # UI - Widgets - Search
    src/ui/widgets/search/SearchWidget.cpp
    src/ui/widgets/search/IconSearchWidget.cpp
    src/ui/widgets/search/CategoryFilterWidget.cpp

    # UI - Widgets - Panels
    src/ui/widgets/panels/CategorySidebarWidget.cpp
    src/ui/widgets/panels/IconDetailsPanel.cpp
    src/ui/widgets/panels/FileBrowserWidget.cpp

    # UI - Widgets - Viewers
    src/ui/widgets/viewers/ImageViewerWidget.cpp

    # UI - Dialogs
    src/ui/dialogs/IconExportDialog.cpp
    src/ui/dialogs/ExportDialog.cpp
    src/ui/dialogs/ImportDialog.cpp
    src/ui/dialogs/PreferencesDialog.cpp
)

# Gallery header files (for MOC)
set(GALLERY_HEADERS
    # Config
    src/config/LayoutConfig.h

    # Core - Types
    src/core/GalleryTypes.h

    # Core - Utils
    src/core/utils/GalleryLogger.h
    src/core/utils/ErrorHandler.h

    # Core - Managers
    src/core/managers/IconMetadataManager.h
    src/core/managers/FavoritesManager.h
    src/core/managers/ContentManager.h
    src/core/managers/ManagerStubs.h

    # Core - Models
    src/core/models/IconItem.h

    # Core - Export
    src/core/BatchExportManager.h
    src/core/ExportProgressWidget.h
    # Note: BatchExportWorker.h is header-only and included via BatchExportManager.cpp
    # Do not add it here to avoid MOC duplicate definition errors

    # UI - Themes
    src/ui/themes/ThemeManager.h

    # UI - Layouts
    src/ui/layouts/ResponsiveLayoutManager.h

    # UI - Controllers
    src/ui/controllers/SearchController.h
    src/ui/controllers/PerformanceMonitor.h

    # UI - Windows
    src/ui/windows/GalleryMainWindow.h

    # UI - Widgets - Grids
    src/ui/widgets/grids/IconGridWidget.h
    src/ui/widgets/grids/IconThumbnailGridWidget.h

    # UI - Widgets - Search
    src/ui/widgets/search/SearchWidget.h
    src/ui/widgets/search/IconSearchWidget.h
    src/ui/widgets/search/CategoryFilterWidget.h

    # UI - Widgets - Panels
    src/ui/widgets/panels/CategorySidebarWidget.h
    src/ui/widgets/panels/IconDetailsPanel.h
    src/ui/widgets/panels/FileBrowserWidget.h

    # UI - Widgets - Viewers
    src/ui/widgets/viewers/ImageViewerWidget.h

    # UI - Dialogs
    src/ui/dialogs/IconExportDialog.h
    src/ui/dialogs/ExportDialog.h
    src/ui/dialogs/ImportDialog.h
    src/ui/dialogs/PreferencesDialog.h
)

# Resource files
set(GALLERY_RESOURCES
    resources/gallery_resources.qrc
)

# Include directories
set(GALLERY_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/themes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/layouts
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/controllers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/grids
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/search
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/widgets/viewers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/windows
)

# Create executable
add_executable(QtLucideGallery
    ${GALLERY_SOURCES}
    ${GALLERY_HEADERS}
    ${GALLERY_RESOURCES}
)

# Include directories
target_include_directories(QtLucideGallery PRIVATE
    ${GALLERY_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
)

# Link libraries
target_link_libraries(QtLucideGallery PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Concurrent
    Qt6::Network
    QtLucide
)

# Set properties
set_target_properties(QtLucideGallery PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    OUTPUT_NAME "QtLucide Gallery"
)

# Install
install(TARGETS QtLucideGallery
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
