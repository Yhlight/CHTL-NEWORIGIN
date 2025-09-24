#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "Util/FileSystem/FileSystemUtil.h"
#include <iostream>
#include <string>

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
                head {}
                body {
                    style {
                        color: blue;
                        .nested { font-weight: bold; }
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
        std::string html_body = generator.generate(*ast);
        std::string global_styles = generator.getGlobalStyles();

        // The main function is now responsible for composition
        if (!global_styles.empty()) {
            std::string style_tag = "  <style>\n" + global_styles + "  </style>\n";
            size_t head_pos = html_body.find("</head>");
            if (head_pos != std::string::npos) {
                html_body.insert(head_pos, style_tag);
            } else {
                // Prepend if no head tag
                html_body = style_tag + html_body;
            }
        }
        std::cout << html_body << std::endl;
    } else {
        std::cout << "Parsing resulted in an empty AST.\n";
    }

    return 0;
}
