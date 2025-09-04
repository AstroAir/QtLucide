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
    ../examples/gallery/IconMetadataManager.cpp
    ../examples/gallery/IconItem.cpp
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
    ../examples/gallery/IconMetadataManager.h
    ../examples/gallery/IconItem.h
)

# Create test executable
add_executable(QtLucideTests ${TEST_SOURCES} ${TEST_HEADERS})

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
