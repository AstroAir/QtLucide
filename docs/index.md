# QtLucide - Lucide Icons for Qt Applications

<div align="center">
  <img src="https://raw.githubusercontent.com/AstroAir/QtLucide/master/resources/logo.png" alt="QtLucide Logo" width="200"/>

  **Beautiful, scalable icons for Qt applications**

  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
  [![Qt Version](https://img.shields.io/badge/Qt-6.0%2B-green.svg)](https://www.qt.io/)
  [![Build Status](https://github.com/AstroAir/QtLucide/workflows/CI/badge.svg)](https://github.com/AstroAir/QtLucide/actions)
  [![Documentation](https://img.shields.io/badge/docs-latest-blue.svg)](https://astroair.github.io/QtLucide/)
  [![Coverage](https://codecov.io/gh/AstroAir/QtLucide/branch/master/graph/badge.svg)](https://codecov.io/gh/AstroAir/QtLucide)
</div>

## Overview

QtLucide is a comprehensive library that brings the beautiful [Lucide icon set](https://lucide.dev/) to Qt applications. With **1,634 carefully crafted icons** from Lucide v0.460.0, QtLucide provides a modern, scalable solution for adding high-quality icons to your Qt projects.

### Key Features

- **🎨 1,634 Beautiful Icons** - Complete Lucide icon set with SVG-based rendering
- **⚡ High Performance** - Efficient caching and optimized rendering pipeline
- **🔧 QtAwesome Compatible** - Drop-in replacement with familiar API
- **📱 Scalable Graphics** - Perfect quality at any size with SVG rendering
- **🎯 Easy Customization** - Runtime color changes, scaling, and theming
- **🏗️ Multiple Build Systems** - CMake, Meson, and XMake support
- **🧪 Comprehensive Testing** - Full test suite with 100% pass rate
- **🔒 Security Focused** - Modern CI/CD with security scanning
- **📊 Performance Monitoring** - Benchmarking and memory profiling
- **🚀 Qt6 Ready** - Modern C++17 with Qt6 integration
- **🌍 Cross Platform** - Works seamlessly on Windows, macOS, and Linux

### Why Choose QtLucide?

=== "Modern Design"

    Lucide icons feature a consistent, modern design language that works perfectly in contemporary applications. Each icon is carefully crafted with attention to detail and visual harmony.

=== "Performance First"

    Built with performance in mind, QtLucide uses efficient caching mechanisms and optimized rendering to ensure your application stays responsive even with hundreds of icons.

=== "Developer Friendly"

    QtLucide provides a clean, intuitive API that's compatible with QtAwesome, making migration easy while offering powerful new features for advanced use cases.

=== "Future Proof"

    Built on Qt6 with modern C++17, QtLucide is designed to grow with your projects and Qt's evolution.

## Quick Example

```cpp
#include <QtLucide/QtLucide.h>

// Initialize QtLucide
lucide::QtLucide lucide;
lucide.initLucide();

// Create icons easily
QPushButton* saveButton = new QPushButton("Save");
saveButton->setIcon(lucide.icon("save"));

// Customize colors and appearance
QVariantMap options;
options["color"] = QColor(Qt::red);
options["scale-factor"] = 1.2;
QPushButton* alertButton = new QPushButton("Alert");
alertButton->setIcon(lucide.icon("alert-circle", options));
```

## What's Included

### Core Components

- **QtLucide Class** - Main interface for icon management and creation
- **QtLucideIconEngine** - High-performance QIconEngine implementation with caching
- **QtLucideIconPainter** - Extensible painter system for custom rendering
- **Icon Enums** - Strongly-typed enum for all 1,634 icons
- **SVG Icon Painter** - Default SVG renderer with color customization

### Build Systems & Tools

- **CMake Support** - Modern CMake with package configuration
- **Meson Support** - Fast, modern build system with pkg-config
- **XMake Support** - Lua-based build system for cross-platform development
- **Resource Generators** - Python scripts for processing icon assets
- **qmake Integration** - Legacy qmake support via .pri file

### Example Applications

- **Basic Usage Example** - Simple demonstration of core features
- **Icon Gallery** - Complete icon browser with search and filtering
- **Advanced Examples** - Custom painters and performance optimization

### Testing & Quality Assurance

- **Comprehensive Test Suite** - Unit tests for all components
- **Performance Benchmarks** - Memory and rendering performance tests
- **Thread Safety Tests** - Multi-threading validation
- **Boundary Condition Tests** - Edge case handling verification
- **CI/CD Pipeline** - Automated testing on multiple platforms

### Documentation & Support

- **Complete API Reference** - Detailed documentation for all classes and methods
- **Migration Guides** - Easy transition from QtAwesome and other icon libraries
- **Examples & Tutorials** - Step-by-step guides for common use cases
- **Performance Guidelines** - Best practices for optimal performance
- **FAQ & Troubleshooting** - Common issues and solutions

## Getting Started

Ready to add beautiful icons to your Qt application? Choose your path:

<div class="grid cards" markdown>

- :material-rocket-launch-outline: **Quick Start**

  ***

  Get up and running in minutes with our quick start guide.

  [:octicons-arrow-right-24: Quick Start](getting-started/quick-start.md)

- :material-download-outline: **Installation**

  ***

  Detailed installation instructions for all platforms and build systems.

  [:octicons-arrow-right-24: Installation](getting-started/installation.md)

- :material-book-open-outline: **User Guide**

  ***

  Comprehensive guide covering all features and capabilities.

  [:octicons-arrow-right-24: User Guide](user-guide/index.md)

- :material-code-braces: **API Reference**

  ***

  Complete API documentation with examples and best practices.

  [:octicons-arrow-right-24: API Reference](api/index.md)

</div>

## Community & Support

- **GitHub Repository**: [AstroAir/QtLucide](https://github.com/AstroAir/QtLucide)
- **Issue Tracker**: [Report bugs and request features](https://github.com/AstroAir/QtLucide/issues)
- **Discussions**: [Community discussions and Q&A](https://github.com/AstroAir/QtLucide/discussions)
- **License**: [MIT License](https://github.com/AstroAir/QtLucide/blob/master/LICENSE)

## Credits

- **Icons**: [Lucide](https://lucide.dev/) - Beautiful open source icons (ISC License)
- **Inspiration**: [QtAwesome](https://github.com/gamecreature/QtAwesome) - The original Qt icon library
- **Framework**: [Qt6](https://www.qt.io/) - The cross-platform application framework

---

_QtLucide is developed and maintained by [Max Qian](https://github.com/AstroAir) and the open source community._
