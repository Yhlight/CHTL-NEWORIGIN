# CHTL Implementation Plan - Conditional Rendering
*Date: 2025-10-07*
*Target: CHTL v2.6.0*

## 📋 Overview

Implementing **CHTL Committee 第三期 - 元素行为与条件渲染** (Element Behaviors and Conditional Rendering) as defined in CHTL.md lines 2403-2516.

## 🎯 Feature Scope

### Conditional Rendering (条件渲染)

#### 1. Static Conditional Rendering
```chtl
body
{
    if
    {
        condition: html.width < 500px,
        display: none,
    }
}
```

#### 2. Dynamic Conditional Rendering (CHTL JS)
```chtl
body
{
    if
    {
        condition: {{html}}->width < 500px,  // Dynamic with {{}}
        display: none,
    }
}
```

#### 3. Else If / Else Support
```chtl
div
{
    if
    {
        condition: width > 100px,
        display: block,
    }
    else if
    {
        condition: width > 50px,
        display: inline-block,
    }
    else
    {
        display: none,
    }
}
```

## 🏗️ Architecture Design

### Component Structure

```
CHTL/
├── CHTLNode/
│   ├── BaseNode.h/cpp           (existing)
│   ├── ConditionalNode.h/cpp     (NEW)
│   └── IfBlockNode.h/cpp         (NEW)
│
├── CHTLParser/
│   ├── CHTLParser.h/cpp         (modify)
│   └── ConditionalParser.h/cpp   (NEW)
│
├── CHTLState/
│   ├── CHTLState.h/cpp          (modify - add IfBlock state)
│   └── ConditionalState.h/cpp    (NEW)
│
├── CHTLStrategy/
│   ├── CHTLStrategy.h           (modify)
│   └── ConditionalStrategy.h    (NEW)
│
└── CHTLGenerator/
    └── CHTLGenerator.cpp        (modify - add if block generation)

CHTL-JS/
├── CHTLJSParser/
│   └── CHTLJSParser.cpp         (modify - detect dynamic conditions)
│
└── CHTLJSGenerator/
    └── CHTLJSGenerator.cpp      (modify - generate dynamic conditionals)

SharedCore/
└── SaltBridge.cpp               (modify - handle condition evaluation)
```

### State Machine Extension

```
Current State → [if keyword detected] → IfBlock State
  ↓
[condition: ...] → Parse Condition Expression
  ↓
[CSS properties] → Collect Conditional Styles
  ↓
[else if/else] → Chain Conditional Blocks
  ↓
Generate Output → Return to Parent State
```

### Strategy Pattern

```cpp
class ConditionalStrategy {
public:
    virtual ~ConditionalStrategy() = default;
    virtual String generateCondition(const ConditionNode& node) = 0;
};

class StaticConditionalStrategy : public ConditionalStrategy {
    // Generate CSS @media or inline conditions
};

class DynamicConditionalStrategy : public ConditionalStrategy {
    // Generate JavaScript event listeners for dynamic conditions
};
```

## 📝 Implementation Steps (TDD Approach)

### Phase 1: Foundation (Static Conditionals)

#### Step 1.1: Create Tests (RED)
```cpp
// tests/conditional_rendering_test.cpp
TEST_CASE("Static conditional rendering - basic if block") {
    String chtl = R"(
        div
        {
            if
            {
                condition: width > 100px,
                display: block,
            }
        }
    )";
    
    auto result = parse(chtl);
    REQUIRE(result.contains("@media"));
    REQUIRE(result.contains("min-width: 100px"));
}
```

#### Step 1.2: Implement Nodes (GREEN)
```cpp
// src/CHTL/CHTLNode/ConditionalNode.h
class ConditionalNode : public BaseNode {
public:
    String condition;           // e.g., "width > 100px"
    Vector<Pair<String, String>> styles;  // CSS properties
    Vector<SharedPtr<ConditionalNode>> elseIfBlocks;
    SharedPtr<ConditionalNode> elseBlock;
    bool isDynamic = false;     // Contains {{}} ?
};
```

#### Step 1.3: Implement Parser
```cpp
// src/CHTL/CHTLParser/ConditionalParser.cpp
SharedPtr<ConditionalNode> ConditionalParser::parseIfBlock(TokenStream& tokens) {
    auto node = makeShared<ConditionalNode>();
    
    // Expect 'if' keyword
    expectKeyword(tokens, "if");
    
    // Expect '{'
    expectToken(tokens, TokenType::LeftBrace);
    
    // Parse condition
    if (matchKeyword(tokens, "condition")) {
        expectToken(tokens, TokenType::Colon);
        node->condition = parseConditionExpression(tokens);
        expectToken(tokens, TokenType::Comma);
    }
    
    // Parse CSS properties
    while (!match(tokens, TokenType::RightBrace)) {
        auto [prop, value] = parseProperty(tokens);
        node->styles.push_back({prop, value});
    }
    
    // Check for else if / else
    if (matchKeyword(tokens, "else")) {
        if (matchKeyword(tokens, "if")) {
            node->elseIfBlocks.push_back(parseIfBlock(tokens));
        } else {
            node->elseBlock = parseElseBlock(tokens);
        }
    }
    
    return node;
}
```

#### Step 1.4: Implement Generator
```cpp
// src/CHTL/CHTLGenerator/CHTLGenerator.cpp
String CHTLGenerator::generateConditional(const ConditionalNode& node) {
    if (node.isDynamic) {
        return generateDynamicConditional(node);
    }
    
    // Static: use CSS @media or data attributes
    String condition = parseStaticCondition(node.condition);
    
    String output;
    if (isMediaQueryCondition(condition)) {
        output = "@media " + condition + " {\n";
        for (const auto& [prop, val] : node.styles) {
            output += "  " + prop + ": " + val + ";\n";
        }
        output += "}\n";
    }
    
    return output;
}
```

#### Step 1.5: Refactor (REFACTOR)
- Extract condition parsing to separate class
- Add validation for supported condition types
- Optimize string operations

### Phase 2: Dynamic Conditionals (CHTL JS Integration)

#### Step 2.1: Create Tests (RED)
```cpp
TEST_CASE("Dynamic conditional rendering - with {{}}") {
    String chtl = R"(
        div
        {
            id: container;
            if
            {
                condition: {{html}}->width < 500px,
                display: none,
            }
        }
    )";
    
    auto result = parse(chtl);
    REQUIRE(result.contains("addEventListener"));
    REQUIRE(result.contains("resize"));
}
```

#### Step 2.2: Salt Bridge Integration (GREEN)
```cpp
// src/SharedCore/SaltBridge.cpp
void SaltBridge::handleDynamicCondition(const ConditionalNode& node) {
    // Detect {{}} in condition
    if (containsEnhancedSelector(node.condition)) {
        // Pass to CHTL JS for processing
        requestCHTLJS("generateDynamicConditional", {
            {"condition", node.condition},
            {"styles", serializeStyles(node.styles)}
        });
    }
}
```

#### Step 2.3: CHTL JS Generator
```cpp
// src/CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.cpp
String CHTLJSGenerator::generateDynamicConditional(const ConditionalNode& node) {
    String selector = extractSelector(node.condition);  // {{html}}
    String expr = extractExpression(node.condition);    // width < 500px
    
    String output = "function updateConditionalStyles() {\n";
    output += "  const element = " + translateSelector(selector) + ";\n";
    output += "  if (" + translateExpression(expr) + ") {\n";
    
    for (const auto& [prop, val] : node.styles) {
        output += "    element.style." + camelCase(prop) + " = '" + val + "';\n";
    }
    
    output += "  }\n";
    output += "}\n";
    output += "window.addEventListener('resize', updateConditionalStyles);\n";
    output += "updateConditionalStyles();\n";
    
    return output;
}
```

### Phase 3: Else If / Else Support

#### Step 3.1: Create Tests (RED)
```cpp
TEST_CASE("Conditional rendering - else if and else") {
    String chtl = R"(
        div
        {
            if
            {
                condition: width > 100px,
                display: block,
            }
            else if
            {
                condition: width > 50px,
                display: inline-block,
            }
            else
            {
                display: none,
            }
        }
    )";
    
    auto result = parse(chtl);
    REQUIRE(result.contains("@media"));
    // Verify cascading conditions
}
```

#### Step 3.2: Implement Chain Logic (GREEN)
```cpp
String CHTLGenerator::generateConditionalChain(const ConditionalNode& node) {
    String output = generateIfBlock(node);
    
    for (const auto& elseIf : node.elseIfBlocks) {
        output += generateElseIfBlock(*elseIf);
    }
    
    if (node.elseBlock) {
        output += generateElseBlock(*node.elseBlock);
    }
    
    return output;
}
```

## 🧪 Test Coverage Plan

### Test Files to Create

1. **conditional_rendering_test.cpp** (Priority: HIGH)
   - [ ] Basic if block parsing
   - [ ] Condition expression validation
   - [ ] CSS property collection
   - [ ] Else if chain
   - [ ] Else block
   - [ ] Dynamic conditional with {{}}
   - [ ] Multiple conditions
   - [ ] Nested conditionals (if needed)

2. **conditional_integration_test.cpp** (Priority: MEDIUM)
   - [ ] Integration with elements
   - [ ] Integration with templates
   - [ ] Integration with custom elements
   - [ ] Salt bridge communication

### Expected Test Count
- **Minimum**: 20 test cases
- **Target**: 30 test cases  
- **Assertions**: 100+

## 📊 Success Criteria

### Functional Requirements
- ✅ Static `if {}` blocks work correctly
- ✅ `else if` and `else` chains work
- ✅ Dynamic conditionals with `{{}}` generate proper JS
- ✅ Condition expressions support all operators
- ✅ Integration with existing style system
- ✅ Salt bridge communication for dynamic features

### Quality Requirements
- ✅ 100% test pass rate
- ✅ Test coverage > 90%
- ✅ No regression in existing tests
- ✅ Follows state machine + strategy pattern
- ✅ Proper error handling

### Performance Requirements
- ✅ Parsing overhead < 5ms per if block
- ✅ Generation overhead < 10ms per condition
- ✅ No memory leaks

## 📅 Timeline

### Day 1: Foundation
- [x] Write comprehensive tests (conditional_rendering_test.cpp)
- [ ] Implement ConditionalNode and IfBlockNode
- [ ] Implement basic parser (if block only)
- [ ] Implement basic generator (static only)
- [ ] **Milestone**: Static if blocks working

### Day 2: Dynamic Support
- [ ] Implement dynamic condition detection
- [ ] Salt bridge integration
- [ ] CHTL JS generator for dynamic conditionals
- [ ] **Milestone**: Dynamic conditionals working

### Day 3: Complete Feature
- [ ] Implement else if / else support
- [ ] Add comprehensive error handling
- [ ] Optimize performance
- [ ] Write documentation
- [ ] **Milestone**: Feature complete

### Day 4: Polish & Integration
- [ ] Integration testing
- [ ] Bug fixes
- [ ] Code review and refactoring
- [ ] Update CHANGELOG
- [ ] **Milestone**: Ready for release

## 🔍 Edge Cases to Consider

1. **Invalid Conditions**
   - Non-existent properties
   - Invalid operators
   - Type mismatches

2. **Complex Scenarios**
   - Nested if blocks (if allowed)
   - Multiple if blocks in same element
   - If blocks in templates/customs

3. **Dynamic Edge Cases**
   - Missing {{}} elements
   - Circular dependencies
   - Performance with many listeners

## 📚 Documentation Needs

1. Update CHTL.md with examples
2. Create CONDITIONAL_RENDERING.md guide
3. Add to QUICK_REFERENCE.md
4. Update test documentation

## 🚀 Next Steps After Conditional Rendering

Once conditional rendering is complete:
1. **CLI Tool Implementation** - Basic command-line interface
2. **util...end Expression** - CHTL JS monitoring expressions
3. **INeverAway Completion** - Advanced function system
4. **VSCode Extension Foundation** - Syntax highlighting

---

*Plan created by: CHTL Development Team*  
*Status: Ready to implement*
