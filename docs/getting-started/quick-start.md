# Quick Start Guide

Get QtLucide working in your Qt application in under 5 minutes!

## Step 1: Create a New Qt Project

=== "CMake"

    Create a new directory and add these files:

    **CMakeLists.txt**
    ```cmake
    cmake_minimum_required(VERSION 3.16)
    project(QtLucideQuickStart)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    find_package(QtLucide REQUIRED)

    qt6_standard_project_setup()

    qt6_add_executable(QtLucideQuickStart main.cpp)
    target_link_libraries(QtLucideQuickStart PRIVATE
        Qt6::Core
        Qt6::Widgets
        QtLucide::QtLucide
    )
    ```

=== "qmake"

    **quickstart.pro**
    ```pro
    QT += core widgets
    CONFIG += c++17

    TARGET = QtLucideQuickStart
    TEMPLATE = app

    SOURCES += main.cpp

    # Include QtLucide
    include(path/to/QtLucide/QtLucide.pri)
    ```

## Step 2: Write Your First QtLucide Application

Create **main.cpp**:

```cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QtLucide/QtLucide.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create main widget
    QWidget window;
    window.setWindowTitle("QtLucide Quick Start");
    window.resize(400, 300);

    // Initialize QtLucide
    lucide::QtLucide lucide;
    if (!lucide.initLucide()) {
        qDebug() << "Failed to initialize QtLucide!";
        return 1;
    }

    // Create layout
    QVBoxLayout* mainLayout = new QVBoxLayout(&window);

    // Add title
    QLabel* title = new QLabel("QtLucide Quick Start Demo");
    title->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // Create buttons with icons
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    // Save button
    QPushButton* saveBtn = new QPushButton("Save");
    saveBtn->setIcon(lucide.icon("save"));
    buttonLayout->addWidget(saveBtn);

    // Open button
    QPushButton* openBtn = new QPushButton("Open");
    openBtn->setIcon(lucide.icon("folder-open"));
    buttonLayout->addWidget(openBtn);

    // Settings button with custom color
    QPushButton* settingsBtn = new QPushButton("Settings");
    QVariantMap options;
    options["color"] = QColor(Qt::blue);
    settingsBtn->setIcon(lucide.icon("settings", options));
    buttonLayout->addWidget(settingsBtn);

    mainLayout->addLayout(buttonLayout);

    // Add some colorful icons
    QHBoxLayout* colorLayout = new QHBoxLayout();

    // Red heart
    QPushButton* heartBtn = new QPushButton("Like");
    QVariantMap redOptions;
    redOptions["color"] = QColor(Qt::red);
    heartBtn->setIcon(lucide.icon("heart", redOptions));
    colorLayout->addWidget(heartBtn);

    // Green check
    QPushButton* checkBtn = new QPushButton("Done");
    QVariantMap greenOptions;
    greenOptions["color"] = QColor(Qt::green);
    checkBtn->setIcon(lucide.icon("check-circle", greenOptions));
    colorLayout->addWidget(checkBtn);

    // Orange alert
    QPushButton* alertBtn = new QPushButton("Warning");
    QVariantMap orangeOptions;
    orangeOptions["color"] = QColor(255, 165, 0); // Orange
    alertBtn->setIcon(lucide.icon("alert-triangle", orangeOptions));
    colorLayout->addWidget(alertBtn);

    mainLayout->addLayout(colorLayout);

    // Add info label
    QLabel* info = new QLabel(QString("QtLucide initialized with %1 icons!")
                             .arg(lucide.availableIcons().size()));
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("color: gray; margin: 10px;");
    mainLayout->addWidget(info);

    // Show window
    window.show();

    return app.exec();
}
```

## Step 3: Build and Run

=== "CMake"

    ```bash
    mkdir build && cd build
    cmake ..
    cmake --build .
    ./QtLucideQuickStart  # or QtLucideQuickStart.exe on Windows
    ```

=== "qmake"

    ```bash
    qmake
    make  # or nmake on Windows
    ./QtLucideQuickStart  # or QtLucideQuickStart.exe on Windows
    ```

## What You Should See

Your application should display a window with:

- A title "QtLucide Quick Start Demo"
- Three buttons in the first row: Save, Open, Settings (with blue icon)
- Three buttons in the second row: Like (red heart), Done (green check), Warning (orange triangle)
- An info label showing the number of available icons

## Understanding the Code

Let's break down what's happening:

### 1. Initialize QtLucide

```cpp
lucide::QtLucide lucide;
if (!lucide.initLucide()) {
    qDebug() << "Failed to initialize QtLucide!";
    return 1;
}
```

This creates a QtLucide instance and initializes the icon system.

### 2. Create Basic Icons

```cpp
saveBtn->setIcon(lucide.icon("save"));
```

Creates an icon using the icon name. QtLucide will use default colors and settings.

### 3. Customize Icon Colors

```cpp
QVariantMap options;
options["color"] = QColor(Qt::red);
heartBtn->setIcon(lucide.icon("heart", options));
```

Creates a red heart icon by passing custom options.

### 4. Get Icon Information

```cpp
lucide.availableIcons().size()
```

Returns the total number of available icons (should be 1,634+).

## Next Steps

Now that you have QtLucide working, explore more features:

### Try Different Icons

Replace icon names with any of these popular ones:

```cpp
// Navigation
lucide.icon("home")
lucide.icon("arrow-left")
lucide.icon("arrow-right")
lucide.icon("menu")

// Actions
lucide.icon("edit")
lucide.icon("trash-2")
lucide.icon("copy")
lucide.icon("download")

// Status
lucide.icon("check")
lucide.icon("x")
lucide.icon("info")
lucide.icon("alert-circle")

// Media
lucide.icon("play")
lucide.icon("pause")
lucide.icon("stop")
lucide.icon("volume-2")
```

### Experiment with Options

Try these customization options:

```cpp
QVariantMap options;
options["color"] = QColor(255, 100, 50);        // Custom RGB color
options["scale-factor"] = 1.5;                  // Make icon 50% larger
options["color-disabled"] = QColor(Qt::gray);   // Color when disabled
```

### Add More UI Elements

```cpp
// Add to menus
QMenu* fileMenu = menuBar()->addMenu("File");
QAction* newAction = fileMenu->addAction(lucide.icon("file-plus"), "New");

// Add to toolbars
QToolBar* toolbar = addToolBar("Main");
toolbar->addAction(lucide.icon("save"), "Save");

// Add to status bar
QLabel* statusIcon = new QLabel();
statusIcon->setPixmap(lucide.icon("wifi").pixmap(16, 16));
statusBar()->addWidget(statusIcon);
```

## Common Issues

**Icons not showing?**

- Make sure `initLucide()` returns `true`
- Check that Qt SVG module is available
- Verify QtLucide is properly linked

**Wrong colors?**

- Colors are applied to the SVG stroke and fill
- Some icons may not respond to color changes as expected
- Try different color values or check the original SVG

**Performance concerns?**

- QtLucide automatically caches rendered icons
- First render may be slower, subsequent uses are fast
- Consider setting default options to avoid repeated customization

## What's Next?

Continue your QtLucide journey:

- [First Steps Tutorial](first-steps.md) - More detailed walkthrough
- [User Guide](../user-guide/index.md) - Complete feature overview
- [Examples](../examples/index.md) - More complex examples
- [API Reference](../api/index.md) - Detailed API documentation

Congratulations! You now have QtLucide working in your application. 🎉
