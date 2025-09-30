#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

int main() {
    // Test cases that should all result in only EOF token
    std::vector<std::string> testCases = {
        "/* comment */",
        "/* comment\n */",
        "/* comment\nmore\n */",
        "/* comment */div"
    };
    
    for (const auto& testCase : testCases) {
        std::cout << "\nTesting: \"" << testCase << "\"" << std::endl;
        CHTL::CHTLLexer lexer;
        lexer.tokenize(testCase);
        const auto& tokens = lexer.getTokens();
        
        std::cout << "Tokens (" << tokens.size() << "): ";
        for (const auto& token : tokens) {
            std::cout << "[" << token.value << "] ";
        }
        std::cout << std::endl;
        
        if (!lexer.getErrorMessage().empty()) {
            std::cout << "Error: " << lexer.getErrorMessage() << std::endl;
        }
    }
    
    return 0;
}