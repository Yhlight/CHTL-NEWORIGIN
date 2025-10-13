cmake_minimum_required(VERSION 3.20)
project(CHTL VERSION 1.0.0 LANGUAGES CXX)

# C++20标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party)

# 源文件
set(CHTL_SOURCES
    src/main.cpp
)

# 头文件
set(CHTL_HEADERS
    include/Common.hpp
    include/Util/StringUtil.hpp
    include/Util/FileSystem.hpp
    include/CHTL/Token.hpp
    include/CHTL/Lexer.hpp
    include/CHTL/Node.hpp
    include/CHTL/Parser.hpp
    include/CHTL/Generator.hpp
    include/CHTL/Compiler.hpp
)

# 可执行文件
add_executable(chtl ${CHTL_SOURCES} ${CHTL_HEADERS})

# 链接库
if(UNIX)
    target_link_libraries(chtl stdc++fs)
endif()

# 测试
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    
    # 测试可执行文件
    add_executable(chtl_test
        test/test_main.cpp
        test/test_lexer.cpp
        test/test_parser.cpp
        test/test_generator.cpp
    )
    
    target_link_libraries(chtl_test)
    
    if(UNIX)
        target_link_libraries(chtl_test stdc++fs)
    endif()
    
    add_test(NAME chtl_test COMMAND chtl_test)
endif()

# 安装
install(TARGETS chtl DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/chtl)

# 示例文件
install(FILES examples/example.chtl DESTINATION share/chtl/examples)
