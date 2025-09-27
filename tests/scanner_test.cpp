#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <algorithm> // Required for std::remove_if
#include "Scanner/UnifiedScanner.h"

// Helper function to remove all whitespace from a string
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

void run_scanner_test(void (*test_func)(), const std::string& test_name) {
    try {
        test_func();
        std::cout << "[PASS] " << test_name << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "[FAIL] " << test_name << " - " << e.what() << std::endl;
    } catch (...) {
        std::cout << "[FAIL] " << test_name << " - Unknown exception" << std::endl;
    }
}

// --- Test Cases ---
void test_simple_js_separation();
void test_heuristic_failure_case();

void test_simple_js_separation() {
    UnifiedScanner scanner;
    std::string source = R"(
        script {
            const a = 1;
            const b = {{c}};
            console.log(a + b);
        }
    )";
    ScannedOutput output = scanner.scan(source);

    // The scanner should identify one script block and process it.
    assert(output.fragments.size() == 1);
    const auto& fragment = output.fragments.begin()->second;

    // It should correctly identify this as CHTL_JS because of the {{c}}
    assert(fragment.type == FragmentType::CHTL_JS);

    // The CHTL content should now contain a placeholder element.
    // The current scanner implementation replaces the entire `script {...}` block.
    assert(output.chtl_with_placeholders.find("chtl_placeholder_0{}") != std::string::npos);

    // The keyword `script` should NOT be in the placeholder output, based on current scanner logic.
    assert(output.chtl_with_placeholders.find("script") == std::string::npos);
}

void test_heuristic_failure_case() {
    UnifiedScanner scanner;
    std::string source = R"(
        script {
            // Do not Listen to this comment.
            const my_Animate_variable = "hello";
            console.log(my_Animate_variable);
        }
    )";
    ScannedOutput output = scanner.scan(source);
    assert(output.fragments.size() == 1);
    const auto& fragment = output.fragments.begin()->second;

    // The current heuristic will fail this, but the correct type is JS.
    assert(fragment.type == FragmentType::JS);
}

int main() {
    std::cout << "Running UnifiedScanner tests..." << std::endl;
    run_scanner_test(test_simple_js_separation, "Simple JS/CHTL_JS Separation");
    run_scanner_test(test_heuristic_failure_case, "Heuristic Failure Case");
    std::cout << "\nScanner tests finished." << std::endl;
    return 0;
}