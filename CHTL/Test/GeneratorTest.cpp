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

    std::string expectedHtml = "<html><body><div>Hello World</div></body></html>";

    Lexer l(input);
    Parser p(l);
    NodePtr program = p.parseProgram();
    checkParserErrors(p);

    Generator g;
    std::string actualHtml = g.generate(program);

    assert(actualHtml == expectedHtml);
    std::cout << "  ...Passed" << std::endl;
}

void testAttributeGeneration() {
    std::cout << "  Testing Attribute Generation..." << std::endl;

    std::string input = R"(
div {
    class: "container";
    id = "main";
}
)";

    std::string expectedHtml = R"(<div class="container" id="main"></div>)";

    Lexer l(input);
    Parser p(l);
    NodePtr program = p.parseProgram();
    checkParserErrors(p);

    Generator g;
    std::string actualHtml = g.generate(program);

    assert(actualHtml == expectedHtml);
    std::cout << "  ...Passed" << std::endl;
}

void testCommentGeneration() {
    std::cout << "  Testing Comment Generation..." << std::endl;

    std::string input = R"(
div {
    # This is a comment
}
)";

    std::string expectedHtml = R"(<div><!-- This is a comment --></div>)";

    Lexer l(input);
    Parser p(l);
    NodePtr program = p.parseProgram();
    checkParserErrors(p);

    Generator g;
    std::string actualHtml = g.generate(program);

    assert(actualHtml == expectedHtml);
    std::cout << "  ...Passed" << std::endl;
}


void RunGeneratorTests() {
    std::cout << "--- Running Generator Tests ---" << std::endl;
    testHtmlGeneration();
    testAttributeGeneration();
    testCommentGeneration();
    std::cout << "-----------------------------" << std::endl;
}
