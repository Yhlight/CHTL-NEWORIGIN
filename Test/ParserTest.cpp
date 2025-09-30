#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ElementNode.h"

TEST_CASE("Parser Initialization", "[parser]") {
    std::vector<CHTL::Token> tokens;
    CHTL::CHTLParser parser(tokens);
    SUCCEED("Parser can be initialized.");
}

TEST_CASE("Parse Empty Element Node", "[parser]") {
    std::string input = "div {}";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getTagName() == "div");
    REQUIRE(elementNode->getChildren().empty());
}

TEST_CASE("Parse Nested Element Node", "[parser]") {
    std::string input = "div { span {} }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    // Check root node
    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* divNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");

    // Check child node
    REQUIRE(divNode->getChildren().size() == 1);
    CHTL::BaseNode* child = divNode->getChildren()[0].get();
    CHTL::ElementNode* spanNode = dynamic_cast<CHTL::ElementNode*>(child);
    REQUIRE(spanNode != nullptr);
    REQUIRE(spanNode->getTagName() == "span");
    REQUIRE(spanNode->getChildren().empty());
}

TEST_CASE("Parse Mixed Content", "[parser]") {
    std::string input = "body { h1 { text { \"Title\" } } p { text { \"Content\" } } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    // Check root node
    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* bodyNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(bodyNode != nullptr);
    REQUIRE(bodyNode->getTagName() == "body");
    REQUIRE(bodyNode->getChildren().size() == 2);

    // Check first child (h1)
    CHTL::ElementNode* h1Node = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[0].get());
    REQUIRE(h1Node != nullptr);
    REQUIRE(h1Node->getTagName() == "h1");
    REQUIRE(h1Node->getChildren().size() == 1);
    CHTL::TextNode* titleNode = dynamic_cast<CHTL::TextNode*>(h1Node->getChildren()[0].get());
    REQUIRE(titleNode != nullptr);
    REQUIRE(titleNode->getValue() == "Title");

    // Check second child (p)
    CHTL::ElementNode* pNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[1].get());
    REQUIRE(pNode != nullptr);
    REQUIRE(pNode->getTagName() == "p");
    REQUIRE(pNode->getChildren().size() == 1);
    CHTL::TextNode* contentNode = dynamic_cast<CHTL::TextNode*>(pNode->getChildren()[0].get());
    REQUIRE(contentNode != nullptr);
    REQUIRE(contentNode->getValue() == "Content");
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