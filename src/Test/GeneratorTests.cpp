#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "Assert.h"
#include <iostream>

void testSimpleElementGeneration() {
    std::cout << "Running testSimpleElementGeneration..." << std::endl;
    std::string source = "div {}";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Generator generator;
    std::string result = generator.generate(*ast);
    std::string expected = "<div />\n";
    CHTL::Test::assertEquals(expected, result, "testSimpleElementGeneration");
}

void testTextNodeGeneration() {
    std::cout << "Running testTextNodeGeneration..." << std::endl;
    std::string source = "text { \"hello\" }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Generator generator;
    std::string result = generator.generate(*ast);
    std::string expected = "hello\n";
    CHTL::Test::assertEquals(expected, result, "testTextNodeGeneration");
}

void testElementWithAttributesGeneration() {
    std::cout << "Running testElementWithAttributesGeneration..." << std::endl;
    std::string source = "div { id: \"main\"; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Generator generator;
    std::string result = generator.generate(*ast);
    std::string expected = "<div id=\"main\" />\n";
    CHTL::Test::assertEquals(expected, result, "testElementWithAttributesGeneration");
}

void testNestedElementsGeneration() {
    std::cout << "Running testNestedElementsGeneration..." << std::endl;
    std::string source = "body { h1 { text { \"Title\" } } }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto ast = parser.parse();
    Generator generator;
    std::string result = generator.generate(*ast);
    std::string expected = "<body>\n  <h1>\n    Title\n  </h1>\n</body>\n";
    CHTL::Test::assertEquals(expected, result, "testNestedElementsGeneration");
}

int main() {
    try {
        testSimpleElementGeneration();
        testTextNodeGeneration();
        testElementWithAttributesGeneration();
        testNestedElementsGeneration();
        std::cout << "All generator tests passed!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Generator test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}