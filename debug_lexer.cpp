#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

void debugTokenization(const std::string& input, const std::string& testName) {
    std::cout << "\n=== " << testName << " ===" << std::endl;
    std::cout << "Input: \"" << input << "\"" << std::endl;
    
    CHTL::CHTLLexer lexer;
    lexer.tokenize(input);
    const auto& tokens = lexer.getTokens();
    
    std::cout << "Tokens (" << tokens.size() << "): ";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "[" << token.value << "] ";
    }
    std::cout << std::endl;
    
    std::cout << "Detailed tokens:" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "  " << i << ": " << token.value << " (type=" << static_cast<int>(token.type) << ")" << std::endl;
    }
}

int main() {
    debugTokenization("div class=\"container\"", "Element with attributes");
    debugTokenization("/* This is a\nmultiline comment */\ndiv", "Multiline comment");
    debugTokenization("+ - * / = == != < >", "Operators");
    debugTokenization("123 45.67 0x1A", "Numbers");
    return 0;
}