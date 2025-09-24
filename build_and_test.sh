#!/bin/bash

# CHTL Build and Test Script
# This script builds the project and runs all tests

set -e  # Exit on any error

echo "=== CHTL Build and Test Script ==="
echo "Current directory: $(pwd)"

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: CMakeLists.txt not found. Please run from project root."
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCHTL_BUILD_TESTS=ON -DCHTL_DEBUG_MODE=ON

# Build the project
echo "Building project..."
cmake --build . --parallel $(nproc 2>/dev/null || echo 4)

# Run tests
echo "Running tests..."
if [ -f "UtilTest/UtilTest" ]; then
    echo "Running UtilTest..."
    ./UtilTest/UtilTest
fi

if [ -f "TokenTest/TokenTest" ]; then
    echo "Running TokenTest..."
    ./TokenTest/TokenTest
fi

if [ -f "StateMachineTest" ]; then
    echo "Running StateMachineTest..."
    ./StateMachineTest
fi

if [ -f "IntegrationTest" ]; then
    echo "Running IntegrationTest..."
    ./IntegrationTest
fi

# Run CTest if available
if command -v ctest &> /dev/null; then
    echo "Running CTest..."
    ctest --verbose
fi

echo "=== Build and Test Complete ==="
echo "All tests passed successfully!"