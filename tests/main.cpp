#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream> // For std::ofstream
#include <cstdio>  // For remove()

// Include all necessary CHTL headers
#include "../src/CHTL/CHTLLexer/Lexer.h"
#include "../src/CHTL/CHTLParser/Parser.h"
#include "../src/CHTL/CHTLGenerator/Generator.h"

// A simple testing framework
void run_test(void (*test_func)(), const std::string& test_name) {
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

void test_referenced_property() {
    std::string source = R"(
        div { id: "box1"; style { width: 100px; } }
        div { id: "box2"; style { height: #box1.width; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    std::string expected_substr = "height: 100px;";
    assert(result.find(expected_substr) != std::string::npos);
}

void test_conditional_expression_true() {
    std::string source = R"(
        div { style { width: 200px; color: width > 100px ? "red" : "blue"; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    std::string expected_substr = "color: red;";
    assert(result.find(expected_substr) != std::string::npos);
}

void test_conditional_expression_false() {
    std::string source = R"(
        div { style { width: 50px; color: width > 100px ? "red" : "blue"; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    std::string expected_substr = "color: blue;";
    assert(result.find(expected_substr) != std::string::npos);
}

void test_custom_style_specialization() {
    std::string source = R"(
        [Custom] @Style Base { color; border: 1px solid black; }
        div { style { @Style Base { color: green; delete border; } } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("color: green;") != std::string::npos);
    assert(result.find("border:") == std::string::npos);
}

void test_custom_element_delete() {
    std::string source = R"(
        [Custom] @Element Box {
            span { text: "hello"; }
            div { text: "world"; }
        }
        body {
            @Element Box {
                delete span;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("<span>") == std::string::npos);
    assert(result.find("<div>") != std::string::npos);
}

void test_custom_element_insert() {
    std::string source = R"(
        [Custom] @Element Box {
            div { text: "world"; }
        }
        body {
            @Element Box {
                insert before div[0] {
                    span { text: "hello"; }
                }
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("<span>") < result.find("<div>"));
}

void test_import() {
    // Create a temporary file to import
    std::ofstream("imported_file.chtl") << "[Template] @Element MyElem { p { text: \"imported\"; } }";

    std::string source = R"(
        [Import] @Chtl from "imported_file.chtl";
        body {
            @Element MyElem;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("<p>") != std::string::npos);
    assert(result.find("imported") != std::string::npos);
    remove("imported_file.chtl");
}


int main() {
    std::cout << "Running CHTL tests..." << std::endl;

    run_test(test_referenced_property, "Referenced Property");
    run_test(test_conditional_expression_true, "Conditional Expression (True)");
    run_test(test_conditional_expression_false, "Conditional Expression (False)");
    run_test(test_custom_style_specialization, "Custom Style Specialization");
    run_test(test_custom_element_delete, "Custom Element Deletion");
    run_test(test_custom_element_insert, "Custom Element Insertion");
    run_test(test_import, "Import Statement");

    std::cout << "Tests finished." << std::endl;
    return 0;
}
