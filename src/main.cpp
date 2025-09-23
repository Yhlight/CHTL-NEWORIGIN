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
    // A comprehensive CHTL source string to test multiple features together.
    std::string chtlSource = R"---(
[Import] @Chtl from "comprehensive_import.chtl";

body {
    @Element TestWidget from comprehensive_import {
        // Delete the footer
        delete div[2];

        // Add style to the header
        div[0] {
            style {
                padding: 10px;
                background-color: padding > 5px ? "lightgray" : "white";
            }
        }

        // Replace the entire widget-body div
        insert replace div[1] {
            div {
                class: "new-widget-body";
                p { text: "This is the new, specialized content!"; }
                p { text: "It has multiple lines."; }
            }
        }
    }
}
)---";

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
