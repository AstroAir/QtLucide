# Changelog

All notable changes to QtLucide will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- GitHub Actions CI/CD pipeline for automated testing
- Contributing guidelines and development documentation
- Code of conduct for community participation
- Issue and pull request templates
- Code formatting configuration (.clang-format, .editorconfig)
- Custom icon painter support with `give()` method for registering custom painters
- Custom painter name tracking in icon options for debugging

### Changed

- Improved project structure and documentation
- Enhanced `QtLucideIconEngine::isNull()` to properly handle custom painters
- Custom painters no longer require an `iconId` to be considered valid

### Fixed

- Fixed custom painter icons incorrectly returning `isNull() == true`
- Fixed thread safety tests failing due to custom painter null check issue
- Fixed boundary condition tests for custom painters
- Fixed memory management tests for custom painter lifecycle
- Fixed UI responsiveness tests to properly skip when GalleryMainWindow is unavailable
- Various minor improvements and bug fixes

## [1.0.0] - 2025-01-XX

### Added

- Initial release of QtLucide
- Complete Lucide icon set (1634+ icons) with SVG-based rendering
- QtAwesome-compatible API for easy migration
- Support for Qt6 with modern CMake build system
- Icon customization options (color, scale, state-specific colors)
- Cross-platform support (Windows, macOS, Linux)
- High-DPI display support with crisp vector rendering
- Efficient icon caching and rendering system
- Comprehensive test suite with unit and integration tests
- Example applications demonstrating usage
- CMake package configuration for easy integration

### Features

- **Icon Management**: Access to 1634+ Lucide icons by name or enum
- **Color Customization**: Runtime color changes with support for different states
- **Scalable Rendering**: Perfect quality at any size using SVG
- **Performance**: Efficient caching system for optimal performance
- **Qt Integration**: Native Qt icon engine for seamless integration
- **Theme Support**: Automatic adaptation to system themes
- **Developer Tools**: Python scripts for resource generation and management

### API Highlights

- `QtLucide::icon(const QString& name)` - Create icon by name
- `QtLucide::icon(Icons iconId)` - Create icon by enum value
- `QtLucide::setDefaultOption()` - Set global default options
- `QtLucide::availableIcons()` - Get list of available icons
- Support for color options: `color`, `color-disabled`, `color-active`, `color-selected`
- Scale factor support with `scale-factor` option (0.1 to 2.0)

### Build System

- Modern CMake 3.16+ with Qt6 integration
- Optional examples and tests building
- Proper installation and packaging support
- Cross-platform build scripts and tools
- Python-based resource generation pipeline

### Documentation

- Comprehensive README with quick start guide
- API reference documentation
- Migration guide from QtAwesome
- Build and installation instructions
- Example code and usage patterns

### Testing

- Unit tests for core functionality
- Integration tests for Qt integration
- SVG rendering tests
- Cross-platform compatibility tests
- Performance benchmarks

---

## Version History Format

### [Version] - Date

#### Added

- New features and capabilities

#### Changed

- Changes in existing functionality

#### Deprecated

- Soon-to-be removed features

#### Removed

- Now removed features

#### Fixed

- Bug fixes

#### Security

- Security vulnerability fixes

---

## Migration Notes

### From QtAwesome

QtLucide provides a compatible API for easy migration from QtAwesome:

```cpp
// QtAwesome (old)
fa::QtAwesome* awesome = new fa::QtAwesome(this);
awesome->initFontAwesome();
QIcon icon = awesome->icon(fa::fa_solid, fa::fa_heart);

// QtLucide (new)
lucide::QtLucide* lucide = new lucide::QtLucide(this);
lucide->initLucide();
QIcon icon = lucide->icon("heart");
```

### Breaking Changes

None in this initial release.

---

## Development

### Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for development setup and contribution guidelines.

### Release Process

1. Update version numbers in CMakeLists.txt
2. Update CHANGELOG.md with release notes
3. Create and test release build
4. Tag release with `git tag v1.0.0`
5. Push tags and create GitHub release
6. Update package managers and documentation

### Versioning

- **Major** (X.0.0): Breaking API changes
- **Minor** (1.X.0): New features, backward compatible
- **Patch** (1.0.X): Bug fixes, backward compatible

---

## Links

- [Repository](https://github.com/AstroAir/QtLucide)
- [Issues](https://github.com/AstroAir/QtLucide/issues)
- [Releases](https://github.com/AstroAir/QtLucide/releases)
- [Lucide Icons](https://lucide.dev/)
- [Qt Framework](https://www.qt.io/)
