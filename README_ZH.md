# CHTL - 超文本语言编译器

CHTL（Custom Hypertext Language）是一个基于C++20实现的现代化超文本语言编译器，旨在提供更符合开发者编写习惯的HTML代码方式。

## 特性

- 🚀 **现代化语法**: 使用更简洁、更直观的语法编写HTML
- 🎨 **局部样式块**: 在元素内部直接定义样式
- 📦 **模板系统**: 支持样式组模板、元素模板和变量组模板
- 🔧 **自定义系统**: 高度灵活的自定义组件系统
- 🔌 **模块化**: CMOD和CJMOD模块系统
- 🌐 **命名空间**: 有效防止模块污染
- ⚡ **C++20**: 使用现代C++特性实现高性能编译

## 快速开始

### 编译项目

```bash
mkdir build
cd build
cmake ..
make
```

### 使用编译器

```bash
# 基本用法
./chtl example.chtl

# 生成内联HTML
./chtl --inline example.chtl

# 生成带默认结构的HTML
./chtl --default-struct example.chtl

# 指定输出目录和文件名
./chtl -o output --output index example.chtl
```

### 示例代码

创建一个 `example.chtl` 文件：

```chtl
html {
    head {
        text { CHTL示例 }
    }
    
    body {
        div {
            id: container;
            class: main-content;
            
            style {
                width: 1200px;
                margin: 0 auto;
                padding: 20px;
            }
            
            text { 欢迎使用CHTL！ }
        }
    }
}
```

编译：

```bash
./chtl example.chtl
```

这将生成 `output.html` 文件。

## 项目结构

```
CHTL/
├── src/                    # 源代码
│   ├── CHTL/              # CHTL编译器核心
│   ├── CHTLJS/            # CHTL JS编译器
│   ├── Util/              # 工具类
│   └── main.cpp           # 主程序
├── include/               # 头文件
│   ├── CHTL/             # CHTL核心头文件
│   ├── Util/             # 工具类头文件
│   └── Common.hpp        # 公共定义
├── test/                  # 测试文件
├── examples/              # 示例文件
├── third-party/           # 第三方库
└── CMakeLists.txt        # CMake配置
```

## 核心组件

### 词法分析器 (Lexer)
- 将源代码分解为token流
- 支持CHTL的所有语法元素
- 识别注释、字符串、数字、标识符、操作符等

### 语法分析器 (Parser)
- 将token流解析为抽象语法树(AST)
- 支持元素、属性、样式、脚本等节点
- 处理嵌套结构和复杂语法

### 代码生成器 (Generator)
- 将AST转换为HTML/CSS/JS
- 支持内联和外部样式/脚本
- 优化输出格式

### 编译器 (Compiler)
- 统一的编译接口
- 文件I/O处理
- 错误和警告管理

## CHTL语法特性

### 元素节点
```chtl
div {
    id: box;
    class: container;
    
    text { 这是文本内容 }
}
```

### 局部样式块
```chtl
div {
    style {
        width: 100px;
        height: 200px;
        background-color: red;
    }
}
```

### 模板系统
```chtl
[Template] @Style DefaultText {
    color: black;
    line-height: 1.6;
}

div {
    style {
        @Style DefaultText;
    }
}
```

### 导入
```chtl
[Import] @Style from styles.css as myStyles;
[Import] @Chtl from components.chtl;
```

### 命名空间
```chtl
[Namespace] myNamespace {
    [Custom] @Element Box {
        div { }
    }
}
```

## 运行测试

```bash
cd build
make
./chtl_test
```

## 依赖

- C++20兼容编译器（GCC 10+, Clang 10+, MSVC 2019+）
- CMake 3.20+
- Catch2（已包含在third-party中）

## 开发路线图

- [x] CHTL核心编译器
- [x] 词法分析器
- [x] 语法分析器
- [x] HTML代码生成器
- [x] 基础测试框架
- [ ] CHTL JS编译器
- [ ] 盐桥通信机制
- [ ] CMOD/CJMOD模块系统
- [ ] CSS属性运算
- [ ] 条件表达式
- [ ] 响应式值
- [ ] 完整的模板和自定义系统

## 贡献

欢迎提交问题和拉取请求！

## 许可证

MIT License

## 作者

CHTL Team

---

更多信息请参考 [CHTL.md](CHTL.md) 完整规格文档。
