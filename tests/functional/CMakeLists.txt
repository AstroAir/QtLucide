# QtLucide Functional Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Functional test sources
set(FUNCTIONAL_TEST_SOURCES
    test_export_functionality.cpp
    test_image_gallery_functionality.cpp
    test_accessibility.cpp
)

# Functional test headers
set(FUNCTIONAL_TEST_HEADERS
    test_export_functionality.h
    test_accessibility.h
)

# Create functional test executable (disabled due to gallery dependencies)
# add_executable(QtLucideFunctionalTests 
#     ../main.cpp
#     ${FUNCTIONAL_TEST_SOURCES} 
#     ${FUNCTIONAL_TEST_HEADERS}
# )

# Add include directories
# target_include_directories(QtLucideFunctionalTests PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/../include
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/core
#     ${CMAKE_CURRENT_SOURCE_DIR}/../examples/gallery/src/ui
# )

# target_link_libraries(QtLucideFunctionalTests
#     PRIVATE
#         QtLucide
#         Qt6::Core
#         Qt6::Gui
#         Qt6::Widgets
#         Qt6::Test
# )

# Add console subsystem flags for Windows
# if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#     target_link_options(QtLucideFunctionalTests PRIVATE -mconsole)
# endif()

# Add tests to CTest
# add_test(NAME QtLucideFunctionalTests COMMAND QtLucideFunctionalTests)

message(STATUS "QtLucide Functional Tests configured (disabled due to gallery dependencies)")
message(STATUS "  - Functional Tests: ${CMAKE_CURRENT_SOURCE_DIR}/")
