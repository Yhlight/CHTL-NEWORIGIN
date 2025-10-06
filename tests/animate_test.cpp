#include "../third-party/catch.hpp"
#include "CHTL-JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL-JS/CHTLJSGenerator/CHTLJSGenerator.h"

using namespace CHTL;
using namespace CHTL::JS;

TEST_CASE("Animate - Basic Structure", "[animate]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Simple animation") {
        String code = R"(
            const anim = Animate {
                target: {{box}},
                duration: 1000,
                begin: { opacity: 0 },
                end: { opacity: 1 }
            };
        )";
        String output = gen.generate(code);
        
        // 应该包含requestAnimationFrame
        REQUIRE(output.find("requestAnimationFrame") != String::npos);
        REQUIRE(output.find("opacity") != String::npos);
    }
    
    SECTION("Animation with easing") {
        String code = R"(
            Animate {
                target: {{.box}},
                duration: 500,
                easing: ease-in-out,
                begin: { transform: 'scale(0)' },
                end: { transform: 'scale(1)' }
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("requestAnimationFrame") != String::npos);
        REQUIRE(output.find("transform") != String::npos);
    }
}

TEST_CASE("Animate - Keyframes", "[animate]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Animation with keyframes") {
        String code = R"(
            Animate {
                target: {{box}},
                duration: 1000,
                begin: { opacity: 0 },
                when: [
                    { at: 0.5, opacity: 0.5, transform: 'scale(1.2)' }
                ],
                end: { opacity: 1 }
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("requestAnimationFrame") != String::npos);
        REQUIRE(output.find("0.5") != String::npos);
    }
    
    SECTION("Multiple keyframes") {
        String code = R"(
            Animate {
                target: {{.box}},
                duration: 2000,
                when: [
                    { at: 0.25, opacity: 0.25 },
                    { at: 0.5, opacity: 0.5 },
                    { at: 0.75, opacity: 0.75 }
                ]
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("0.25") != String::npos);
        REQUIRE(output.find("0.5") != String::npos);
        REQUIRE(output.find("0.75") != String::npos);
    }
}

TEST_CASE("Animate - Advanced Features", "[animate]") {
    JSGeneratorConfig config;
    config.wrapIIFE = false;
    CHTLJSGenerator gen(config);
    
    SECTION("Animation with loop") {
        String code = R"(
            Animate {
                target: {{box}},
                duration: 1000,
                begin: { opacity: 0 },
                end: { opacity: 1 },
                loop: -1
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("requestAnimationFrame") != String::npos);
    }
    
    SECTION("Animation with callback") {
        String code = R"(
            Animate {
                target: {{box}},
                duration: 1000,
                begin: { opacity: 0 },
                end: { opacity: 1 },
                callback: () => { console.log('done'); }
            };
        )";
        String output = gen.generate(code);
        
        REQUIRE(output.find("requestAnimationFrame") != String::npos);
        REQUIRE(output.find("console.log") != String::npos);
    }
}
