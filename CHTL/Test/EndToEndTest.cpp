#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTL/CHTLContext/CHTLContext.h"
#include <iostream>
#include <string>
#include <algorithm>

// Helper function to remove whitespace for robust comparison
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

int main() {
    std::string input = R"(
        [Template] @Style DefaultText
        {
            color: black;
            line-height: 1.6;
        }

        div {
            id: "main";

            style {
                @Style DefaultText;
                font-size: 16px;
            }

            p {
                text { "Styled div with template" }
            }
        }
    )";

    CHTL::Lexer lexer(input);
    CHTL::CHTLContext context;
    CHTL::Parser parser(lexer, context);

    auto program = parser.parseProgram();

    if (!program) {
        std::cerr << "Parsing failed: parseProgram() returned null." << std::endl;
        return 1;
    }

    const auto& errors = parser.getErrors();
    if (!errors.empty()) {
        std::cerr << "Parser has " << errors.size() << " errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    // Generate HTML from the AST
    CHTL::Generator generator;
    std::string html_output = generator.generate(*program);

    std::string expected_html = "<div id=\"main\" style=\"color:black;line-height:1.6;font-size:16px;\"><p>Styled div with template</p></div>";

    std::cout << "--- End-to-End Test with Style Templates ---" << std::endl;
    std::cout << "Generated HTML: " << html_output << std::endl;
    std::cout << "Expected HTML:  " << expected_html << std::endl;

    // Use a more forgiving comparison that ignores whitespace differences
    if (remove_whitespace(html_output) != remove_whitespace(expected_html)) {
        std::cerr << "\n--- Test Failed! ---" << std::endl;
        std::cerr << "Mismatch after removing whitespace." << std::endl;
        return 1;
    }

    std::cout << "\n--- Test Passed! ---" << std::endl;

    return 0;
}
