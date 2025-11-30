# QtLucide Unit Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Core unit test sources
set(CORE_UNIT_TEST_SOURCES
    core/test_qtlucide.cpp
    core/test_icon_engine.cpp
    core/test_icon_painter.cpp
    core/test_icon_loading.cpp
    core/test_svg_rendering.cpp
    core/test_error_handling.cpp
    core/test_thread_safety.cpp
    core/test_boundary_conditions.cpp
    core/test_memory_management.cpp)

# Core unit test headers
set(CORE_UNIT_TEST_HEADERS
    core/test_qtlucide.h
    core/test_icon_engine.h
    core/test_icon_painter.h
    core/test_icon_loading.h
    core/test_svg_rendering.h
    core/test_error_handling.h
    core/test_thread_safety.h
    core/test_boundary_conditions.h
    core/test_memory_management.h)

# Component unit test sources
set(COMPONENT_UNIT_TEST_SOURCES
    components/test_theme_manager.cpp
    components/test_modern_search_widget.cpp
    components/test_modern_sidebar_widget.cpp
    components/test_batch_export_manager.cpp
    components/test_icon_metadata_manager.cpp
    components/test_responsive_layout_manager.cpp
    components/test_file_browser_widget.cpp
    components/test_export_dialog.cpp
    components/test_import_dialog.cpp)

# Component unit test headers
set(COMPONENT_UNIT_TEST_HEADERS
    components/test_theme_manager.h
    components/test_modern_search_widget.h
    components/test_modern_sidebar_widget.h
    components/test_batch_export_manager.h
    components/test_icon_metadata_manager.h
    components/test_responsive_layout_manager.h
    components/test_file_browser_widget.h
    components/test_export_dialog.h
    components/test_import_dialog.h)

# Create core unit test executable
add_executable(QtLucideCoreUnitTests ../main.cpp ${CORE_UNIT_TEST_SOURCES}
                                     ${CORE_UNIT_TEST_HEADERS})

# Add include directories
target_include_directories(
  QtLucideCoreUnitTests
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
          ${CMAKE_CURRENT_SOURCE_DIR}/core
          ${CMAKE_CURRENT_SOURCE_DIR}/components)

target_link_libraries(
  QtLucideCoreUnitTests PRIVATE QtLucide Qt6::Core Qt6::Gui Qt6::Widgets
                                Qt6::Svg Qt6::Test)

# Add console subsystem flags for Windows
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_options(QtLucideCoreUnitTests PRIVATE -mconsole)
endif()

# Add tests to CTest
add_test(NAME QtLucideCoreUnitTests COMMAND QtLucideCoreUnitTests)

# Component tests (disabled due to gallery dependencies)
# add_executable(QtLucideComponentUnitTests ../main.cpp
# ${COMPONENT_UNIT_TEST_SOURCES} ${COMPONENT_UNIT_TEST_HEADERS} )
# target_include_directories(QtLucideComponentUnitTests PRIVATE
# ${CMAKE_CURRENT_SOURCE_DIR}/../../include
# ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/gallery/src )
# target_link_libraries(QtLucideComponentUnitTests PRIVATE QtLucide Qt6::Test)
# add_test(NAME QtLucideComponentUnitTests COMMAND QtLucideComponentUnitTests)

message(STATUS "QtLucide Unit Tests configured:")
message(STATUS "  - Core Unit Tests: ${CMAKE_CURRENT_SOURCE_DIR}/core/")
message(
  STATUS
    "  - Component Unit Tests: ${CMAKE_CURRENT_SOURCE_DIR}/components/ (disabled)"
)
