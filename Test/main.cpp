#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"

void runTest(const std::string& testName, const std::string& chtlSource, const std::string& expectedHtml) {
    std::cout << "Running test: " << testName << "..." << std::endl;

    // 1. Lexer
    Lexer lexer(chtlSource);
    std::vector<Token> tokens = lexer.scanTokens();

    // 2. Parser
    Parser parser(tokens);
    std::unique_ptr<ElementNode> ast = parser.parse();

    // 3. Generator
    Generator generator;
    std::string actualHtml = generator.generate(*ast);

    // 4. Assert
    // Simple string comparison for now. A real test suite would do a more robust diff.
    if (actualHtml == expectedHtml) {
        std::cout << "PASSED" << std::endl;
    } else {
        std::cerr << "FAILED" << std::endl;
        std::cerr << "--- Expected ---\n" << expectedHtml << std::endl;
        std::cerr << "--- Actual ---\n" << actualHtml << std::endl;
        assert(false);
    }
}

int main() {
    std::string simpleChtl = R"(
// This is a comment.
html {
    body {
        div {
            text { "Hello CHTL!" }
        }
        br; // A void element
    }
}
)";

    std::string expectedSimpleHtml = R"(<!-- This is a comment. -->
<html>
  <body>
    <div>Hello CHTL!</div>
    <br>
    <!-- A void element -->
  </body>
</html>
)";

    runTest("Simple CHTL Document", simpleChtl, expectedSimpleHtml);

    std::cout << "\nAll tests passed!" << std::endl;

    return 0;
}
