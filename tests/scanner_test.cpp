#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "../src/Scanner/UnifiedScanner.h"
#include <string>
#include <map>
#include <sstream>
#include <iomanip>

// Helper function to convert a string to its hex representation for debugging
std::string to_hex(const std::string& s) {
    std::ostringstream ret;
    for (std::string::size_type i = 0; i < s.length(); ++i) {
        ret << std::hex << std::setfill('0') << std::setw(2) << (int)(unsigned char)s[i] << " ";
    }
    return ret.str();
}


TEST_CASE("UnifiedScanner tests", "[scanner]") {
    UnifiedScanner scanner;

    SECTION("Script with only standard JavaScript") {
        std::string source = "function hello() { console.log('world'); } var x = 1;";
        std::string result = scanner.scan(source);
        const auto& placeholders = scanner.getPlaceholders();

        REQUIRE(placeholders.size() == 1);
        REQUIRE(result.find("_JS_CODE_PLACEHOLDER_") == 0);
        REQUIRE(placeholders.at(result) == source);
    }

    SECTION("Script with only CHTL JS") {
        std::string source = "{{.box}}->Listen{click:()=>{}}";
        std::string result = scanner.scan(source);
        const auto& placeholders = scanner.getPlaceholders();

        REQUIRE(placeholders.empty());
        REQUIRE(result == source);
    }

    SECTION("Script with a mix of JS and CHTL JS") {
        std::string source = "console.log('start'); {{.btn}}->Listen{click:()=>{}}; console.log('end');";
        std::string result = scanner.scan(source);
        const auto& placeholders = scanner.getPlaceholders();

        REQUIRE(placeholders.size() == 2);
        REQUIRE(result.find("{{.btn}}->Listen{click:()=>{}};") != std::string::npos);

        REQUIRE(placeholders.count("_JS_CODE_PLACEHOLDER_0_"));
        REQUIRE(placeholders.at("_JS_CODE_PLACEHOLDER_0_") == "console.log('start'); ");
        REQUIRE(placeholders.count("_JS_CODE_PLACEHOLDER_1_"));
        REQUIRE(placeholders.at("_JS_CODE_PLACEHOLDER_1_") == " console.log('end');");
    }

    SECTION("Script with multiple CHTL JS features separated by a space") {
        std::string source = R"RAW(var x = 1; {{.box}}->Listen{click:()=>{}}; const a = Animate{target:{{.other}}};)RAW";
        // The semicolon is correctly placed *after* the closing brace of the Listen block.
        std::string expected_result = R"RAW(_JS_CODE_PLACEHOLDER_0_{{.box}}->Listen{click:()=>{}};_JS_CODE_PLACEHOLDER_1_Animate{target:{{.other}}};)RAW";

        std::string result = scanner.scan(source);
        const auto& placeholders = scanner.getPlaceholders();

        REQUIRE(placeholders.size() == 2);

        INFO("Result hex:   " << to_hex(result));
        INFO("Expected hex: " << to_hex(expected_result));
        REQUIRE(result == expected_result);

        REQUIRE(placeholders.at("_JS_CODE_PLACEHOLDER_0_") == "var x = 1; ");
        REQUIRE(placeholders.at("_JS_CODE_PLACEHOLDER_1_") == " const a = ");
    }

    SECTION("Empty script") {
        std::string source = "";
        std::string result = scanner.scan(source);
        const auto& placeholders = scanner.getPlaceholders();

        REQUIRE(placeholders.empty());
        REQUIRE(result.empty());
    }

    SECTION("Script with only whitespace") {
        std::string source = "  \t\n  ";
        std::string result = scanner.scan(source);
        const auto& placeholders = scanner.getPlaceholders();

        REQUIRE(placeholders.empty());
        REQUIRE(result == source);
    }

    SECTION("CHTL JS keywords in comments or strings should be ignored") {
        std::string source = R"RAW(// A keyword Listen here
let myListener = 'Listen';)RAW";
        std::string result = scanner.scan(source);
        const auto& placeholders = scanner.getPlaceholders();

        REQUIRE(placeholders.size() == 1);
        REQUIRE(result.find("_JS_CODE_PLACEHOLDER_") == 0);
        REQUIRE(placeholders.at(result) == source);
    }
}