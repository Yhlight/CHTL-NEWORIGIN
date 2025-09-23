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
    // A CHTL source string to test the template system:
    // - Defines Var, Style, and Element templates.
    // - Uses each type of template.
    std::string chtlSource = R"(
[Template] @Var Theme {
    primaryColor: "blue";
    basePadding: "10px";
}

[Template] @Style BoxStyle {
    border: 1px solid black;
    background-color: #eee;
}

[Template] @Element Card {
    h2 { text: "Card Title"; }
    p { text: "Some card content."; }
}

html {
    head { }
    body {
        div {
            id: main-card;
            style {
                padding: Theme(basePadding);
                color: Theme(primaryColor);
                @Style BoxStyle;
            }
            @Element Card;
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
        std::string htmlOutput = generator.generate(ast, parser.globalStyleContent);

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
