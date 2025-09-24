# Defines the CHTL_Compiler library.
add_library(CHTL_Compiler STATIC
    CHTL.cpp
    CHTLState/State.h
    CHTLStrategy/Strategy.h
    CHTLContext/Context.h
    CHTLContext/Context.cpp
    CHTLLexer/Token.h
    CHTLLexer/CHTLLexer.h
    CHTLLexer/CHTLLexer.cpp
    CHTLNode/BaseNode.h
    CHTLNode/AttributeNode.h
    CHTLNode/TextNode.h
    CHTLNode/ElementNode.h
    CHTLState/ParseBaseState.h
    CHTLState/ParseBaseState.cpp
    CHTLState/ParseElementState.h
    CHTLState/ParseElementState.cpp
    CHTLParser/CHTLParser.h
    CHTLParser/CHTLParser.cpp
    CHTLGenerator/CHTLGenerator.h
    CHTLGenerator/CHTLGenerator.cpp
)

# Exposes the 'CHTL' directory for include lookups.
target_include_directories(CHTL_Compiler
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
