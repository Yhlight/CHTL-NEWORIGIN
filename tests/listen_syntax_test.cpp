#include "../third-party/catch.hpp"
#include "CHTL-JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

using namespace CHTL;
using namespace CHTL::JS;

TEST_CASE("CHTL JS Parser - EventBinding Parsing", "[listen][parser]") {
    SECTION("Simple event binding") {
        CHTLJSParser parser("");
        
        auto binding = parser.parseEventBinding("click: handleClick");
        
        REQUIRE(binding.has_value());
        REQUIRE(binding->eventName == "click");
        REQUIRE(binding->handler == "handleClick");
        REQUIRE_FALSE(binding->isOptional);
    }
    
    SECTION("Event binding with arrow function") {
        CHTLJSParser parser("");
        
        auto binding = parser.parseEventBinding("click: () => { console.log('clicked'); }");
        
        REQUIRE(binding.has_value());
        REQUIRE(binding->eventName == "click");
        REQUIRE(binding->handler == "() => { console.log('clicked'); }");
    }
    
    SECTION("Event binding with function keyword") {
        CHTLJSParser parser("");
        
        auto binding = parser.parseEventBinding("mouseenter: function() { alert('enter'); }");
        
        REQUIRE(binding.has_value());
        REQUIRE(binding->eventName == "mouseenter");
        REQUIRE(binding->handler == "function() { alert('enter'); }");
    }
    
    SECTION("Optional event binding") {
        CHTLJSParser parser("");
        
        auto binding = parser.parseEventBinding("mouseleave?: optionalHandler");
        
        REQUIRE(binding.has_value());
        REQUIRE(binding->eventName == "mouseleave");
        REQUIRE(binding->handler == "optionalHandler");
        REQUIRE(binding->isOptional);
    }
    
    SECTION("Event binding with trailing comma") {
        CHTLJSParser parser("");
        
        auto binding = parser.parseEventBinding("click: handler,");
        
        REQUIRE(binding.has_value());
        REQUIRE(binding->eventName == "click");
        REQUIRE(binding->handler == "handler");
    }
    
    SECTION("Event binding with whitespace") {
        CHTLJSParser parser("");
        
        auto binding = parser.parseEventBinding("  click  :  handler  ");
        
        REQUIRE(binding.has_value());
        REQUIRE(binding->eventName == "click");
        REQUIRE(binding->handler == "handler");
    }
}

TEST_CASE("CHTL JS Parser - Multiple Event Bindings", "[listen][parser]") {
    SECTION("Two simple bindings") {
        CHTLJSParser parser("");
        
        String code = "click: handleClick, mouseenter: handleEnter";
        auto bindings = parser.parseEventBindings(code);
        
        REQUIRE(bindings.size() == 2);
        REQUIRE(bindings[0].eventName == "click");
        REQUIRE(bindings[0].handler == "handleClick");
        REQUIRE(bindings[1].eventName == "mouseenter");
        REQUIRE(bindings[1].handler == "handleEnter");
    }
    
    SECTION("Bindings with arrow functions") {
        CHTLJSParser parser("");
        
        String code = "click: () => {}, hover: () => alert('hi')";
        auto bindings = parser.parseEventBindings(code);
        
        REQUIRE(bindings.size() == 2);
        REQUIRE(bindings[0].eventName == "click");
        REQUIRE(bindings[0].handler == "() => {}");
        REQUIRE(bindings[1].eventName == "hover");
        REQUIRE(bindings[1].handler == "() => alert('hi')");
    }
    
    SECTION("Mixed function types") {
        CHTLJSParser parser("");
        
        String code = "click: handleClick, mouseenter: () => {}, mouseleave: function() { console.log('leave'); }";
        auto bindings = parser.parseEventBindings(code);
        
        REQUIRE(bindings.size() == 3);
        REQUIRE(bindings[0].handler == "handleClick");
        REQUIRE(bindings[1].handler == "() => {}");
        REQUIRE(bindings[2].handler == "function() { console.log('leave'); }");
    }
    
    SECTION("Nested braces in function") {
        CHTLJSParser parser("");
        
        String code = "click: () => { if (true) { console.log('nested'); } }, hover: fn2";
        auto bindings = parser.parseEventBindings(code);
        
        REQUIRE(bindings.size() == 2);
        REQUIRE(bindings[0].eventName == "click");
        REQUIRE(bindings[0].handler == "() => { if (true) { console.log('nested'); } }");
        REQUIRE(bindings[1].eventName == "hover");
    }
}

TEST_CASE("CHTL JS Parser - ListenBlock Parsing", "[listen][parser]") {
    SECTION("Basic Listen block with enhanced selector") {
        CHTLJSParser parser("");
        
        String code = "{{.box}}->Listen { click: handleClick }";
        auto block = parser.parseListenBlock(code);
        
        REQUIRE(block.has_value());
        REQUIRE(block->target == "{{.box}}");
        REQUIRE(block->bindings.size() == 1);
        REQUIRE(block->bindings[0].eventName == "click");
    }
    
    SECTION("Listen block with identifier target") {
        CHTLJSParser parser("");
        
        String code = "element->Listen { click: fn }";
        auto block = parser.parseListenBlock(code);
        
        REQUIRE(block.has_value());
        REQUIRE(block->target == "element");
        REQUIRE(block->bindings.size() == 1);
    }
    
    SECTION("Listen block with multiple events") {
        CHTLJSParser parser("");
        
        String code = "{{#btn}}->Listen { click: fn1, mouseenter: fn2, mouseleave: fn3 }";
        auto block = parser.parseListenBlock(code);
        
        REQUIRE(block.has_value());
        REQUIRE(block->target == "{{#btn}}");
        REQUIRE(block->bindings.size() == 3);
        REQUIRE(block->bindings[0].eventName == "click");
        REQUIRE(block->bindings[1].eventName == "mouseenter");
        REQUIRE(block->bindings[2].eventName == "mouseleave");
    }
    
    SECTION("Listen block with arrow functions") {
        CHTLJSParser parser("");
        
        String code = "{{button}}->Listen { click: () => { alert('hi'); }, hover: () => {} }";
        auto block = parser.parseListenBlock(code);
        
        REQUIRE(block.has_value());
        REQUIRE(block->bindings.size() == 2);
        REQUIRE(block->bindings[0].handler == "() => { alert('hi'); }");
        REQUIRE(block->bindings[1].handler == "() => {}");
    }
    
    SECTION("Listen block with optional binding") {
        CHTLJSParser parser("");
        
        String code = "{{box}}->Listen { click: fn1, hover?: optFn }";
        auto block = parser.parseListenBlock(code);
        
        REQUIRE(block.has_value());
        REQUIRE(block->bindings.size() == 2);
        REQUIRE_FALSE(block->bindings[0].isOptional);
        REQUIRE(block->bindings[1].isOptional);
    }
}

TEST_CASE("CHTL JS Parser - findListenBlock", "[listen][parser]") {
    SECTION("Find single Listen block") {
        CHTLJSParser parser("");
        
        String code = "const x = 5; {{box}}->Listen { click: fn }; console.log(x);";
        auto pos = parser.findListenBlock(code);
        
        REQUIRE(pos.has_value());
        REQUIRE(pos->first < pos->second);
        
        String extracted = code.substr(pos->first, pos->second - pos->first);
        REQUIRE(extracted.find("Listen") != String::npos);
    }
    
    SECTION("Find multiple Listen blocks") {
        CHTLJSParser parser("");
        
        String code = "{{box1}}->Listen { click: fn1 }; {{box2}}->Listen { hover: fn2 };";
        
        auto pos1 = parser.findListenBlock(code, 0);
        REQUIRE(pos1.has_value());
        
        auto pos2 = parser.findListenBlock(code, pos1->second);
        REQUIRE(pos2.has_value());
        REQUIRE(pos2->first > pos1->second);
    }
    
    SECTION("No Listen block found") {
        CHTLJSParser parser("");
        
        String code = "const x = 5; console.log(x);";
        auto pos = parser.findListenBlock(code);
        
        REQUIRE_FALSE(pos.has_value());
    }
}

TEST_CASE("CHTL JS Generator - Listen Code Generation", "[listen][generator]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Generate addEventListener from Listen block") {
        String code = "{{.box}}->Listen { click: handleClick };";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.querySelector('.box')") != String::npos);
        REQUIRE(output.find(".addEventListener('click', handleClick)") != String::npos);
    }
    
    SECTION("Generate multiple addEventListener calls") {
        String code = "{{#btn}}->Listen { click: fn1, mouseenter: fn2 };";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.getElementById('btn')") != String::npos);
        REQUIRE(output.find(".addEventListener('click', fn1)") != String::npos);
        REQUIRE(output.find(".addEventListener('mouseenter', fn2)") != String::npos);
    }
    
    SECTION("Generate with arrow function") {
        String code = "{{button}}->Listen { click: () => { console.log('test'); } };";
        String output = gen.generate(code);
        
        REQUIRE(output.find("document.querySelector('button')") != String::npos);
        REQUIRE(output.find(".addEventListener('click', () => { console.log('test'); })") != String::npos);
    }
    
    SECTION("Generate with function keyword") {
        String code = "element->Listen { click: function() { alert('hi'); } };";
        String output = gen.generate(code);
        
        REQUIRE(output.find(".addEventListener('click', function() { alert('hi'); })") != String::npos);
    }
}

TEST_CASE("CHTL JS - Listen Full Pipeline Integration", "[listen][integration]") {
    SECTION("Complete CHTL to JavaScript with Listen") {
        String source = R"(
            button {
                class: submit-btn;
                
                script {
                    {{&}}->Listen {
                        click: () => {
                            alert('Button clicked!');
                        },
                        mouseenter: function() {
                            console.log('Mouse entered');
                        }
                    };
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // 应该包含转换后的JavaScript（注意：Token间有空格）
        REQUIRE(html.find("document.querySelector('.submit-btn')") != String::npos);
        REQUIRE(html.find("addEventListener") != String::npos);
        REQUIRE(html.find("click") != String::npos);
        REQUIRE(html.find("mouseenter") != String::npos);
        REQUIRE(html.find("alert") != String::npos);
        REQUIRE(html.find("Button clicked") != String::npos);
    }
    
    SECTION("Multiple Listen blocks in same script") {
        String source = R"(
            div {
                script {
                    {{.box1}}->Listen {
                        click: handler1
                    };
                    
                    {{.box2}}->Listen {
                        hover: handler2
                    };
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        REQUIRE(html.find("document.querySelector('.box1')") != String::npos);
        REQUIRE(html.find("document.querySelector('.box2')") != String::npos);
        REQUIRE(html.find("addEventListener") != String::npos);
        REQUIRE(html.find("handler") != String::npos);
    }
    
    SECTION("Listen with nested functions") {
        String source = R"(
            div {
                id: container;
                
                script {
                    {{&}}->Listen {
                        click: () => {
                            const nested = () => {
                                console.log('nested');
                            };
                            nested();
                        }
                    };
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        REQUIRE(html.find("document.getElementById('container')") != String::npos);
        REQUIRE(html.find("nested") != String::npos);
        REQUIRE(html.find("console") != String::npos);
    }
}

TEST_CASE("CHTL JS - Listen Edge Cases", "[listen][edge-cases]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Listen with string in event handler") {
        String code = R"({{box}}->Listen { click: () => { alert("click, with comma"); } };)";
        String output = gen.generate(code);
        
        REQUIRE(output.find("alert(\"click, with comma\")") != String::npos);
    }
    
    SECTION("Listen with complex nested structure") {
        String code = R"(
            {{box}}->Listen {
                click: () => {
                    const obj = { a: 1, b: 2 };
                    if (obj.a) {
                        console.log('test');
                    }
                }
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("const obj = { a: 1, b: 2 }") != String::npos);
        REQUIRE(output.find("if (obj.a)") != String::npos);
    }
    
    SECTION("Listen with trailing commas and semicolons") {
        String code = "{{box}}->Listen { click: fn1,; hover: fn2;, };";
        String output = gen.generate(code);
        
        // Should still parse correctly despite trailing punctuation
        REQUIRE(output.find(".addEventListener('click', fn1)") != String::npos);
        REQUIRE(output.find(".addEventListener('hover', fn2)") != String::npos);
    }
}
