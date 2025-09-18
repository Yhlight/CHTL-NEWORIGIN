#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>
#include <cassert>

void runTest(const std::string& name, const std::string& source, const std::string& expected) {
    std::cout << "--- Testing: " << name << " ---" << std::endl;
    std::cout << "Source: " << source << std::endl;

    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.tokenize();

    CHTL::Parser parser(tokens);
    CHTL::NodePtr ast = parser.parse();

    CHTL::CHTLGenerator generator;
    std::string result = generator.generate(ast);

    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Result:   " << result << std::endl;

    assert(result == expected);
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
    runTest("Element with Inline Styles", "div { style { font-size: 16px; color: red; } }", "<div style=\"color: red;font-size: 16px;\"></div>");

    std::cout << "\nAll generator tests passed!" << std::endl;

    return 0;
}
