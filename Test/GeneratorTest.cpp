#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"

TEST_CASE("Generator: Simple Element", "[generator]") {
    std::string input = "div { text { \"hello\" } }";
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(*ast);
    REQUIRE(result == "<div>hello</div>");
}

TEST_CASE("Generator: Inline Style", "[generator]") {
    std::string input = "div { style { color: red; } }";
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(*ast);
    REQUIRE(result == "<div style=\"color:red;\"></div>");
}

TEST_CASE("Generator: Automated Class Rule", "[generator]") {
    std::string input = "div { style { .box { width: 100px; } } }";
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(*ast);
    std::string expected_css = "<style>\n.box {\n  width: 100px;\n}\n</style>\n";
    std::string expected_html = "<div class=\"box\"></div>";
    REQUIRE(result == expected_css + expected_html);
}

TEST_CASE("Generator: Automated ID Rule", "[generator]") {
    std::string input = "div { style { #main { height: 200px; } } }";
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(*ast);
    std::string expected_css = "<style>\n#main {\n  height: 200px;\n}\n</style>\n";
    std::string expected_html = "<div id=\"main\"></div>";
    REQUIRE(result == expected_css + expected_html);
}

TEST_CASE("Generator: Mixed Inline and Rule Styles", "[generator]") {
    std::string input = "p { style { color: blue; .important { font-weight: bold; } } }";
    CHTL::CHTLLexer lexer;
    auto tokens = lexer.tokenize(input);
    CHTL::CHTLParser parser(tokens);
    auto ast = parser.parse();
    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(*ast);
    std::string expected_css = "<style>\n.important {\n  font-weight: bold;\n}\n</style>\n";
    std::string expected_html = "<p class=\"important\" style=\"color:blue;\"></p>";
    REQUIRE(result == expected_css + expected_html);
}