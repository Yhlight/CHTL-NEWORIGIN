#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

// Helper function to read a file's content into a single string.
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function to write a string to a specified file.
void writeFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not write to file " << filepath << std::endl;
        return;
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "CHTL Compiler" << std::endl;
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl> [output_file.html]" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile;
    if (argc > 2) {
        outputFile = argv[2];
    }

    // 1. Read Source File
    std::string source = readFile(inputFile);
    if (source.empty()) {
        return 1;
    }

    try {
        // 2. Lexing: Convert source code to a stream of tokens.
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.getAllTokens();

        // 3. Parsing: Build an Abstract Syntax Tree from the tokens.
        CHTL::CHTLParser parser(tokens);
        std::unique_ptr<CHTL::ElementNode> ast = parser.parse();

        // 4. Generation: Traverse the AST to produce the final HTML string.
        CHTL::CHTLGenerator generator;
        std::string html_output = generator.generate(*ast);

        // 5. Output the result.
        if (!outputFile.empty()) {
            writeFile(outputFile, html_output);
            std::cout << "Successfully compiled " << inputFile << " to " << outputFile << std::endl;
        } else {
            // If no output file is specified, print to standard output.
            std::cout << html_output;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
