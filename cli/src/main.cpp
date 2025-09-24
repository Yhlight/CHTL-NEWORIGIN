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
    bool flagInline = false, flagInlineCss = false, flagInlineJs = false, flagDefaultStruct = false;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--inline") flagInline = true;
        else if (a == "--inline-css") flagInlineCss = true;
        else if (a == "--inline-js") flagInlineJs = true;
        else if (a == "--default-struct") flagDefaultStruct = true;
    }
    chtl::dispatcher::CompilerDispatcher dispatcher;
    std::string html = dispatcher.compileToHtml(source);
    std::cout << html;
    return 0;
}
