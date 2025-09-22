#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/Nodes.h"

int main() {
    std::ifstream file("input.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input.chtl" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- CHTL Source ---" << std::endl;
    std::cout << source << std::endl;

    CHTL::Lexer lexer(source);
    CHTL::Parser parser(lexer);

    auto astRoot = parser.ParseProgram();

    const auto& errors = parser.Errors();
    if (!errors.empty()) {
        std::cerr << "--- Parser Errors ---" << std::endl;
        for (const auto& error : errors) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    if (!astRoot) {
        std::cerr << "Parsing failed and resulted in a null AST root." << std::endl;
        return 1;
    }

    CHTL::Generator generator;
    std::string html_output = generator.Generate(*astRoot);

    std::cout << "\n--- Generated HTML ---" << std::endl;
    std::cout << html_output << std::endl;

    return 0;
}
