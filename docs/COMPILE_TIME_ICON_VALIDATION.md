# QtLucide 编译期图标验证

QtLucide 现在包含编译期图标验证功能，可以在构建时检查示例和测试代码中使用的图标名称是否有效。

## 功能特性

- ✅ **编译期检查**：在构建过程中自动验证图标名称
- ✅ **智能建议**：为无效图标名称提供相似的有效替代建议
- ✅ **构建集成**：与 Meson 构建系统完全集成
- ✅ **错误阻止**：无效图标名称会阻止构建完成
- ✅ **参考文档**：自动生成图标名称参考文档

## 工作原理

### 1. 自动验证
当构建示例时，验证工具会：
1. 从 `QtLucideStrings.h` 加载所有可用图标名称（1634个图标）
2. 扫描示例代码中的图标使用模式
3. 检查每个使用的图标名称是否存在
4. 如果发现无效图标，提供建议并阻止构建

### 2. 验证范围
目前验证以下文件中的图标使用：
- `examples/basic_usage/MainWindow.cpp`
- `examples/gallery/main.cpp`

验证工具专门查找这种模式：
```cpp
m_sampleIcons << "icon-name1" << "icon-name2" << "icon-name3";
```

### 3. 错误示例
如果你使用了无效的图标名称：
```cpp
m_sampleIcons << "edit" << "home" << "invalid-icon";
```

构建时会显示：
```
❌ Found 3 invalid icons:
  Line 32: 'edit'
    Suggestions: pencil, pen, square-pen
  Line 33: 'home'  
    Suggestions: house, house-plus
  Line 34: 'invalid-icon'
    Suggestions: tent, x
```

## 使用方法

### 手动运行验证
```bash
# 检查示例中的图标使用
python tools/check_example_icons.py .

# 运行完整的图标使用验证（包括测试）
python tools/validate_icon_usage.py .
```

### 构建时自动验证
使用 Meson 构建时会自动运行验证：
```bash
meson compile -C builddir_meson
```

如果验证失败，构建会停止并显示错误信息。

## 常见图标名称映射

| 期望名称 | 实际名称 | 备选名称 |
|---------|---------|---------|
| `edit` | `pencil` | `pen`, `square-pen` |
| `home` | `house` | `house-plus` |
| `delete` | `trash` | `trash-2` |
| `remove` | `x` | `minus`, `trash` |
| `add` | `plus` | `plus-circle` |
| `close` | `x` | `x-circle` |
| `options` | `settings` | `cog` |
| `config` | `settings` | `cog` |
| `preferences` | `settings` | `sliders-horizontal` |

## 生成的文件

验证工具会生成以下文件：
- `ICON_NAME_FIXES.md` - 包含常见映射和所有可用图标的完整列表

## 配置选项

### 禁用验证
如果需要临时禁用验证，可以在 `meson.build` 中注释掉相关部分：
```meson
# icon_validator = find_program('python3', required: false)
```

### 添加新的验证文件
要验证其他文件中的图标使用，修改 `tools/check_example_icons.py` 中的 `example_files` 列表：
```python
example_files = [
    examples_dir / "basic_usage" / "MainWindow.cpp",
    examples_dir / "gallery" / "main.cpp",
    examples_dir / "your_new_example" / "SomeFile.cpp",  # 添加新文件
]
```

## 故障排除

### 误报问题
如果验证工具误识别了非图标字符串，可以：
1. 检查 `tools/validate_icon_usage.py` 中的 `non_icon_words` 集合
2. 添加被误识别的字符串到排除列表

### 验证失败
如果验证意外失败：
1. 检查 `include/QtLucide/QtLucideStrings.h` 是否存在
2. 确认 Python3 可用
3. 查看构建日志中的详细错误信息

## 技术细节

### 验证工具
- `tools/check_example_icons.py` - 专门用于示例的精确验证
- `tools/validate_icon_usage.py` - 通用的图标使用验证工具

### 构建集成
验证通过 Meson 的 `custom_target` 集成：
```meson
icon_validation = custom_target('validate_icons',
  output: 'icon_validation_done.stamp',
  command: [icon_validator, icon_check_script, meson.current_source_dir(), '@OUTPUT@'],
  build_by_default: build_examples,
  console: true
)
```

### 依赖关系
示例构建依赖于图标验证的完成：
```meson
if is_variable('icon_validation_dep')
  example_deps += icon_validation_dep
endif
```

这确保了只有在图标验证通过后才会编译示例代码。
