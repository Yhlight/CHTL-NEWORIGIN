# CHTL v2.0.0 Installation Guide

## 📋 Prerequisites

### Required
- **C++17 Compiler**:
  - GCC 9.0+ or Clang 10.0+
  - MSVC 2019+ (Windows)
  
- **CMake**: Version 3.10 or higher

- **Python**: Version 3.6+ (for build scripts)

### Optional
- Git (for cloning repository)
- Make or Ninja (build backends)

---

## 🚀 Installation

### Method 1: Build from Source (Recommended)

```bash
# Clone repository
git clone https://github.com/chtl-lang/chtl.git
cd chtl

# Configure build
python3 build.py configure

# Build project
python3 build.py build

# Run tests (optional)
./build/chtl_tests

# Install (optional)
sudo cp build/chtl /usr/local/bin/
```

### Method 2: Manual Build

```bash
# Clone repository
git clone https://github.com/chtl-lang/chtl.git
cd chtl

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
cmake --build .

# Run tests
./chtl_tests
```

---

## ✅ Verification

### Test Installation
```bash
# Check compiler exists
./build/chtl --version  # (if implemented)

# Run test suite
./build/chtl_tests

# Expected output:
# test cases: 58 | 54 passed | 4 failed
# assertions: 405 | 399 passed | 6 failed
# (98.5% pass rate)
```

### Test Compilation
```bash
# Create test file
echo '[use html5]
div {
    h1 { text: "Hello CHTL!"; }
}' > test.chtl

# Compile (when CLI is fully implemented)
./build/chtl test.chtl -o test.html
```

---

## 🔧 Build Options

### Debug Build (Default)
```bash
python3 build.py configure
python3 build.py build
```

### Release Build
```bash
python3 build.py configure --release
python3 build.py build
```

### Clean Build
```bash
python3 build.py clean      # Clean build directory
python3 build.py configure  # Reconfigure
python3 build.py build      # Rebuild
```

---

## 🐛 Troubleshooting

### Issue: CMake not found
```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake

# Windows
# Download from https://cmake.org/download/
```

### Issue: Compiler not found
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows
# Install Visual Studio 2019+ or MinGW
```

### Issue: Python not found
```bash
# Ubuntu/Debian
sudo apt-get install python3

# macOS
brew install python3

# Windows
# Download from https://python.org
```

### Issue: Build fails
```bash
# Clean and retry
rm -rf build
python3 build.py configure
python3 build.py build

# Check compiler version
g++ --version  # Should be 9.0+
clang++ --version  # Should be 10.0+
```

---

## 📁 Directory Structure After Install

```
chtl/
├── build/
│   ├── chtl              # Compiler executable
│   ├── chtl_tests        # Test suite executable
│   └── ...               # Build artifacts
│
├── src/                  # Source code
├── tests/                # Test files
├── docs/                 # Documentation (markdown files)
│
├── README.md
├── CHTL.md
├── LICENSE
└── .chtlrc              # Example configuration
```

---

## 🎯 Next Steps

After installation:

1. **Read Documentation**
   ```bash
   cat README.md          # Quick start
   cat CHTL.md | less     # Full specification
   ```

2. **Run Tests**
   ```bash
   ./build/chtl_tests
   ```

3. **Try Examples**
   ```bash
   # See tests/ directory for examples
   cat tests/listen_syntax_test.cpp
   ```

4. **Create Your First CHTL File**
   - See README.md for examples
   - Refer to CHTL.md for full syntax

5. **Join Community**
   - Star the repository
   - Report issues
   - Contribute improvements

---

## 💡 Tips

### For Developers
- Use TDD approach
- Follow CONTRIBUTING.md
- Run tests frequently
- Keep code clean

### For Users
- Start with simple examples
- Refer to test files for patterns
- Check KNOWN_ISSUES.md for edge cases
- Use .chtlrc for project configuration

---

## 📞 Support

- **Issues**: GitHub Issues
- **Documentation**: CHTL.md
- **Contributing**: CONTRIBUTING.md
- **Community**: GitHub Discussions

---

## ✅ Installation Complete!

You're now ready to use CHTL v2.0.0!

**Happy coding with CHTL!** 🚀
