#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

void debugComment(const std::string& input) {
    std::cout << "Input: \"" << input << "\"" << std::endl;
    std::cout << "Length: " << input.length() << std::endl;
    
    CHTL::CHTLLexer lexer;
    lexer.tokenize(input);
    const auto& tokens = lexer.getTokens();
    
    std::cout << "Tokens (" << tokens.size() << "): ";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "[" << token.value << "] ";
    }
    std::cout << std::endl;
    
    if (!lexer.getErrorMessage().empty()) {
        std::cout << "Error: " << lexer.getErrorMessage() << std::endl;
    }
}

int main() {
    debugComment("/* comment */");
    debugComment("/* comment */div");
    debugComment("/* This is a\nmultiline comment */\ndiv");
    return 0;
}