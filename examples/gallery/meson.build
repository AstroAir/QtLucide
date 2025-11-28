# QtLucide Gallery Example - Meson Build
# Modern Qt6 icon gallery showcasing QtLucide library

# Find additional Qt6 modules needed for gallery
qt6_concurrent_dep = dependency('qt6', modules: ['Concurrent'], required: true)
qt6_network_dep = dependency('qt6', modules: ['Network'], required: true)

# Gallery source files
gallery_sources = [
  # Main
  'src/main/main.cpp',

  # Core - Utils
  'src/core/utils/GalleryLogger.cpp',
  'src/core/utils/ErrorHandler.cpp',

  # Core - Managers
  'src/core/managers/IconMetadataManager.cpp',
  'src/core/managers/FavoritesManager.cpp',
  'src/core/managers/ContentManager.cpp',
  'src/core/managers/ManagerStubs.cpp',

  # Core - Models
  'src/core/models/IconItem.cpp',

  # Core - Export
  'src/core/BatchExportManager.cpp',
  'src/core/ExportProgressWidget.cpp',

  # UI - Themes
  'src/ui/themes/ThemeManager.cpp',

  # UI - Layouts
  'src/ui/layouts/ResponsiveLayoutManager.cpp',

  # UI - Controllers
  'src/ui/controllers/SearchController.cpp',
  'src/ui/controllers/PerformanceMonitor.cpp',

  # UI - Windows
  'src/ui/windows/GalleryMainWindow.cpp',

  # UI - Widgets - Grids
  'src/ui/widgets/grids/IconGridWidget.cpp',
  'src/ui/widgets/grids/IconThumbnailGridWidget.cpp',

  # UI - Widgets - Search
  'src/ui/widgets/search/SearchWidget.cpp',
  'src/ui/widgets/search/IconSearchWidget.cpp',
  'src/ui/widgets/search/CategoryFilterWidget.cpp',

  # UI - Widgets - Panels
  'src/ui/widgets/panels/CategorySidebarWidget.cpp',
  'src/ui/widgets/panels/IconDetailsPanel.cpp',
  'src/ui/widgets/panels/FileBrowserWidget.cpp',

  # UI - Widgets - Viewers
  'src/ui/widgets/viewers/ImageViewerWidget.cpp',

  # UI - Dialogs
  'src/ui/dialogs/IconExportDialog.cpp',
  'src/ui/dialogs/ExportDialog.cpp',
  'src/ui/dialogs/ImportDialog.cpp',
  'src/ui/dialogs/PreferencesDialog.cpp',
]

# Gallery header files (for MOC)
gallery_headers = [
  # Config
  'src/config/LayoutConfig.h',

  # Core - Types
  'src/core/GalleryTypes.h',
  # Note: BatchExportWorker.h is header-only and included via BatchExportManager.cpp
  # Do not add it here to avoid MOC duplicate definition errors

  # Core - Utils
  'src/core/utils/GalleryLogger.h',
  'src/core/utils/ErrorHandler.h',

  # Core - Managers
  'src/core/managers/IconMetadataManager.h',
  'src/core/managers/FavoritesManager.h',
  'src/core/managers/ContentManager.h',
  'src/core/managers/ManagerStubs.h',

  # Core - Models
  'src/core/models/IconItem.h',

  # Core - Export
  'src/core/BatchExportManager.h',
  'src/core/ExportProgressWidget.h',

  # UI - Themes
  'src/ui/themes/ThemeManager.h',

  # UI - Layouts
  'src/ui/layouts/ResponsiveLayoutManager.h',

  # UI - Controllers
  'src/ui/controllers/SearchController.h',
  'src/ui/controllers/PerformanceMonitor.h',

  # UI - Windows
  'src/ui/windows/GalleryMainWindow.h',

  # UI - Widgets - Grids
  'src/ui/widgets/grids/IconGridWidget.h',
  'src/ui/widgets/grids/IconThumbnailGridWidget.h',

  # UI - Widgets - Search
  'src/ui/widgets/search/SearchWidget.h',
  'src/ui/widgets/search/IconSearchWidget.h',
  'src/ui/widgets/search/CategoryFilterWidget.h',

  # UI - Widgets - Panels
  'src/ui/widgets/panels/CategorySidebarWidget.h',
  'src/ui/widgets/panels/IconDetailsPanel.h',
  'src/ui/widgets/panels/FileBrowserWidget.h',

  # UI - Widgets - Viewers
  'src/ui/widgets/viewers/ImageViewerWidget.h',

  # UI - Dialogs
  'src/ui/dialogs/IconExportDialog.h',
  'src/ui/dialogs/ExportDialog.h',
  'src/ui/dialogs/ImportDialog.h',
  'src/ui/dialogs/PreferencesDialog.h',
]

# Include directories
gallery_inc = include_directories(
  'src',
  'src/config',
  'src/core',
  'src/core/utils',
  'src/core/managers',
  'src/core/models',
  'src/ui',
  'src/ui/themes',
  'src/ui/layouts',
  'src/ui/controllers',
  'src/ui/widgets',
  'src/ui/widgets/grids',
  'src/ui/widgets/search',
  'src/ui/widgets/panels',
  'src/ui/widgets/viewers',
  'src/ui/dialogs',
  'src/ui/windows',
)

# Process MOC files for headers
gallery_moc = qt6.compile_moc(
  headers: gallery_headers,
  dependencies: [qt6_dep, qt6_concurrent_dep, qt6_network_dep],
  include_directories: [inc_dir, gallery_inc]
)

# Process MOC files for sources that include .moc files
# BatchExportManager.cpp includes BatchExportManager.moc
gallery_src_moc = qt6.compile_moc(
  sources: ['src/core/BatchExportManager.cpp'],
  dependencies: [qt6_dep, qt6_concurrent_dep],
  include_directories: [inc_dir, gallery_inc]
)

# Compile resources if gallery_resources.qrc exists
gallery_qrc_file = files('resources/gallery_resources.qrc')
gallery_resources = qt6.compile_resources(
  name: 'gallery_resources',
  sources: gallery_qrc_file
)

# Gallery dependencies
gallery_deps = [qt6_dep, qt6_concurrent_dep, qt6_network_dep, qtlucide_dep]

# Add icon validation dependency if available
if is_variable('icon_validation_dep')
  gallery_deps += icon_validation_dep
endif

# Create gallery executable
qtlucide_gallery = executable('QtLucideGallery',
  gallery_sources,
  gallery_headers,
  gallery_moc,
  gallery_src_moc,
  gallery_resources,
  dependencies: gallery_deps,
  include_directories: [inc_dir, gallery_inc],
  win_subsystem: 'windows',
  install: true,
  install_dir: get_option('bindir')
)
