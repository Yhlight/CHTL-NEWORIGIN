#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "Util/FileSystem/FileSystemUtil.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string source;
    if (argc > 1) {
        try {
            source = CHTL::Util::FileSystemUtil::readFile(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Error reading file: " << e.what() << std::endl;
            return 1;
        }
    } else {
        std::cout << "No input file provided. Using default sample source." << std::endl;
        source = R"(
            html {
                id: root;
                head {}
                body {
                    class: "main-content";
                    p {
                        // text node parsing is not yet implemented
                    }
                }
            }
        )";
    }

    std::cout << "--- CHTL Compiler ---\n";
    std::cout << "Parsing source...\n";

    CHTL::CHTLParser parser;
    std::unique_ptr<CHTL::BaseNode> ast;

    try {
        ast = parser.parse(source);
        std::cout << "Parsing complete.\n";
    } catch (const std::exception& e) {
        std::cerr << "Parsing Error: " << e.what() << std::endl;
        return 1;
    }

    if (ast) {
        std::cout << "\n--- Generated HTML ---\n";
        CHTL::CHTLGenerator generator;
        std::string html = generator.generate(*ast);
        std::cout << html << std::endl;
    } else {
        std::cout << "Parsing resulted in an empty AST.\n";
    }

    return 0;
}
