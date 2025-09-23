#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/BaseNode.h"

// This main function serves as the entry point for the CHTL compiler.
// It demonstrates the workflow: Lexer -> Parser -> Generator.
int main() {
    // A simple CHTL source code string for testing the compiler pipeline.
    // This includes nested elements, a standard comment (to be ignored),
    // a text block, and a hash comment (to be rendered).
    std::string chtlSource = R"(
html {
    head {
        // A title would go here, but this comment will be ignored.
    }
    body {
        div {
            text { "Hello, CHTL!" }
        }
        # This is a CHTL comment that will be rendered to HTML.
    }
}
)";

    try {
        // 1. Lexing Stage
        // The Lexer takes the raw CHTL source and converts it into a stream of tokens.
        Lexer lexer(chtlSource);

        // 2. Parsing Stage
        // The Parser takes the token stream and builds an Abstract Syntax Tree (AST).
        Parser parser(lexer);
        std::vector<std::unique_ptr<BaseNode>> ast = parser.parse();

        // 3. Generation Stage
        // The Generator traverses the AST to produce the final HTML output.
        Generator generator;
        std::string htmlOutput = generator.generate(ast);

        // 4. Output Results
        // Print the original source and the generated HTML for verification.
        std::cout << "--- CHTL Source ---" << std::endl;
        std::cout << chtlSource << std::endl;
        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << htmlOutput << std::endl;

    } catch (const std::runtime_error& e) {
        // Catch and report any errors that occurred during compilation.
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
