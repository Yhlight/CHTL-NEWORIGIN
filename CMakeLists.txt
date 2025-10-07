cmake_minimum_required(VERSION 3.10)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third-party
)

# 源文件目录
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Util工具层
file(GLOB_RECURSE UTIL_SOURCES 
    "${SRC_DIR}/Util/*.cpp"
)

# CHTL编译器
file(GLOB_RECURSE CHTL_SOURCES 
    "${SRC_DIR}/CHTL/*.cpp"
)

# CHTL JS编译器
file(GLOB_RECURSE CHTLJS_SOURCES 
    "${SRC_DIR}/CHTL-JS/*.cpp"
)

# 确保包含新的Parser文件
list(APPEND CHTLJS_SOURCES 
    "${SRC_DIR}/CHTL-JS/CHTLJSParser/CHTLJSParser.cpp"
)

# 盐桥核心
file(GLOB_RECURSE SHARED_SOURCES 
    "${SRC_DIR}/SharedCore/*.cpp"
)

# 第三方库
# 使用系统 libzip

# 手动查找 libzip（直接指定路径）
find_path(LIBZIP_INCLUDE_DIR 
    NAMES zip.h
    PATHS /usr/include /usr/local/include
)

find_library(LIBZIP_LIBRARY 
    NAMES zip
    PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
)

if(LIBZIP_INCLUDE_DIR AND LIBZIP_LIBRARY)
    set(LIBZIP_FOUND TRUE)
    set(LIBZIP_INCLUDE_DIRS ${LIBZIP_INCLUDE_DIR})
    set(LIBZIP_LIBRARIES ${LIBZIP_LIBRARY})
    message(STATUS "Found libzip: ${LIBZIP_LIBRARY}")
    message(STATUS "  Include: ${LIBZIP_INCLUDE_DIR}")
    include_directories(${LIBZIP_INCLUDE_DIRS})
else()
    message(WARNING "libzip not found, module packing features will be disabled")
    set(LIBZIP_FOUND FALSE)
endif()

# 添加第三方库包含目录
include_directories(${CMAKE_SOURCE_DIR}/third-party)

# 主可执行文件
add_executable(chtl
    ${SRC_DIR}/main.cpp
    ${UTIL_SOURCES}
    ${CHTL_SOURCES}
    ${CHTLJS_SOURCES}
    ${SHARED_SOURCES}
)

# 链接 libzip（如果可用）
if(LIBZIP_FOUND)
    target_link_libraries(chtl ${LIBZIP_LIBRARIES})
endif()

# 测试可执行文件
enable_testing()

file(GLOB_RECURSE TEST_SOURCES 
    "${CMAKE_SOURCE_DIR}/tests/*.cpp"
)

if(TEST_SOURCES)
    add_executable(chtl_tests
        ${TEST_SOURCES}
        ${UTIL_SOURCES}
        ${CHTL_SOURCES}
        ${CHTLJS_SOURCES}
        ${SHARED_SOURCES}
    )
    
    # 链接 libzip（如果可用）
    if(LIBZIP_FOUND)
        target_link_libraries(chtl_tests ${LIBZIP_LIBRARIES})
    endif()
    
    add_test(NAME chtl_tests COMMAND chtl_tests)
endif()

# 安装规则
install(TARGETS chtl DESTINATION bin)
