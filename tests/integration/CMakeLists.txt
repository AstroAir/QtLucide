# QtLucide Integration Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Integration test sources
set(INTEGRATION_TEST_SOURCES
    test_integration.cpp
    test_ui_components.cpp
)

# Integration test headers
set(INTEGRATION_TEST_HEADERS
    test_integration.h
    test_ui_components.h
)

# Create integration test executable (disabled due to gallery dependencies)
# add_executable(QtLucideIntegrationTests 
#     ../main.cpp
#     ${INTEGRATION_TEST_SOURCES} 
#     ${INTEGRATION_TEST_HEADERS}
# )

# Add include directories
# target_include_directories(QtLucideIntegrationTests PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../include
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui
# )

# target_link_libraries(QtLucideIntegrationTests
#     PRIVATE
#         QtLucide
#         Qt6::Core
#         Qt6::Gui
#         Qt6::Widgets
#         Qt6::Test
# )

# Add console subsystem flags for Windows
# if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     target_link_options(QtLucideIntegrationTests PRIVATE -mconsole)
# endif()

# Add tests to CTest
# add_test(NAME QtLucideIntegrationTests COMMAND QtLucideIntegrationTests)

message(STATUS "QtLucide Integration Tests configured (disabled due to gallery dependencies)")
message(STATUS "  - Integration Tests: ${CMAKE_CURRENT_SOURCE_DIR}/")
