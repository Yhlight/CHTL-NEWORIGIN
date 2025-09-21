cmake_minimum_required(VERSION 3.10)
project(CHTLCompiler)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for headers
include_directories(
    .
)

# Add the executable
add_executable(chtl_compiler main.cpp)

# Link the implementation files to the executable
target_sources(chtl_compiler PRIVATE
    Scanner/UnifiedScanner.cpp
    CHTL/CHTLLexer/Lexer.cpp
    CHTL/CHTLParser/Parser.cpp
    CHTL/CHTLGenerator/Generator.cpp
    CHTL/CHTLResolver/Resolver.cpp
    CompilerDispatcher/CompilerDispatcher.cpp
    CodeMerger/CodeMerger.cpp
)
