# CHTLParser CMakeLists.txt
add_library(CHTLParser STATIC
    CHTLParser.h
    CHTLParser.cpp
    ParserState.h
    ParserState.cpp
    ParserStrategy.h
    ParserStrategy.cpp
)

target_include_directories(CHTLParser PUBLIC
    ${CMAKE_SOURCE_DIR}/CHTL/CHTLParser
)

target_link_libraries(CHTLParser PUBLIC
    CHTLNode
    CHTLLexer
    Scanner
    Util
)