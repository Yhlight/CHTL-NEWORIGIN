#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../src/Scanner/UnifiedScanner.h"

TEST_CASE("UnifiedScanner basic test", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = "div { style { color: red; } }";
    ScannedOutput output = scanner.scan(source);
    REQUIRE(output.fragments.size() == 1);
    REQUIRE(output.fragments.begin()->second.type == FragmentType::CSS);
    REQUIRE(output.fragments.begin()->second.content == " color: red; ");
}

TEST_CASE("UnifiedScanner with nested JS", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = "script { let a = { b: 1 }; }";
    ScannedOutput output = scanner.scan(source);
    REQUIRE(output.fragments.size() == 1);
    REQUIRE(output.fragments.begin()->second.type == FragmentType::JS);
    REQUIRE(output.fragments.begin()->second.content == " let a = { b: 1 }; ");
}

TEST_CASE("UnifiedScanner with CHTL JS", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = "script { {{box}}->click(); }";
    ScannedOutput output = scanner.scan(source);
    REQUIRE(output.fragments.size() == 1);
    REQUIRE(output.fragments.begin()->second.type == FragmentType::CHTL_JS);
    REQUIRE(output.fragments.begin()->second.content == " {{box}}->click(); ");
}

TEST_CASE("UnifiedScanner failing case with nested arrow function", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = "Listen {click: ()=>{ {{box}}->()=>{} }}";
    ScannedOutput output = scanner.scan(source);
    REQUIRE(output.fragments.size() == 1);
    REQUIRE(output.fragments.begin()->second.type == FragmentType::CHTL_JS);
    // The fragment should contain the entire Listen block for the CHTL JS parser.
    REQUIRE(output.fragments.begin()->second.content == "Listen {click: ()=>{ {{box}}->()=>{} }}");
}