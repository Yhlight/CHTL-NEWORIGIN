#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include <memory>
#include <string>

// Helper function to check for a substring in a string and print context on failure
void requireSubstring(const std::string& content, const std::string& substring) {
    if (content.find(substring) == std::string::npos) {
        FAIL("Expected substring not found: \"" << substring << "\" in content:\n" << content);
    } else {
        SUCCEED("Found expected substring: \"" << substring << "\"");
    }
}

TEST_CASE("Custom Style Templates with specialization", "[integration]") {
    const std::string entryFile = "Test/integration/custom_style_test.chtl";

    CHTLProcessor processor(entryFile);
    std::unique_ptr<DocumentNode> doc = processor.process();

    REQUIRE(doc != nullptr);

    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    // Test 1: Check if valueless property got its value
    std::string expected_style1 = "id=\"button1\" style=\"border: 1px solid black; padding: 10px; background-color: blue;\"";
    requireSubstring(result, expected_style1);

    // Test 2: Check if 'border' property was deleted
    std::string expected_style2 = "id=\"button2\" style=\"padding: 10px; background-color: red;\"";
    requireSubstring(result, expected_style2);

    // Test 3: Check if 'padding' property was overridden
    std::string expected_style3 = "id=\"button3\" style=\"border: 1px solid black; padding: 20px; background-color: green;\"";
    requireSubstring(result, expected_style3);
}