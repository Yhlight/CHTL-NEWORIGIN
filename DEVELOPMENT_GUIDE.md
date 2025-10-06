# CHTL开发指南

## 🎯 如何继续开发CHTL

本指南为后续开发者提供清晰的开发路线和最佳实践。

---

## 🚦 优先级路线图

### 🔴 紧急且重要（立即开始）

#### 1. 完善增强选择器转换
**当前状态**: 已识别但未完全转换  
**需要做的**:
```cpp
// 在CHTLJSGenerator中实现完整的转换
String CHTLJSGenerator::processEnhancedSelector(const String& selector) {
    // 使用SaltBridge解析选择器
    auto info = bridge_.parseSelector(selector);
    return info.toJavaScript();
}
```

**测试**:
```chtl
script {
    {{.box}}->textContent = "Hello";
}
// 应生成: document.querySelector('.box').textContent = "Hello";
```

#### 2. 实现模板展开机制
**当前状态**: 模板可以定义但未展开  
**需要做的**:
1. 在Parser中收集所有模板定义
2. 在遇到模板使用时，展开模板内容
3. 处理模板继承

**文件**: `src/CHTL/CHTLParser/TemplateExpander.h/cpp`

#### 3. 完善属性运算
**当前状态**: ExpressionParser已实现但未集成到CSS生成  
**需要做的**:
1. 在parseAttributeValue时检测表达式
2. 如果包含运算符，使用ExpressionParser解析
3. 将结果替换到CSS中

**修改文件**: `src/CHTL/CHTLParser/CHTLParser.cpp`

### 🟡 重要但不紧急（本周完成）

#### 4. Listen语法完整实现
**文件**: 创建`src/CHTL-JS/CHTLJSParser/ListenParser.h/cpp`

**示例**:
```chtl
{{box}}->Listen {
    click: () => { ... },
    mouseenter: handler
};
```

**生成**:
```javascript
document.querySelector('.box').addEventListener('click', () => { ... });
document.querySelector('.box').addEventListener('mouseenter', handler);
```

#### 5. Delegate语法实现
**文件**: 创建`src/CHTL-JS/CHTLJSParser/DelegateParser.h/cpp`

**实现要点**:
- 事件委托到父元素
- 使用event.target判断
- 维护全局委托注册表

#### 6. & 引用完整解析
**当前状态**: SaltBridge已支持，但需要在生成时替换  
**需要做的**:
1. 在style块中，& 优先解析为class
2. 在script块中，& 优先解析为id
3. 生成时替换为实际的选择器

### 🟢 可以延后（后续迭代）

#### 7. 模块系统
- CMOD打包和加载
- CJMOD API实现
- 模块搜索路径

#### 8. 配置系统
- [Configuration]解析
- 自定义关键字
- 行为配置

#### 9. 导入系统
- [Import]完整实现
- 路径解析
- 命名空间管理

---

## 📖 开发流程

### 添加新特性的标准流程

#### 步骤1: 设计
1. 阅读CHTL.md相关语法
2. 设计AST节点（如需要）
3. 设计状态和策略（如需要）
4. 编写设计文档

#### 步骤2: 测试先行（TDD）
```cpp
TEST_CASE("New Feature - Basic", "[feature]") {
    String source = R"(
        // CHTL代码示例
    )";
    
    // 解析测试
    CHTLParser parser(source);
    auto ast = parser.parse();
    REQUIRE(!parser.hasErrors());
    
    // 生成测试
    String html = HtmlGenerator::generate(ast);
    REQUIRE(html.find("expected") != String::npos);
}
```

#### 步骤3: 实现
1. 更新Token类型（如需要）
2. 更新Lexer（如需要）
3. 更新Parser
4. 更新AST节点（如需要）
5. 更新Generator

#### 步骤4: 测试验证
```bash
python3 build.py test
```

#### 步骤5: 文档更新
1. 更新README.md
2. 添加示例到examples/
3. 更新NEXT_STEPS.md

---

## 🛠️ 常见开发任务

### 任务1: 添加新的HTML标签支持

**文件**: `src/CHTL/CHTLLexer/GlobalMap.cpp`

```cpp
void GlobalMap::initializeHtmlTags() {
    // 在这里添加新标签
    htmlTags.insert("new-tag");
}
```

### 任务2: 添加新的CHTL关键字

**文件**: 
1. `src/CHTL/CHTLLexer/Token.h` - 添加TokenType
2. `src/CHTL/CHTLLexer/GlobalMap.cpp` - 添加到keywordMap
3. `src/CHTL/CHTLParser/CHTLParser.cpp` - 添加解析逻辑

### 任务3: 添加新的AST节点

**文件**: `src/CHTL/CHTLNode/BaseNode.h`

```cpp
class NewNode : public BaseNode {
public:
    NewNode(const SourceRange& range);
    
    void accept(NodeVisitor& visitor) override;
    String toString(int indent = 0) const override;
    SharedPtr<BaseNode> clone() const override;
    
private:
    // 节点特有的数据
};
```

### 任务4: 添加新的生成逻辑

**文件**: `src/CHTL/CHTLGenerator/CHTLGenerator.cpp`

```cpp
void CHTLGenerator::visit(NewNode& node) {
    // 实现生成逻辑
    appendHtml("<new-element>");
    // ...
}
```

---

## 🔍 代码导航

### 核心文件位置

```
词法分析:
  src/CHTL/CHTLLexer/Lexer.h/cpp
  src/CHTL/CHTLLexer/Token.h/cpp
  src/CHTL/CHTLLexer/GlobalMap.h/cpp

语法分析:
  src/CHTL/CHTLParser/CHTLParser.h/cpp
  src/CHTL/CHTLParser/ExpressionParser.h/cpp

AST节点:
  src/CHTL/CHTLNode/BaseNode.h/cpp

状态机:
  src/CHTL/CHTLState/CHTLState.h/cpp

策略模式:
  src/CHTL/CHTLStrategy/CHTLStrategy.h

代码生成:
  src/CHTL/CHTLGenerator/CHTLGenerator.h/cpp

CHTL JS:
  src/CHTL-JS/CHTLJSLexer/CHTLJSLexer.h/cpp
  src/CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h/cpp

盐桥:
  src/SharedCore/SaltBridge.h/cpp

工具:
  src/Util/StringUtil/StringUtil.h/cpp
  src/Util/FileSystem/FileSystem.h/cpp
```

### 测试文件
```
tests/lexer_test.cpp - 所有测试（词法、语法、表达式）
```

---

## 🧪 测试指南

### 运行测试
```bash
# 运行所有测试
python3 build.py test

# 运行特定测试
./build/chtl_tests "[lexer]"
./build/chtl_tests "[parser]"
./build/chtl_tests "[expression]"

# 查看详细输出
./build/chtl_tests -s
```

### 添加新测试
```cpp
TEST_CASE("Feature - Description", "[tag]") {
    SECTION("Test scenario") {
        // Arrange
        String source = "...";
        
        // Act
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        // Assert
        REQUIRE(!parser.hasErrors());
        REQUIRE(ast->getChildren().size() == expected);
    }
}
```

---

## 🐛 调试技巧

### 1. 查看Token流
```bash
./build/chtl file.chtl --tokens
```

### 2. 查看AST
```bash
./build/chtl file.chtl --ast
```

### 3. 使用GDB调试
```bash
gdb ./build/chtl
(gdb) run examples/simple.chtl
(gdb) break CHTLParser::parseElement
(gdb) continue
```

### 4. 添加调试输出
```cpp
#ifdef DEBUG
    std::cout << "Debug: " << info << std::endl;
#endif
```

---

## 📋 代码检查清单

每次提交前检查：

- [ ] 代码编译通过
- [ ] 所有测试通过
- [ ] 没有内存泄漏
- [ ] 添加了必要的注释
- [ ] 更新了相关文档
- [ ] 遵循编码规范
- [ ] 没有提交构建产物

---

## 🎯 具体实现建议

### 建议1: 实现属性运算集成

**文件**: `src/CHTL/CHTLParser/CHTLParser.cpp`

```cpp
String CHTLParser::parseAttributeValue() {
    // 保存起始位置
    size_t startPos = current_;
    
    // 收集所有Token直到分隔符
    Vector<Token> exprTokens;
    while (!check(TokenType::Semicolon) && 
           !check(TokenType::Comma) && 
           !check(TokenType::RightBrace) && 
           !isAtEnd()) {
        exprTokens.push_back(advance());
    }
    
    // 检查是否包含运算符
    bool hasOperator = false;
    for (const auto& token : exprTokens) {
        if (token.isOperator()) {
            hasOperator = true;
            break;
        }
    }
    
    if (hasOperator) {
        // 使用ExpressionParser解析
        ExpressionParser exprParser(exprTokens);
        auto expr = exprParser.parse();
        return expr->evaluate();
    } else {
        // 简单值
        if (!exprTokens.empty()) {
            return exprTokens[0].getValue();
        }
        return "";
    }
}
```

### 建议2: 实现模板展开

**文件**: 创建`src/CHTL/CHTLParser/TemplateExpander.h/cpp`

```cpp
class TemplateExpander {
public:
    // 收集所有模板定义
    void collectTemplates(const SharedPtr<ProgramNode>& program);
    
    // 展开模板使用
    SharedPtr<BaseNode> expandTemplate(const String& name, const String& type);
    
private:
    HashMap<String, SharedPtr<TemplateNode>> styleTemplates_;
    HashMap<String, SharedPtr<TemplateNode>> elementTemplates_;
    HashMap<String, SharedPtr<TemplateNode>> varTemplates_;
};
```

### 建议3: 实现Listen语法

**文件**: 创建`src/CHTL-JS/CHTLJSParser/CHTLJSParser.h/cpp`

```cpp
class CHTLJSParser {
public:
    String parseListen(const String& target, const Vector<JSToken>& tokens);
    
private:
    struct EventHandler {
        String event;
        String handler;
    };
    
    Vector<EventHandler> parseEventHandlers();
};
```

---

## 🎓 学习资源

### 编译器理论
- 《编译原理》（龙书）
- 《现代编译器实现》
- LLVM教程

### C++学习
- 《Effective Modern C++》
- 《C++ Primer》
- CppReference.com

### 设计模式
- 《设计模式》（GoF）
- 《重构》
- 《代码整洁之道》

---

## 💬 贡献指南

### 如何贡献

1. Fork项目
2. 创建特性分支
3. 编写测试
4. 实现功能
5. 确保测试通过
6. 提交PR

### 代码规范

- 遵循Google C++编码规范
- 使用4空格缩进
- 类名使用PascalCase
- 函数名使用camelCase
- 常量使用UPPER_CASE

### 提交信息规范

```
[类型] 简短描述

详细描述（可选）

类型：
- feat: 新特性
- fix: 修复bug
- refactor: 重构
- test: 测试
- docs: 文档
```

---

## 🎯 项目完成的定义

### CHTL编译器1.0的标准

#### 必须完成 (Must Have)
- [x] 词法分析器
- [x] 语法分析器
- [x] 代码生成器
- [ ] 完整的CHTL语法支持
- [ ] 完整的CHTL JS支持
- [ ] 盐桥机制完善
- [ ] 模板系统完整
- [ ] 自定义系统完整

#### 应该完成 (Should Have)
- [ ] 模块系统（CMOD/CJMOD）
- [ ] 配置系统
- [ ] 导入和命名空间
- [ ] 错误恢复和警告
- [ ] 性能优化

#### 可以完成 (Could Have)
- [ ] VSCode扩展
- [ ] 官方模块库
- [ ] 在线编译器
- [ ] 文档网站

---

## 🚀 快速开始下一个特性

### 示例：实现Animate语法

#### 1. 阅读规范
查看CHTL.md第1524-1556行的Animate语法定义。

#### 2. 编写测试
```cpp
TEST_CASE("CHTL JS - Animate", "[chtljs]") {
    String source = R"(
        const anim = Animate {
            target: {{.box}},
            duration: 1000,
            begin: { opacity: 0 },
            end: { opacity: 1 }
        };
    )";
    
    // 测试解析和生成
}
```

#### 3. 实现解析
创建`AnimateParser.h/cpp`。

#### 4. 实现生成
在`CHTLJSGenerator`中添加`processAnimate`方法。

#### 5. 运行测试
```bash
python3 build.py test
```

#### 6. 创建示例
```chtl
// examples/animate_demo.chtl
```

---

## 📚 项目文档索引

| 文档 | 用途 | 读者 |
|------|------|------|
| CHTL.md | 语法规范 | 所有人 |
| README.md | 项目说明 | 新用户 |
| DEVELOPMENT_GUIDE.md | 开发指南 | 开发者 |
| NEXT_STEPS.md | 下一步计划 | 规划者 |
| PROJECT_STATUS.md | 进度报告 | 管理者 |
| MILESTONE_REPORT.md | 里程碑 | 所有人 |
| ACHIEVEMENTS.md | 成就总结 | 所有人 |

---

## 🎊 鼓励的话

你现在继承了一个**优秀的项目基础**：

- ✅ 清晰的架构
- ✅ 完善的测试
- ✅ 详细的文档
- ✅ 可工作的代码

**相信自己，继续前进！** 🚀

每一个新特性的实现，都会让CHTL变得更强大。

**保持代码质量，保持测试通过，保持文档更新。**

---

**Happy Coding!** 💻✨
