#include "../catch.hpp"
#include "../src/CHTL_JS/CHTLJSCompiler/CHTLJSCompiler.h"
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to remove all whitespace from a string for robust comparison.
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); }), str.end());
    return str;
}

TEST_CASE("CHTLJSCompiler correctly processes mixed JS and CHTL JS", "[CHTLJSCompiler]") {
    CHTLJSCompiler compiler;

    SECTION("Script with mixed JS and CHTL JS content") {
        std::string source = R"(
            let x = 10;
            {{my_element}}->Listen {
                click: () => { console.log("Clicked!"); }
            }
            if (x > 5) {
                console.log("x is greater than 5");
            }
        )";

        auto ast = compiler.parse(source);
        std::string generated_js = compiler.generate(ast);
        std::string processed_js = remove_whitespace(generated_js);

        // Check that the pure JS parts were preserved and the CHTL JS was compiled.
        REQUIRE(processed_js.find("letx=10;") != std::string::npos);
        REQUIRE(processed_js.find("document.querySelector('my_element').addEventListener('click',()=>{console.log(\"Clicked!\");});") != std::string::npos);
        REQUIRE(processed_js.find("if(x>5){console.log(\"xisgreaterthan5\");}") != std::string::npos);
    }

    SECTION("Script with only pure JS") {
        std::string source = R"(
            function greet(name) {
                return "Hello, " + name;
            }
            greet("World");
        )";

        auto ast = compiler.parse(source);
        std::string generated_js = compiler.generate(ast);
        std::string processed_js = remove_whitespace(generated_js);

        // The generated JS should be identical to the source, as there's no CHTL JS to compile.
        REQUIRE(processed_js.find("functiongreet(name){return\"Hello,\"+name;}") != std::string::npos);
        REQUIRE(processed_js.find("greet(\"World\");") != std::string::npos);
    }

    SECTION("Script with only CHTL JS and an empty block") {
        std::string source = "{{a}}->Listen{}";

        auto ast = compiler.parse(source);
        std::string generated_js = compiler.generate(ast);

        // The generated JS should be empty because an empty Listen block does nothing.
        REQUIRE(generated_js.empty());
    }
}