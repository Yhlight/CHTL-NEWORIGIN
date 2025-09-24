#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/CHTLNode.h"
#include "../Util/AstPrinter.h"

int main() {
    std::cout << "--- Parser Test ---" << std::endl;
    std::string source = R"(
        html {
            body {
                text { "Welcome to CHTL"; }
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

        // 3. Print AST for verification
        if (ast) {
            AstPrinter printer;
            std::string ast_representation = printer.print(*ast);
            std::cout << "Generated AST:\n" << ast_representation << std::endl;
        } else {
            std::cout << "Parser did not produce an AST." << std::endl;
        }

    } catch (const CHTLParser::ParseError& e) {
        std::cerr << "Caught a parse error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Caught an unexpected error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
