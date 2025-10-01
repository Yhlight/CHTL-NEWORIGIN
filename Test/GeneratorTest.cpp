#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"
#include <memory>

// Helper to get the generated string from a CHTL snippet
std::string generate_from_string(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();
    CHTL::CHTLGenerator generator;
    return generator.generate(root.get());
}

TEST_CASE("Generator expands style templates", "[generator]") {
    std::string input = R"(
        [Template] @Style DefaultText {
            color: black;
            font-size: 16px;
        }
        p {
            style {
                @Style DefaultText;
                font-weight: bold;
            }
        }
    )";
    std::string result = generate_from_string(input);
    // Note: This test will fail until the generator is updated to handle template expansion.
    // The expected output assumes that the generator will eventually expand the style template
    // into the p element's inline style.
    REQUIRE(result == "<p style=\"color: black;font-size: 16px;font-weight: bold;\"></p>");
}

TEST_CASE("Generator expands element templates", "[generator]") {
    std::string input = R"(
        [Template] @Element MyElement {
            h1 { text { "Title" } }
            p { text { "Content" } }
        }
        div {
            @Element MyElement;
        }
    )";
    std::string result = generate_from_string(input);
    // Note: This test will also fail until the generator is updated.
    REQUIRE(result == "<div><h1>Title</h1><p>Content</p></div>");
}

TEST_CASE("Generator evaluates conditional expressions correctly", "[generator]") {
    // True case
    std::string input_true = R"(
        div {
            style {
                width: 100px;
                background-color: width > 50px ? "red" : "blue";
            }
        }
    )";
    std::string result_true = generate_from_string(input_true);
    std::string expected_style_true = R"(style="width: 100px;background-color: red;")";
    REQUIRE(result_true.find(expected_style_true) != std::string::npos);

    // False case
    std::string input_false = R"(
        div {
            style {
                width: 40px;
                background-color: width > 50px ? "red" : "blue";
            }
        }
    )";
    std::string result_false = generate_from_string(input_false);
    std::string expected_style_false = R"(style="width: 40px;background-color: blue;")";
    REQUIRE(result_false.find(expected_style_false) != std::string::npos);
}