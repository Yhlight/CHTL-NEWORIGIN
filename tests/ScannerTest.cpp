#include "../catch.hpp"
#include "../src/Scanner/UnifiedScanner.h"
#include <regex>
#include <string>

// Helper to count occurrences of a substring
int count_substring(const std::string& str, const std::string& sub) {
    int count = 0;
    size_t pos = 0;
    while ((pos = str.find(sub, pos)) != std::string::npos) {
        count++;
        pos += sub.length();
    }
    return count;
}

TEST_CASE("UnifiedScanner: Full Placeholder Mechanism for JS", "[UnifiedScanner][Recursive]") {
    UnifiedScanner scanner;
    std::string source = R"(
        script {
            // This is standard JS that should be extracted
            const prefix = "hello";

            // This is a CHTL JS construct that should remain
            Listen {
                click: function(e) { // This standard JS function block should be extracted
                    const target = {{e.target}}; // This CHTL JS should remain inside the JS fragment
                    console.log(prefix, target);
                }
            }

            // This is more standard JS that should be extracted
            let suffix = "world";
        }
    )";
    ScannedOutput output = scanner.scan(source);

    // The scanner should find two JS fragments: one before Listen, and one after.
    // The 'function' block inside Listen is part of the JS fragment that contains the CHTL JS.
    REQUIRE(output.fragments.size() >= 2);

    // The main placeholder string should only contain the CHTL JS `Listen` block, surrounded by JS placeholders.
    REQUIRE(output.chtl_with_placeholders.find("Listen {") != std::string::npos);
    REQUIRE(output.chtl_with_placeholders.find("const prefix") == std::string::npos);
    REQUIRE(output.chtl_with_placeholders.find("let suffix") == std::string::npos);
    REQUIRE(count_substring(output.chtl_with_placeholders, "__JS_PLACEHOLDER_") >= 2);

    // Verify that the JS fragments were extracted correctly.
    bool found_prefix_js = false;
    bool found_suffix_js = false;
    for(const auto& pair : output.fragments) {
        if (pair.second.type == FragmentType::JS) {
            if (pair.second.content.find("const prefix") != std::string::npos) {
                found_prefix_js = true;
            }
            if (pair.second.content.find("let suffix") != std::string::npos) {
                found_suffix_js = true;
            }
        }
    }
    REQUIRE(found_prefix_js);
    REQUIRE(found_suffix_js);
}

TEST_CASE("UnifiedScanner: Full Placeholder Mechanism for CSS", "[UnifiedScanner][Recursive]") {
    UnifiedScanner scanner;
    std::string source = R"(
        style {
            // This is a standard CSS rule that should be extracted
            .my-class {
                font-size: 16px;
            }

            // This is a CHTL feature that should remain
            color: 10px + 20px;

            // This is another standard CSS rule to be extracted
            #my-id:hover {
                text-decoration: underline;
            }
        }
    )";
    ScannedOutput output = scanner.scan(source);

    // The scanner should extract the two standard CSS rules as fragments.
    REQUIRE(output.fragments.size() == 2);

    // The main placeholder string should only contain the CHTL part, surrounded by CSS placeholders.
    std::string processed_style_block = std::regex_replace(output.chtl_with_placeholders, std::regex(R"(\s+)"), " ");
    REQUIRE(processed_style_block.find("style { /*__CSS_PLACEHOLDER_0__*/ color: 10px + 20px; /*__CSS_PLACEHOLDER_1__*/ }") != std::string::npos);

    // Check that the standard CSS rules are not in the main string.
    REQUIRE(processed_style_block.find(".my-class") == std::string::npos);
    REQUIRE(processed_style_block.find("#my-id:hover") == std::string::npos);

    // Check that the fragments contain the standard CSS
    bool found_class_rule = false;
    bool found_id_rule = false;
    for(const auto& pair : output.fragments) {
        if (pair.second.type == FragmentType::CSS) {
            if (pair.second.content.find(".my-class") != std::string::npos) {
                found_class_rule = true;
            }
            if (pair.second.content.find("#my-id:hover") != std::string::npos) {
                found_id_rule = true;
            }
        }
    }
    REQUIRE(found_class_rule);
    REQUIRE(found_id_rule);
}

TEST_CASE("UnifiedScanner: Basic CSS-only style block", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = "style { .foo { color: blue; } }";
    ScannedOutput output = scanner.scan(source);

    // A block with only standard CSS should result in one CSS fragment.
    REQUIRE(output.fragments.size() == 1);
    REQUIRE(output.fragments.begin()->second.type == FragmentType::CSS);
    REQUIRE(output.fragments.begin()->second.content.find(".foo { color: blue; }") != std::string::npos);
    // The placeholder should be inside the style block
    REQUIRE(output.chtl_with_placeholders.find("style { /*__CSS_PLACEHOLDER_0__*/ }") != std::string::npos);
}

TEST_CASE("UnifiedScanner: Basic CHTL-JS-only script block", "[UnifiedScanner]") {
    UnifiedScanner scanner;
    std::string source = "script { {{box}}->click(); }";
    ScannedOutput output = scanner.scan(source);
    // Pure CHTL-JS doesn't get fragmented, it's the native language of the block.
    REQUIRE(output.fragments.empty());
    REQUIRE(output.chtl_with_placeholders.find("{{box}}->click();") != std::string::npos);
}