#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/Generator.h"
#include "../../CHTL/CHTLContext/Context.h"
#include <iostream>
#include <string>
#include <cassert>

void run_test(const std::string& test_name, void (*test_func)()) {
    std::cout << "--- Running Test: " << test_name << " ---" << std::endl;
    test_func();
    std::cout << "--- Test Passed: " << test_name << " ---" << std::endl << std::endl;
}

void test_attributes_and_literals() {
    std::string source = R"(
        div {
            id: "main-div";
            class = container;
            data-value: 'some data';
        }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find("id=\"main-div\"") != std::string::npos);
    assert(final_output.find("class=\"container\"") != std::string::npos);
    assert(final_output.find("data-value=\"some data\"") != std::string::npos);
}

void test_inline_style_block() {
    std::string source = R"(
        div { style { width: 100px; height: 200px; } }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find("style=\"height:200px;width:100px;\"") != std::string::npos);
}

void test_advanced_style_autoclass() {
     std::string source = R"(
        html { head {} body { div { style { .box { color: red; } } } } }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find("class=\"box\"") != std::string::npos);
    assert(final_output.find(".box") != std::string::npos);
    assert(final_output.find("color: red") != std::string::npos);
}

void test_style_templates() {
    std::string source = R"(
        [Template] @Style DefaultText { color: black; }
        p { style { @Style DefaultText; } }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find("style=\"color:black;\"") != std::string::npos);
}

void test_style_context_deduction() {
    std::string source = R"(
        html { head {} body {
            div {
                class: "box";
                style {
                    &:hover {
                        color: red;
                    }
                }
            }
        } }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find(".box:hover") != std::string::npos);
    assert(final_output.find("color: red") != std::string::npos);
}

void test_element_templates() {
    std::string source = R"(
        [Template] @Element Box
        {
            span { text { "This is a box." } }
            div {}
        }

        body {
            @Element Box;
        }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find("<span>") != std::string::npos);
    assert(final_output.find("This is a box.") != std::string::npos);
    assert(final_output.find("<div>") != std::string::npos);
}

void test_variable_templates() {
    std::string source = R"(
        [Template] @Var ThemeColor
        {
            mainColor: "red";
        }

        div {
            style {
                background-color: ThemeColor(mainColor);
            }
        }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find("background-color:red") != std::string::npos);
}

void test_template_inheritance() {
    std::string source = R"(
        [Template] @Style BaseStyle {
            color: red;
            font-size: 12px;
        }
        [Template] @Style ChildStyle {
            @Style BaseStyle;
            font-size: 16px; // This should NOT be overridden now
        }
        p { style { @Style ChildStyle; } }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    // Last-defined property wins.
    assert(final_output.find("color:red") != std::string::npos);
    assert(final_output.find("font-size:16px") != std::string::npos);
    assert(final_output.find("font-size:12px") == std::string::npos);
}

void test_attribute_arithmetic() {
    std::string source = R"(
        div {
            style {
                width: 100px + 50px - 25px;
            }
        }
    )";
    CHTLLexer lexer(source);
    CHTLParser parser(lexer.scanTokens());
    CHTLGenerator generator;
    CHTLContext context;
    generator.generate(parser.parse(), context);
    std::string final_output = context.getFinalHtml();

    assert(final_output.find("width:125px") != std::string::npos);
}

int main() {
    try {
        run_test("Attributes and Literals", test_attributes_and_literals);
        run_test("Inline Style Blocks", test_inline_style_block);
        run_test("Advanced Styles (Auto-Class)", test_advanced_style_autoclass);
        run_test("Style Templates", test_style_templates);
        run_test("Style Context Deduction (&)", test_style_context_deduction);
        run_test("Element Templates", test_element_templates);
        run_test("Variable Templates", test_variable_templates);
        run_test("Template Inheritance", test_template_inheritance);
        run_test("Attribute Arithmetic", test_attribute_arithmetic);
    } catch (const std::exception& e) {
        std::cerr << "A test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
