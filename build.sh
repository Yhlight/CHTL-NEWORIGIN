#!/bin/bash

# CHTL项目构建脚本

echo "开始构建CHTL项目..."

# 检查是否存在build目录
if [ ! -d "build" ]; then
    echo "创建build目录..."
    mkdir build
fi

# 进入build目录
cd build

# 配置CMake
echo "配置CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
echo "编译项目..."
make -j$(nproc)

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo ""
    echo "可执行文件："
    echo "  - chtl: 主编译器"
    echo "  - test_compiler: 测试程序"
    echo ""
    echo "运行测试："
    echo "  ./test_compiler"
    echo ""
    echo "使用编译器："
    echo "  ./chtl ../Test/test_example.chtl"
else
    echo "编译失败！"
    exit 1
fi