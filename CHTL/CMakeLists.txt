add_library(CHTL STATIC
    CHTLLexer/CHTLLexer.cpp
    CHTLParser/CHTLParser.cpp
    CHTLParser/CHTLParserContext.cpp
    CHTLState/InitialState.cpp
    CHTLStrategy/ElementParsingStrategy.cpp
    CHTLStrategy/TextParsingStrategy.cpp
    CHTLStrategy/StyleParsingStrategy.cpp
    CHTLStrategy/TemplateParsingStrategy.cpp
    CHTLStrategy/CustomParsingStrategy.cpp
    CHTLStrategy/OriginParsingStrategy.cpp
    CHTLGenerator/CHTLGenerator.cpp
)

target_include_directories(CHTL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)