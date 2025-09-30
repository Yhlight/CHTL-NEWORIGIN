#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

int main() {
    // Test a simple case first
    std::string input = "/* comment */";
    std::cout << "Simple comment: \"" << input << "\"" << std::endl;
    
    CHTL::CHTLLexer lexer1;
    lexer1.tokenize(input);
    const auto& tokens1 = lexer1.getTokens();
    std::cout << "Tokens: " << tokens1.size() << std::endl;
    
    // Test multiline comment
    std::string input2 = "/* comment\n */";
    std::cout << "\nMultiline comment: \"" << input2 << "\"" << std::endl;
    
    CHTL::CHTLLexer lexer2;
    lexer2.tokenize(input2);
    const auto& tokens2 = lexer2.getTokens();
    std::cout << "Tokens: " << tokens2.size() << std::endl;
    
    for (size_t i = 0; i < tokens2.size(); ++i) {
        const auto& token = tokens2[i];
        std::cout << "  " << i << ": \"" << token.value << "\" (type=" << static_cast<int>(token.type) << ")" << std::endl;
    }
    
    return 0;
}