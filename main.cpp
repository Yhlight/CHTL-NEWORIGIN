#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTL/Lexer/Lexer.h"
#include "CHTL/Parser/Parser.h"
#include "CHTL/Generator/Generator.h"

int main() {
    std::ifstream file("test.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open test.chtl" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        CHTL::TemplateStore store;
        CHTL::Lexer lexer(source);
        CHTL::Parser parser(lexer, store);
        auto ast = parser.parse();

        std::cout << "--- AST ---" << std::endl;
        std::cout << ast->toString() << std::endl;

        CHTL::Generator generator(store);
        std::string html = generator.generate(*ast);

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
