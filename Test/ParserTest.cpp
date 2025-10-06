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
#include "../CHTL/CHTLNode/ImportNode.h"
#include "../CHTL/CHTLNode/NamespaceNode.h"
#include "../CHTL/CHTLNode/ConfigurationNode.h"
#include "../CHTL/CHTLNode/TemplateUsageNode.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"
#include <memory>

#include <vector>
#include <string>

// Helper to get the first and only node from the parser's root
std::shared_ptr<CHTL::BaseNode> getFirstNode(const std::string& input) {
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::CHTLLexer lexer(input, configManager);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens, configManager);
    auto root = parser.parse();
    REQUIRE(root != nullptr);
    REQUIRE(root->getChildren().size() == 1);
    return root->getChildren()[0];
}

TEST_CASE("Parser handles simple element", "[parser]") {
    auto node = getFirstNode("div {}");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(node);
    REQUIRE(element->getTagName() == "div");
}

TEST_CASE("Parser handles attributes", "[parser]") {
    auto node = getFirstNode(R"(div { id: "main"; class: "container"; })");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(node);
    REQUIRE(element->getTagName() == "div");
    auto attributes = element->getAttributes();
    REQUIRE(attributes.size() == 2);
    REQUIRE(attributes.at("id") == "main");
    REQUIRE(attributes.at("class") == "container");
}

TEST_CASE("Parser handles nested elements", "[parser]") {
    auto node = getFirstNode("body { div { span {} } }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto body = std::dynamic_pointer_cast<CHTL::ElementNode>(node);
    REQUIRE(body->getTagName() == "body");
    REQUIRE(body->getChildren().size() == 1);
    auto div = std::dynamic_pointer_cast<CHTL::ElementNode>(body->getChildren()[0]);
    REQUIRE(div->getTagName() == "div");
    REQUIRE(div->getChildren().size() == 1);
    auto span = std::dynamic_pointer_cast<CHTL::ElementNode>(div->getChildren()[0]);
    REQUIRE(span->getTagName() == "span");
}

TEST_CASE("Parser handles text nodes", "[parser]") {
    auto node = getFirstNode(R"(p { text { "Hello, World!" } })");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_ELEMENT);
    auto p = std::dynamic_pointer_cast<CHTL::ElementNode>(node);
    REQUIRE(p->getTagName() == "p");
    REQUIRE(p->getChildren().size() == 1);
    auto textNode = std::dynamic_pointer_cast<CHTL::TextNode>(p->getChildren()[0]->getChildren()[0]);
    REQUIRE(textNode != nullptr);
    REQUIRE(textNode->getContent() == "Hello, World!");
}

TEST_CASE("Parser handles style blocks", "[parser]") {
    auto node = getFirstNode("div { style { color: red; width: 100px; } }");
    REQUIRE(node != nullptr);
    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(node);
    REQUIRE(element->getTagName() == "div");
    REQUIRE(element->getChildren().size() == 1);

    auto styleNode = std::dynamic_pointer_cast<CHTL::StyleNode>(element->getChildren()[0]);
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getChildren().size() == 2);
    auto prop1 = std::dynamic_pointer_cast<CHTL::PropertyNode>(styleNode->getChildren()[0]);
    REQUIRE(prop1->getKey() == "color");
    REQUIRE(prop1->getValue() == "red");
    auto prop2 = std::dynamic_pointer_cast<CHTL::PropertyNode>(styleNode->getChildren()[1]);
    REQUIRE(prop2->getKey() == "width");
    REQUIRE(prop2->getValue() == "100px");
}

TEST_CASE("Parser handles style template", "[parser]") {
    auto node = getFirstNode("[Template] @Style DefaultText { color: black; font-size: 16px; }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_TEMPLATE);
    auto templateNode = std::dynamic_pointer_cast<CHTL::TemplateNode>(node);
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
    auto node = getFirstNode("[Template] @Element MyElement { div { text { \"I am a template\" } } }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_TEMPLATE);
    auto templateNode = std::dynamic_pointer_cast<CHTL::TemplateNode>(node);
    REQUIRE(templateNode != nullptr);
    REQUIRE(templateNode->getName() == "MyElement");
    REQUIRE(templateNode->getTemplateType() == CHTL::TemplateType::ELEMENT);
    REQUIRE(templateNode->getChildren().size() == 1);
    auto div = std::dynamic_pointer_cast<CHTL::ElementNode>(templateNode->getChildren()[0]);
    REQUIRE(div != nullptr);
    REQUIRE(div->getTagName() == "div");
}

TEST_CASE("Parser rejects inheritance in anonymous style blocks", "[parser][error]") {
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    std::string input = "div { style: SomeBaseStyle { color: red; } }";
    CHTL::CHTLLexer lexer(input, configManager);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    CHTL::CHTLParser parser(tokens, configManager);
    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

TEST_CASE("Parser handles variable template usage", "[parser]") {
    auto node = getFirstNode("div { style { color: MyTheme(primary); } }");
    REQUIRE(node != nullptr);
    auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(node);
    REQUIRE(element->getTagName() == "div");
    REQUIRE(element->getChildren().size() == 1);
    auto styleNode = std::dynamic_pointer_cast<CHTL::StyleNode>(element->getChildren()[0]);
    REQUIRE(styleNode != nullptr);
    REQUIRE(styleNode->getChildren().size() == 1);
    auto propNode = std::dynamic_pointer_cast<CHTL::PropertyNode>(styleNode->getChildren()[0]);
    REQUIRE(propNode != nullptr);
    REQUIRE(propNode->getKey() == "color");
    REQUIRE(propNode->getChildren().size() == 1);
    auto usageNode = std::dynamic_pointer_cast<CHTL::TemplateUsageNode>(propNode->getChildren()[0]);
    REQUIRE(usageNode != nullptr);
    REQUIRE(usageNode->getName() == "MyTheme");
    REQUIRE(usageNode->getUsageType() == CHTL::TemplateUsageType::VAR);
    REQUIRE(usageNode->getVariableName() == "primary");
}

TEST_CASE("Parser handles template usage", "[parser]") {
    auto node = getFirstNode("div { @Element MyTemplate; }");
    auto div = std::dynamic_pointer_cast<CHTL::ElementNode>(node);
    REQUIRE(div != nullptr);
    REQUIRE(div->getChildren().size() == 1);
    auto usageNode = std::dynamic_pointer_cast<CHTL::TemplateUsageNode>(div->getChildren()[0]);
    REQUIRE(usageNode != nullptr);
    REQUIRE(usageNode->getName() == "MyTemplate");
    REQUIRE(usageNode->getUsageType() == CHTL::TemplateUsageType::ELEMENT);
}

TEST_CASE("Parser handles configuration block", "[parser]") {
    auto node = getFirstNode("[Configuration] { INDEX_INITIAL_COUNT = 0; DEBUG_MODE = false; }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_CONFIG);
    auto configNode = std::dynamic_pointer_cast<CHTL::ConfigurationNode>(node);
    REQUIRE(configNode != nullptr);
    auto settings = configNode->getSettings();
    REQUIRE(settings.size() == 2);
    REQUIRE(settings[0]->getKey() == "INDEX_INITIAL_COUNT");
    REQUIRE(settings[0]->getValue() == "0");
    REQUIRE(settings[1]->getKey() == "DEBUG_MODE");
    REQUIRE(settings[1]->getValue() == "false");
}

TEST_CASE("Parser handles custom style block", "[parser]") {
    auto node = getFirstNode("[Custom] @Style MyCustomStyle { color: red; }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_CUSTOM);
    auto customNode = std::dynamic_pointer_cast<CHTL::CustomNode>(node);
    REQUIRE(customNode != nullptr);
    REQUIRE(customNode->getName() == "MyCustomStyle");
    REQUIRE(customNode->getCustomType() == CHTL::CustomType::STYLE);
    REQUIRE(customNode->getChildren().size() == 1);
    auto prop1 = std::dynamic_pointer_cast<CHTL::PropertyNode>(customNode->getChildren()[0]);
    REQUIRE(prop1->getKey() == "color");
    REQUIRE(prop1->getValue() == "red");
}

TEST_CASE("Parser handles origin block", "[parser]") {
    auto node = getFirstNode("[Origin] @Html { <div>Hello</div> }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_ORIGIN);
    auto originNode = std::dynamic_pointer_cast<CHTL::OriginNode>(node);
    REQUIRE(originNode != nullptr);
    REQUIRE(originNode->getOriginType() == "@Html");
    REQUIRE(originNode->getName() == "");
    REQUIRE(originNode->getContent() == "<div>Hello</div>");
}

TEST_CASE("Parser handles named origin block", "[parser]") {
    auto node = getFirstNode("[Origin] @JavaScript myScript { console.log(\"Hello\"); }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_ORIGIN);
    auto originNode = std::dynamic_pointer_cast<CHTL::OriginNode>(node);
    REQUIRE(originNode != nullptr);
    REQUIRE(originNode->getOriginType() == "@JavaScript");
    REQUIRE(originNode->getName() == "myScript");
    REQUIRE(originNode->getContent() == "console.log(\"Hello\");");
}

TEST_CASE("Parser handles import statement", "[parser]") {
    auto node = getFirstNode("[Import] @Chtl from \"./components.chtl\" as Components;");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_IMPORT);
    auto importNode = std::dynamic_pointer_cast<CHTL::ImportNode>(node);
    REQUIRE(importNode != nullptr);
    REQUIRE(importNode->getImportType() == CHTL::ImportType::CHTL);
    REQUIRE(importNode->getPath() == "./components.chtl");
    REQUIRE(importNode->getAlias() == "Components");
}

TEST_CASE("Parser handles namespace block", "[parser]") {
    auto node = getFirstNode("[Namespace] MyNamespace { div {} }");
    REQUIRE(node != nullptr);
    REQUIRE(node->getType() == CHTL::NodeType::NODE_NAMESPACE);
    auto namespaceNode = std::dynamic_pointer_cast<CHTL::NamespaceNode>(node);
    REQUIRE(namespaceNode != nullptr);
    REQUIRE(namespaceNode->getName() == "MyNamespace");
    REQUIRE(namespaceNode->getChildren().size() == 1);
    auto div = std::dynamic_pointer_cast<CHTL::ElementNode>(namespaceNode->getChildren()[0]);
    REQUIRE(div != nullptr);
    REQUIRE(div->getTagName() == "div");
}