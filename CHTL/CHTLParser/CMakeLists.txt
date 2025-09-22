add_library(CHTLParser STATIC
    Parser.cpp
)

# The parser needs to know about the Token definitions from the Lexer
# and the Node definitions from the Node library.
target_link_libraries(CHTLParser PUBLIC
    CHTLLexer
    CHTLNode
)

# Expose the parser's own headers
target_include_directories(CHTLParser PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
