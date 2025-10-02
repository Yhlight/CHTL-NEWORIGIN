#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "CHTLProcessor/CHTLProcessor.h"
#include "CHTLGenerator/HtmlGenerator.h"
#include "CHTLNode/DocumentNode.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <entry_filename>" << std::endl;
        return 1;
    }

    try {
        CHTLProcessor processor(argv[1]);
        std::unique_ptr<DocumentNode> ast = processor.process();
        if (ast) {
            HtmlGenerator generator;
            ast->accept(generator);
            std::cout << generator.getResult();
        } else {
            std::cerr << "Failed to process the document." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Processing error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}