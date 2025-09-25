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
    // A CHTL source string to test the new conditional styling feature.
    std::string chtlSource = R"(
html {
    head { }
    body {
        div {
            // Define some base attributes and styles to be used in the conditions.
            width: 100px;
            height: 200px;
            border: 1px solid black;

            // Conditional styling chain
            if {
                condition: width > 50px,
                background-color: "green"; // This should be applied
                border-color: "darkgreen";
            }
            else if {
                condition: height > 250px,
                background-color: "blue"; // This should NOT be applied
            }
            else {
                background-color: "red"; // This should NOT be applied
            }

            // Another element to test a different condition
            span {
                width: 40px;
                if {
                    condition: width > 50px,
                    display: "block"; // Should NOT be applied
                }
                else {
                    display: "inline-block"; // Should be applied
                }
            }
        }
    }
}
)";

    try {
        // 1. Lexing Stage
        Lexer lexer(chtlSource);

        // 2. Parsing Stage
        Parser parser(lexer);
        std::vector<std::unique_ptr<BaseNode>> ast = parser.parse();

        // 3. Generation Stage
        Generator generator;
        std::string htmlOutput = generator.generate(ast, parser.globalStyleContent, parser.outputHtml5Doctype);

        // 4. Output Results
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
