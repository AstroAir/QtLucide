# Gallery应用日志系统

QtLucide Gallery应用现在包含了一个增强的日志系统，提供详细的调试信息和性能监控功能。

## 功能特性

### 🔍 多级日志记录

- **Debug**: 详细的调试信息
- **Info**: 一般信息消息
- **Warning**: 警告消息
- **Critical**: 严重错误消息

### 📂 分类日志

- `gallery.main`: 主应用程序流程
- `gallery.init`: 初始化过程
- `gallery.ui`: 用户界面操作
- `gallery.metadata`: 元数据加载和处理
- `gallery.icons`: 图标相关操作
- `gallery.search`: 搜索和过滤功能
- `gallery.export`: 导出功能
- `gallery.perf`: 性能监控

### ⏱️ 性能监控

- 操作计时器
- 内存使用监控
- 自动性能统计

### 📄 文件日志

- 自动日志文件创建
- 日志轮转（超过10MB时）
- 结构化日志格式

## 使用方法

### 命令行参数

```bash
# 启用调试日志
QtLucideGallery.exe --debug

# 启用详细日志
QtLucideGallery.exe --verbose

# 启用文件日志
QtLucideGallery.exe --log-file

# 使用系统主题
QtLucideGallery.exe --system-theme

# 组合使用
QtLucideGallery.exe --debug --log-file
```

### 日志文件位置

日志文件自动保存在：

- **Windows**: `%APPDATA%/QtLucide/gallery.log`
- **Linux**: `~/.local/share/QtLucide/gallery.log`
- **macOS**: `~/Library/Application Support/QtLucide/gallery.log`

### 日志格式

```
[时间戳] [级别] [分类] [线程ID] [位置] 消息内容
```

示例：

```
[2025-09-06 16:03:23.000] [INFO ] [gallery.main] [0x3b0d4] Gallery Logger initialized
[2025-09-06 16:03:23.004] [DEBUG] [gallery.init] [GalleryMainWindow.cpp:37] Window properties set
[2025-09-06 16:03:24.255] [WARN ] [gallery.metadata] Failed to load some metadata
```

## 开发者使用

### 在代码中添加日志

```cpp
#include "GalleryLogger.h"

// 基本日志记录
GALLERY_LOG_DEBUG(galleryUI, "Button clicked");
GALLERY_LOG_INFO(galleryMain, "Application started");
GALLERY_LOG_WARNING(galleryMetadata, "Metadata file not found");
GALLERY_LOG_CRITICAL(galleryInit, "Critical initialization error");

// 性能计时
GALLERY_START_TIMER("Database query");
// ... 执行操作 ...
GALLERY_END_TIMER("Database query");

// 内存使用监控
GALLERY_LOG_MEMORY("After loading icons");
```

### 日志分类指南

- **galleryMain**: 应用程序生命周期、启动、退出
- **galleryInit**: 组件初始化、配置加载
- **galleryUI**: 用户界面事件、窗口操作
- **galleryMetadata**: 元数据加载、解析、缓存
- **galleryIcons**: 图标加载、渲染、缓存
- **gallerySearch**: 搜索算法、过滤逻辑
- **galleryExport**: 文件导出、格式转换
- **galleryPerf**: 性能测量、内存监控

## 调试技巧

### 1. 启动问题调试

```bash
QtLucideGallery.exe --debug --log-file
```

检查日志文件中的初始化过程。

### 2. 性能问题分析

查找日志中的性能计时信息：

```
[INFO ] [gallery.perf] Timer 'Metadata loading' completed in 1250ms
[INFO ] [gallery.perf] Memory usage (After loading): 45MB
```

### 3. 搜索功能调试

启用搜索分类日志：

```cpp
logger->enableCategory("gallery.search", true);
```

### 4. 内存泄漏检测

定期检查内存使用情况：

```cpp
GALLERY_LOG_MEMORY("Before operation");
// ... 执行操作 ...
GALLERY_LOG_MEMORY("After operation");
```

## 配置选项

### 程序化配置

```cpp
GalleryLogger* logger = GalleryLogger::instance();

// 设置日志级别
logger->setLogLevel(GalleryLogger::Debug);

// 启用特定分类
logger->enableCategory("gallery.metadata", true);
logger->enableCategory("gallery.perf", false);

// 清空日志文件
logger->clearLogFile();

// 手动轮转日志
logger->rotateLogFile();
```

## 故障排除

### 常见问题

1. **日志文件未创建**
   - 检查应用程序是否有写入权限
   - 确认使用了 `--log-file` 参数

2. **日志信息过多**
   - 使用更高的日志级别（Info或Warning）
   - 禁用不需要的分类

3. **性能影响**
   - 在发布版本中禁用Debug级别日志
   - 考虑只启用关键分类的日志

### 日志分析工具

推荐使用以下工具分析日志：

- **Windows**: Notepad++, VS Code
- **Linux**: grep, awk, less
- **跨平台**: LogExpert, Glogg

## 示例用法

### 调试应用启动问题

```bash
QtLucideGallery.exe --debug --log-file > startup.log 2>&1
```

### 监控特定功能

```cpp
// 在搜索功能中
GALLERY_START_TIMER("Icon search");
GALLERY_LOG_DEBUG(gallerySearch, QString("Searching for: %1").arg(searchTerm));
// ... 搜索逻辑 ...
GALLERY_LOG_INFO(gallerySearch, QString("Found %1 results").arg(results.size()));
GALLERY_END_TIMER("Icon search");
```

这个增强的日志系统将大大提高Gallery应用的可调试性和可维护性。
