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

int main() {
    try {
        run_test("Attributes and Literals", test_attributes_and_literals);
        run_test("Inline Style Blocks", test_inline_style_block);
        run_test("Advanced Styles (Auto-Class)", test_advanced_style_autoclass);
        run_test("Style Templates", test_style_templates);
    } catch (const std::exception& e) {
        std::cerr << "A test failed with exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
