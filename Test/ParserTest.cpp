#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TemplateStyleNode.h"
#include "CHTLNode/TemplateElementNode.h"
#include "CHTLNode/TemplateVarNode.h"
#include "CHTLContext/CHTLContext.h"
#include <memory>

TEST_CASE("Parser handles a single element", "[parser]") {
    const std::string input = "div { }";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);

    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);
    REQUIRE(root->toString().find("<div>") != std::string::npos);
}

TEST_CASE("Parser handles text nodes", "[parser]") {
    const std::string input = R"(
div {
    text { "Hello, CHTL!" }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("<div>") != std::string::npos);
    REQUIRE(output.find("Hello, CHTL!") != std::string::npos);
    REQUIRE(output.find("</div>") != std::string::npos);
}

TEST_CASE("Parser handles attributes", "[parser]") {
    const std::string input = R"(
div {
    id: "my-div";
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("<div id=\"my-div\">") != std::string::npos);
}

TEST_CASE("Parser handles attributes with equals sign", "[parser]") {
    const std::string input = R"(
div {
    id = "my-div-equals";
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("<div id=\"my-div-equals\">") != std::string::npos);
}

TEST_CASE("Parser handles unquoted literal attributes", "[parser]") {
    const std::string input = R"(
p {
    color: red;
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("<p color=\"red\">") != std::string::npos);
}

TEST_CASE("Parser handles text attribute", "[parser]") {
    const std::string input = R"(
div {
    text: "Hello from attribute!";
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("text=\"Hello from attribute!\"") == std::string::npos);
    REQUIRE(output.find("Hello from attribute!") != std::string::npos);
}

TEST_CASE("Parser handles style blocks", "[parser]") {
    const std::string input = R"(
div {
    style {
        color: blue;
        font-size: 16px;
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("/* Style Block: color: blue; font-size: 16px; */") != std::string::npos);
}

TEST_CASE("Parser handles comment nodes", "[parser]") {
    const std::string input = R"(
div {
    # This is a comment
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("<div>") != std::string::npos);
    REQUIRE(output.find("<!-- This is a comment -->") != std::string::npos);
    REQUIRE(output.find("</div>") != std::string::npos);
}

TEST_CASE("Parser handles nested elements", "[parser]") {
    const std::string input = R"(
div {
    span { }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);
    ElementNode* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("<div>") != std::string::npos);
    REQUIRE(output.find("<span>") != std::string::npos);
    REQUIRE(output.find("</span>") != std::string::npos);
    REQUIRE(output.find("</div>") != std::string::npos);
}

TEST_CASE("Parser handles style template declarations", "[parser]") {
    const std::string input = R"(
[Template] @Style DefaultText
{
    color: black;
    font-size: 16px;
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 0);

    const auto* tmplNode = context.getStyleTemplate("DefaultText");
    REQUIRE(tmplNode != nullptr);
    REQUIRE(tmplNode->getName() == "DefaultText");

    const auto& props = tmplNode->getProperties();
    REQUIRE(props.size() == 2);
    REQUIRE(props[0].first == "color");
    REQUIRE(props[0].second == "black");
    REQUIRE(props[1].first == "font-size");
    REQUIRE(props[1].second == "16px");
}

TEST_CASE("Parser handles element template declarations", "[parser]") {
    const std::string input = R"(
[Template] @Element Box
{
    div { }
    span { text { "I am in a box" } }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 0);

    const auto* tmplNode = context.getElementTemplate("Box");
    REQUIRE(tmplNode != nullptr);
    REQUIRE(tmplNode->getName() == "Box");

    const auto& tmplChildren = tmplNode->getChildren();
    REQUIRE(tmplChildren.size() == 2);

    auto* divNode = dynamic_cast<ElementNode*>(tmplChildren[0].get());
    REQUIRE(divNode != nullptr);

    auto* spanNode = dynamic_cast<ElementNode*>(tmplChildren[1].get());
    REQUIRE(spanNode != nullptr);
}

TEST_CASE("Parser handles var template declarations", "[parser]") {
    const std::string input = R"END(
[Template] @Var ThemeColor
{
    tableColor: "rgb(255, 192, 203)";
    textColor: "black";
}
)END";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 0);

    const auto* tmplNode = context.getVarTemplate("ThemeColor");
    REQUIRE(tmplNode != nullptr);
    REQUIRE(tmplNode->getName() == "ThemeColor");

    const auto& vars = tmplNode->getVariables();
    REQUIRE(vars.size() == 2);
    REQUIRE(vars.at("tableColor") == "rgb(255, 192, 203)");
    REQUIRE(vars.at("textColor") == "black");
}

TEST_CASE("Parser applies style templates", "[parser]") {
    const std::string input = R"END(
[Template] @Style DefaultText
{
    color: black;
    line-height: 1.6;
}

div {
    style {
        @Style DefaultText;
    }
}
)END";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);

    auto* root = dynamic_cast<ElementNode*>(children[0].get());
    REQUIRE(root != nullptr);

    std::string output = root->toString();
    REQUIRE(output.find("/* Style Block: color: black; line-height: 1.6; */") != std::string::npos);
}