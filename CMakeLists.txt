cmake_minimum_required(VERSION 3.10)
project(CHTL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtl src/main.cpp src/CHTL/CHTLLexer/Lexer.cpp src/CHTL/CHTLParser/Parser.cpp src/CHTL/CHTLGenerator/Generator.cpp)

# Add third-party libraries
# add_subdirectory(third-party/libzippp)
# add_subdirectory(third-party/antlr4)
# add_subdirectory(third-party/libsass)
# add_subdirectory(third-party/quickjs)

# target_link_libraries(chtl PRIVATE libzippp antlr4 libsass quickjs)

# Tests
add_executable(lexer_tests src/Test/LexerTests.cpp src/CHTL/CHTLLexer/Lexer.cpp)
add_executable(parser_tests src/Test/ParserTests.cpp src/CHTL/CHTLParser/Parser.cpp src/CHTL/CHTLLexer/Lexer.cpp)
add_executable(generator_tests src/Test/GeneratorTests.cpp src/CHTL/CHTLGenerator/Generator.cpp src/CHTL/CHTLParser/Parser.cpp src/CHTL/CHTLLexer/Lexer.cpp)