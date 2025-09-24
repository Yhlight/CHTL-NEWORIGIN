#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL/CHTLNode/CHTLNode.h"

int main() {
    std::cout << "--- Generator Test with Comments ---" << std::endl;
    std::string source = R"(
        // This is a single-line comment, it should be ignored.
        html {
            /* This is a multi-line comment.
               It should also be ignored. */
            body {
                # This is a generator comment, it should become an HTML comment.
                text { "Hello CHTL!" }
            }
        }
    )";

    std::cout << "Source:\n" << source << "\n" << std::endl;

    try {
        // 1. Lex
        CHTLLexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // 2. Parse
        CHTLParser parser(tokens);
        std::unique_ptr<CHTLNode> ast = parser.parse();

        // 3. Generate HTML
        if (ast) {
            CHTLGenerator generator;
            std::string html = generator.generate(*ast);
            std::cout << "Generated HTML:\n" << html << std::endl;
        } else {
            std::cout << "Parser did not produce an AST." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught an error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
