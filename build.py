#!/usr/bin/env python3
"""
CHTL Build Script
This script handles CMake builds with proper path handling to avoid common issues.
Usage: python build.py [--clean] [--debug] [--test] [--install]
"""

import os
import sys
import shutil
import subprocess
import argparse
from pathlib import Path

class Colors:
    """ANSI color codes for terminal output"""
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def print_step(message):
    """Print a build step message"""
    print(f"{Colors.HEADER}{Colors.BOLD}==> {message}{Colors.ENDC}")

def print_success(message):
    """Print a success message"""
    print(f"{Colors.OKGREEN}✓ {message}{Colors.ENDC}")

def print_error(message):
    """Print an error message"""
    print(f"{Colors.FAIL}✗ {message}{Colors.ENDC}", file=sys.stderr)

def print_warning(message):
    """Print a warning message"""
    print(f"{Colors.WARNING}⚠ {message}{Colors.ENDC}")

def run_command(cmd, cwd=None, env=None):
    """Run a shell command and handle errors"""
    try:
        result = subprocess.run(
            cmd,
            cwd=cwd,
            env=env,
            check=True,
            capture_output=True,
            text=True
        )
        return result.returncode == 0
    except subprocess.CalledProcessError as e:
        print_error(f"Command failed: {' '.join(cmd)}")
        if e.stdout:
            print(e.stdout)
        if e.stderr:
            print(e.stderr)
        return False

def main():
    parser = argparse.ArgumentParser(description='Build CHTL project')
    parser.add_argument('--clean', action='store_true', help='Clean build directory before building')
    parser.add_argument('--debug', action='store_true', help='Build in debug mode')
    parser.add_argument('--test', action='store_true', help='Run tests after building')
    parser.add_argument('--install', action='store_true', help='Install after building')
    args = parser.parse_args()

    # Get project root (where this script is located)
    project_root = Path(__file__).parent.absolute()
    build_dir = project_root / "build"
    
    print_step("CHTL Build System")
    print(f"Project root: {project_root}")
    
    # Clean build directory if requested
    if args.clean and build_dir.exists():
        print_step("Cleaning build directory...")
        shutil.rmtree(build_dir)
        print_success("Build directory cleaned")
    
    # Create build directory
    build_dir.mkdir(exist_ok=True)
    
    # Determine build type
    build_type = "Debug" if args.debug else "Release"
    print_step(f"Configuring CMake ({build_type})...")
    
    # Configure CMake
    cmake_args = [
        "cmake",
        "-S", str(project_root),
        "-B", str(build_dir),
        f"-DCMAKE_BUILD_TYPE={build_type}",
    ]
    
    if not run_command(cmake_args):
        print_error("CMake configuration failed")
        return 1
    
    print_success("CMake configuration successful")
    
    # Build
    print_step("Building project...")
    if not run_command(["cmake", "--build", str(build_dir), "--config", build_type, "-j"]):
        print_error("Build failed")
        return 1
    
    print_success("Build successful")
    
    # Run tests if requested
    if args.test:
        print_step("Running tests...")
        if not run_command(["ctest", "--output-on-failure"], cwd=build_dir):
            print_warning("Some tests failed")
        else:
            print_success("All tests passed")
    
    # Install if requested
    if args.install:
        print_step("Installing...")
        if not run_command(["cmake", "--install", str(build_dir)]):
            print_error("Installation failed")
            return 1
        print_success("Installation successful")
    
    print_step("Build complete!")
    print(f"Executables are in: {build_dir / 'bin'}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
