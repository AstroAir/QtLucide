# QtLucide Gallery Basic Tests
# Tests for basic functionality and core library integration

# Basic test sources
set(BASIC_TEST_SOURCES
    test_core_integration.cpp
    test_library_availability.cpp
    test_resource_loading.cpp
    basic_test_runner.cpp
)

# Basic test headers
set(BASIC_TEST_HEADERS
    test_core_integration.h
    test_library_availability.h
    test_resource_loading.h
)

# Create basic tests executable
create_gallery_test_executable(QtLucideGalleryBasicTests "${BASIC_TEST_SOURCES}" "${BASIC_TEST_HEADERS}")

# Set test properties for basic tests
set_tests_properties(QtLucideGalleryBasicTests PROPERTIES LABELS "gallery;basic")

message(STATUS "QtLucide Gallery Basic Tests configured")
