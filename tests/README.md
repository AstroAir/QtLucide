# QtLucide Test Suite

This document describes the reorganized test structure for the QtLucide project, designed to provide clear categorization and easy navigation of different test types.

## Test Organization

The test suite is organized into logical categories based on testing scope and purpose:

```txt
tests/
├── unit/                           # Individual component/class unit tests
│   ├── core/                      # Core QtLucide functionality tests
│   └── components/                # Individual component tests
├── integration/                   # Cross-component integration tests
├── functional/                    # End-to-end functional tests
├── performance/                   # Performance and benchmark tests
├── platform/                     # Platform-specific tests
└── gallery/                      # Gallery-specific tests
    ├── basic/                    # Basic gallery functionality
    ├── components/               # Gallery component tests
    ├── integration/              # Gallery integration tests
    └── performance/              # Gallery performance tests
```

## Test Categories

### Unit Tests (`tests/unit/`)

**Core Unit Tests** (`tests/unit/core/`)

| Test File | Implementation File | Coverage |
|-----------|---------------------|----------|
| `test_qtlucide.cpp/h` | `src/QtLucide.cpp` | Core API: initialization, icon creation, options, availableIcons |
| `test_icon_engine.cpp/h` | `src/QtLucideIconEngine.cpp` | Icon engine: paint, pixmap, clone, caching, isNull, actualSize |
| `test_icon_painter.cpp/h` | `src/QtLucideIconPainter.cpp` | Painters: SVG processing, color handling, custom painters |
| `test_icon_loading.cpp/h` | `src/QtLucide.cpp` | SVG data loading, resource access, pixmap generation |
| `test_svg_rendering.cpp/h` | `src/QtLucideIconPainter.cpp` | SVG rendering quality, color replacement |
| `test_error_handling.cpp/h` | All core files | Invalid inputs, null parameters, edge cases |
| `test_thread_safety.cpp/h` | All core files | Concurrent access, resource mutex |
| `test_boundary_conditions.cpp/h` | All core files | Extreme values, limits |
| `test_memory_management.cpp/h` | All core files | Allocation, cleanup, leaks |

**Component Unit Tests** (`tests/unit/components/`)

- `test_theme_manager.cpp/h` - Theme management system
- `test_batch_export_manager.cpp/h` - Export management system
- `test_icon_metadata_manager.cpp/h` - Metadata handling
- `test_responsive_layout_manager.cpp/h` - Layout management
- `test_file_browser_widget.cpp/h` - File browser functionality
- `test_export_dialog.cpp/h` - Export dialog component
- `test_import_dialog.cpp/h` - Import dialog component

### Integration Tests (`tests/integration/`)

- `test_integration.cpp/h` - General cross-component integration
- `test_ui_components.cpp/h` - UI component interaction testing

### Functional Tests (`tests/functional/`)

- `test_export_functionality.cpp/h` - End-to-end export workflows
- `test_image_gallery_functionality.cpp` - Gallery functionality testing
- `test_accessibility.cpp/h` - Accessibility compliance validation

### Performance Tests (`tests/performance/`)

- `test_performance_regression.cpp/h` - Performance regression testing

### Platform Tests (`tests/platform/`)

- `test_platform_specific.cpp/h` - Platform-specific functionality

### Gallery Tests (`tests/gallery/`)

Gallery-specific tests maintain their existing well-organized structure with basic, component, integration, and performance categories.

## Building and Running Tests

### Build Configuration

```bash
# Configure with tests enabled
cmake -S . -B build -DQTLUCIDE_BUILD_TESTS=ON

# Build specific test categories
cmake --build build --target QtLucideCoreUnitTests
cmake --build build --target QtLucideGalleryBasicTests
```

### Running Tests

```bash
# Run all tests
ctest --test-dir build

# Run specific test categories
ctest --test-dir build -R QtLucideCoreUnitTests
ctest --test-dir build -R QtLucideGallery

# Run with verbose output
ctest --test-dir build --output-on-failure
```

### Available Test Targets

- `QtLucideCoreUnitTests` - Core functionality unit tests
- `QtLucideGalleryBasicTests` - Basic gallery functionality
- `QtLucideGalleryComponentTests` - Gallery component tests
- `QtLucideGalleryIntegrationTests` - Gallery integration tests

## Test Dependencies

### Currently Enabled

- **Core Unit Tests**: Fully functional, no external dependencies
- **Gallery Tests**: Functional with gallery application dependencies

### Currently Disabled

- **Component Unit Tests**: Require gallery dependencies
- **Integration Tests**: Require gallery dependencies  
- **Functional Tests**: Require gallery dependencies
- **Performance Tests**: Require gallery dependencies
- **Platform Tests**: Require gallery dependencies

## Adding New Tests

### Guidelines

1. **Choose the appropriate category** based on test scope:
   - Unit tests for individual components/classes
   - Integration tests for cross-component functionality
   - Functional tests for end-to-end workflows
   - Performance tests for benchmarking
   - Platform tests for OS-specific features

2. **Follow naming conventions**:
   - Files: `test_[component_name].cpp/h`
   - Classes: `Test[ComponentName]`
   - Methods: `test[FunctionName]()`

3. **Update CMakeLists.txt** in the appropriate directory

4. **Add proper documentation** and test descriptions

## Migration Notes

This reorganized structure was created to improve:

- **Discoverability**: Easy to find specific types of tests
- **Maintainability**: Clear separation of concerns
- **Scalability**: Easy to add new test categories
- **Build efficiency**: Ability to build/run specific test categories

The reorganization maintains backward compatibility while providing a cleaner, more intuitive structure for future development.
