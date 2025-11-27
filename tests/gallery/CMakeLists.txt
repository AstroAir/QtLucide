# QtLucide Gallery Test Suite
# Complete test suite for QtLucide Gallery application
# Organized into logical test categories with proper naming conventions

cmake_minimum_required(VERSION 3.16)

# Find required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Test Concurrent Network Svg)

# Set common include directories for all gallery tests
set(GALLERY_COMMON_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/themes
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/layouts
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/search
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/grids
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/windows
)

# Set common gallery files needed for testing (headers and sources)
set(GALLERY_COMMON_FILES
    # Core components
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/themes/ThemeManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/themes/ThemeManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/BatchExportManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/BatchExportManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/ExportProgressWidget.cpp

    # Managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/IconMetadataManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/IconMetadataManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/ContentManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/ContentManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/ImageMetadataManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/ImageMetadataManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/FavoritesManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers/FavoritesManager.cpp

    # Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/utils/GalleryLogger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/utils/GalleryLogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/utils/ErrorHandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/utils/ErrorHandler.cpp

    # UI Layouts
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/layouts/ResponsiveLayoutManager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/layouts/ResponsiveLayoutManager.cpp

    # UI Widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/panels/FileBrowserWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/panels/FileBrowserWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/grids/IconGridWidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/grids/IconGridWidget.cpp

    # Models
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/models/IconItem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/models/IconItem.cpp
)

# Include directories for gallery components
set(GALLERY_COMMON_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/core/models
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/themes
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/grids
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/panels
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/widgets/search
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/dialogs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src/ui/layouts
)



# Set common link libraries for all gallery tests
set(GALLERY_COMMON_LIBRARIES
    QtLucide
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    Qt6::Concurrent
    Qt6::Network
    Qt6::Svg
)

# Function to create a gallery test executable
function(create_gallery_test_executable TARGET_NAME TEST_SOURCES TEST_HEADERS)
    add_executable(${TARGET_NAME}
        ${TEST_SOURCES}
        # Note: Gallery components removed to fix linking issues
        # ${GALLERY_COMMON_FILES}
    )

    target_include_directories(${TARGET_NAME} PRIVATE ${GALLERY_COMMON_INCLUDES})
    target_link_libraries(${TARGET_NAME} PRIVATE ${GALLERY_COMMON_LIBRARIES})

    # Set target properties
    set_target_properties(${TARGET_NAME} PROPERTIES
        WIN32_EXECUTABLE FALSE
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    # Add console subsystem flags for Windows to fix WinMain linking error
    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(${TARGET_NAME} PRIVATE -mconsole)
    endif()

    # Add to CTest
    add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

    # Set test properties
    set_tests_properties(${TARGET_NAME} PROPERTIES
        TIMEOUT 300
        LABELS "gallery"
    )
endfunction()

# Include test category subdirectories
add_subdirectory(basic)
add_subdirectory(components)
add_subdirectory(unit)

# Enable/disable GUI-heavy integration tests (disabled by default; enable explicitly when a GUI is available)
option(QTLUCIDE_GALLERY_ENABLE_INTEGRATION_TESTS "Enable GUI-heavy gallery integration tests" OFF)
if(QTLUCIDE_GALLERY_ENABLE_INTEGRATION_TESTS)
    add_subdirectory(integration)
else()
    message(STATUS "Gallery integration tests disabled by default; enable with -DQTLUCIDE_GALLERY_ENABLE_INTEGRATION_TESTS=ON")
endif()

# Performance tests (can be enabled separately)
option(QTLUCIDE_GALLERY_ENABLE_PERFORMANCE_TESTS "Enable gallery performance tests" OFF)
if(QTLUCIDE_GALLERY_ENABLE_PERFORMANCE_TESTS)
    add_subdirectory(performance)
else()
    message(STATUS "Gallery performance tests disabled by default; enable with -DQTLUCIDE_GALLERY_ENABLE_PERFORMANCE_TESTS=ON")
endif()

# Create main gallery test suite that runs all tests
set(GALLERY_TEST_SOURCES
    gallery_test_runner.cpp
)

create_gallery_test_executable(QtLucideGalleryTests "${GALLERY_TEST_SOURCES}" "")

# Smoke test: run the built gallery in headless mode to validate runtime wiring
add_test(NAME QtLucideGallerySmoke COMMAND $<TARGET_FILE:QtLucideGallery> --smoke)
set_tests_properties(QtLucideGallerySmoke PROPERTIES LABELS "gallery;smoke")

# Set test properties for main gallery tests
set_tests_properties(QtLucideGalleryTests PROPERTIES LABELS "gallery;main")

# Set up test labels for organized test execution (after tests are created)
# Note: Test properties will be set in individual CMakeLists.txt files where tests are created
# Note: Additional test labels can be added when more test categories are implemented
# set_tests_properties(QtLucideGalleryIntegrationTests PROPERTIES LABELS "gallery;integration")
# set_tests_properties(QtLucideGalleryEndToEndTests PROPERTIES LABELS "gallery;end_to_end")
# set_tests_properties(QtLucideGalleryPerformanceTests PROPERTIES LABELS "gallery;performance")

# Print test configuration summary
message(STATUS "QtLucide Gallery Test Suite Configuration:")
message(STATUS "  - Basic Tests: QtLucideGalleryBasicTests")
message(STATUS "  - Component Tests: QtLucideGalleryComponentTests")
message(STATUS "  - Unit Tests: QtLucideGalleryUnitTests")
message(STATUS "  - Integration Tests: QtLucideGalleryIntegrationTests (optional)")
message(STATUS "  - Performance Tests: QtLucideGalleryPerformanceTests (optional)")
message(STATUS "  - Main Gallery Tests: QtLucideGalleryTests")
message(STATUS "  - Smoke Test: QtLucideGallerySmoke")
message(STATUS "  - Test Labels: gallery, basic, components, unit, integration, performance, smoke, main")
message(STATUS "  - Run specific category: ctest -L gallery.unit")
message(STATUS "  - Run all gallery tests: ctest -L gallery")
