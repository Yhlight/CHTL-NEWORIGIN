#include <iostream>
#include <string>
#include <vector>
#include "Scanner/UnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"
#include "CodeMerger/CodeMerger.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: chtl_compiler <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];

    try {
        // 1. Scan
        CHTL::UnifiedScanner scanner;
        std::string source = scanner.scan(filePath);

        // 2. Dispatch (This now runs the CHTL compiler internally)
        CHTL::CompilerDispatcher dispatcher;
        CHTL::CompilationResult compile_result = dispatcher.dispatch(source);

        // 3. Merge
        CHTL::CodeMerger merger;
        std::string final_html = merger.merge(compile_result);

        // 4. Print output
        std::cout << final_html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
