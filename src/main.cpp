#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "Scanner/UnifiedScanner.h"

// This main function serves as the entry point for the CHTL compiler.
// It demonstrates the new workflow: Scanner -> Lexer -> Parser -> Generator.
int main() {
    std::string chtlSource = R"(
[Template] @Style BaseStyle {
    font-family: "Arial";
    font-size: 16px;
}

div {
    text: "This is a test of the unified scanner.";
    style {
        @Style BaseStyle;
    }
    script {
        console.log("This script block should be separated.");
    }
}
)";

    try {
        // 1. Scanning Stage
        UnifiedScanner scanner;
        std::vector<CodeFragment> fragments = scanner.scan(chtlSource);

        std::cout << "--- CHTL Source ---" << std::endl;
        std::cout << chtlSource << std::endl;
        std::cout << "--- Scanned Fragments ---" << std::endl;
        for (const auto& fragment : fragments) {
            std::cout << "Fragment Type: " << static_cast<int>(fragment.type) << std::endl;
            std::cout << "Content:\n" << fragment.content << std::endl;
            std::cout << "---------------------" << std::endl;
        }

        // The rest of the compilation pipeline will be integrated here in the future.
        // For now, we just demonstrate the scanner.

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}