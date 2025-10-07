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

# 源文件
set(CHTL_LIBRARY_SOURCES
    src/Util/FileSystem/FileSystem.cpp
    src/Util/StringUtil/StringUtil.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/CHTLGenerator/ConfigLoader.cpp
    src/CHTL/CHTLLexer/GlobalMap.cpp
    src/CHTL/CHTLLexer/Lexer.cpp
    src/CHTL/CHTLLexer/Token.cpp
    src/CHTL/CHTLNode/BaseNode.cpp
    src/CHTL/CHTLNode/nodes/ProgramNode.cpp
    src/CHTL/CHTLNode/nodes/ElementNode.cpp
    src/CHTL/CHTLNode/nodes/TextNode.cpp
    src/CHTL/CHTLNode/nodes/CommentNode.cpp
    src/CHTL/CHTLNode/nodes/AttributeNode.cpp
    src/CHTL/CHTLNode/nodes/StyleNode.cpp
    src/CHTL/CHTLNode/nodes/ScriptNode.cpp
    src/CHTL/CHTLNode/nodes/TemplateNode.cpp
    src/CHTL/CHTLNode/nodes/CustomNode.cpp
    src/CHTL/CHTLNode/nodes/OriginNode.cpp
    src/CHTL/CHTLNode/nodes/ImportNode.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLParser/ExpressionParser.cpp
    src/CHTL/CHTLState/CHTLState.cpp
    src/CHTL/CHTLTemplate/TemplateRegistry.cpp
    src/CHTL/CMODSystem/ModuleLoader.cpp
    src/CHTL/CMODSystem/ModulePacker.cpp
    src/CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.cpp
    src/CHTL-JS/CHTLJSGenerator/VirRegistry.cpp
    src/CHTL-JS/CHTLJSLexer/CHTLJSLexer.cpp
    src/CHTL-JS/CHTLJSLexer/CHTLJSToken.cpp
    src/CHTL-JS/CHTLJSParser/CHTLJSParser.cpp
    src/CHTL-JS/CJMODSystem/CJMODApi.cpp
    src/SharedCore/SaltBridge.cpp
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
add_executable(chtl src/main.cpp ${CHTL_LIBRARY_SOURCES})

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
        ${CHTL_LIBRARY_SOURCES}
    )
    
    # 链接 libzip（如果可用）
    if(LIBZIP_FOUND)
        target_link_libraries(chtl_tests ${LIBZIP_LIBRARIES})
    endif()
    
    add_test(NAME chtl_tests COMMAND chtl_tests)
endif()

# 安装规则
install(TARGETS chtl DESTINATION bin)
