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
void test_keyword_aliasing();
void test_hyphenated_property();
void test_attribute_expression();
void test_conditional_attribute();
void test_delete_style_property();
void test_var_template_usage();
void test_var_template_specialization();
void test_style_auto_add_class();
void test_except_clause_extended();
void test_ampersand_selector_order();
void test_delete_element_inheritance();
void test_calc_with_percentage();
void test_implicit_style_template_inheritance();
void test_unquoted_literal_support();
void test_text_block_literals();
void test_enhanced_selector();
void test_responsive_value();


void test_text_block_literals() {
    std::string source = R"(
        div {
            text { This is unquoted }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("This is unquoted") != std::string::npos);
}

void test_unquoted_literal_support() {
    std::string source = R"(
        div {
            text: This is an unquoted attribute value;
            style {
                font-family: Times New Roman;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);

    assert(result.find("This is an unquoted attribute value") != std::string::npos);
    assert(result.find("font-family: Times New Roman;") != std::string::npos);
}

void test_enhanced_selector() {
    std::string source = R"(
        div {
            id: "my-div";
            script {
                const myDiv = {{#my-div}};
                myDiv.innerText = "Hello CHTL JS";
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    // The generator will now preserve whitespace from the source.
    std::string expected_script = "const myDiv = document.querySelector('#my-div');\n                myDiv.innerText = \"Hello CHTL JS\";";
    assert(result.find(expected_script) != std::string::npos);
}

void test_responsive_value() {
    std::string source = R"(
        div {
            class: $myClass$;
            style {
                width: $myWidth$px;
            }
            script {
                myClass = "responsive-box";
                myWidth = 250;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);

    // Check for the generated element with its unique ID
    assert(result.find("<div id=\"chtl-id-0\"") != std::string::npos);
    // Check for the runtime script setup
    assert(result.find("const __chtl = {") != std::string::npos);
    // Check for variable registration in the runtime script
    assert(result.find("__chtl.registerBinding('myClass', 'chtl-id-0', 'class', '');") != std::string::npos);
    assert(result.find("__chtl.registerBinding('myWidth', 'chtl-id-0', 'style.width', 'px');") != std::string::npos);
    // Check for the proxy creation
    assert(result.find("Object.defineProperty(window, 'myClass'") != std::string::npos);
    // Check that the user's script is still present
    assert(result.find("myClass = \"responsive-box\";") != std::string::npos);
}

void test_calc_generation() {
    std::string source = R"(
        div { style { width: 100% - 20px; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("<p>") != std::string::npos);
    assert(result.find("imported") != std::string::npos);
    remove("imported_file.chtl");
}


// Declarations for tests in unquoted_literals.cpp
void test_unquoted_in_text_block();
void test_unquoted_in_attribute();
void test_unquoted_in_style_block();
void test_unquoted_with_numbers_and_hyphens();
void test_unquoted_stops_at_semicolon();
void test_unquoted_with_arithmetic();


int main() {
    std::cout << "Running CHTL tests..." << std::endl;

    run_test(test_referenced_property, "Referenced Property");
    run_test(test_conditional_expression_true, "Conditional Expression (True)");
    run_test(test_conditional_expression_false, "Conditional Expression (False)");
    run_test(test_custom_style_specialization, "Custom Style Specialization");
    run_test(test_custom_element_delete, "Custom Element Deletion");
    run_test(test_custom_element_insert, "Custom Element Insertion");
    run_test(test_custom_element_insert_at_top_bottom, "Custom Element Insertion At Top/Bottom");
    run_test(test_import, "Import Statement");
    run_test(test_text_block_literals, "Text Block Literals");
    run_test(test_unquoted_literal_support, "Unquoted Literal Support");
    run_test(test_enhanced_selector, "Enhanced Selector");
    run_test(test_responsive_value, "Responsive Value");
    run_test(test_calc_generation, "Calc() Generation");
    run_test(test_except_constraint, "Except Constraint");
    run_test(test_named_origin_and_import, "Named Origin and Import");
    run_test(test_delete_style_inheritance, "Delete Style Inheritance");

    run_test(test_namespace_template_access, "Namespace-qualified Template Access");
    run_test(test_keyword_aliasing, "Keyword Aliasing");
    run_test(test_hyphenated_property, "Hyphenated CSS Property");
    run_test(test_attribute_expression, "Attribute Expression");
    run_test(test_conditional_attribute, "Conditional Attribute");
    run_test(test_delete_style_property, "Delete Style Property");
    run_test(test_var_template_usage, "Var Template Usage");
    run_test(test_var_template_specialization, "Var Template Specialization");
    run_test(test_style_auto_add_class, "Style Auto Add Class");
    run_test(test_except_clause_extended, "Extended Except Clause");
    run_test(test_ampersand_selector_order, "Ampersand Selector Order");
    run_test(test_delete_element_inheritance, "Delete Element Inheritance");
    run_test(test_calc_with_percentage, "Calc With Percentage");
    run_test(test_implicit_style_template_inheritance, "Implicit Style Template Inheritance");

    // --- New Unquoted Literal Tests ---
    std::cout << "\n--- Running Unquoted Literal Tests ---\n";
    run_test(test_unquoted_in_text_block, "Unquoted literal in text block");
    run_test(test_unquoted_in_attribute, "Unquoted literal in attribute");
    run_test(test_unquoted_in_style_block, "Unquoted literal in style block");
    run_test(test_unquoted_with_numbers_and_hyphens, "Unquoted literal with numbers and hyphens");
    run_test(test_unquoted_stops_at_semicolon, "Unquoted literal stops at semicolon");
    run_test(test_unquoted_with_arithmetic, "Unquoted literal with arithmetic");


    std::cout << "\nTests finished." << std::endl;
    return 0;
}

void test_calc_with_percentage() {
    std::string source = R"(
        div { style { width: 50% + 20px; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("width: calc(50% + 20px);") != std::string::npos);
}

void test_implicit_style_template_inheritance() {
    std::string source = R"(
        [Template] @Style Base { color: red; }
        [Template] @Style Child {
            @Style Base;
            font-size: 16px;
        }
        div {
            style {
                @Style Child;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("color: red;") != std::string::npos);
    assert(result.find("font-size: 16px;") != std::string::npos);
}

void test_delete_element_inheritance() {
    std::string source = R"(
        [Template] @Element ToBeDeleted {
            p { text: "This should be deleted."; }
        }
        [Custom] @Element MyComponent {
            @Element ToBeDeleted;
            div { text: "This should remain."; }
        }
        body {
            @Element MyComponent {
                delete @Element ToBeDeleted;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("This should be deleted.") == std::string::npos);
    assert(result.find("This should remain.") != std::string::npos);
}

void test_ampersand_selector_order() {
    std::string source = R"(
        html {
            head {}
            body {
                div {
                    style {
                        &:hover { background-color: red; }
                        .my-class { color: blue; }
                    }
                }
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find(".my-class:hover") != std::string::npos);
    assert(result.find("class=\"my-class\"") != std::string::npos);
}

void test_except_clause_extended() {
    // Test case 1: Forbid a specific custom element
    std::string source1 = R"(
        [Custom] @Element MyComponent { div{} }
        html {
            head {}
            body {
                except [Custom] @Element MyComponent;
                @Element MyComponent;
            }
        }
    )";
    Lexer lexer1(source1);
    Parser parser1(lexer1);
    bool exception_thrown1 = false;
    try {
        parser1.parse();
    } catch (const std::runtime_error& e) {
        exception_thrown1 = true;
        std::string msg = e.what();
        assert(msg.find("is not allowed") != std::string::npos);
    }
    assert(exception_thrown1);

    // Test case 2: Forbid all HTML tags
    std::string source2 = R"(
        html {
            head {}
            body {
                except @Html;
                div { }
            }
        }
    )";
    Lexer lexer2(source2);
    Parser parser2(lexer2);
    bool exception_thrown2 = false;
    try {
        parser2.parse();
    } catch (const std::runtime_error& e) {
        exception_thrown2 = true;
        std::string msg = e.what();
        assert(msg.find("is not allowed") != std::string::npos);
    }
    assert(exception_thrown2);
}

void test_style_auto_add_class() {
    // Test case 1: Class should be added automatically
    std::string source1 = R"(
        html {
            head {}
            body {
                div {
                    style {
                        .my-class { color: blue; }
                    }
                }
            }
        }
    )";
    Lexer lexer1(source1);
    Parser parser1(lexer1);
    auto nodes1 = parser1.parse();
    Generator generator1;
    std::string result1 = generator1.generate(nodes1, parser1.globalStyleContent, parser1.sharedContext, false);
    assert(result1.find("class=\"my-class\"") != std::string::npos);
    assert(result1.find(".my-class") != std::string::npos); // Check selector exists
    assert(result1.find("color: blue;") != std::string::npos); // Check rule exists

    // Test case 2: Class should NOT be added if disabled
    std::string source2 = R"(
        [Configuration] {
            DISABLE_STYLE_AUTO_ADD_CLASS = true;
        }
        html {
            head {}
            body {
                div {
                    style {
                        .my-class { color: blue; }
                    }
                }
            }
        }
    )";
    Lexer lexer2(source2);
    Parser parser2(lexer2);
    auto nodes2 = parser2.parse();
    Generator generator2;
    std::string result2 = generator2.generate(nodes2, parser2.globalStyleContent, parser2.sharedContext, false);
    assert(result2.find("class=\"my-class\"") == std::string::npos);
    assert(result2.find(".my-class") != std::string::npos); // The style rule should still be generated
}

void test_var_template_specialization() {
    std::string source = R"(
        [Template] @Var MyTheme {
            primary-color: "blue";
        }
        div {
            style {
                color: MyTheme(primary-color = "green");
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("color: green;") != std::string::npos);
}

void test_var_template_usage() {
    std::string source = R"(
        [Template] @Var MyTheme {
            primary-color: "blue";
            secondary-color: "red";
        }
        div {
            style {
                color: MyTheme(primary-color);
                border-color: MyTheme(secondary-color);
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("color: blue;") != std::string::npos);
    assert(result.find("border-color: red;") != std::string::npos);
}

void test_delete_style_property() {
    std::string source = R"(
        [Template] @Style Base {
            color: red;
            font-size: 16px;
            font-weight: bold;
        }
        div {
            style {
                @Style Base {
                    delete font-size, font-weight;
                }
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("color: red;") != std::string::npos);
    assert(result.find("font-size") == std::string::npos);
    assert(result.find("font-weight") == std::string::npos);
}

void test_conditional_attribute() {
    std::string source = R"(
        div {
            width: 100px;
            height: width > 50px ? 200px : 50px;
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("height=\"200px\"") != std::string::npos);
}

void test_attribute_expression() {
    std::string source = R"(
        div { width: 100px * 2; }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("width=\"200px\"") != std::string::npos);
}

void test_hyphenated_property() {
    std::string source = R"(
        div { style { font-family: "Arial"; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);
    assert(result.find("font-family: Arial;") != std::string::npos);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);

    // Assertions
    assert(result.find("class=\"box-in-space\"") != std::string::npos);
    assert(result.find("border: 1px solid blue;") != std::string::npos);
    assert(result.find("color: blue;") != std::string::npos);
    assert(result.find("class=\"nested-box\"") != std::string::npos);
}

void test_keyword_aliasing() {
    std::string source = R"(
        [Configuration] {
            [Name] {
                KEYWORD_DELETE = "remove";
                KEYWORD_CUSTOM = ["C", "Custom"];
            }
        }

        [C] @Style Base {
            color: white;
            font-size: 16px;
        }

        div {
            style {
                @Style Base {
                    remove font-size;
                }
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, parser.sharedContext, false);

    // Assertions
    assert(result.find("color: white;") != std::string::npos);
    assert(result.find("font-size") == std::string::npos);
}