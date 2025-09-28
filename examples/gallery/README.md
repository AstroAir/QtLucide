# QtLucide Gallery Example

A comprehensive gallery application demonstrating the full capabilities of the QtLucide icon library. This example serves as both a showcase and a practical tool for browsing, searching, and managing Lucide icons.

## Features

### 🎨 Icon Management

- **Complete Icon Library**: Browse all 1634 Lucide icons
- **Advanced Search**: Search by name, category, tags, and metadata
- **Favorites System**: Mark and manage favorite icons
- **Icon Details**: View comprehensive metadata for each icon
- **Export/Import**: Export icons in various formats (SVG, PNG, ICO)

### 🖼️ Image Gallery

- **Image Browser**: Browse and view images from local directories
- **Metadata Display**: View image properties, EXIF data, and file information
- **Thumbnail Grid**: Efficient thumbnail generation and display
- **Image Viewer**: Full-featured image viewer with zoom and navigation

### 🎯 User Interface

- **Modern Design**: Clean, responsive interface with multiple view modes
- **Theme System**: Light, dark, and system themes with custom theme support
- **Responsive Layout**: Adaptive layout that works on different screen sizes
- **Keyboard Shortcuts**: Comprehensive keyboard navigation support
- **Drag & Drop**: Support for dragging icons and images

### 🔧 Technical Features

- **High Performance**: Optimized for large icon sets with lazy loading
- **Memory Management**: Efficient caching and resource management
- **Multi-threading**: Background loading and processing
- **Settings Persistence**: Save and restore user preferences
- **Error Handling**: Graceful error handling and recovery

## Building

### Prerequisites

- Qt6 (Core, Gui, Widgets, Svg, Concurrent, Network)
- CMake 3.16+ or Meson 0.60+
- C++17 compatible compiler

### CMake Build

```bash
# Configure with examples enabled
cmake -S . -B build -DQTLUCIDE_BUILD_EXAMPLES=ON -DQTLUCIDE_BUILD_TESTS=ON

# Build the gallery
cmake --build build --target QtLucideGallery -j

# Run the gallery
./build/QtLucide\ Gallery.exe  # Windows
./build/QtLucide\ Gallery      # Linux/macOS
```

### Quick smoke check (headless-friendly)

```bash
# On Windows PowerShell
& "./build/QtLucide Gallery.exe" --smoke

# On Linux/macOS
./build/QtLucide\ Gallery --smoke
```

### Meson Build

```bash
# Configure with examples enabled
meson setup builddir_meson -Dexamples=enabled -Dtests=enabled

# Build the gallery
meson compile -C builddir_meson

# Run the gallery
./builddir_meson/examples/gallery/QtLucideGallery
```

## Testing

The gallery includes comprehensive tests covering all major functionality:

```bash
# Run all gallery tests
ctest --test-dir build -L gallery

# Run specific test categories
ctest --test-dir build -L gallery.basic      # Basic functionality
ctest --test-dir build -L gallery.components # Component tests
ctest --test-dir build -L gallery.integration # Integration tests
```

### Test Categories

> Note: Integration tests are GUI-heavy and disabled by default. To enable them when a GUI environment is available, configure CMake with:
> `-DQTLUCIDE_GALLERY_ENABLE_INTEGRATION_TESTS=ON`, then run:
> `ctest --test-dir build -L gallery.integration`

- **Basic Tests**: Core functionality and component initialization (41 tests)
- **Component Tests**: Individual widget and manager testing (13 tests)
- **Integration Tests**: End-to-end workflow testing (GUI environment dependent)
- **Main Tests**: Application lifecycle and startup testing

## Architecture

### Core Components

- **ContentManager**: Manages icons and images, handles loading and caching
- **IconMetadataManager**: Handles icon metadata, categories, and search indexing
- **ImageMetadataManager**: Manages image metadata and thumbnail generation
- **FavoritesManager**: Handles favorite icons and persistence
- **ThemeManager**: Manages application themes and styling

### UI Components

- **GalleryMainWindow**: Main application window with responsive layout
- **IconGridWidget**: Displays icons in a grid with various view modes
- **ModernSearchWidget**: Advanced search interface with filtering
- **IconDetailsPanel**: Shows detailed information about selected icons
- **ImageViewerWidget**: Full-featured image viewer
- **FileBrowserWidget**: File system browser for images

## Usage

### Basic Navigation

1. **Browse Icons**: Use the main grid to browse all available icons
2. **Search**: Use the search bar to find specific icons by name or category
3. **Filter**: Apply category and tag filters to narrow results
4. **View Details**: Click on an icon to see detailed information
5. **Favorites**: Click the star icon to add/remove favorites

### Advanced Features

- **Export Icons**: Select icons and use File → Export to save in various formats
- **Import Settings**: Import/export application settings and favorites
- **Theme Switching**: Use View → Theme to switch between light/dark themes
- **Keyboard Shortcuts**: Press F1 or use Help → Shortcuts for a complete list

### Keyboard Shortcuts

- `Ctrl+F`: Focus search
- `Ctrl+1-9`: Switch categories
- `F11`: Toggle fullscreen
- `Ctrl+Plus/Minus`: Zoom in/out
- `Space`: Toggle favorite for selected icon
- `Escape`: Clear search/selection

## Configuration

The gallery stores settings in the system's standard location:

- **Windows**: `HKEY_CURRENT_USER\Software\QtLucide\QtLucide Gallery`
- **Linux**: `~/.config/QtLucide/QtLucide Gallery.conf`
- **macOS**: `~/Library/Preferences/org.qtlucide.QtLucide Gallery.plist`

### Customizable Settings

- Window size and position
- Theme preferences
- Search history
- Favorite icons
- View mode preferences
- Export settings

## Development

### Adding New Features

1. Follow the existing architecture patterns
2. Add appropriate tests for new functionality
3. Update documentation
4. Ensure code follows the project's style guidelines

### Code Style

- Use 4-space indentation
- Follow Qt naming conventions
- Document public APIs with Doxygen comments
- Keep lines under 100 characters

## Troubleshooting

### Common Issues

- **Icons not loading**: Ensure QtLucide resources are properly built
- **Slow performance**: Check available memory and reduce icon cache size
- **Theme issues**: Reset theme settings or restart the application

### Debug Mode

Build with debug symbols for detailed logging:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DQTLUCIDE_BUILD_EXAMPLES=ON
```

## Contributing

This gallery example serves as a reference implementation. Contributions are welcome:

1. Fork the repository
2. Create a feature branch
3. Add tests for new functionality
4. Submit a pull request

## License

This example is part of the QtLucide project and follows the same license terms.
