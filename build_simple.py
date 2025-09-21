#!/usr/bin/env python3
"""
CHTL Simple Build Script
Builds only the core components without external dependencies
"""

import os
import subprocess
import sys
from pathlib import Path

class Colors:
    RED = '\033[0;31m'
    GREEN = '\033[0;32m'
    YELLOW = '\033[1;33m'
    BLUE = '\033[0;34m'
    NC = '\033[0m'

def print_header():
    print(f"{Colors.BLUE}CHTL Simple Build Script v1.0.0{Colors.NC}")
    print(f"{Colors.BLUE}{'='*40}{Colors.NC}")

def check_dependencies():
    print(f"{Colors.YELLOW}Checking dependencies...{Colors.NC}")
    
    # Check for C++ compiler
    if not (os.system("which g++ > /dev/null 2>&1") == 0 or os.system("which clang++ > /dev/null 2>&1") == 0):
        print(f"{Colors.RED}Error: No C++ compiler found{Colors.NC}")
        return False
    
    print(f"{Colors.GREEN}Dependencies check passed{Colors.NC}")
    return True

def build_core_components():
    print(f"{Colors.YELLOW}Building core components...{Colors.NC}")
    
    # Create build directory
    build_dir = Path("build_simple")
    build_dir.mkdir(exist_ok=True)
    
    # Core source files
    sources = [
        "src/Util/StringUtil/StringUtil.cpp",
        "src/Util/FileSystem/FileSystem.cpp", 
        "src/CHTL/CHTLLexer/Token.cpp",
        "src/CHTL/CHTLLexer/TokenUtil.cpp",
        "src/CHTL/CHTLLexer/CHTLLexer.cpp",
        "src/CHTL/CHTLNode/ASTNode.cpp",
        "src/CHTL/CHTLParser/CHTLParser.cpp",
        "src/CHTL/CHTLGenerator/CHTLGenerator.cpp",
        "src/Scanner/UnifiedScanner.cpp",
        "src/CodeMerger.cpp",
        "src/CompilerDispatcher.cpp",
        "src/main.cpp"
    ]
    
    # Include directories
    includes = [
        "-Iinclude",
        "-Isrc"
    ]
    
    # Compiler flags
    flags = [
        "-std=c++17",
        "-Wall",
        "-Wextra",
        "-O2",
        "-pthread"
    ]
    
    # Build command
    cmd = ["g++"] + flags + includes + sources + ["-o", str(build_dir / "chtl")]
    
    print(f"  {Colors.BLUE}Compiling...{Colors.NC}")
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"{Colors.RED}Compilation failed:{Colors.NC}")
            print(result.stderr)
            return False
        print(f"  {Colors.GREEN}✓ Compilation successful{Colors.NC}")
        return True
    except Exception as e:
        print(f"{Colors.RED}Build error: {e}{Colors.NC}")
        return False

def build_tests():
    print(f"{Colors.YELLOW}Building tests...{Colors.NC}")
    
    build_dir = Path("build_simple")
    
    # Test sources
    test_sources = [
        "tests/basic_test.cpp"
    ]
    
    includes = ["-Iinclude", "-Isrc"]
    flags = ["-std=c++17", "-Wall", "-Wextra", "-O2"]
    
    cmd = ["g++"] + flags + includes + test_sources + ["-o", str(build_dir / "simple_test")]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"{Colors.RED}Test compilation failed:{Colors.NC}")
            print(result.stderr)
            return False
        print(f"  {Colors.GREEN}✓ Test compilation successful{Colors.NC}")
        return True
    except Exception as e:
        print(f"{Colors.RED}Test build error: {e}{Colors.NC}")
        return False

def run_tests():
    print(f"{Colors.YELLOW}Running tests...{Colors.NC}")
    
    test_executable = Path("build_simple/simple_test")
    if not test_executable.exists():
        print(f"{Colors.RED}Test executable not found{Colors.NC}")
        return False
    
    try:
        result = subprocess.run([str(test_executable)], capture_output=True, text=True)
        print(result.stdout)
        if result.stderr:
            print(result.stderr)
        
        if result.returncode == 0:
            print(f"  {Colors.GREEN}✓ All tests passed{Colors.NC}")
            return True
        else:
            print(f"  {Colors.RED}✗ Some tests failed{Colors.NC}")
            return False
    except Exception as e:
        print(f"{Colors.RED}Test execution error: {e}{Colors.NC}")
        return False

def test_compiler():
    print(f"{Colors.YELLOW}Testing compiler...{Colors.NC}")
    
    compiler_executable = Path("build_simple/chtl")
    if not compiler_executable.exists():
        print(f"{Colors.RED}Compiler executable not found{Colors.NC}")
        return False
    
    # Test with help
    try:
        result = subprocess.run([str(compiler_executable), "--help"], capture_output=True, text=True)
        if result.returncode == 0:
            print(f"  {Colors.GREEN}✓ Help command works{Colors.NC}")
        else:
            print(f"  {Colors.RED}✗ Help command failed{Colors.NC}")
            return False
    except Exception as e:
        print(f"{Colors.RED}Compiler test error: {e}{Colors.NC}")
        return False
    
    return True

def main():
    print_header()
    
    if not check_dependencies():
        return 1
    
    if not build_core_components():
        return 1
    
    if not build_tests():
        return 1
    
    if not run_tests():
        return 1
    
    if not test_compiler():
        return 1
    
    print(f"\n{Colors.GREEN}Build completed successfully!{Colors.NC}")
    print(f"{Colors.BLUE}{'='*40}{Colors.NC}")
    print(f"Compiler: build_simple/chtl")
    print(f"Tests: build_simple/simple_test")
    print()
    print(f"Try: ./build_simple/chtl --help")
    print(f"Try: ./build_simple/chtl example.chtl")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())