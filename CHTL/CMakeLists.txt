# CHTL CMakeLists.txt
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLNode)
add_subdirectory(CHTLParser)

# Create combined CHTL library
add_library(CHTL STATIC
    CHTLLexer/Token.cpp
    CHTLLexer/GlobalMap.cpp
    CHTLLexer/Lexer.cpp
    CHTLNode/BaseNode.cpp
    CHTLNode/ElementNode.cpp
    CHTLParser/CHTLParser.cpp
    CHTLParser/ParserState.cpp
    CHTLParser/ParserStrategy.cpp
)

target_include_directories(CHTL PUBLIC
    ${CMAKE_SOURCE_DIR}/CHTL
)

target_link_libraries(CHTL PUBLIC
    CHTLLexer
    CHTLNode
    CHTLParser
    Scanner
    Util
    ThirdParty
)