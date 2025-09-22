add_library(CHTLLexer STATIC
    Lexer.cpp
)

# The lexer needs the shared TokenTypes and the Context.
target_link_libraries(CHTLLexer PUBLIC
    CHTLCommon
    CHTLContext
)

# Expose the lexer's own headers.
target_include_directories(CHTLLexer PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
