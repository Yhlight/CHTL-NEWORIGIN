# CHTLLexer CMakeLists.txt
add_library(CHTLLexer STATIC
    Token.h
    Token.cpp
    GlobalMap.h
    GlobalMap.cpp
    Lexer.h
    Lexer.cpp
)

target_include_directories(CHTLLexer PUBLIC
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLLexer
)

target_link_libraries(CHTLLexer PUBLIC Util)