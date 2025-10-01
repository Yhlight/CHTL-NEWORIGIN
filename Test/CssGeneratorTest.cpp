#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"
#include "TestUtil.h"
#include <memory>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>

// Helper to get the generated string from a CHTL snippet
std::string generate_css_from_string(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    parser.parse();
    auto ast_nodes = parser.getAST();
    CHTL::CHTLGenerator generator;
    return generator.generate(ast_nodes);
}


TEST_CASE("Generator produces correct inline styles", "[css_generator]") {
    std::string input = R"(
        div {
            style {
                color: red;
                font-size: 16px;
            }
        }
    )";
    std::string result = generate_css_from_string(input);
    std::string expected = "<html><head><style></style></head><body><div style=\"color: red;font-size: 16px;\"></div></body></html>";
    REQUIRE(remove_whitespace(result) == remove_whitespace(expected));
}

TEST_CASE("Generator produces correct global styles from class selectors", "[css_generator]") {
    std::string input = R"(
        div {
            style {
                .my-class {
                    background-color: blue;
                }
            }
        }
    )";
    std::string result = generate_css_from_string(input);
    std::string expected = "<html><head><style>.my-class{background-color:blue;}</style></head><body><div class=\"my-class\"></div></body></html>";
    REQUIRE(remove_whitespace(result) == remove_whitespace(expected));
}

TEST_CASE("Generator handles arithmetic expressions in styles", "[css_generator]") {
    std::string input = R"(
        div {
            style {
                width: 100 + 50px;
                height: 2 * 25em;
            }
        }
    )";
    std::string result = generate_css_from_string(input);
    std::string expected = "<html><head><style></style></head><body><div style=\"width: 150px;height: 50em;\"></div></body></html>";
    REQUIRE(remove_whitespace(result) == remove_whitespace(expected));
}

TEST_CASE("Generator handles property references in styles", "[css_generator]") {
    std::string input = R"(
        div {
            id: my-div;
            style {
                width: 100px;
            }
        }
        span {
            style {
                width: #my-div.width;
            }
        }
    )";
    std::string result = generate_css_from_string(input);
    std::string expected_html = "<html><head><style></style></head><body><div id=\"my-div\" style=\"width: 100px;\"></div><span style=\"width: 100px;\"></span></body></html>";
    REQUIRE(remove_whitespace(result) == remove_whitespace(expected_html));
}