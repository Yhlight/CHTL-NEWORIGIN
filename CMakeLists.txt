cmake_minimum_required(VERSION 3.10)

project(CHTL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(chtl src/main.cpp)

# Also add miniz.c to the compilation
target_sources(chtl PRIVATE
    miniz.c
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLLexer/Token.cpp
    CHTL/CHTLNode/Node.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
    CHTL/CHTLContext.cpp
)

# Include directories for headers
target_include_directories(chtl PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/CHTL/CHTLLexer
    ${PROJECT_SOURCE_DIR}/CHTL/CHTLParser
    ${PROJECT_SOURCE_DIR}/CHTL/CHTLNode
    ${PROJECT_SOURCE_DIR}/CHTL/CHTLGenerator
    ${PROJECT_SOURCE_DIR}/Util
)
