#!/bin/bash

# Exit on error
set -e

# --- Compile ---
echo "--- Compiling the project ---"
rm -rf build
mkdir build
cd build
cmake ..
make
cd ..
echo "--- Compilation successful ---"

# --- Run Tests ---
COMPILER="./build/chtl-compiler"

TEST_FILES=(
    "test.chtl"
    "test_style.chtl"
    "test_autoclass.chtl"
    "test_ampersand.chtl"
    "test_arithmetic.chtl"
    "test_template_style.chtl"
)

for test_file in "${TEST_FILES[@]}"; do
    echo -e "\n\n--- Running test: $test_file ---"
    $COMPILER "$test_file"
done

echo -e "\n\n--- All tests completed ---"
