#pragma once

#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLLoader/CHTLLoader.h"
#include "../CHTL/CHTLContext/GenerationContext.h"
#include "../CHTL/SemanticAnalyzer.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"
#include "../SharedCore/ConcreteSaltBridge.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <memory>

// Helper to normalize HTML strings for comparison
inline std::string normalize_html(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); }), str.end());
    return str;
}

// Helper to normalize CSS strings for comparison
inline std::string normalize_css(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); }), str.end());
    return str;
}

// Helper to perform the full compilation process and return the generator
inline CHTL::CHTLGenerator generateOutput(const std::string& input) {
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();

    // --- First Pass: Find and apply configuration ---
    {
        CHTL::CHTLLexer initial_lexer(input, configManager);
        std::vector<CHTL::Token> initial_tokens;
        CHTL::Token token = initial_lexer.getNextToken();
        while (token.type != CHTL::TokenType::TOKEN_EOF) {
            initial_tokens.push_back(token);
            token = initial_lexer.getNextToken();
        }

        CHTL::CHTLParser initial_parser(initial_tokens, configManager);
        auto initial_ast = initial_parser.parse();

        for (const auto& child : initial_ast->getChildren()) {
            if (child->getType() == CHTL::NodeType::NODE_CONFIG) {
                if (auto config_node = std::dynamic_pointer_cast<CHTL::ConfigurationNode>(child)) {
                    configManager->update(config_node);
                    break;
                }
            }
        }
    }

    // --- Second Pass: Main compilation with updated configuration ---
    CHTL::CHTLLexer main_lexer(input, configManager);
    std::vector<CHTL::Token> main_tokens;
    CHTL::Token main_token = main_lexer.getNextToken();
    while (main_token.type != CHTL::TokenType::TOKEN_EOF) {
        main_tokens.push_back(main_token);
        main_token = main_lexer.getNextToken();
    }

    CHTL::CHTLParser main_parser(main_tokens, configManager);
    auto ast = main_parser.parse();

    CHTL::CHTLLoader loader(".", configManager);
    loader.loadImports(ast);

    CHTL::SemanticAnalyzer semantic_analyzer;
    semantic_analyzer.analyze(ast);

    CHTL::GenerationContext context;
    loader.gatherTemplates(ast, context);
    for (const auto& pair : loader.getLoadedAsts()) {
        loader.gatherTemplates(pair.second, context);
    }

    CHTL::ConcreteSaltBridge bridge;
    CHTL::CHTLGenerator generator;
    generator.generate(ast, context, &bridge);

    return generator;
}