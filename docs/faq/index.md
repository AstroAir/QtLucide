# FAQ & Troubleshooting

Frequently asked questions and solutions to common issues with QtLucide.

## Overview

This section provides answers to the most common questions about QtLucide and solutions to typical problems you might encounter.

<div class="grid cards" markdown>

-   :material-help-circle: **Common Issues**

    ---

    Solutions to the most frequently encountered problems and error messages.

    [:octicons-arrow-right-24: Common Issues](common-issues.md)

-   :material-speedometer: **Performance**

    ---

    Performance-related questions and optimization techniques.

    [:octicons-arrow-right-24: Performance FAQ](performance.md)

-   :material-monitor: **Platform Specific**

    ---

    Platform-specific issues and solutions for Windows, macOS, and Linux.

    [:octicons-arrow-right-24: Platform Issues](platform-specific.md)

</div>

## Quick Answers

### General Questions

??? question "What is QtLucide?"

    QtLucide is a Qt library that provides easy access to the Lucide icon set (1,634+ beautiful SVG icons) in Qt applications. It offers a QtAwesome-compatible API with modern SVG-based rendering.

??? question "Is QtLucide free to use?"

    Yes! QtLucide is released under the MIT License, making it free for both commercial and non-commercial use. The Lucide icons themselves are under the ISC License.

??? question "Which Qt versions are supported?"

    QtLucide requires Qt 6.0 or later. It's built and tested with Qt 6.2+ but should work with Qt 6.0+.

??? question "Can I use QtLucide with Qt 5?"

    No, QtLucide is designed for Qt 6 and uses modern Qt 6 features. For Qt 5 projects, consider using QtAwesome or migrating to Qt 6.

??? question "How many icons are available?"

    QtLucide includes 1,634+ icons from the Lucide icon set, covering a wide range of categories including navigation, actions, media, business, and more.

### Installation & Setup

??? question "How do I install QtLucide?"

    The easiest way is using CMake with `find_package(QtLucide REQUIRED)`. You can also build from source or use package managers. See the [Installation Guide](../getting-started/installation.md) for detailed instructions.

??? question "Do I need to install fonts?"

    No! Unlike font-based icon libraries, QtLucide uses SVG rendering, so no font installation is required.

??? question "Can I use QtLucide without CMake?"

    Yes, you can use the provided `QtLucide.pri` file for qmake projects, or manually configure your build system.

### Usage Questions

??? question "How do I create an icon?"

    ```cpp
    lucide::QtLucide lucide;
    lucide.initLucide();
    QIcon icon = lucide.icon("heart");
    ```

??? question "How do I change icon colors?"

    ```cpp
    QVariantMap options;
    options["color"] = QColor(Qt::red);
    QIcon redIcon = lucide.icon("heart", options);
    ```

??? question "Can I use custom icons?"

    Yes! You can create custom painters or register custom SVG data. See the [Custom Painters](../examples/custom-painters.md) example.

??? question "How do I list all available icons?"

    ```cpp
    QStringList icons = lucide.availableIcons();
    qDebug() << "Available icons:" << icons.size();
    ```

### Performance Questions

??? question "Are icons cached automatically?"

    Yes, QtLucide automatically caches rendered icons based on name, size, and options for optimal performance.

??? question "How much memory does QtLucide use?"

    Memory usage depends on how many unique icon/size/option combinations you use. Typically 2-10KB per cached icon.

??? question "Is QtLucide thread-safe?"

    Icon creation is thread-safe after initialization. Configuration methods (`setDefaultOption`, `give`) should be called from the main thread.

### Compatibility Questions

??? question "Can I migrate from QtAwesome?"

    Yes! QtLucide provides a compatible API. See the [Migration Guide](../migration/from-qtawesome.md) for step-by-step instructions.

??? question "Does QtLucide work with Qt Designer?"

    QtLucide icons work with any Qt widget that accepts QIcon, but Qt Designer doesn't have built-in QtLucide support. You'll need to set icons programmatically.

??? question "Can I use QtLucide in QML?"

    QtLucide is designed for Qt Widgets. For QML, you can expose icons through C++ or use the SVG data directly.

## Error Messages

### Common Error Messages and Solutions

??? error "Failed to initialize QtLucide"

    **Cause**: Resource loading failed or Qt SVG module not available.
    
    **Solution**:
    ```cpp
    // Check Qt SVG availability
    #include <QSvgRenderer>
    if (!QSvgRenderer::isSupported()) {
        qWarning() << "Qt SVG support not available";
    }
    
    // Ensure proper linking
    // CMake: target_link_libraries(your_target Qt6::Svg)
    // qmake: QT += svg
    ```

??? error "Unknown icon name: 'icon-name'"

    **Cause**: Icon name doesn't exist in the Lucide set.
    
    **Solution**:
    ```cpp
    // Check available icons
    QStringList available = lucide.availableIcons();
    if (available.contains("my-icon")) {
        QIcon icon = lucide.icon("my-icon");
    } else {
        qDebug() << "Icon not found, using fallback";
        QIcon icon = lucide.icon("circle");  // fallback
    }
    ```

??? error "QIcon is null/empty"

    **Cause**: Icon creation failed due to invalid name or initialization failure.
    
    **Solution**:
    ```cpp
    // Always check initialization
    if (!lucide.initLucide()) {
        qWarning() << "QtLucide initialization failed";
        return;
    }
    
    // Check icon validity
    QIcon icon = lucide.icon("heart");
    if (icon.isNull()) {
        qWarning() << "Failed to create icon";
    }
    ```

??? error "CMake Error: Could not find QtLucide"

    **Cause**: QtLucide not installed or CMake can't find it.
    
    **Solution**:
    ```bash
    # Install QtLucide first
    cmake --install . --prefix /usr/local
    
    # Or specify path
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/qtlucide
    
    # Or use FetchContent
    include(FetchContent)
    FetchContent_Declare(QtLucide
        GIT_REPOSITORY https://github.com/AstroAir/QtLucide.git
    )
    FetchContent_MakeAvailable(QtLucide)
    ```

## Best Practices

### Do's ✅

- **Initialize once**: Call `initLucide()` once at application startup
- **Reuse icons**: Store frequently used icons in variables
- **Use enums**: Prefer `Icons::heart` over `"heart"` for better performance
- **Set defaults**: Use `setDefaultOption()` for consistent styling
- **Check initialization**: Always verify `initLucide()` returns `true`

### Don'ts ❌

- **Don't create repeatedly**: Avoid creating the same icon multiple times
- **Don't ignore errors**: Check return values and handle failures
- **Don't modify after init**: Avoid changing default options after initialization
- **Don't assume names**: Verify icon names exist before using them
- **Don't forget SVG**: Remember to link Qt6::Svg module

## Getting More Help

### Documentation Resources

- [User Guide](../user-guide/index.md) - Comprehensive usage guide
- [API Reference](../api/index.md) - Complete API documentation
- [Examples](../examples/index.md) - Working code examples
- [Migration Guide](../migration/index.md) - Migrating from other libraries

### Community Support

- **GitHub Issues**: [Report bugs](https://github.com/AstroAir/QtLucide/issues)
- **GitHub Discussions**: [Ask questions](https://github.com/AstroAir/QtLucide/discussions)
- **Stack Overflow**: Tag questions with `qtlucide`
- **Qt Forum**: Post in the Qt forum with QtLucide tag

### Reporting Issues

When reporting issues, please include:

1. **QtLucide version**: Check with `git describe --tags`
2. **Qt version**: From `qmake -query QT_VERSION`
3. **Platform**: OS and compiler version
4. **Minimal example**: Code that reproduces the issue
5. **Error messages**: Complete error output
6. **Build configuration**: CMake options used

### Contributing

Found a solution that's not documented? Consider contributing:

- Update this FAQ
- Add examples to the documentation
- Submit bug fixes or improvements
- Share your use cases and solutions

## Quick Reference

### Essential Code Snippets

```cpp
// Basic setup
lucide::QtLucide lucide;
if (!lucide.initLucide()) {
    // Handle error
}

// Create icon with error checking
QIcon createIcon(const QString& name) {
    QIcon icon = lucide.icon(name);
    if (icon.isNull()) {
        qWarning() << "Failed to create icon:" << name;
        return lucide.icon("circle");  // fallback
    }
    return icon;
}

// Custom colors
QVariantMap redOptions;
redOptions["color"] = QColor(Qt::red);
QIcon redIcon = lucide.icon("heart", redOptions);

// Check available icons
QStringList available = lucide.availableIcons();
qDebug() << "Total icons:" << available.size();
```

### Useful Debug Code

```cpp
// Debug QtLucide state
void debugQtLucide(const lucide::QtLucide& lucide) {
    qDebug() << "QtLucide Debug Info:";
    qDebug() << "  Available icons:" << lucide.availableIcons().size();
    qDebug() << "  Total icon count:" << lucide::ICON_COUNT;
    
    // Test basic functionality
    QIcon testIcon = lucide.icon("heart");
    qDebug() << "  Test icon valid:" << !testIcon.isNull();
}
```

---

*Can't find what you're looking for? Check the specific FAQ sections or ask in our [GitHub Discussions](https://github.com/AstroAir/QtLucide/discussions).*
