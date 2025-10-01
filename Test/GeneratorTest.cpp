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

TEST_CASE("Generator produces global styles from local style blocks", "[generator]") {
    std::string input = R"(
        div {
            style {
                .my-class {
                    color: red;
                    font-size: 16px;
                }
            }
        }
    )";
    std::string result = generate_from_string(input);
    std::string expected_body = R"(<div class="my-class"></div>)";
    std::string expected_style = R"(.my-class{color:red;font-size:16px;})";
    std::string expected_html = "<html><head><style>" + expected_style + "</style></head><body>" + expected_body + "</body></html>";

    // For debugging, let's check parts
    // INFO("Generated: " << result);
    // INFO("Expected:  " << expected_html);

    REQUIRE(result.find(expected_body) != std::string::npos);
    REQUIRE(result.find(expected_style) != std::string::npos);
}

TEST_CASE("Generator correctly expands ampersand selector", "[generator]") {
    std::string input = R"(
        div {
            style {
                .my-class {
                    color: blue;
                }
                &:hover {
                    color: red;
                }
            }
        }
    )";
    std::string result = generate_from_string(input);
    std::string expected_body = R"(<div class="my-class"></div>)";
    std::string expected_style1 = ".my-class{color:blue;}";
    std::string expected_style2 = ".my-class:hover{color:red;}";

    REQUIRE(result.find(expected_body) != std::string::npos);
    REQUIRE(result.find(expected_style1) != std::string::npos);
    REQUIRE(result.find(expected_style2) != std::string::npos);
}