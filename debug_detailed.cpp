#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

void debugTokens(const std::string& input) {
    std::cout << "\nInput: \"" << input << "\"" << std::endl;
    CHTL::CHTLLexer lexer;
    lexer.tokenize(input);
    const auto& tokens = lexer.getTokens();
    
    std::cout << "Tokens (" << tokens.size() << "):" << std::endl;
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << "  " << i << ": \"" << token.value << "\" (type=" << static_cast<int>(token.type) << ")" << std::endl;
    }
}

int main() {
    debugTokens("/* comment */");
    debugTokens("/* comment\n */");
    debugTokens("/* comment */div");
    return 0;
}