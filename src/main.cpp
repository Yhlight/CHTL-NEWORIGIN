#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

// Function to read file content into a string
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    std::string chtl_source = readFile(filepath);

    // 1. Lexing
    Lexer lexer(chtl_source);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parsing
    Parser parser(tokens);
    std::unique_ptr<ElementNode> ast_root = parser.parse();

    // 3. Generation
    if (ast_root) {
        Generator generator;
        generator.generate(*ast_root);

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << generator.get_html() << std::endl;

        std::cout << "\n--- Generated CSS ---" << std::endl;
        std::cout << generator.get_css() << std::endl;

    } else {
        std::cout << "Parsing failed." << std::endl;
    }

    return 0;
}
