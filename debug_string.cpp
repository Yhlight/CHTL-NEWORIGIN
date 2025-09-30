#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

int main() {
    std::string input = "/* comment\n */";
    std::cout << "Input length: " << input.length() << std::endl;
    std::cout << "Input bytes: ";
    for (size_t i = 0; i < input.length(); ++i) {
        std::cout << static_cast<int>(input[i]) << " ";
    }
    std::cout << std::endl;
    
    CHTL::CHTLLexer lexer;
    lexer.tokenize(input);
    const auto& tokens = lexer.getTokens();
    
    std::cout << "Tokens (" << tokens.size() << "):" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "  " << i << ": \"" << token.value << "\" (type=" << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    if (!lexer.getErrorMessage().empty()) {
        std::cout << "Error: " << lexer.getErrorMessage() << std::endl;
    }
    
    return 0;
}