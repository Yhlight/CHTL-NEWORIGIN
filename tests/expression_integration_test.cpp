#include "../third-party/catch.hpp"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLParser/ExpressionParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

using namespace CHTL;

TEST_CASE("Expression Parser - CSS Arithmetic", "[expression][css]") {
    SECTION("Addition with same units") {
        String source = R"(
            div {
                style {
                    width: 100px + 50px;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        // Generate HTML (内联样式在HTML中)
        GeneratorConfig config;
        String html = HtmlGenerator::generate(ast, config);
        
        // Should evaluate to 150px in inline style
        REQUIRE(html.find("150px") != String::npos);
        REQUIRE(html.find("style=") != String::npos);
    }
    
    SECTION("Subtraction") {
        String source = R"(
            div {
                style {
                    width: 200px - 50px;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        REQUIRE(html.find("150px") != String::npos);
    }
    
    SECTION("Multiplication") {
        String source = R"(
            div {
                style {
                    width: 50px * 2;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        REQUIRE(html.find("100px") != String::npos);
    }
    
    SECTION("Division") {
        String source = R"(
            div {
                style {
                    width: 100px / 2;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        REQUIRE(html.find("50px") != String::npos);
    }
}

TEST_CASE("Expression Parser - Complex Expressions", "[expression][css]") {
    SECTION("Multiple operations with precedence") {
        String source = R"(
            div {
                style {
                    width: 100px + 50px * 2;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        // 应该是 100px + (50px * 2) = 100px + 100px = 200px
        REQUIRE(html.find("200px") != String::npos);
    }
    
    SECTION("Parentheses") {
        String source = R"(
            div {
                style {
                    width: (100px + 50px) * 2;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        // (100px + 50px) * 2 = 150px * 2 = 300px
        REQUIRE(html.find("300px") != String::npos);
    }
}

TEST_CASE("Expression Parser - Unit Handling", "[expression][css]") {
    SECTION("Different units use calc()") {
        String source = R"(
            div {
                style {
                    width: 100px + 50%;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        // 不同单位应该使用calc()
        REQUIRE(html.find("calc(") != String::npos);
    }
    
    SECTION("Unitless multiplication") {
        String source = R"(
            div {
                style {
                    width: 50px * 3;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        REQUIRE(html.find("150px") != String::npos);
    }
}

TEST_CASE("Expression Parser - Conditional Expressions", "[expression][css]") {
    SECTION("Simple conditional") {
        String source = R"(
            div {
                style {
                    display: 1 ? block : none;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        REQUIRE(html.find("block") != String::npos);
    }
    
    SECTION("Conditional with comparison") {
        String source = R"(
            div {
                style {
                    width: 100 > 50 ? 200px : 50px;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        REQUIRE(html.find("200px") != String::npos);
    }
}
