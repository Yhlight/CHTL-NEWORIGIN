#!/usr/bin/env python3
"""
CHTL Project Setup Script
This script helps set up the CHTL development environment
"""

import os
import sys
import subprocess
import shutil
import platform
from pathlib import Path

class Colors:
    RED = '\033[0;31m'
    GREEN = '\033[0;32m'
    YELLOW = '\033[1;33m'
    BLUE = '\033[0;34m'
    PURPLE = '\033[0;35m'
    CYAN = '\033[0;36m'
    WHITE = '\033[1;37m'
    NC = '\033[0m'  # No Color

class CHTLSetup:
    def __init__(self):
        self.project_root = Path(__file__).parent
        self.build_dir = self.project_root / "build"
        self.install_prefix = "/usr/local"
        
    def print_header(self):
        print(f"{Colors.BLUE}CHTL Project Setup Script v1.0.0{Colors.NC}")
        print(f"{Colors.BLUE}{'='*40}{Colors.NC}")
        print()
        
    def check_dependencies(self):
        print(f"{Colors.YELLOW}Checking dependencies...{Colors.NC}")
        
        dependencies = {
            'cmake': 'CMake build system',
            'g++': 'GCC C++ compiler',
            'clang++': 'Clang C++ compiler',
            'make': 'Make build tool',
            'ninja': 'Ninja build tool'
        }
        
        missing = []
        available = []
        
        for cmd, desc in dependencies.items():
            if shutil.which(cmd):
                available.append((cmd, desc))
                print(f"  {Colors.GREEN}✓{Colors.NC} {cmd} - {desc}")
            else:
                if cmd in ['g++', 'clang++'] and any(shutil.which(c) for c in ['g++', 'clang++']):
                    continue  # Skip if we have at least one C++ compiler
                if cmd in ['make', 'ninja'] and any(shutil.which(c) for c in ['make', 'ninja']):
                    continue  # Skip if we have at least one build tool
                missing.append((cmd, desc))
                print(f"  {Colors.RED}✗{Colors.NC} {cmd} - {desc}")
        
        if missing:
            print(f"\n{Colors.RED}Missing dependencies:{Colors.NC}")
            for cmd, desc in missing:
                print(f"  - {cmd}: {desc}")
            print(f"\n{Colors.YELLOW}Please install the missing dependencies and try again.{Colors.NC}")
            return False
            
        print(f"\n{Colors.GREEN}All dependencies available{Colors.NC}")
        return True
        
    def setup_directories(self):
        print(f"{Colors.YELLOW}Setting up project directories...{Colors.NC}")
        
        directories = [
            "src/CHTL/CHTLContext",
            "src/CHTL/CHTLLexer", 
            "src/CHTL/CHTLParser",
            "src/CHTL/CHTLGenerator",
            "src/CHTL/CHTLNode",
            "src/CHTL/CHTLLoader",
            "src/CHTL/CHTLState",
            "src/CHTL/CMODSystem",
            "src/CHTLJS/CHTLJSContext",
            "src/CHTLJS/CHTLJSLexer",
            "src/CHTLJS/CHTLJSParser", 
            "src/CHTLJS/CHTLJSGenerator",
            "src/CHTLJS/CHTLJSNode",
            "src/CHTLJS/CHTLJSLoader",
            "src/CHTLJS/CHTLJSState",
            "src/CHTLJS/CJMODSystem",
            "src/Scanner",
            "src/Util/FileSystem",
            "src/Util/StringUtil", 
            "src/Util/ZipUtil",
            "src/ThirdParty",
            "src/CSS",
            "src/JS",
            "include",
            "tests",
            "module",
            "examples",
            "docs"
        ]
        
        for dir_path in directories:
            full_path = self.project_root / dir_path
            full_path.mkdir(parents=True, exist_ok=True)
            print(f"  {Colors.GREEN}✓{Colors.NC} {dir_path}")
            
    def create_gitignore(self):
        print(f"{Colors.YELLOW}Creating .gitignore...{Colors.NC}")
        
        gitignore_content = """# Build directories
build/
dist/
out/

# Compiled files
*.o
*.obj
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# IDE files
.vscode/
.idea/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db

# CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
Makefile

# Temporary files
*.tmp
*.temp
*.log

# Test output
test_results/
coverage/

# Module files
*.cmod
*.cjmod
"""
        
        gitignore_path = self.project_root / ".gitignore"
        with open(gitignore_path, 'w') as f:
            f.write(gitignore_content)
        print(f"  {Colors.GREEN}✓{Colors.NC} .gitignore created")
        
    def create_readme(self):
        print(f"{Colors.YELLOW}Creating README.md...{Colors.NC}")
        
        readme_content = """# CHTL - C++ HyperText Language

CHTL is a C++-based hypertext language that provides a more developer-friendly way to write HTML code.

## Features

- **Modern Syntax**: Clean, readable syntax inspired by modern programming languages
- **Template System**: Powerful template and custom system for code reuse
- **Module Support**: CMOD and CJMOD modules for extensibility
- **CHTL JS**: Extended JavaScript syntax with enhanced features
- **Unified Scanner**: Intelligent code separation and processing
- **Cross-platform**: Works on Windows, macOS, and Linux

## Quick Start

### Building

```bash
# Clone the repository
git clone <repository-url>
cd CHTL

# Build the compiler
./build.sh

# Or use Python setup
python3 setup.py --build
```

### Usage

```bash
# Compile a CHTL file
./build/chtl example.chtl

# Generate HTML with default structure
./build/chtl --default-struct example.chtl

# Inline CSS and JS
./build/chtl --inline example.chtl
```

### Example

```chtl
html
{
    head
    {
        title
        {
            text { My CHTL Page }
        }
    }
    
    body
    {
        div
        {
            style
            {
                width: 100%;
                height: 100vh;
                background-color: #f0f0f0;
            }
            
            h1
            {
                text { Welcome to CHTL! }
            }
        }
    }
}
```

## Documentation

See `CHTL.md` for complete language documentation.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## License

MIT License - see LICENSE file for details.
"""
        
        readme_path = self.project_root / "README.md"
        with open(readme_path, 'w') as f:
            f.write(readme_content)
        print(f"  {Colors.GREEN}✓{Colors.NC} README.md created")
        
    def build_project(self):
        print(f"{Colors.YELLOW}Building CHTL project...{Colors.NC}")
        
        try:
            # Create build directory
            self.build_dir.mkdir(exist_ok=True)
            
            # Configure with CMake
            print(f"  {Colors.BLUE}Configuring with CMake...{Colors.NC}")
            subprocess.run([
                "cmake", 
                "-DCMAKE_BUILD_TYPE=Release",
                "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
                ".."
            ], cwd=self.build_dir, check=True)
            
            # Build
            print(f"  {Colors.BLUE}Building...{Colors.NC}")
            if shutil.which("ninja"):
                subprocess.run(["ninja"], cwd=self.build_dir, check=True)
            else:
                subprocess.run(["make", "-j", str(os.cpu_count())], cwd=self.build_dir, check=True)
                
            print(f"  {Colors.GREEN}✓{Colors.NC} Build completed successfully")
            
        except subprocess.CalledProcessError as e:
            print(f"  {Colors.RED}✗{Colors.NC} Build failed: {e}")
            return False
            
        return True
        
    def run_tests(self):
        print(f"{Colors.YELLOW}Running tests...{Colors.NC}")
        
        try:
            subprocess.run(["ctest", "--output-on-failure"], cwd=self.build_dir, check=True)
            print(f"  {Colors.GREEN}✓{Colors.NC} All tests passed")
            return True
        except subprocess.CalledProcessError as e:
            print(f"  {Colors.RED}✗{Colors.NC} Tests failed: {e}")
            return False
            
    def create_example(self):
        print(f"{Colors.YELLOW}Creating example files...{Colors.NC}")
        
        # Create examples directory
        examples_dir = self.project_root / "examples"
        examples_dir.mkdir(exist_ok=True)
        
        # Simple example
        simple_example = examples_dir / "simple.chtl"
        with open(simple_example, 'w') as f:
            f.write("""html
{
    head
    {
        title
        {
            text { Simple CHTL Example }
        }
    }
    
    body
    {
        div
        {
            h1
            {
                text { Hello, CHTL! }
            }
            
            p
            {
                text { This is a simple CHTL document. }
            }
        }
    }
}
""")
        
        print(f"  {Colors.GREEN}✓{Colors.NC} Example files created")
        
    def print_summary(self):
        print(f"\n{Colors.GREEN}Setup completed successfully!{Colors.NC}")
        print(f"{Colors.BLUE}{'='*40}{Colors.NC}")
        print(f"Project root: {self.project_root}")
        print(f"Build directory: {self.build_dir}")
        print()
        print(f"Next steps:")
        print(f"  1. Run tests: python3 setup.py --test")
        print(f"  2. Build: python3 setup.py --build")
        print(f"  3. Try example: ./build/chtl example.chtl")
        print()
        
    def run(self, args=None):
        self.print_header()
        
        if not self.check_dependencies():
            return 1
            
        self.setup_directories()
        self.create_gitignore()
        self.create_readme()
        self.create_example()
        
        if args and '--build' in args:
            if not self.build_project():
                return 1
                
        if args and '--test' in args:
            if not self.run_tests():
                return 1
                
        self.print_summary()
        return 0

if __name__ == "__main__":
    setup = CHTLSetup()
    sys.exit(setup.run(sys.argv[1:]))