# CHTL Compiler Library

set(CHTL_SOURCES
    CHTLContext.cpp
    CHTLLexer/Lexer.cpp
    CHTLLexer/Token.cpp
    CHTLLexer/GlobalMap.cpp
    CHTLParser/Parser.cpp
    CHTLNode/BaseNode.cpp
    CHTLNode/ElementNode.cpp
    CHTLNode/TextNode.cpp
    CHTLNode/CommentNode.cpp
    CHTLNode/StyleNode.cpp
    CHTLNode/ScriptNode.cpp
    CHTLNode/TemplateNode.cpp
    CHTLNode/CustomNode.cpp
    CHTLNode/OriginNode.cpp
    CHTLNode/ImportNode.cpp
    CHTLNode/ConfigNode.cpp
    CHTLNode/NamespaceNode.cpp
    CHTLGenerator/Generator.cpp
    CHTLState/State.cpp
    CHTLStrategy/Strategy.cpp
    CHTLLoader/Loader.cpp
)

set(CHTL_HEADERS
    CHTLContext.h
    CHTLLexer/Lexer.h
    CHTLLexer/Token.h
    CHTLLexer/GlobalMap.h
    CHTLParser/Parser.h
    CHTLNode/BaseNode.h
    CHTLNode/ElementNode.h
    CHTLNode/TextNode.h
    CHTLNode/CommentNode.h
    CHTLNode/StyleNode.h
    CHTLNode/ScriptNode.h
    CHTLNode/TemplateNode.h
    CHTLNode/CustomNode.h
    CHTLNode/OriginNode.h
    CHTLNode/ImportNode.h
    CHTLNode/ConfigNode.h
    CHTLNode/NamespaceNode.h
    CHTLGenerator/Generator.h
    CHTLState/State.h
    CHTLStrategy/Strategy.h
    CHTLLoader/Loader.h
)

add_library(chtl_lib STATIC ${CHTL_SOURCES} ${CHTL_HEADERS})

target_include_directories(chtl_lib 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/third-party
)

target_link_libraries(chtl_lib 
    PUBLIC 
        shared_core
        util_lib
)
