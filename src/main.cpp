#include "Scanner/UnifiedScanner.h"
#include "CHTLJS/CHTLJSCompiler.h"
#include "Core/CodeMerger.h"
#include "Core/CHTLJSPreprocessor.h"
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>

// Function to extract content from between two tags
std::string extractScriptContent(const std::string& file_content) {
    size_t start = file_content.find("<script>");
    if (start == std::string::npos) {
        return "";
    }
    start += std::string("<script>").length();

    size_t end = file_content.find("</script>", start);
    if (end == std::string::npos) {
        return "";
    }

    return file_content.substr(start, end - start);
}

int main() {
    // Read the example.chtl file
    std::ifstream file("example.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open example.chtl" << '\n';
        return 1;
    }
    std::string file_content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    // Extract the script content from the file
    std::string source_code = extractScriptContent(file_content);
    if (source_code.empty()) {
        std::cerr << "Error: Could not find a <script> block in example.chtl" << '\n';
        return 1;
    }

    std::cout << "--- Original Script from file ---" << '\n';
    std::cout << source_code << '\n';
    std::cout << "---------------------------------" << '\n';

    // Initialize the components
    UnifiedScanner scanner;
    CHTLJSCompiler compiler;
    CodeMerger merger;
    CHTLJSPreprocessor preprocessor;

    // 1. Scan the code to separate CHTL JS from standard JS
    std::string scanned_output = scanner.scan(source_code);

    // 2. Compile the CHTL JS parts
    std::string compiled_output = compiler.compile(scanned_output);

    // 3. Merge the compiled parts with the original JS
    const auto& placeholders = scanner.getPlaceholders();
    std::string merged_code = merger.merge(compiled_output, placeholders);

    // 4. Preprocess the final merged code for Vir objects
    preprocessor.process(merged_code);

    std::cout << "\n--- Final Processed Code ---" << '\n';
    std::cout << merged_code << '\n';
    std::cout << "--------------------------" << '\n';

    return 0;
}