#include "catch.hpp"
#include "../src/Scanner/UnifiedScanner.h"

TEST_CASE("UnifiedScanner basic test", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = "div { style { color: red; } }";
    ScannedOutput output = scanner.scan(source);

    // The new scanner is smarter. For a style block containing only standard CSS,
    // it should not extract any fragments. It leaves the block as-is for the
    // CHTL parser to treat as a literal.
    REQUIRE(output.fragments.size() == 0);
    REQUIRE(output.chtl_with_placeholders == source);
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

TEST_CASE("UnifiedScanner with mixed CHTL and CSS in style block", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = R"(
        style {
            .class-one {
                color: red;
                width: 100px + 20px; // CHTL syntax
            }
            .class-two {
                @Style MyTemplate; // CHTL syntax
                border: 1px solid black;
            }
        }
    )";

    ScannedOutput output = scanner.scan(source);

    // The scanner should find two CHTL fragments inside the style block.
    REQUIRE(output.fragments.size() == 2);

    // Check that the CHTL source has placeholders.
    // Note: The exact placeholder names don't matter, but their presence does.
    REQUIRE(output.chtl_with_placeholders.find("CHTL_PLACEHOLDER_") != std::string::npos);
    REQUIRE(output.chtl_with_placeholders.find("color: red;") != std::string::npos); // Standard CSS should remain.
    REQUIRE(output.chtl_with_placeholders.find("border: 1px solid black;") != std::string::npos); // Standard CSS should remain.


    // Check that the fragments were extracted correctly.
    bool found_arithmetic = false;
    bool found_template = false;
    for (const auto& pair : output.fragments) {
        if (pair.second.content == "100px + 20px") {
            REQUIRE(pair.second.type == FragmentType::CHTL);
            found_arithmetic = true;
        }
        if (pair.second.content == "@Style MyTemplate;") {
            REQUIRE(pair.second.type == FragmentType::CHTL);
            found_template = true;
        }
    }
    REQUIRE(found_arithmetic);
    REQUIRE(found_template);
}