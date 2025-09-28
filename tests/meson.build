# QtLucide Tests

# Find Qt6 Test module
qt6_test_dep = dependency('qt6', modules: ['Test'], required: true)

# Test sources (using actual test files that exist)
test_sources = [
  'main.cpp',
  'unit/core/test_qtlucide.cpp',
  'unit/core/test_icon_loading.cpp',
  'unit/core/test_svg_rendering.cpp',
  'unit/core/test_error_handling.cpp',
  'unit/core/test_thread_safety.cpp',
  'unit/core/test_boundary_conditions.cpp',
  'unit/core/test_memory_management.cpp',
  'integration/test_integration.cpp',
  'integration/test_ui_components.cpp',
  'performance/test_performance_regression.cpp',
  'platform/test_platform_specific.cpp',
  'functional/test_accessibility.cpp',
  'functional/test_export_functionality.cpp',
]

# Test headers that need MOC processing (using actual headers that exist)
test_headers = [
  'unit/core/test_qtlucide.h',
  'unit/core/test_icon_loading.h',
  'unit/core/test_svg_rendering.h',
  'unit/core/test_error_handling.h',
  'unit/core/test_thread_safety.h',
  'unit/core/test_boundary_conditions.h',
  'unit/core/test_memory_management.h',
  'integration/test_integration.h',
  'integration/test_ui_components.h',
  'performance/test_performance_regression.h',
  'platform/test_platform_specific.h',
  'functional/test_accessibility.h',
  'functional/test_export_functionality.h',
]

# Process MOC files
test_moc = qt6.compile_moc(
  headers: test_headers,
  dependencies: [qt6_dep, qt6_test_dep],
  include_directories: [inc_dir]
)

# Create test executable
qtlucide_tests = executable('QtLucideTests',
  test_sources,
  test_headers,
  test_moc,
  dependencies: [qt6_dep, qt6_test_dep, qtlucide_dep],
  include_directories: [inc_dir],
  win_subsystem: 'console',
  install: false
)

# Register test
test('QtLucideTests', qtlucide_tests)
