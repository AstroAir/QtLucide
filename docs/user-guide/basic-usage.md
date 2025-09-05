# Basic Usage

Learn the fundamentals of using QtLucide in your Qt applications.

## Getting Started

### Initialization

Before using any QtLucide features, you must initialize the library:

```cpp
#include <QtLucide/QtLucide.h>

lucide::QtLucide lucide;
if (!lucide.initLucide()) {
    qWarning() << "Failed to initialize QtLucide";
    // Handle error appropriately
    return false;
}
```

!!! tip "Initialization Best Practices" - Initialize QtLucide once at application startup - Always check the return value of `initLucide()` - Consider using a singleton pattern for application-wide access

### Creating Your First Icon

Once initialized, creating icons is straightforward:

```cpp
// Create a basic icon
QIcon homeIcon = lucide.icon("home");

// Use the icon in a button
QPushButton* homeButton = new QPushButton("Home");
homeButton->setIcon(homeIcon);

// Or create and set in one line
QPushButton* settingsButton = new QPushButton("Settings");
settingsButton->setIcon(lucide.icon("settings"));
```

## Icon Creation Methods

QtLucide provides multiple ways to create icons:

### 1. By String Name

The most common method uses string names:

```cpp
QIcon saveIcon = lucide.icon("save");
QIcon openIcon = lucide.icon("folder-open");
QIcon editIcon = lucide.icon("edit");
```

### 2. By Enum Value

For better performance and type safety, use enum values:

```cpp
QIcon saveIcon = lucide.icon(lucide::Icons::save);
QIcon openIcon = lucide.icon(lucide::Icons::folder_open);
QIcon editIcon = lucide.icon(lucide::Icons::edit);
```

!!! note "Enum Naming Convention"
Enum names use underscores instead of hyphens: `folder-open` becomes `folder_open`

### 3. With Custom Options

Customize icons with various options:

```cpp
QVariantMap options;
options["color"] = QColor(Qt::red);
options["scale-factor"] = 1.2;

QIcon customIcon = lucide.icon("heart", options);
```

## Common Usage Patterns

### Menus and Actions

Icons work seamlessly with Qt's action system:

```cpp
void MainWindow::setupMenus()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* newAction = fileMenu->addAction(lucide.icon("file-plus"), "&New");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::newFile);

    QAction* openAction = fileMenu->addAction(lucide.icon("folder-open"), "&Open");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

    fileMenu->addSeparator();

    QAction* saveAction = fileMenu->addAction(lucide.icon("save"), "&Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);
}
```

### Toolbars

Add icons to toolbars for a professional appearance:

```cpp
void MainWindow::setupToolbar()
{
    QToolBar* mainToolbar = addToolBar("Main");
    mainToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    // Add actions with icons
    mainToolbar->addAction(lucide.icon("file-plus"), "New", this, &MainWindow::newFile);
    mainToolbar->addAction(lucide.icon("folder-open"), "Open", this, &MainWindow::openFile);
    mainToolbar->addAction(lucide.icon("save"), "Save", this, &MainWindow::saveFile);

    mainToolbar->addSeparator();

    mainToolbar->addAction(lucide.icon("copy"), "Copy", this, &MainWindow::copy);
    mainToolbar->addAction(lucide.icon("scissors"), "Cut", this, &MainWindow::cut);
    mainToolbar->addAction(lucide.icon("clipboard"), "Paste", this, &MainWindow::paste);
}
```

### Buttons

Enhance buttons with meaningful icons:

```cpp
void Dialog::setupButtons()
{
    QHBoxLayout* buttonLayout = new QHBoxLayout();

    // OK button with check icon
    QPushButton* okButton = new QPushButton("OK");
    okButton->setIcon(lucide.icon("check"));
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    // Cancel button with X icon
    QPushButton* cancelButton = new QPushButton("Cancel");
    cancelButton->setIcon(lucide.icon("x"));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Apply button with check-circle icon
    QPushButton* applyButton = new QPushButton("Apply");
    applyButton->setIcon(lucide.icon("check-circle"));
    connect(applyButton, &QPushButton::clicked, this, &Dialog::applyChanges);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(applyButton);
}
```

## Icon Discovery

### Listing Available Icons

Get a list of all available icons:

```cpp
QStringList availableIcons = lucide.availableIcons();
qDebug() << "Total icons available:" << availableIcons.size();

// Check if a specific icon exists
if (availableIcons.contains("heart")) {
    QIcon heartIcon = lucide.icon("heart");
}
```

### Popular Icon Categories

Here are some commonly used icons organized by category:

#### Navigation

- `home` - Home/dashboard
- `arrow-left`, `arrow-right` - Navigation arrows
- `chevron-left`, `chevron-right` - Subtle navigation
- `menu` - Hamburger menu
- `more-horizontal`, `more-vertical` - More options

#### File Operations

- `file`, `file-plus` - File operations
- `folder`, `folder-open` - Folder operations
- `save` - Save document
- `download`, `upload` - Transfer operations
- `trash-2` - Delete

#### Editing

- `edit`, `pencil` - Edit content
- `copy` - Copy
- `scissors` - Cut
- `clipboard` - Paste
- `undo`, `redo` - Undo/redo operations

#### Status & Feedback

- `check`, `check-circle` - Success/completion
- `x`, `x-circle` - Error/cancel
- `alert-triangle` - Warning
- `info` - Information
- `help-circle` - Help

#### Media Controls

- `play`, `pause` - Playback controls
- `stop`, `square` - Stop playback
- `skip-back`, `skip-forward` - Skip controls
- `volume-2`, `volume-x` - Volume controls

## Error Handling

Always handle potential errors when working with icons:

```cpp
bool createIconSafely(const QString& iconName, QPushButton* button)
{
    // Check if QtLucide is initialized
    if (!lucide.initLucide()) {
        qWarning() << "QtLucide not initialized";
        return false;
    }

    // Check if icon exists
    QStringList available = lucide.availableIcons();
    if (!available.contains(iconName)) {
        qWarning() << "Icon not found:" << iconName;
        // Use a fallback icon
        button->setIcon(lucide.icon("circle"));
        return false;
    }

    // Create the icon
    QIcon icon = lucide.icon(iconName);
    if (icon.isNull()) {
        qWarning() << "Failed to create icon:" << iconName;
        return false;
    }

    button->setIcon(icon);
    return true;
}
```

## Performance Tips

### Icon Reuse

Store frequently used icons to avoid repeated creation:

```cpp
class IconCache
{
public:
    static QIcon getIcon(const QString& name)
    {
        static QHash<QString, QIcon> cache;
        static lucide::QtLucide lucide;
        static bool initialized = lucide.initLucide();

        if (!initialized) {
            return QIcon();
        }

        if (!cache.contains(name)) {
            cache[name] = lucide.icon(name);
        }

        return cache[name];
    }
};

// Usage
QPushButton* button = new QPushButton("Save");
button->setIcon(IconCache::getIcon("save"));
```

### Default Options

Set default options to avoid repetitive customization:

```cpp
void setupIconDefaults()
{
    lucide.setDefaultOption("color", QColor(64, 64, 64));
    lucide.setDefaultOption("scale-factor", 1.1);

    // All subsequent icons will use these defaults
    QIcon icon1 = lucide.icon("home");     // Uses gray color and 1.1 scale
    QIcon icon2 = lucide.icon("settings"); // Uses gray color and 1.1 scale
}
```

## Next Steps

Now that you understand basic usage, explore:

- [Icon Customization](customization.md) - Learn about colors, scaling, and effects
- [Advanced Features](advanced-features.md) - Custom painters and advanced techniques
- [Performance Guide](performance.md) - Optimization strategies
- [Examples](../examples/index.md) - Complete working examples
