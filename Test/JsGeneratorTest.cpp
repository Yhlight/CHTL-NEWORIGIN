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
std::string generate_js_from_string(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    parser.parse();
    auto ast_nodes = parser.getAST();
    CHTL::CHTLGenerator generator;
    return generator.generate(ast_nodes);
}

TEST_CASE("Generator produces correct JS for simple enhanced selector", "[js_generator]") {
    std::string input = R"(
        div {
            script {
                const myDiv = {{.my-div}};
            }
        }
    )";
    std::string result = generate_js_from_string(input);
    std::string expected_html = "<html><head><style></style></head><body><div><script>const myDiv = document.querySelector('.my-div');</script></div></body></html>";
    REQUIRE(remove_whitespace(result) == remove_whitespace(expected_html));
}

TEST_CASE("Generator produces correct JS for indexed enhanced selector", "[js_generator]") {
    std::string input = R"(
        div {
            script {
                const firstBtn = {{button[0]}};
            }
        }
    )";
    std::string result = generate_js_from_string(input);
    std::string expected_html = "<html><head><style></style></head><body><div><script>const firstBtn = document.querySelectorAll('button')[0];</script></div></body></html>";
    REQUIRE(remove_whitespace(result) == remove_whitespace(expected_html));
}

TEST_CASE("Generator handles mixed raw script and enhanced selectors", "[js_generator]") {
    std::string input = R"(
        div {
            script {
                console.log("Setting up click listener...");
                {{#my-btn}}.addEventListener('click', () => {
                    console.log("Button clicked!");
                });
            }
        }
    )";
    std::string result = generate_js_from_string(input);
    std::string expected_html = "<html><head><style></style></head><body><div><script>console.log(\"Setting up click listener...\");document.querySelector('#my-btn').addEventListener('click', () => {console.log(\"Button clicked!\");});</script></div></body></html>";
    REQUIRE(remove_whitespace(result) == remove_whitespace(expected_html));
}