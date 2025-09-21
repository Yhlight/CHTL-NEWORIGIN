#!/bin/bash

# CHTL Build Script
# This script builds the CHTL compiler and related tools

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Build configuration
BUILD_TYPE=${BUILD_TYPE:-Release}
BUILD_DIR=${BUILD_DIR:-build}
INSTALL_PREFIX=${INSTALL_PREFIX:-/usr/local}

echo -e "${BLUE}CHTL Build Script v1.0.0${NC}"
echo -e "${BLUE}========================${NC}"

# Check dependencies
echo -e "${YELLOW}Checking dependencies...${NC}"

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake is not installed${NC}"
    echo "Please install CMake 3.20 or later"
    exit 1
fi

# Check for C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Error: No C++ compiler found${NC}"
    echo "Please install GCC or Clang"
    exit 1
fi

# Check for make or ninja
if ! command -v make &> /dev/null && ! command -v ninja &> /dev/null; then
    echo -e "${RED}Error: No build system found${NC}"
    echo "Please install make or ninja"
    exit 1
fi

echo -e "${GREEN}Dependencies check passed${NC}"

# Create build directory
echo -e "${YELLOW}Creating build directory...${NC}"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
echo -e "${YELLOW}Building CHTL compiler...${NC}"
if command -v ninja &> /dev/null; then
    ninja
else
    make -j$(nproc)
fi

# Run tests if requested
if [ "$1" = "--test" ]; then
    echo -e "${YELLOW}Running tests...${NC}"
    ctest --output-on-failure
fi

# Install if requested
if [ "$1" = "--install" ]; then
    echo -e "${YELLOW}Installing CHTL compiler...${NC}"
    sudo make install
fi

echo -e "${GREEN}Build completed successfully!${NC}"
echo -e "${BLUE}========================${NC}"
echo -e "Build type: $BUILD_TYPE"
echo -e "Build directory: $BUILD_DIR"
echo -e "Install prefix: $INSTALL_PREFIX"
echo -e ""
echo -e "To run the compiler:"
echo -e "  ./$BUILD_DIR/chtl --help"
echo -e ""
echo -e "To test with example:"
echo -e "  ./$BUILD_DIR/chtl example.chtl"