#include "Lexer.h"

Lexer::Lexer(std::string source) : source(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    // For now, just return an EOF token.
    tokens.emplace_back(TokenType::EndOfFile, "");
    return tokens;
}