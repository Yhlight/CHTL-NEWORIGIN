#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

int main() {
    std::string input = "/* comment\n */";
    std::cout << "Input: \"" << input << "\"" << std::endl;
    std::cout << "Input length: " << input.length() << std::endl;
    
    // Let's manually check the comment detection
    for (size_t i = 0; i < input.length(); ++i) {
        std::cout << "Position " << i << ": '" << input[i] << "' (ASCII: " << static_cast<int>(input[i]) << ")" << std::endl;
    }
    
    CHTL::CHTLLexer lexer;
    lexer.tokenize(input);
    const auto& tokens = lexer.getTokens();
    
    std::cout << "\nTokens (" << tokens.size() << "):" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "  " << i << ": \"" << token.value << "\" (type=" << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    if (!lexer.getErrorMessage().empty()) {
        std::cout << "Error: " << lexer.getErrorMessage() << std::endl;
    }
    
    return 0;
}