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