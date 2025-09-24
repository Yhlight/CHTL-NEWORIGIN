#include "CompilerDispatcher.hpp"
#include "FileSystem.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: chtlc <input.chtl>\n";
        return 1;
    }
    std::string inputPath = argv[1];
    std::string source = chtl::util::readFileToString(inputPath);
    if (source.empty()) {
        std::cerr << "Failed to read: " << inputPath << "\n";
        return 1;
    }
    chtl::dispatcher::CompilerDispatcher dispatcher;
    std::string html = dispatcher.compileToHtml(source);
    std::cout << html;
    return 0;
}
