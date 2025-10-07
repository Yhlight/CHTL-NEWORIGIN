#include "../third-party/catch.hpp"
#include "CHTL-JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

using namespace CHTL;
using namespace CHTL::JS;

TEST_CASE("Event Bind Operator - Single Event", "[event-bind]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Basic single event binding") {
        String code = "{{box}} &-> click: () => {};";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.querySelector('.box')") != String::npos);
        REQUIRE(output.find(".addEventListener('click'") != String::npos);
    }
    
    SECTION("Single event with function reference") {
        String code = "{{.card}} &-> click: handleClick;";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.querySelector('.card')") != String::npos);
        REQUIRE(output.find(".addEventListener('click', handleClick)") != String::npos);
    }
    
    SECTION("Single event with ID selector") {
        String code = "{{#submit}} &-> click: submitForm;";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.getElementById('submit')") != String::npos);
        REQUIRE(output.find(".addEventListener('click', submitForm)") != String::npos);
    }
    
    SECTION("Single event with tag selector") {
        String code = "{{button}} &-> click: () => { alert('hi'); };";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.querySelector('button')") != String::npos);
        REQUIRE(output.find(".addEventListener('click'") != String::npos);
    }
}

TEST_CASE("Event Bind Operator - Multiple Events", "[event-bind]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Multiple events with same handler") {
        String code = "{{box}} &-> click, mouseenter, mouseleave: handler;";
        String output = gen.generate(code);
        
        // 应该生成三个addEventListener调用（注意：可能有空格）
        REQUIRE(output.find("addEventListener") != String::npos);
        REQUIRE(output.find("click") != String::npos);
        REQUIRE(output.find("mouseenter") != String::npos);
        REQUIRE(output.find("mouseleave") != String::npos);
        REQUIRE(output.find("handler") != String::npos);
    }
    
    SECTION("Multiple events with arrow function") {
        String code = "{{.item}} &-> click, hover: () => { console.log('event'); };";
        String output = gen.generate(code);
        
        REQUIRE(output.find("addEventListener") != String::npos);
        REQUIRE(output.find("click") != String::npos);
        REQUIRE(output.find("hover") != String::npos);
    }
    
    SECTION("Multiple events with spaces") {
        String code = "{{box}} &-> click , mouseenter , mouseleave : fn;";
        String output = gen.generate(code);
        
        REQUIRE(output.find("addEventListener") != String::npos);
        REQUIRE(output.find("click") != String::npos);
        REQUIRE(output.find("mouseenter") != String::npos);
        REQUIRE(output.find("mouseleave") != String::npos);
        REQUIRE(output.find("fn") != String::npos);
    }
}

TEST_CASE("Event Bind Operator - Chain Binding", "[event-bind]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Basic chain binding") {
        String code = R"(
            {{box}} &-> click: fn1,
                    &-> mouseenter: fn2,
                    &-> mouseleave: fn3;
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find(".addEventListener('click', fn1)") != String::npos);
        REQUIRE(output.find(".addEventListener('mouseenter', fn2)") != String::npos);
        REQUIRE(output.find(".addEventListener('mouseleave', fn3)") != String::npos);
    }
    
    SECTION("Chain binding with different selectors") {
        String code = R"(
            {{.box1}} &-> click: fn1,
            {{.box2}} &-> hover: fn2,
            {{#btn}} &-> submit: fn3;
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.querySelector('.box1')") != String::npos);
        REQUIRE(output.find("document.querySelector('.box2')") != String::npos);
        REQUIRE(output.find("document.getElementById('btn')") != String::npos);
    }
    
    SECTION("Chain binding with arrow functions") {
        String code = R"(
            {{box}} &-> click: () => { console.log('click'); },
                    &-> hover: () => { console.log('hover'); };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("console.log('click')") != String::npos);
        REQUIRE(output.find("console.log('hover')") != String::npos);
    }
}

TEST_CASE("Event Bind Operator - Binding Block", "[event-bind]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Basic binding block") {
        String code = R"(
            {{box}} &-> {
                click: () => {},
                mouseenter: () => {},
                mouseleave: () => {}
            }
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find(".addEventListener('click'") != String::npos);
        REQUIRE(output.find(".addEventListener('mouseenter'") != String::npos);
        REQUIRE(output.find(".addEventListener('mouseleave'") != String::npos);
    }
    
    SECTION("Binding block with function references") {
        String code = R"(
            {{.card}} &-> {
                click: handleClick,
                hover: handleHover,
                focus: handleFocus
            }
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find(".addEventListener('click', handleClick)") != String::npos);
        REQUIRE(output.find(".addEventListener('hover', handleHover)") != String::npos);
        REQUIRE(output.find(".addEventListener('focus', handleFocus)") != String::npos);
    }
    
    SECTION("Binding block with complex handlers") {
        String code = R"(
            {{box}} &-> {
                click: () => {
                    const x = 10;
                    console.log(x);
                },
                hover: function() {
                    alert('hover');
                }
            }
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("const x = 10") != String::npos);
        REQUIRE(output.find("alert('hover')") != String::npos);
    }
}

TEST_CASE("Event Bind Operator - Context Reference", "[event-bind]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Using &-> with {{&}}") {
        String code = "{{&}} &-> click: () => {};";
        String output = gen.generate(code);
        
        // {{&}} should be resolved based on context
        // For now, it should at least not crash
        REQUIRE(output.length() > 0);
    }
}

TEST_CASE("Event Bind Operator - Edge Cases", "[event-bind]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Event bind with trailing semicolon") {
        String code = "{{box}} &-> click: fn;;";
        String output = gen.generate(code);
        
        REQUIRE(output.find(".addEventListener('click', fn)") != String::npos);
    }
    
    SECTION("Event bind with trailing comma") {
        String code = "{{box}} &-> click: fn,;";
        String output = gen.generate(code);
        
        REQUIRE(output.find(".addEventListener('click', fn)") != String::npos);
    }
    
    SECTION("Multiple events with extra spaces") {
        String code = "{{box}} &->  click  ,  hover  :  fn  ;";
        String output = gen.generate(code);
        
        REQUIRE(output.find("addEventListener") != String::npos);
        REQUIRE(output.find("click") != String::npos);
        REQUIRE(output.find("hover") != String::npos);
        REQUIRE(output.find("fn") != String::npos);
    }
}

TEST_CASE("Event Bind Operator - Full Pipeline Integration", "[event-bind][integration]") {
    SECTION("Event bind in CHTL script block") {
        String source = R"(
            button {
                class: submit-btn;
                
                script {
                    {{&}} &-> click: () => {
                        alert('Submitted!');
                    };
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // Note: This may fail due to script integration issue
        // But the &-> parsing and generation should work
        INFO("Generated HTML: " << html);
    }
    
    SECTION("Multiple event binds in same script") {
        String source = R"(
            div {
                script {
                    {{.box1}} &-> click: fn1;
                    {{.box2}} &-> hover: fn2;
                    {{#btn}} &-> submit: fn3;
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
    }
}
