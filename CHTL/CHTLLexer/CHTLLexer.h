#pragma once

#include "Token.h"
#include <string>
#include <vector>

namespace CHTL {

    class CHTLLexer {
    public:
        CHTLLexer(const std::string& source);
        Token getNextToken();

    private:
        std::string source;
        size_t position;
        int line;
        int column;

        char peek();
        char advance();
        void skipWhitespace();
        Token makeToken(TokenType type, const std::string& lexeme);
        Token lexIdentifierOrLiteral();
        Token stringLiteral(char quoteType);
        Token singleLineComment();
        Token multiLineComment();
        Token generatorComment();
        Token errorToken(const std::string& message);
    };

}