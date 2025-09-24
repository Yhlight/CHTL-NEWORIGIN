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

void test_namespace_template_access();

void test_unquoted_literals() {
    std::string source = R"(
        div {
            class: card red_text;
            text { This is unquoted }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("class=\"card red_text\"") != std::string::npos);
    assert(result.find("This is unquoted") != std::string::npos);
}

void test_calc_generation() {
    std::string source = R"(
        div { style { width: 100% - 20px; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("width: calc(100% - 20px);") != std::string::npos);
}

void test_except_constraint() {
    std::string source = R"(
        div {
            except span;
            span { text: "forbidden"; }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    bool exception_thrown = false;
    try {
        parser.parse();
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        std::string msg = e.what();
        assert(msg.find("is not allowed") != std::string::npos);
    }
    assert(exception_thrown);
}

void test_named_origin_and_import() {
    std::ofstream("imported_script.js") << "console.log('imported');";
    std::string source = R"(
        [Import] @JavaScript from "imported_script.js" as my_script;
        [Origin] @Html my_html { <div>Hello</div> }

        body {
            [Origin] @Html my_html;
            script {
                [Origin] @JavaScript my_script;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("<div>Hello</div>") != std::string::npos);
    assert(result.find("console.log('imported');") != std::string::npos);
    remove("imported_script.js");
}

void test_delete_style_inheritance() {
    std::string source = R"(
        [Template] @Style Parent { color: red; font_size: 16px; }
        [Custom] @Style Child { inherit @Style Parent; font_weight: bold; }
        div {
            style {
                @Style Child {
                    delete @Style Parent;
                    font_size: 20px;
                }
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("color: red;") == std::string::npos); // Parent style should be deleted
    assert(result.find("font_weight: bold;") != std::string::npos); // Child style should remain
    assert(result.find("font_size: 20px;") != std::string::npos); // Specialized style should remain
}

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

void test_custom_element_insert_at_top_bottom() {
    std::string source = R"(
        [Custom] @Element Box {
            div { text: "middle"; }
        }
        body {
            @Element Box {
                insert at top {
                    header { text: "top"; }
                }
                insert at bottom {
                    footer { text: "bottom"; }
                }
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);
    assert(result.find("<header>") != std::string::npos);
    assert(result.find("<footer>") != std::string::npos);
    assert(result.find("<header>") < result.find("<div>"));
    assert(result.find("<div>") < result.find("<footer>"));
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
    run_test(test_custom_style_specialization, "Custom Style Specialization"); // Disabled due to unsolved bug
    run_test(test_custom_element_delete, "Custom Element Deletion");
    // The test below is also disabled as it fails with the same bug as custom style specialization.
    run_test(test_custom_element_insert, "Custom Element Insertion");
    run_test(test_custom_element_insert_at_top_bottom, "Custom Element Insertion At Top/Bottom");
    run_test(test_import, "Import Statement");
    run_test(test_unquoted_literals, "Unquoted Literals");
    run_test(test_calc_generation, "Calc() Generation");
    run_test(test_except_constraint, "Except Constraint");
    run_test(test_named_origin_and_import, "Named Origin and Import");
    run_test(test_delete_style_inheritance, "Delete Style Inheritance");

    run_test(test_namespace_template_access, "Namespace-qualified Template Access");

    std::cout << "Tests finished." << std::endl;
    return 0;
}

void test_namespace_template_access() {
    std::string source = R"(
        [Namespace] my_space {
            [Template] @Element MyBox {
                div { class: "box-in-space"; }
            }

            [Template] @Style MyStyle {
                border: 1px solid blue;
            }

            [Template] @Var MyVars {
                brandColor: "blue";
            }

            [Namespace] nested {
                 [Template] @Element NestedBox {
                    span { class: "nested-box"; }
                 }
            }
        }

        body {
            @Element MyBox from my_space;

            p {
                style {
                    @Style MyStyle from my_space;
                    color: MyVars(brandColor) from my_space;
                }
            }

            @Element NestedBox from my_space.nested;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, false);

    // Assertions
    assert(result.find("class=\"box-in-space\"") != std::string::npos);
    assert(result.find("border: 1px solid blue;") != std::string::npos);
    assert(result.find("color: blue;") != std::string::npos);
    assert(result.find("class=\"nested-box\"") != std::string::npos);
}
