# CHTL 编译器项目

CHTL是基于C++17实现的超文本语言编译器，提供了一种更符合开发者习惯的HTML编写方式。

## 项目状态

### ✅ 已完成功能（v0.2.0-alpha）

#### 1. 完整的编译流程 ✅
- ✅ 词法分析器（Lexer）- 70+种Token类型
- ✅ 语法分析器（Parser）- 完整的CHTL语法解析
- ✅ AST节点系统 - 11种节点类型
- ✅ 代码生成器（Generator）- HTML/CSS生成
- ✅ 状态机架构 - 状态驱动的解析
- ✅ 策略模式架构 - 可插拔的策略系统

#### 2. 工具层（Util）
- ✅ **StringUtil**: 17个字符串处理方法
- ✅ **FileSystem**: 15个文件系统操作方法

#### 3. CHTL词法分析器（CHTLLexer）
- ✅ **Token系统**: 70+种Token类型，完整支持CHTL语法
- ✅ **GlobalMap**: 60+个HTML5标签，30+个CHTL关键字
- ✅ **Lexer**: 完整的词法分析器实现

#### 4. CHTL语法分析器（CHTLParser）
- ✅ **Parser**: 状态机驱动的语法分析
- ✅ **State**: 9种解析状态
- ✅ **Strategy**: 策略模式支持
- ✅ **错误处理**: 详细的错误信息和恢复机制

#### 5. AST节点系统（CHTLNode）
- ✅ **BaseNode**: 节点基类
- ✅ **ElementNode**: 元素节点（支持属性）
- ✅ **TextNode**: 文本节点
- ✅ **StyleNode**: 样式节点（内联+CSS规则）
- ✅ **ScriptNode**: 脚本节点
- ✅ **CommentNode**: 注释节点
- ✅ **TemplateNode**: 模板节点
- ✅ **CustomNode**: 自定义节点
- ✅ **OriginNode**: 原始嵌入节点
- ✅ **ImportNode**: 导入节点
- ✅ **ProgramNode**: 程序根节点

#### 6. 代码生成器（CHTLGenerator）
- ✅ **HtmlGenerator**: HTML代码生成
- ✅ **CssGenerator**: CSS代码生成
- ✅ **JsGenerator**: JavaScript代码收集
- ✅ **美化输出**: 可配置的格式化选项

#### 7. 测试系统
- ✅ Catch2测试框架集成
- ✅ 10个测试用例，100%通过
- ✅ 词法分析器测试
- ✅ 语法分析器测试

#### 8. CLI工具
- ✅ 完整的命令行界面
- ✅ --tokens选项（显示Token）
- ✅ --ast选项（显示AST）
- ✅ -o选项（指定输出文件）

### 🎯 支持的CHTL语法

- ✅ HTML元素（所有HTML5标签）
- ✅ 属性定义（class、id等）
- ✅ 文本节点（text关键字和属性）
- ✅ 注释（//、/**/、#）
- ✅ 局部样式块（style {}）
- ✅ CSS选择器（.class、#id）
- ✅ 脚本块（script {}）
- ✅ 模板（[Template] @Style/@Element/@Var）
- ✅ 自定义（[Custom] @Style/@Element/@Var）
- ✅ 原始嵌入（[Origin] @Html/@Style/@JavaScript）

### 📋 待实现功能

1. **CHTL核心**
   - [ ] 语法分析器（Parser）
   - [ ] AST节点系统
   - [ ] 代码生成器（Generator）
   - [ ] 上下文管理（Context）
   - [ ] 模块加载器（Loader）
   - [ ] CMOD系统

2. **CHTL JS**
   - [ ] CHTL JS词法分析器
   - [ ] CHTL JS语法分析器
   - [ ] CHTL JS代码生成器
   - [ ] CJMOD系统

3. **盐桥机制**
   - [ ] SharedCore通信桥梁
   - [ ] CHTL与CHTL JS数据交换

4. **外部编译器集成**
   - [ ] CSS编译器集成
   - [ ] JavaScript编译器集成

## 快速开始

### 前置要求

- CMake 3.10+
- C++17兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）
- Python 3.6+（用于构建脚本）

### 构建步骤

```bash
# 一键构建（推荐）
python3 build.py all

# 或者分步构建
python3 build.py clean       # 清理
python3 build.py configure   # 配置
python3 build.py build       # 编译
python3 build.py test        # 测试
```

### 使用编译器

```bash
# 编译CHTL文件到HTML
./build/chtl examples/simple.chtl

# 指定输出文件
./build/chtl input.chtl -o output.html

# 查看Token列表
./build/chtl examples/simple.chtl --tokens

# 查看AST
./build/chtl examples/simple.chtl --ast

# 查看帮助
./build/chtl --help
```

### 示例

**输入 (simple.chtl):**
```chtl
html {
    body {
        div {
            class: container;
            
            style {
                .container {
                    width: 100%;
                    max-width: 1200px;
                }
            }
            
            text: "Hello, CHTL!";
        }
    }
}
```

**输出 (simple.html):**
```html
<!DOCTYPE html>
<html>
  <head>
    <style>
.container {
  width: 100%; 
  max-width: 1200px;
}
    </style>
  </head>
  <body>
    <div class="container">Hello, CHTL!</div>
  </body>
</html>
```

## 项目架构

项目严格遵循**状态机 + 策略模式**驱动的架构：

```
src/
├── Common.h                      # 公共类型定义
├── main.cpp                      # 主程序入口
├── Util/                         # 工具层
│   ├── StringUtil/              # 字符串工具
│   ├── FileSystem/              # 文件系统工具
│   └── ZipUtil/                 # ZIP工具（待实现）
├── CHTL/                        # CHTL编译器
│   ├── CHTLLexer/              # 词法分析器 ✅
│   ├── CHTLParser/             # 语法分析器
│   ├── CHTLNode/               # AST节点
│   ├── CHTLState/              # 状态机
│   ├── CHTLStrategy/           # 策略模式
│   ├── CHTLGenerator/          # 代码生成器
│   ├── CHTLContext/            # 上下文管理
│   ├── CHTLLoader/             # 模块加载器
│   └── CMODSystem/             # CMOD系统
├── CHTL-JS/                     # CHTL JS编译器
│   ├── CHTLJSLexer/            # 词法分析器
│   ├── CHTLJSParser/           # 语法分析器
│   ├── CHTLJSNode/             # AST节点
│   ├── CHTLJSState/            # 状态机
│   ├── CHTLJSStrategy/         # 策略模式
│   ├── CHTLJSGenerator/        # 代码生成器
│   ├── CHTLJSContext/          # 上下文管理
│   ├── CHTLJSLoader/           # 模块加载器
│   └── CJMODSystem/            # CJMOD系统
└── SharedCore/                  # 盐桥机制
    └── 待实现
```

## 开发规范

### TDD原则

项目使用测试驱动开发（TDD）：
1. 先编写测试用例
2. 实现功能使测试通过
3. 重构代码优化
4. 确保测试始终通过

### 代码风格

- 使用C++17标准
- 遵循Google C++代码风格
- 使用智能指针管理内存
- 避免使用原始指针

### 提交规范

- 不提交构建产物
- 不提交二进制文件
- 每次提交前运行测试
- 编写清晰的提交信息

## CHTL语法特性

详见 [CHTL.md](CHTL.md) 文档，包括：

- 注释（//、/**/、#）
- 文本节点
- 字面量（无修饰、带引号）
- CE对等式（: = Equal）
- 元素节点
- 属性
- 局部样式块
- 模板和自定义
- 原始嵌入
- 导入和命名空间
- CHTL JS扩展语法

## 测试

```bash
# 运行所有测试
python3 build.py test

# 或直接运行测试程序
./build/chtl_tests

# 查看详细测试输出
./build/chtl_tests -s
```

当前测试覆盖：
- ✅ 词法分析器基础Token
- ✅ 字面量识别
- ✅ 关键字识别
- ✅ 注释处理
- ✅ CHTL结构解析
- ✅ 增强选择器

## 许可证

MIT License

## 致谢

本项目使用了以下第三方库：
- Catch2 - 测试框架
- miniz - ZIP文件处理

---

**当前版本**: v0.1.0-alpha  
**最后更新**: 2025-10-06
