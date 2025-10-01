#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StylePropertyNode.h"

// Helper to get the generated string from a CHTL snippet
std::string generate_from_string(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::BaseNode> root = parser.parse();
    CHTL::CHTLGenerator generator;
    return generator.generate(root.get());
}

TEST_CASE("Generator handles style property with arithmetic", "[generator]") {
    std::string input = "div { style { width: 10px * 2; height: 100% / 2; } }";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<div style=\"width: 20px;height: 50%;\"></div>");
}

TEST_CASE("Generator handles simple element with text", "[generator]") {
    std::string input = "p { text { \"Hello World\" } }";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<p>Hello World</p>");
}

TEST_CASE("Generator handles attributes and styles together", "[generator]") {
    std::string input = "a { href: \"#\"; style { color: blue; font-size: 10pt + 2pt; } }";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<a href=\"#\" style=\"color: blue;font-size: 12pt;\"></a>");
}

TEST_CASE("Generator handles nested elements and mixed content", "[generator]") {
    std::string input = "main { id: \"app\"; section { class: \"container\"; h1 { text { \"Title\" } } p { text { \"Content\" } } } }";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<main id=\"app\"><section class=\"container\"><h1>Title</h1><p>Content</p></section></main>");
}

TEST_CASE("Generator creates global stylesheet from selector blocks", "[generator]") {
    std::string input = "div { style { .my-class { color: blue; } width: 100px; } }";
    std::string result = generate_from_string(input);
    std::string expected_css = ".my-class {color:blue;}";
    std::string expected_body = "<div class=\"my-class\" style=\"width: 100px;\"></div>";
    std::string expected_html = "<html><head><style>" + expected_css + "</style></head><body>" + expected_body + "</body></html>";
    REQUIRE(result == expected_html);
}