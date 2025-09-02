# QtLucide Tests

find_package(Qt6 REQUIRED COMPONENTS Test)

# Test sources
set(TEST_SOURCES
    main.cpp
)

# Create test executable
add_executable(QtLucideTests ${TEST_SOURCES})

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
