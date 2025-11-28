# QtLucide Tests

# Find Qt6 Test module
qt6_test_dep = dependency('qt6', modules: ['Test'], required: true)

# Core unit test sources
core_test_sources = [
  'main.cpp',
  'unit/core/test_qtlucide.cpp',
  'unit/core/test_icon_engine.cpp',
  'unit/core/test_icon_painter.cpp',
  'unit/core/test_icon_loading.cpp',
  'unit/core/test_svg_rendering.cpp',
  'unit/core/test_error_handling.cpp',
  'unit/core/test_thread_safety.cpp',
  'unit/core/test_boundary_conditions.cpp',
  'unit/core/test_memory_management.cpp',
]

# Core unit test headers
core_test_headers = [
  'unit/core/test_qtlucide.h',
  'unit/core/test_icon_engine.h',
  'unit/core/test_icon_painter.h',
  'unit/core/test_icon_loading.h',
  'unit/core/test_svg_rendering.h',
  'unit/core/test_error_handling.h',
  'unit/core/test_thread_safety.h',
  'unit/core/test_boundary_conditions.h',
  'unit/core/test_memory_management.h',
]

# Process MOC files for core tests
core_test_moc = qt6.compile_moc(
  headers: core_test_headers,
  dependencies: [qt6_dep, qt6_test_dep],
  include_directories: [inc_dir]
)

# Create core test executable
qtlucide_core_tests = executable('QtLucideCoreTests',
  core_test_sources,
  core_test_headers,
  core_test_moc,
  dependencies: [qt6_dep, qt6_test_dep, qtlucide_dep],
  include_directories: [inc_dir],
  win_subsystem: 'console',
  install: false
)

# Register core tests
test('QtLucideCoreTests', qtlucide_core_tests)

# Integration test sources (disabled by default due to gallery dependencies)
# Uncomment when gallery dependencies are resolved
# integration_test_sources = [
#   'main.cpp',
#   'integration/test_integration.cpp',
#   'integration/test_ui_components.cpp',
# ]
# integration_test_headers = [
#   'integration/test_integration.h',
#   'integration/test_ui_components.h',
# ]

# Functional test sources (disabled by default)
# functional_test_sources = [
#   'main.cpp',
#   'functional/test_accessibility.cpp',
#   'functional/test_export_functionality.cpp',
# ]
# functional_test_headers = [
#   'functional/test_accessibility.h',
#   'functional/test_export_functionality.h',
# ]

# Performance test sources (disabled by default)
# performance_test_sources = [
#   'main.cpp',
#   'performance/test_performance_regression.cpp',
# ]
# performance_test_headers = [
#   'performance/test_performance_regression.h',
# ]

# Platform test sources (disabled by default)
# platform_test_sources = [
#   'main.cpp',
#   'platform/test_platform_specific.cpp',
# ]
# platform_test_headers = [
#   'platform/test_platform_specific.h',
# ]

# Summary message
message('QtLucide Test Suite configured:')
message('  - Core Unit Tests: enabled')
message('  - Integration Tests: disabled (gallery dependencies)')
message('  - Functional Tests: disabled (gallery dependencies)')
message('  - Performance Tests: disabled (gallery dependencies)')
message('  - Platform Tests: disabled (gallery dependencies)')
