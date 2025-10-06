#include "../third-party/catch.hpp"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "SharedCore/SaltBridge.h"

using namespace CHTL;

TEST_CASE("Property Reference - Basic Usage", "[property-reference]") {
    SECTION("Reference width property") {
        String source = R"(
            div {
                id: box;
                style {
                    width: 100px;
                }
            }
            
            div {
                style {
                    width: box.width + 20px;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // First div should have width: 100px
        REQUIRE(html.find("100px") != String::npos);
        // Second div should have width: 120px (100px + 20px)
        REQUIRE(html.find("120px") != String::npos);
    }
    
    SECTION("Reference multiple properties") {
        String source = R"(
            div {
                id: box;
                style {
                    width: 100px;
                    height: 50px;
                }
            }
            
            div {
                style {
                    width: box.width * 2;
                    height: box.height + 10px;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // Debug: print html
        INFO("Generated HTML: " << html);
        
        // box.width * 2 = 200px
        REQUIRE(html.find("200px") != String::npos);
        // box.height + 10px = 60px (注意：可能包含其他值如50px，需要更精确的检查)
        size_t pos60 = html.find("60px");
        INFO("Position of 60px: " << pos60);
        REQUIRE(pos60 != String::npos);
    }
}

TEST_CASE("Property Reference - Class Selector", "[property-reference]") {
    SECTION("Reference via class") {
        String source = R"(
            div {
                class: container;
                style {
                    width: 500px;
                }
            }
            
            div {
                style {
                    width: .container.width / 2;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // .container.width / 2 = 500px / 2 = 250px
        REQUIRE(html.find("250px") != String::npos);
    }
}

TEST_CASE("Property Reference - Complex Expressions", "[property-reference]") {
    SECTION("Property reference in complex expression") {
        String source = R"(
            div {
                id: base;
                style {
                    width: 100px;
                }
            }
            
            div {
                style {
                    width: (base.width + 50px) * 2;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // (100px + 50px) * 2 = 300px
        REQUIRE(html.find("300px") != String::npos);
    }
}

TEST_CASE("Property Reference - Not Found", "[property-reference]") {
    SECTION("Non-existent selector") {
        String source = R"(
            div {
                style {
                    width: nonexistent.width;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // Should contain comment about not found
        REQUIRE(html.find("not found") != String::npos);
    }
}
