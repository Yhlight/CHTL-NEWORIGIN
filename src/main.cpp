#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/Node.h"
#include "CHTL/CHTLContext.h"
#include "Util/FileLoader.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: chtl <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string source_code;
    try {
        source_code = CHTL::load_file(filename);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // 0. Setup Context
    ParsingContext context;

    // 1. Lexing
    Lexer lexer(source_code);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parsing
    Parser parser(tokens, context);
    std::vector<std::unique_ptr<BaseNode>> ast = parser.parse();

    // 3. Generation
    Generator generator;
    GeneratedOutput output = generator.generate(ast);

    // 4. Print Final Output
    // For a real application, you might put the CSS in a <style> tag in the <head>
    // or write it to a separate file. For this test, we'll print them separately.
    std::cout << "--- Generated CSS ---\n" << output.css << std::endl;
    std::cout << "--- Generated HTML ---\n" << output.html << std::endl;

    return 0;
}
