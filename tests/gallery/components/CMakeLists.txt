# QtLucide Gallery Component Tests
# Tests for individual gallery components

# Component test sources (only include existing files)
set(COMPONENT_TEST_SOURCES
    # Core component tests
    core/test_theme_manager.cpp

    # Widget component tests
    widgets/test_modern_search_widget.cpp

    # Main test runner
    component_test_runner.cpp

    # Concrete implementation sources needed for tests
    ../../../examples/gallery/src/ui/themes/ThemeManager.cpp
    ../../../examples/gallery/src/ui/widgets/search/ModernSearchWidget.cpp
    ../../../examples/gallery/src/core/managers/IconMetadataManager.cpp
    ../../../examples/gallery/src/core/BatchExportManager.cpp
    ../../../examples/gallery/src/core/ExportProgressWidget.cpp
    ../../../examples/gallery/src/core/utils/GalleryLogger.cpp
    ../../../examples/gallery/src/core/utils/ErrorHandler.cpp
    ../../../examples/gallery/src/core/GalleryTypes.h

    # Note: Additional component test files can be added as they are implemented
    # core/test_batch_export_manager.cpp
    # core/test_icon_metadata_manager.cpp
    # core/test_content_manager.cpp
    # core/test_favorites_manager.cpp
    # core/test_image_metadata_manager.cpp
    # widgets/test_modern_icon_grid_widget.cpp
    # widgets/test_file_browser_widget.cpp
    # widgets/test_modern_sidebar_widget.cpp
    # widgets/test_icon_grid_widget.cpp
    # dialogs/test_export_dialog.cpp
    # dialogs/test_import_dialog.cpp
    # layouts/test_responsive_layout_manager.cpp
)

# Component test headers (only include existing files)
set(COMPONENT_TEST_HEADERS
    # Core component headers
    core/test_theme_manager.h
    core/test_batch_export_manager.h

    # Widget component headers
    widgets/test_modern_search_widget.h

    # Concrete implementation headers needed for tests
    ../../../examples/gallery/src/ui/themes/ThemeManager.h
    ../../../examples/gallery/src/ui/widgets/search/ModernSearchWidget.h
    ../../../examples/gallery/src/core/managers/IconMetadataManager.h
    ../../../examples/gallery/src/core/BatchExportManager.h
    ../../../examples/gallery/src/core/utils/GalleryLogger.h
    ../../../examples/gallery/src/core/utils/ErrorHandler.h

    # Note: Additional component test headers can be added as they are implemented
    # core/test_icon_metadata_manager.h
    # core/test_content_manager.h
    # core/test_favorites_manager.h
    # core/test_image_metadata_manager.h
    # widgets/test_modern_icon_grid_widget.h
    # widgets/test_file_browser_widget.h
    # widgets/test_modern_sidebar_widget.h
    # widgets/test_icon_grid_widget.h
    # dialogs/test_export_dialog.h
    # dialogs/test_import_dialog.h
    # layouts/test_responsive_layout_manager.h
)

# Create component tests executable
create_gallery_test_executable(QtLucideGalleryComponentTests "${COMPONENT_TEST_SOURCES}" "${COMPONENT_TEST_HEADERS}")

# Set test properties for component tests
set_tests_properties(QtLucideGalleryComponentTests PROPERTIES LABELS "gallery;components")

message(STATUS "QtLucide Gallery Component Tests configured")
