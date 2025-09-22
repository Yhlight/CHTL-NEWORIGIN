#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include "../CHTL/CHTLLoader/Loader.h"
#include <iostream>
#include <string>
#include <optional>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string input_file_path = argv[1];

    CHTL::Loader loader;
    auto file_content = loader.loadFile(input_file_path);

    if (!file_content) {
        std::cerr << "Error: Could not open file " << input_file_path << std::endl;
        return 1;
    }

    // 1. Lexing, 2. Parsing
    CHTL::Lexer lexer(*file_content);
    CHTL::Parser parser(lexer);
    auto program = parser.parseProgram();

    const auto& errors = parser.errors();
    if (!errors.empty()) {
        std::cerr << "Parser has " << errors.size() << " errors:" << std::endl;
        for (const auto& msg : errors) {
            std::cerr << "  - " << msg << std::endl;
        }
        return 1;
    }

    // 3. Generation
    CHTL::Generator generator(parser.styleTemplates, parser.elementTemplates);
    std::string html_output = generator.generate(program.get());
    std::string css_output = generator.getCss();

    // 4. Output
    // For now, just print the HTML. A real compiler would write to files.
    std::cout << html_output << std::endl;

    // If there's CSS, we could print it too, maybe in a style tag.
    if (!css_output.empty()) {
        // This is a simplistic way to show the CSS.
        // A full implementation would inject this into the <head>.
        std::cout << "<style>" << css_output << "</style>" << std::endl;
    }

    return 0;
}
