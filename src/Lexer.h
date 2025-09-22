#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>

enum class TokenType {
    Identifier,
    StringLiteral,
    Semicolon,
    Colon,
    Equals,
    LeftBrace,
    RightBrace,
    Comment,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t current = 0;
    int line = 1;

    Token nextToken();
    void skipWhitespaceAndComments();
    Token handleIdentifier();
    Token handleStringLiteral();
    char peek();
    char advance();
    bool isAtEnd();
};

#endif //CHTL_LEXER_H
