#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"

#include <vector>
#include <string>

std::shared_ptr<CHTL::BaseNode> parseInput(const std::string& input) {
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens);
    return parser.parse();
}

TEST_CASE("Generator handles simple element", "[generator]") {
    std::string input = "div {}";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    REQUIRE(generator.getHtml() == "<div></div>");
}

TEST_CASE("Generator handles element with text", "[generator]") {
    std::string input = R"(p { text { "Hello World" } })";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    REQUIRE(generator.getHtml() == "<p>Hello World</p>");
}

TEST_CASE("Generator handles element with attributes", "[generator]") {
    std::string input = R"(a { href: "#"; class: "link"; })";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    std::string html = generator.getHtml();
    REQUIRE(html.find(R"(href="#")") != std::string::npos);
    REQUIRE(html.find(R"(class="link")") != std::string::npos);
}

TEST_CASE("Generator handles nested elements", "[generator]") {
    std::string input = R"(
        body {
            div {
                span { text { "Nested" } }
            }
        }
    )";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    REQUIRE(generator.getHtml() == "<body><div><span>Nested</span></div></body>");
}

TEST_CASE("Generator handles inline styles", "[generator]") {
    std::string input = R"(
        div {
            style {
                color: red;
                font-size: 16px;
            }
        }
    )";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    REQUIRE(generator.getHtml() == R"(<div style="color: red;font-size: 16px;"></div>)");
}