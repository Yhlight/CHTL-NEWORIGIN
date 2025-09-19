#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

// Helper function to get base name and replace extension
std::string get_base_path(const std::string& path, const std::string& new_ext) {
    size_t last_dot = path.find_last_of(".");
    if (last_dot == std::string::npos) {
        return path + new_ext;
    }
    return path.substr(0, last_dot) + new_ext;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: chtl-compiler <input_file> [output_html_file]" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];

    // 1. Scan
    CHTL::UnifiedScanner scanner;
    CHTL::CodeFragments fragments = scanner.scan(filepath);
    if (fragments.chtl_part.empty()) {
        std::cerr << "Error: Could not read or file is empty: " << filepath << std::endl;
        return 1;
    }

    // 2. Lex
    CHTL::CHTLLexer lexer(fragments.chtl_part);
    std::vector<CHTL::Token> tokens = lexer.tokenize();

    // 3. Parse
    CHTL::CHTLParser parser(tokens);
    std::unique_ptr<CHTL::ElementNode> ast_root = parser.parse();

    if (!ast_root) {
        std::cerr << "Error: Failed to parse the CHTL file." << std::endl;
        return 1;
    }

    // 4. Generate
    CHTL::CHTLGenerator generator;
    CHTL::CompilationOutput output = generator.generate(*ast_root);

    std::cout << "\n--- HTML Output ---" << std::endl;
    std::cout << output.html << std::endl;

    std::cout << "\n--- CSS Output ---" << std::endl;
    std::cout << output.css << std::endl;

    // 5. Write to file if output file is provided
    if (argc > 2) {
        std::string html_filepath = argv[2];
        std::ofstream html_file(html_filepath);
        if (!html_file.is_open()) {
            std::cerr << "Error: Could not open output file " << html_filepath << std::endl;
            return 1;
        }
        html_file << output.html;
        std::cout << "\nSuccessfully wrote HTML to " << html_filepath << std::endl;

        if (!output.css.empty()) {
            std::string css_filepath = get_base_path(html_filepath, ".css");
            std::ofstream css_file(css_filepath);
            if (!css_file.is_open()) {
                std::cerr << "Error: Could not open output file " << css_filepath << std::endl;
                return 1;
            }
            css_file << output.css;
            std::cout << "Successfully wrote CSS to " << css_filepath << std::endl;
        }
    }

    return 0;
}
