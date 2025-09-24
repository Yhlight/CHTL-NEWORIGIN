# Define the CHTL_Compiler library
add_library(CHTL_Compiler STATIC
    Parser.cpp
    CHTLState/IState.h
    CHTLState/GlobalState.h
    CHTLState/GlobalState.cpp
    CHTLState/ElementState.h
    CHTLState/ElementState.cpp
    CHTLStrategy/IStrategy.h
    CHTLLexer/Lexer.cpp
    CHTLLexer/Lexer.h
    CHTLLexer/Token.h
    CHTLLexer/TokenType.h
    CHTLNode/BaseNode.h
    CHTLNode/INodeVisitor.h
    CHTLNode/ElementNode.h
    CHTLNode/AttributeNode.h
    CHTLNode/TextNode.h
    CHTLNode/StyleNode.h
    CHTLNode/CommentNode.h
    CHTLNode/CssPropertyNode.h
    CHTLState/StyleBlockState.h
    CHTLState/StyleBlockState.cpp
    CHTLStrategy/IStyleValueParseStrategy.h
    CHTLStrategy/SimpleValueStrategy.h
    CHTLStrategy/SimpleValueStrategy.cpp
    CHTLStrategy/ArithmeticExpressionStrategy.h
    CHTLStrategy/ArithmeticExpressionStrategy.cpp
    CHTLGenerator/Generator.h
    CHTLGenerator/Generator.cpp
)

# Public headers for the CHTL_Compiler library
target_include_directories(CHTL_Compiler
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# The CHTL compiler needs utilities
target_link_libraries(CHTL_Compiler
    PRIVATE
    Util
)
