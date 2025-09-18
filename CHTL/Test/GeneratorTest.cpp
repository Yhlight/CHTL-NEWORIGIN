#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <string>
#include <cassert>

// Forward declaration from ParserTest.cpp to reuse the error checking function.
void checkParserErrors(const Parser& p);

void testHtmlGeneration() {
    std::cout << "  Testing Basic HTML Generation..." << std::endl;

    std::string input = R"(
html {
    body {
        div {
            text { "Hello World" }
        }
    }
}
)";

    // The generator will produce a compact string with no extra whitespace.
    std::string expectedHtml = "<html><body><div>Hello World</div></body></html>";

    // 1. Lex the input
    Lexer l(input);

    // 2. Parse the tokens into an AST
    Parser p(l);
    NodePtr program = p.parseProgram();
    checkParserErrors(p); // Ensure the AST is valid before generating.

    // 3. Generate HTML from the AST
    Generator g;
    std::string actualHtml = g.generate(program);

    // 4. Assert that the generated HTML matches the expected output.
    assert(actualHtml == expectedHtml);

    std::cout << "  ...Passed" << std::endl;
}


void RunGeneratorTests() {
    std::cout << "--- Running Generator Tests ---" << std::endl;
    testHtmlGeneration();
    std::cout << "-----------------------------" << std::endl;
}
