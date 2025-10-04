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

TEST_CASE("Generator handles global and inline styles", "[generator]") {
    std::string input = R"(
        div {
            style {
                color: blue;
                .my-class {
                    background-color: green;
                }
            }
        }
    )";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);

    std::string html = generator.getHtml();
    REQUIRE(html.find(R"(class="my-class")") != std::string::npos);
    REQUIRE(html.find(R"(style="color: blue;")") != std::string::npos);

    std::string css = generator.getCss();
    REQUIRE(css == ".my-class { background-color: green; }");
}

TEST_CASE("Generator handles @Html origin block", "[generator]") {
    std::string input = R"([Origin] @Html { <div>Raw</div> })";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    REQUIRE(generator.getHtml() == "<div>Raw</div>");
    REQUIRE(generator.getCss().empty());
}

TEST_CASE("Generator handles @Style origin block", "[generator]") {
    std::string input = R"([Origin] @Style { .raw { color: red; } })";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    REQUIRE(generator.getHtml().empty());
    REQUIRE(generator.getCss() == ". raw { color : red ; }");
}

TEST_CASE("Generator handles @JavaScript origin block", "[generator]") {
    std::string input = R"([Origin] @JavaScript { alert("raw"); })";
    auto ast = parseInput(input);
    CHTL::CHTLGenerator generator;
    generator.generate(ast);
    REQUIRE(generator.getHtml() == "alert ( raw ) ;");
    REQUIRE(generator.getCss().empty());
}