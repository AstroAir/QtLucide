# QtLucide - Lucide Icons for Qt Applications
# Meson build configuration equivalent to CMakeLists.txt

project('QtLucide', 'cpp',
  version: '1.0.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=2',
    'werror=false'
  ],
  meson_version: '>=0.59.0'
)

# Project information
project_version = meson.project_version()
version_array = project_version.split('.')
project_version_major = version_array[0].to_int()
project_version_minor = version_array[1].to_int()
project_version_patch = version_array[2].to_int()

# Detect if QtLucide is being built as a subproject
is_subproject = meson.is_subproject()

# Build options - default to false when used as subproject
if is_subproject
  default_examples = false
  default_tests = false
else
  default_examples = true
  default_tests = true
endif

build_examples = get_option('examples').disable_auto_if(is_subproject).allowed()
build_tests = get_option('tests').disable_auto_if(is_subproject).allowed()

# Find dependencies
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Svg'], required: true)
python3 = find_program('python3', 'python', required: true)

# Qt6 tools
qt6 = import('qt6')

# Include directories
inc_dir = include_directories('include')

# Add subdirectories
subdir('src')

# Icon validation (compile-time check)
icon_validator = find_program('python3', required: false)
if icon_validator.found()
  icon_check_script = files('tools/check_example_icons.py')

  # Run icon validation as a custom target
  icon_validation = custom_target('validate_icons',
    output: 'icon_validation_done.stamp',
    command: [icon_validator, icon_check_script, meson.current_source_dir(), '@OUTPUT@'],
    build_by_default: build_examples,
    console: true
  )

  # Make examples depend on icon validation
  if build_examples
    # This will be referenced in examples/meson.build
    icon_validation_dep = declare_dependency(sources: icon_validation)
  endif
endif

if build_examples
  subdir('examples')
endif

if build_tests
  subdir('tests')
endif

# Installation (only when not used as subproject or explicitly requested)
build_install = get_option('install')
if not is_subproject and build_install
  install_headers(
    'include/QtLucide/QtLucide.h',
    'include/QtLucide/QtLucideIconEngine.h',
    'include/QtLucide/QtLucideIconPainter.h',
    'include/QtLucide/QtLucideEnums.h',
    'include/QtLucide/QtLucideStrings.h',
    subdir: 'QtLucide'
  )

  # Generate pkg-config file
  pkg = import('pkgconfig')
  pkg.generate(qtlucide_lib,
    name: 'QtLucide',
    description: 'Lucide Icons for Qt Applications',
    version: project_version,
    requires: ['Qt6Core', 'Qt6Gui', 'Qt6Widgets', 'Qt6Svg'],
    subdirs: 'QtLucide'
  )
endif

# When used as subproject, declare dependency for parent project
if is_subproject
  qtlucide_dep = declare_dependency(
    link_with: qtlucide_lib,
    include_directories: inc_dir,
    dependencies: qt6_dep
  )
endif

# Summary
summary({
  'Version': project_version,
  'Examples': build_examples,
  'Tests': build_tests,
  'Install prefix': get_option('prefix'),
}, section: 'Configuration')

summary({
  'Qt6': qt6_dep.found(),
  'Python3': python3.found(),
}, section: 'Dependencies')
