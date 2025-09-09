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
example_deps = [qt6_dep, qtlucide_dep]

# Add icon validation dependency if available
if is_variable('icon_validation_dep')
  example_deps += icon_validation_dep
endif

qtlucide_example = executable('QtLucideExample',
  example_sources,
  example_headers,
  example_moc,
  dependencies: example_deps,
  win_subsystem: 'windows',
  install: false
)
