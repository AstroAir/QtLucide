# Testing Framework

QtLucide includes a comprehensive testing framework that ensures reliability, performance, and correctness across all supported platforms and use cases.

## Overview

The testing framework consists of multiple test suites covering different aspects of the library:

- **Core Functionality Tests** - Basic QtLucide operations
- **Icon Loading Tests** - Icon creation and validation
- **SVG Rendering Tests** - SVG processing and rendering
- **Gallery Integration Tests** - Gallery application components
- **Error Handling Tests** - Edge cases and error conditions
- **Thread Safety Tests** - Multi-threading scenarios
- **Boundary Condition Tests** - Limits and edge cases
- **Memory Management Tests** - Memory leaks and resource management

## Test Structure

### Test Organization

```
tests/
├── main.cpp                      # Test runner
├── test_qtlucide.cpp/.h         # Core functionality tests
├── test_icon_loading.cpp/.h     # Icon loading tests
├── test_svg_rendering.cpp/.h    # SVG rendering tests
├── test_gallery_icon_loading.cpp/.h  # Gallery tests
├── test_error_handling.cpp/.h   # Error handling tests
├── test_thread_safety.cpp/.h    # Thread safety tests
├── test_boundary_conditions.cpp/.h   # Boundary tests
└── test_memory_management.cpp/.h     # Memory tests
```

### Test Framework

QtLucide uses Qt Test framework for all testing:

```cpp
#include <QtTest/QtTest>
#include <QtLucide/QtLucide.h>

class TestQtLucide : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();    // Setup before all tests
    void cleanupTestCase(); // Cleanup after all tests
    void init();           // Setup before each test
    void cleanup();        // Cleanup after each test
    
    // Test methods
    void testInitialization();
    void testIconCreation();
    void testCustomOptions();

private:
    lucide::QtLucide* m_lucide;
};
```

## Core Functionality Tests

### Initialization Tests

```cpp
void TestQtLucide::testInitialization()
{
    QVERIFY(m_lucide != nullptr);
    QVERIFY(m_lucide->initLucide());

    // Test that we have the expected number of icons
    QStringList icons = m_lucide->availableIcons();
    QVERIFY(icons.size() == 1634); // Exact count from Lucide v0.460.0
    QVERIFY(icons.contains("activity"));
    QVERIFY(icons.contains("circle-alert"));
}
```

### Icon Creation Tests

```cpp
void TestQtLucide::testIconCreation()
{
    QVERIFY(m_lucide->initLucide());

    // Test icon creation by name
    QIcon icon = m_lucide->icon("activity");
    QVERIFY(!icon.isNull());
    QVERIFY(!icon.pixmap(32, 32).isNull());

    // Test icon creation by enum
    QIcon enumIcon = m_lucide->icon(lucide::Icons::activity);
    QVERIFY(!enumIcon.isNull());

    // Test invalid icon
    QIcon invalidIcon = m_lucide->icon("non-existent-icon");
    QVERIFY(invalidIcon.isNull());
}
```

### Customization Tests

```cpp
void TestQtLucide::testCustomOptions()
{
    QVERIFY(m_lucide->initLucide());

    // Test color customization
    QVariantMap options;
    options["color"] = QColor(Qt::red);
    options["scale-factor"] = 1.5;

    QIcon customIcon = m_lucide->icon("heart", options);
    QVERIFY(!customIcon.isNull());

    // Test default options
    m_lucide->setDefaultOption("color", QColor(Qt::blue));
    QVariant defaultColor = m_lucide->defaultOption("color");
    QCOMPARE(defaultColor.value<QColor>(), QColor(Qt::blue));
}
```

## SVG Rendering Tests

### SVG Data Validation

```cpp
void TestSvgRendering::testSvgDataRetrieval()
{
    QVERIFY(m_lucide->initLucide());

    // Test SVG data by name
    QByteArray svgData = m_lucide->svgData("heart");
    QVERIFY(!svgData.isEmpty());
    QVERIFY(svgData.contains("<svg"));
    QVERIFY(svgData.contains("</svg>"));

    // Test SVG data by enum
    QByteArray enumSvgData = m_lucide->svgData(lucide::Icons::heart);
    QCOMPARE(svgData, enumSvgData);

    // Test invalid icon
    QByteArray invalidSvgData = m_lucide->svgData("invalid-icon");
    QVERIFY(invalidSvgData.isEmpty());
}
```

### Color Replacement Tests

```cpp
void TestSvgRendering::testColorReplacement()
{
    QVERIFY(m_lucide->initLucide());

    QVariantMap options;
    options["color"] = QColor(255, 0, 0); // Red

    QIcon redIcon = m_lucide->icon("heart", options);
    QVERIFY(!redIcon.isNull());

    // Render to pixmap and check color
    QPixmap pixmap = redIcon.pixmap(32, 32);
    QVERIFY(!pixmap.isNull());
    
    // Basic color validation (red should be present)
    QImage image = pixmap.toImage();
    bool hasRed = false;
    for (int y = 0; y < image.height() && !hasRed; ++y) {
        for (int x = 0; x < image.width() && !hasRed; ++x) {
            QColor pixel = image.pixelColor(x, y);
            if (pixel.red() > 200 && pixel.green() < 50 && pixel.blue() < 50) {
                hasRed = true;
            }
        }
    }
    QVERIFY(hasRed);
}
```

## Thread Safety Tests

### Concurrent Icon Creation

```cpp
void TestThreadSafety::testConcurrentIconCreation()
{
    QVERIFY(m_lucide->initLucide());

    const int threadCount = 4;
    const int iconsPerThread = 100;
    QVector<QThread*> threads;
    QVector<IconCreationWorker*> workers;

    // Create worker threads
    for (int i = 0; i < threadCount; ++i) {
        QThread* thread = new QThread();
        IconCreationWorker* worker = new IconCreationWorker(m_lucide, iconsPerThread);
        worker->moveToThread(thread);
        
        connect(thread, &QThread::started, worker, &IconCreationWorker::createIcons);
        connect(worker, &IconCreationWorker::finished, thread, &QThread::quit);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        
        threads.append(thread);
        workers.append(worker);
    }

    // Start all threads
    for (QThread* thread : threads) {
        thread->start();
    }

    // Wait for completion
    for (QThread* thread : threads) {
        QVERIFY(thread->wait(10000)); // 10 second timeout
    }

    // Verify results
    for (IconCreationWorker* worker : workers) {
        QCOMPARE(worker->successCount(), iconsPerThread);
        QCOMPARE(worker->errorCount(), 0);
    }
}
```

## Memory Management Tests

### Memory Leak Detection

```cpp
void TestMemoryManagement::testMemoryLeaks()
{
    QVERIFY(m_lucide->initLucide());

    // Baseline memory usage
    size_t baselineMemory = getCurrentMemoryUsage();

    // Create many icons
    const int iconCount = 1000;
    QVector<QIcon> icons;
    icons.reserve(iconCount);

    for (int i = 0; i < iconCount; ++i) {
        QString iconName = m_lucide->availableIcons().at(i % m_lucide->availableIcons().size());
        icons.append(m_lucide->icon(iconName));
    }

    // Check memory usage
    size_t peakMemory = getCurrentMemoryUsage();
    
    // Clear icons
    icons.clear();
    
    // Force garbage collection
    QCoreApplication::processEvents();
    
    // Check final memory usage
    size_t finalMemory = getCurrentMemoryUsage();
    
    // Memory should not have grown significantly
    size_t memoryGrowth = finalMemory - baselineMemory;
    QVERIFY(memoryGrowth < 1024 * 1024); // Less than 1MB growth
}
```

### Cache Management Tests

```cpp
void TestMemoryManagement::testCacheManagement()
{
    QVERIFY(m_lucide->initLucide());

    // Create icons to populate cache
    QVector<QIcon> icons;
    for (int i = 0; i < 200; ++i) {
        icons.append(m_lucide->icon("heart"));
    }

    // Cache should limit size automatically
    // This is tested indirectly through memory usage
    size_t memoryUsage = getCurrentMemoryUsage();
    QVERIFY(memoryUsage < 50 * 1024 * 1024); // Less than 50MB
}
```

## Performance Tests

### Icon Creation Benchmarks

```cpp
void TestPerformance::benchmarkIconCreation()
{
    QVERIFY(m_lucide->initLucide());

    QBENCHMARK {
        QIcon icon = m_lucide->icon("heart");
        Q_UNUSED(icon);
    }
}

void TestPerformance::benchmarkPixmapGeneration()
{
    QVERIFY(m_lucide->initLucide());
    QIcon icon = m_lucide->icon("heart");

    QBENCHMARK {
        QPixmap pixmap = icon.pixmap(32, 32);
        Q_UNUSED(pixmap);
    }
}
```

## Error Handling Tests

### Invalid Input Tests

```cpp
void TestErrorHandling::testInvalidIconNames()
{
    QVERIFY(m_lucide->initLucide());

    // Test various invalid inputs
    QStringList invalidNames = {
        "",
        "non-existent-icon",
        "invalid/name",
        "icon with spaces",
        QString::fromUtf8("🚀"), // Unicode
        QString(1000, 'a')       // Very long string
    };

    for (const QString& name : invalidNames) {
        QIcon icon = m_lucide->icon(name);
        QVERIFY(icon.isNull());
        
        QByteArray svgData = m_lucide->svgData(name);
        QVERIFY(svgData.isEmpty());
    }
}
```

### Resource Exhaustion Tests

```cpp
void TestErrorHandling::testResourceExhaustion()
{
    QVERIFY(m_lucide->initLucide());

    // Try to create an excessive number of icons
    QVector<QIcon> icons;
    const int maxIcons = 10000;
    
    for (int i = 0; i < maxIcons; ++i) {
        QIcon icon = m_lucide->icon("heart");
        if (icon.isNull()) {
            break; // Graceful degradation
        }
        icons.append(icon);
    }

    // Should handle resource limits gracefully
    QVERIFY(icons.size() > 100); // Should create at least some icons
}
```

## Running Tests

### CMake

```bash
cd QtLucide/build
cmake .. -DQTLUCIDE_BUILD_TESTS=ON
cmake --build .
ctest --output-on-failure
```

### Meson

```bash
cd QtLucide
meson setup builddir -Dtests=true
meson compile -C builddir
meson test -C builddir
```

### XMake

```bash
cd QtLucide
xmake config --tests=true
xmake build
xmake run QtLucideTests
```

### Individual Test Execution

```bash
# Run specific test
./QtLucideTests -test TestQtLucide::testInitialization

# Run with verbose output
./QtLucideTests -v2

# Generate XML output
./QtLucideTests -xml -o test_results.xml
```

## Continuous Integration

QtLucide uses GitHub Actions for automated testing:

- **Multi-platform Testing** - Windows, macOS, Linux
- **Multiple Qt Versions** - Qt 6.7.3, 6.8.0
- **Build System Testing** - CMake, Meson, XMake
- **Code Coverage** - LCOV coverage reporting
- **Performance Monitoring** - Benchmark regression detection
- **Memory Testing** - Valgrind integration on Linux

### Coverage Reports

Test coverage is automatically generated and reported:

```bash
# Generate coverage report locally
cmake .. -DCMAKE_CXX_FLAGS="--coverage"
cmake --build .
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

## Contributing Tests

When contributing to QtLucide:

1. **Add tests for new features** - All new functionality should have tests
2. **Update existing tests** - Modify tests when changing behavior
3. **Run full test suite** - Ensure all tests pass before submitting
4. **Add performance tests** - For performance-critical changes
5. **Document test cases** - Explain complex test scenarios

### Test Guidelines

- **Focused Tests** - Each test should verify one specific behavior
- **Descriptive Names** - Test names should clearly indicate what is tested
- **Independent Tests** - Tests should not depend on each other
- **Cleanup Resources** - Always clean up in test destructors
- **Use QVERIFY/QCOMPARE** - Prefer Qt Test macros over assertions

## See Also

- [Development Guide](index.md) - General development information
- [Contributing](contributing.md) - How to contribute to QtLucide
- [Performance Guide](../user-guide/performance.md) - Performance optimization
- [API Reference](../api/index.md) - Complete API documentation
