#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "Loader.h"
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];

    // 0. Loading
    Loader loader;
    std::string chtl_source = loader.load(filepath);

    // 1. Lexing
    Lexer lexer(chtl_source);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parsing
    Parser parser(tokens);
    std::unique_ptr<ProgramNode> ast_root = parser.parse();

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
