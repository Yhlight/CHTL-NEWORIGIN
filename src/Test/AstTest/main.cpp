#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "AstPrinter.h"
#include <iostream>

int main()
{
    std::string source = R"(
html {
    id = "main-page";
    class: "container";

    text {
        "Hello CHTL!"
    }

    div {
        style {
            color: red;
            width: (10px + 50px) * 2;
            height: box.height;
        }
    }
}
)";

    std::cout << "--- Tokenizing CHTL Source ---" << std::endl;
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.tokenize();

    std::cout << "\n--- Parsing Tokens ---" << std::endl;
    CHTL::Parser parser(tokens);
    try {
        std::vector<std::unique_ptr<CHTL::BaseNode>> ast = parser.parse();

        std::cout << "\n--- Printing AST ---" << std::endl;
        CHTL::AstPrinter printer;
        printer.print(ast);

    } catch (const std::exception& e) {
        std::cerr << "An error occurred during parsing: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
