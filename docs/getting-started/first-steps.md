# First Steps Tutorial

This comprehensive tutorial will walk you through creating a complete QtLucide application, covering all the essential concepts and features you need to know.

## What We'll Build

We'll create a simple text editor with a modern icon-based interface featuring:

- Toolbar with file operations (New, Open, Save)
- Edit menu with formatting options
- Status bar with document information
- Custom icon colors and themes

## Prerequisites

- QtLucide installed and working (see [Installation Guide](installation.md))
- Basic Qt knowledge (widgets, layouts, signals/slots)
- C++17 compatible development environment

## Project Setup

### 1. Create Project Structure

```
text-editor/
├── CMakeLists.txt
├── main.cpp
├── MainWindow.h
├── MainWindow.cpp
└── TextEditor.h
```

### 2. CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(QtLucideTextEditor VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(QtLucide REQUIRED)

qt6_standard_project_setup()

qt6_add_executable(QtLucideTextEditor
    main.cpp
    MainWindow.cpp
    MainWindow.h
    TextEditor.h
)

target_link_libraries(QtLucideTextEditor PRIVATE
    Qt6::Core
    Qt6::Widgets
    QtLucide::QtLucide
)
```

## Step 1: Basic Application Structure

### main.cpp

```cpp
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("QtLucide Text Editor");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("QtLucide Tutorial");

    MainWindow window;
    window.show();

    return app.exec();
}
```

### MainWindow.h

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QtLucide/QtLucide.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newDocument();
    void openDocument();
    void saveDocument();
    void toggleBold();
    void toggleItalic();
    void about();
    void updateStatus();

private:
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    void connectSignals();

    // UI Components
    QTextEdit* m_textEdit;
    QMenuBar* m_menuBar;
    QToolBar* m_mainToolBar;
    QStatusBar* m_statusBar;
    QLabel* m_statusLabel;
    QLabel* m_positionLabel;

    // QtLucide
    lucide::QtLucide* m_lucide;

    // Actions
    QAction* m_newAction;
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_boldAction;
    QAction* m_italicAction;
    QAction* m_aboutAction;
};

#endif // MAINWINDOW_H
```

## Step 2: Initialize QtLucide

### MainWindow.cpp - Constructor

```cpp
#include "MainWindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QTextCursor>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_textEdit(nullptr)
    , m_lucide(nullptr)
{
    // Initialize QtLucide first
    m_lucide = new lucide::QtLucide(this);
    if (!m_lucide->initLucide()) {
        QMessageBox::critical(this, "Error",
            "Failed to initialize QtLucide!\n"
            "The application may not display icons correctly.");
    } else {
        qDebug() << "QtLucide initialized with"
                 << m_lucide->availableIcons().size() << "icons";
    }

    setupUI();
    setupMenus();
    setupToolbars();
    setupStatusBar();
    connectSignals();

    // Set window properties
    setWindowTitle("QtLucide Text Editor");
    setMinimumSize(800, 600);
    resize(1000, 700);
}

MainWindow::~MainWindow()
{
    // QtLucide will be deleted automatically as it's a child of this widget
}
```

## Step 3: Create the User Interface

### Setup UI Components

```cpp
void MainWindow::setupUI()
{
    // Create central text editor
    m_textEdit = new QTextEdit(this);
    m_textEdit->setPlaceholderText("Start typing your document here...");
    setCentralWidget(m_textEdit);

    // Set default font
    QFont font("Consolas", 11);
    if (!font.exactMatch()) {
        font.setFamily("Courier New");
    }
    m_textEdit->setFont(font);
}
```

### Setup Menus with Icons

```cpp
void MainWindow::setupMenus()
{
    m_menuBar = menuBar();

    // File Menu
    QMenu* fileMenu = m_menuBar->addMenu("&File");

    m_newAction = fileMenu->addAction(m_lucide->icon("file-plus"), "&New");
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip("Create a new document");

    m_openAction = fileMenu->addAction(m_lucide->icon("folder-open"), "&Open");
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("Open an existing document");

    fileMenu->addSeparator();

    m_saveAction = fileMenu->addAction(m_lucide->icon("save"), "&Save");
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("Save the current document");

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction(m_lucide->icon("x"), "E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    exitAction->setStatusTip("Exit the application");
    connect(exitAction, &QAction::triggered, this, &QWidget::close);

    // Edit Menu
    QMenu* editMenu = m_menuBar->addMenu("&Edit");

    // Use custom colors for formatting actions
    QVariantMap boldOptions;
    boldOptions["color"] = QColor(Qt::darkBlue);
    m_boldAction = editMenu->addAction(m_lucide->icon("bold", boldOptions), "&Bold");
    m_boldAction->setShortcut(QKeySequence::Bold);
    m_boldAction->setCheckable(true);
    m_boldAction->setStatusTip("Make text bold");

    QVariantMap italicOptions;
    italicOptions["color"] = QColor(Qt::darkMagenta);
    m_italicAction = editMenu->addAction(m_lucide->icon("italic", italicOptions), "&Italic");
    m_italicAction->setShortcut(QKeySequence::Italic);
    m_italicAction->setCheckable(true);
    m_italicAction->setStatusTip("Make text italic");

    // Help Menu
    QMenu* helpMenu = m_menuBar->addMenu("&Help");

    QVariantMap aboutOptions;
    aboutOptions["color"] = QColor(Qt::blue);
    m_aboutAction = helpMenu->addAction(m_lucide->icon("info", aboutOptions), "&About");
    m_aboutAction->setStatusTip("About this application");
}
```

## Step 4: Create Toolbars

```cpp
void MainWindow::setupToolbars()
{
    m_mainToolBar = addToolBar("Main");
    m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // File operations
    m_mainToolBar->addAction(m_newAction);
    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);

    m_mainToolBar->addSeparator();

    // Formatting operations
    m_mainToolBar->addAction(m_boldAction);
    m_mainToolBar->addAction(m_italicAction);

    m_mainToolBar->addSeparator();

    // Add some utility buttons with custom styling
    QVariantMap searchOptions;
    searchOptions["color"] = QColor(Qt::darkGreen);
    searchOptions["scale-factor"] = 1.1;
    QAction* searchAction = m_mainToolBar->addAction(
        m_lucide->icon("search", searchOptions), "Search");
    searchAction->setStatusTip("Search in document");

    QVariantMap settingsOptions;
    settingsOptions["color"] = QColor(Qt::gray);
    QAction* settingsAction = m_mainToolBar->addAction(
        m_lucide->icon("settings", settingsOptions), "Settings");
    settingsAction->setStatusTip("Application settings");
}
```

## Step 5: Status Bar with Icons

```cpp
void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();

    // Document status
    m_statusLabel = new QLabel("Ready");
    m_statusBar->addWidget(m_statusLabel);

    // Add a separator with an icon
    QLabel* separatorIcon = new QLabel();
    separatorIcon->setPixmap(m_lucide->icon("separator-vertical").pixmap(16, 16));
    m_statusBar->addPermanentWidget(separatorIcon);

    // Cursor position
    m_positionLabel = new QLabel("Line 1, Column 1");
    m_statusBar->addPermanentWidget(m_positionLabel);

    // Add status icons
    QLabel* statusIcon = new QLabel();
    QVariantMap statusOptions;
    statusOptions["color"] = QColor(Qt::darkGreen);
    statusIcon->setPixmap(m_lucide->icon("check-circle", statusOptions).pixmap(16, 16));
    m_statusBar->addPermanentWidget(statusIcon);
}
```

## Step 6: Connect Signals and Implement Functionality

```cpp
void MainWindow::connectSignals()
{
    // File operations
    connect(m_newAction, &QAction::triggered, this, &MainWindow::newDocument);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openDocument);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveDocument);

    // Formatting
    connect(m_boldAction, &QAction::triggered, this, &MainWindow::toggleBold);
    connect(m_italicAction, &QAction::triggered, this, &MainWindow::toggleItalic);

    // Help
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::about);

    // Text editor signals
    connect(m_textEdit, &QTextEdit::cursorPositionChanged,
            this, &MainWindow::updateStatus);
    connect(m_textEdit, &QTextEdit::textChanged,
            this, &MainWindow::updateStatus);
}

// Implement slot functions
void MainWindow::newDocument()
{
    m_textEdit->clear();
    m_statusLabel->setText("New document created");
}

void MainWindow::openDocument()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open Document", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            m_textEdit->setPlainText(in.readAll());
            m_statusLabel->setText(QString("Opened: %1").arg(fileName));
        }
    }
}

void MainWindow::saveDocument()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save Document", "", "Text Files (*.txt);;All Files (*)");

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << m_textEdit->toPlainText();
            m_statusLabel->setText(QString("Saved: %1").arg(fileName));
        }
    }
}

void MainWindow::toggleBold()
{
    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontWeight(m_boldAction->isChecked() ? QFont::Bold : QFont::Normal);
    cursor.setCharFormat(format);
    m_textEdit->setTextCursor(cursor);
}

void MainWindow::toggleItalic()
{
    QTextCursor cursor = m_textEdit->textCursor();
    QTextCharFormat format = cursor.charFormat();
    format.setFontItalic(m_italicAction->isChecked());
    cursor.setCharFormat(format);
    m_textEdit->setTextCursor(cursor);
}

void MainWindow::about()
{
    QMessageBox::about(this, "About QtLucide Text Editor",
        QString("QtLucide Text Editor v1.0\n\n"
                "Built with QtLucide icon library\n"
                "Icons: %1 available\n\n"
                "A demonstration of QtLucide capabilities.")
        .arg(m_lucide->availableIcons().size()));
}

void MainWindow::updateStatus()
{
    QTextCursor cursor = m_textEdit->textCursor();
    int line = cursor.blockNumber() + 1;
    int column = cursor.columnNumber() + 1;

    m_positionLabel->setText(QString("Line %1, Column %2").arg(line).arg(column));

    int wordCount = m_textEdit->toPlainText().split(QRegExp("\\s+"),
                                                   Qt::SkipEmptyParts).size();
    m_statusLabel->setText(QString("Words: %1").arg(wordCount));
}
```

## Step 7: Build and Test

```bash
mkdir build && cd build
cmake ..
cmake --build .
./QtLucideTextEditor
```

## What You've Learned

This tutorial covered:

### ✅ QtLucide Initialization

- Creating and initializing QtLucide instances
- Error handling for initialization failures

### ✅ Icon Usage Patterns

- Basic icon creation with `icon(name)`
- Custom colors and options
- Using icons in menus, toolbars, and status bars

### ✅ Icon Customization

- Color customization with `QVariantMap`
- Scale factors for different sizes
- State-specific styling

### ✅ Integration Techniques

- Combining icons with Qt's action system
- Using icons in different UI contexts
- Performance considerations

## Next Steps

Now that you understand the basics, explore:

- [User Guide](../user-guide/index.md) - Advanced features and techniques
- [API Reference](../api/index.md) - Complete API documentation
- [Examples](../examples/index.md) - More complex examples
- [Custom Painters](../user-guide/advanced-features.md#custom-painters) - Create your own icon renderers

## Complete Source Code

The complete source code for this tutorial is available in the QtLucide repository under `examples/text-editor/`.

Congratulations! You've built a complete QtLucide application! 🎉
