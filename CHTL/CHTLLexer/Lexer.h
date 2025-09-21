#pragma once

#include <string>
#include <vector>

namespace CHTL {

enum class TokenType {
    Identifier,
    StringLiteral,
    OpenBrace,
    CloseBrace,
    OpenBracket,
    CloseBracket,
    At,
    Colon,
    Semicolon,
    EndOfFile,
    Unknown
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string m_source;
    size_t m_current = 0;
    int m_line = 1;
    int m_column = 1;

    Token nextToken();
    char advance();
    char peek();
    void skipWhitespaceAndComments();
};

} // namespace CHTL
