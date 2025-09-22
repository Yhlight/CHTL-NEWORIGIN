#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"

void run(const std::string& source) {
    try {
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        CHTL::Parser parser(tokens);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        CHTL::HtmlGenerator generator;
        std::string html = generator.generate(ast.get());

        std::cout << html << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file.chtl>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    run(source);

    return 0;
}
