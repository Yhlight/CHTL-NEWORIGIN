#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <algorithm>

// Helper function to remove all whitespace from a string
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

// A simple testing framework
void run_test(void (*test_func)(), const std::string& test_name) {
    try {
        test_func();
    } catch (const std::runtime_error& e) {
        std::cout << "[FAIL] " << test_name << " - " << e.what() << std::endl;
    } catch (...) {
        std::cout << "[FAIL] " << test_name << " - Unknown exception" << std::endl;
    }
}

// --- Test Cases ---
void test_basic_element_and_text_attribute();
void test_element_with_id_attribute();
void test_nested_elements();
void test_single_line_comment_is_ignored();
void test_local_inline_style_block();
void test_style_template_definition_and_usage();
void test_element_template_definition_and_usage();
void test_chtl_js_listen_block();
void test_event_delegation();
void test_placeholder_based_parsing_and_generation();


int main() {
    std::cout << "Running CHTL tests..." << std::endl;

    // --- Running Core Feature Tests ---
    std::cout << "\n--- Running Core Feature Tests ---\n";
    run_test(test_basic_element_and_text_attribute, "Basic Element and Text Attribute");
    run_test(test_element_with_id_attribute, "Element with ID Attribute");
    run_test(test_nested_elements, "Nested Elements");
    run_test(test_single_line_comment_is_ignored, "Single-Line Comment Is Ignored");
    run_test(test_local_inline_style_block, "Local Inline Style Block");
    run_test(test_style_template_definition_and_usage, "Style Template Definition and Usage");
    run_test(test_element_template_definition_and_usage, "Element Template Definition and Usage");

    // --- Running CHTL JS Feature Tests ---
    std::cout << "\n--- Running CHTL JS Feature Tests ---\n";
    run_test(test_chtl_js_listen_block, "CHTL JS Listen Block");
    run_test(test_event_delegation, "Event Delegation");

    // --- Running Placeholder Verification Test ---
    std::cout << "\n--- Running Placeholder Verification Test ---\n";
    run_test(test_placeholder_based_parsing_and_generation, "Placeholder-Based Parsing and Generation");

    std::cout << "\nTests finished." << std::endl;
    return 0;
}