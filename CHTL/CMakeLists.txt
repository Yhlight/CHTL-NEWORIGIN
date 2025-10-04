add_library(CHTL STATIC
    CHTLLexer/CHTLLexer.cpp
    CHTLLoader/CHTLLoader.cpp
    CHTLParser/CHTLParser.cpp
    CHTLParser/CHTLParserContext.cpp
    CHTLParser/ParsingUtils.cpp
    CHTLState/InitialState.cpp
    CHTLStrategy/ElementParsingStrategy.cpp
    CHTLStrategy/TextParsingStrategy.cpp
    CHTLStrategy/StyleParsingStrategy.cpp
    CHTLStrategy/TemplateParsingStrategy.cpp
    CHTLStrategy/CustomParsingStrategy.cpp
    CHTLStrategy/OriginParsingStrategy.cpp
    CHTLStrategy/ImportParsingStrategy.cpp
    CHTLStrategy/NamespaceParsingStrategy.cpp
    CHTLStrategy/ConfigurationParsingStrategy.cpp
    CHTLStrategy/UseParsingStrategy.cpp
    CHTLStrategy/IfParsingStrategy.cpp
    CHTLStrategy/ConstraintParsingStrategy.cpp
    CHTLStrategy/SpecializationParsingStrategy.cpp
    CHTLStrategy/ScriptParsingStrategy.cpp
    CHTLGenerator/CHTLGenerator.cpp
    CHTLContext/ConfigurationManager.cpp
    SemanticAnalyzer.cpp
)

target_include_directories(CHTL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/SharedCore
)