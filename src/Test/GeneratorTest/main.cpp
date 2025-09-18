#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/HtmlGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main()
{
    std::string source;
    try {
        source = readFile("test.chtl");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    std::cout << "--- CHTL to HTML Compilation ---" << std::endl;
    std::cout << "Input CHTL from test.chtl:\n" << source << std::endl;

    try {
        // 1. Lexing
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.tokenize();

        // 2. Parsing
        CHTL::Parser parser(tokens);
        std::vector<std::unique_ptr<CHTL::BaseNode>> ast = parser.parse();

        // 3. Generation
        CHTL::HtmlGenerator generator;
        std::string htmlOutput = generator.generate(ast);

        std::cout << "\n--- Generated HTML ---" << std::endl;
        std::cout << htmlOutput << std::endl;
        std::cout << "--- End of HTML ---" << std::endl;


    } catch (const std::exception& e) {
        std::cerr << "An error occurred during compilation: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
