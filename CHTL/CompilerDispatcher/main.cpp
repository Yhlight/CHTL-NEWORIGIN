#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTL/CHTLNode/ElementNode.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file.chtl>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // 1. Lexing
    Lexer lexer(source);

    // 2. Parsing
    Parser parser(lexer);
    std::unique_ptr<ElementNode> ast; // Changed from BaseNode to ElementNode
    try {
        ast = parser.parse();
        if (!ast) {
            std::cerr << "Parsing failed: No AST was generated." << std::endl;
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "A runtime error occurred during parsing: " << e.what() << std::endl;
        return 1;
    }

    // 3. Code Generation
    Generator generator;
    std::string html_output = generator.generate(*ast);

    // Print the final result
    std::cout << html_output;

    return 0;
}
