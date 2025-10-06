# CHTL 编译器项目

CHTL是基于C++17实现的超文本语言编译器，提供了一种更符合开发者习惯的HTML编写方式。

## 项目状态

### ✅ 已完成功能

#### 1. 项目基础结构
- ✅ 完整的CMake构建系统
- ✅ Python构建脚本（避免路径问题）
- ✅ .gitignore版本控制配置
- ✅ 项目目录结构（遵循状态机+策略模式架构）

#### 2. 工具层（Util）
- ✅ **StringUtil**: 字符串处理工具
  - 去除空白、分割、拼接
  - 大小写转换
  - 字符串判断和替换
  - 转义处理
  
- ✅ **FileSystem**: 文件系统操作
  - 文件读写
  - 路径操作
  - 文件搜索

#### 3. CHTL词法分析器（CHTLLexer）
- ✅ **Token系统**: 完整的Token类型定义
  - 支持所有CHTL语法元素
  - 支持CHTL JS关键字
  - 详细的位置信息追踪
  
- ✅ **GlobalMap**: 关键字和HTML标签映射
  - HTML5所有标签支持
  - CHTL关键字识别
  - CHTL JS关键字识别
  
- ✅ **Lexer**: 词法分析器核心
  - 注释支持（//、/**/、#）
  - 字面量支持（字符串、数字、无修饰）
  - 关键字识别
  - 增强选择器 {{...}}
  - 完整的错误处理

#### 4. 测试系统
- ✅ 使用Catch2测试框架
- ✅ 完整的词法分析器测试用例
- ✅ 所有测试通过

### 🚧 进行中的工作

- 状态机架构（CHTLState）
- 策略模式架构（CHTLStrategy）
- 语法分析器（CHTLParser）
- AST节点系统（CHTLNode）

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

## 构建和运行

### 前置要求

- CMake 3.10+
- C++17兼容的编译器（GCC 7+, Clang 5+, MSVC 2017+）
- Python 3.6+（用于构建脚本）

### 构建步骤

```bash
# 使用Python构建脚本（推荐）
python3 build.py all

# 或者分步构建
python3 build.py configure
python3 build.py build
python3 build.py test

# 清理构建
python3 build.py clean
```

### 运行编译器

```bash
# 查看帮助
./build/chtl --help

# 显示Token列表
./build/chtl examples/simple.chtl --tokens

# 显示AST（待实现）
./build/chtl examples/simple.chtl --ast

# 编译CHTL文件
./build/chtl input.chtl -o output.html
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
