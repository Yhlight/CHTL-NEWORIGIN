#include "Scanner/Tokenizer.h"
#include "CHTLJS/CHTLJSCompiler.h"
#include "Core/CHTLJSPreprocessor.h"
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>

// Function to extract content from between two tags
std::string extractScriptContent(const std::string& file_content) {
    size_t start = file_content.find("<script>");
    if (start == std::string::npos) return "";
    start += std::string("<script>").length();
    size_t end = file_content.find("</script>", start);
    if (end == std::string::npos) return "";
    return file_content.substr(start, end - start);
}

int main() {
    std::ifstream file("example.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open example.chtl" << '\n';
        return 1;
    }
    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::string source_code = extractScriptContent(file_content);
    if (source_code.empty()) {
        std::cerr << "Error: Could not find a <script> block in example.chtl" << '\n';
        return 1;
    }

    std::cout << "--- Original Script ---" << '\n';
    std::cout << source_code << '\n';
    std::cout << "-----------------------" << '\n';

    // --- FINAL, CORRECT, TOKEN-BASED PIPELINE ---

    // 1. Tokenize the source code
    Tokenizer tokenizer;
    std::vector<Token> tokens = tokenizer.tokenize(source_code);

    // 2. Preprocess the token stream for metaprogramming (Vir, iNeverAway)
    CHTLJSPreprocessor preprocessor;
    CHTLJSCompiler compiler; // Needs to be instantiated here to be passed
    preprocessor.process(tokens, compiler);

    // 3. Compile the preprocessed token stream
    std::string final_code = compiler.compile(tokens);

    // --- END NEW PIPELINE ---

    std::cout << "\n--- Final Compiled Code ---" << '\n';
    std::cout << final_code << '\n';
    std::cout << "---------------------------" << '\n';

    return 0;
}