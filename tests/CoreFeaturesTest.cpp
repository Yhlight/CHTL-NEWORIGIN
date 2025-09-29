#include "catch.hpp"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/Bridge/SharedContext.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <regex>

// Helper function to normalize HTML output for consistent testing.
// This removes indentation and extra newlines.
static std::string normalize_html_for_core_test(const std::string& html) {
    std::string s = html;
    // Replace newlines with a single space for a flat comparison, then trim.
    s = std::regex_replace(s, std::regex(R"(\n)"), "");
    // Remove space between tags
    s = std::regex_replace(s, std::regex(R"(>\s*<)"), "><");
    // Remove leading/trailing whitespace
    s = std::regex_replace(s, std::regex(R"(^\s+|\s+$)"), "");
    return s;
}


// Helper function to streamline the process of converting CHTL code to HTML for testing.
static std::string chtl_to_html_for_core_test(const std::string& chtl_code) {
    Lexer lexer(chtl_code);
    Parser parser(lexer);
    auto ast = parser.parse();

    Generator generator;
    SharedContext context;
    // Generate with default parameters (no doctype, no external files)
    return generator.generate(ast, "", "", context, false, false, "", "");
}

TEST_CASE("Core CHTL Feature Parsing and Generation", "[core]") {

    SECTION("A single element with a text attribute should be parsed and generated correctly") {
        std::string chtl = R"(
            p {
                text: "Hello, CHTL.";
            }
        )";
        std::string expected_html = "<p>Hello, CHTL.</p>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("Nested elements should be parsed and generated in the correct hierarchy") {
        std::string chtl = R"(
            html {
                body {
                    div {
                        text: "I am in a div.";
                    }
                }
            }
        )";
        std::string expected_html = "<html><body><div>I am in a div.</div></body></html>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("An element with a text block should be parsed and generated correctly") {
        std::string chtl = R"(
            div {
                text {
                    "This is a block of text."
                }
            }
        )";
        std::string expected_html = "<div>This is a block of text.</div>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("Unquoted text in a text block should be concatenated") {
        std::string chtl = R"(
            p {
                text {
                    some unquoted words
                }
            }
        )";
        std::string expected_html = "<p>some unquoted words</p>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("A mix of child elements and text nodes should be ordered correctly") {
        std::string chtl = R"(
            main {
                h1 { text: "Title"; }
                p { text: "Paragraph 1."; }
                p { text: "Paragraph 2."; }
            }
        )";
        std::string expected_html = "<main><h1>Title</h1><p>Paragraph 1.</p><p>Paragraph 2.</p></main>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }

    SECTION("[Origin] block should output its content raw") {
        std::string chtl = R"(
            body {
                [Origin] @Html {
                    <div class="raw">
                        <span>This is raw HTML</span>
                    </div>
                }
            }
        )";
        std::string expected_html = R"(<body><div class="raw"><span>This is raw HTML</span></div></body>)";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == normalize_html_for_core_test(expected_html));
    }

    SECTION("Named [Origin] block should be definable and reusable") {
        std::string chtl = R"(
            [Origin] @Html MyRawBlock {
                <footer>Copyright 2024</footer>
            }

            html {
                body {
                    p { text: "Some content"; }
                    [Origin] @Html MyRawBlock;
                }
            }
        )";
        std::string expected_html = "<html><body><p>Some content</p><footer>Copyright 2024</footer></body></html>";
        std::string actual_html = chtl_to_html_for_core_test(chtl);
        REQUIRE(normalize_html_for_core_test(actual_html) == expected_html);
    }
}