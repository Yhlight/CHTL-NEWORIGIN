#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>
#include <cassert>
#include <algorithm>
#include <cctype>

// Helper to normalize CSS for comparison (remove whitespace)
std::string normalize_css(std::string s) {
    s.erase(std::remove_if(s.begin(), s.end(), isspace), s.end());
    return s;
}

void runTest(const std::string& name, const std::string& source, const std::string& expected_html, const std::string& expected_css = "") {
    std::cout << "--- Testing: " << name << " ---" << std::endl;
    std::cout << "Source: " << source << std::endl;

    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.tokenize();

    CHTL::Parser parser(tokens);
    CHTL::NodePtr ast = parser.parse();

    CHTL::CHTLGenerator generator;
    CHTL::GeneratedResult result = generator.generate(ast);

    std::cout << "Expected HTML: " << expected_html << std::endl;
    std::cout << "Result HTML:   " << result.html << std::endl;
    assert(result.html == expected_html);

    std::cout << "Expected CSS: " << expected_css << std::endl;
    std::cout << "Result CSS:   " << result.css << std::endl;
    assert(normalize_css(result.css) == normalize_css(expected_css));

    std::cout << "Test PASSED." << std::endl;
    std::cout << "------------------------" << std::endl;
}

int main() {
    runTest("Simple Element", "div {}", "<div></div>");
    runTest("Nested Element", "html { body {} }", "<html><body></body></html>");
    runTest("Element with Attributes", "div { id: main; class: \"box styled\"; }", "<div id=\"main\" class=\"box styled\"></div>");
    runTest("Element with Text", "p { text { Hello CHTL } }", "<p>Hello CHTL</p>");
    runTest("Self-closing Tag", "img { src: \"image.png\"; }", "<img src=\"image.png\">");
    runTest("Generator Comment", "div { # This is a comment\n}", "<div><!-- This is a comment --></div>");
    runTest("Mixed Content", "body { h1 { text { Title } } p { text { Paragraph. } } img {} }", "<body><h1>Title</h1><p>Paragraph.</p><img></body>");
    runTest("Element with Inline Styles", "div { style { font-size: 16px; color: red; } }", "<div style=\"font-size: 16px;color: red;\"></div>");

    // New test for automatic class generation
    std::string auto_class_source = "div { style { .box { color: blue; } } }";
    std::string expected_html = "<div class=\"box\"></div>";
    std::string expected_css = ".box { color: blue; }";
    runTest("Automatic Class Generation", auto_class_source, expected_html, expected_css);

    // New test for automatic ID generation
    std::string auto_id_source = "div { style { #main { border: 1px solid black; } } }";
    std::string expected_html_id = "<div id=\"main\"></div>";
    std::string expected_css_id = "#main { border: 1px solid black; }";
    runTest("Automatic ID Generation", auto_id_source, expected_html_id, expected_css_id);

    // New test for context deduction
    std::string context_source = "a { class: link; style { &:hover { color: red; } } }";
    std::string expected_html_context = "<a class=\"link\"></a>";
    std::string expected_css_context = "a.link:hover { color: red; }";
    runTest("Context Deduction (& Selector)", context_source, expected_html_context, expected_css_context);


    std::cout << "\nAll generator tests passed!" << std::endl;

    return 0;
}
