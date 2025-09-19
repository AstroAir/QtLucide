# Performance Guide

This guide covers performance optimization techniques and best practices for using QtLucide efficiently in your applications.

## Overview

QtLucide is designed for high performance, but understanding its internals and following best practices can significantly improve your application's performance, especially when dealing with large numbers of icons.

## Performance Characteristics

### Icon Creation Performance

| Operation | Time (avg) | Notes |
|-----------|------------|-------|
| `icon("name")` | ~0.1ms | String lookup + enum conversion |
| `icon(Icons::enum)` | ~0.05ms | Direct enum access (preferred) |
| `icon.pixmap(32,32)` | ~2-5ms | SVG rendering + caching |
| Cache hit | ~0.01ms | Cached pixmap retrieval |

### Memory Usage

| Component | Memory Usage | Notes |
|-----------|--------------|-------|
| QtLucide instance | ~50KB | Icon mappings and metadata |
| SVG data (all icons) | ~2MB | Compressed SVG strings |
| Pixmap cache (100 icons) | ~5-10MB | Depends on sizes and formats |
| Icon engine overhead | ~1KB per icon | QIconEngine instances |

## Optimization Strategies

### 1. Use Enum Values Instead of Strings

**Recommended:**

```cpp
QIcon icon = lucide.icon(lucide::Icons::heart);  // Fast enum lookup
```

**Avoid:**

```cpp
QIcon icon = lucide.icon("heart");  // Slower string lookup
```

**Performance Impact:** 2x faster icon creation

### 2. Cache Frequently Used Icons

Create a simple icon cache for commonly used icons:

```cpp
class IconCache
{
public:
    static QIcon getIcon(lucide::Icons iconId)
    {
        static QHash<lucide::Icons, QIcon> cache;
        static lucide::QtLucide lucide;
        static bool initialized = lucide.initLucide();

        if (!initialized) {
            return QIcon();
        }

        if (!cache.contains(iconId)) {
            cache[iconId] = lucide.icon(iconId);
        }

        return cache[iconId];
    }
};

// Usage
QPushButton* button = new QPushButton("Save");
button->setIcon(IconCache::getIcon(lucide::Icons::save));
```

### 3. Set Default Options Once

Instead of passing options repeatedly, set defaults:

```cpp
// Inefficient - options passed every time
for (int i = 0; i < 100; ++i) {
    QVariantMap options;
    options["color"] = QColor(Qt::gray);
    options["scale-factor"] = 1.1;
    icons.append(lucide.icon(iconNames[i], options));
}

// Efficient - set defaults once
lucide.setDefaultOption("color", QColor(Qt::gray));
lucide.setDefaultOption("scale-factor", 1.1);
for (int i = 0; i < 100; ++i) {
    icons.append(lucide.icon(iconNames[i]));
}
```

### 4. Optimize Pixmap Sizes

Request appropriate pixmap sizes to avoid unnecessary scaling:

```cpp
// Good - request exact size needed
QIcon icon = lucide.icon("heart");
QPixmap pixmap = icon.pixmap(16, 16);  // For 16x16 display

// Avoid - requesting large pixmap then scaling down
QPixmap pixmap = icon.pixmap(256, 256).scaled(16, 16);
```

### 5. Lazy Loading for Large Icon Sets

For applications displaying many icons (like the Gallery), use lazy loading:

```cpp
class LazyIconModel : public QAbstractListModel
{
private:
    QStringList m_iconNames;
    mutable QHash<int, QIcon> m_iconCache;
    lucide::QtLucide* m_lucide;

public:
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (role == Qt::DecorationRole) {
            // Load icon on demand
            if (!m_iconCache.contains(index.row())) {
                QString iconName = m_iconNames.at(index.row());
                m_iconCache[index.row()] = m_lucide->icon(iconName);
            }
            return m_iconCache[index.row()];
        }
        return QVariant();
    }
};
```

## Memory Management

### 1. Understand Caching Behavior

QtLucide uses multiple levels of caching:

- **SVG Data Cache** - SVG strings are cached permanently
- **Pixmap Cache** - Rendered pixmaps are cached with size limits
- **QIcon Cache** - Qt's internal icon caching

### 2. Monitor Memory Usage

```cpp
class MemoryMonitor
{
public:
    static void logMemoryUsage(const QString& context)
    {
        #ifdef Q_OS_WIN
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            qDebug() << context << "Memory:" << pmc.WorkingSetSize / 1024 / 1024 << "MB";
        }
        #elif defined(Q_OS_LINUX)
        QFile file("/proc/self/status");
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream stream(&file);
            QString line;
            while (stream.readLineInto(&line)) {
                if (line.startsWith("VmRSS:")) {
                    qDebug() << context << "Memory:" << line;
                    break;
                }
            }
        }
        #endif
    }
};

// Usage
MemoryMonitor::logMemoryUsage("Before icon creation");
// ... create many icons ...
MemoryMonitor::logMemoryUsage("After icon creation");
```

### 3. Clear Caches When Appropriate

```cpp
// Clear Qt's internal icon cache
QIcon::setThemeName(QIcon::themeName()); // Force cache clear

// For custom caches, implement cleanup
class ManagedIconCache
{
private:
    static QHash<QString, QIcon> s_cache;
    static const int MAX_CACHE_SIZE = 500;

public:
    static QIcon getIcon(const QString& name)
    {
        if (s_cache.size() > MAX_CACHE_SIZE) {
            s_cache.clear(); // Simple cleanup strategy
        }
        
        if (!s_cache.contains(name)) {
            s_cache[name] = lucide.icon(name);
        }
        
        return s_cache[name];
    }
};
```

## Threading Considerations

### 1. Thread-Safe Icon Creation

QtLucide is thread-safe for icon creation after initialization:

```cpp
class IconWorker : public QObject
{
    Q_OBJECT

public slots:
    void createIcons()
    {
        // Safe to create icons from worker threads
        QVector<QIcon> icons;
        for (const QString& name : m_iconNames) {
            icons.append(m_lucide->icon(name));
        }
        emit iconsCreated(icons);
    }

signals:
    void iconsCreated(const QVector<QIcon>& icons);

private:
    lucide::QtLucide* m_lucide;
    QStringList m_iconNames;
};
```

### 2. Avoid UI Operations in Worker Threads

```cpp
// Wrong - pixmap creation in worker thread
void WorkerThread::run()
{
    QIcon icon = lucide.icon("heart");
    QPixmap pixmap = icon.pixmap(32, 32); // May cause issues
}

// Correct - defer pixmap creation to main thread
void WorkerThread::run()
{
    QIcon icon = lucide.icon("heart");
    emit iconReady(icon); // Pass icon to main thread
}

void MainWindow::onIconReady(const QIcon& icon)
{
    QPixmap pixmap = icon.pixmap(32, 32); // Safe in main thread
    label->setPixmap(pixmap);
}
```

## Profiling and Benchmarking

### 1. Built-in Benchmarks

QtLucide includes performance benchmarks:

```bash
# Run benchmarks
./QtLucideTests -benchmark

# Specific benchmark
./QtLucideTests -benchmark TestPerformance::benchmarkIconCreation
```

### 2. Custom Profiling

```cpp
class PerformanceProfiler
{
public:
    static void profileIconCreation(lucide::QtLucide& lucide, int count)
    {
        QElapsedTimer timer;
        timer.start();
        
        QVector<QIcon> icons;
        icons.reserve(count);
        
        for (int i = 0; i < count; ++i) {
            icons.append(lucide.icon(lucide::Icons::heart));
        }
        
        qint64 elapsed = timer.elapsed();
        qDebug() << "Created" << count << "icons in" << elapsed << "ms";
        qDebug() << "Average:" << (double)elapsed / count << "ms per icon";
    }
};
```

### 3. Memory Profiling

Use tools like Valgrind (Linux) or Application Verifier (Windows):

```bash
# Linux - Valgrind
valgrind --tool=massif ./QtLucideExample
ms_print massif.out.* > memory_profile.txt

# Windows - Visual Studio Diagnostic Tools
# Use built-in memory profiler in Visual Studio
```

## Platform-Specific Optimizations

### Windows

```cpp
// Enable high-DPI awareness
#ifdef Q_OS_WIN
QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
```

### macOS

```cpp
// Optimize for Retina displays
#ifdef Q_OS_MACOS
QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
// Consider 2x pixmaps for Retina displays
QIcon icon = lucide.icon("heart");
QPixmap pixmap = icon.pixmap(32 * devicePixelRatio(), 32 * devicePixelRatio());
#endif
```

### Linux

```cpp
// Optimize for various DPI settings
#ifdef Q_OS_LINUX
QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
// Check for Wayland-specific optimizations
if (QGuiApplication::platformName() == "wayland") {
    // Wayland-specific optimizations
}
#endif
```

## Performance Monitoring

### 1. Real-time Monitoring

```cpp
class PerformanceMonitor : public QObject
{
    Q_OBJECT

public:
    PerformanceMonitor(QObject* parent = nullptr) : QObject(parent)
    {
        m_timer.start();
        connect(&m_updateTimer, &QTimer::timeout, this, &PerformanceMonitor::updateStats);
        m_updateTimer.start(1000); // Update every second
    }

    void recordIconCreation()
    {
        m_iconCreationCount++;
        m_totalIconCreationTime += m_timer.restart();
    }

private slots:
    void updateStats()
    {
        if (m_iconCreationCount > 0) {
            double avgTime = (double)m_totalIconCreationTime / m_iconCreationCount;
            qDebug() << "Icons created:" << m_iconCreationCount 
                     << "Avg time:" << avgTime << "ms";
            
            // Reset counters
            m_iconCreationCount = 0;
            m_totalIconCreationTime = 0;
        }
    }

private:
    QElapsedTimer m_timer;
    QTimer m_updateTimer;
    int m_iconCreationCount = 0;
    qint64 m_totalIconCreationTime = 0;
};
```

### 2. Performance Alerts

```cpp
class PerformanceAlert
{
public:
    static void checkIconCreationTime(qint64 elapsed)
    {
        const qint64 WARNING_THRESHOLD = 10; // 10ms
        const qint64 ERROR_THRESHOLD = 50;   // 50ms
        
        if (elapsed > ERROR_THRESHOLD) {
            qCritical() << "Slow icon creation:" << elapsed << "ms";
        } else if (elapsed > WARNING_THRESHOLD) {
            qWarning() << "Icon creation slower than expected:" << elapsed << "ms";
        }
    }
};
```

## Best Practices Summary

1. **Use enum values** instead of string names for better performance
2. **Cache frequently used icons** to avoid repeated creation
3. **Set default options once** rather than passing them repeatedly
4. **Request appropriate pixmap sizes** to avoid unnecessary scaling
5. **Use lazy loading** for large icon sets
6. **Monitor memory usage** and implement cache cleanup strategies
7. **Profile your application** to identify performance bottlenecks
8. **Consider threading** for icon-heavy operations
9. **Optimize for target platforms** with platform-specific code
10. **Test with realistic data sets** to ensure good performance

## Troubleshooting Performance Issues

### Common Performance Problems

1. **Slow icon creation** - Use enum values instead of strings
2. **High memory usage** - Implement cache size limits
3. **UI freezing** - Move icon creation to worker threads
4. **Pixmap rendering slow** - Check requested sizes and caching
5. **Memory leaks** - Ensure proper cleanup of custom caches

### Debugging Tools

- **Qt Creator Profiler** - Built-in performance analysis
- **Valgrind** - Memory profiling on Linux
- **Visual Studio Diagnostics** - Windows performance tools
- **Instruments** - macOS performance profiling
- **Custom benchmarks** - Application-specific measurements

## See Also

- [User Guide](index.md) - General usage information
- [Testing Framework](../development/testing.md) - Performance testing
- [API Reference](../api/index.md) - Complete API documentation
- [Examples](../examples/index.md) - Performance optimization examples
