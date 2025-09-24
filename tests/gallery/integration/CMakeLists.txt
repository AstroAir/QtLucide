# QtLucide Gallery Integration Tests
# Tests for end-to-end gallery functionality and component integration

# Integration test sources
set(INTEGRATION_TEST_SOURCES
    test_gallery_integration.cpp
    test_application_startup.cpp
    integration_test_runner.cpp
    # Include concrete implementation sources for gallery classes
    ../../../examples/gallery/src/ui/themes/ThemeManager.cpp
    ../../../examples/gallery/src/ui/widgets/search/ModernSearchWidget.cpp
    ../../../examples/gallery/src/core/managers/IconMetadataManager.cpp
    ../../../examples/gallery/src/core/BatchExportManager.cpp
    ../../../examples/gallery/src/core/ExportProgressWidget.cpp
    ../../../examples/gallery/src/core/utils/GalleryLogger.cpp
    ../../../examples/gallery/src/core/utils/ErrorHandler.cpp
    ../../../examples/gallery/src/ui/windows/GalleryMainWindow.cpp
)

# Integration test headers
set(INTEGRATION_TEST_HEADERS
    test_gallery_integration.h
    test_application_startup.h
    # Include concrete implementation headers for gallery classes
    ../../../examples/gallery/src/ui/themes/ThemeManager.h
    ../../../examples/gallery/src/ui/widgets/search/ModernSearchWidget.h
    ../../../examples/gallery/src/core/managers/IconMetadataManager.h
    ../../../examples/gallery/src/core/BatchExportManager.h
    ../../../examples/gallery/src/core/utils/GalleryLogger.h
    ../../../examples/gallery/src/core/utils/ErrorHandler.h
)

# Create integration tests executable
create_gallery_test_executable(QtLucideGalleryIntegrationTests "${INTEGRATION_TEST_SOURCES}" "${INTEGRATION_TEST_HEADERS}")

# Set test properties for integration tests
set_tests_properties(QtLucideGalleryIntegrationTests PROPERTIES LABELS "gallery;integration")

message(STATUS "QtLucide Gallery Integration Tests configured")
