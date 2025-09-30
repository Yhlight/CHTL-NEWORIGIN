#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/TextNode.h"

TEST_CASE("Parser Initialization", "[parser]") {
    std::vector<CHTL::Token> tokens;
    CHTL::CHTLParser parser(tokens);
    SUCCEED("Parser can be initialized.");
}

TEST_CASE("Parse Text Node", "[parser]") {
    // 1. Lex the input string
    std::string input = "text { \"Hello, Parser!\" }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    // 2. Parse the tokens
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    // 3. Assert the results
    REQUIRE(rootNode != nullptr);
    CHTL::TextNode* textNode = dynamic_cast<CHTL::TextNode*>(rootNode.get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->getValue() == "Hello, Parser!");
}