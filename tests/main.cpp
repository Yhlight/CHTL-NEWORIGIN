#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <fstream> // For std::ofstream
#include <cstdio>  // For remove()
#include <filesystem>
#include <cstdlib> // For system()
#include <algorithm> // for std::remove_if

// Helper function to remove all whitespace from a string
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

// Include all necessary CHTL headers
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "Scanner/UnifiedScanner.h"
#include "Dispatcher/CompilerDispatcher.h"
#include "CHTL_JS/CHTLLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSNode/RawJavaScriptNode.h"
#include "CHTL_JS/CHTLJSNode/CHTLJSEnhancedSelectorNode.h"
#include "Dispatcher/CompilationResult.h"
#include "CHTL_JS/CHTLJSNode/VirtualObjectNode.h"
#include "CHTL_JS/CHTLJSNode/ListenNode.h"

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

void test_lexer_configuration_keyword();
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
void test_use_html5_directive();
void test_named_configuration();
void test_precise_style_import();
void test_precise_var_import();
void test_precise_style_import_with_alias();
void test_precise_import_not_found();
void test_unified_scanner_script_separation();
void test_scanner_differentiates_js_and_chtl_js();
void test_unified_scanner_style_separation();
void test_scanner_ignores_nested_style();
void test_scanner_handles_nested_braces_in_script();
void test_dispatcher_workflow();
void test_info_block_parsing();
void test_unquoted_literal_support();
void test_text_block_literals();
void test_enhanced_selector();
void test_responsive_value();
void test_literal_types();
void test_ignored_comments();
void test_colon_equal_equivalence_in_info();
void test_colon_equal_equivalence_in_config();
void test_colon_equal_equivalence_in_var_template();
void test_style_property_power_operator();
void test_style_property_modulo_operator();
void test_conditional_rendering();
void test_cmod_import();
void test_compiler_dispatcher_full_workflow();
void test_chtl_js_lexer_and_parser();
void test_chtl_js_listen_block();
void test_chtl_js_event_binding_operator();
void test_chtl_js_delegate_block();
void test_chtl_js_animate_block();
void test_chtl_js_script_loader();
void test_chtl_js_virtual_object_declaration();
void test_chtl_js_virtual_object_usage();
void test_chtl_js_router();
void test_dynamic_conditional_expression();
void test_dynamic_conditional_rendering();
void test_module_resolver_path_searching();
void test_cmod_export_enforcement();
void test_cli_inline_flag();
void test_comprehensive_file();


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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");

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
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    // The CHTL JS compiler will generate the querySelector call.
    std::string expected_js = "const myDiv = document.querySelector('#my-div'); myDiv.innerText = \"Hello CHTL JS\";";

    // The final result should contain the script tag with the compiled JS.
    // We use remove_whitespace to make the test less brittle.
    assert(remove_whitespace(result.js_content).find(remove_whitespace(expected_js)) != std::string::npos);
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
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, true);

    // Check for the generated element with its unique ID
    assert(result.html_content.find("<div id=\"chtl-id-0\"") != std::string::npos);
    // Check for the runtime script setup
    assert(result.js_content.find("const __chtl = {") != std::string::npos);
    // Check for variable registration in the runtime script
    assert(result.js_content.find("__chtl.registerBinding('myClass', 'chtl-id-0', 'class', '');") != std::string::npos);
    assert(result.js_content.find("__chtl.registerBinding('myWidth', 'chtl-id-0', 'style.width', 'px');") != std::string::npos);
    // Check for the proxy creation
    assert(result.js_content.find("Object.defineProperty(window, 'myClass'") != std::string::npos);
    // Check that the user's script is still present
    assert(result.js_content.find("myClass = \"responsive-box\";") != std::string::npos);
}

void test_calc_generation() {
    std::string source = R"(
        div { style { width: 100% - 20px; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("width: calc(100% - 20px);") != std::string::npos);
}

void test_except_constraints_parsing_and_enforcement() {
    // Test 1: Simple tag name constraint
    std::string source1 = R"(div { except span; span {} })";
    Lexer lexer1(source1);
    Parser parser1(lexer1);
    bool thrown1 = false;
    try { parser1.parse(); } catch (const std::runtime_error& e) { thrown1 = true; }
    assert(thrown1);

    // Test 2: Valid nesting with a tag name constraint
    std::string source2 = R"(div { except span; p {} })";
    Lexer lexer2(source2);
    Parser parser2(lexer2);
    bool thrown2 = false;
    try { parser2.parse(); } catch (const std::runtime_error& e) { thrown2 = true; }
    assert(!thrown2);

    // Test 3: Qualified name constraint ([Custom] @Element)
    std::string source3 = R"([Custom] @Element Box {} div { except [Custom] @Element Box; @Element Box; })";
    Lexer lexer3(source3);
    Parser parser3(lexer3);
    bool thrown3 = false;
    try { parser3.parse(); } catch (const std::runtime_error& e) { thrown3 = true; }
    assert(thrown3);

    // Test 4: HTML category constraint
    std::string source4 = R"(div { except @Html; p {} })";
    Lexer lexer4(source4);
    Parser parser4(lexer4);
    bool thrown4 = false;
    try { parser4.parse(); } catch (const std::runtime_error& e) { thrown4 = true; }
    assert(thrown4);

    // Test 5: [Template] category constraint
    std::string source5 = R"([Template] @Element Box {} div { except [Template]; @Element Box; })";
    Lexer lexer5(source5);
    Parser parser5(lexer5);
    bool thrown5 = false;
    try { parser5.parse(); } catch (const std::runtime_error& e) { thrown5 = true; }
    assert(thrown5);

    // Test 6: [Custom] category constraint
    std::string source6 = R"([Custom] @Element Box {} div { except [Custom]; @Element Box; })";
    Lexer lexer6(source6);
    Parser parser6(lexer6);
    bool thrown6 = false;
    try { parser6.parse(); } catch (const std::runtime_error& e) { thrown6 = true; }
    assert(thrown6);

    // Test 7: Multiple constraints in one line
    std::string source7 = R"(div { except p, span; span{} })";
    Lexer lexer7(source7);
    Parser parser7(lexer7);
    bool thrown7 = false;
    try { parser7.parse(); } catch (const std::runtime_error& e) { thrown7 = true; }
    assert(thrown7);
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
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, true); // Inline to check content easily

    assert(result.html_content.find("<div>Hello</div>") != std::string::npos);
    assert(result.html_content.find("console.log('imported');") != std::string::npos);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("<p>") != std::string::npos);
    assert(result.find("imported") != std::string::npos);
    remove("imported_file.chtl");
}


int main() {
    std::cout << "Running CHTL tests..." << std::endl;

    run_test(test_lexer_configuration_keyword, "Lexer Configuration Keyword");
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
    run_test(test_except_constraints_parsing_and_enforcement, "Except Constraints Parsing and Enforcement");
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
    run_test(test_ampersand_selector_order, "Ampersand Selector Order");
    run_test(test_delete_element_inheritance, "Delete Element Inheritance");
    run_test(test_calc_with_percentage, "Calc With Percentage");
    run_test(test_implicit_style_template_inheritance, "Implicit Style Template Inheritance");
    run_test(test_use_html5_directive, "Use HTML5 Directive");
    run_test(test_named_configuration, "Named Configuration");
    run_test(test_precise_style_import, "Precise Style Import");
    run_test(test_precise_var_import, "Precise Var Import");
    run_test(test_precise_style_import_with_alias, "Precise Style Import with Alias");
    run_test(test_precise_import_not_found, "Precise Import Not Found");
    run_test(test_unified_scanner_script_separation, "Unified Scanner Script Separation");
    run_test(test_scanner_differentiates_js_and_chtl_js, "Scanner Differentiates JS and CHTL JS");
    run_test(test_unified_scanner_style_separation, "Unified Scanner Style Separation");
    run_test(test_scanner_ignores_nested_style, "Scanner Ignores Nested Style");
    run_test(test_scanner_handles_nested_braces_in_script, "Scanner Handles Nested Braces in Script");
    run_test(test_dispatcher_workflow, "Dispatcher Workflow");
    run_test(test_info_block_parsing, "Info Block Parsing");
    run_test(test_literal_types, "Literal Types");
    run_test(test_ignored_comments, "Ignored Comments");
    run_test(test_colon_equal_equivalence_in_info, "Colon-Equal Equivalence in Info Block");
    run_test(test_colon_equal_equivalence_in_config, "Colon-Equal Equivalence in Config Block");
    run_test(test_colon_equal_equivalence_in_var_template, "Colon-Equal Equivalence in Var Template");
    run_test(test_style_property_power_operator, "Style Property Power Operator");
    run_test(test_style_property_modulo_operator, "Style Property Modulo Operator");
    run_test(test_conditional_rendering, "Conditional Rendering");
    run_test(test_cmod_import, "CMOD Module Import");
    run_test(test_compiler_dispatcher_full_workflow, "Compiler Dispatcher Full Workflow");

    run_test(test_chtl_js_lexer_and_parser, "CHTL JS Lexer and Parser");
    run_test(test_chtl_js_listen_block, "CHTL JS Listen Block");
    run_test(test_chtl_js_event_binding_operator, "CHTL JS Event Binding Operator");
    run_test(test_chtl_js_delegate_block, "CHTL JS Delegate Block");
    run_test(test_chtl_js_animate_block, "CHTL JS Animate Block");
    run_test(test_chtl_js_script_loader, "CHTL JS ScriptLoader");
    run_test(test_chtl_js_virtual_object_declaration, "CHTL JS Virtual Object Declaration");
    run_test(test_chtl_js_virtual_object_usage, "CHTL JS Virtual Object Usage");
    run_test(test_chtl_js_router, "CHTL JS Router");
    run_test(test_dynamic_conditional_expression, "Dynamic Conditional Expression");
    run_test(test_dynamic_conditional_rendering, "Dynamic Conditional Rendering");
    run_test(test_module_resolver_path_searching, "Module Resolver Path Searching");
    run_test(test_cmod_export_enforcement, "CMOD Export Enforcement");
    run_test(test_cli_inline_flag, "CLI --inline Flag");
    run_test(test_comprehensive_file, "Comprehensive Integration Test");

    std::cout << "Tests finished." << std::endl;
    return 0;
}

void test_info_block_parsing() {
    std::string source = R"(
        [Info] {
            name = "MyModule";
            version = "1.0.0";
            author = "CHTL Team";
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    parser.parse();

    assert(parser.infoNode != nullptr);
    assert(parser.infoNode->metadata["name"] == "MyModule");
    assert(parser.infoNode->metadata["version"] == "1.0.0");
    assert(parser.infoNode->metadata["author"] == "CHTL Team");
}

void test_dispatcher_workflow() {
    std::string source = R"(
        style {
            body { color: red; }
        }
        html {
            head {}
            body {
                div {
                    text: "Hello";
                }
            }
        }
    )";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);
    assert(result.css_content.find("body { color: red; }") != std::string::npos);
    assert(result.html_content.find("<div>") != std::string::npos);
}

void test_scanner_handles_nested_braces_in_script() {
    std::string source = R"(
        script {
            if (true) {
                console.log("nested");
            }
        }
    )";
    UnifiedScanner scanner;
    auto output = scanner.scan(source);
    assert(output.fragments.size() == 1);
    const auto& fragment = output.fragments.begin()->second;
    assert(fragment.type == FragmentType::JS);
    assert(fragment.content.find("console.log(\"nested\");") != std::string::npos);
    assert(output.chtl_with_placeholders.find("chtl_placeholder_0{}") != std::string::npos);
}

void test_scanner_ignores_nested_style() {
    std::string source = R"(
        div {
            style {
                color: red;
            }
        }
    )";
    UnifiedScanner scanner;
    auto output = scanner.scan(source);
    assert(output.fragments.empty());
    assert(output.chtl_with_placeholders == source);
}

void test_unified_scanner_style_separation() {
    std::string source = R"(
        style {
            body {
                background-color: #f0f0f0;
            }
        }
        div {
            text: "Hello";
        }
    )";
    UnifiedScanner scanner;
    auto output = scanner.scan(source);
    assert(output.fragments.size() == 1);
    const auto& fragment = output.fragments.begin()->second;
    assert(fragment.type == FragmentType::CSS);
    assert(fragment.content.find("background-color") != std::string::npos);
    assert(output.chtl_with_placeholders.find("chtl_placeholder_0{}") != std::string::npos);
    assert(output.chtl_with_placeholders.find("div {") != std::string::npos);
}

void test_unified_scanner_script_separation() {
    std::string source = R"(
        div {
            p { text: "Hello"; }
            script {
                console.log("World");
            }
            span { text: "!"; }
        }
    )";
    UnifiedScanner scanner;
    auto output = scanner.scan(source);
    assert(output.fragments.size() == 1);
    const auto& fragment = output.fragments.begin()->second;
    // This test now correctly asserts that plain JS is identified as JS.
    assert(fragment.type == FragmentType::JS);
    assert(fragment.content.find("console.log(\"World\")") != std::string::npos);
    assert(output.chtl_with_placeholders.find("chtl_placeholder_0{}") != std::string::npos);
    assert(output.chtl_with_placeholders.find("p { text: \"Hello\"; }") != std::string::npos);
}

void test_scanner_differentiates_js_and_chtl_js() {
    // Test 1: Plain JS should be identified as JS
    std::string js_source = "script { const x = 1; }";
    UnifiedScanner scanner_js;
    auto output_js = scanner_js.scan(js_source);
    assert(output_js.fragments.size() == 1);
    assert(output_js.fragments.begin()->second.type == FragmentType::JS);

    // Test 2: CHTL JS (with '->') should be identified as CHTL_JS
    std::string chtl_js_source_1 = "script { {{sel}}->Listen{}; }";
    UnifiedScanner scanner_chtl_js_1;
    auto output_chtl_js_1 = scanner_chtl_js_1.scan(chtl_js_source_1);
    assert(output_chtl_js_1.fragments.size() == 1);
    assert(output_chtl_js_1.fragments.begin()->second.type == FragmentType::CHTL_JS);

    // Test 3: CHTL JS (with '{{}}') should be identified as CHTL_JS
    std::string chtl_js_source_2 = "script { const y = {{z}}; }";
    UnifiedScanner scanner_chtl_js_2;
    auto output_chtl_js_2 = scanner_chtl_js_2.scan(chtl_js_source_2);
    assert(output_chtl_js_2.fragments.size() == 1);
    assert(output_chtl_js_2.fragments.begin()->second.type == FragmentType::CHTL_JS);
}

void test_precise_import_not_found() {
    std::ofstream("styles.chtl") << "[Template] @Style MyStyle { color: blue; }";
    std::string source = R"(
        [Import] @Style NonExistentStyle from "styles.chtl";
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    bool exception_thrown = false;
    try {
        parser.parse();
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        std::string msg = e.what();
        assert(msg.find("not found in") != std::string::npos);
    }
    assert(exception_thrown);
    remove("styles.chtl");
}

void test_precise_style_import_with_alias() {
    std::ofstream("styles.chtl") << "[Template] @Style MyStyle { color: green; }";
    std::string source = R"(
        [Import] @Style MyStyle from "styles.chtl" as MyAlias;
        div {
            style {
                @Style MyAlias;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("color: green;") != std::string::npos);
    remove("styles.chtl");
}

void test_precise_style_import() {
    std::ofstream("styles.chtl") << "[Template] @Style MyStyle { color: blue; }";
    std::string source = R"(
        [Import] @Style MyStyle from "styles.chtl";
        div {
            style {
                @Style MyStyle;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("color: blue;") != std::string::npos);
    remove("styles.chtl");
}

void test_precise_var_import() {
    std::ofstream("vars.chtl") << "[Template] @Var MyVars { primaryColor: \"red\"; }";
    std::string source = R"(
        [Import] @Var MyVars from "vars.chtl";
        div {
            style {
                color: MyVars(primaryColor);
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("color: red;") != std::string::npos);
    remove("vars.chtl");
}

void test_use_html5_directive() {
    std::string source = R"(
        use html5;
        html {
            body {
                text: "Hello";
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, parser.outputHtml5Doctype, true, "", "");
    assert(result.rfind("<!DOCTYPE html>", 0) == 0);
}

void test_named_configuration() {
    std::string source = R"(
        [Configuration] @Config MyConfig {
            [Name] {
                KEYWORD_DELETE = "remove";
            }
        }

        use @Config MyConfig;

        [Template] @Style Base {
            color: red;
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, parser.outputHtml5Doctype, true, "", "");
    assert(result.find("font-size") == std::string::npos);
    assert(result.find("color: red;") != std::string::npos);
}

void test_calc_with_percentage() {
    std::string source = R"(
        div { style { width: 50% + 20px; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(parser.globalStyleContent.find(".my-class:hover") != std::string::npos);
    assert(result.find("class=\"my-class\"") != std::string::npos);
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
    std::string result1 = generator1.generate(nodes1, parser1.globalStyleContent, "", parser1.sharedContext, false, true, "", "");
    assert(result1.find("class=\"my-class\"") != std::string::npos);
    assert(parser1.globalStyleContent.find(".my-class") != std::string::npos); // Check selector exists
    assert(parser1.globalStyleContent.find("color: blue;") != std::string::npos); // Check rule exists

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
    std::string result2 = generator2.generate(nodes2, parser2.globalStyleContent, "", parser2.sharedContext, false, true, "", "");
    assert(result2.find("class=\"my-class\"") == std::string::npos);
    assert(parser2.globalStyleContent.find(".my-class") != std::string::npos); // The style rule should still be generated
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("width=\"200px\"") != std::string::npos);
}

void test_literal_types() {
    std::string source = R"(
        div {
            text: 'single-quoted attribute';
        }
        p {
            text: "double-quoted attribute";
        }
        span {
            text { 'single-quoted block' }
        }
        i {
            text { "double-quoted block" }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");

    assert(result.find("single-quoted attribute") != std::string::npos);
    assert(result.find("double-quoted attribute") != std::string::npos);
    assert(result.find("single-quoted block") != std::string::npos);
    assert(result.find("double-quoted block") != std::string::npos);
}

void test_colon_equal_equivalence_in_config() {
    std::string source = R"(
        [Configuration] {
            DEBUG_MODE: true;

            [Name] {
                KEYWORD_DELETE: "remove";
            }
        }

        [Template] @Style Base {
            color: red;
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");

    assert(parser.configManager.getActiveConfig()->debugMode == true);
    assert(result.find("font-size") == std::string::npos);
    assert(result.find("color: red;") != std::string::npos);
}

void test_colon_equal_equivalence_in_var_template() {
    std::string source = R"(
        [Template] @Var MyTheme {
            primary-color = "blue";
        }
        div {
            style {
                color: MyTheme(primary-color);
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("color: blue;") != std::string::npos);
}

void test_colon_equal_equivalence_in_info() {
    std::string source = R"(
        [Info] {
            name: "MyModuleWithColon";
            version: "1.0.0";
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    parser.parse();

    assert(parser.infoNode != nullptr);
    assert(parser.infoNode->metadata["name"] == "MyModuleWithColon");
    assert(parser.infoNode->metadata["version"] == "1.0.0");
}

void test_ignored_comments() {
    std::string source = R"(
        // This is a single-line comment.
        div {
            /* This is a
               multi-line comment. */
            text: "Hello"; // This should be parsed.
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");

    assert(result.find("This is a single-line comment.") == std::string::npos);
    assert(result.find("This is a\n               multi-line comment.") == std::string::npos);
    assert(result.find("Hello") != std::string::npos);
}

void test_hyphenated_property() {
    std::string source = R"(
        div { style { font-family: "Arial"; } }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
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
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, true);

    // Assertions
    assert(result.html_content.find("class=\"box-in-space\"") != std::string::npos);
    assert(result.html_content.find("border: 1px solid blue;") != std::string::npos);
    assert(result.html_content.find("color: blue;") != std::string::npos);
    assert(result.html_content.find("class=\"nested-box\"") != std::string::npos);
}

void test_lexer_configuration_keyword() {
    std::string source = "[Configuration] { DEBUG_MODE = true; }";
    Lexer lexer(source);
    std::vector<Token> tokens;
    Token token = lexer.getNextToken();
    while (token.type != TokenType::EndOfFile) {
        tokens.push_back(token);
        token = lexer.getNextToken();
    }

    assert(tokens.size() == 9);
    assert(tokens[0].type == TokenType::OpenBracket);
    assert(tokens[1].type == TokenType::Configuration);
    assert(tokens[2].type == TokenType::CloseBracket);
    assert(tokens[3].type == TokenType::OpenBrace);
    assert(tokens[4].type == TokenType::Identifier);
    assert(tokens[4].value == "DEBUG_MODE");
    assert(tokens[5].type == TokenType::Equals);
    assert(tokens[6].type == TokenType::Identifier);
    assert(tokens[6].value == "true");
    assert(tokens[7].type == TokenType::Semicolon);
    assert(tokens[8].type == TokenType::CloseBrace);
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
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");

    // Assertions
    assert(result.find("color: white;") != std::string::npos);
    assert(result.find("font-size") == std::string::npos);
}

void test_style_property_power_operator() {
    std::string source = R"(
        div {
            style {
                width: 10px ** 2;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("width: 100px;") != std::string::npos);
}

void test_style_property_modulo_operator() {
    std::string source = R"(
        div {
            style {
                width: 10px % 3;
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string result = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");
    assert(result.find("width: 1px;") != std::string::npos);
}

void test_conditional_rendering() {
    // Test case 1: 'if' condition is true
    std::string source1 = R"(
        div {
            if {
                condition: 10 > 5;
                p { text: "Visible"; }
            }
        }
    )";
    Lexer lexer1(source1);
    Parser parser1(lexer1);
    auto nodes1 = parser1.parse();
    Generator generator1;
    std::string result1 = generator1.generate(nodes1, parser1.globalStyleContent, "", parser1.sharedContext, false, true, "", "");
    assert(result1.find("Visible") != std::string::npos);

    // Test case 2: 'if' condition is false, no else block
    std::string source2 = R"(
        div {
            if {
                condition: 5 > 10;
                p { text: "Hidden"; }
            }
        }
    )";
    Lexer lexer2(source2);
    Parser parser2(lexer2);
    auto nodes2 = parser2.parse();
    Generator generator2;
    std::string result2 = generator2.generate(nodes2, parser2.globalStyleContent, "", parser2.sharedContext, false, true, "", "");
    assert(result2.find("Hidden") == std::string::npos);

    // Test case 3: 'if' is false, 'else' is rendered
    std::string source3 = R"(
        div {
            if {
                condition: 5 > 10;
                p { text: "Hidden"; }
            } else {
                p { text: "Visible"; }
            }
        }
    )";
    Lexer lexer3(source3);
    Parser parser3(lexer3);
    auto nodes3 = parser3.parse();
    Generator generator3;
    std::string result3 = generator3.generate(nodes3, parser3.globalStyleContent, "", parser3.sharedContext, false, true, "", "");
    assert(result3.find("Hidden") == std::string::npos);
    assert(result3.find("Visible") != std::string::npos);

    // Test case 4: 'else if' is rendered
    std::string source4 = R"(
        div {
            if {
                condition: 1 == 0;
                p { text: "Hidden 1"; }
            } else if {
                condition: 2 > 1;
                p { text: "Visible"; }
            } else {
                p { text: "Hidden 2"; }
            }
        }
    )";
    Lexer lexer4(source4);
    Parser parser4(lexer4);
    auto nodes4 = parser4.parse();
    Generator generator4;
    std::string result4 = generator4.generate(nodes4, parser4.globalStyleContent, "", parser4.sharedContext, false, true, "", "");
    assert(result4.find("Hidden 1") == std::string::npos);
    assert(result4.find("Hidden 2") == std::string::npos);
    assert(result4.find("Visible") != std::string::npos);
}

void test_cmod_import() {
    namespace fs = std::filesystem;
    const std::string module_name = "test_module";
    const std::string cmod_file = module_name + ".cmod";
    fs::path module_path(module_name);

    // 1. Create directory structure
    fs::create_directory(module_path);
    fs::create_directory(module_path / "src");
    fs::create_directory(module_path / "info");

    // 2. Create module files
    std::ofstream info_file(module_path / "info" / (module_name + ".chtl"));
    info_file << "[Info] { name: \"Test Module\"; }";
    info_file.close();

    std::ofstream src_file(module_path / "src" / "main.chtl");
    src_file << "[Template] @Element MyCmodComponent { p { text: \"Hello from CMOD\"; } }";
    src_file.close();

    // 3. Package the module using the cmod_packer
    // Note: This assumes the test runner and cmod_packer are in the same directory
    std::string command = "./cmod_packer " + module_name + " " + cmod_file;
    int result = system(command.c_str());
    assert(result == 0);

    // 4. Test importing the CMOD
    std::string source = R"(
        [Import] @Chtl from ")" + cmod_file + R"(";
        body {
            @Element MyCmodComponent;
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    auto nodes = parser.parse();
    Generator generator;
    std::string html_output = generator.generate(nodes, parser.globalStyleContent, "", parser.sharedContext, false, true, "", "");

    assert(html_output.find("Hello from CMOD") != std::string::npos);

    // 5. Cleanup
    fs::remove(cmod_file);
    fs::remove_all(module_path);
}

void test_compiler_dispatcher_full_workflow() {
    std::string source = R"(
        style {
            body { font-family: Arial; }
        }
        html {
            head {
                title { text: "Test Page"; }
            }
            body {
                div {
                    class: "container";
                    p { text: "Hello, CHTL!"; }
                }
                script {
                    console.log("Hello from the script block!");
                }
            }
        }
    )";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    // 1. Check for CHTL-generated content
    assert(result.html_content.find("<title>Test Page</title>") != std::string::npos);
    assert(result.html_content.find("<div class=\"container\">") != std::string::npos);
    assert(result.html_content.find("<p>Hello, CHTL!</p>") != std::string::npos);

    // 2. Check for global CSS
    assert(result.css_content.find("body { font-family: Arial; }") != std::string::npos);

    // 3. Check for re-inserted JavaScript
    assert(result.js_content.find("console.log(\"Hello from the script block!\");") != std::string::npos);
}

void test_chtl_js_lexer_and_parser() {
    std::string source = "const a = {{#my-id}};";

    // Test Lexer
    CHTLJSLexer lexer(source);
    auto tokens = lexer.tokenize();

    // Expected token stream: RawJS, OpenDoubleBrace, RawJS, CloseDoubleBrace, Semicolon, EndOfFile
    assert(tokens.size() == 6);
    assert(tokens[0].type == CHTLJSTokenType::RawJS && tokens[0].value == "const a = ");
    assert(tokens[1].type == CHTLJSTokenType::OpenDoubleBrace);
    assert(tokens[2].type == CHTLJSTokenType::RawJS && tokens[2].value == "#my-id");
    assert(tokens[3].type == CHTLJSTokenType::CloseDoubleBrace);
    assert(tokens[4].type == CHTLJSTokenType::Semicolon);
    assert(tokens[5].type == CHTLJSTokenType::EndOfFile);

    // Test Parser
    CHTLJSParser parser(tokens);
    auto nodes = parser.parse();

    assert(nodes.size() == 3);
    assert(nodes[0]->getType() == CHTLJSNodeType::RawJavaScript);
    assert(nodes[1]->getType() == CHTLJSNodeType::EnhancedSelector);
    assert(nodes[2]->getType() == CHTLJSNodeType::RawJavaScript);

    // Verify the content of the parsed nodes
    auto* raw_node_1 = static_cast<RawJavaScriptNode*>(nodes[0].get());
    assert(raw_node_1->js_code == "const a = ");

    auto* selector_node = static_cast<CHTLJSEnhancedSelectorNode*>(nodes[1].get());
    assert(selector_node->selector_text == "#my-id");

    auto* raw_node_2 = static_cast<RawJavaScriptNode*>(nodes[2].get());
    assert(raw_node_2->js_code == ";");
}

void test_chtl_js_listen_block() {
    std::string source = R"(
        div {
            id: "my-btn";
            script {
                {{#my-btn}} -> Listen {
                    click: () => { console.log("Clicked!"); },
                    mouseover: () => { console.log("Hovered!"); }
                }
            }
        }
    )";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    // Check that the generated JS is correct
    std::string expected_js1 = "document.querySelector('#my-btn').addEventListener('click', () => { console.log(\"Clicked!\"); });";
    std::string expected_js2 = "document.querySelector('#my-btn').addEventListener('mouseover', () => { console.log(\"Hovered!\"); });";

    assert(remove_whitespace(result.js_content).find(remove_whitespace(expected_js1)) != std::string::npos);
    assert(remove_whitespace(result.js_content).find(remove_whitespace(expected_js2)) != std::string::npos);
}

void test_chtl_js_event_binding_operator() {
    // Test case 1: Single event binding
    std::string source1 = R"(
        div {
            id: "my-btn";
            script {
                {{#my-btn}} &-> click: () => { console.log("Clicked!"); };
            }
        }
    )";
    CompilerDispatcher dispatcher1;
    CompilationResult result1 = dispatcher1.compile(source1);
    std::string expected_js1 = "document.querySelector('#my-btn').addEventListener('click', () => { console.log(\"Clicked!\"); });";
    assert(remove_whitespace(result1.js_content).find(remove_whitespace(expected_js1)) != std::string::npos);

    // Test case 2: Multiple event bindings
    std::string source2 = R"(
        div {
            id: "my-area";
            script {
                {{#my-area}} &-> mouseover, mouseout: (e) => { e.target.classList.toggle("hover"); };
            }
        }
    )";
    CompilerDispatcher dispatcher2;
    CompilationResult result2 = dispatcher2.compile(source2);
    std::string expected_js2_1 = "document.querySelector('#my-area').addEventListener('mouseover', (e) => { e.target.classList.toggle(\"hover\"); });";
    std::string expected_js2_2 = "document.querySelector('#my-area').addEventListener('mouseout', (e) => { e.target.classList.toggle(\"hover\"); });";
    assert(remove_whitespace(result2.js_content).find(remove_whitespace(expected_js2_1)) != std::string::npos);
    assert(remove_whitespace(result2.js_content).find(remove_whitespace(expected_js2_2)) != std::string::npos);
}

void test_chtl_js_virtual_object_declaration() {
    std::string source = R"(
        Vir myTest = Listen {
            click: () => {}
        };
    )";

    // We test the parser directly, as the compiler/generator logic for Vir is not yet implemented.
    CHTLJSLexer lexer(source);
    auto tokens = lexer.tokenize();
    CHTLJSParser parser(tokens);
    auto nodes = parser.parse();

    assert(nodes.size() == 1);
    assert(nodes[0]->getType() == CHTLJSNodeType::VirtualObject);

    auto* virNode = static_cast<VirtualObjectNode*>(nodes[0].get());
    assert(virNode->name == "myTest");
    assert(virNode->value != nullptr);
    assert(virNode->value->getType() == CHTLJSNodeType::Listen);
}

void test_chtl_js_virtual_object_usage() {
    std::string source = R"(
        Vir myListener = Listen {
            click: () => { console.log("Virtual click!"); }
        };

        {{#my-button}} -> myListener->click;
    )";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    std::string expected_js = "document.querySelector('#my-button').addEventListener('click', () => { console.log(\"Virtual click!\"); });";
    assert(remove_whitespace(result.js_content).find(remove_whitespace(expected_js)) != std::string::npos);
}

void test_chtl_js_router() {
    std::string source = R"(
        script {
            Router {
                mode: "history",
                root: "/",
                url: "/home", "/about",
                page: {{#home-page}}, {{#about-page}}
            }
        }
    )";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);
    std::string result_no_space = remove_whitespace(result.js_content);

    // 1. Check for the injected router library
    assert(result_no_space.find(remove_whitespace("window.CHTLRouter=CHTLRouter;")) != std::string::npos);

    // 2. Check for the configuration call
    assert(result_no_space.find(remove_whitespace("CHTLRouter.config({mode:'history',rootPath:'/',});")) != std::string::npos);

    // 3. Check for the route adding calls
    assert(result_no_space.find(remove_whitespace("CHTLRouter.add('/home','#home-page');")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("CHTLRouter.add('/about','#about-page');")) != std::string::npos);
}

void test_chtl_js_delegate_block() {
    std::string source = R"(
        div {
            id: "parent-container";
            script {
                {{#parent-container}} -> Delegate {
                    target: {{.child-button}},
                    click: (event) => { console.log("Delegated click happened!"); }
                }
            }
        }
    )";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    // Check that the generated JS contains the key parts of the delegation logic
    std::string result_no_space = remove_whitespace(result.js_content);
    assert(result_no_space.find(remove_whitespace("const parent_for_delegation_")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace(".addEventListener('click', (event) => {")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("let target = event.target;")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("while (target && target !== parent_for_delegation_")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("if (target.matches('.child-button'))")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("((event) => { console.log(\"Delegated click happened!\"); }).call(target, event);")) != std::string::npos);
}

void test_chtl_js_animate_block() {
    std::string source = R"(
        script {
            const myAnimation = Animate {
                target: {{#my-element}},
                duration: 1000,
                easing: 'ease-in-out'
            };
        }
    )";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    // Check for the main properties
    std::string result_no_space = remove_whitespace(result.js_content);
    assert(result_no_space.find(remove_whitespace("const myAnimation = {")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("target: document.querySelector('#my-element')")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("duration: 1000")) != std::string::npos);
    assert(result_no_space.find(remove_whitespace("easing: 'ease-in-out'")) != std::string::npos);
}

void test_chtl_js_script_loader() {
    std::string source = R"(
        script {
            ScriptLoader {
                load: "./module1.js", "module2.js";
                load: "module3.js";
            }
        }
    )";

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    // Check for the injected loader
    assert(result.js_content.find("window.ScriptLoader = ScriptLoader;") != std::string::npos);

    // Check for the load calls
    std::string expected_js = "ScriptLoader.load('./module1.js', 'module2.js', 'module3.js');";
    assert(result.js_content.find(expected_js) != std::string::npos);
}

void test_dynamic_conditional_expression() {
    std::string source = R"(
        div {
            id: "source-box";
            style { width: 200px; }
        }
        div {
            style {
                height: {{#source-box}}->width > 100px ? 50px : 20px;
            }
        }
    )";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, true);

    // Check that the dynamic binding was registered in the JS runtime
    std::string binding_registration = "__chtl.registerDynamicBinding({targetElementId: 'chtl-id-0',targetProperty: 'style.height',expression: {selector: '#source-box',property: 'width',op: '>',value_to_compare: 100,true_branch: '50px',false_branch: '20px'}});";
    assert(remove_whitespace(result.js_content).find(remove_whitespace(binding_registration)) != std::string::npos);

    // Check that the runtime evaluation logic is present
    assert(result.js_content.find("evaluateDynamicBindings") != std::string::npos);
    assert(result.js_content.find("new MutationObserver") != std::string::npos);
}

void test_cli_inline_flag() {
    std::string source = R"(
        style {
            body { color: purple; }
        }
        html {
            head { title { text: "Inline Test"; } }
            body { p { text: "Hello"; } }
        }
    )";

    // Test 1: With inlining enabled
    CompilerDispatcher dispatcher_inline;
    CompilationResult result_inline = dispatcher_inline.compile(source, true);
    assert(result_inline.html_content.find("<style>") != std::string::npos);
    assert(result_inline.html_content.find("body { color: purple; }") != std::string::npos);
    assert(result_inline.html_content.find("</style>") != std::string::npos);

    // Test 2: With inlining disabled (default)
    CompilerDispatcher dispatcher_no_inline;
    CompilationResult result_no_inline = dispatcher_no_inline.compile(source, false);
    assert(result_no_inline.html_content.find("<style>") == std::string::npos);
    assert(result_no_inline.css_content.find("body { color: purple; }") != std::string::npos);
}

void test_comprehensive_file() {
    std::ifstream file_stream("comprehensive.chtl");
    if (!file_stream) {
        throw std::runtime_error("Could not open comprehensive.chtl for testing.");
    }
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    std::string source = buffer.str();

    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, true); // Inline everything for easier testing

    // --- Assertions ---

    // 1. Static content and structure
    assert(result.html_content.find("<title>Comprehensive Test</title>") != std::string::npos);
    assert(result.html_content.find("<h1>CHTL Comprehensive Test Page</h1>") != std::string::npos);

    // 2. Static conditional rendering
    assert(result.html_content.find("This static condition is true.") != std::string::npos);

    // 3. Dynamic conditional rendering
    assert(result.html_content.find("<div id=\"chtl-dyn-render-") != std::string::npos);
    assert(result.html_content.find("This content appears on wide screens.") != std::string::npos);
    assert(result.js_content.find("dynamicRenderingBindings") != std::string::npos);

    // 4. Template usage and specialization
    assert(result.html_content.find("<h2 class=\"card-title\" style=\"color: blue;\">Specialized Card Title</h2>") != std::string::npos);
    assert(result.html_content.find("<div class=\"special-insert\">This was inserted!</div>") != std::string::npos);
    assert(remove_whitespace(result.html_content).find(remove_whitespace(".card {box-shadow: 0 2px 4px rgba(0,0,0,0.1);}")) != std::string::npos);

    // 5. Responsive values and dynamic styles
    assert(result.html_content.find("id=\"responsive-div\"") != std::string::npos);
    assert(result.js_content.find("__chtl.registerBinding(\"boxClass\", \"responsive-div\", \"class\", \"\");") != std::string::npos);
    assert(result.js_content.find("__chtl.registerBinding(\"boxWidth\", \"responsive-div\", \"style.width\", \"px\");") != std::string::npos);
    assert(result.html_content.find("height: calc(100px / 2);") != std::string::npos);
    assert(result.html_content.find("border-color: red;") != std::string::npos);
    assert(result.js_content.find("targetProperty: 'style.border-color'") != std::string::npos);

    // 6. CHTL JS features
    assert(result.html_content.find("{{#my-button}} -> Listen") == std::string::npos); // Should be compiled away
    assert(result.js_content.find("document.querySelector('#my-button').addEventListener('click'") != std::string::npos);
    assert(result.js_content.find("boxWidth = boxWidth + 50;") != std::string::npos);
    assert(result.js_content.find("document.querySelector('#main-container').addEventListener('mousemove'") != std::string::npos);
}

void test_module_resolver_path_searching() {
    // The test executable runs from the 'build' directory. The ModuleResolver currently
    // uses the CWD to resolve paths. So we create the test module inside the build dir.
    namespace fs = std::filesystem;
    fs::path test_modules_dir = "modules";
    fs::create_directory(test_modules_dir);
    std::ofstream test_module_file(test_modules_dir / "MyModule.chtl");
    test_module_file << "[Template] @Element MyModuleComponent { p { text: \"Hello from a resolved module!\"; } }";
    test_module_file.close();

    std::string source = R"(
        [Import] @Chtl from "MyModule"; // Import by name, without path or extension
        body {
            @Element MyModuleComponent;
        }
    )";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source);

    // Clean up
    fs::remove_all(test_modules_dir);

    assert(result.html_content.find("Hello from a resolved module!") != std::string::npos);
}

void test_cmod_export_enforcement() {
    namespace fs = std::filesystem;
    const std::string module_filename = "export_test_module.chtl";

    // 1. Create a single module file with Info, Export, and definitions.
    std::ofstream test_module_file(module_filename);
    test_module_file << R"(
        [Info] { name: "Export Test"; }
        [Export] { [Template] @Element ExportedComponent; }

        [Template] @Element ExportedComponent { p { text: "I am exported."; } }
        [Template] @Element SecretComponent { p { text: "I am not exported."; } }
    )";
    test_module_file.close();

    // 2. Test importing the exported component (should succeed)
    std::string source_success = R"([Import] @Element ExportedComponent from ")" + module_filename + R"("; body { @Element ExportedComponent; })";
    CompilerDispatcher dispatcher_success;
    CompilationResult result_success = dispatcher_success.compile(source_success);
    assert(result_success.html_content.find("I am exported.") != std::string::npos);

    // 3. Test importing the non-exported component (should fail)
    std::string source_fail = R"([Import] @Element SecretComponent from ")" + module_filename + R"(";)";
    bool exception_thrown = false;
    try {
        CompilerDispatcher dispatcher_fail;
        dispatcher_fail.compile(source_fail);
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        std::string msg = e.what();
        assert(msg.find("is not exported by module") != std::string::npos);
    }
    assert(exception_thrown);

    // 4. Cleanup
    fs::remove(module_filename);
}

void test_dynamic_conditional_rendering() {
    std::string source = R"(
        div {
            id: "source-box";
            style { width: 200px; }
        }
        if {
            condition: {{#source-box}}->width > 100px;
            p { text: "This should be rendered"; }
        }
    )";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, true);

    // Check for the wrapper div with a unique ID
    assert(result.html_content.find("<div id=\"chtl-dyn-render-") != std::string::npos);
    // Check that the content is inside the wrapper
    assert(result.html_content.find("<p>This should be rendered</p>") != std::string::npos);
    // Check that the dynamic rendering logic is in the script
    assert(result.js_content.find("dynamicRenderingBindings") != std::string::npos);
    assert(result.js_content.find("targetEl.style.display = condition ? '' : 'none';") != std::string::npos);
}