#pragma once

#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLLoader/CHTLLoader.h"
#include "../CHTL/CHTLContext/GenerationContext.h"
#include "../CHTL/SemanticAnalyzer.h"
#include <vector>
#include <string>
#include <algorithm>

// Helper to perform the full compilation process and return the generator
inline CHTL::CHTLGenerator generateOutput(const std::string& input) {
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens);
    auto ast = parser.parse();

    CHTL::CHTLLoader loader(".");
    loader.loadImports(ast);

    CHTL::GenerationContext context;
    loader.gatherTemplates(ast, context);
    for (const auto& pair : loader.getLoadedAsts()) {
        loader.gatherTemplates(pair.second, context);
    }

    CHTL::SemanticAnalyzer semantic_analyzer;
    semantic_analyzer.analyze(ast);

    CHTL::CHTLGenerator generator;
    generator.generate(ast, context);

    return generator;
}