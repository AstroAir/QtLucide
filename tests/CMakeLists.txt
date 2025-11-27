# QtLucide Tests - Reorganized Structure

find_package(Qt6 REQUIRED COMPONENTS Test)

# Core unit test sources
set(CORE_UNIT_TEST_SOURCES
    unit/core/test_qtlucide.cpp
    unit/core/test_icon_engine.cpp
    unit/core/test_icon_painter.cpp
    unit/core/test_icon_loading.cpp
    unit/core/test_svg_rendering.cpp
    unit/core/test_error_handling.cpp
    unit/core/test_thread_safety.cpp
    unit/core/test_boundary_conditions.cpp
    unit/core/test_memory_management.cpp
)

# Core unit test headers
set(CORE_UNIT_TEST_HEADERS
    unit/core/test_qtlucide.h
    unit/core/test_icon_engine.h
    unit/core/test_icon_painter.h
    unit/core/test_icon_loading.h
    unit/core/test_svg_rendering.h
    unit/core/test_error_handling.h
    unit/core/test_thread_safety.h
    unit/core/test_boundary_conditions.h
    unit/core/test_memory_management.h
)

# Component unit test sources
set(COMPONENT_UNIT_TEST_SOURCES
    unit/components/test_theme_manager.cpp
    unit/components/test_modern_search_widget.cpp
    unit/components/test_modern_sidebar_widget.cpp
    unit/components/test_batch_export_manager.cpp
    unit/components/test_icon_metadata_manager.cpp
    unit/components/test_responsive_layout_manager.cpp
    unit/components/test_file_browser_widget.cpp
    unit/components/test_export_dialog.cpp
    unit/components/test_import_dialog.cpp
)

# Component unit test headers
set(COMPONENT_UNIT_TEST_HEADERS
    unit/components/test_theme_manager.h
    unit/components/test_batch_export_manager.h
    unit/components/test_icon_metadata_manager.h
    unit/components/test_responsive_layout_manager.h
    unit/components/test_file_browser_widget.h
    unit/components/test_export_dialog.h
    unit/components/test_import_dialog.h
)

# Integration test sources
set(INTEGRATION_TEST_SOURCES
    integration/test_integration.cpp
    integration/test_ui_components.cpp
)

# Integration test headers
set(INTEGRATION_TEST_HEADERS
    integration/test_integration.h
    integration/test_ui_components.h
)

# Functional test sources
set(FUNCTIONAL_TEST_SOURCES
    functional/test_export_functionality.cpp
    functional/test_image_gallery_functionality.cpp
    functional/test_accessibility.cpp
)

# Functional test headers
set(FUNCTIONAL_TEST_HEADERS
    functional/test_export_functionality.h
    functional/test_accessibility.h
)

# Performance test sources
set(PERFORMANCE_TEST_SOURCES
    performance/test_performance_regression.cpp
)

# Performance test headers
set(PERFORMANCE_TEST_HEADERS
    performance/test_performance_regression.h
)

# Platform test sources
set(PLATFORM_TEST_SOURCES
    platform/test_platform_specific.cpp
)

# Platform test headers
set(PLATFORM_TEST_HEADERS
    platform/test_platform_specific.h
)

# Combine all test sources and headers
set(ALL_TEST_SOURCES
    main.cpp
    ${CORE_UNIT_TEST_SOURCES}
    # Component tests disabled due to gallery dependencies
    # ${COMPONENT_UNIT_TEST_SOURCES}
    # ${INTEGRATION_TEST_SOURCES}
    # ${FUNCTIONAL_TEST_SOURCES}
    # ${PERFORMANCE_TEST_SOURCES}
    # ${PLATFORM_TEST_SOURCES}
)

set(ALL_TEST_HEADERS
    ${CORE_UNIT_TEST_HEADERS}
    # Component test headers disabled due to gallery dependencies
    # ${COMPONENT_UNIT_TEST_HEADERS}
    # ${INTEGRATION_TEST_HEADERS}
    # ${FUNCTIONAL_TEST_HEADERS}
    # ${PERFORMANCE_TEST_HEADERS}
    # ${PLATFORM_TEST_HEADERS}
)

# Create core unit test executable (only core tests enabled for now)
add_executable(QtLucideCoreTests ${ALL_TEST_SOURCES} ${ALL_TEST_HEADERS})

# Add include directories
target_include_directories(QtLucideCoreTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/core
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/components
    ${CMAKE_CURRENT_SOURCE_DIR}/integration
    ${CMAKE_CURRENT_SOURCE_DIR}/functional
    ${CMAKE_CURRENT_SOURCE_DIR}/performance
    ${CMAKE_CURRENT_SOURCE_DIR}/platform
)

target_link_libraries(QtLucideCoreTests
    PRIVATE
        QtLucide
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Test
)

# Add tests to CTest
add_test(NAME QtLucideCoreTests COMMAND QtLucideCoreTests)

# Set test properties
set_target_properties(QtLucideCoreTests PROPERTIES
    WIN32_EXECUTABLE FALSE
)

# Add console subsystem flags for Windows to fix WinMain linking error
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(QtLucideCoreTests PRIVATE -mconsole)
endif()

# Create individual test executables for different categories (when dependencies are available)
# TODO: Enable these when gallery dependencies are resolved

# Component Tests
# add_executable(QtLucideComponentTests
#     main.cpp
#     ${COMPONENT_UNIT_TEST_SOURCES}
#     ${COMPONENT_UNIT_TEST_HEADERS}
# )
# target_link_libraries(QtLucideComponentTests PRIVATE QtLucide Qt6::Test)
# add_test(NAME QtLucideComponentTests COMMAND QtLucideComponentTests)

# Integration Tests
# add_executable(QtLucideIntegrationTests
#     main.cpp
#     ${INTEGRATION_TEST_SOURCES}
#     ${INTEGRATION_TEST_HEADERS}
# )
# target_link_libraries(QtLucideIntegrationTests PRIVATE QtLucide Qt6::Test)
# add_test(NAME QtLucideIntegrationTests COMMAND QtLucideIntegrationTests)

# Functional Tests
# add_executable(QtLucideFunctionalTests
#     main.cpp
#     ${FUNCTIONAL_TEST_SOURCES}
#     ${FUNCTIONAL_TEST_HEADERS}
# )
# target_link_libraries(QtLucideFunctionalTests PRIVATE QtLucide Qt6::Test)
# add_test(NAME QtLucideFunctionalTests COMMAND QtLucideFunctionalTests)

# Performance Tests
# add_executable(QtLucidePerformanceTests
#     main.cpp
#     ${PERFORMANCE_TEST_SOURCES}
#     ${PERFORMANCE_TEST_HEADERS}
# )
# target_link_libraries(QtLucidePerformanceTests PRIVATE QtLucide Qt6::Test)
# add_test(NAME QtLucidePerformanceTests COMMAND QtLucidePerformanceTests)

# Platform Tests
# add_executable(QtLucidePlatformTests
#     main.cpp
#     ${PLATFORM_TEST_SOURCES}
#     ${PLATFORM_TEST_HEADERS}
# )
# target_link_libraries(QtLucidePlatformTests PRIVATE QtLucide Qt6::Test)
# add_test(NAME QtLucidePlatformTests COMMAND QtLucidePlatformTests)

# Include test subdirectories
add_subdirectory(unit)
add_subdirectory(integration)
add_subdirectory(functional)
add_subdirectory(performance)
add_subdirectory(platform)

# Include gallery test suite if gallery is built
if(QTLUCIDE_BUILD_EXAMPLES)
    add_subdirectory(gallery)
    message(STATUS "QtLucide Gallery Test Suite enabled")
else()
    message(STATUS "QtLucide Gallery Test Suite disabled (QTLUCIDE_BUILD_EXAMPLES=OFF)")
endif()

# Print test organization summary
message(STATUS "QtLucide Test Suite Organization:")
message(STATUS "  - Core Unit Tests: ${CMAKE_CURRENT_SOURCE_DIR}/unit/core/")
message(STATUS "  - Component Unit Tests: ${CMAKE_CURRENT_SOURCE_DIR}/unit/components/")
message(STATUS "  - Integration Tests: ${CMAKE_CURRENT_SOURCE_DIR}/integration/")
message(STATUS "  - Functional Tests: ${CMAKE_CURRENT_SOURCE_DIR}/functional/")
message(STATUS "  - Performance Tests: ${CMAKE_CURRENT_SOURCE_DIR}/performance/")
message(STATUS "  - Platform Tests: ${CMAKE_CURRENT_SOURCE_DIR}/platform/")
message(STATUS "  - Gallery Tests: ${CMAKE_CURRENT_SOURCE_DIR}/gallery/")
