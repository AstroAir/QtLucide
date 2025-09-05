# QtLucide library source files

# Source files
qtlucide_sources = [
  'QtLucide.cpp',
  'QtLucideIconEngine.cpp',
  'QtLucideIconPainter.cpp'
]

# Header files
qtlucide_headers = [
  '../include/QtLucide/QtLucide.h',
  '../include/QtLucide/QtLucideIconEngine.h',
  '../include/QtLucide/QtLucideIconPainter.h',
  '../include/QtLucide/QtLucideEnums.h',
  '../include/QtLucide/QtLucideStrings.h'
]

# Resource generation
resources_dir = meson.current_source_dir() / '../resources/icons'
svg_dir = resources_dir / 'svg'
qrc_file = resources_dir / 'lucide_icons.qrc'
metadata_dir = resources_dir / 'metadata'
build_resources_script = meson.current_source_dir() / '../tools/build_resources_meson.py'

# Find all SVG files for dependency tracking
svg_files_cmd = run_command('python3', '-c',
  'import os; print("\\n".join([f for f in os.listdir("@0@") if f.endswith(".svg")]))'.format(svg_dir),
  check: false)

if svg_files_cmd.returncode() == 0
  svg_file_names = svg_files_cmd.stdout().strip().split('\n')
  svg_files_list = []
  foreach name : svg_file_names
    if name != ''
      svg_files_list += files(svg_dir / name)
    endif
  endforeach
else
  # Fallback - assume some common SVG files exist
  svg_files_list = []
endif

# Custom target to generate resources
# Note: This runs the script which generates files in the source tree
generate_resources = custom_target('generate_qtlucide_resources',
  output: 'resource_generation_done.stamp',
  input: svg_files_list,
  command: [python3, build_resources_script, meson.current_source_dir() / '..', '@OUTPUT@'],
  depends: [],
  build_by_default: true,
  install: false
)

# The QRC file is generated in the source tree, so reference it directly
qrc_source_file = files('../resources/icons/lucide_icons.qrc')

# Compile Qt resources
qtlucide_resources = qt6.compile_resources(
  name: 'qtlucide_resources',
  sources: qrc_source_file,
  depends: generate_resources
)

# Process headers that need MOC
qtlucide_moc = qt6.compile_moc(
  headers: '../include/QtLucide/QtLucide.h',
  include_directories: inc_dir,
  dependencies: qt6_dep
)

# Create the QtLucide library
qtlucide_lib = library('QtLucide',
  qtlucide_sources,
  qtlucide_headers,
  qtlucide_resources,
  qtlucide_moc,
  dependencies: qt6_dep,
  include_directories: inc_dir,
  version: project_version,
  soversion: project_version_major,
  install: true,
  install_dir: get_option('libdir')
)

# Create dependency object for use by other targets
qtlucide_dep = declare_dependency(
  link_with: qtlucide_lib,
  include_directories: inc_dir,
  dependencies: qt6_dep
)
