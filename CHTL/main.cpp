#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ElementNode.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    try {
        CHTLParser parser(content);
        std::unique_ptr<ElementNode> ast = parser.parse();
        if (ast) {
            std::cout << ast->toString();
        } else {
            std::cerr << "Failed to parse the document." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}