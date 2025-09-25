#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "CHTL/CHTLScanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTLJS/CHTLJSLexer/Lexer.h"

// This main function serves as the entry point for the CHTL compiler.
// It demonstrates the workflow: Scanner -> Lexer -> Parser -> Generator.
int main() {
    // A more complex CHTL source string to test the Unified Scanner.
    std::string chtlSource = R"(
html {
    head {
        // This is a style block that should be extracted by the scanner.
        style {
            body {
                font-family: sans-serif;
                background-color: #f0f0f0;
            }
        }
    }
    body {
        div {
            class: "main-content";
            text: "Welcome to CHTL!";
        }
        // This is a script block that should be extracted by the scanner.
        script {
            const el = document.querySelector('.main-content');
            el -> Listen {
                click: () => {
                    alert('Clicked!');
                }
            }
        }
    }
}
)";

    try {
        // 1. Scanning Stage
        UnifiedScanner scanner(chtlSource);
        std::vector<std::unique_ptr<CodeFragment>> fragments = scanner.scan();

        std::string finalHtml;
        std::string finalCss;
        std::string finalJs;

        std::string chtlContent;
        for (const auto& fragment : fragments) {
            if (fragment->lang == CodeFragment::Language::CHTL) {
                chtlContent += fragment->content;
            }
        }

        // Process CHTL content
        if (!chtlContent.empty()) {
            Lexer lexer(chtlContent);
            Parser parser(lexer);
            auto ast = parser.parse();
            Generator generator;
            finalHtml = generator.generate(ast, parser.globalStyleContent, parser.sharedContext, parser.outputHtml5Doctype);
        }

        // Process other fragments
        for (const auto& fragment : fragments) {
            switch (fragment->lang) {
                case CodeFragment::Language::CSS:
                    finalCss += "/* --- CSS Fragment --- */\n" + fragment->content + "\n";
                    break;
                case CodeFragment::Language::JS: {
                    finalJs += "/* --- JS Fragment Tokens --- */\n";
                    CHTLJS::Lexer jsLexer(fragment->content);
                    CHTLJS::Token token;
                    do {
                        token = jsLexer.getNextToken();
                        finalJs += "Type: " + std::to_string(static_cast<int>(token.type)) + ", Value: '" + token.value + "'\n";
                    } while (token.type != CHTLJS::TokenType::EndOfFile);
                    break;
                }
                default:
                    // CHTL is already handled
                    break;
            }
        }

        // 4. Output Results
        std::cout << "--- CHTL Source ---" << std::endl;
        std::cout << chtlSource << std::endl;
        std::cout << "\n--- Generated HTML ---" << std::endl;
        std::cout << finalHtml << std::endl;
        if (!finalCss.empty()) {
            std::cout << "\n--- Generated CSS ---" << std::endl;
            std::cout << finalCss << std::endl;
        }
        if (!finalJs.empty()) {
            std::cout << "\n--- Generated JS ---" << std::endl;
            std::cout << finalJs << std::endl;
        }

    } catch (const std::runtime_error& e) {
        // Catch and report any errors that occurred during compilation.
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
