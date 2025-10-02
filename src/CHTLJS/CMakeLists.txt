# CHTL JS Compiler Library

set(CHTLJS_SOURCES
    CHTLJSContext.cpp
    CHTLJSLexer/Lexer.cpp
    CHTLJSLexer/Token.cpp
    CHTLJSLexer/GlobalMap.cpp
    CHTLJSParser/Parser.cpp
    CHTLJSNode/BaseNode.cpp
    CHTLJSGenerator/Generator.cpp
    CHTLJSState/State.cpp
    CHTLJSStrategy/Strategy.cpp
    CHTLJSLoader/Loader.cpp
)

set(CHTLJS_HEADERS
    CHTLJSContext.h
    CHTLJSLexer/Lexer.h
    CHTLJSLexer/Token.h
    CHTLJSLexer/GlobalMap.h
    CHTLJSParser/Parser.h
    CHTLJSNode/BaseNode.h
    CHTLJSGenerator/Generator.h
    CHTLJSState/State.h
    CHTLJSStrategy/Strategy.h
    CHTLJSLoader/Loader.h
)

add_library(chtljs_lib STATIC ${CHTLJS_SOURCES} ${CHTLJS_HEADERS})

target_include_directories(chtljs_lib 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/third-party
)

target_link_libraries(chtljs_lib 
    PUBLIC 
        shared_core
        util_lib
)
