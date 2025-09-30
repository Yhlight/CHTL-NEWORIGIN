#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

int main() {
    std::string input = "/* comment\n */";
    std::cout << "Input: \"" << input << "\"" << std::endl;
    
    // Let's manually trace through the tokenization
    CHTL::CHTLLexer lexer;
    
    // Check if the comment detection works
    std::cout << "Position 0: '" << input[0] << "'" << std::endl;
    std::cout << "Position 1: '" << input[1] << "'" << std::endl;
    
    if (input[0] == '/' && input[1] == '*') {
        std::cout << "Comment detected!" << std::endl;
    } else {
        std::cout << "No comment detected!" << std::endl;
    }
    
    lexer.tokenize(input);
    const auto& tokens = lexer.getTokens();
    
    std::cout << "Tokens (" << tokens.size() << "):" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "  " << i << ": \"" << token.value << "\" (type=" << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    return 0;
}