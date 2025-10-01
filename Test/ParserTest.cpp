#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/LiteralNode.h"

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

TEST_CASE("Parse Simple Attribute", "[parser]") {
    std::string input = "div { id: \"my-id\"; }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getTagName() == "div");
    REQUIRE(elementNode->getChildren().empty());
    REQUIRE(elementNode->getAttributes().size() == 1);
    REQUIRE(elementNode->getAttribute("id") == "my-id");
}

TEST_CASE("Parse Attribute with Equal Sign", "[parser]") {
    std::string input = "div { id = \"my-id\"; }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getAttributes().size() == 1);
    REQUIRE(elementNode->getAttribute("id") == "my-id");
}

TEST_CASE("Parse Attribute with Unquoted Literal", "[parser]") {
    std::string input = "div { class: box; }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getAttributes().size() == 1);
    REQUIRE(elementNode->getAttribute("class") == "box");
}

TEST_CASE("Parse Simple Style Block", "[parser]") {
    std::string input = "div { style { color: red; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);

    const CHTL::StyleNode* styleNode = elementNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getProperties().size() == 1);

    const CHTL::StylePropertyNode* propNode = styleNode->getProperties()[0].get();
    REQUIRE(propNode->getKey() == "color");

    const CHTL::ExpressionNode* valueExpr = propNode->getValue();
    REQUIRE(valueExpr != nullptr);
    REQUIRE(valueExpr->getType() == CHTL::ExpressionType::LITERAL);
    const CHTL::LiteralNode* litNode = dynamic_cast<const CHTL::LiteralNode*>(valueExpr);
    REQUIRE(litNode != nullptr);
    REQUIRE(litNode->getValue().value == "red");
}

TEST_CASE("Parse Style Property with Dimension", "[parser]") {
    std::string input = "div { style { width: 100px; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);

    const CHTL::StyleNode* styleNode = elementNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getProperties().size() == 1);

    const CHTL::StylePropertyNode* propNode = styleNode->getProperties()[0].get();
    REQUIRE(propNode->getKey() == "width");

    const CHTL::ExpressionNode* valueExpr = propNode->getValue();
    REQUIRE(valueExpr != nullptr);
    REQUIRE(valueExpr->getType() == CHTL::ExpressionType::LITERAL);
    const CHTL::LiteralNode* litNode = dynamic_cast<const CHTL::LiteralNode*>(valueExpr);
    REQUIRE(litNode != nullptr);
    REQUIRE(litNode->getValue().type == CHTL::TokenType::DIMENSION);
    REQUIRE(litNode->getValue().value == "100px");
}

TEST_CASE("Parse Simple Script Block", "[parser]") {
    std::string input = "div { script { let a = 1; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getTagName() == "div");
    REQUIRE(elementNode->getChildren().empty());

    const CHTL::ScriptNode* scriptNode = elementNode->getScript();
    REQUIRE(scriptNode != nullptr);
    REQUIRE(scriptNode->getContent() == " let a = 1; ");
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

TEST_CASE("Parse Style Property with Simple Arithmetic Expression", "[parser]") {
    std::string input = "div { style { width: 100 + 50; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);

    const CHTL::StyleNode* styleNode = elementNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getProperties().size() == 1);

    const CHTL::StylePropertyNode* propNode = styleNode->getProperties()[0].get();
    REQUIRE(propNode->getKey() == "width");

    const CHTL::ExpressionNode* exprNode = propNode->getValue();
    REQUIRE(exprNode != nullptr);
    REQUIRE(exprNode->getType() == CHTL::ExpressionType::BINARY_OP);

    const auto* binOpNode = dynamic_cast<const CHTL::BinaryOpNode*>(exprNode);
    REQUIRE(binOpNode != nullptr);
    REQUIRE(binOpNode->getOperator().type == CHTL::TokenType::PLUS);

    const auto* leftLit = dynamic_cast<const CHTL::LiteralNode*>(binOpNode->getLeft());
    REQUIRE(leftLit != nullptr);
    REQUIRE(leftLit->getValue().type == CHTL::TokenType::NUMBER);
    REQUIRE(leftLit->getValue().value == "100");

    const auto* rightLit = dynamic_cast<const CHTL::LiteralNode*>(binOpNode->getRight());
    REQUIRE(rightLit != nullptr);
    REQUIRE(rightLit->getValue().type == CHTL::TokenType::NUMBER);
    REQUIRE(rightLit->getValue().value == "50");
}

TEST_CASE("Parse Text Attribute as TextNode", "[parser]") {
    std::string input = "div { text: \"hello world\"; }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getTagName() == "div");

    // Ensure it's not treated as a regular attribute
    REQUIRE(elementNode->getAttributes().find("text") == elementNode->getAttributes().end());

    // Ensure a TextNode child was created
    REQUIRE(elementNode->getChildren().size() == 1);
    CHTL::TextNode* textNode = dynamic_cast<CHTL::TextNode*>(elementNode->getChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->getValue() == "hello world");
}

TEST_CASE("Parse 'text' as an element tag with attributes", "[parser]") {
    std::string input = "div { text { id: \"my-text\"; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* divNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");

    REQUIRE(divNode->getChildren().size() == 1);
    CHTL::ElementNode* textElementNode = dynamic_cast<CHTL::ElementNode*>(divNode->getChildren()[0].get());
    REQUIRE(textElementNode != nullptr);
    REQUIRE(textElementNode->getTagName() == "text");
    REQUIRE(textElementNode->getAttributes().size() == 1);
    REQUIRE(textElementNode->getAttribute("id") == "my-text");
}