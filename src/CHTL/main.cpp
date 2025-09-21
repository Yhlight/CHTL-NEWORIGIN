#include <iostream>
#include <string>
#include <vector>

#include <iostream>
#include <string>
#include <vector>

#include "Util/FileSystem/FileSystem.h"
#include "Scanner/UnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"

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

    CHTL::UnifiedScanner scanner;
    CHTL::CompilerDispatcher dispatcher;
    CHTL::CodeMerger merger;

    auto scanResult = scanner.Scan(source);
    auto compilationResult = dispatcher.Dispatch(scanResult);

    // The dispatcher will print errors. If the main CHTL compilation fails,
    // the content will be empty. We can check that here.
    if (compilationResult.chtlOutput.content.empty() && !scanResult.chtlSource.empty()) {
        std::cerr << "Compilation failed. Aborting." << std::endl;
        return 1;
    }

    std::string finalOutput = merger.Merge(compilationResult);

    std::cout << finalOutput << std::endl;

    return 0;
}
