# CHTL Compiler Project

CHTL是基于C++语言实现的超文本语言编译器，提供更符合开发者编写HTML代码的方式。

## 项目特性

- **C++17标准**: 使用现代C++特性
- **CMake构建**: 跨平台构建系统
- **状态机 + 策略模式**: 严格的架构设计
- **TDD驱动开发**: 使用Catch2单元测试框架
- **盐桥机制**: CHTL与CHTL JS编译器通信桥梁

## 项目结构

```
CHTL/
├── src/
│   ├── CHTL/              # CHTL编译器
│   │   ├── CHTLContext    # 编译上下文
│   │   ├── CHTLLexer/     # 词法分析器
│   │   ├── CHTLParser/    # 语法分析器
│   │   ├── CHTLNode/      # AST节点
│   │   ├── CHTLGenerator/ # 代码生成器
│   │   ├── CHTLState/     # 状态机
│   │   ├── CHTLStrategy/  # 策略模式
│   │   └── CHTLLoader/    # 文件加载器
│   ├── CHTLJS/            # CHTL JS编译器
│   ├── SharedCore/        # 盐桥(CHTL与CHTL JS通信)
│   ├── Util/              # 工具库
│   └── main.cpp           # 主入口
├── test/                  # 单元测试
├── third-party/           # 第三方库
├── CMakeLists.txt         # CMake配置
├── build.py               # Python构建脚本
└── .gitignore             # Git忽略配置
```

## 构建说明

### 前置要求

- C++17兼容的编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+
- Python 3.6+ (用于构建脚本)

### 使用Python构建脚本 (推荐)

```bash
# 清理并构建
python3 build.py --clean

# Debug模式构建
python3 build.py --debug

# 构建并运行测试
python3 build.py --test

# 完整构建并安装
python3 build.py --clean --test --install
```

### 使用CMake直接构建

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## 运行编译器

```bash
# 编译CHTL文件
./build/bin/chtl-compiler input.chtl -o output.html

# 生成内联HTML
./build/bin/chtl-compiler input.chtl --inline -o output.html

# 启用调试模式
./build/bin/chtl-compiler input.chtl --debug
```

## 运行测试

```bash
# 使用构建脚本
python3 build.py --test

# 或直接运行
cd build
ctest --output-on-failure
```

## 架构说明

### 状态机模式

项目使用状态机管理解析过程的不同状态:
- `InitialState`: 初始状态
- `ElementState`: 元素解析状态
- `StyleBlockState`: 样式块状态
- `ScriptBlockState`: 脚本块状态
- `TextBlockState`: 文本块状态

### 策略模式

使用策略模式处理不同类型的语法元素:
- `ParseStrategy`: 解析策略基类
- `ElementParseStrategy`: 元素解析策略
- `StyleParseStrategy`: 样式解析策略
- `ScriptParseStrategy`: 脚本解析策略

### 盐桥机制

`SharedCore/SaltBridge` 提供CHTL与CHTL JS编译器之间的双向通信:
- CHTL处理HTML和CSS
- CHTL JS处理JavaScript和动态特性
- 通过消息传递机制协同工作

## 开发指南

### 添加新的语法特性

1. 在`Token.h`中添加新的Token类型
2. 在`Lexer.cpp`中实现词法识别
3. 创建对应的Node类 (继承自`BaseNode`)
4. 实现对应的State和Strategy
5. 添加单元测试
6. 更新生成器

### 编码规范

- 使用C++17特性
- 遵循Google C++ Style Guide
- 所有public接口都要有文档注释
- 新功能必须有对应的单元测试

## 许可证

MIT License

## 贡献

欢迎贡献! 请确保:
1. 代码通过所有测试
2. 遵循项目编码规范
3. 提交前运行linter
4. 添加必要的单元测试

## 联系方式

请查看CHTL.md了解完整的语言规范。
