#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"
#include <memory>
#include <vector>

// Helper to get the generated string from a CHTL snippet
std::string generate_from_string(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    parser.parse();
    auto ast_nodes = parser.getAST();
    CHTL::CHTLGenerator generator;
    return generator.generate(ast_nodes);
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
    REQUIRE(result == "<html><head><style></style></head><body><p style=\"color: black;font-size: 16px;font-weight: bold;\"></p></body></html>");
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
    REQUIRE(result == "<html><head><style></style></head><body><div><h1>Title</h1><p>Content</p></div></body></html>");
}

TEST_CASE("Generator evaluates var templates", "[generator]") {
    std::string input = R"(
        [Template] @Var MyTheme {
            primaryColor: #007bff;
        }
        div {
            style {
                background-color: MyTheme(primaryColor);
            }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<html><head><style></style></head><body><div style=\"background-color: #007bff;\"></div></body></html>");
}