cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/CHTL)
include_directories(${CMAKE_SOURCE_DIR}/CHTL\ JS)
include_directories(${CMAKE_SOURCE_DIR}/Util)
include_directories(${CMAKE_SOURCE_DIR}/ThirdParty)

# 查找依赖
find_package(PkgConfig REQUIRED)

# 添加子目录
add_subdirectory(Util)
add_subdirectory(CHTL)
add_subdirectory(CHTL\ JS)
add_subdirectory(Scanner)
add_subdirectory(CodeMerger)
add_subdirectory(CompilerDispatcher)
add_subdirectory(Test)

# 主可执行文件
add_executable(chtl main.cpp)
target_link_libraries(chtl 
    CHTLCore
    CHTJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

# 测试可执行文件
add_executable(test_compiler Test/test_compiler.cpp)
target_link_libraries(test_compiler 
    CHTLCore
    CHTJSCore
    Scanner
    CodeMerger
    CompilerDispatcher
    Util
)

# 安装规则
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY Module/ DESTINATION share/chtl/modules)