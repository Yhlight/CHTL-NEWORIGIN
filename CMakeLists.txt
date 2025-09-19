cmake_minimum_required(VERSION 3.16)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/CHTL)
include_directories(${CMAKE_SOURCE_DIR}/CHTL_JS)
include_directories(${CMAKE_SOURCE_DIR}/Scanner)
include_directories(${CMAKE_SOURCE_DIR}/CodeMerger)
include_directories(${CMAKE_SOURCE_DIR}/CompilerDispatcher)
include_directories(${CMAKE_SOURCE_DIR}/Util)

# 源文件
set(CHTL_SOURCES
    CHTL/CHTLLexer/CHTLLexer.cpp
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLNode/BaseNode.cpp
    Util/StringUtil/StringUtil.cpp
    Util/FileSystem/FileSystem.cpp
)

set(CHTL_JS_SOURCES
    # 暂时为空，后续实现
)

set(UTIL_SOURCES
    Util/FileSystem/FileSystem.cpp
    Util/StringUtil/StringUtil.cpp
)

set(SCANNER_SOURCES
    Scanner/UnifiedScanner.cpp
)

set(COMPILER_SOURCES
    # 暂时为空，后续实现
)

# 创建库
add_library(CHTL_LIB ${CHTL_SOURCES})
add_library(UTIL_LIB ${UTIL_SOURCES})
add_library(SCANNER_LIB ${SCANNER_SOURCES})

# 暂时注释掉空的库
# add_library(CHTL_JS_LIB ${CHTL_JS_SOURCES})
# add_library(COMPILER_LIB ${COMPILER_SOURCES})

# 链接库
target_link_libraries(CHTL_LIB UTIL_LIB)
target_link_libraries(SCANNER_LIB UTIL_LIB)
# target_link_libraries(CHTL_JS_LIB UTIL_LIB SCANNER_LIB)
# target_link_libraries(COMPILER_LIB CHTL_LIB CHTL_JS_LIB SCANNER_LIB UTIL_LIB)

# 创建可执行文件
add_executable(chtl_compiler main.cpp)
target_link_libraries(chtl_compiler CHTL_LIB SCANNER_LIB UTIL_LIB)

# 测试
enable_testing()
# add_subdirectory(Test)  # 暂时注释掉，后续实现

# 安装
install(TARGETS chtl_compiler DESTINATION bin)
install(TARGETS CHTL_LIB UTIL_LIB SCANNER_LIB DESTINATION lib)