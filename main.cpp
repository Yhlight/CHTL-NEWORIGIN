#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "CHTL/Lexer/Lexer.h"
#include "CHTL/Parser/Parser.h"
#include "CHTL/Generator/Generator.h"
#include "CHTL/Node/Node.h"

void print_usage(const char* prog_name) {
    std::cerr << "Usage: " << prog_name << " <input_file.chtl>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
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

    // 1. Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();

    // 2. Parser
    Parser parser(tokens);
    NodeList ast = parser.parse();

    // 3. Generator
    Generator generator;
    std::string html = generator.generate(ast);

    // 4. Print final HTML
    std::cout << "--- Generated HTML ---" << std::endl;
    std::cout << html;
    std::cout << "----------------------" << std::endl;

    return 0;
}
