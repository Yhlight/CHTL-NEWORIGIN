#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TemplateStyleNode.h"
#include "CHTLNode/TemplateElementNode.h"
#include "CHTLNode/TemplateVarNode.h"
#include "CHTLNode/CustomStyleNode.h"
#include "CHTLNode/CustomElementNode.h"
#include "CHTLNode/CustomVarNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/StyleNode.h"
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

    const auto& styleChildren = root->getChildren();
    REQUIRE(styleChildren.size() == 1);
    auto* styleNode = dynamic_cast<StyleNode*>(styleChildren[0].get());
    REQUIRE(styleNode != nullptr);
    const auto& props = styleNode->getProperties();
    REQUIRE(props.size() == 2);
    REQUIRE(props[0].first == "color");
    REQUIRE(std::holds_alternative<std::string>(props[0].second));
    REQUIRE(std::get<std::string>(props[0].second) == "blue");
    REQUIRE(props[1].first == "font-size");
    REQUIRE(std::holds_alternative<std::string>(props[1].second));
    REQUIRE(std::get<std::string>(props[1].second) == "16px");
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

    const auto& styleChildren = root->getChildren();
    REQUIRE(styleChildren.size() == 1);
    auto* styleNode = dynamic_cast<StyleNode*>(styleChildren[0].get());
    REQUIRE(styleNode != nullptr);
    const auto& props = styleNode->getProperties();
    REQUIRE(props.size() == 2);
    REQUIRE(props[0].first == "color");
    REQUIRE(std::holds_alternative<std::string>(props[0].second));
    REQUIRE(std::get<std::string>(props[0].second) == "black");
    REQUIRE(props[1].first == "line-height");
    REQUIRE(std::holds_alternative<std::string>(props[1].second));
    REQUIRE(std::get<std::string>(props[1].second) == "1.6");
}

TEST_CASE("Parser handles custom style declarations with delete", "[parser]") {
    const std::string input = R"(
[Custom] @Style YellowText
{
    color: yellow;
    delete line-height, border;
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 0); // Declarations are non-emitting

    const auto* customNode = context.getCustomStyle("YellowText");
    REQUIRE(customNode != nullptr);
    REQUIRE(customNode->getName() == "YellowText");

    const auto& props = customNode->getProperties();
    REQUIRE(props.size() == 1);
    REQUIRE(props[0].first == "color");
    REQUIRE(props[0].second == "yellow");

    const auto& deletedProps = customNode->getDeletedProperties();
    REQUIRE(deletedProps.size() == 2);
    REQUIRE(deletedProps.count("line-height") == 1);
    REQUIRE(deletedProps.count("border") == 1);
}

TEST_CASE("Parser handles custom element declarations with delete", "[parser]") {
    const std::string input = R"(
[Custom] @Element MyCustomBox
{
    delete span;
    div { }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 0);

    const auto* customNode = context.getCustomElement("MyCustomBox");
    REQUIRE(customNode != nullptr);
    REQUIRE(customNode->getName() == "MyCustomBox");

    const auto& customChildren = customNode->getChildren();
    REQUIRE(customChildren.size() == 1);
    auto* divNode = dynamic_cast<ElementNode*>(customChildren[0].get());
    REQUIRE(divNode != nullptr);
    REQUIRE(divNode->getTagName() == "div");


    const auto& deletedElements = customNode->getDeletedElements();
    REQUIRE(deletedElements.size() == 1);
    REQUIRE(deletedElements.count("span") == 1);
}

TEST_CASE("Parser handles custom var declarations", "[parser]") {
    const std::string input = R"END(
[Custom] @Var CustomTheme
{
    mainColor: "blue";
}
)END";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 0);

    const auto* customNode = context.getCustomVar("CustomTheme");
    REQUIRE(customNode != nullptr);
    REQUIRE(customNode->getName() == "CustomTheme");

    const auto& vars = customNode->getVariables();
    REQUIRE(vars.size() == 1);
    REQUIRE(vars.at("mainColor") == "blue");
}

TEST_CASE("Parser handles import declarations", "[parser]") {
    const std::string input = R"([Import] @Chtl from "my/lib.chtl" as MyLib)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    // Imports are non-emitting at the top level of the document for now
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1);

    auto* importNode = dynamic_cast<ImportNode*>(children[0].get());
    REQUIRE(importNode != nullptr);
    REQUIRE(importNode->getFullType() == "@Chtl");
    REQUIRE(importNode->getFilePath() == "my/lib.chtl");
    REQUIRE(importNode->getAlias() == "MyLib");
}

TEST_CASE("Parser handles namespace declarations", "[parser]") {
    const std::string input = R"(
[Namespace] MyTestSpace {
    [Template] @Style MyStyle { color: purple; }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    std::unique_ptr<DocumentNode> doc = parser.parse();

    REQUIRE(doc != nullptr);
    const auto& children = doc->getChildren();
    REQUIRE(children.size() == 1); // The NamespaceNode

    auto* nsNode = dynamic_cast<NamespaceNode*>(children[0].get());
    REQUIRE(nsNode != nullptr);
    REQUIRE(nsNode->getName() == "MyTestSpace");
}

TEST_CASE("Context handles nested namespaces and scoped lookups", "[parser]") {
    const std::string input = R"(
[Namespace] Outer {
    [Template] @Style OuterStyle { color: red; }

    [Namespace] Inner {
        [Template] @Style InnerStyle { color: blue; }
    }
}
)";
    CHTLContext context;
    CHTLParser parser(input, context);
    parser.parse(); // Parse the input to populate the context

    // After parsing, we should be back in the global scope
    REQUIRE(context.getStyleTemplate("OuterStyle") == nullptr);
    REQUIRE(context.getStyleTemplate("InnerStyle") == nullptr);

    // Enter the outer scope and check again
    context.enterNamespace("Outer");
    const auto* outerStyle = context.getStyleTemplate("OuterStyle");
    REQUIRE(outerStyle != nullptr);
    REQUIRE(outerStyle->getName() == "OuterStyle");
    REQUIRE(context.getStyleTemplate("InnerStyle") == nullptr); // Inner is not visible here directly

    // Enter the inner scope
    context.enterNamespace("Inner");
    const auto* innerStyle = context.getStyleTemplate("InnerStyle");
    REQUIRE(innerStyle != nullptr);
    REQUIRE(innerStyle->getName() == "InnerStyle");

    // Check that we can still resolve names from the outer scope
    const auto* outerFromInner = context.getStyleTemplate("OuterStyle");
    REQUIRE(outerFromInner != nullptr);
    REQUIRE(outerFromInner->getName() == "OuterStyle");

    // Leave the inner scope
    context.leaveNamespace();
    REQUIRE(context.getStyleTemplate("InnerStyle") == nullptr); // No longer visible
    REQUIRE(context.getStyleTemplate("OuterStyle") != nullptr); // Outer still visible

    // Leave the outer scope
    context.leaveNamespace();
    REQUIRE(context.getStyleTemplate("OuterStyle") == nullptr); // No longer visible
}

TEST_CASE("Parser handles style with url() value", "[parser]") {
    const std::string input = R"(
div {
    style {
        background: url("image.png");
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

    const auto& styleChildren = root->getChildren();
    REQUIRE(styleChildren.size() == 1);
    auto* styleNode = dynamic_cast<StyleNode*>(styleChildren[0].get());
    REQUIRE(styleNode != nullptr);

    const auto& props = styleNode->getProperties();
    REQUIRE(props.size() == 1);
    REQUIRE(props[0].first == "background");
    REQUIRE(std::holds_alternative<std::string>(props[0].second));
    REQUIRE(std::get<std::string>(props[0].second) == "url(\"image.png\")");
}