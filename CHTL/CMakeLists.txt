# This will be a library containing the CHTL compiler components
add_library(CHTL STATIC
    CHTL.cpp # Dummy file, will be removed as real components are added

    # Core Interfaces
    CHTLNode/BaseNode.cpp
    CHTLState/StateMachine.cpp

    # Lexer
    CHTLLexer/Lexer.cpp

    # Parser and States
    CHTLParser/Parser.cpp
    CHTLState/GlobalScopeState.cpp

    # Context
    CHTLContext/Context.cpp
)

# Publicly expose the CHTL directory for includes, so other modules can use these interfaces.
target_include_directories(CHTL PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
