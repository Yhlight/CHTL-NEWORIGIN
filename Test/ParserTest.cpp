#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/LiteralNode.h"

TEST_CASE("Parser Initialization with State Machine", "[parser_state_machine]") {
    std::vector<CHTL::Token> tokens;
    CHTL::CHTLParser parser(tokens);
    auto root = parser.parse();
    REQUIRE(root != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(root.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getTagName() == "<root>");
    SUCCEED("Parser can be initialized.");
}

TEST_CASE("Parse Single Empty Element with State Machine", "[parser_state_machine]") {
    std::string input = "div {}";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getTagName() == "<root>");
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");
    REQUIRE(divNode->getChildren().empty());
}

TEST_CASE("Parse Generator-Aware Comment", "[parser_features]") {
    std::string input = "div { # This is a generator comment }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getChildren().size() == 1);

    auto commentNode = dynamic_cast<CHTL::CommentNode*>(divNode->getChildren()[0].get());
    REQUIRE(commentNode != nullptr);
    REQUIRE(commentNode->getValue() == "This is a generator comment ");
}

TEST_CASE("Parser Ignores Multi-Line Block Comments", "[parser_features]") {
    std::string input = R"(
        /* This is a
           multi-line comment. */
        div {
            /* Another comment */
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");
    REQUIRE(divNode->getChildren().empty());
}

TEST_CASE("Parse Element with a Single Attribute", "[parser_features]") {
    std::string input = "div { id: \"my-div\"; }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getAttributes().size() == 1);
    REQUIRE(divNode->getAttribute("id") == "my-div");
}

TEST_CASE("Parse Attribute with Equal Sign", "[parser_features]") {
    std::string input = "a { href = \"#top\"; }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement->getChildren().size() == 1);

    auto aNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(aNode != nullptr);
    REQUIRE(aNode->getAttributes().size() == 1);
    REQUIRE(aNode->getAttribute("href") == "#top");
}

TEST_CASE("Parse Attribute with Unquoted Literal", "[parser_features]") {
    std::string input = "div { class: card-body; }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getAttributes().size() == 1);
    REQUIRE(divNode->getAttribute("class") == "card-body");
}

TEST_CASE("Parse Element with a Local Style Block", "[parser_features]") {
    std::string input = "div { style { color: red; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);

    const CHTL::StyleNode* styleNode = divNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getProperties().size() == 1);

    const auto* propNode = styleNode->getProperties()[0].get();
    REQUIRE(propNode->getKey() == "color");

    const auto* valueNode = dynamic_cast<const CHTL::LiteralNode*>(propNode->getValue());
    REQUIRE(valueNode != nullptr);
    REQUIRE(valueNode->getValue().value == "red");
}

TEST_CASE("Parse Element with a Local Script Block", "[parser_features]") {
    std::string input = "div { script { let x = 1; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);

    const CHTL::ScriptNode* scriptNode = divNode->getScript();
    REQUIRE(scriptNode != nullptr);
    REQUIRE(scriptNode->getContent() == " let x = 1; ");
}

TEST_CASE("Parse Nested Elements with State Machine", "[parser_state_machine]") {
    std::string input = "html { body { div {} } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    // Check root node
    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getTagName() == "<root>");
    REQUIRE(rootElement->getChildren().size() == 1);

    // Check html node
    auto htmlNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(htmlNode != nullptr);
    REQUIRE(htmlNode->getTagName() == "html");
    REQUIRE(htmlNode->getChildren().size() == 1);

    // Check body node
    auto bodyNode = dynamic_cast<CHTL::ElementNode*>(htmlNode->getChildren()[0].get());
    REQUIRE(bodyNode != nullptr);
    REQUIRE(bodyNode->getTagName() == "body");
    REQUIRE(bodyNode->getChildren().size() == 1);

    // Check div node
    auto divNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");
    REQUIRE(divNode->getChildren().empty());
}

TEST_CASE("Parse Multiple Top-Level Elements with State Machine", "[parser_state_machine]") {
    std::string input = "div {} span {}";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getChildren().size() == 2);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");

    auto spanNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[1].get());
    REQUIRE(spanNode != nullptr);
    REQUIRE(spanNode->getTagName() == "span");
}

TEST_CASE("Parse Element with Text Node using Strategies", "[parser_strategy]") {
    std::string input = "p { text { \"Hello, Strategy!\" } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getChildren().size() == 1);

    auto pNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(pNode != nullptr);
    REQUIRE(pNode->getTagName() == "p");
    REQUIRE(pNode->getChildren().size() == 1);

    auto textNode = dynamic_cast<CHTL::TextNode*>(pNode->getChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->getValue() == "Hello, Strategy!");
}

TEST_CASE("Parser Ignores Single-Line Comments", "[parser_features]") {
    std::string input = R"(
        // This is a top-level comment.
        div { // This comment is next to a brace.
            // This is a comment inside an element.
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto rootElement = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(rootElement != nullptr);
    REQUIRE(rootElement->getChildren().size() == 1);

    auto divNode = dynamic_cast<CHTL::ElementNode*>(rootElement->getChildren()[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");
    REQUIRE(divNode->getChildren().empty());
}