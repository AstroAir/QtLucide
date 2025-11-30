# Advanced Features

Explore QtLucide's advanced capabilities including custom painters, caching strategies, and integration techniques.

## Custom Icon Painters

### Understanding the Painter System

QtLucide uses a flexible painter system that allows you to create completely custom icon rendering logic. The `QtLucideIconPainter` abstract base class provides the interface for custom implementations.

!!! note "Painter Ownership"
    When you pass a painter to `give()` or `icon()`, QtLucide takes ownership. Do not delete the painter manually or use it elsewhere after passing it.

### Creating Custom Painters

#### Basic Custom Painter

```cpp
class TextIconPainter : public QtLucideIconPainter
{
public:
    TextIconPainter(const QString& text, const QFont& font = QFont())
        : m_text(text), m_font(font) {}

    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state,
               const QVariantMap& options) override
    {
        // Get color from options
        QColor color = options.value("color", Qt::black).value<QColor>();

        // Adjust color based on mode
        switch (mode) {
            case QIcon::Disabled:
                color = color.lighter(150);
                break;
            case QIcon::Active:
                color = color.darker(110);
                break;
            case QIcon::Selected:
                color = color.darker(120);
                break;
            default:
                break;
        }

        // Setup painter
        painter->save();
        painter->setPen(color);

        // Set font size based on rect
        QFont font = m_font;
        font.setPixelSize(rect.height() * 0.7);
        painter->setFont(font);

        // Draw text centered
        painter->drawText(rect, Qt::AlignCenter, m_text);
        painter->restore();
    }

    QString iconText() const override
    {
        return QString("text-%1").arg(m_text);
    }

    QtLucideIconPainter* clone() const override
    {
        return new TextIconPainter(m_text, m_font);
    }

private:
    QString m_text;
    QFont m_font;
};
```

#### Advanced Custom Painter with Effects

```cpp
class GlowIconPainter : public QtLucideIconPainter
{
public:
    GlowIconPainter(const QString& iconName, const QColor& glowColor = Qt::blue)
        : m_iconName(iconName), m_glowColor(glowColor) {}

    void paint(QtLucide* lucide, QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state,
               const QVariantMap& options) override
    {
        painter->save();

        // Get base color
        QColor baseColor = options.value("color", Qt::black).value<QColor>();
        double glowIntensity = options.value("glow-intensity", 0.5).toDouble();

        // Create glow effect
        if (glowIntensity > 0.0) {
            drawGlowEffect(painter, rect, glowIntensity);
        }

        // Draw the base icon
        drawBaseIcon(lucide, painter, rect, baseColor, mode, state);

        painter->restore();
    }

    QString iconText() const override
    {
        return QString("glow-%1-%2").arg(m_iconName).arg(m_glowColor.name());
    }

    QtLucideIconPainter* clone() const override
    {
        return new GlowIconPainter(m_iconName, m_glowColor);
    }

private:
    QString m_iconName;
    QColor m_glowColor;

    void drawGlowEffect(QPainter* painter, const QRect& rect, double intensity)
    {
        // Create radial gradient for glow
        QRadialGradient gradient(rect.center(), rect.width() * 0.6);
        QColor glowColor = m_glowColor;
        glowColor.setAlphaF(intensity * 0.3);
        gradient.setColorAt(0, glowColor);
        glowColor.setAlphaF(0);
        gradient.setColorAt(1, glowColor);

        painter->setBrush(gradient);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect);
    }

    void drawBaseIcon(QtLucide* lucide, QPainter* painter, const QRect& rect,
                     const QColor& color, QIcon::Mode mode, QIcon::State state)
    {
        // Get SVG data and render with custom color
        QByteArray svgData = lucide->svgData(m_iconName);
        if (!svgData.isEmpty()) {
            // Custom SVG rendering implementation
            renderCustomSvg(painter, rect, svgData, color);
        }
    }

    void renderCustomSvg(QPainter* painter, const QRect& rect,
                        const QByteArray& svgData, const QColor& color);
};
```

### Using Custom Painters

#### Direct Usage

```cpp
// Create and use custom painter directly
TextIconPainter* textPainter = new TextIconPainter("★");
QIcon starIcon = lucide.icon(textPainter);

// Use in UI
button->setIcon(starIcon);
```

#### Registration for Reuse

```cpp
// Register custom painters for easy reuse
lucide.give("star-text", new TextIconPainter("★"));
lucide.give("heart-text", new TextIconPainter("♥"));
lucide.give("glow-star", new GlowIconPainter("star", Qt::yellow));

// Use like built-in icons
QIcon starIcon = lucide.icon("star-text");
QIcon heartIcon = lucide.icon("heart-text");
QIcon glowIcon = lucide.icon("glow-star");
```

!!! warning "Replacing Painters"
    If you call `give()` with a name that already exists, the old painter is deleted and replaced with the new one.

## Advanced Caching Strategies

### Custom Cache Implementation

```cpp
class AdvancedIconCache
{
public:
    static QIcon getIcon(lucide::QtLucide& lucide, const QString& name,
                        const QVariantMap& options = {})
    {
        QString key = generateCacheKey(name, options);

        // Check memory cache first
        if (m_memoryCache.contains(key)) {
            return m_memoryCache[key];
        }

        // Check disk cache
        QIcon icon = loadFromDiskCache(key);
        if (!icon.isNull()) {
            m_memoryCache[key] = icon;
            return icon;
        }

        // Create new icon
        icon = lucide.icon(name, options);

        // Store in caches
        m_memoryCache[key] = icon;
        saveToDiskCache(key, icon);

        // Cleanup if cache is too large
        if (m_memoryCache.size() > MAX_CACHE_SIZE) {
            cleanupCache();
        }

        return icon;
    }

    static void clearCache()
    {
        m_memoryCache.clear();
        clearDiskCache();
    }

private:
    static QHash<QString, QIcon> m_memoryCache;
    static const int MAX_CACHE_SIZE = 1000;

    static QString generateCacheKey(const QString& name, const QVariantMap& options);
    static QIcon loadFromDiskCache(const QString& key);
    static void saveToDiskCache(const QString& key, const QIcon& icon);
    static void cleanupCache();
    static void clearDiskCache();
};
```

### Preloading Strategies

```cpp
class IconPreloader
{
public:
    static void preloadCommonIcons(lucide::QtLucide& lucide)
    {
        // Preload frequently used icons
        QStringList commonIcons = {
            "home", "settings", "user", "search", "menu",
            "save", "open", "close", "edit", "delete",
            "check", "x", "plus", "minus", "arrow-left"
        };

        // Preload at common sizes
        QList<QSize> commonSizes = {
            QSize(16, 16), QSize(24, 24), QSize(32, 32), QSize(48, 48)
        };

        for (const QString& iconName : commonIcons) {
            QIcon icon = lucide.icon(iconName);
            for (const QSize& size : commonSizes) {
                // Force pixmap creation and caching
                icon.pixmap(size);
            }
        }
    }

    static void preloadThemeIcons(lucide::QtLucide& lucide, const QString& theme)
    {
        QVariantMap themeOptions = getThemeOptions(theme);

        // Preload with theme-specific options
        QStringList themeIcons = getThemeIconList(theme);
        for (const QString& iconName : themeIcons) {
            lucide.icon(iconName, themeOptions);
        }
    }

private:
    static QVariantMap getThemeOptions(const QString& theme);
    static QStringList getThemeIconList(const QString& theme);
};
```

## Integration Patterns

### Singleton Pattern

```cpp
class IconManager
{
public:
    static IconManager& instance()
    {
        static IconManager instance;
        return instance;
    }

    QIcon getIcon(const QString& name, const QVariantMap& options = {})
    {
        return m_lucide.icon(name, options);
    }

    QIcon getThemedIcon(const QString& name, const QString& theme = "default")
    {
        QVariantMap options = getThemeOptions(theme);
        return m_lucide.icon(name, options);
    }

    void setTheme(const QString& theme)
    {
        m_currentTheme = theme;
        applyThemeDefaults();
    }

    QStringList availableIcons() const
    {
        return m_lucide.availableIcons();
    }

private:
    IconManager()
    {
        if (!m_lucide.initLucide()) {
            qWarning() << "Failed to initialize QtLucide";
        }
        applyThemeDefaults();
    }

    void applyThemeDefaults();
    QVariantMap getThemeOptions(const QString& theme);

    lucide::QtLucide m_lucide;
    QString m_currentTheme = "default";
};

// Usage throughout the application
QIcon homeIcon = IconManager::instance().getIcon("home");
QIcon themedIcon = IconManager::instance().getThemedIcon("star", "dark");
```

### Factory Pattern

```cpp
class IconFactory
{
public:
    enum IconStyle {
        Default,
        Thin,
        Bold,
        Subtle
    };

    static QIcon createIcon(const QString& name, IconStyle style = Default,
                           const QColor& color = Qt::black)
    {
        QVariantMap options;
        options["color"] = color;

        switch (style) {
            case Thin:
                options["stroke-width"] = 1.5;  // Range: 0.5-4.0
                break;
            case Bold:
                options["stroke-width"] = 3.0;
                break;
            case Subtle:
                options["stroke-width"] = 1.5;
                options["opacity"] = 0.7;
                break;
            default:
                options["stroke-width"] = 2.0;  // Default
                break;
        }

        return getInstance().icon(name, options);
    }

    static QIcon createButtonIcon(const QString& name, const QColor& color = Qt::black)
    {
        QVariantMap options;
        options["color"] = color;
        options["scale-factor"] = 0.8;
        return getInstance().icon(name, options);
    }

    static QIcon createToolbarIcon(const QString& name, const QColor& color = Qt::black)
    {
        QVariantMap options;
        options["color"] = color;
        options["scale-factor"] = 0.9;
        return getInstance().icon(name, options);
    }

private:
    static lucide::QtLucide& getInstance()
    {
        static lucide::QtLucide instance;
        static bool initialized = false;
        if (!initialized) {
            instance.initLucide();
            initialized = true;
        }
        return instance;
    }
};
```

## Performance Monitoring

### Icon Usage Analytics

```cpp
class IconAnalytics
{
public:
    static void recordIconUsage(const QString& iconName)
    {
        m_usageCount[iconName]++;
        m_lastUsed[iconName] = QDateTime::currentDateTime();
    }

    static QStringList getMostUsedIcons(int count = 10)
    {
        QList<QPair<int, QString>> usage;
        for (auto it = m_usageCount.begin(); it != m_usageCount.end(); ++it) {
            usage.append({it.value(), it.key()});
        }

        std::sort(usage.begin(), usage.end(), std::greater<QPair<int, QString>>());

        QStringList result;
        for (int i = 0; i < qMin(count, usage.size()); ++i) {
            result.append(usage[i].second);
        }
        return result;
    }

    static void generateReport()
    {
        qDebug() << "Icon Usage Report:";
        qDebug() << "Total unique icons used:" << m_usageCount.size();
        qDebug() << "Most used icons:" << getMostUsedIcons(5);
    }

private:
    static QHash<QString, int> m_usageCount;
    static QHash<QString, QDateTime> m_lastUsed;
};
```

## Thread Safety Considerations

### Thread-Safe Icon Creation

```cpp
class ThreadSafeIconProvider
{
public:
    static QIcon getIcon(const QString& name, const QVariantMap& options = {})
    {
        QMutexLocker locker(&m_mutex);

        // Ensure QtLucide is initialized
        if (!m_initialized) {
            if (!m_lucide.initLucide()) {
                qWarning() << "Failed to initialize QtLucide";
                return QIcon();
            }
            m_initialized = true;
        }

        return m_lucide.icon(name, options);
    }

private:
    static lucide::QtLucide m_lucide;
    static QMutex m_mutex;
    static bool m_initialized;
};
```

## See Also

- [Basic Usage](basic-usage.md) - Learn the fundamentals
- [Icon Customization](customization.md) - Color and styling options
- [Performance Guide](performance.md) - Optimization strategies
- [API Reference](../api/icon-painter.md) - Custom painter API documentation
