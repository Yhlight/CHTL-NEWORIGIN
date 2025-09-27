#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <sstream>

#include "Dispatcher/CompilerDispatcher.h"

#include <algorithm>

// This main function serves as the entry point for the CHTL compiler.
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl> [--inline]" << std::endl;
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    bool inline_mode = false;
    std::string input_file;

    auto it = std::find(args.begin(), args.end(), "--inline");
    if (it != args.end()) {
        inline_mode = true;
        args.erase(it);
    }

    if (args.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        return 1;
    }
    input_file = args[0];

    std::ifstream file_stream(input_file);
    if (!file_stream) {
        std::cerr << "Error: Could not open file " << input_file << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    std::string chtlSource = buffer.str();

    try {
        CompilerDispatcher dispatcher;
        // The dispatcher will handle the full compilation and inlining.
        // For now, we will just pass a dummy flag.
        // In the next steps, we will update the dispatcher and generator.
        std::string htmlOutput = dispatcher.compile(chtlSource);

        // For now, we just print the output. In a real CLI, we'd write to files.
        std::cout << htmlOutput << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}