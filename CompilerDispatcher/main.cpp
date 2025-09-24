#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "CHTLLoader/Loader.h"
#include "CHTLNode/Node.h"
#include "CHTLGenerator/CodeGenerator.h"
#include "FileSystem/File.h"

void PrintUsage() {
    std::cout << "Usage: chtl_compiler <input.chtl> <output.html>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        PrintUsage();
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    // 1. Create the loader, which owns the context and orchestrates parsing
    CHTL::Loader loader;

    // 2. Load the main file, which will recursively load all imports
    std::unique_ptr<CHTL::Node> astRoot = loader.LoadFile(inputFile);
    if (!astRoot) {
        std::cerr << "Error: Failed to parse file." << std::endl;
        return 1;
    }

    // 3. Generate HTML from the final AST
    CHTL::CodeGenerator generator;
    std::string html = generator.Generate(astRoot.get(), loader.getContext());

    // 4. Write the generated HTML to the output file
    bool success = CHTL::Util::WriteFile(outputFile, html);
    if (!success) {
        std::cerr << "Error: Could not write to output file '" << outputFile << "'" << std::endl;
        return 1;
    }

    std::cout << "Successfully compiled '" << inputFile << "' to '" << outputFile << "'" << std::endl;

    return 0;
}
