# CHTL CLI 使用指南
**版本**: v2.6.0  
**更新日期**: 2025-10-07

---

## 📋 概述

CHTL CLI是CHTL语言的命令行编译工具，用于将`.chtl`文件编译为HTML/CSS/JS。

---

## 🚀 基础使用

### 安装
```bash
cd /workspace
python3 build.py all
```

### 编译CHTL文件
```bash
# 基础编译
./build/chtl input.chtl

# 输出: input.html, input.css (如果有CSS)
```

---

## 📖 命令行选项

### 基础选项

#### 帮助信息
```bash
chtl --help
chtl -h
```

#### 版本信息
```bash
chtl --version
chtl -v
```

#### 指定输出文件
```bash
chtl input.chtl -o output.html
```

---

## 🎨 输出选项

### 内联模式（根据CHTL.md 2367-2374）

#### 内联所有资源
```bash
chtl input.chtl --inline
```
**效果**: CSS和JS全部内联到HTML中

#### 仅内联CSS
```bash
chtl input.chtl --inline-css
```
**效果**: CSS内联，JS分离文件

#### 仅内联JS
```bash
chtl input.chtl --inline-js
```
**效果**: JS内联，CSS分离文件

### 分离文件模式（默认）

```bash
chtl input.chtl --separate
```
**效果**: 生成独立的HTML、CSS、JS文件

**注意**: 根据CHTL.md规范，默认情况下就是分离文件，`--separate`可省略

---

## 🏗️ 结构选项

### 默认HTML结构（根据CHTL.md 2362-2365）

```bash
chtl input.chtl --default-struct
```

**生成**:
```html
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>CHTL Page</title>
  <!-- CSS在这里 -->
</head>
<body>
  <!-- 你的CHTL内容 -->
  <!-- JS在这里 -->
</body>
</html>
```

**默认行为**: 不添加结构（支持SPA），只生成用户写的内容

---

## 🔍 调试选项

### 查看Tokens
```bash
chtl input.chtl --tokens
```
**输出**: 词法分析结果（不生成文件）

### 查看AST
```bash
chtl input.chtl --ast
```
**输出**: 语法分析树（不生成文件）

---

## 💡 使用示例

### 示例1：基础编译
```bash
chtl examples/simple.chtl
```
**生成**:
- `simple.html` - HTML结构
- `simple.css` - 样式文件（如果有）

### 示例2：内联所有资源
```bash
chtl examples/simple.chtl --inline
```
**生成**:
- `simple.html` - 包含内联CSS的单一文件

### 示例3：完整的HTML页面
```bash
chtl examples/simple.chtl --default-struct --inline
```
**生成**:
- `simple.html` - 完整的HTML5页面，CSS/JS全部内联

### 示例4：条件渲染编译
```bash
chtl examples/conditional_rendering_demo.chtl --inline --default-struct
```
**生成**:
- 包含条件渲染JavaScript的完整HTML页面

---

## 📂 输出文件说明

### 默认行为（无选项）
```
input.chtl  →  input.html + input.css + input.js
```

### --inline
```
input.chtl  →  input.html (CSS和JS内联)
```

### --inline-css
```
input.chtl  →  input.html (CSS内联) + input.js
```

### --inline-js
```
input.chtl  →  input.html (JS内联) + input.css
```

### --default-struct
```
input.chtl  →  完整的HTML5结构 + input.css + input.js
```

### --default-struct --inline
```
input.chtl  →  完整的HTML5结构（CSS/JS内联）
```

---

## ⚠️ 注意事项

### 1. 默认不添加结构
根据CHTL.md规范（2362-2365行）:
> CHTL编译器不应该提供默认的结构，而是根据用户写了什么内容，生成什么内容

这是为了更好地支持SPA单页应用。如需完整HTML结构，使用`--default-struct`。

### 2. 默认分离文件
根据CHTL.md规范（2368行）:
> 默认情况下，CHTL编译后将得到独立的HTML，CSS，JS文件

如需内联，使用`--inline`相关选项。

### 3. 文件覆盖
CLI会覆盖同名文件，使用时注意备份。

---

## 🎯 实际使用场景

### 开发调试
```bash
# 快速查看结果
chtl mypage.chtl --inline --default-struct
```

### 生产部署
```bash
# 生成分离文件（便于CDN缓存）
chtl mypage.chtl
```

### SPA页面
```bash
# 不添加默认结构
chtl components.chtl --inline
```

---

## 🔧 故障排除

### 问题：找不到输入文件
```
Error: File not found: input.chtl
```
**解决**: 检查文件路径是否正确

### 问题：解析错误
```
Parse errors:
  [Line 5, Column 10] Expected '{' after element name
```
**解决**: 检查CHTL语法是否正确

### 问题：无法写入输出文件
```
❌ Failed to write output file: output.html
```
**解决**: 检查目录权限，确保有写入权限

---

## 📊 支持的CHTL特性

CLI完全支持所有CHTL特性：

- ✅ 元素、属性、文本
- ✅ 局部样式块
- ✅ 模板和自定义
- ✅ 导入和命名空间
- ✅ CHTL JS全部特性
- ✅ 条件渲染（if/else if/else）
- ✅ 模块系统

---

## 🎓 学习资源

- **完整规范**: `CHTL.md`
- **示例代码**: `examples/` 目录
- **快速参考**: `QUICK_REFERENCE.md`

---

## 📝 命令速查表

```bash
# 帮助
chtl --help

# 编译（默认：分离文件，无结构）
chtl file.chtl

# 内联所有
chtl file.chtl --inline

# 完整HTML5页面
chtl file.chtl --default-struct --inline

# 自定义输出
chtl file.chtl -o custom.html

# 调试
chtl file.chtl --tokens
chtl file.chtl --ast
```

---

*CHTL CLI v2.6.0*  
*MIT License*
