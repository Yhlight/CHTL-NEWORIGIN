cmake_minimum_required(VERSION 3.10)

project(CHTL_Compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtl-compiler
    src/main.cpp
    src/Scanner/UnifiedScanner.cpp
    src/CHTL/CHTLLexer/CHTLLexer.cpp
    src/CHTL/CHTLParser/CHTLParser.cpp
    src/CHTL/CHTLGenerator/CHTLGenerator.cpp
    src/CHTL/Expression/ExpressionParser.cpp
    src/CHTL/Expression/ExpressionEvaluator.cpp
    src/CHTL/Template/TemplateRegistry.cpp
)

target_include_directories(chtl-compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
