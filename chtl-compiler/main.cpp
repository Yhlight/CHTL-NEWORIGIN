#include "chtl-compiler/CHTL/CHTLLexer/Lexer.h"
#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include "chtl-compiler/CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string chtl_source;
    try {
        chtl_source = readFile(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    try {
        Lexer lexer(chtl_source);
        Parser parser(lexer);
        NodeList ast = parser.parse();

        if (!ast.empty()) {
            Generator generator;
            std::string html_output = generator.generate(ast);
            std::cout << html_output << std::endl;
        } else {
            std::cerr << "Failed to parse the source." << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}