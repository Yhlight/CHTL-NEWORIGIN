#include "TestUtils.h"
#include <stdexcept>
#include "../CHTL/CHTLStrategy/ImportParsingStrategy.h" // Add missing header
#include "../CHTL/CHTLParser/CHTLParserContext.h"     // Add missing header

// =================================================================================================
// Import System Tests
// =================================================================================================

TEST_CASE("Import: Full file with default namespace", "[import]") {
    std::string input = R"(
        [Import] @Chtl from "imports/default_namespace_lib.chtl";

        body {
            @Element MyDefaultTemplate from default_namespace_lib;
        }
    )";

    // The test runner runs from the root, so the base path for the loader should be Test/
    // We need a custom generateOutput function for this.
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::CHTLLexer lexer(input, configManager);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens, configManager);
    auto ast = parser.parse();

    CHTL::CHTLLoader loader("Test", configManager); // Set base path to "Test/"
    loader.loadImports(ast);

    CHTL::GenerationContext context;
    loader.gatherTemplates(ast, context);
    for (const auto& pair : loader.getLoadedAsts()) {
        loader.gatherTemplates(pair.second, context);
    }

    CHTL::ConcreteSaltBridge bridge;
    CHTL::CHTLGenerator generator;
    generator.generate(ast, context, &bridge);

    std::string expected_html = R"(
        <body>
            <p>Hello from the default namespace!</p>
        </body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Import: Full file with explicit namespace", "[import]") {
    std::string input = R"(
        [Import] @Chtl from "imports/explicit_namespace_lib.chtl";

        body {
            @Element MyExplicitTemplate from MyExplicitNS;
        }
    )";

    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::CHTLLexer lexer(input, configManager);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens, configManager);
    auto ast = parser.parse();

    CHTL::CHTLLoader loader("Test", configManager);
    loader.loadImports(ast);

    CHTL::GenerationContext context;
    loader.gatherTemplates(ast, context);
    for (const auto& pair : loader.getLoadedAsts()) {
        loader.gatherTemplates(pair.second, context);
    }

    CHTL::ConcreteSaltBridge bridge;
    CHTL::CHTLGenerator generator;
    generator.generate(ast, context, &bridge);

    std::string expected_html = R"(
        <body>
            <div style="color:blue;">Hello from the explicit namespace!</div>
        </body>
    )";
    REQUIRE(normalize_html(generator.getHtml()) == normalize_html(expected_html));
}

TEST_CASE("Parser: Handles import syntax correctly", "[import][parser]") {
    // This test now tests the ImportParsingStrategy in isolation.
    std::string input = R"([Template] @Element MyTemplate from "./my/path/lib.chtl" as MyAlias;)";

    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::CHTLLexer lexer(input, configManager);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParserContext context(tokens, configManager);
    CHTL::ImportParsingStrategy strategy;
    auto node = strategy.parse(&context);

    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_IMPORT);
    auto importNode = std::dynamic_pointer_cast<CHTL::ImportNode>(node);

    REQUIRE(importNode != nullptr);
    REQUIRE(importNode->getCategory() == CHTL::ImportCategory::TEMPLATE);
    REQUIRE(importNode->getItemType() == "@Element");
    REQUIRE(importNode->getItemName() == "MyTemplate");
    REQUIRE(importNode->getPath() == "./my/path/lib.chtl");
    REQUIRE(importNode->getAlias() == "MyAlias");
}

TEST_CASE("Loader: Throws on unresolved import path", "[import][loader]") {
    std::string input = R"(
        [Import] @Chtl from "non_existent_file.chtl";
    )";
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::CHTLLexer lexer(input, configManager);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens, configManager);
    auto ast = parser.parse();

    CHTL::CHTLLoader loader("Test", configManager);
    REQUIRE_THROWS_AS(loader.loadImports(ast), std::runtime_error);
}