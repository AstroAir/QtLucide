# QtLucide Performance Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Performance test sources
set(PERFORMANCE_TEST_SOURCES
    test_performance_regression.cpp
)

# Performance test headers
set(PERFORMANCE_TEST_HEADERS
    test_performance_regression.h
)

# Create performance test executable (disabled due to gallery dependencies)
# add_executable(QtLucidePerformanceTests 
#     ../main.cpp
#     ${PERFORMANCE_TEST_SOURCES} 
#     ${PERFORMANCE_TEST_HEADERS}
# )

# Add include directories
# target_include_directories(QtLucidePerformanceTests PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../include
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui
# )

# target_link_libraries(QtLucidePerformanceTests
#     PRIVATE
#         QtLucide
#         Qt6::Core
#         Qt6::Gui
#         Qt6::Widgets
#         Qt6::Test
# )

# Add console subsystem flags for Windows
# if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     target_link_options(QtLucidePerformanceTests PRIVATE -mconsole)
# endif()

# Add tests to CTest
# add_test(NAME QtLucidePerformanceTests COMMAND QtLucidePerformanceTests)

message(STATUS "QtLucide Performance Tests configured (disabled due to gallery dependencies)")
message(STATUS "  - Performance Tests: ${CMAKE_CURRENT_SOURCE_DIR}/")
