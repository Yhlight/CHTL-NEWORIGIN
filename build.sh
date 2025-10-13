#!/bin/bash

# CHTL 构建脚本

set -e

echo "========================================="
echo "CHTL 编译器构建脚本"
echo "========================================="

# 创建构建目录
if [ ! -d "build" ]; then
    echo "创建 build 目录..."
    mkdir build
fi

cd build

# 运行 CMake
echo "运行 CMake..."
cmake ..

# 编译
echo "编译项目..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "========================================="
echo "构建完成！"
echo "========================================="
echo ""
echo "可执行文件: ./build/chtl"
echo ""
echo "使用示例:"
echo "  ./build/chtl examples/example.chtl"
echo "  ./build/chtl --inline examples/example.chtl"
echo "  ./build/chtl --help"
echo ""
echo "运行测试:"
echo "  cd build && ./chtl_test"
echo ""
