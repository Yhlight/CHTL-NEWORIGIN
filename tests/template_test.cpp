#include "../third-party/catch.hpp"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLTemplate/TemplateRegistry.h"

using namespace CHTL;

TEST_CASE("Template System - @Style Template", "[template]") {
    // 清理注册表
    TemplateRegistry::getInstance().clear();
    
    SECTION("Basic style template") {
        String source = R"(
            [Template] @Style Button {
                padding: 10px 20px;
                border-radius: 5px;
                cursor: pointer;
            }
            
            button {
                style {
                    @Style Button;
                    background: blue;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // 应包含模板的样式
        REQUIRE(html.find("padding: 10px 20px") != String::npos);
        REQUIRE(html.find("border-radius: 5px") != String::npos);
        REQUIRE(html.find("cursor: pointer") != String::npos);
        // 也应包含元素自己的样式
        REQUIRE(html.find("background: blue") != String::npos);
    }
    
    SECTION("Multiple style templates") {
        String source = R"(
            [Template] @Style Primary {
                color: white;
                background: blue;
            }
            
            [Template] @Style Rounded {
                border-radius: 8px;
            }
            
            div {
                style {
                    @Style Primary;
                    @Style Rounded;
                    padding: 10px;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        REQUIRE(html.find("color: white") != String::npos);
        REQUIRE(html.find("background: blue") != String::npos);
        REQUIRE(html.find("border-radius: 8px") != String::npos);
        REQUIRE(html.find("padding: 10px") != String::npos);
    }
    
    SECTION("Template with expressions") {
        String source = R"(
            [Template] @Style Box {
                width: 100px;
                height: 50px;
            }
            
            div {
                id: base;
                style {
                    @Style Box;
                }
            }
            
            div {
                style {
                    width: base.width + 20px;
                    height: base.height * 2;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // 第一个div应有模板样式
        REQUIRE(html.find("100px") != String::npos);
        REQUIRE(html.find("50px") != String::npos);
        // 第二个div应有计算后的样式
        REQUIRE(html.find("120px") != String::npos);
        REQUIRE(html.find("100px") != String::npos);
    }
}

TEST_CASE("Template System - Template Override", "[template]") {
    TemplateRegistry::getInstance().clear();
    
    SECTION("Later property overrides earlier") {
        String source = R"(
            [Template] @Style Theme {
                color: white;
                background: blue;
            }
            
            div {
                style {
                    @Style Theme;
                    background: red;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // color应该来自模板
        REQUIRE(html.find("color: white") != String::npos);
        // background应该被覆盖为red
        REQUIRE(html.find("background: red") != String::npos);
    }
}

TEST_CASE("Template System - @Element Template", "[template]") {
    TemplateRegistry::getInstance().clear();
    
    SECTION("Basic element template") {
        String source = R"(
            [Template] @Element Box {
                div {
                    class: box-item;
                    style {
                        width: 100px;
                        height: 100px;
                    }
                }
                span {
                    text: "Box content";
                }
            }
            
            body {
                @Element Box;
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // 应该包含模板中的div
        REQUIRE(html.find("<div") != String::npos);
        REQUIRE(html.find("box-item") != String::npos);
        REQUIRE(html.find("width: 100px") != String::npos);
        // 应该包含模板中的span
        REQUIRE(html.find("<span") != String::npos);
        REQUIRE(html.find("Box content") != String::npos);
    }
    
    SECTION("Multiple element template usage") {
        String source = R"(
            [Template] @Element Card {
                div {
                    class: card;
                    style {
                        padding: 20px;
                    }
                }
            }
            
            body {
                @Element Card;
                @Element Card;
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // 应该包含两个card div
        size_t first = html.find("class=\"card\"");
        REQUIRE(first != String::npos);
        size_t second = html.find("class=\"card\"", first + 1);
        REQUIRE(second != String::npos);
    }
}

TEST_CASE("Template System - @Var Template", "[template]") {
    TemplateRegistry::getInstance().clear();
    
    SECTION("Basic variable template") {
        String source = R"(
            [Template] @Var Colors {
                primary: blue;
                secondary: red;
            }
            
            div {
                style {
                    color: Colors(primary);
                    background: Colors(secondary);
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        REQUIRE(html.find("color: blue") != String::npos);
        REQUIRE(html.find("background: red") != String::npos);
    }
    
    SECTION("Variable template in expressions") {
        String source = R"(
            [Template] @Var Sizes {
                base: 10px;
                large: 20px;
            }
            
            div {
                style {
                    width: Sizes(base) * 2;
                    height: Sizes(large) + 10px;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        REQUIRE(html.find("20px") != String::npos);
        REQUIRE(html.find("30px") != String::npos);
    }
}
