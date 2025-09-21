cmake_minimum_required(VERSION 3.10)
project(CHTL_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the main executable
add_executable(chtl_compiler
    main.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
    CHTL/CMODSystem/CMODSystem.cpp
    CHTL/Loader.cpp
    CHTL/Compiler.cpp
    CHTL/CHTLExpressionParser.cpp
    ThirdParty/miniz.c
)

# Include directories
target_include_directories(chtl_compiler
    PUBLIC
        .
        ThirdParty
)
