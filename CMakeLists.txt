cmake_minimum_required(VERSION 3.10)
project(CHTL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtl
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Generator.cpp
    src/ValueLexer.cpp
    src/ExpressionParser.cpp
    src/Loader.cpp
)
