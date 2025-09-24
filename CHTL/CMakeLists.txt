# This library will contain the core CHTL compiler logic (lexer, parser, generator, etc.)
add_library(CHTL_Lib STATIC
    CHTLContext/Context.cpp
    CHTLState/State.cpp
    CHTLState/ParsingState.cpp
    CHTLLexer/Lexer.cpp
    CHTLNode/ElementNode.cpp
    CHTLNode/TextNode.cpp
    CHTLNode/CommentNode.cpp
    CHTLParser/Parser.cpp
    CHTLLoader/Loader.cpp
    CHTLStrategy/ParseElementStrategy.cpp
    CHTLStrategy/ParseTemplateStrategy.cpp
    CHTLStrategy/ParseImportStrategy.cpp
    CHTLStrategy/ParseNamespaceStrategy.cpp
    CHTLStrategy/ParseConfigurationStrategy.cpp
    CHTLStrategy/ParseCustomStrategy.cpp
    CHTLGenerator/AstPrinter.cpp
    CHTLGenerator/CodeGenerator.cpp
)

# Add the current directory to the public include path for future headers
target_include_directories(CHTL_Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
