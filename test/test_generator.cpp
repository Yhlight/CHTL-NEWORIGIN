#include "../third-party/catch.hpp"
#include "../include/CHTL/Compiler.hpp"

using namespace CHTL;

TEST_CASE("Generator基础功能", "[generator]") {
    SECTION("生成简单HTML") {
        String source = R"(
            div {
                text { Hello World }
            }
        )";
        
        Compiler compiler;
        auto result = compiler.compile(source);
        
        REQUIRE(result.success == true);
        REQUIRE(result.html.find("<div>") != String::npos);
        REQUIRE(result.html.find("Hello World") != String::npos);
        REQUIRE(result.html.find("</div>") != String::npos);
    }
    
    SECTION("生成带属性的HTML") {
        String source = R"(
            div {
                id: box;
                class: container;
                text { Content }
            }
        )";
        
        Compiler compiler;
        auto result = compiler.compile(source);
        
        REQUIRE(result.success == true);
        REQUIRE(result.html.find("id=\"box\"") != String::npos);
        REQUIRE(result.html.find("class=\"container\"") != String::npos);
    }
    
    SECTION("生成嵌套HTML") {
        String source = R"(
            html {
                body {
                    div {
                        span {
                            text { Test }
                        }
                    }
                }
            }
        )";
        
        Compiler compiler;
        auto result = compiler.compile(source);
        
        REQUIRE(result.success == true);
        REQUIRE(result.html.find("<html>") != String::npos);
        REQUIRE(result.html.find("<body>") != String::npos);
        REQUIRE(result.html.find("<div>") != String::npos);
        REQUIRE(result.html.find("<span>") != String::npos);
    }
}

TEST_CASE("Generator高级功能", "[generator]") {
    SECTION("处理内联样式") {
        String source = R"(
            div {
                style {
                    width: 100px;
                    height: 200px;
                }
            }
        )";
        
        Compiler compiler;
        auto result = compiler.compile(source);
        
        REQUIRE(result.success == true);
        REQUIRE(result.html.find("style=") != String::npos);
    }
    
    SECTION("生成默认结构") {
        String source = "div { }";
        
        CompilerOptions options;
        options.defaultStruct = true;
        
        Compiler compiler(options);
        auto result = compiler.compile(source);
        
        REQUIRE(result.success == true);
        REQUIRE(result.html.find("<!DOCTYPE html>") != String::npos);
        REQUIRE(result.html.find("<meta charset=\"UTF-8\">") != String::npos);
    }
}
