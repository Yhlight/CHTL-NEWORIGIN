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
    // A CHTL source string to test the complete namespace system,
    // including cross-namespace @Var usage.
    std::string chtlSource = R"(
[Namespace] UI {
    [Template] @Var Colors {
        primary: "blue";
    }

    [Template] @Element Card {
        p { text: "UI Card"; }
    }
}

[Namespace] Page {
    // This template uses a template from another namespace
    [Template] @Element MyPage {
        h1 { text: "My Page"; }
        @Element Card from UI;
    }
}

html {
    head { }
    body {
        div {
            style {
                // Use a variable from the UI namespace
                color: Colors(primary from UI);
            }

            // Use a template from the 'Page' namespace
            @Element MyPage from Page;
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
