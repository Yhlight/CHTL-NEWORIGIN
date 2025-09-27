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

// Helper function to create a directory for test modules
void setup_test_environment() {
    std::filesystem::create_directory("test_modules");
    std::filesystem::create_directory("test_modules/nested");
}

// Helper function to clean up created files and directories
void cleanup_test_environment() {
    std::filesystem::remove_all("test_modules");
}

// Helper function to write a file
void write_file(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
    file.close();
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
void test_text_block_literals();
void test_unquoted_literal_support();
void test_enhanced_selector();
void test_responsive_value();
void test_calc_generation();
void test_except_constraints_parsing_and_enforcement();
void test_named_origin_and_import();
void test_delete_style_inheritance();
void test_referenced_property();
void test_conditional_expression_true();
void test_conditional_expression_false();
void test_custom_style_specialization();
void test_custom_element_delete();
void test_custom_element_insert();
void test_custom_element_insert_at_top_bottom();
void test_import();
void test_namespace_template_access();
void test_keyword_aliasing();
void test_hyphenated_property();
void test_attribute_expression();
void test_conditional_attribute();
void test_delete_style_property();
void test_var_template_usage();
void test_var_template_specialization();
void test_style_auto_add_class();
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
void test_dispatcher_workflow();
void test_info_block_parsing();
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
void test_lexer_configuration_keyword();
void test_import_basic_element();
void test_import_relative_path();
void test_import_nested();
void test_import_with_alias();
void test_import_export_enforcement_pass();
void test_import_export_enforcement_fail();
void test_import_custom_template();
void test_custom_style_definition_delete();
void test_style_template_with_calculation();
void test_style_template_with_var_reference();
void test_keyword_aliasing();
void test_custom_origin_type();


void test_text_block_literals() {
    std::string source = R"(div { text { This is unquoted } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("This is unquoted") != std::string::npos);
}

void test_unquoted_literal_support() {
    std::string source = R"(div { text: This is an unquoted attribute value; style { font-family: Times New Roman; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("This is an unquoted attribute value") != std::string::npos);
    assert(result.html_content.find("font-family: Times New Roman;") != std::string::npos);
}

void test_enhanced_selector() {
    std::string source = R"(div { id: "my-div"; script { const myDiv = {{#my-div}}; myDiv.innerText = "Hello CHTL JS"; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    std::string expected_js = "const myDiv = document.querySelector('#my-div'); myDiv.innerText = \"Hello CHTL JS\";";
    assert(remove_whitespace(result.js_content).find(remove_whitespace(expected_js)) != std::string::npos);
}

void test_responsive_value() {
    std::string source = R"(div { class: $myClass$; style { width: $myWidth$px; } script { myClass = "responsive-box"; myWidth = 250; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("<div id=\"chtl-id-0\"") != std::string::npos);
    assert(result.js_content.find("const __chtl = {") != std::string::npos);
    assert(result.js_content.find("__chtl.registerBinding('myClass', 'chtl-id-0', 'class', '');") != std::string::npos);
}

void test_calc_generation() {
    std::string source = R"(div { style { width: 100% - 20px; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("width: calc(100% - 20px);") != std::string::npos);
}

void test_except_constraints_parsing_and_enforcement() {
    std::string source = R"(div { except span; span {} })";
    Lexer lexer(source);
    Parser parser(lexer, "");
    bool thrown = false;
    try { parser.parse(); } catch (const std::runtime_error&) { thrown = true; }
    assert(thrown);
}

void test_named_origin_and_import() {
    std::ofstream("imported_script.js") << "console.log('imported');";
    std::string source = R"([Import] @JavaScript from "imported_script.js" as my_script; [Origin] @Html my_html { <div>Hello</div> } body { [Origin] @Html my_html; script { [Origin] @JavaScript my_script; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "main.chtl", true);
    assert(result.html_content.find("<div>Hello</div>") != std::string::npos);
    assert(result.html_content.find("console.log('imported');") != std::string::npos);
    remove("imported_script.js");
}

void test_delete_style_inheritance() {
    std::string source = R"([Template] @Style Parent { color: red; font_size: 16px; } [Custom] @Style Child { inherit @Style Parent; font_weight: bold; } div { style { @Style Child { delete @Style Parent; font_size: 20px; } } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("color: red;") == std::string::npos);
    assert(result.html_content.find("font_weight: bold;") != std::string::npos);
}

void test_referenced_property() {
    std::string source = R"(div { id: "box1"; style { width: 100px; } } div { id: "box2"; style { height: #box1.width; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("height: 100px;") != std::string::npos);
}

void test_conditional_expression_true() {
    std::string source = R"(div { style { width: 200px; color: width > 100px ? "red" : "blue"; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("color: red;") != std::string::npos);
}

void test_conditional_expression_false() {
    std::string source = R"(div { style { width: 50px; color: width > 100px ? "red" : "blue"; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("color: blue;") != std::string::npos);
}

void test_custom_style_specialization() {
    std::string source = R"([Custom] @Style Base { color; border: 1px solid black; } div { style { @Style Base { color: green; delete border; } } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("color: green;") != std::string::npos);
    assert(result.html_content.find("border:") == std::string::npos);
}

void test_custom_element_delete() {
    std::string source = R"([Custom] @Element Box { span { text: "hello"; } div { text: "world"; } p { text: "paragraph"; } } body { @Element Box { delete span; delete p[0]; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("<span>hello</span>") == std::string::npos);
    assert(result.html_content.find("<p>paragraph</p>") == std::string::npos);
    assert(result.html_content.find("<div>world</div>") != std::string::npos);
}

void test_custom_element_insert() {
    std::string source = R"([Custom] @Element Box { div { text: "middle"; } } body { @Element Box { insert at top { header { text: "top"; } } insert before div[0] { span { text: "before"; } } insert after div[0] { p { text: "after"; } } insert at bottom { footer { text: "bottom"; } } } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    size_t top_pos = result.html_content.find("top");
    size_t before_pos = result.html_content.find("before");
    size_t middle_pos = result.html_content.find("middle");
    size_t after_pos = result.html_content.find("after");
    size_t bottom_pos = result.html_content.find("bottom");
    assert(top_pos < before_pos && before_pos < middle_pos && middle_pos < after_pos && after_pos < bottom_pos);
}

void test_custom_element_insert_at_top_bottom() {
    std::string source = R"([Custom] @Element Box { div { text: "middle"; } } body { @Element Box { insert at top { header { text: "top"; } } insert at bottom { footer { text: "bottom"; } } } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("<header>") < result.html_content.find("<div>"));
    assert(result.html_content.find("<div>") < result.html_content.find("<footer>"));
}

void test_import() {
    std::ofstream("imported_file.chtl") << "[Template] @Element MyElem { p { text: \"imported\"; } }";
    std::string source = R"([Import] @Chtl from "imported_file.chtl"; body { @Element MyElem; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test.chtl", true);
    assert(result.html_content.find("imported") != std::string::npos);
    remove("imported_file.chtl");
}

void test_import_basic_element() {
    write_file("test_modules/module.chtl", R"([Template] @Element MyComponent { div { text: "Hello from module"; } })");
    std::string source = R"([Import] @Element MyComponent from "./module.chtl"; body { @Element MyComponent; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test_modules/main.chtl", true);
    assert(result.html_content.find("Hello from module") != std::string::npos);
}

void test_import_relative_path() {
    write_file("test_modules/nested/module.chtl", R"([Template] @Element NestedComponent { p { text: "I am nested"; } })");
    std::string source = R"([Import] @Element NestedComponent from "./nested/module.chtl"; body { @Element NestedComponent; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test_modules/main.chtl", true);
    assert(result.html_content.find("I am nested") != std::string::npos);
}

void test_import_nested() {
    write_file("test_modules/level2.chtl", R"([Template] @Element Level2 { span { text: "Deeply nested component"; } })");
    write_file("test_modules/level1.chtl", R"([Import] @Element Level2 from "./level2.chtl"; [Template] @Element Level1 { @Element Level2; })");
    std::string source = R"([Import] @Element Level1 from "./level1.chtl"; body { @Element Level1; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test_modules/main.chtl", true);
    assert(result.html_content.find("Deeply nested component") != std::string::npos);
}

void test_import_with_alias() {
    write_file("test_modules/aliased.chtl", R"([Template] @Element OriginalName { h1 { text: "Aliased"; } })");
    std::string source = R"([Import] @Element OriginalName from "./aliased.chtl" as MyAlias; body { @Element MyAlias; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test_modules/main.chtl", true);
    assert(result.html_content.find("<h1>Aliased</h1>") != std::string::npos);
}

void test_import_export_enforcement_pass() {
    write_file("test_modules/exported.chtl", R"([Info] {} [Export] { [Template] @Element ExportedComponent; } [Template] @Element ExportedComponent { div { text: "Successfully exported"; } })");
    std::string source = R"([Import] @Element ExportedComponent from "./exported.chtl"; body { @Element ExportedComponent; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test_modules/main.chtl", true);
    assert(result.html_content.find("Successfully exported") != std::string::npos);
}

void test_import_export_enforcement_fail() {
    write_file("test_modules/not_exported.chtl", R"([Info] {} [Export] { } [Template] @Element NotExported { p { text: "Should not be imported"; } })");
    std::string source = R"([Import] @Element NotExported from "./not_exported.chtl"; body { @Element NotExported; })";
    CompilerDispatcher dispatcher;
    bool thrown = false;
    try {
        dispatcher.compile(source, "test_modules/main.chtl", true);
    } catch (const std::runtime_error& e) {
        thrown = true;
        std::string error = e.what();
        assert(error.find("is not exported") != std::string::npos);
    }
    assert(thrown);
}

void test_import_custom_template() {
    write_file("test_modules/custom.chtl", R"([Info] {} [Export] { [Custom] @Element MyCustom; } [Custom] @Element MyCustom { span { text: "default"; } })");
    std::string source = R"([Import] [Custom] @Element MyCustom from "./custom.chtl"; body { @Element MyCustom { span { text: "overridden"; } } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "test_modules/main.chtl", true);
    assert(result.html_content.find("<span>overridden</span>") != std::string::npos);
    assert(result.html_content.find("default") == std::string::npos);
}

void test_custom_style_definition_delete() {
    std::string source = R"([Template] @Style Base { color: red; background-color: blue; } [Custom] @Style Child { inherit @Style Base; delete background-color; } div { style { @Style Child; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("color: red;") != std::string::npos);
    assert(result.html_content.find("background-color") == std::string::npos);
}

void test_style_template_with_calculation() {
    std::string source = R"([Template] @Style MyBox { width: 100px + 50px; } div { style { @Style MyBox; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("width: 150px;") != std::string::npos);
}

void test_style_template_with_var_reference() {
    std::string source = R"([Template] @Var MyTheme { primary_color: "blue"; } [Template] @Style MyBox { color: MyTheme(primary_color); } div { style { @Style MyBox; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("color: blue;") != std::string::npos);
}

void test_keyword_aliasing() {
    std::string source = R"([Configuration] { [Name] { KEYWORD_DELETE = remove; } } [Custom] @Element Box { span { text: "content"; } } body { @Element Box { remove span; } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("<span>content</span>") == std::string::npos);
}

void test_custom_origin_type() {
    std::string source = R"([Configuration] { [OriginType] { ORIGINTYPE_VUE = @Vue; } } body { [Origin] @Vue my_vue { <div id="app">{{ message }}</div> } })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    assert(result.html_content.find("<div id=\"app\">{{ message }}</div>") != std::string::npos);
}

void test_chtl_js_listen_block() {
    std::string source = R"(button { id: my-btn; } script { {{#my-btn}}->Listen { click: () => { console.log("Clicked!"); }, mouseover: myMouseOverHandler, }; })";
    CompilerDispatcher dispatcher;
    CompilationResult result = dispatcher.compile(source, "", true);
    std::string expected_js1 = R"(document.querySelector('#my-btn').addEventListener('click', () => { console.log("Clicked!"); });)";
    std::string expected_js2 = R"(document.querySelector('#my-btn').addEventListener('mouseover', myMouseOverHandler);)";
    assert(remove_whitespace(result.js_content).find(remove_whitespace(expected_js1)) != std::string::npos);
    assert(remove_whitespace(result.js_content).find(remove_whitespace(expected_js2)) != std::string::npos);
}

int main() {
    std::cout << "Running CHTL tests..." << std::endl;

    setup_test_environment();

    run_test(test_text_block_literals, "Text Block Literals");
    run_test(test_unquoted_literal_support, "Unquoted Literal Support");
    run_test(test_enhanced_selector, "Enhanced Selector");
    run_test(test_responsive_value, "Responsive Value");
    run_test(test_calc_generation, "Calc() Generation");
    run_test(test_except_constraints_parsing_and_enforcement, "Except Constraints Parsing and Enforcement");
    run_test(test_named_origin_and_import, "Named Origin and Import");
    run_test(test_delete_style_inheritance, "Delete Style Inheritance");
    run_test(test_referenced_property, "Referenced Property");
    run_test(test_conditional_expression_true, "Conditional Expression (True)");
    run_test(test_conditional_expression_false, "Conditional Expression (False)");
    run_test(test_custom_style_specialization, "Custom Style Specialization");
    run_test(test_custom_element_delete, "Custom Element Deletion");
    run_test(test_custom_element_insert, "Custom Element Insertion");
    run_test(test_custom_element_insert_at_top_bottom, "Custom Element Insertion At Top/Bottom");
    run_test(test_import, "Import Statement");

    std::cout << "\n--- Running Import/Export Tests ---\n";
    run_test(test_import_basic_element, "Import: Basic Element");
    run_test(test_import_relative_path, "Import: Relative Path");
    run_test(test_import_nested, "Import: Nested");
    run_test(test_import_with_alias, "Import: With Alias");
    run_test(test_import_export_enforcement_pass, "Import: Export Enforcement Pass");
    run_test(test_import_export_enforcement_fail, "Import: Export Enforcement Fail");
    run_test(test_import_custom_template, "Import: Custom Template");
    run_test(test_custom_style_definition_delete, "Custom Style Definition Delete");
    run_test(test_style_template_with_calculation, "Style Template with Calculation");
    run_test(test_style_template_with_var_reference, "Style Template with Var Reference");
    run_test(test_keyword_aliasing, "Keyword Aliasing");
    run_test(test_custom_origin_type, "Custom Origin Type");
    run_test(test_chtl_js_listen_block, "CHTL JS Listen Block");

    cleanup_test_environment();

    std::cout << "\nTests finished." << std::endl;
    return 0;
}

// Dummy definitions for tests that are not yet refactored to dispatcher
void test_lexer_configuration_keyword() {}
void test_namespace_template_access() {}
void test_hyphenated_property() {}
void test_attribute_expression() {}
void test_conditional_attribute() {}
void test_delete_style_property() {}
void test_var_template_usage() {}
void test_var_template_specialization() {}
void test_style_auto_add_class() {}
void test_except_clause_extended() {}
void test_ampersand_selector_order() {}
void test_delete_element_inheritance() {}
void test_calc_with_percentage() {}
void test_implicit_style_template_inheritance() {}
void test_use_html5_directive() {}
void test_named_configuration() {}
void test_precise_style_import() {}
void test_precise_var_import() {}
void test_precise_style_import_with_alias() {}
void test_precise_import_not_found() {}
void test_dispatcher_workflow() {}
void test_info_block_parsing() {}
void test_literal_types() {}
void test_ignored_comments() {}
void test_colon_equal_equivalence_in_info() {}
void test_colon_equal_equivalence_in_config() {}
void test_colon_equal_equivalence_in_var_template() {}
void test_style_property_power_operator() {}
void test_style_property_modulo_operator() {}
void test_conditional_rendering() {}
void test_cmod_import() {}
void test_compiler_dispatcher_full_workflow() {}
void test_chtl_js_lexer_and_parser() {}
void test_chtl_js_event_binding_operator() {}
void test_chtl_js_delegate_block() {}
void test_chtl_js_animate_block() {}
void test_chtl_js_script_loader() {}
void test_chtl_js_virtual_object_declaration() {}
void test_chtl_js_virtual_object_usage() {}
void test_chtl_js_router() {}
void test_dynamic_conditional_expression() {}
void test_dynamic_conditional_rendering() {}
void test_module_resolver_path_searching() {}
void test_cmod_export_enforcement() {}
void test_cli_inline_flag() {}
void test_comprehensive_file() {}
void test_precise_import_with_alias() {}