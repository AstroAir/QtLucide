# QtLucide Gallery Performance Tests
# Performance and benchmark tests for gallery components

# Performance test sources
set(PERFORMANCE_TEST_SOURCES
    test_gallery_performance.cpp
    test_performance.cpp
    test_icon_rendering_performance.cpp

    # Include concrete implementation sources for gallery classes
    ../../../examples/gallery/src/ui/themes/ThemeManager.cpp
    ../../../examples/gallery/src/core/managers/IconMetadataManager.cpp
    ../../../examples/gallery/src/core/managers/ManagerStubs.cpp
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

# Performance test headers
set(PERFORMANCE_TEST_HEADERS
    test_gallery_performance.h
    test_icon_rendering_performance.h

    ../../../examples/gallery/src/ui/themes/ThemeManager.h
    ../../../examples/gallery/src/core/managers/IconMetadataManager.h
    ../../../examples/gallery/src/core/managers/ManagerStubs.h
    ../../../examples/gallery/src/core/managers/ContentManager.h
    ../../../examples/gallery/src/core/managers/FavoritesManager.h
    ../../../examples/gallery/src/core/utils/GalleryLogger.h
    ../../../examples/gallery/src/core/utils/ErrorHandler.h
    ../../../examples/gallery/src/ui/widgets/grids/IconGridWidget.h
    ../../../examples/gallery/src/ui/widgets/grids/IconThumbnailGridWidget.h
    ../../../examples/gallery/src/ui/widgets/search/SearchWidget.h
    ../../../examples/gallery/src/ui/widgets/search/IconSearchWidget.h
    ../../../examples/gallery/src/ui/widgets/search/CategoryFilterWidget.h
    ../../../examples/gallery/src/ui/widgets/panels/CategorySidebarWidget.h
    ../../../examples/gallery/src/ui/widgets/panels/FileBrowserWidget.h
    ../../../examples/gallery/src/ui/widgets/viewers/ImageViewerWidget.h
    ../../../examples/gallery/src/ui/layouts/ResponsiveLayoutManager.h
)

# Create performance test executable
create_gallery_test_executable(
    QtLucideGalleryPerformanceTests
    "${PERFORMANCE_TEST_SOURCES}"
    "${PERFORMANCE_TEST_HEADERS}"
)

# Set test properties
set_tests_properties(QtLucideGalleryPerformanceTests PROPERTIES
    LABELS "gallery;performance"
    TIMEOUT 600  # Performance tests may take longer
)

message(STATUS "Gallery Performance Tests configured:")
message(STATUS "  - Test executable: QtLucideGalleryPerformanceTests")
message(STATUS "  - Test count: 3 test files")
message(STATUS "  - Test files:")
message(STATUS "    * test_gallery_performance.cpp")
message(STATUS "    * test_performance.cpp")
message(STATUS "    * test_icon_rendering_performance.cpp (NEW)")
message(STATUS "  - Run with: ctest -L gallery.performance")

