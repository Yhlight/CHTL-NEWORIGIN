#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include "Dispatcher/CompilerDispatcher.h"

// This main function serves as the entry point for the CHTL compiler.
// It demonstrates the new workflow: CompilerDispatcher -> Final Output.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::ifstream file_stream(input_file);
    if (!file_stream) {
        std::cerr << "Error: Could not open file " << input_file << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    std::string chtlSource = buffer.str();

    try {
        // 1. Dispatching Stage
        CompilerDispatcher dispatcher;
        std::string htmlOutput = dispatcher.compile(chtlSource);

        // 2. Output Results
        std::cout << "--- CHTL Source ---" << std::endl;
        std::cout << chtlSource << std::endl;
        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << htmlOutput << std::endl;

    } catch (const std::runtime_error& e) {
        // Catch and report any errors that occurred during compilation.
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}