# QtLucide Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Test sources
set(TEST_SOURCES
    main.cpp
    test_qtlucide.cpp
    test_icon_loading.cpp
    test_svg_rendering.cpp
    test_gallery_icon_loading.cpp
    test_error_handling.cpp
    test_thread_safety.cpp
    test_boundary_conditions.cpp
    test_memory_management.cpp
    test_integration.cpp
    test_ui_components.cpp
    test_performance_regression.cpp
    test_platform_specific.cpp
    test_export_functionality.cpp
    test_accessibility.cpp
    ../examples/gallery/src/core/utils/GalleryLogger.cpp
    ../examples/gallery/src/core/managers/IconMetadataManager.cpp
    ../examples/gallery/src/core/managers/ManagerStubs.cpp
    ../examples/gallery/src/core/models/IconItem.cpp
    ../examples/gallery/src/ui/widgets/search/SearchWidget.cpp
    ../examples/gallery/src/ui/widgets/search/CategoryFilterWidget.cpp
    ../examples/gallery/src/ui/widgets/grids/IconGridWidget.cpp
    ../examples/gallery/src/ui/widgets/panels/IconDetailsPanel.cpp
    ../examples/gallery/src/ui/dialogs/PreferencesDialog.cpp
    ../examples/gallery/src/ui/dialogs/IconExportDialog.cpp
    ../examples/gallery/src/ui/windows/GalleryMainWindow.cpp
)

# Test headers (needed for MOC processing)
set(TEST_HEADERS
    test_qtlucide.h
    test_icon_loading.h
    test_svg_rendering.h
    test_gallery_icon_loading.h
    test_error_handling.h
    test_thread_safety.h
    test_boundary_conditions.h
    test_memory_management.h
    test_integration.h
    test_ui_components.h
    test_performance_regression.h
    test_platform_specific.h
    test_export_functionality.h
    test_accessibility.h
    ../examples/gallery/src/core/managers/IconMetadataManager.h
    ../examples/gallery/src/core/models/IconItem.h
    ../examples/gallery/src/ui/widgets/search/SearchWidget.h
    ../examples/gallery/src/ui/widgets/search/CategoryFilterWidget.h
    ../examples/gallery/src/ui/widgets/grids/IconGridWidget.h
    ../examples/gallery/src/ui/widgets/panels/IconDetailsPanel.h
    ../examples/gallery/src/ui/dialogs/PreferencesDialog.h
    ../examples/gallery/src/ui/dialogs/IconExportDialog.h
    ../examples/gallery/src/ui/windows/GalleryMainWindow.h
)

# Create test executable
add_executable(QtLucideTests ${TEST_SOURCES} ${TEST_HEADERS})

# Add include directories for the reorganized gallery structure
target_include_directories(QtLucideTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/config
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui/widgets/grids
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui/widgets/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui/widgets/search
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui/widgets/viewers
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui/windows
)

target_link_libraries(QtLucideTests
    PRIVATE
        QtLucide
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
)

# Add tests to CTest
add_test(NAME QtLucideTests COMMAND QtLucideTests)

# Set test properties
set_target_properties(QtLucideTests PROPERTIES
    WIN32_EXECUTABLE FALSE
)
