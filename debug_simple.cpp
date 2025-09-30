#include <iostream>
#include <vector>
#include <string>
#include "CHTL/CHTLLexer.h"

int main() {
    // Test simple multiline comment
    CHTL::CHTLLexer lexer1;
    lexer1.tokenize("/* comment */");
    std::cout << "Simple comment tokens: " << lexer1.getTokens().size() << std::endl;
    
    // Test multiline comment with newline
    CHTL::CHTLLexer lexer2;
    lexer2.tokenize("/* comment\n */");
    std::cout << "Multiline comment tokens: " << lexer2.getTokens().size() << std::endl;
    
    // Test multiline comment followed by content
    CHTL::CHTLLexer lexer3;
    lexer3.tokenize("/* comment */div");
    std::cout << "Comment + div tokens: " << lexer3.getTokens().size() << std::endl;
    
    return 0;
}