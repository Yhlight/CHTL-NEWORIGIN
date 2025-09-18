#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

int main(int argc, char* argv[]) {
    std::string filename = "test.chtl";
    if (argc > 1) {
        filename = argv[1];
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Compiling " << filename << " ---" << std::endl << std::endl;

    try {
        // 1. Lexing
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.tokenize();

        // 2. Parsing
        CHTL::Parser parser(tokens);
        std::unique_ptr<CHTL::ElementNode> ast = parser.parse();

        // 3. Generating
        CHTL::Generator generator;
        std::string html = generator.generate(*ast);

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << html << std::endl;
        std::cout << "----------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
