#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL
{
    class Lexer
    {
    public:
        Lexer(const std::string &source);
        std::vector<Token> tokenize();

    private:
        void scanToken();
        char advance();
        bool match(char expected);
        char peek() const;
        char peekNext() const;
        bool isAtEnd() const;
        bool isAlpha(char c) const;
        bool isDigit(char c) const;
        bool isAlphaNumeric(char c) const;

        void handleIdentifierOrLiteral();
        void handleQuotedString(char quote_type);
        void handleComment();
        void handleGeneratorComment();

        Token makeToken(TokenType type) const;
        Token makeToken(TokenType type, const std::string &lexeme) const;

        const std::string &source;
        std::vector<Token> tokens;
        size_t start = 0;
        size_t current = 0;
        int line = 1;
    };
}
