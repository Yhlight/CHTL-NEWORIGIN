#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"

// Helper to get the generated string from a CHTL snippet
std::string generate_from_string(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();
    CHTL::CHTLGenerator generator;
    return generator.generate(root.get());
}

TEST_CASE("Generator handles conditional style properties", "[generator]") {
    SECTION("Condition is true") {
        std::string input = "div { style { width: 100px; background-color: width > 50px ? 'red' : 'blue'; } }";
        std::string result = generate_from_string(input);
        REQUIRE(result == "<div style=\"width: 100px;background-color: red;\"></div>");
    }
    SECTION("Condition is false") {
        std::string input = "div { style { width: 40px; background-color: width > 50px ? 'red' : 'blue'; } }";
        std::string result = generate_from_string(input);
        REQUIRE(result == "<div style=\"width: 40px;background-color: blue;\"></div>");
    }
}

TEST_CASE("Generator handles complex conditional expressions", "[generator]") {
    std::string input = "div { style { width: 60px; height: 120px; border: width > 50px && height > 100px ? '1px solid green' : 'none'; } }";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<div style=\"width: 60px;height: 120px;border: 1px solid green;\"></div>");
}

TEST_CASE("Generator handles property references", "[generator]") {
    std::string input = R"(
        body {
            div {
                class: "box";
                style { width: 100px; }
            }
            span {
                style {
                    width: .box.width + 50px;
                }
            }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<body><div class=\"box\" style=\"width: 100px;\"></div><span style=\"width: 150px;\"></span></body>");
}

TEST_CASE("Generator handles property references in conditionals", "[generator]") {
    std::string input = R"(
        body {
            div {
                class: "box";
                style { width: 100px; }
            }
            span {
                style {
                    color: .box.width > 50px ? 'green' : 'red';
                }
            }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<body><div class=\"box\" style=\"width: 100px;\"></div><span style=\"color: green;\"></span></body>");
}