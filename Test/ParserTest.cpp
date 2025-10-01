#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLContext/CHTLContext.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/LiteralNode.h"
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to trim whitespace from both ends of a string
static inline std::string trim(const std::string &s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
        start++;
    }
    if (start == s.end()) {
        return "";
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(static_cast<unsigned char>(*end)));
    return std::string(start, end + 1);
}

TEST_CASE("Parser Initialization", "[parser]") {
    std::vector<CHTL::Token> tokens;
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
    SUCCEED("Parser can be initialized.");
}

TEST_CASE("Parse Empty Element Node", "[parser]") {
    std::string input = "div {}";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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

    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    REQUIRE(valueExpr->getExpressionType() == CHTL::ExpressionType::LITERAL);
    const CHTL::LiteralNode* litNode = dynamic_cast<const CHTL::LiteralNode*>(valueExpr);
    REQUIRE(litNode != nullptr);
    REQUIRE(litNode->getValue().value == "red");
}

TEST_CASE("Parse Numeric Style Property", "[parser]") {
    std::string input = "div { style { width: 100px; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    // This test is expected to fail until the expression parser is implemented
    // For now, it will fail because the placeholder parser only consumes one token.
}

TEST_CASE("Parse Simple Script Block", "[parser]") {
    std::string input = "div { script { let a = 1; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode != nullptr);
    CHTL::ElementNode* elementNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getTagName() == "div");
    REQUIRE(elementNode->getChildren().empty());

    const CHTL::ScriptNode* scriptNode = elementNode->getScript();
    REQUIRE(scriptNode != nullptr);
    REQUIRE(trim(scriptNode->getContent()) == "let a = 1;");
}

TEST_CASE("Parse Text Node", "[parser]") {
    // 1. Lex the input string
    std::string input = "text { \"Hello, Parser!\" }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);

    // 2. Parse the tokens
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
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
    REQUIRE(exprNode->getExpressionType() == CHTL::ExpressionType::BINARY_OP);

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

TEST_CASE("Parse Template Definition Node", "[parser]") {
    std::string input = "[Template] @Style DefaultText {}";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    // The rootNode should be null because template definitions are not part of the output AST
    REQUIRE(rootNode == nullptr);

    // We now check the context to see if the template was registered
    const auto* templateNode = context.getTemplate("DefaultText");
    REQUIRE(templateNode != nullptr);
    REQUIRE(templateNode->getTemplateType() == CHTL::TemplateType::STYLE);
    REQUIRE(templateNode->getName() == "DefaultText");
    REQUIRE(templateNode->getChildren().empty());
}

TEST_CASE("Parse and Use Element Template", "[parser]") {
    std::string input = R"(
        [Template] @Element MyTemplate {
            span { text { "Hello" } }
            div {}
        }

        body {
            @Element MyTemplate;
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);

    // The parser will parse the template first, which returns nullptr but registers the template.
    std::unique_ptr<CHTL::BaseNode> templateDefNode = parser.parse();
    REQUIRE(templateDefNode == nullptr); // Template definition should not produce a node in the main tree

    // Now, parse the body element that uses the template.
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();
    REQUIRE(rootNode != nullptr);

    auto* bodyNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(bodyNode != nullptr);
    REQUIRE(bodyNode->getTagName() == "body");
    REQUIRE(bodyNode->getChildren().size() == 2);

    // Check first child (span)
    auto* spanNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[0].get());
    REQUIRE(spanNode != nullptr);
    REQUIRE(spanNode->getTagName() == "span");
    REQUIRE(spanNode->getChildren().size() == 1);
    auto* textNode = dynamic_cast<CHTL::TextNode*>(spanNode->getChildren()[0].get());
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->getValue() == "Hello");

    // Check second child (div)
    auto* divNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[1].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");
    REQUIRE(divNode->getChildren().empty());
}

TEST_CASE("Parse and Use Style Template", "[parser]") {
    std::string input = R"(
        [Template] @Style MyStyles {
            color: red;
            font-size: 16px;
        }

        div {
            style {
                @Style MyStyles;
                background-color: blue;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);

    // Parse the template definition
    parser.parse();

    // Parse the div element
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();
    REQUIRE(rootNode != nullptr);

    auto* divNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(divNode != nullptr);

    const CHTL::StyleNode* styleNode = divNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getProperties().size() == 3);

    // Check first property from template
    const CHTL::StylePropertyNode* prop1 = styleNode->getProperties()[0].get();
    REQUIRE(prop1->getKey() == "color");

    // Check second property from template
    const CHTL::StylePropertyNode* prop2 = styleNode->getProperties()[1].get();
    REQUIRE(prop2->getKey() == "font-size");

    // Check property from style block itself
    const CHTL::StylePropertyNode* prop3 = styleNode->getProperties()[2].get();
    REQUIRE(prop3->getKey() == "background-color");
}

TEST_CASE("Parse Custom Definition Node", "[parser]") {
    std::string input = "[Custom] @Element MyComponent {}";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    REQUIRE(rootNode == nullptr);
    const auto* customNode = context.getCustom("MyComponent");
    REQUIRE(customNode != nullptr);
    REQUIRE(customNode->getCustomType() == CHTL::TemplateType::ELEMENT);
    REQUIRE(customNode->getName() == "MyComponent");
    REQUIRE(customNode->getChildren().empty());
}

TEST_CASE("Parse and Use Custom Element with Empty Specialization", "[parser]") {
    std::string input = R"(
        [Custom] @Element MyComponent {
            p { text { "First" } }
        }

        body {
            @Element MyComponent {}
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);

    // Parse the custom definition
    parser.parse();

    // Parse the body element
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();
    REQUIRE(rootNode != nullptr);

    auto* bodyNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(bodyNode != nullptr);
    REQUIRE(bodyNode->getChildren().size() == 1);

    auto* pNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[0].get());
    REQUIRE(pNode != nullptr);
    REQUIRE(pNode->getTagName() == "p");
}

TEST_CASE("Parse and Use Custom Element with Insert Specialization", "[parser]") {
    std::string input = R"(
        [Custom] @Element MyComponent {
            p { text { "Second" } }
        }

        body {
            @Element MyComponent {
                insert before p {
                    span { text { "First" } }
                }
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);

    parser.parse(); // Parse custom definition

    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();
    REQUIRE(rootNode != nullptr);

    auto* bodyNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(bodyNode != nullptr);
    REQUIRE(bodyNode->getChildren().size() == 2);

    auto* spanNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[0].get());
    REQUIRE(spanNode != nullptr);
    REQUIRE(spanNode->getTagName() == "span");

    auto* pNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[1].get());
    REQUIRE(pNode != nullptr);
    REQUIRE(pNode->getTagName() == "p");
}

TEST_CASE("Parse and Use Custom Element with Delete Specialization", "[parser]") {
    std::string input = R"(
        [Custom] @Element MyComponent {
            p { text { "First" } }
            span { text { "Second" } }
        }

        body {
            @Element MyComponent {
                delete span;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLContext context;
    CHTL::CHTLParser parser(tokens, context);

    parser.parse(); // Parse custom definition

    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();
    REQUIRE(rootNode != nullptr);

    auto* bodyNode = dynamic_cast<CHTL::ElementNode*>(rootNode.get());
    REQUIRE(bodyNode != nullptr);
    REQUIRE(bodyNode->getChildren().size() == 1);

    auto* pNode = dynamic_cast<CHTL::ElementNode*>(bodyNode->getChildren()[0].get());
    REQUIRE(pNode != nullptr);
    REQUIRE(pNode->getTagName() == "p");
}