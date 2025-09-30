#include <iostream>
#include <vector>
#include "CHTL/CHTLLexer.h"

void testLexer() {
    std::cout << "=== CHTL Lexer Test ===" << std::endl;
    
    CHTL::CHTLLexer lexer;
    
    // Test 1: Simple element
    std::cout << "\nTest 1: Simple element" << std::endl;
    lexer.tokenize("div");
    const auto& tokens1 = lexer.getTokens();
    std::cout << "Tokens: ";
    for (const auto& token : tokens1) {
        std::cout << "[" << token.value << "] ";
    }
    std::cout << std::endl;
    
    // Test 2: Element with attributes
    std::cout << "\nTest 2: Element with attributes" << std::endl;
    lexer.tokenize("div class=\"container\" id=\"main\"");
    const auto& tokens2 = lexer.getTokens();
    std::cout << "Tokens: ";
    for (const auto& token : tokens2) {
        std::cout << "[" << token.value << "] ";
    }
    std::cout << std::endl;
    
    // Test 3: Text content
    std::cout << "\nTest 3: Text content" << std::endl;
    lexer.tokenize("text \"Hello World\"");
    const auto& tokens3 = lexer.getTokens();
    std::cout << "Tokens: ";
    for (const auto& token : tokens3) {
        std::cout << "[" << token.value << "] ";
    }
    std::cout << std::endl;
    
    // Test 4: Comments
    std::cout << "\nTest 4: Comments" << std::endl;
    lexer.tokenize("// This is a comment\ndiv");
    const auto& tokens4 = lexer.getTokens();
    std::cout << "Tokens: ";
    for (const auto& token : tokens4) {
        std::cout << "[" << token.value << "] ";
    }
    std::cout << std::endl;
}

int main() {
    testLexer();
    return 0;
}