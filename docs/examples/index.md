# Examples

Comprehensive examples demonstrating QtLucide features and integration patterns.

## Overview

This section provides practical, working examples that demonstrate how to use QtLucide in real-world scenarios. Each example is complete and can be built and run independently.

<div class="grid cards" markdown>

- :material-play-circle: **Basic Usage**

  ***

  Fundamental examples showing core QtLucide functionality and common patterns.

  [:octicons-arrow-right-24: Basic Usage](basic-usage.md)

- :material-view-gallery: **Icon Gallery**

  ***

  Complete icon browser application showcasing all QtLucide features.

  [:octicons-arrow-right-24: Gallery App](gallery-application.md)

- :material-brush: **Custom Painters**

  ***

  Advanced examples showing how to create and use custom icon painters.

  [:octicons-arrow-right-24: Custom Painters](custom-painters.md)

- :material-puzzle: **Integration Examples**

  ***

  Real-world integration patterns with Qt frameworks and common use cases.

  [:octicons-arrow-right-24: Integration](integration.md)

</div>

## Example Categories

### Getting Started Examples

Perfect for developers new to QtLucide:

- **Hello QtLucide** - Minimal working example
- **Button Icons** - Adding icons to buttons and actions
- **Menu Integration** - Icons in menus and toolbars
- **Color Customization** - Basic color and styling options

### Intermediate Examples

For developers comfortable with Qt basics:

- **Dynamic Icons** - Changing icons at runtime
- **Theme Integration** - Adapting icons to application themes
- **Performance Optimization** - Efficient icon usage patterns
- **State Management** - Icons that respond to application state

### Advanced Examples

For experienced developers exploring advanced features:

- **Custom Painters** - Creating completely custom icon renderers
- **Plugin Systems** - Loading icons from plugins
- **High-DPI Support** - Optimizing for different display densities
- **Memory Management** - Advanced caching and resource management

## Quick Start Examples

### Minimal Example

The simplest possible QtLucide application:

```cpp
#include <QApplication>
#include <QPushButton>
#include <QtLucide/QtLucide.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    lucide::QtLucide lucide;
    lucide.initLucide();

    QPushButton button("Hello QtLucide");
    button.setIcon(lucide.icon("heart"));
    button.show();

    return app.exec();
}
```

### Common Patterns

#### Application Toolbar

```cpp
void MainWindow::setupToolbar()
{
    lucide::QtLucide lucide;
    lucide.initLucide();

    QToolBar* toolbar = addToolBar("Main");

    // File operations
    toolbar->addAction(lucide.icon("file-plus"), "New", this, &MainWindow::newFile);
    toolbar->addAction(lucide.icon("folder-open"), "Open", this, &MainWindow::openFile);
    toolbar->addAction(lucide.icon("save"), "Save", this, &MainWindow::saveFile);

    toolbar->addSeparator();

    // Edit operations
    toolbar->addAction(lucide.icon("copy"), "Copy", this, &MainWindow::copy);
    toolbar->addAction(lucide.icon("scissors"), "Cut", this, &MainWindow::cut);
    toolbar->addAction(lucide.icon("clipboard"), "Paste", this, &MainWindow::paste);
}
```

#### Status Indicators

```cpp
class StatusWidget : public QWidget
{
private:
    lucide::QtLucide m_lucide;
    QLabel* m_statusIcon;
    QLabel* m_statusText;

public:
    StatusWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
        m_lucide.initLucide();

        QHBoxLayout* layout = new QHBoxLayout(this);
        m_statusIcon = new QLabel();
        m_statusText = new QLabel("Ready");

        layout->addWidget(m_statusIcon);
        layout->addWidget(m_statusText);

        setStatus(Status::Ready);
    }

    enum Status { Ready, Working, Error, Success };

    void setStatus(Status status)
    {
        QVariantMap options;
        QString iconName;
        QString text;

        switch (status) {
        case Ready:
            iconName = "circle";
            options["color"] = QColor(Qt::gray);
            text = "Ready";
            break;
        case Working:
            iconName = "loader";
            options["color"] = QColor(Qt::blue);
            text = "Working...";
            break;
        case Error:
            iconName = "x-circle";
            options["color"] = QColor(Qt::red);
            text = "Error";
            break;
        case Success:
            iconName = "check-circle";
            options["color"] = QColor(Qt::green);
            text = "Success";
            break;
        }

        m_statusIcon->setPixmap(m_lucide.icon(iconName, options).pixmap(16, 16));
        m_statusText->setText(text);
    }
};
```

#### Dynamic Icon Updates

```cpp
class PlayButton : public QPushButton
{
    Q_OBJECT

private:
    lucide::QtLucide m_lucide;
    bool m_isPlaying;

public:
    PlayButton(QWidget* parent = nullptr)
        : QPushButton(parent), m_isPlaying(false)
    {
        m_lucide.initLucide();
        updateIcon();

        connect(this, &QPushButton::clicked, this, &PlayButton::toggle);
    }

private slots:
    void toggle()
    {
        m_isPlaying = !m_isPlaying;
        updateIcon();
        emit stateChanged(m_isPlaying);
    }

private:
    void updateIcon()
    {
        QVariantMap options;
        options["color"] = QColor(m_isPlaying ? Qt::red : Qt::green);

        QString iconName = m_isPlaying ? "pause" : "play";
        setIcon(m_lucide.icon(iconName, options));
        setText(m_isPlaying ? "Pause" : "Play");
    }

signals:
    void stateChanged(bool playing);
};
```

## Building the Examples

All examples are included in the QtLucide repository and can be built using CMake:

### Build All Examples

```bash
git clone https://github.com/AstroAir/QtLucide.git
cd QtLucide
mkdir build && cd build
cmake .. -DQTLUCIDE_BUILD_EXAMPLES=ON
cmake --build .
```

### Build Individual Examples

```bash
# Basic usage example
./QtLucideExample

# Icon gallery application
./QtLucide\ Gallery  # or QtLucideGallery.exe on Windows
```

### Example Structure

Each example includes:

- **Source code** - Complete, commented implementation
- **CMakeLists.txt** - Build configuration
- **README.md** - Description and build instructions
- **Screenshots** - Visual demonstration of the example

## Example Projects

### 1. Basic Usage (`examples/basic_usage/`)

Demonstrates fundamental QtLucide usage patterns:

- Icon creation and customization
- Integration with Qt widgets
- Color and scaling options
- Performance best practices

### 2. Icon Gallery (`examples/gallery/`)

A complete icon browser application featuring:

- Browse all 1,634 Lucide icons with responsive grid layout
- Advanced search and category filtering
- Real-time customization preview with color options
- Icon details panel with SVG data and usage examples
- Export icons in SVG and PNG formats
- Copy functionality for icon names and code snippets
- Theme support (dark/light modes)
- Performance optimization techniques for large datasets

### 3. Text Editor (`examples/text_editor/`)

A simple text editor showcasing:

- Menu and toolbar integration
- Document state indicators
- Theme-aware icon colors
- Keyboard shortcuts with icons

### 4. Custom Painter Demo (`examples/custom_painters/`)

Advanced examples showing:

- Creating custom icon painters
- Animated icons
- Data-driven icon rendering
- Integration with custom widgets

## Learning Path

We recommend exploring examples in this order:

1. **Start with Basic Usage** - Learn fundamental concepts
2. **Explore the Gallery** - See all available icons and options
3. **Study Integration Examples** - Learn real-world patterns
4. **Try Custom Painters** - Explore advanced customization

## Contributing Examples

We welcome community contributions! If you have a useful QtLucide example:

1. Fork the repository
2. Add your example to the `examples/` directory
3. Include complete source code and documentation
4. Submit a pull request

### Example Guidelines

- **Complete**: Examples should compile and run without modification
- **Documented**: Include clear comments and README files
- **Focused**: Each example should demonstrate specific concepts
- **Practical**: Show real-world usage patterns

## Getting Help

If you have questions about the examples:

- Check the [FAQ](../faq/index.md) for common issues
- Browse [GitHub Discussions](https://github.com/AstroAir/QtLucide/discussions)
- Open an [issue](https://github.com/AstroAir/QtLucide/issues) for bugs

## What's Next?

After exploring the examples:

- Read the [User Guide](../user-guide/index.md) for comprehensive feature coverage
- Check the [API Reference](../api/index.md) for detailed documentation
- Consider [contributing](../development/contributing.md) your own examples or improvements
