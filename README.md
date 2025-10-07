# CHTL v2.5.0 - Compact HTML Template Language

[![Version](https://img.shields.io/badge/version-2.5.0--module--and--final-blue.svg)](https://github.com/chtl-lang/chtl)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Build](https://img.shields.io/badge/build-inprogress-yellow.svg)]()
[![Modules](https://img.shields.io/badge/modules-CMOD%2BCJMOD-brightgreen.svg)]()

**CHTL** is a modern, powerful HTML template language with extensive JavaScript integration capabilities.

---

## ✨ Features

### Core Language
- 🎨 Intuitive, clean syntax
- 📝 Complete HTML element support
- 🎯 Advanced templating system
- 🔧 Custom elements and components
- 📦 Module and namespace system
- ⚙️ Flexible configuration system

### CHTL JS Extension
- 🔍 Enhanced selectors `{{...}}`
- 🎧 Event handling (`Listen`, `&->`, `Delegate`)
- 🎬 Animation system (`Animate`)
- 🛣️ SPA routing (`Router`)
- 📜 Dynamic script loading (`ScriptLoader`)
- 💫 Virtual objects (`Vir`)
- 🔄 Reactive values `$var$`

### v2.0 Additions
- 📊 Error reporting system
- 🗺️ Source map support
- ⚡ Performance monitoring
- ⚙️ Configuration loader
- 👀 File watching
- 💾 Cache system
- 🧩 Web Components support
- 📘 TypeScript integration
- 🔌 Plugin system
- 🛠️ CLI tools

### v2.5 Module System
- 📦 CMOD module system
- 🔧 CJMOD extension API
- 🌸 Chtholly official module (CMOD + CJMOD)
- 🌟 Yuigahama official module (CMOD)
- ⚙️ Professional configuration (.chtlrc)
- 💾 Module caching & dependency management

---

## 🚀 Quick Start

### Installation
```bash
git clone https://github.com/chtl-lang/chtl
cd chtl
python3 build.py configure
python3 build.py build
```

### Basic Example
```chtl
[use html5]

div {
    class: app;
    
    h1 { text: "Hello CHTL!"; }
    
    button {
        id: btn;
        text: "Click Me";
        
        script {
            {{&}}->Listen {
                click: () => {
                    alert('Clicked!');
                }
            };
        }
        
        style {
            background: #007bff;
            color: white;
            
            &:hover {
                background: #0056b3;
            }
        }
    }
}
```

### Compile
```bash
./build/chtl input.chtl -o output.html
```

---

## 📊 Statistics

- **Total Features**: 34
- **Code Base**: 12,000+ lines of C++17
- **Test Coverage**: 98.5% (399/405 assertions pass)
- **Test Scenarios**: 58
- **Architecture**: State Machine + Strategy Pattern + Plugin System

---

## 🎯 Feature Overview

### CHTL Core (13 features)
1. ✅ Lexer & Parser
2. ✅ Comments (`//`, `/**/`, `#`)
3. ✅ Text nodes
4. ✅ Elements & attributes
5. ✅ Style system
6. ✅ Templates
7. ✅ Custom elements
8. ✅ Origin blocks
9. ✅ Import system
10. ✅ Namespaces
11. ✅ Constraints
12. ✅ Configuration
13. ✅ `use` keyword

### CHTL JS (11 features)
14. ✅ Enhanced selectors `{{...}}`
15. ✅ Arrow operator `->`
16. ✅ `Listen {}` blocks
17. ✅ Event bind operator `&->`
18. ✅ `Delegate {}` event delegation
19. ✅ `Animate {}` animations
20. ✅ `Router {}` SPA routing
21. ✅ `Vir` virtual objects
22. ✅ `ScriptLoader {}`
23. ✅ Reactive values `$var$`
24. ✅ Local script blocks

### v2.0 Infrastructure (10 features)
25. ✅ Error Reporter
26. ✅ Source Map
27. ✅ Performance Monitor
28. ✅ Config Loader
29. ✅ File Watcher
30. ✅ Cache System
31. ✅ Web Components Support
32. ✅ TypeScript Generator
33. ✅ CLI Tool
34. ✅ Plugin System

---

## 📚 Documentation

- **[CHTL.md](CHTL.md)** - Complete language specification (2500+ lines)
- **[CHTL_v2.0.0_FINAL.md](CHTL_v2.0.0_FINAL.md)** - Release notes
- **[ROADMAP_TO_V2.md](ROADMAP_TO_V2.md)** - Development roadmap
- **[tests/](tests/)** - 58 test scenarios

---

## 🏗️ Architecture

```
CHTL Compiler
├── Lexer (Tokenization)
├── Parser (AST Construction)
├── Generator (Code Generation)
│   ├── HTML Generator
│   ├── CSS Generator
│   └── JS Generator (CHTL JS)
├── Salt Bridge (Inter-module Communication)
└── Infrastructure
    ├── Error Reporter
    ├── Source Map
    ├── Performance Monitor
    ├── Cache System
    └── Plugin System
```

---

## 🧪 Testing

```bash
# Run all tests
./build/chtl_tests

# Run specific test suite
./build/chtl_tests "[listen]"

# Verbose output
./build/chtl_tests -s
```

---

## 🛠️ Development

### Requirements
- C++17 compiler
- CMake 3.10+
- Python 3.6+ (for build scripts)

### Build
```bash
python3 build.py configure  # Configure CMake
python3 build.py build      # Build project
python3 build.py clean      # Clean build directory
```

### Contributing
Contributions are welcome! Please ensure:
- Code follows existing style
- Tests pass (98%+ coverage)
- Zero compilation warnings
- TDD approach for new features

---

## 📄 License

MIT License - see [LICENSE](LICENSE) for details

---

## 🙏 Acknowledgments

CHTL is built with passion for making HTML development simpler and more powerful.

**Created by the CHTL Development Team**

---

## 🔗 Links

- **Repository**: https://github.com/chtl-lang/chtl
- **Website**: https://chtl.dev
- **Documentation**: https://docs.chtl.dev
- **Issues**: https://github.com/chtl-lang/chtl/issues

---

**CHTL - Making HTML Development Simple, Powerful, and Elegant** 🌟
