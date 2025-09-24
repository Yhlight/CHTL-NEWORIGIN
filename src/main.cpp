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
    // A CHTL source string to test the [Custom] @Style feature,
    // including specialization with property completion and deletion.
    std::string chtlSource = R"(
[Template] @Style BaseStyle {
    font-family: "Arial";
    font-size: 16px;
    padding: 10px;
}

[Custom] @Style CustomBox {
    inherit @Style BaseStyle;
    border: 1px solid black;
    color; // Valueless property to be completed upon use
}

html {
    head { }
    body {
        div {
            text: "This box is specialized.";

            // Use and specialize the CustomBox template inside a style block, as per the spec
            style {
                @Style CustomBox {
                    delete font-family; // Delete an inherited property
                    font-size: 18px;   // Overwrite an inherited property
                    color: "red";      // Complete the valueless property
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
