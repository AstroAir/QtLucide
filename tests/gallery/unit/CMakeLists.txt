# QtLucide Gallery Unit Tests
# Unit tests for individual gallery components and functionality

# Common implementation sources needed by all unit tests
set(UNIT_COMMON_SOURCES
    ../../../examples/gallery/src/ui/windows/GalleryMainWindow.cpp
    ../../../examples/gallery/src/ui/themes/ThemeManager.cpp
    ../../../examples/gallery/src/core/managers/IconMetadataManager.cpp
    ../../../examples/gallery/src/core/managers/ImageMetadataManager.cpp
    ../../../examples/gallery/src/core/managers/ContentManager.cpp
    ../../../examples/gallery/src/core/managers/FavoritesManager.cpp
    ../../../examples/gallery/src/core/utils/GalleryLogger.cpp
    ../../../examples/gallery/src/core/utils/ErrorHandler.cpp
    ../../../examples/gallery/src/ui/widgets/grids/IconGridWidget.cpp
    ../../../examples/gallery/src/ui/widgets/grids/IconThumbnailGridWidget.cpp
    ../../../examples/gallery/src/ui/widgets/search/SearchWidget.cpp
    ../../../examples/gallery/src/ui/widgets/search/IconSearchWidget.cpp
    ../../../examples/gallery/src/ui/widgets/search/CategoryFilterWidget.cpp
    ../../../examples/gallery/src/ui/widgets/panels/CategorySidebarWidget.cpp
    ../../../examples/gallery/src/ui/widgets/panels/FileBrowserWidget.cpp
    ../../../examples/gallery/src/ui/widgets/viewers/ImageViewerWidget.cpp
    ../../../examples/gallery/src/ui/layouts/ResponsiveLayoutManager.cpp
)

# Create separate test executable for each test file (each has its own QTEST_MAIN)
set(UNIT_TEST_FILES
    test_gallery_components
    test_image_browsing
    test_search
    test_sidebar
    test_thumbnail_grid
    test_ui_responsiveness
)

foreach(TEST_FILE ${UNIT_TEST_FILES})
    create_gallery_test_executable(
        QtLucideGallery_${TEST_FILE}
        "${TEST_FILE}.cpp;${UNIT_COMMON_SOURCES}"
        ""
    )

    set_tests_properties(QtLucideGallery_${TEST_FILE} PROPERTIES
        LABELS "gallery;unit"
        TIMEOUT 300
    )
endforeach()

message(STATUS "Gallery Unit Tests configured:")
message(STATUS "  - Test executables: 6 separate test executables")
message(STATUS "  - Run with: ctest -L gallery")

