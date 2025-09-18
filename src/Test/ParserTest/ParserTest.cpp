#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../AstTest/ASTPrinter.h"
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

    CHTL::ASTPrinter printer;
    std::string result = printer.print(ast);

    std::cout << "Expected: " << expected << std::endl;
    std::cout << "Result:   " << result << std::endl;

    assert(result == expected);
    std::cout << "Test PASSED." << std::endl;
    std::cout << "------------------------" << std::endl;
}

int main() {
    runTest("Simple Element", "div {}", "(root (div))");
    runTest("Nested Elements", "html { body { span {} } }", "(root (html (body (span))))");
    runTest("Peer Elements", "div { p {} img {} }", "(root (div (p) (img)))");
    runTest("Element with Attributes", "div { id: \"main\"; class: content; }", "(root (div (attr id=\"main\") (attr class=\"content\")))");
    runTest("Element with Text Node", "p { text { \"hello world\" } }", "(root (p (text \"hello world\")))");
    runTest("Element with Unquoted Text", "p { text { hello } }", "(root (p (text \"hello\")))");
    runTest("Mixed Content", "div { id: app; text { Welcome } p { text { More text } } }", "(root (div (attr id=\"app\") (text \"Welcome\") (p (text \"More text\"))))");


    std::cout << "\nAll parser tests passed!" << std::endl;

    return 0;
}
