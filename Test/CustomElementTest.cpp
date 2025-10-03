#include "catch.hpp"
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLNode/DocumentNode.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include <memory>
#include <string>
#include <iostream>

// Helper function to check for a substring in a string and print context on failure
static void requireSubstring(const std::string& content, const std::string& substring) {
    if (content.find(substring) == std::string::npos) {
        std::cout << "Test Failure: Expected substring not found: \"" << substring << "\"\n";
        std::cout << "Full content:\n" << content << "\n";
        FAIL("Expected substring not found: \"" << substring << "\"");
    } else {
        SUCCEED("Found expected substring: \"" << substring << "\"");
    }
}

// Helper function to check for the absence of a substring
static void requireNoSubstring(const std::string& content, const std::string& substring) {
    if (content.find(substring) != std::string::npos) {
        std::cout << "Test Failure: Unexpected substring found: \"" << substring << "\"\n";
        std::cout << "Full content:\n" << content << "\n";
        FAIL("Unexpected substring found: \"" << substring << "\"");
    } else {
        SUCCEED("Correctly did not find unexpected substring: \"" << substring << "\"");
    }
}

TEST_CASE("Custom Element Templates with full specialization", "[integration]") {
    const std::string entryFile = "Test/integration/custom_element_test.chtl";

    CHTLProcessor processor(entryFile);
    std::unique_ptr<DocumentNode> doc = processor.process();

    REQUIRE(doc != nullptr);

    HtmlGenerator generator;
    doc->accept(generator);
    std::string result = generator.getResult();

    // Test 1: Basic style specialization
    requireSubstring(result, "<div id=\"div1\" style=\"color: blue;\">");
    requireSubstring(result, "<span id=\"span1\" style=\"font-weight: bold;\">");

    // Test 2: Indexed access specialization
    requireSubstring(result, "<div id=\"div2\" style=\"border: 1px solid red;\">");

    // Test 3: Insert 'after' and 'before'
    size_t test3_start = result.find("<div id=\"test3\">");
    size_t test4_start_for_t3 = result.find("<div id=\"test4\">");
    REQUIRE(test3_start != std::string::npos);
    REQUIRE(test4_start_for_t3 != std::string::npos);
    std::string test3_content = result.substr(test3_start, test4_start_for_t3 - test3_start);
    size_t before_pos_t3 = test3_content.find("Inserted Before");
    size_t div1_pos_t3 = test3_content.find("<div id=\"div1\"");
    size_t span1_pos_t3 = test3_content.find("<span id=\"span1\"");
    size_t after_pos_t3 = test3_content.find("Inserted After");
    REQUIRE(before_pos_t3 != std::string::npos);
    REQUIRE(div1_pos_t3 != std::string::npos);
    REQUIRE(span1_pos_t3 != std::string::npos);
    REQUIRE(after_pos_t3 != std::string::npos);
    REQUIRE(before_pos_t3 < div1_pos_t3);
    REQUIRE(div1_pos_t3 < span1_pos_t3);
    REQUIRE(span1_pos_t3 < after_pos_t3);

    // Test 4: Insert 'at top' and 'at bottom'
    size_t test4_start = result.find("<div id=\"test4\">");
    size_t test5_start_for_t4 = result.find("<div id=\"test5\">");
    REQUIRE(test4_start != std::string::npos);
    REQUIRE(test5_start_for_t4 != std::string::npos);
    std::string test4_content = result.substr(test4_start, test5_start_for_t4 - test4_start);
    size_t top_pos_t4 = test4_content.find("Top");
    size_t bottom_pos_t4 = test4_content.find("Bottom");
    size_t div1_pos_t4 = test4_content.find("<div id=\"div1\"");
    REQUIRE(top_pos_t4 != std::string::npos);
    REQUIRE(bottom_pos_t4 != std::string::npos);
    REQUIRE(div1_pos_t4 != std::string::npos);
    REQUIRE(top_pos_t4 < div1_pos_t4);
    REQUIRE(div1_pos_t4 < bottom_pos_t4);

    // Test 5: Replace an element
    size_t test5_start = result.find("<div id=\"test5\">");
    size_t test6_start_for_t5 = result.find("<div id=\"test6\">");
    REQUIRE(test5_start != std::string::npos);
    REQUIRE(test6_start_for_t5 != std::string::npos);
    std::string test5_content = result.substr(test5_start, test6_start_for_t5 - test5_start);
    requireSubstring(test5_content, "Replaced");
    requireNoSubstring(test5_content, "<p id=\"p1\">");

    // Test 6: Delete elements
    size_t test6_start = result.find("<div id=\"test6\">");
    size_t body_end = result.rfind("</body>");
    REQUIRE(test6_start != std::string::npos);
    REQUIRE(body_end != std::string::npos);
    std::string test6_content = result.substr(test6_start, body_end - test6_start);
    requireNoSubstring(test6_content, "span");
    requireNoSubstring(test6_content, "div2");
    requireSubstring(test6_content, "<div id=\"div1\">");
}