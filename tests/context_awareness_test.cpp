#include "../third-party/catch.hpp"
#include "SharedCore/SaltBridge.h"
#include "CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

using namespace CHTL;
using namespace CHTL::JS;
using namespace CHTL::Bridge;

TEST_CASE("Context Awareness - & Resolution", "[context]") {
    SECTION("& with class context") {
        SaltBridge& bridge = SaltBridge::getInstance();
        
        // 设置上下文
        ContextInfo ctx;
        ctx.currentClass = "my-card";
        ctx.currentTag = "div";
        bridge.pushContext(ctx);
        
        // 测试 & 解析
        String selector = bridge.resolveAmpersand(false);  // script中优先id
        REQUIRE(!selector.empty());
        REQUIRE(selector.find("my-card") != String::npos);
        
        bridge.popContext();
    }
    
    SECTION("& with id context") {
        SaltBridge& bridge = SaltBridge::getInstance();
        
        ContextInfo ctx;
        ctx.currentId = "submit-btn";
        ctx.currentTag = "button";
        bridge.pushContext(ctx);
        
        String selector = bridge.resolveAmpersand(false);  // script中优先id
        REQUIRE(!selector.empty());
        REQUIRE(selector.find("submit-btn") != String::npos);
        
        bridge.popContext();
    }
}

TEST_CASE("Context Awareness - Full Pipeline", "[context][integration]") {
    SECTION("{{&}} in script with class") {
        String source = R"(
            div {
                class: test-card;
                
                script {
                    {{&}}->classList.add('active');
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        // Generate JavaScript
        GeneratorConfig config;
        String js = JsGenerator::generate(ast, config);
        
        // Should convert {{&}} to the actual selector
        // In script context, & with class should resolve to document.querySelector('.test-card')
        REQUIRE(js.find("test-card") != String::npos);
        REQUIRE(js.find("classList") != String::npos);
        REQUIRE(js.find("{{") == String::npos);
    }
    
    SECTION("{{&}} in script with id") {
        String source = R"(
            button {
                id: submit-btn;
                
                script {
                    {{&}}->disabled = true;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        GeneratorConfig config;
        String js = JsGenerator::generate(ast, config);
        
        // With id, should use getElementById
        REQUIRE(js.find("submit-btn") != String::npos);
        REQUIRE(js.find("disabled") != String::npos);
    }
}

TEST_CASE("Context Awareness - Nested Elements", "[context]") {
    SECTION("Nested elements maintain correct context") {
        String source = R"(
            div {
                class: outer;
                
                div {
                    class: inner;
                    
                    script {
                        {{&}}->textContent = 'Inner';
                    }
                }
                
                script {
                    {{&}}->textContent = 'Outer';
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String js = JsGenerator::generate(ast);
        
        // Both contexts should be preserved
        REQUIRE(js.find("inner") != String::npos);
        REQUIRE(js.find("outer") != String::npos);
    }
}
