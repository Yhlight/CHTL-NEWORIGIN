# CHTL Build Documentation

## Overview

CHTL (C++ HyperText Language) is a comprehensive compiler system that transforms CHTL source code into HTML, CSS, and JavaScript. The project uses a state machine + strategy pattern architecture with CMake for building.

## Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.20 or higher
- Make or Ninja build system

## Building

### Standard Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Build with Options

```bash
cd build
cmake .. -DCHTL_BUILD_CLI=ON -DCHTL_BUILD_TESTS=ON -DCHTL_ENABLE_SANITIZERS=OFF
cmake --build .
```

### Path Issues

If you encounter path issues with CMake, use the following command from the project root:

```bash
cd /app
cmake -S . -B build
cmake --build build
```

## Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `CHTL_BUILD_CLI` | ON | Build CLI tools |
| `CHTL_BUILD_TESTS` | ON | Build test suite |
| `CHTL_ENABLE_SANITIZERS` | OFF | Enable address/undefined sanitizers |
| `CHTL_STRICT_WARNINGS` | ON | Enable strict compiler warnings |
| `CHTL_WITH_ANTLR` | OFF | Enable ANTLR-based CSS/JS compilers |
| `CHTL_DEBUG_MODE` | OFF | Enable debug mode |

## Project Structure

```
CHTL/
├── CMakeLists.txt              # Main CMake configuration
├── Util/                       # Utility libraries
│   ├── FileSystem/            # File system operations
│   ├── StringUtil/            # String utilities
│   └── ZipUtil/               # ZIP archive handling
├── Scanner/                    # Unified Scanner
│   ├── StateMachine.h         # State machine framework
│   ├── Strategy.h             # Strategy pattern framework
│   └── UnifiedScanner.h       # Main scanner implementation
├── CHTL/                      # CHTL Compiler
│   ├── CHTLLexer/             # Token system and lexer
│   ├── CHTLNode/              # AST node system
│   ├── CHTLParser/            # Parser implementation
│   ├── CHTLGenerator/         # Code generator
│   └── ...                    # Other CHTL components
├── CHTL_JS/                   # CHTL JS Compiler
├── CompilerDispatcher/        # Compiler coordination
├── CodeMerger/               # Output merging
├── CLI/                      # Command-line interface
└── Test/                     # Test suite
```

## Architecture

### State Machine + Strategy Pattern

All components use a unified state machine framework with strategy pattern for extensibility:

- **State Machine**: Manages component lifecycle and transitions
- **Strategy Pattern**: Provides pluggable algorithms and behaviors
- **Visitor Pattern**: Enables AST traversal and transformation

### Component Interaction

1. **Unified Scanner**: Separates CHTL, CHTL JS, CSS, and JS code using placeholder mechanism
2. **Compiler Dispatcher**: Coordinates parallel compilation of different language fragments
3. **CHTL Compiler**: Handles CHTL syntax, templates, customizations, imports, namespaces
4. **CHTL JS Compiler**: Processes CHTL JS syntax with virtual objects, selectors, and CJMOD API
5. **Code Merger**: Combines outputs with inline options and placeholder decoding

## Usage

### Basic Compilation

```bash
./chtl compile input.chtl
```

### Advanced Options

```bash
./chtl compile --inline --default-struct --output dist input.chtl
```

### Validation

```bash
./chtl validate input.chtl
```

### Formatting

```bash
./chtl format --in-place input.chtl
```

## Testing

Run the test suite:

```bash
cd build
ctest
```

Run specific tests:

```bash
cd build
ctest -R "UtilTest"
```

## Development

### Adding New Components

1. Create directory structure following existing patterns
2. Implement state machine + strategy pattern interfaces
3. Add CMakeLists.txt with proper linking
4. Update main CMakeLists.txt
5. Add comprehensive tests

### Code Style

- Use C++20 features appropriately
- Follow RAII principles
- Implement move semantics where beneficial
- Use smart pointers for memory management
- Follow existing naming conventions

### Debugging

Enable debug mode:

```bash
cmake .. -DCHTL_DEBUG_MODE=ON
cmake --build . --config Debug
```

Enable sanitizers:

```bash
cmake .. -DCHTL_ENABLE_SANITIZERS=ON
cmake --build .
```

## Troubleshooting

### Common Issues

1. **Path Problems**: Use `cd /app` before running cmake
2. **Memory Issues**: Use compiler monitor with appropriate limits
3. **Build Failures**: Check C++20 compiler compatibility
4. **Test Failures**: Run with verbose output for details

### Getting Help

- Check build logs in `build/` directory
- Use `--verbose` flag for detailed output
- Enable debug mode for additional information
- Check test output with `ctest --verbose`

## Contributing

1. Follow the state machine + strategy pattern architecture
2. Ensure all components are modular and non-interfering
3. Add comprehensive tests for new functionality
4. Update documentation for new features
5. Follow the established coding standards

## License

MIT License - see LICENSE file for details.