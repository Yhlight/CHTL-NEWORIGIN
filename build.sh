#!/bin/bash

# CHTL Build Script
# This script builds the CHTL project using CMake

set -e  # Exit on any error

echo "CHTL Build Script"
echo "================="

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building CHTL..."
make -j$(nproc)

# Run tests if they exist
if [ -f "chtl_tests" ]; then
    echo "Running tests..."
    ./chtl_tests
    echo "Tests completed successfully!"
fi

echo "Build completed successfully!"
echo "Executable: ./chtl"
echo "Test executable: ./chtl_tests"