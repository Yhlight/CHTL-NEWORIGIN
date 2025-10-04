#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLLoader/CHTLLoader.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include <fstream>
#include <vector>
#include <string>

// Helper to parse input string into an AST
std::shared_ptr<CHTL::BaseNode> parseString(const std::string& input) {
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens);
    return parser.parse();
}

TEST_CASE("Loader handles a simple import", "[loader]") {
    // Create a temporary file to be imported
    std::ofstream outfile("test_import.chtl");
    outfile << "div { text { \"imported\" } }";
    outfile.close();

    // Main file content that imports the temporary file
    std::string mainInput = R"([Import] @Chtl from "test_import.chtl";)";
    auto mainAst = parseString(mainInput);

    CHTL::CHTLLoader loader("."); // Base path is the current directory
    loader.loadImports(mainAst);

    auto loadedAsts = loader.getLoadedAsts();
    REQUIRE(loadedAsts.size() == 1);

    auto importedAst = loadedAsts.at("./test_import.chtl");
    REQUIRE(importedAst != nullptr);
    REQUIRE(importedAst->getChildren().size() == 1);
    auto importedNode = importedAst->getChildren()[0];
    REQUIRE(importedNode->getType() == CHTL::NodeType::NODE_ELEMENT);

    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(importedNode);
    REQUIRE(element->getTagName() == "div");

    // Clean up the temporary file
    remove("test_import.chtl");
}