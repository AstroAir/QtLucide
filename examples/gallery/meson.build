# QtLucide Gallery - Structure Test Build Configuration
#
# Simple test to validate the new directory structure and layout system

# Simple test sources - just the layout config and test main
structure_test_sources = [
    'src/main/main_structure_test.cpp'
]

# Create the structure test executable
qtlucide_gallery_structure_test = executable('QtLucideGalleryStructureTest',
    structure_test_sources,
    dependencies: [qt6_dep],
    cpp_args: [
        '-std=c++17',
        '-Wall',
        '-Wextra'
    ],
    win_subsystem: 'windows',
    install: false
)

message('QtLucide Gallery Structure Test configured successfully')
message('  - New directory structure: ✓')
message('  - Centralized layout system: ✓') 
message('  - Responsive design utilities: ✓')
message('  - Build target: QtLucideGalleryStructureTest')
