#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/StyleNode.h"
#include "../CHTL/CHTLNode/TemplateNode.h"
#include "../CHTL/CHTLNode/CustomNode.h"
#include "../CHTL/CHTLNode/PropertyNode.h"
#include "../CHTL/CHTLNode/OriginNode.h"

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

TEST_CASE("Parser handles style blocks", "[parser]") {
    std::string input = R"(
div {
    style {
        color: red;
        width: 100px;
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
    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(root);
    REQUIRE(element->getTagName() == "div");
    REQUIRE(element->getChildren().size() == 1);

    auto styleNode = std::dynamic_pointer_cast<CHTL::StyleNode>(element->getChildren()[0]);
    REQUIRE(styleNode != nullptr);
    auto properties = styleNode->getProperties();
    REQUIRE(properties.size() == 2);
    REQUIRE(properties[0]->getKey() == "color");
    REQUIRE(properties[0]->getValue() == "red");
    REQUIRE(properties[1]->getKey() == "width");
    REQUIRE(properties[1]->getValue() == "100px");
}

TEST_CASE("Parser handles style template", "[parser]") {
    std::string input = R"(
[Template] @Style DefaultText {
    color: black;
    font-size: 16px;
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
    REQUIRE(root->getType() == CHTL::NodeType::NODE_TEMPLATE);
    auto templateNode = std::dynamic_pointer_cast<CHTL::TemplateNode>(root);
    REQUIRE(templateNode != nullptr);
    REQUIRE(templateNode->getName() == "DefaultText");
    REQUIRE(templateNode->getTemplateType() == CHTL::TemplateType::STYLE);
    REQUIRE(templateNode->getChildren().size() == 2);
    auto prop1 = std::dynamic_pointer_cast<CHTL::PropertyNode>(templateNode->getChildren()[0]);
    REQUIRE(prop1->getKey() == "color");
    REQUIRE(prop1->getValue() == "black");
    auto prop2 = std::dynamic_pointer_cast<CHTL::PropertyNode>(templateNode->getChildren()[1]);
    REQUIRE(prop2->getKey() == "font-size");
    REQUIRE(prop2->getValue() == "16px");
}

TEST_CASE("Parser handles element template", "[parser]") {
    std::string input = R"(
[Template] @Element MyElement {
    div {
        text { "I am a template" }
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
    REQUIRE(root->getType() == CHTL::NodeType::NODE_TEMPLATE);
    auto templateNode = std::dynamic_pointer_cast<CHTL::TemplateNode>(root);
    REQUIRE(templateNode != nullptr);
    REQUIRE(templateNode->getName() == "MyElement");
    REQUIRE(templateNode->getTemplateType() == CHTL::TemplateType::ELEMENT);
    REQUIRE(templateNode->getChildren().size() == 1);
    auto div = std::dynamic_pointer_cast<CHTL::ElementNode>(templateNode->getChildren()[0]);
    REQUIRE(div != nullptr);
    REQUIRE(div->getTagName() == "div");
}

TEST_CASE("Parser handles custom style block", "[parser]") {
    std::string input = R"(
[Custom] @Style MyCustomStyle {
    color: red;
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
    REQUIRE(root->getType() == CHTL::NodeType::NODE_CUSTOM);
    auto customNode = std::dynamic_pointer_cast<CHTL::CustomNode>(root);
    REQUIRE(customNode != nullptr);
    REQUIRE(customNode->getName() == "MyCustomStyle");
    REQUIRE(customNode->getCustomType() == CHTL::CustomType::STYLE);
    REQUIRE(customNode->getChildren().size() == 1);
    auto prop1 = std::dynamic_pointer_cast<CHTL::PropertyNode>(customNode->getChildren()[0]);
    REQUIRE(prop1->getKey() == "color");
    REQUIRE(prop1->getValue() == "red");
}

TEST_CASE("Parser handles origin block", "[parser]") {
    std::string input = R"(
[Origin] @Html {
    <div>Hello</div>
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
    REQUIRE(root->getType() == CHTL::NodeType::NODE_ORIGIN);
    auto originNode = std::dynamic_pointer_cast<CHTL::OriginNode>(root);
    REQUIRE(originNode != nullptr);
    REQUIRE(originNode->getOriginType() == "@Html");
    REQUIRE(originNode->getName() == "");
    REQUIRE(originNode->getContent() == "<div>Hello</div>");
}

TEST_CASE("Parser handles named origin block", "[parser]") {
    std::string input = R"(
[Origin] @JavaScript myScript {
    console.log("Hello");
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
    REQUIRE(root->getType() == CHTL::NodeType::NODE_ORIGIN);
    auto originNode = std::dynamic_pointer_cast<CHTL::OriginNode>(root);
    REQUIRE(originNode != nullptr);
    REQUIRE(originNode->getOriginType() == "@JavaScript");
    REQUIRE(originNode->getName() == "myScript");
    REQUIRE(originNode->getContent() == "console.log(Hello);");
}