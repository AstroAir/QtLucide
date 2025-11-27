# QtLucide Gallery - Complete Gallery Application
# Comprehensive build configuration for the full QtLucide Gallery

# Gallery source files
gallery_sources = [
    # Configuration
    'src/config/LayoutConfig.h',

    # Core functionality
    'src/core/BatchExportManager.h',
    'src/core/BatchExportManager.cpp',
    'src/core/ExportProgressWidget.cpp',

    # Core managers
    'src/core/managers/ContentManager.h',
    'src/core/managers/ContentManager.cpp',
    'src/core/managers/IconMetadataManager.h',
    'src/core/managers/IconMetadataManager.cpp',
    'src/core/managers/ImageMetadataManager.h',
    'src/core/managers/ImageMetadataManager.cpp',
    'src/core/managers/ManagerStubs.h',
    'src/core/managers/ManagerStubs.cpp',
    'src/core/managers/FavoritesManager.h',
    'src/core/managers/FavoritesManager.cpp',

    # Core models
    'src/core/models/IconItem.h',
    'src/core/models/IconItem.cpp',

    # Core utilities
    'src/core/utils/GalleryLogger.h',
    'src/core/utils/GalleryLogger.cpp',
    'src/core/utils/ErrorHandler.h',
    'src/core/utils/ErrorHandler.cpp',

    # Core types
    'src/core/GalleryTypes.h',

    # UI themes
    'src/ui/themes/ThemeManager.h',
    'src/ui/themes/ThemeManager.cpp',

    # UI layouts
    'src/ui/layouts/ResponsiveLayoutManager.h',
    'src/ui/layouts/ResponsiveLayoutManager.cpp',

    # UI controllers
    'src/ui/controllers/SearchController.h',
    'src/ui/controllers/SearchController.cpp',
    'src/ui/controllers/PerformanceMonitor.h',
    'src/ui/controllers/PerformanceMonitor.cpp',

    # UI widgets - grids
    'src/ui/widgets/grids/IconGridWidget.h',
    'src/ui/widgets/grids/IconGridWidget.cpp',
    'src/ui/widgets/grids/IconThumbnailGridWidget.h',
    'src/ui/widgets/grids/IconThumbnailGridWidget.cpp',

    # UI widgets - panels
    'src/ui/widgets/panels/IconDetailsPanel.h',
    'src/ui/widgets/panels/IconDetailsPanel.cpp',
    'src/ui/widgets/panels/CategorySidebarWidget.h',
    'src/ui/widgets/panels/CategorySidebarWidget.cpp',
    'src/ui/widgets/panels/FileBrowserWidget.h',
    'src/ui/widgets/panels/FileBrowserWidget.cpp',

    # UI widgets - search
    'src/ui/widgets/search/SearchWidget.h',
    'src/ui/widgets/search/SearchWidget.cpp',
    'src/ui/widgets/search/IconSearchWidget.h',
    'src/ui/widgets/search/IconSearchWidget.cpp',
    'src/ui/widgets/search/CategoryFilterWidget.h',
    'src/ui/widgets/search/CategoryFilterWidget.cpp',

    # UI widgets - viewers
    'src/ui/widgets/viewers/ImageViewerWidget.h',
    'src/ui/widgets/viewers/ImageViewerWidget.cpp',

    # UI dialogs
    'src/ui/dialogs/ExportDialog.h',
    'src/ui/dialogs/ExportDialog.cpp',
    'src/ui/dialogs/ImportDialog.h',
    'src/ui/dialogs/ImportDialog.cpp',
    'src/ui/dialogs/IconExportDialog.h',
    'src/ui/dialogs/IconExportDialog.cpp',
    'src/ui/dialogs/PreferencesDialog.h',
    'src/ui/dialogs/PreferencesDialog.cpp',

    # UI windows
    'src/ui/windows/GalleryMainWindow.h',
    'src/ui/windows/GalleryMainWindow.cpp',

    # Main application
    'src/main/main.cpp'
]

# Gallery resource files
gallery_resources = qt6.compile_resources(
    name: 'gallery_resources',
    sources: 'resources/gallery_resources.qrc'
)

# Gallery dependencies - need to include Concurrent and Network modules
qt6_concurrent_dep = dependency('qt6', modules: ['Concurrent'])
qt6_network_dep = dependency('qt6', modules: ['Network'])
gallery_deps = [qt6_dep, qt6_concurrent_dep, qt6_network_dep, qtlucide_dep]

# Add icon validation dependency if available
if is_variable('icon_validation_dep')
    gallery_deps += icon_validation_dep
endif

# Process MOC files for Qt classes
gallery_moc = qt6.compile_moc(
    headers: [
        'src/core/BatchExportManager.h',
        'src/core/managers/ContentManager.h',
        'src/core/managers/IconMetadataManager.h',
        'src/core/managers/ImageMetadataManager.h',
        'src/core/managers/FavoritesManager.h',
        'src/core/models/IconItem.h',
        'src/core/utils/GalleryLogger.h',
        'src/core/utils/ErrorHandler.h',
        'src/ui/themes/ThemeManager.h',
        'src/ui/layouts/ResponsiveLayoutManager.h',
        'src/ui/widgets/grids/IconGridWidget.h',
        'src/ui/widgets/grids/IconThumbnailGridWidget.h',
        'src/ui/widgets/panels/IconDetailsPanel.h',
        'src/ui/widgets/panels/CategorySidebarWidget.h',
        'src/ui/widgets/panels/FileBrowserWidget.h',
        'src/ui/widgets/search/SearchWidget.h',
        'src/ui/widgets/search/IconSearchWidget.h',
        'src/ui/widgets/search/CategoryFilterWidget.h',
        'src/ui/widgets/viewers/ImageViewerWidget.h',
        'src/ui/dialogs/ExportDialog.h',
        'src/ui/dialogs/ImportDialog.h',
        'src/ui/dialogs/IconExportDialog.h',
        'src/ui/dialogs/PreferencesDialog.h',
        'src/ui/windows/GalleryMainWindow.h'
    ],
    dependencies: qt6_dep
)

# Create the gallery executable
qtlucide_gallery = executable('QtLucideGallery',
    gallery_sources,
    gallery_moc,
    gallery_resources,
    dependencies: gallery_deps,
    cpp_args: [
        '-std=c++17',
        '-Wall',
        '-Wextra',
        '-DQTLUCIDE_AVAILABLE'
    ],
    include_directories: [
        'src',
        'src/core',
        'src/core/utils',
        'src/core/managers',
        'src/core/models',
        'src/ui',
        'src/ui/themes',
        'src/ui/controllers',
        'src/ui/widgets',
        'src/ui/widgets/grids',
        'src/ui/widgets/panels',
        'src/ui/widgets/search',
        'src/ui/widgets/viewers',
        'src/ui/dialogs',
        'src/ui/layouts',
        'src/ui/windows',
        'src/main',
        'src/config'
    ],
    win_subsystem: 'windows',
    install: false
)

message('QtLucide Gallery configured successfully')
message('  - Complete gallery application: ✓')
message('  - Icon browsing and search: ✓')
message('  - Image gallery with metadata: ✓')
message('  - Theme switching: ✓')
message('  - Export/import functionality: ✓')
message('  - Responsive layout system: ✓')
message('  - QtLucide integration: ✓')
message('  - Build target: QtLucideGallery')
