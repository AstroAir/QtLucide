# Contributing to QtLucide

Thank you for your interest in contributing to QtLucide! This document provides guidelines and information for contributors.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Setup](#development-setup)
- [Making Changes](#making-changes)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Submitting Changes](#submitting-changes)
- [Reporting Issues](#reporting-issues)

## Code of Conduct

This project adheres to a [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## Getting Started

1. Fork the repository on GitHub
2. Clone your fork locally
3. Set up the development environment
4. Create a feature branch for your changes
5. Make your changes and test them
6. Submit a pull request

## Development Setup

### Prerequisites

- Qt 6.5.0 or later
- CMake 3.16 or later
- C++17 compatible compiler
- Python 3.x (for build scripts)

### Building from Source

```bash
git clone https://github.com/your-username/QtLucide.git
cd QtLucide
mkdir build && cd build
cmake -DQTLUCIDE_BUILD_EXAMPLES=ON -DQTLUCIDE_BUILD_TESTS=ON ..
cmake --build . --config Release
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Making Changes

### Branch Naming

Use descriptive branch names:

- `feature/add-new-icon-set`
- `bugfix/fix-memory-leak`
- `docs/update-api-documentation`
- `refactor/improve-performance`

### Commit Messages

Follow conventional commit format:

```
type(scope): description

[optional body]

[optional footer]
```

Types:

- `feat`: New features
- `fix`: Bug fixes
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test additions or modifications
- `chore`: Build process or auxiliary tool changes

Examples:

```
feat(icons): add support for custom icon themes
fix(rendering): resolve SVG scaling issues on high-DPI displays
docs(api): update QtLucide class documentation
```

## Coding Standards

### C++ Style Guide

- Follow Qt coding conventions
- Use `.clang-format` configuration provided in the repository
- Use meaningful variable and function names
- Add documentation comments for public APIs

### Code Formatting

Before submitting, ensure your code is properly formatted:

```bash
clang-format -i src/*.cpp include/QtLucide/*.h
```

### Documentation

- Document all public APIs using Doxygen-style comments
- Include usage examples for complex functions
- Update README.md if adding new features

Example documentation:

```cpp
/**
 * @brief Creates an icon with the specified name and options
 * @param name The icon name (e.g., "heart", "activity")
 * @param options Icon rendering options (color, scale, etc.)
 * @return QIcon instance, or null icon if name not found
 *
 * @code
 * QIcon icon = lucide->icon("heart", {{"color", QColor(Qt::red)}});
 * @endcode
 */
QIcon icon(const QString& name, const QVariantMap& options = {});
```

## Testing

### Writing Tests

- Add unit tests for new functionality
- Use Qt Test framework
- Place tests in the `tests/` directory
- Follow existing test patterns

### Test Categories

1. **Unit Tests**: Test individual components
2. **Integration Tests**: Test component interactions
3. **Visual Tests**: Test icon rendering (when applicable)

### Running Specific Tests

```bash
cd build
ctest -R test_name_pattern
```

## Submitting Changes

### Pull Request Process

1. Ensure all tests pass locally
2. Update documentation if needed
3. Add entries to CHANGELOG.md
4. Create a pull request with:
   - Clear title and description
   - Reference related issues
   - Include screenshots for UI changes

### Pull Request Template

Use the provided PR template and fill in all relevant sections.

### Review Process

- All PRs require at least one review
- Address reviewer feedback promptly
- Keep PRs focused and reasonably sized
- Rebase on master before merging

## Reporting Issues

### Bug Reports

Use the bug report template and include:

- Qt version and platform
- Steps to reproduce
- Expected vs actual behavior
- Minimal code example
- Screenshots if applicable

### Feature Requests

Use the feature request template and include:

- Clear description of the feature
- Use cases and benefits
- Proposed API (if applicable)
- Implementation considerations

### Security Issues

For security-related issues, please email the maintainers directly rather than creating a public issue.

## Development Tips

### Icon Resource Management

- Icons are generated from Lucide SVG files
- Use `tools/build_resources.py` to regenerate resources
- Test icon changes with the gallery example

### Performance Considerations

- Profile changes that might affect rendering performance
- Consider memory usage for large icon sets
- Test on different screen densities

### Platform-Specific Testing

- Test on Windows, macOS, and Linux when possible
- Pay attention to high-DPI display behavior
- Consider different Qt styles and themes

## Getting Help

- Check existing issues and discussions
- Ask questions in GitHub Discussions
- Join our community chat (if available)
- Contact maintainers for complex issues

## Recognition

Contributors will be acknowledged in:

- CHANGELOG.md for significant contributions
- README.md contributors section
- Release notes for major features

Thank you for contributing to QtLucide!
