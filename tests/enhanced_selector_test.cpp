#include "../third-party/catch.hpp"
#include "SharedCore/SaltBridge.h"
#include "CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

using namespace CHTL;
using namespace CHTL::JS;
using namespace CHTL::Bridge;

TEST_CASE("Enhanced Selector - Basic Conversions", "[enhanced-selector]") {
    SaltBridge& bridge = SaltBridge::getInstance();
    
    SECTION("Class selector") {
        String result = bridge.convertEnhancedSelector(".card");
        REQUIRE(result == "document.querySelector('.card')");
    }
    
    SECTION("ID selector") {
        String result = bridge.convertEnhancedSelector("#box");
        REQUIRE(result == "document.getElementById('box')");
    }
    
    SECTION("Tag selector") {
        // Auto type now correctly recognizes HTML tags
        String result = bridge.convertEnhancedSelector("button");
        REQUIRE(result == "document.querySelector('button')");
    }
    
    SECTION("Descendant selector") {
        String result = bridge.convertEnhancedSelector(".box button");
        REQUIRE(result == "document.querySelector('.box button')");
    }
}

TEST_CASE("Enhanced Selector - Index Access", "[enhanced-selector]") {
    SaltBridge& bridge = SaltBridge::getInstance();
    
    SECTION("Class with index") {
        String result = bridge.convertEnhancedSelector(".card[0]");
        REQUIRE(result.find("querySelectorAll") != String::npos);
        REQUIRE(result.find("[0]") != String::npos);
    }
    
    SECTION("Tag with index") {
        String result = bridge.convertEnhancedSelector("button[2]");
        REQUIRE(result.find("querySelectorAll") != String::npos);
        REQUIRE(result.find("[2]") != String::npos);
    }
}

TEST_CASE("CHTL JS Generator - Arrow Operator", "[chtljs]") {
    CHTLJSGenerator gen;
    
    SECTION("Simple arrow operator") {
        String input = "test->method()";
        String output = gen.generate(input);
        REQUIRE(output.find("test.method()") != String::npos);
        REQUIRE(output.find("->") == String::npos);
    }
    
    SECTION("Chained arrow operators") {
        String input = "obj->prop->method()";
        String output = gen.generate(input);
        REQUIRE(output.find("obj.prop.method()") != String::npos);
        REQUIRE(output.find("->") == String::npos);
    }
}

TEST_CASE("CHTL JS Generator - Enhanced Selector Conversion", "[chtljs]") {
    CHTLJSGenerator gen;
    
    SECTION("Class selector with arrow") {
        String input = "{{.card}}->addEventListener('click', fn)";
        String output = gen.generate(input);
        REQUIRE(output.find("document.querySelector('.card')") != String::npos);
        REQUIRE(output.find(".addEventListener") != String::npos);
        REQUIRE(output.find("{{") == String::npos);
        REQUIRE(output.find("->") == String::npos);
    }
    
    SECTION("ID selector") {
        String input = "{{#myId}}->textContent = 'test'";
        String output = gen.generate(input);
        REQUIRE(output.find("document.getElementById('myId')") != String::npos);
        REQUIRE(output.find("{{") == String::npos);
    }
    
    SECTION("Multiple selectors") {
        String input = "{{.card}}->style.color = 'red'; {{#box}}->style.width = '100px'";
        String output = gen.generate(input);
        REQUIRE(output.find("document.querySelector('.card')") != String::npos);
        REQUIRE(output.find("document.getElementById('box')") != String::npos);
    }
}

TEST_CASE("Full Pipeline - Enhanced Selectors in Script", "[integration]") {
    SECTION("Simple script with enhanced selector") {
        String source = R"(
            div {
                class: test-box;
                
                script {
                    {{.test-box}}->addEventListener('click', function() {
                        console.log('clicked');
                    });
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(ast != nullptr);
        REQUIRE(!parser.hasErrors());
        
        // Generate JavaScript
        GeneratorConfig config;
        String js = JsGenerator::generate(ast, config);
        
        // Should have document.querySelector
        REQUIRE(js.find("document.querySelector('.test-box')") != String::npos);
        // Should have addEventListener
        REQUIRE(js.find("addEventListener") != String::npos);
        // Should NOT have {{ or ->
        REQUIRE(js.find("{{") == String::npos);
        REQUIRE(js.find("->") == String::npos);
    }
    
    SECTION("Multiple scripts with selectors") {
        String source = R"(
            div {
                class: card;
                script {
                    {{.card}}->classList.add('active');
                }
            }
            button {
                id: submit-btn;
                script {
                    {{#submit-btn}}->disabled = true;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String js = JsGenerator::generate(ast);
        
        // Check both conversions
        REQUIRE(js.find("document.querySelector('.card')") != String::npos);
        REQUIRE(js.find("document.getElementById('submit-btn')") != String::npos);
    }
}

TEST_CASE("Enhanced Selector - Ampersand Reference", "[enhanced-selector]") {
    SECTION("Ampersand in script context") {
        // Note: & resolution requires context from parent element
        // This is tested in context awareness tests
        CHTLJSGenerator gen;
        
        // Without context, & should resolve to 'this'
        String input = "{{&}}->addEventListener('click', fn)";
        String output = gen.generate(input);
        
        // Should convert & to 'this' when no context
        REQUIRE(output.find("this") != String::npos);
        REQUIRE(output.find("addEventListener") != String::npos);
    }
}
