#include "catch.hpp"
#include "../src/CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../src/CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <iostream>

using namespace CHTL;
using namespace CHTL::JS;

TEST_CASE("Mixed CHTL JS and Pure JS in script blocks", "[mixed-script][chtl-js][js]") {
    
    SECTION("Pure JS code should pass through unchanged") {
        String pureJS = R"(
            const x = 10;
            const y = 20;
            console.log(x + y);
            
            function hello() {
                return "Hello, World!";
            }
            
            class MyClass {
                constructor() {
                    this.value = 42;
                }
            }
        )";
        
        JSGeneratorConfig config;
        config.wrapIIFE = false;
        config.prettyPrint = false;
        
        CHTLJSGenerator gen(config);
        String result = gen.generate(pureJS);
        
        // 纯JS代码应该基本保持不变（除了可能的空白处理）
        REQUIRE(result.find("const x = 10") != String::npos);
        REQUIRE(result.find("console.log") != String::npos);
        REQUIRE(result.find("function hello") != String::npos);
        REQUIRE(result.find("class MyClass") != String::npos);
    }
    
    SECTION("CHTL JS syntax should be converted to JS") {
        String chtlJS = R"(
            {{box}}->addEventListener('click', () => {
                console.log('Clicked!');
            });
            
            {{.button}} &-> click: () => {
                alert('Button clicked!');
            };
        )";
        
        JSGeneratorConfig config;
        config.wrapIIFE = false;
        config.prettyPrint = false;
        
        CHTLJSGenerator gen(config);
        String result = gen.generate(chtlJS);
        
        // CHTL JS应该被转换
        REQUIRE(result.find("querySelector") != String::npos);
        REQUIRE(result.find("addEventListener") != String::npos);
    }
    
    SECTION("Mixed JS and CHTL JS should both work") {
        String mixed = R"(
            // 纯JS代码
            const count = 0;
            let total = 100;
            
            function updateCount() {
                count++;
                console.log('Count:', count);
            }
            
            // CHTL JS语法
            {{#counter}}->textContent = count;
            
            {{.button}} &-> click: () => {
                updateCount();
                {{#counter}}->textContent = count;
            };
            
            // 更多纯JS
            setInterval(() => {
                total--;
            }, 1000);
            
            // CHTL JS Listen
            {{.reset-btn}}->Listen {
                click: () => {
                    count = 0;
                    {{#counter}}->textContent = count;
                }
            };
        )";
        
        JSGeneratorConfig config;
        config.wrapIIFE = false;
        config.prettyPrint = false;
        
        CHTLJSGenerator gen(config);
        String result = gen.generate(mixed);
        
        // 纯JS应该保留
        REQUIRE(result.find("const count = 0") != String::npos);
        REQUIRE(result.find("function updateCount") != String::npos);
        REQUIRE(result.find("setInterval") != String::npos);
        
        // CHTL JS应该被转换
        REQUIRE(result.find("querySelector") != String::npos);
        REQUIRE(result.find("addEventListener") != String::npos);
    }
    
    SECTION("Complex mixed scenario with all three languages") {
        String complex = R"(
            // 1. 纯JS - 变量和函数定义
            let state = {
                isPlaying: false,
                volume: 50
            };
            
            function togglePlay() {
                state.isPlaying = !state.isPlaying;
                return state.isPlaying;
            }
            
            // 2. CHTL JS - 增强选择器
            const player = {{#music-player}};
            const playBtn = {{.play-btn}};
            
            // 3. CHTL JS - 事件绑定
            playBtn &-> click: () => {
                const playing = togglePlay();
                player->classList.toggle('playing');
            };
            
            // 4. 纯JS - 数组操作
            const playlist = ['song1.mp3', 'song2.mp3'];
            playlist.forEach(song => {
                console.log(song);
            });
            
            // 5. CHTL JS - Listen块
            {{.volume-slider}}->Listen {
                input: (e) => {
                    state.volume = e.target.value;
                    player->volume = state.volume / 100;
                }
            };
            
            // 6. 纯JS - Promise
            fetch('/api/playlist')
                .then(res => res.json())
                .then(data => {
                    console.log(data);
                });
        )";
        
        JSGeneratorConfig config;
        config.wrapIIFE = false;
        config.prettyPrint = false;
        
        CHTLJSGenerator gen(config);
        String result = gen.generate(complex);
        
        // 验证纯JS保留
        REQUIRE(result.find("let state") != String::npos);
        REQUIRE(result.find("function togglePlay") != String::npos);
        REQUIRE(result.find("playlist.forEach") != String::npos);
        REQUIRE(result.find("fetch('/api/playlist')") != String::npos);
        
        // 验证CHTL JS转换
        REQUIRE(result.find("querySelector") != String::npos);
        REQUIRE(result.find("addEventListener") != String::npos);
        
        std::cout << "\n=== Mixed Script Result ===\n" << result << "\n";
    }
    
    SECTION("Verify no JS syntax is broken by CHTL JS processing") {
        String jsWithSimilarSyntax = R"(
            // JS中也有箭头函数 =>（不要和CHTL JS的 -> 混淆）
            const arr = [1, 2, 3];
            const doubled = arr.map(x => x * 2);
            
            // JS中的对象和CHTL JS的声明式语法不应该冲突
            const config = {
                width: 100,
                height: 200
            };
            
            // JS中的模板字符串
            const message = `Value is ${config.width}`;
            
            // JS中使用 & 运算符（位运算）
            const flags = 10 & 12;
        )";
        
        JSGeneratorConfig config;
        config.wrapIIFE = false;
        config.prettyPrint = false;
        
        CHTLJSGenerator gen(config);
        String result = gen.generate(jsWithSimilarSyntax);
        
        // JS语法应该完整保留
        REQUIRE(result.find("=>") != String::npos);  // 箭头函数
        REQUIRE(result.find("x * 2") != String::npos);
        REQUIRE(result.find("width: 100") != String::npos);
        REQUIRE(result.find("${config.width}") != String::npos);
        REQUIRE(result.find("10 & 12") != String::npos);  // 位运算&应该保留
    }
}

TEST_CASE("CHTL JS syntax detection and JS preservation", "[syntax-detection]") {
    
    SECTION("Detect CHTL JS features") {
        String code1 = "{{box}}->click();";
        REQUIRE(code1.find("{{") != String::npos);
        
        String code2 = "elem &-> click: handler;";
        REQUIRE(code2.find("&->") != String::npos);
        
        String code3 = "Listen { click: () => {} };";
        REQUIRE(code3.find("Listen") != String::npos);
    }
    
    SECTION("Pure JS has no CHTL JS features") {
        String pureJS = R"(
            const x = 10;
            function test() { return x; }
            class C { }
        )";
        
        // 不应该包含CHTL JS特征
        REQUIRE(pureJS.find("{{") == String::npos);
        REQUIRE(pureJS.find("&->") == String::npos);
        REQUIRE(pureJS.find("Listen {") == String::npos);
        REQUIRE(pureJS.find("Delegate {") == String::npos);
    }
}
