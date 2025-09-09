# QtLucide Tests

# Find Qt6 Test module
qt6_test_dep = dependency('qt6', modules: ['Test'], required: true)

# Test sources
test_sources = [
  'main.cpp',
  'test_qtlucide.cpp',
  'test_icon_loading.cpp',
  'test_svg_rendering.cpp',
  'test_gallery_icon_loading.cpp',
  'test_error_handling.cpp',
  'test_thread_safety.cpp',
  'test_boundary_conditions.cpp',
  'test_memory_management.cpp',
  # Re-enabling tests one by one after fixing
  # 'test_accessibility.cpp',
  # 'test_integration.cpp',
  # 'test_export_functionality.cpp',
  # 'test_platform_specific.cpp', # Disabled: incomplete implementation
  # 'test_performance_regression.cpp',
  # 'test_ui_components.cpp',
  '../examples/gallery/IconMetadataManager.cpp',
  '../examples/gallery/IconItem.cpp'
]

# Test headers that need MOC processing
test_headers = [
  'test_qtlucide.h',
  'test_icon_loading.h',
  'test_svg_rendering.h',
  'test_gallery_icon_loading.h',
  'test_error_handling.h',
  'test_thread_safety.h',
  'test_boundary_conditions.h',
  'test_memory_management.h',
  # Re-enabling tests one by one after fixing
  # 'test_accessibility.h',
  # 'test_integration.h',
  # 'test_export_functionality.h',
  # 'test_platform_specific.h', # Disabled: incomplete implementation
  # 'test_performance_regression.h',
  # 'test_ui_components.h',
  '../examples/gallery/IconMetadataManager.h',
  '../examples/gallery/IconItem.h'
]

# Process MOC files
test_moc = qt6.compile_moc(
  headers: test_headers,
  dependencies: [qt6_dep, qt6_test_dep],
  include_directories: [inc_dir, include_directories('../examples/gallery')]
)

# Create test executable
qtlucide_tests = executable('QtLucideTests',
  test_sources,
  test_headers,
  test_moc,
  dependencies: [qt6_dep, qt6_test_dep, qtlucide_dep],
  include_directories: [inc_dir, include_directories('../examples/gallery')],
  win_subsystem: 'console',
  install: false
)

# Register test
test('QtLucideTests', qtlucide_tests)
