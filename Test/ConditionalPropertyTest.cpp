#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include <memory>
#include <string>

// Helper function to check for a substring in a string and print context on failure
static void requireSubstring(const std::string& content, const std::string& substring) {
    if (content.find(substring) == std::string::npos) {
        FAIL("Expected substring not found: \"" << substring << "\" in content:\n" << content);
    } else {
        SUCCEED("Found expected substring: \"" << substring << "\"");
    }
}

TEST_CASE("Attribute Conditional Expressions", "[integration]") {
    const std::string entryFile = "Test/integration/conditional_property_test.chtl";

    CHTLProcessor processor(entryFile);
    std::unique_ptr<DocumentNode> doc = processor.process();

    REQUIRE(doc != nullptr);

    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    // Test 1: Simple true condition
    requireSubstring(result, "id=\"test1\"");
    requireSubstring(result, "style=\"background-color: red;\"");

    // Test 2: Simple false condition with else
    requireSubstring(result, "id=\"test2\"");
    requireSubstring(result, "style=\"background-color: blue;\"");

    // Test 3: Chained condition, first branch true
    requireSubstring(result, "id=\"test3\"");
    requireSubstring(result, "style=\"border: 1px solid green;\"");

    // Test 4: Chained condition, second branch true
    requireSubstring(result, "id=\"test4\"");
    requireSubstring(result, "style=\"border: 1px solid yellow;\"");

    // Test 5: Chained condition, else branch taken
    requireSubstring(result, "id=\"test5\"");
    requireSubstring(result, "style=\"border: 1px solid black;\"");

    // Test 6: Chained condition with no else, first branch true
    requireSubstring(result, "id=\"test6\"");
    requireSubstring(result, "style=\"background-color: purple;\"");

    // Test 7: Chained condition with no else, no branch true (should have no style attribute)
    size_t test7_pos = result.find("id=\"test7\"");
    REQUIRE(test7_pos != std::string::npos);

    // Find the start of the tag for test7
    size_t tag_start_pos = result.rfind("<div", test7_pos);
    REQUIRE(tag_start_pos != std::string::npos);

    size_t tag_end_pos = result.find(">", test7_pos);
    REQUIRE(tag_end_pos != std::string::npos);

    std::string test7_tag = result.substr(tag_start_pos, tag_end_pos - tag_start_pos);
    REQUIRE(test7_tag.find("style=") == std::string::npos);
}