# CHTL快速开始指南

## 快速构建

```bash
# 方法1: 使用Python构建脚本（推荐）
python3 build.py --clean --test

# 方法2: 使用CMake
mkdir build && cd build
cmake ..
make -j
```

## 运行编译器

```bash
# 查看版本
./build/bin/chtl-compiler --version

# 查看帮助
./build/bin/chtl-compiler --help

# 编译CHTL文件（当前为框架版本，完整实现正在进行中）
./build/bin/chtl-compiler input.chtl -o output.html
```

## 运行测试

```bash
# 使用构建脚本
python3 build.py --test

# 直接运行
./build/bin/chtl_tests

# 使用CTest
cd build && ctest --output-on-failure
```

## 项目结构

```
CHTL/
├── src/
│   ├── CHTL/              # CHTL编译器核心（72个源文件）
│   │   ├── CHTLContext    # 编译上下文
│   │   ├── CHTLLexer/     # 词法分析器 ✅
│   │   ├── CHTLParser/    # 语法分析器 ✅
│   │   ├── CHTLNode/      # AST节点系统 ✅
│   │   ├── CHTLGenerator/ # 代码生成器 ✅
│   │   ├── CHTLState/     # 状态机 ✅
│   │   ├── CHTLStrategy/  # 策略模式 ✅
│   │   └── CHTLLoader/    # 文件加载器 ✅
│   ├── CHTLJS/            # CHTL JS编译器（占位）
│   ├── SharedCore/        # 盐桥通信 ✅
│   ├── Util/              # 工具库 ✅
│   └── main.cpp           # 主程序 ✅
├── test/                  # 单元测试 ✅
├── build.py               # Python构建脚本 ✅
├── CMakeLists.txt         # CMake配置 ✅
└── .gitignore             # Git配置 ✅
```

## 当前实现状态

### ✅ 已完成
1. 完整的项目架构（状态机 + 策略模式）
2. CMake构建系统 + Python构建脚本
3. 基础词法分析器（Lexer + Token）
4. 完整的AST节点系统
5. 状态机和策略模式框架
6. 盐桥通信机制（CHTL ↔ CHTL JS）
7. 工具库（FileSystem, StringUtil, ZipUtil）
8. 单元测试框架（Catch2）
9. 版本控制配置

### 🚧 进行中
- 完整的Parser实现（基础框架已完成）
- 完整的Generator实现（基础框架已完成）
- CHTL.md中定义的高级语法特征

### 📋 待实现
- 局部样式块功能（属性运算、条件表达式等）
- 模板系统（@Style, @Element, @Var）
- 自定义系统（[Custom]）
- 原始嵌入（[Origin]）
- 导入系统（[Import]）
- 命名空间（[Namespace]）
- 配置组（[Configuration]）
- CHTL JS完整实现
- CMOD/CJMOD模块系统

## 架构特点

### 状态机模式
```
InitialState → ElementState → StyleBlockState
             → ScriptBlockState
             → TextBlockState
             → TemplateState
```

### 策略模式
```
ParseStrategy → ElementParseStrategy
              → StyleParseStrategy
              → ScriptParseStrategy
              → TextParseStrategy
              → TemplateParseStrategy
              → CustomParseStrategy
              → CommentParseStrategy

GenerationStrategy → HTMLGenerationStrategy
                   → CSSGenerationStrategy
                   → JSGenerationStrategy
```

### 盐桥机制
```
CHTL Compiler ←→ SaltBridge ←→ CHTL JS Compiler
     │                              │
     ├─ HTML/CSS处理                ├─ JavaScript处理
     ├─ 静态特征                    ├─ 动态特征
     └─ 样式块                      └─ 脚本块
```

## 开发指南

### 添加新功能

1. **添加Token类型** (如需要)
   ```cpp
   // src/CHTL/CHTLLexer/Token.h
   enum class TokenType {
       // ... 现有类型
       YOUR_NEW_TOKEN,
   };
   ```

2. **添加State** (如需要)
   ```cpp
   // src/CHTL/CHTLState/State.h
   class YourNewState : public State {
       // 实现必需的方法
   };
   ```

3. **添加Strategy** (如需要)
   ```cpp
   // src/CHTL/CHTLStrategy/Strategy.h
   class YourParseStrategy : public ParseStrategy {
       // 实现parse和canHandle方法
   };
   ```

4. **添加Node类型** (如需要)
   ```cpp
   // src/CHTL/CHTLNode/YourNode.h
   class YourNode : public BaseNode {
       // 实现必需的方法
   };
   ```

5. **添加测试**
   ```cpp
   // test/test_your_feature.cpp
   TEST_CASE("Your feature works", "[your-tag]") {
       // 测试代码
   }
   ```

### 编码规范

- C++17标准
- 使用智能指针（`std::shared_ptr`, `std::unique_ptr`）
- RAII原则
- 所有public方法都要有文档注释
- 新功能必须有单元测试

### 调试

```bash
# Debug模式构建
python3 build.py --debug

# 运行调试版本
./build/bin/chtl-compiler input.chtl --debug

# 使用gdb
gdb ./build/bin/chtl-compiler
```

## 常见问题

**Q: 构建失败怎么办？**
A: 
1. 确保使用`cd /workspace`回到根目录
2. 使用`python3 build.py --clean`清理后重新构建
3. 检查编译器是否支持C++17

**Q: 测试失败怎么办？**
A:
1. 查看具体的失败信息
2. 运行`./build/bin/chtl_tests -s`查看详细输出
3. 检查是否修改了已有功能导致回归

**Q: 如何添加新的语法特征？**
A: 
1. 先查阅CHTL.md了解语法规范
2. 按照"开发指南"的步骤添加
3. 确保不覆写已完成的功能
4. 使用TDD方式开发（先写测试）

## 下一步

1. **实现基础语法解析**: 元素、属性、文本、注释
2. **实现样式块功能**: 内联样式、类/ID自动化
3. **实现文本节点**: 字符串字面量、无修饰字面量
4. **完善Generator**: 正确生成HTML/CSS/JS
5. **增加更多测试**: 覆盖更多场景

## 文档

- `CHTL.md`: 完整语言规范
- `PROJECT_README.md`: 项目说明
- `PROJECT_STATUS.md`: 详细的状态报告
- `QUICK_START.md`: 本文档

## 许可证

MIT License

---

**当前版本**: v1.0.0-基础架构完成  
**构建状态**: ✅ 编译成功，测试通过  
**最后更新**: 2025年10月2日
