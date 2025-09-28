#include "../catch.hpp"
#include "../src/Scanner/UnifiedScanner.h"
#include "../src/Core/Placeholder.h"

TEST_CASE("UnifiedScanner granularly processes script blocks", "[UnifiedScanner]") {
    UnifiedScanner scanner;

    SECTION("Script with mixed JS and CHTL JS content") {
        std::string source = R"(
            script {
                let x = 10;
                {{my_element}}->Listen {
                    click: () => { console.log("Clicked!"); }
                }
                if (x > 5) {
                    console.log("x is greater than 5");
                }
            }
        )";

        ScannedOutput output = scanner.scan(source);

        // Check the structure of the output with placeholders
        REQUIRE(output.chtl_with_placeholders.find("_JS_CODE_PLACEHOLDER_0_") != std::string::npos);
        REQUIRE(output.chtl_with_placeholders.find("{{my_element}}") != std::string::npos);
        REQUIRE(output.chtl_with_placeholders.find("->") != std::string::npos);
        REQUIRE(output.chtl_with_placeholders.find("Listen") != std::string::npos);
        REQUIRE(output.chtl_with_placeholders.find("_JS_CODE_PLACEHOLDER_1_") != std::string::npos);
        REQUIRE(output.chtl_with_placeholders.find("_JS_CODE_PLACEHOLDER_2_") != std::string::npos);

        // Check the content of the placeholders
        REQUIRE(output.placeholder_manager.getContent("_JS_CODE_PLACEHOLDER_0_") == "\n                let x = 10;\n                ");
        REQUIRE(output.placeholder_manager.getContent("_JS_CODE_PLACEHOLDER_1_") == " {\n                    click: () => { console.log(\"Clicked!\"); }\n                }\n                ");
        REQUIRE(output.placeholder_manager.getContent("_JS_CODE_PLACEHOLDER_2_") == "\n                if (x > 5) {\n                    console.log(\"x is greater than 5\");\n                }\n            \n        ");
    }

    SECTION("Script with only pure JS") {
        std::string source = R"(
            script {
                function greet(name) {
                    return "Hello, " + name;
                }
                greet("World");
            }
        )";

        ScannedOutput output = scanner.scan(source);

        REQUIRE(output.chtl_with_placeholders.find("_JS_CODE_PLACEHOLDER_0_") != std::string::npos);
        REQUIRE(output.placeholder_manager.placeholder_map.size() == 1);
        REQUIRE(output.placeholder_manager.getContent("_JS_CODE_PLACEHOLDER_0_").find("function greet(name)") != std::string::npos);
    }

    SECTION("Script with only CHTL JS") {
        std::string source = R"(
            script {
                {{a}}->Listen{}
            }
        )";

        ScannedOutput output = scanner.scan(source);

        // No JS placeholders should be generated
        REQUIRE(output.chtl_with_placeholders.find("_JS_CODE_PLACEHOLDER_") == std::string::npos);
        REQUIRE(output.chtl_with_placeholders.find("{{a}}->Listen{}") != std::string::npos);
        REQUIRE(output.placeholder_manager.placeholder_map.empty());
    }
}