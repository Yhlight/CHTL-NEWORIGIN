#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <string>

// Helper function to remove whitespace for comparison
#include <algorithm>
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

int main() {
    std::string input = R"(
        html {
            head {
                // A title would go here
            }
            body {
                div {
                    text { "Hello CHTL" }
                }
                span {}
            }
        }
    )";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

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

    std::string expected_html = "<html><head></head><body><div>Hello CHTL</div><span></span></body></html>";

    std::cout << "--- Generator Test ---" << std::endl;
    std::cout << "Generated HTML: " << html_output << std::endl;
    std::cout << "Expected HTML:  " << expected_html << std::endl;

    if (remove_whitespace(html_output) != remove_whitespace(expected_html)) {
        std::cerr << "\n--- Generator Test Failed! ---" << std::endl;
        return 1;
    }

    std::cout << "\n--- Generator Test Passed! ---" << std::endl;

    return 0;
}
