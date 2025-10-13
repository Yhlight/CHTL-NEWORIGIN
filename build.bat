@echo off
REM CHTL Windows 构建脚本

echo =========================================
echo CHTL 编译器构建脚本
echo =========================================

REM 创建构建目录
if not exist build mkdir build
cd build

REM 运行 CMake
echo 运行 CMake...
cmake ..

REM 编译
echo 编译项目...
cmake --build . --config Release

echo.
echo =========================================
echo 构建完成！
echo =========================================
echo.
echo 可执行文件: build\Release\chtl.exe
echo.
echo 使用示例:
echo   build\Release\chtl.exe examples\example.chtl
echo   build\Release\chtl.exe --inline examples\example.chtl
echo   build\Release\chtl.exe --help
echo.
echo 运行测试:
echo   cd build && ctest
echo.

cd ..
