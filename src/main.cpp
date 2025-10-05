#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"

int main() {
    std::ifstream file("sample.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open sample.chtl" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // 1. Lexing
    CHTLLexer lexer;
    std::vector<Token> tokens = lexer.tokenize(content);

    // 2. Parsing
    CHTLParser parser(tokens);
    std::vector<std::unique_ptr<Node>> ast = parser.parse();

    // 3. Print AST
    std::cout << "--- Abstract Syntax Tree ---" << std::endl;
    for (const auto& node : ast) {
        std::cout << node->toString() << std::endl;
    }

    return 0;
}