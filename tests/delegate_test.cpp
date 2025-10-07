#include "../third-party/catch.hpp"
#include "CHTL-JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

using namespace CHTL;
using namespace CHTL::JS;

TEST_CASE("Delegate - Basic Structure", "[delegate]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Simple delegate with single target") {
        String code = R"(
            {{parent}}->Delegate {
                target: {{child}},
                click: handleClick
            };
        )";
        String output = gen.generate(code);
        
        // 应该生成事件委托代码
        REQUIRE(output.find("parent") != String::npos);
        REQUIRE(output.find("child") != String::npos);
        REQUIRE(output.find("click") != String::npos);
        REQUIRE(output.find("handleClick") != String::npos);
    }
    
    SECTION("Delegate with multiple targets") {
        String code = R"(
            {{.container}}->Delegate {
                target: [{{.item}}, {{.card}}],
                click: handleClick
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("container") != String::npos);
        REQUIRE(output.find("item") != String::npos);
        REQUIRE(output.find("card") != String::npos);
    }
    
    SECTION("Delegate with multiple events") {
        String code = R"(
            {{#list}}->Delegate {
                target: {{.list-item}},
                click: onClick,
                mouseenter: onEnter,
                mouseleave: onLeave
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("list") != String::npos);
        REQUIRE(output.find("click") != String::npos);
        REQUIRE(output.find("mouseenter") != String::npos);
        REQUIRE(output.find("mouseleave") != String::npos);
    }
}

TEST_CASE("Delegate - Code Generation", "[delegate]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Generate delegation code") {
        String code = R"(
            {{.parent}}->Delegate {
                target: {{.child}},
                click: (e) => { console.log(e.target); }
            };
        )";
        String output = gen.generate(code);
        
        // 应该生成带有事件委托的代码
        // parent.addEventListener('click', function(e) { if (e.target.matches('.child')) { ... } })
        REQUIRE(output.find("addEventListener") != String::npos);
        REQUIRE(output.find("click") != String::npos);
    }
}

TEST_CASE("Delegate - Full Pipeline", "[delegate][integration]") {
    SECTION("Delegate in CHTL script block") {
        String source = R"(
            div {
                class: todo-list;
                
                script {
                    {{&}}->Delegate {
                        target: {{.todo-item}},
                        click: (e) => {
                            console.log('Todo item clicked:', e.target);
                        }
                    };
                }
            }
        )";
        
        CHTLParser parser(source);
        auto ast = parser.parse();
        
        REQUIRE(!parser.hasErrors());
        
        String html = HtmlGenerator::generate(ast);
        
        // 应该包含事件委托代码
        REQUIRE(html.find("todo-list") != String::npos);
        REQUIRE(html.find("Delegate") == String::npos);  // Delegate应被转换
    }
}
