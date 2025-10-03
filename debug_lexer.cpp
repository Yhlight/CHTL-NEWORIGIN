#include "CHTL/CHTLLexer/Lexer.h"
#include <iostream>

int main() {
    CHTL::Lexer lexer;
    
    // Test single character operators
    std::cout << "Testing single character operators:" << std::endl;
    auto tokens = lexer.tokenize(": = ; , . # & ? ! * + - / % ^ | ~");
    std::cout << "Token count: " << tokens.size() << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "[" << i << "] " << tokens[i].toString() << std::endl;
    }
    
    std::cout << "\nTesting multi-character operators:" << std::endl;
    tokens = lexer.tokenize("== != <= >= && || ++ -- += -= *= /= %= **=");
    std::cout << "Token count: " << tokens.size() << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "[" << i << "] " << tokens[i].toString() << std::endl;
    }
    
    return 0;
}