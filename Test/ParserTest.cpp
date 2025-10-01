#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/LiteralNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/CustomDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/VariableAccessNode.h"
#include "CHTLNode/DeleteNode.h"
#include "CHTLNode/InsertNode.h"
#include "CHTLNode/OriginNode.h"
#include "CHTLManager/TemplateManager.h"

TEST_CASE("Parser Initialization", "[parser]") {
    std::string input = "";
    std::vector<CHTL::Token> tokens;
    CHTL::CHTLParser parser(input, tokens);
    SUCCEED("Parser can be initialized.");
}

TEST_CASE("Parser correctly handles 'use html5;' directive", "[parser]") {
    std::string input = "use html5; div {}";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    std::unique_ptr<CHTL::DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    REQUIRE(doc->useHtml5 == true);
    REQUIRE(doc->children.size() == 1);
}

TEST_CASE("Parse Var Template Definition", "[parser]") {
    std::string input = R"(
        [Template] @Var MyVars {
            mainColor: #ff0000;
            secondaryColor: blue;
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    auto doc = parser.parse();

    REQUIRE(doc != nullptr);
    REQUIRE(doc->children.empty());

    const auto* templateDef = dynamic_cast<const CHTL::TemplateDefinitionNode*>(CHTL::TemplateManager::getInstance().getTemplate("MyVars"));
    REQUIRE(templateDef != nullptr);
    REQUIRE(templateDef->getTemplateType() == CHTL::TemplateType::VAR);
    REQUIRE(templateDef->getName() == "MyVars");
    REQUIRE(templateDef->getChildren().size() == 2);
}

TEST_CASE("Parse Variable Access", "[parser]") {
    std::string input = "div { style { color: MyVars(mainColor); } }";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    auto doc = parser.parse();

    REQUIRE(doc != nullptr);
    REQUIRE(doc->children.size() == 1);
    auto* root = doc->children[0].get();
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(root);
    REQUIRE(elementNode != nullptr);

    const CHTL::StyleNode* styleNode = elementNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getProperties().size() == 1);

    const CHTL::StylePropertyNode* propNode = styleNode->getProperties()[0].get();
    const CHTL::ExpressionNode* valueExpr = propNode->getValue();
    REQUIRE(valueExpr->getType() == CHTL::ExpressionType::VARIABLE_ACCESS);

    const auto* varAccess = static_cast<const CHTL::VariableAccessNode*>(valueExpr);
    REQUIRE(varAccess->getGroupName() == "MyVars");
    REQUIRE(varAccess->getVariableName() == "mainColor");
}

TEST_CASE("Parse Style Template with Inheritance", "[parser]") {
    CHTL::TemplateManager::getInstance().clear();
    std::string input = R"(
        [Template] @Style Base { color: red; }
        [Template] @Style Derived {
            inherit @Style Base;
            font-size: 16px;
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    parser.parse(); // This will register the templates

    const auto* derivedTemplate = dynamic_cast<const CHTL::TemplateDefinitionNode*>(CHTL::TemplateManager::getInstance().getTemplate("Derived"));
    REQUIRE(derivedTemplate != nullptr);
    REQUIRE(derivedTemplate->getChildren().size() == 2);

    const auto* usageNode = dynamic_cast<const CHTL::TemplateUsageNode*>(derivedTemplate->getChildren()[0].get());
    REQUIRE(usageNode != nullptr);
    REQUIRE(usageNode->getName() == "Base");

    const auto* propNode = dynamic_cast<const CHTL::StylePropertyNode*>(derivedTemplate->getChildren()[1].get());
    REQUIRE(propNode != nullptr);
    REQUIRE(propNode->getKey() == "font-size");
}

TEST_CASE("Parse Custom Style Template with Delete", "[parser]") {
    CHTL::TemplateManager::getInstance().clear();
    std::string input = R"(
        [Custom] @Style MyCustomStyle {
            delete color, font-size;
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    parser.parse();

    const auto* customDef = dynamic_cast<const CHTL::CustomDefinitionNode*>(CHTL::TemplateManager::getInstance().getTemplate("MyCustomStyle"));
    REQUIRE(customDef != nullptr);
    REQUIRE(customDef->getChildren().size() == 1);
    const auto* deleteNode = dynamic_cast<const CHTL::DeleteNode*>(customDef->getChildren()[0].get());
    REQUIRE(deleteNode != nullptr);
    REQUIRE(deleteNode->getTargets().size() == 2);
    REQUIRE(deleteNode->getTargets()[0] == "color");
    REQUIRE(deleteNode->getTargets()[1] == "font-size");
}

TEST_CASE("Parse Specialized Template Usage", "[parser]") {
    std::string input = R"(
        div {
            style {
                @Style MyTemplate {
                    delete color;
                }
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    auto doc = parser.parse();

    REQUIRE(doc != nullptr);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(doc->children[0].get());
    REQUIRE(elementNode != nullptr);
    const auto* styleNode = elementNode->getStyle();
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getTemplateUsages().size() == 1);
    const auto* usageNode = styleNode->getTemplateUsages()[0].get();
    REQUIRE(usageNode->getSpecializations().size() == 1);
    const auto* deleteNode = dynamic_cast<const CHTL::DeleteNode*>(usageNode->getSpecializations()[0].get());
    REQUIRE(deleteNode != nullptr);
    REQUIRE(deleteNode->getTargets().size() == 1);
    REQUIRE(deleteNode->getTargets()[0] == "color");
}

TEST_CASE("Parse Origin Block", "[parser]") {
    std::string input = R"(
        [Origin] @Html {<div class="raw">Raw HTML</div>}
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    auto doc = parser.parse();

    REQUIRE(doc != nullptr);
    REQUIRE(doc->children.size() == 1);
    auto* originNode = dynamic_cast<CHTL::OriginNode*>(doc->children[0].get());
    REQUIRE(originNode != nullptr);
    REQUIRE(originNode->getOriginType() == CHTL::OriginType::HTML);
    REQUIRE(originNode->getContent() == "<div class=\"raw\">Raw HTML</div>");
}

TEST_CASE("Parse simple if statement", "[parser]") {
    std::string input = R"(
        div {
            if {
                condition: width > 100px,
                color: red;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    auto doc = parser.parse();

    REQUIRE(doc != nullptr);
    REQUIRE(doc->children.size() == 1);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(doc->children[0].get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getIfBlocks().size() == 1);

    const auto* ifNode = elementNode->getIfBlocks()[0].get();
    REQUIRE(ifNode != nullptr);
    REQUIRE(ifNode->body.size() == 1);

    const auto* propNode = ifNode->body[0].get();
    REQUIRE(propNode->getKey() == "color");
}

TEST_CASE("Parse if-else-if-else chain", "[parser]") {
    std::string input = R"(
        div {
            if {
                condition: width > 200px,
                color: blue;
            } else if {
                condition: width > 100px,
                color: green;
            } else {
                color: red;
            }
        }
    )";
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(input, tokens);
    auto doc = parser.parse();

    REQUIRE(doc != nullptr);
    auto* elementNode = dynamic_cast<CHTL::ElementNode*>(doc->children[0].get());
    REQUIRE(elementNode != nullptr);
    REQUIRE(elementNode->getIfBlocks().size() == 1);

    const auto* ifNode = elementNode->getIfBlocks()[0].get();
    REQUIRE(ifNode != nullptr);
    REQUIRE(ifNode->else_branch != nullptr);

    const auto* elseIfNode = dynamic_cast<const CHTL::ElseIfNode*>(ifNode->else_branch.get());
    REQUIRE(elseIfNode != nullptr);
    REQUIRE(elseIfNode->body.size() == 1);
    REQUIRE(elseIfNode->next_else_branch != nullptr);

    const auto* elseNode = dynamic_cast<const CHTL::ElseNode*>(elseIfNode->next_else_branch.get());
    REQUIRE(elseNode != nullptr);
    REQUIRE(elseNode->body.size() == 1);
}