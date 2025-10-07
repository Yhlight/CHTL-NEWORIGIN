# Contributing to CHTL

Thank you for your interest in contributing to CHTL!

---

## 🎯 Development Principles

1. **Quality First**: Production-ready code only
2. **Test-Driven**: TDD approach required
3. **Zero Warnings**: Clean compilation mandatory  
4. **Documentation**: Code must be well-documented
5. **State Machine**: Follow architectural patterns

---

## 🔧 Getting Started

### Prerequisites
- C++17 compiler (Clang 10+ or GCC 9+)
- CMake 3.10+
- Python 3.6+

### Setup
```bash
git clone https://github.com/chtl-lang/chtl
cd chtl
python3 build.py configure
python3 build.py build
./build/chtl_tests
```

---

## 📝 Development Workflow

### 1. Create Feature Branch
```bash
git checkout -b feature/your-feature-name
```

### 2. Write Tests First (TDD)
```cpp
TEST_CASE("Your Feature", "[feature]") {
    SECTION("Basic functionality") {
        // Write failing test
        REQUIRE(feature.works() == true);
    }
}
```

### 3. Implement Feature
- Follow existing code style
- Add comprehensive comments
- Use state machine patterns where applicable

### 4. Run Tests
```bash
python3 build.py build
./build/chtl_tests
```

### 5. Ensure Quality
- ✅ All tests pass (98%+ coverage required)
- ✅ Zero compilation warnings
- ✅ Code documented
- ✅ Follows architectural patterns

### 6. Submit Pull Request
- Clear description
- Reference related issues
- Include test results

---

## 🏗️ Code Structure

```
src/
├── CHTL/              # Core CHTL implementation
│   ├── CHTLLexer/     # Lexical analysis
│   ├── CHTLParser/    # Syntax analysis  
│   ├── CHTLNode/      # AST nodes
│   ├── CHTLState/     # State machine
│   ├── CHTLTemplate/  # Template system
│   └── CHTLGenerator/ # Code generation
├── CHTL-JS/           # CHTL JS extension
│   ├── CHTLJSLexer/
│   ├── CHTLJSParser/
│   └── CHTLJSGenerator/
├── SharedCore/        # Salt Bridge
├── CLI/               # CLI tools
├── Plugins/           # Plugin system
└── Util/              # Utilities

tests/                 # Test suites
```

---

## 📋 Coding Standards

### C++ Style
- Use C++17 features
- Follow existing naming conventions
- Class names: `PascalCase`
- Functions: `camelCase`
- Variables: `camelCase`
- Constants: `UPPER_SNAKE_CASE`

### Comments
```cpp
// Brief description of what this does
void functionName() {
    // Implementation details
}
```

### Error Handling
```cpp
if (condition) {
    return std::nullopt;  // Use Optional for failure cases
}
```

---

## 🧪 Testing Guidelines

### Test Structure
```cpp
TEST_CASE("Feature Name", "[tag]") {
    SECTION("Specific scenario") {
        // Arrange
        Setup test data
        
        // Act
        Execute feature
        
        // Assert
        REQUIRE(result == expected);
    }
}
```

### Test Coverage
- Every feature must have tests
- Edge cases must be covered
- Integration tests for full pipeline
- Minimum 95% assertion pass rate

---

## 🐛 Bug Reports

### Include
1. CHTL code that reproduces the issue
2. Expected vs actual behavior
3. CHTL version
4. Compiler version
5. OS information

### Template
```markdown
**Description**: Brief description

**CHTL Code**:
```chtl
// Your code here
```

**Expected**: What should happen
**Actual**: What actually happens
**Version**: CHTL v2.0.0
**Compiler**: GCC 11.2
**OS**: Ubuntu 22.04
```

---

## 💡 Feature Requests

### Checklist
- [ ] Clear use case described
- [ ] Example CHTL code provided
- [ ] Backwards compatibility considered
- [ ] Performance impact assessed

---

## 🎨 Areas for Contribution

### High Priority
- [ ] Fix remaining test failures (Delegate, multi-event &->)
- [ ] Performance optimizations
- [ ] Documentation improvements
- [ ] Example projects

### Medium Priority
- [ ] VSCode extension
- [ ] Additional plugins
- [ ] More test scenarios
- [ ] Internationalization

### Future
- [ ] LSP implementation
- [ ] DAP implementation
- [ ] Browser DevTools extension
- [ ] Official component library

---

## 📞 Contact

- **Issues**: https://github.com/chtl-lang/chtl/issues
- **Discussions**: https://github.com/chtl-lang/chtl/discussions
- **Email**: dev@chtl.dev

---

## 🙏 Thank You

Every contribution helps make CHTL better!

**Let's build the future of HTML development together** 🚀
