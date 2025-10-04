#include "../third-party/catch.hpp"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLLoader/CHTLLoader.h"
#include "../CHTL/CHTLContext/GenerationContext.h"

#include <vector>
#include <string>
#include <algorithm>

// Helper to perform the full compilation process and return the generator
CHTL::CHTLGenerator generateOutput(const std::string& input) {
    CHTL::CHTLLexer lexer(input);
    std::vector<CHTL::Token> tokens;
    CHTL::Token token = lexer.getNextToken();
    while (token.type != CHTL::TokenType::TOKEN_EOF) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }
    CHTL::CHTLParser parser(tokens);
    auto ast = parser.parse();

    CHTL::CHTLLoader loader(".");
    loader.loadImports(ast);

    CHTL::GenerationContext context;
    loader.gatherTemplates(ast, context);
    for (const auto& pair : loader.getLoadedAsts()) {
        loader.gatherTemplates(pair.second, context);
    }

    CHTL::CHTLGenerator generator;
    generator.generate(ast, context);

    return generator;
}


TEST_CASE("Generator handles simple element", "[generator]") {
    auto generator = generateOutput("div {}");
    REQUIRE(generator.getHtml() == "<div></div>");
}

TEST_CASE("Generator handles element with text", "[generator]") {
    auto generator = generateOutput(R"(p { text { "Hello World" } })");
    REQUIRE(generator.getHtml() == "<p>Hello World</p>");
}

TEST_CASE("Generator handles element with attributes", "[generator]") {
    auto generator = generateOutput(R"(a { href: "#"; class: "link"; })");
    std::string html = generator.getHtml();
    REQUIRE(html.find(R"(href="#")") != std::string::npos);
    REQUIRE(html.find(R"(class="link")") != std::string::npos);
}

TEST_CASE("Generator handles nested elements", "[generator]") {
    auto generator = generateOutput("body { div { span { text { \"Nested\" } } } }");
    REQUIRE(generator.getHtml() == "<body><div><span>Nested</span></div></body>");
}

TEST_CASE("Generator handles inline styles", "[generator]") {
    auto generator = generateOutput("div { style { color: red; font-size: 16px; } }");
    REQUIRE(generator.getHtml() == R"(<div style="color:red;font-size:16px;"></div>)");
}

TEST_CASE("Generator handles global and inline styles", "[generator]") {
    auto generator = generateOutput("div { style { color: blue; .my-class { background-color: green; } } }");
    REQUIRE(generator.getHtml() == R"(<div class="my-class" style="color:blue;"></div>)");
    REQUIRE(generator.getCss() == ".my-class{background-color:green;}");
}

TEST_CASE("Generator handles @Html origin block", "[generator]") {
    auto generator = generateOutput("[Origin] @Html { <div>Raw</div> }");
    REQUIRE(generator.getHtml() == "<div>Raw</div>");
    REQUIRE(generator.getCss().empty());
}

TEST_CASE("Generator handles @Style origin block", "[generator]") {
    auto generator = generateOutput("[Origin] @Style { .raw { color: red; } }");
    REQUIRE(generator.getHtml().empty());
    REQUIRE(generator.getCss() == ".raw{color:red;}");
}

TEST_CASE("Generator handles @JavaScript origin block", "[generator]") {
    auto generator = generateOutput("[Origin] @JavaScript { alert(\"raw\"); }");
    REQUIRE(generator.getHtml() == "alert(\"raw\");");
    REQUIRE(generator.getCss().empty());
}

TEST_CASE("Generator handles 'use html5' directive", "[generator]") {
    auto generator = generateOutput("use html5;");
    REQUIRE(generator.getHtml() == "<!DOCTYPE html>");
}

TEST_CASE("Generator handles 'use html5' directive with element", "[generator]") {
    auto generator = generateOutput("use html5; div {}");
    REQUIRE(generator.getHtml() == "<!DOCTYPE html><div></div>");
}

TEST_CASE("Generator expands element template", "[generator]") {
    std::string input = R"(
        [Template] @Element MyCard {
            div { class: "card"; text { "A card" } }
        }
        body { @Element MyCard; }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<body><div class="card">A card</div></body>)");
}

TEST_CASE("Generator expands style template", "[generator]") {
    std::string input = R"(
        [Template] @Style ButtonStyle {
            color: white;
            background-color: blue;
        }
        button {
            style {
                @Style ButtonStyle;
                border: none;
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<button style="color:white;background-color:blue;border:none;"></button>)");
}

TEST_CASE("Generator expands variable template", "[generator]") {
    std::string input = R"(
        [Template] @Var MyTheme {
            primaryColor: "blue";
            textColor: "white";
        }

        div {
            style {
                background-color: MyTheme(primaryColor);
                color: MyTheme(textColor);
            }
        }
    )";
    auto generator = generateOutput(input);
    REQUIRE(generator.getHtml() == R"(<div style="background-color:blue;color:white;"></div>)");
}