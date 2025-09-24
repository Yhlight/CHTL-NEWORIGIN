#include <iostream>
#include <vector>
#include <string>
#include "CHTLLexer/Token/Token.h"

int main() {
    std::vector<chtl::lexer::Token> tokens;
    tokens.push_back({chtl::lexer::TokenType::Identifier, "div", 1, 1});
    tokens.push_back({chtl::lexer::TokenType::LBrace, "{", 1, 5});
    tokens.push_back({chtl::lexer::TokenType::KwStyle, "style", 2, 5});
    tokens.push_back({chtl::lexer::TokenType::RBrace, "}", 5, 1});
    for (const auto& t : tokens) {
        std::cout << static_cast<int>(t.type) << ":" << t.lexeme << "@" << t.line << "," << t.column << "\n";
    }
    return 0;
}

