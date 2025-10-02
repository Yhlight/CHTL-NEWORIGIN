#include "catch.hpp"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include <memory>

TEST_CASE("Parser handles a single element", "[parser]") {
    const std::string input = "div { }";
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

    REQUIRE(root != nullptr);
    // In a real scenario, we'd have a better way to check the tag name
    // but for now, we'll rely on the toString() method for verification.
    REQUIRE(root->toString().find("<div>") != std::string::npos);
}

TEST_CASE("Parser handles text nodes", "[parser]") {
    const std::string input = R"(
div {
    text { "Hello, CHTL!" }
}
)";
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

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
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

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
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

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
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

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
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

    REQUIRE(root != nullptr);

    std::string output = root->toString();
    // Ensure 'text' is not treated as a regular attribute.
    REQUIRE(output.find("text=\"Hello from attribute!\"") == std::string::npos);
    // Ensure the content is added as a text node.
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
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

    REQUIRE(root != nullptr);

    // For now, we'll check if the StyleNode's content is captured.
    // A more robust test would inspect the AST directly.
    std::string output = root->toString();
    REQUIRE(output.find("/* Style Block: color: blue; font-size: 16px; */") != std::string::npos);
}

TEST_CASE("Parser handles comment nodes", "[parser]") {
    const std::string input = R"(
div {
    # This is a comment
}
)";
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

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
    CHTLParser parser(input);
    std::unique_ptr<ElementNode> root = parser.parse();

    REQUIRE(root != nullptr);

    // Using toString() for verification. This is not ideal but works for now.
    // A better approach would be to inspect the AST directly.
    std::string output = root->toString();
    REQUIRE(output.find("<div>") != std::string::npos);
    REQUIRE(output.find("<span>") != std::string::npos);
    REQUIRE(output.find("</span>") != std::string::npos);
    REQUIRE(output.find("</div>") != std::string::npos);
}