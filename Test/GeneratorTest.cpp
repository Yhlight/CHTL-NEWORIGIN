#include "catch.hpp"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLManager/TemplateManager.h"
#include <memory>

// Helper to get the generated string from a CHTL snippet
std::string generate_from_string(const std::string& chtl_input) {
    CHTL::CHTLLexer lexer;
    std::vector<CHTL::Token> tokens = lexer.tokenize(chtl_input);
    CHTL::CHTLParser parser(chtl_input, tokens);
    std::unique_ptr<CHTL::DocumentNode> root = parser.parse();
    CHTL::CHTLGenerator generator;
    return generator.generate(root.get());
}

TEST_CASE("Generator produces DOCTYPE with 'use html5;'", "[generator]") {
    std::string input = R"(
        use html5;
        p {
            text { "Hello" }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result.rfind("<!DOCTYPE html>", 0) == 0);
}

TEST_CASE("Generator handles variable substitution", "[generator]") {
    CHTL::TemplateManager::getInstance().clear();
    std::string input = R"(
        [Template] @Var MyTheme {
            primaryColor: #3498db;
            basePadding: 10px;
        }
        div {
            style {
                color: MyTheme(primaryColor);
                padding: MyTheme(basePadding) * 2;
            }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<div style=\\\"color: #3498db;padding: 20px;\\\"></div>");
}

TEST_CASE("Generator handles single-level style template inheritance", "[generator]") {
    CHTL::TemplateManager::getInstance().clear();
    std::string input = R"(
        [Template] @Style BaseStyle {
            color: red;
        }
        [Template] @Style DerivedStyle {
            inherit @Style BaseStyle;
            font-size: 16px;
        }
        div {
            style {
                @Style DerivedStyle;
            }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<div style=\\\"color: red;font-size: 16px;\\\"></div>");
}

TEST_CASE("Generator handles property override in template inheritance", "[generator]") {
    CHTL::TemplateManager::getInstance().clear();
    std::string input = R"(
        [Template] @Style Base {
            color: red;
            font-size: 12px;
        }
        [Template] @Style Derived {
            inherit @Style Base;
            color: green;
        }
        div {
            style {
                @Style Derived;
            }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<div style=\\\"color: green;font-size: 12px;\\\"></div>");
}

TEST_CASE("Generator handles delete specialization", "[generator]") {
    CHTL::TemplateManager::getInstance().clear();
    std::string input = R"(
        [Template] @Style Base {
            color: red;
            font-size: 12px;
        }
        div {
            style {
                @Style Base {
                    delete font-size;
                }
            }
        }
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result == "<div style=\\\"color: red;\\\"></div>");
}

TEST_CASE("Generator handles [Origin] blocks", "[generator]") {
    CHTL::TemplateManager::getInstance().clear();
    std::string input = R"(
        [Origin] @Html {<p>Raw HTML</p>}
        [Origin] @Style {body { margin: 0; }}
        [Origin] @JavaScript {console.log("raw js");}
    )";
    std::string result = generate_from_string(input);
    REQUIRE(result.find("<p>Raw HTML</p>") != std::string::npos);
    REQUIRE(result.find("body { margin: 0; }") != std::string::npos);
    REQUIRE(result.find("<script>console.log(\"raw js\");</script>") != std::string::npos);
}