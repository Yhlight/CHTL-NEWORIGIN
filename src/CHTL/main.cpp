#include <iostream>
#include <string>
#include <vector>

#include "../Util/FileSystem/FileSystem.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"

void printParseErrors(const std::vector<std::string>& errors) {
    std::cerr << "Whoops! We ran into some issues." << std::endl;
    std::cerr << "Parser has " << errors.size() << " error(s):" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "\t" << msg << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.chtl>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::string source;

    try {
        source = CHTL::FileSystem::ReadFile(filename);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        return 1;
    }

    CHTL::Lexer lexer(source);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    if (!parser.Errors().empty()) {
        printParseErrors(parser.Errors());
        return 1;
    }

    CHTL::Generator generator;
    std::string output = generator.Generate(program.get());

    std::cout << output << std::endl;

    return 0;
}
