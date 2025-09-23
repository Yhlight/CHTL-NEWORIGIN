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
    // A CHTL source string to test the template inheritance system.
    std::string chtlSource = R"(
// Base templates
[Template] @Var BaseTheme {
    textColor: "black";
    bgColor: "#fff";
}

[Template] @Style BaseStyle {
    font-family: "Arial, sans-serif";
    font-size: 16px;
}

[Template] @Element BaseCard {
    p { text: "This is a base card."; }
}

// Derived templates that inherit and override
[Template] @Var DarkTheme {
    inherit @Var BaseTheme;
    textColor: "white";
    bgColor: "#333";
}

[Template] @Style TitleStyle {
    @Style BaseStyle;
    font-size: 24px; // Override base
    font-weight: bold;
}

[Template] @Element FancyCard {
    inherit @Element BaseCard;
    h2 { text: "This is a fancy card!"; }
}

html {
    head { }
    body {
        // Use the derived templates
        div {
            style {
                color: DarkTheme(textColor);
                background-color: DarkTheme(bgColor);
                @Style TitleStyle;
            }

            @Element FancyCard;
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
