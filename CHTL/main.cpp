#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLLoader/CHTLLoader.h"
#include "CHTLContext/GenerationContext.h"
#include "SemanticAnalyzer.h"
#include "CHTLContext/ConfigurationManager.h"
#include "SharedCore/ConcreteSaltBridge.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

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
    std::shared_ptr<CHTL::BaseNode> ast = main_parser.parse();

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

    std::string html = generator.getHtml();
    std::string css = generator.getCss();

    // Combine HTML and CSS for self-contained output
    std::stringstream final_html;
    final_html << "<html><head><style>" << css << "</style></head><body>" << html << "</body></html>";

    std::cout << final_html.str() << std::endl;

    return 0;
}