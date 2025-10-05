#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

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

    // 3. Generating HTML
    CHTLGenerator generator(ast);
    std::string html = generator.generate();

    // 4. Print HTML
    std::cout << html << std::endl;

    return 0;
}