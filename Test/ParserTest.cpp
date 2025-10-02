#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"

#include <vector>
#include <string>

TEST_CASE("Parser handles simple element", "[parser]") {
    std::string input = "div {}";
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();

    REQUIRE(root != nullptr);
    REQUIRE(root->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(root);
    REQUIRE(element->getTagName() == "div");
}

TEST_CASE("Parser handles attributes", "[parser]") {
    std::string input = R"(div { id: "main"; class: "container"; })";
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();

    REQUIRE(root != nullptr);
    REQUIRE(root->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(root);
    REQUIRE(element->getTagName() == "div");
    auto attributes = element->getAttributes();
    REQUIRE(attributes.size() == 2);
    REQUIRE(attributes.at("id") == "main");
    REQUIRE(attributes.at("class") == "container");
}

TEST_CASE("Parser handles nested elements", "[parser]") {
    std::string input = R"(
body {
    div {
        span {}
    }
}
)";
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();

    REQUIRE(root != nullptr);
    REQUIRE(root->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto body = std::dynamic_pointer_cast<CHTL::ElementNode>(root);
    REQUIRE(body->getTagName() == "body");
    REQUIRE(body->getChildren().size() == 1);
    auto div = std::dynamic_pointer_cast<CHTL::ElementNode>(body->getChildren()[0]);
    REQUIRE(div->getTagName() == "div");
    REQUIRE(div->getChildren().size() == 1);
    auto span = std::dynamic_pointer_cast<CHTL::ElementNode>(div->getChildren()[0]);
    REQUIRE(span->getTagName() == "span");
}

TEST_CASE("Parser handles text nodes", "[parser]") {
    std::string input = R"(p { text { "Hello, World!" } })";
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();

    REQUIRE(root != nullptr);
    REQUIRE(root->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto p = std::dynamic_pointer_cast<CHTL::ElementNode>(root);
    REQUIRE(p->getTagName() == "p");
    REQUIRE(p->getChildren().size() == 1);
    auto text = std::dynamic_pointer_cast<CHTL::TextNode>(p->getChildren()[0]);
    REQUIRE(text != nullptr);
    REQUIRE(text->getContent() == "Hello, World!");
}