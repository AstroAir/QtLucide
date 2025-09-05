# QtLucide Gallery Application

# Source files
gallery_sources = [
  'main.cpp',
  'GalleryMainWindow.cpp',
  'IconGridWidget.cpp',
  'IconDetailsPanel.cpp',
  'SearchWidget.cpp',
  'CategoryFilterWidget.cpp',
  'IconMetadataManager.cpp',
  'IconItem.cpp'
]

# Header files that need MOC processing
gallery_headers = [
  'GalleryMainWindow.h',
  'IconGridWidget.h',
  'IconDetailsPanel.h',
  'SearchWidget.h',
  'CategoryFilterWidget.h',
  'IconMetadataManager.h',
  'IconItem.h'
]

# Process MOC files
gallery_moc = qt6.compile_moc(
  headers: gallery_headers,
  dependencies: qt6_dep
)

# Create executable
qtlucide_gallery = executable('QtLucideGallery',
  gallery_sources,
  gallery_headers,
  gallery_moc,
  dependencies: [qt6_dep, qtlucide_dep],
  gui_app: true,
  install: true,
  install_dir: get_option('bindir')
)
