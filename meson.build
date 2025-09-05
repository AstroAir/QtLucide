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
  default_debug = false
else
  default_examples = true
  default_tests = true
  default_debug = false
endif

build_examples = get_option('examples').disable_auto_if(is_subproject).allowed()
build_tests = get_option('tests').disable_auto_if(is_subproject).allowed()
build_debug_executables = get_option('debug_executables')

# Find dependencies
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets', 'Svg'], required: true)
python3 = find_program('python3', 'python', required: true)

# Qt6 tools
qt6 = import('qt6')

# Include directories
inc_dir = include_directories('include')

# Add subdirectories
subdir('src')

if build_examples
  subdir('examples')
endif

if build_tests
  subdir('tests')
endif

# Debug executables (optional)
if build_debug_executables
  # QtLucideGalleryDebug
  qtlucide_gallery_debug = executable('QtLucideGalleryDebug',
    'debug_gallery.cpp',
    'examples/gallery/IconMetadataManager.cpp',
    'examples/gallery/IconGridWidget.cpp',
    'examples/gallery/IconItem.cpp',
    dependencies: [qt6_dep, qtlucide_dep],
    include_directories: [inc_dir, include_directories('examples/gallery')],
    install: false
  )

  # SimpleGalleryTest
  simple_gallery_test = executable('SimpleGalleryTest',
    'test_gallery_simple.cpp',
    dependencies: [qt6_dep, qtlucide_dep],
    include_directories: inc_dir,
    install: false
  )

  # TestMetadataManager
  test_metadata_manager = executable('TestMetadataManager',
    'test_metadata_manager.cpp',
    'examples/gallery/IconMetadataManager.cpp',
    dependencies: [qt6_dep, qtlucide_dep],
    include_directories: [inc_dir, include_directories('examples/gallery')],
    install: false
  )

  # TestResources
  test_resources = executable('TestResources',
    'test_resources.cpp',
    dependencies: [qt6_dep, qtlucide_dep],
    include_directories: inc_dir,
    install: false
  )

  # TestGalleryMinimal
  test_gallery_minimal = executable('TestGalleryMinimal',
    'test_gallery_minimal.cpp',
    'examples/gallery/IconMetadataManager.cpp',
    dependencies: [qt6_dep, qtlucide_dep],
    include_directories: [inc_dir, include_directories('examples/gallery')],
    install: false
  )

  # TestIconNames
  test_icon_names = executable('TestIconNames',
    'test_icon_names.cpp',
    dependencies: [qt6_dep, qtlucide_dep],
    include_directories: inc_dir,
    install: false
  )

  # TestResourcePackaging
  test_resource_packaging = executable('TestResourcePackaging',
    'test_resource_packaging.cpp',
    dependencies: [qt6_dep, qtlucide_dep],
    include_directories: inc_dir,
    install: false
  )
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
  'Debug executables': build_debug_executables,
  'Install prefix': get_option('prefix'),
}, section: 'Configuration')

summary({
  'Qt6': qt6_dep.found(),
  'Python3': python3.found(),
}, section: 'Dependencies')
