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
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/ConditionalExpressionNode.h"
#include "CHTLNode/ValueListNode.h"

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

TEST_CASE("Parse Numeric Style Property", "[parser]") {
    std::string input = "div { style { width: 100px; } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> rootNode = parser.parse();

    // This test is expected to fail until the expression parser is implemented
    // For now, it will fail because the placeholder parser only consumes one token.
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

#include "CHTLManager/TemplateManager.h"

#include "CHTLManager/TemplateManager.h"

TEST_CASE("Parse Style Template Definition", "[parser]") {
    std::string input = R"(
        [Template] @Style MyTemplate {
            color: red;
            font-size: 16px;
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();

    // A file with only a template definition should not produce a root node
    REQUIRE(root == nullptr);

    // But it should register the template
    const auto* templateDef = CHTL::TemplateManager::getInstance().getTemplate("MyTemplate");
    REQUIRE(templateDef != nullptr);
    REQUIRE(templateDef->getTemplateType() == CHTL::TemplateType::STYLE);
    REQUIRE(templateDef->getName() == "MyTemplate");
    REQUIRE(templateDef->getChildren().size() == 2);

    auto* prop1 = dynamic_cast<const CHTL::StylePropertyNode*>(templateDef->getChildren()[0].get());
    REQUIRE(prop1 != nullptr);
    REQUIRE(prop1->getKey() == "color");

    auto* prop2 = dynamic_cast<const CHTL::StylePropertyNode*>(templateDef->getChildren()[1].get());
    REQUIRE(prop2 != nullptr);
    REQUIRE(prop2->getKey() == "font-size");
}

TEST_CASE("Parse Style Template Usage", "[parser]") {
    std::string input = R"(
        div {
            style {
                @Style MyTemplate;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();

    REQUIRE(root != nullptr);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(root.get());
    REQUIRE(elementNode != nullptr);

    const CHTL::StyleNode* styleNode = elementNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getTemplateUsages().size() == 1);

    const auto* templateUsage = styleNode->getTemplateUsages()[0].get();
    REQUIRE(templateUsage->getTemplateType() == CHTL::TemplateType::STYLE);
    REQUIRE(templateUsage->getName() == "MyTemplate");
}

TEST_CASE("Parse Style Property with Conditional Expression", "[parser]") {
    std::string input = "div { style { color: width > 100 ? 'red' : 'blue'; } }";
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
    REQUIRE(propNode->getKey() == "color");

    const CHTL::ExpressionNode* exprNode = propNode->getValue();
    REQUIRE(exprNode != nullptr);
    REQUIRE(exprNode->getType() == CHTL::ExpressionType::CONDITIONAL);

    const auto* condNode = dynamic_cast<const CHTL::ConditionalExpressionNode*>(exprNode);
    REQUIRE(condNode != nullptr);

    // Check the condition
    const auto* condition = dynamic_cast<const CHTL::BinaryOpNode*>(condNode->getCondition());
    REQUIRE(condition != nullptr);
    REQUIRE(condition->getOperator().type == CHTL::TokenType::GREATER);

    // Check the true expression
    const auto* trueExpr = dynamic_cast<const CHTL::LiteralNode*>(condNode->getTrueExpression());
    REQUIRE(trueExpr != nullptr);
    REQUIRE(trueExpr->getValue().value == "red");

    // Check the false expression
    const auto* falseExpr = dynamic_cast<const CHTL::LiteralNode*>(condNode->getFalseExpression());
    REQUIRE(falseExpr != nullptr);
    REQUIRE(falseExpr->getValue().value == "blue");
}

TEST_CASE("Parse Chained Conditional Expressions", "[parser]") {
    std::string input = "div { style { prop: a > 1 ? 'red', b > 2 ? 'blue'; } }";
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
    REQUIRE(propNode->getKey() == "prop");

    const CHTL::ExpressionNode* exprNode = propNode->getValue();
    REQUIRE(exprNode != nullptr);
    REQUIRE(exprNode->getType() == CHTL::ExpressionType::VALUE_LIST);

    const auto* valueListNode = dynamic_cast<const CHTL::ValueListNode*>(exprNode);
    REQUIRE(valueListNode != nullptr);
    REQUIRE(valueListNode->values.size() == 2);

    const auto* cond1 = dynamic_cast<const CHTL::ConditionalExpressionNode*>(valueListNode->values[0].get());
    REQUIRE(cond1 != nullptr);
    const auto* cond2 = dynamic_cast<const CHTL::ConditionalExpressionNode*>(valueListNode->values[1].get());
    REQUIRE(cond2 != nullptr);
}

TEST_CASE("Parse Conditional Expression with Optional Else", "[parser]") {
    std::string input = "div { style { prop: a > 1 ? 'red'; } }";
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
    REQUIRE(propNode->getKey() == "prop");

    const CHTL::ExpressionNode* exprNode = propNode->getValue();
    REQUIRE(exprNode != nullptr);
    REQUIRE(exprNode->getType() == CHTL::ExpressionType::CONDITIONAL);

    const auto* condNode = dynamic_cast<const CHTL::ConditionalExpressionNode*>(exprNode);
    REQUIRE(condNode != nullptr);
    REQUIRE(condNode->getFalseExpression() == nullptr);
}