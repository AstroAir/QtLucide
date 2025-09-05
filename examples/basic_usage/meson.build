# Basic Usage Example

# Source files
example_sources = [
  'main.cpp',
  'MainWindow.cpp'
]

# Header files that need MOC processing
example_headers = [
  'MainWindow.h'
]

# Process MOC files
example_moc = qt6.compile_moc(
  headers: example_headers,
  dependencies: qt6_dep
)

# Create executable
qtlucide_example = executable('QtLucideExample',
  example_sources,
  example_headers,
  example_moc,
  dependencies: [qt6_dep, qtlucide_dep],
  gui_app: true,
  install: false
)
