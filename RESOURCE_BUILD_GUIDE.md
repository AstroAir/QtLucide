# QtLucide 资源构建指南

本指南说明如何使用自动化脚本来构建QtLucide库，确保所有SVG图标都嵌入到编译的库中。

## 概述

QtLucide现在提供了完全自动化的资源构建系统，可以：
- 自动扫描所有SVG文件
- 生成Qt资源文件(.qrc)
- 生成C++头文件和枚举
- 在编译时自动嵌入所有SVG到库中

## 快速开始

### 方法1：使用便捷脚本（推荐）

**Linux/macOS:**
```bash
python3 build_all_resources.py
```

**Windows:**
```cmd
build_resources.bat
```
或
```cmd
python build_all_resources.py
```

### 方法2：直接使用构建工具
```bash
python3 tools/build_resources.py
```

### 方法3：通过CMake自动构建
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

CMake会自动检测SVG文件变化并重新生成资源。

## 添加新的SVG图标

1. 将新的SVG文件放入 `resources/icons/svg/` 目录
2. 运行资源构建脚本：
   ```bash
   python3 build_all_resources.py
   ```
3. 重新编译库：
   ```bash
   cmake --build build
   ```

## 生成的文件

资源构建过程会生成以下文件：

### 元数据文件
- `resources/icons/metadata/icons.json` - 图标元数据
- `resources/icons/metadata/categories.json` - 图标分类
- `resources/icons/metadata/tags.json` - 图标标签

### Qt资源文件
- `resources/icons/lucide_icons.qrc` - Qt资源文件

### C++头文件
- `include/QtLucide/QtLucideEnums.h` - 图标枚举定义
- `include/QtLucide/QtLucideStrings.h` - 字符串映射

## 在代码中使用图标

生成的头文件提供了类型安全的图标访问：

```cpp
#include <QtLucide/QtLucide.h>
#include <QtLucide/QtLucideEnums.h>

// 使用枚举
QIcon icon = QtLucide::icon(lucide::Icons::home);

// 使用字符串名称
QIcon icon2 = QtLucide::icon("home");

// 获取所有可用图标数量
int count = lucide::ICON_COUNT;
```

## 构建系统集成

### CMake集成
CMake配置已经设置了自动依赖关系：
- 当任何SVG文件发生变化时，会自动重新生成资源
- 构建库之前会确保所有资源都是最新的

### 手动触发资源重建
如果需要强制重新生成资源：
```bash
# 删除生成的文件
rm resources/icons/lucide_icons.qrc
rm resources/icons/metadata/icons.json

# 重新构建
cmake --build build
```

## 故障排除

### Python未找到
确保Python 3.6+已安装并在PATH中：
```bash
python3 --version
```

### SVG文件未包含
检查SVG文件是否在正确的目录：
```
resources/icons/svg/your-icon.svg
```

### 构建失败
检查构建日志中的错误信息：
```bash
cmake --build build --verbose
```

## 自定义配置

### 修改图标分类
编辑 `tools/build_resources.py` 中的 `_categorize_icon` 方法来自定义图标分类逻辑。

### 修改生成的头文件
编辑 `tools/generate_headers.py` 来自定义生成的C++代码。

## 性能说明

- 所有SVG文件都嵌入到编译的库中，无需外部文件
- 图标在运行时从内存加载，性能优异
- 支持Qt的图标缓存机制

## 版本兼容性

- 需要Qt 6.0+
- 需要Python 3.6+
- 需要CMake 3.16+

## 贡献

如果您添加了新的图标或改进了构建脚本，请确保：
1. 运行完整的资源构建测试
2. 验证生成的代码编译无误
3. 更新相关文档
