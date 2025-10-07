#include "chtl-compiler/CHTL/CHTLLexer/Lexer.h"
#include "chtl-compiler/CHTL/CHTLParser/Parser.h"
#include "chtl-compiler/CHTL/CHTLGenerator/Generator.h"
#include "chtl-compiler/CHTL/Loader/Loader.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    try {
        // Assume the compiler is run from the project root.
        // The Loader will prepend this path to relative imports.
        Loader loader(".");
        std::string initial_filepath = argv[1];

        std::string chtl_source = loader.loadFile(initial_filepath);

        Lexer lexer(chtl_source);
        Parser parser(lexer, loader, initial_filepath);
        NodeList ast = parser.parse();

        if (!ast.empty()) {
            Generator generator;
            std::string html_output = generator.generate(ast);
            std::cout << html_output << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}