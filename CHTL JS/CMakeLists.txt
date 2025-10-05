add_library(CHTLJS STATIC
    CHTLJSLexer/CHTLJSLexer.cpp
    CHTLJSParser/CHTLJSParser.cpp
    CHTLJSGenerator/CHTLJSGenerator.cpp
)

target_include_directories(CHTLJS PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/SharedCore
)