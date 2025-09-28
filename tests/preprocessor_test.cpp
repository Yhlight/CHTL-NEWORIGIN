#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../src/Core/CHTLJSPreprocessor.h"
#include "../src/Util/StringUtils.h" // For findMatchingBrace, which is used by the preprocessor
#include <string>
#include <algorithm>

// Helper to remove whitespace for easier comparison
std::string remove_whitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}

TEST_CASE("CHTLJSPreprocessor tests", "[preprocessor]") {
    CHTLJSPreprocessor preprocessor;

    SECTION("Vir declaration is removed and access is substituted") {
        std::string source = R"RAW(
            Vir myEvents = Listen {
                click: () => { console.log("Clicked!"); }
            };

            const myButton = document.querySelector('#btn');
            myButton.addEventListener('click', myEvents->click);
        )RAW";

        preprocessor.process(source);

        std::string expected = R"JS(
            const myButton = document.querySelector('#btn');
            myButton.addEventListener('click', () => { console.log("Clicked!"); });
        )JS";

        REQUIRE(remove_whitespace(source) == remove_whitespace(expected));
    }

    SECTION("Multiple Vir objects and accesses") {
        std::string source = R"RAW(
            Vir events = Listen {
                click: clickHandler,
                mouseenter: mouseEnterHandler
            };
            Vir config = {
                duration: 500
            };

            Animate {
                target: {{.box}},
                duration: config->duration
            };

            {{.box}}->Listen {
                click: events->click
            };
        )RAW";

        preprocessor.process(source);

        std::string expected = R"JS(
            Animate {
                target: {{.box}},
                duration: 500
            };

            {{.box}}->Listen {
                click: clickHandler
            };
        )JS";

        REQUIRE(remove_whitespace(source) == remove_whitespace(expected));
    }

    SECTION("No Vir objects") {
        std::string source = "const x = 1; console.log(x);";
        std::string original_source = source;
        preprocessor.process(source);
        REQUIRE(source == original_source);
    }
}