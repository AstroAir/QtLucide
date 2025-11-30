# QtLucide Platform-Specific Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Platform test sources
set(PLATFORM_TEST_SOURCES test_platform_specific.cpp)

# Platform test headers
set(PLATFORM_TEST_HEADERS test_platform_specific.h)

# Create platform test executable (disabled due to gallery dependencies)
# add_executable(QtLucidePlatformTests ../main.cpp ${PLATFORM_TEST_SOURCES}
# ${PLATFORM_TEST_HEADERS} )

# Add include directories target_include_directories(QtLucidePlatformTests
# PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include
# ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src
# ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core
# ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui )

# target_link_libraries(QtLucidePlatformTests PRIVATE QtLucide Qt6::Core
# Qt6::Gui Qt6::Widgets Qt6::Test )

# Add console subsystem flags for Windows if(WIN32 AND CMAKE_CXX_COMPILER_ID
# STREQUAL "GNU") target_link_options(QtLucidePlatformTests PRIVATE -mconsole)
# endif()

# Add tests to CTest add_test(NAME QtLucidePlatformTests COMMAND
# QtLucidePlatformTests)

message(
  STATUS
    "QtLucide Platform Tests configured (disabled due to gallery dependencies)")
message(STATUS "  - Platform Tests: ${CMAKE_CURRENT_SOURCE_DIR}/")
